#include "cpu.h"

#include <iostream>

namespace nesemu {

/* Addressing Mode
  0 - Immediate
  1 - ZeroPage
  2 - ZeroPageX
  3 - ZeroPageY
  4 - Abs
  5 - Abs X
  6 - Abs Y
  7 - IndirectX
  8 - IndirectY
  9 - Accumulator
  10 - Relative
  11 - Implied
  12 - Indirect
*/

// 0x00 - 0xFF
const int mode_table[256] = {
//0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F 
 11, 7,-1,-1,-1, 1, 1,-1,11, 0, 9,-1,-1, 4, 4,-1, //0
 10, 8,-1,-1,-1, 2, 2,-1,11, 6,-1,-1,-1, 5, 5,-1, //1
  4, 7,-1,-1, 1, 1, 1,-1,11, 0, 9,-1, 4, 4, 4,-1, //2
 10, 8,-1,-1,-1, 2, 2,-1,11, 6,-1,-1,-1, 5, 5,-1, //3
 11, 7,-1,-1,-1, 1, 1,-1,11, 0, 9,-1, 4, 4, 4,-1, //4
 10, 8,-1,-1,-1, 2, 2,-1,11, 6,-1,-1,-1, 5, 5,-1, //5
 11, 7,-1,-1,-1, 1, 1,-1,11, 0, 9,-1,12, 4, 4,-1, //6
 10, 8,-1,-1,-1, 2, 2,-1,11, 6,-1,-1,-1, 5, 5,-1, //7
 -1, 7,-1,-1, 1, 1, 1,-1,11,-1,11,-1, 4, 4, 4,-1, //8
 10, 8,-1,-1, 2, 2, 3,-1,11, 6,11,-1,-1, 5,-1,-1, //9
  0, 7, 0,-1, 1, 1, 1,-1,11, 0,11,-1, 4, 4, 4,-1, //A
 10, 8,-1,-1, 2, 2, 3,-1,11, 6,11,-1, 5, 5, 6,-1, //B
  0, 7,-1,-1, 1, 1, 1,-1,11, 0,11,-1, 4, 4, 4,-1, //C
 10, 8,-1,-1,-1, 2, 2,-1,11, 6,-1,-1,-1, 5, 5,-1, //D
  0, 7,-1,-1, 1, 1, 1,-1,11, 0,11,-1, 4, 4, 4,-1, //E
 10, 8,-1,-1,-1, 2, 2,-1,11, 6,-1,-1,-1, 5, 5,-1  //F
};

const int valid_inst_mode[56][13] = {
  {1,1,1,0,1,1,1,1,1,0,0,0,0}, // ADC
  {1,1,1,0,1,1,1,1,1,0,0,0,0}, // AND
  {0,1,1,0,1,1,0,0,0,1,0,0,0}, // ASL
  {0,0,0,0,0,0,0,0,0,0,1,0,0}, // BCC
  {0,0,0,0,0,0,0,0,0,0,1,0,0}, // BCS
  {0,0,0,0,0,0,0,0,0,0,1,0,0}, // BEQ
  {0,1,0,0,1,0,0,0,0,0,0,0,0}, // BIT
  {0,0,0,0,0,0,0,0,0,0,1,0,0}, // BMI
  {0,0,0,0,0,0,0,0,0,0,1,0,0}, // BNE
  {0,0,0,0,0,0,0,0,0,0,1,0,0}, // BPL
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // BRK
  {0,0,0,0,0,0,0,0,0,0,1,0,0}, // BVC
  {0,0,0,0,0,0,0,0,0,0,1,0,0}, // BVS
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // CLC
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // CLD
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // CLI
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // CLV
  {1,1,1,0,1,1,1,1,1,0,0,0,0}, // CMP
  {1,1,0,0,1,0,0,0,0,0,0,0,0}, // CPX
  {1,1,0,0,1,0,0,0,0,0,0,0,0}, // CPY
  {0,1,1,0,1,1,0,0,0,0,0,0,0}, // DEC
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // DEX
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // DEY
  {1,1,1,0,1,1,1,1,1,0,0,0,0}, // EOR
  {0,1,1,0,1,1,0,0,0,0,0,0,0}, // INC
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // INX
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // INY
  {0,0,0,0,1,0,0,0,0,0,0,0,1}, // JMP
  {0,0,0,0,1,0,0,0,0,0,0,0,0}, // JSR
  {1,1,1,0,1,1,1,1,1,0,0,0,0}, // LDA
  {1,1,0,1,1,0,1,0,0,0,0,0,0}, // LDX
  {1,1,1,0,1,1,0,0,0,0,0,0,0}, // LDY
  {0,1,1,0,1,1,0,0,0,1,0,0,0}, // LSR
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // NOP
  {1,1,1,0,1,1,1,1,1,0,0,0,0}, // ORA
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // PHA
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // PHP
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // PLA
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // PLP
  {0,1,1,0,1,1,0,0,0,1,0,0,0}, // ROL
  {0,1,1,0,1,1,0,0,0,1,0,0,0}, // ROR
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // RTI
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // RTS
  {1,1,1,0,1,1,1,1,1,0,0,0,0}, // SBC
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // SEC
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // SED
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // SEI
  {0,1,1,0,1,1,1,1,1,0,0,0,0}, // STA
  {0,1,0,1,1,0,0,0,0,0,0,0,0}, // STX
  {0,1,1,0,1,0,0,0,0,0,0,0,0}, // STY
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // TAX
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // TAY
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // TSX
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // TXA
  {0,0,0,0,0,0,0,0,0,0,0,1,0}, // TXS
  {0,0,0,0,0,0,0,0,0,0,0,1,0}  // TYA
};

const int instruction_type[256] = {
// 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  10,34,-1,-1,-1,34, 2,-1,36,34, 2,-1,-1,34, 2,-1,//0
   9,34,-1,-1,-1,34, 2,-1,13,34,-1,-1,-1,34, 2,-1,//1
  28, 1,-1,-1, 6, 1,39,-1,38, 1,39,-1, 6, 1,39,-1,//2
   7, 1,-1,-1,-1, 1,39,-1,44, 1,-1,-1,-1, 1,39,-1,//3
  41,23,-1,-1,-1,23,32,-1,35,23,32,-1,27,23,32,-1,//4
  11,23,-1,-1,-1,23,32,-1,15,23,-1,-1,-1,23,32,-1,//5
  42, 0,-1,-1,-1, 0,40,-1,37, 0,40,-1,27, 0,40,-1,//6
  12, 0,-1,-1,-1, 0,40,-1,46, 0,-1,-1,-1, 0,40,-1,//7
  -1,47,-1,-1,49,47,48,-1,22,-1,53,-1,49,47,48,-1,//8
   3,47,-1,-1,49,47,48,-1,55,47,54,-1,-1,47,-1,-1,//9
  31,29,30,-1,31,29,30,-1,51,29,50,-1,31,29,30,-1,//A
   4,29,-1,-1,31,29,30,-1,16,29,52,-1,31,29,30,-1,//B
  19,17,-1,-1,19,17,20,-1,26,17,21,-1,19,17,20,-1,//C
   8,17,-1,-1,-1,17,20,-1,14,17,-1,-1,-1,17,20,-1,//D
  18,43,-1,-1,18,43,24,-1,25,43,33,-1,18,43,24,-1,//E
   5,43,-1,-1,-1,43,24,-1,45,43,-1,-1,-1,43,24,-1 //F
};

const int mode_byte_size[13] = {
  2, // Immediate
  2, // Zero page
  2, // Zero page, X
  2, // Zero page, Y
  3, // Absolute
  3, // Absolute, X
  3, // Absolute, Y
  2, // IndirectX
  2, // IndirectY
  1, // Accumulator
  2, // Relative
  1, // Implied
  3  // Indirect
};

const int instruction_cycles[256] = {
//0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//0
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//1
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//2
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//3
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//4
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//5
  0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0,//6
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//7
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//8
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//9
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//A
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//B
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//C
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//D
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//E
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 //F
};

CPU::CPU() {
  pc = 0x34;
  sp = 0xFD;
  r_x = 0;
  r_y = 0;
  r_acc = 0;
  r_st = 0;
  cycles = 0;
  memset(memory, 0, sizeof memory);
}

int CPU::step() {
  uint8_t opcode = memory[pc];
  int instruction = instruction_type[opcode];
  // check valid opcode
  if (instruction == -1) {
    return 1;
  }

  uint16_t address = get_operand(opcode);
  int mode = mode_table[opcode];
  pc += mode_byte_size[mode];
  execute(instruction, address, mode);
  cycles += instruction_cycles[opcode];
  return 0;
}

uint16_t CPU::get_operand(uint8_t opcode) const {
  int mode = mode_table[opcode];
  uint16_t address = 0;

  //TODO get rid of magic numbers
  switch (mode) {
    case 0: // Immediate
      address = pc + 1;
      break;
    case 1: // Zero Page 
      address = memory[pc + 1];
      break;
    case 2: // Zero Page X
      address = (uint8_t)(memory[pc + 1] + r_x);
      break;
    case 3: // Zero Page Y
      address = (uint8_t)(memory[pc + 1] + r_y);
      break;
    case 4: // Absolute
      address = memory[pc + 2];
      address = (address << 8) | memory[pc + 1];
      break;
    case 5: // Absolute X
      address = (uint16_t(memory[pc + 2]) << 8) | memory[pc + 1];
      address += r_x;
      break;
    case 6: // Absolute Y
      address = (uint16_t(memory[pc + 2]) << 8) | memory[pc + 1];
      address += r_y;
      break;
    case 7: // Indirect X
      address = uint8_t(memory[memory[pc + 1]] + r_x);
      address = (memory[address + 1] << 8) | memory[address];
      break;
    case 8: // Indirect Y
      address = uint16_t(memory[memory[pc + 1] + 1] << 8) | memory[memory[pc + 1]];
      address += r_y;
      break;
    case 9: // Accumulator
      // ignore
      break;
    case 10: // Relative
      address = memory[pc + 1];
      if (address < 80) {
        address = pc + address + 2;
      } else {
        address = pc + address + 2 - 0x100;
      }
      break;
    case 11: // Implied
      // ignore
      break;
    case 12: // Indirect 
      address = (uint16_t(memory[pc + 2]) << 8) | memory[pc + 1];
      address = (uint16_t(memory[address + 1]) << 8) | memory[address];
      break;
    default:
      std::cerr << "Bad operand!" << std::endl;
  }
  return address;
}

// Instruction functions
int CPU::execute(int instruction, uint16_t address, int mode) {
  if (instruction > 55 || instruction < 0 || mode < 0 || mode > 12) {
    return 1;
  }

  if (!valid_inst_mode[instruction][mode]) {
    return 1;
  }

  uint8_t operand = 0;
  uint16_t val16 = 0;
  uint8_t val8 = 0;
  int err = 0;

  switch (instruction) {
    case 0: // ADC
      operand = memory[address];
      val16 = uint16_t(r_acc) + operand + get_carry();
      r_acc = uint8_t(val16);
      clear_carry();
      clear_zero();
      clear_negative();
      if (val16 > 0xFF) { // update carry flag
        set_carry();
      }
      if (val16 == 0) { // update zero flag
        set_zero();
      }
      if (val16 & 0x80) { // update negative flag
        set_negative();
      }
      break;
    case 1: // AND
      operand = memory[address];
      r_acc = r_acc & operand;
      clear_zero();
      clear_negative();
      if (r_acc & 0x80) { // update negative flag
        set_negative();
      }
      if (r_acc == 0) { // update zero flag
        set_zero();
      }
      break;
    case 2: // Asl
      val8 = (mode == 9) ? r_acc : memory[address]; // checking if mode is accumulator
      clear_carry();
      clear_negative();
      clear_zero();
      if (val8 & 0x80) { // carry flag
        set_carry();
      }
      if (val8 & 0x40) { // negative flag
        set_negative();
      }
      val8 = val8 << 1;
      if (val8 == 0) {
        set_zero();
      }
      if (mode == 9) { // Accumulator Mode
        r_acc = val8;
      } else {
        memory[address] = val8;
      }
      break;
    case 3: // BCC
      if (!get_carry()) {
        pc = address;
      }
      break;
    case 4: // BCS
      if (get_carry()) {
        pc = address;
      }
      break;
    case 5: // BEQ
      if (get_zero()) {
        pc = address;
      }
      break;
    case 6: // BIT
      val8 = memory[address];
      clear_zero();
      clear_overflow();
      clear_negative();
      if (val8 & 0x80) { // negative flag
        set_negative();
      }      
      if (val8 & 0x40) { // overflow flag
        set_overflow();
      }
      val8 = r_acc & val8;
      if (!val8) { // zero flag
        set_zero();
      }
      break;
    case 7: // BMI
      if (get_negative()) {
        pc = address;
      }
      break;
    case 8: // BNE
      if (!get_zero()) {
        pc = address;
      }
      break;
    case 9: // BPL
      if (!get_negative()) {
        pc = address;
      }
      break;
    case 10: // BRK
      pc++;
      memory[0x0100 + (sp++)] = uint8_t(pc); 
      memory[0x0100 + (sp++)] = uint8_t(pc >> 8);
      memory[0x0100 + (sp++)] = r_st;
      pc = (uint16_t(memory[0xFFFF]) << 8) | memory[0xFFFE];
      set_break();
      break;
    case 11: // BVC
      if (!get_overflow()) {
        pc = address;
      }
      break;
    case 12: // BVS
      if (get_overflow()) {
        pc = address;
      }
      break;
    case 13: // CLC
      clear_carry();
      break;
    case 14: // CLD
      clear_decimal();
      break;
    case 15: // CLI
      clear_interrupt_disable();
      break;
    case 16: // CLV
      clear_overflow();
      break;
    case 17: // CMP
      val8 = memory[address];
      clear_carry();
      clear_zero();
      clear_negative();
      if (r_acc >= val8) { // cary flag
        set_carry();
      }
      val8 = r_acc - val8;
      if (val8 == 0) { // zero flag
        set_zero();
      }
      if (val8 & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 18: // CPX
      val8 = memory[address];
      clear_carry();
      clear_zero();
      clear_negative();
      if (r_x >= val8) { // carry flag
        set_carry();
      }
      val8 = r_x - val8;
      if (val8 == 0) { // zero flag
        set_zero();
      }
      if (val8 & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 19: // CPY
      val8 = memory[address];
      clear_carry();
      clear_zero();
      clear_negative();
      if (r_y >= val8) { // cary flag
        set_carry();
      }
      val8 = r_y - val8;
      if (val8 == 0) { // zero flag
        set_zero();
      }
      if (val8 & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 20: // DEC
      memory[address]--;
      clear_zero();
      clear_negative();
      if (!memory[address]) { // zero flag
        set_zero();
      }
      if (memory[address] & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 21: // DEX
      r_x--;
      clear_zero();
      clear_negative();
      if (!r_x) { // zero flag
        set_zero();
      }
      if (r_x & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 22: // DEY
      r_y--;
      clear_zero();
      clear_negative();
      if (!r_y) { // zero flag
        set_zero();
      }
      if (r_y & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 23: // EOR
      r_acc ^= memory[address];
      clear_zero();
      clear_negative();
      if (!r_acc) { // zero flag
        set_zero();
      }
      if (r_acc & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 24: // INC
      memory[address]++;
      clear_zero();
      clear_negative();
      if (!memory[address]) { // zero flag
        set_zero();
      }
      if (memory[address] & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 25: // INX
      r_x++;
      clear_zero();
      clear_negative();
      if (!r_x) { // zero flag
        set_zero();
      }
      if (r_x & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 26: // INY
      r_y++;
      clear_zero();
      clear_negative();
      if (!r_y) { // zero flag
        set_zero();
      }
      if (r_y & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 27: // JMP
      pc = address;
      break;
    case 28: // JSR
      memory[0x0100 + (sp++)] = uint8_t(pc - 1);
      memory[0x0100 + (sp++)] = uint8_t((pc - 1) >> 8);
      pc = uint16_t(memory[address + 1] << 8) | memory[address];
      break;
    case 29: // LDA
      r_acc = memory[address];
      clear_zero();
      clear_negative();
      if (r_acc == 0) { // zero flag
        set_zero();
      }
      if (r_acc & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 30: // LDX
      r_x = memory[address];
      clear_zero();
      clear_negative();
      if (r_x == 0) { // zero flag
        set_zero();
      }
      if (r_x & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 31: // LDY
      r_y = memory[address];
      clear_zero();
      clear_negative();
      if (r_y == 0) { // zero flag
        set_zero();
      }
      if (r_y & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 32: // LSR
      val8 = (mode == 9) ? r_acc : memory[address];
      clear_carry();
      clear_zero();
      clear_negative(); // always 0
      if (val8 & 1) { //carry flag
        set_carry();
      }
      val8 >>= 1;
      if (val8 == 0) {
        set_zero();
      }
      if (mode == 9) { // accumulator mode
        r_acc = val8;
      } else {
        memory[address] = val8;
      }
      break;
    case 33: // NOP
      // do nothing
      break;
    case 34: // ORA
      val8 = memory[address];
      r_acc |= val8;
      clear_zero();
      clear_negative();
      if (!r_acc) { // zero flag
        set_zero();
      }
      if (r_acc & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 35: // PHA
      memory[0x0100 + sp] = r_acc;
      sp--;
      break;
    case 36: // PHP
      memory[0x0100 + sp] = r_st;
      sp--;
      break;
    case 37: // PLA
      sp++;
      r_acc = memory[0x0100 + sp];
      clear_zero();
      clear_negative();
      if (!r_acc) { // zero flag
        set_zero();
      }
      if (r_acc & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 38: // PLP
      sp++;
      r_st = memory[0x0100 + sp];
      break;
    case 39: // ROL
      val8 = (mode == 9) ? r_acc : memory[address];
      val16 = val8; // template holder
      val8 <<= 1;
      val8 |= get_carry();
      clear_carry();
      clear_zero();
      clear_negative();
      if (val16 & 0x80) { // carry flag
        set_carry();
      }
      if (val8 == 0) { // zero flag
        set_zero();
      }
      if (val8 & 0x80) {
        set_negative();
      }
      if (mode == 9) { // accumulator mode
        r_acc = val8;
      } else {
        memory[address] = val8;
      }
      break;
    case 40: // ROR
      val8 = (mode == 9) ? r_acc : memory[address];
      val16 = val8; // template holder
      val8 >>= 1;
      val8 |= (get_carry() << 7);
      clear_carry();
      clear_zero();
      clear_negative();
      if (val16 & 1) { // carry flag
        set_carry();
      }
      if (!val8) { // zero flag
        set_zero();
      }
      if (val8 & 0x80) { // negative flag
        set_negative();
      }
      if (mode == 9) { // accumulator mode
        r_acc = val8;
      } else {
        memory[address] = val8;
      }
      break;
    case 41: // RTI
      r_st = memory[0x0100 + (--sp)];
      pc = uint16_t(memory[0x0100 + (--sp)]) << 8; // high byte
      pc |= memory[0x0100 + (--sp)]; // low byte
      break;
    case 42: // RTS
      pc = uint16_t(memory[0x0100 + (--sp)]) << 8; // high byte
      pc |= memory[0x0100 + (--sp)]; // low byte
      pc++;
      break;
    case 43: // SBC
      val8 = memory[address];
      val16 = uint16_t(r_acc) - val8 - get_carry();
      clear_carry();
      clear_zero();
      clear_negative();
      r_acc = uint8_t(val16);
      if (val16 <= 0xFF) { // positive, set carry flag
        set_carry();
      }
      if (!r_acc) { // zero flag
        set_zero();
      }
      if (r_acc & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 44: // SEC
      set_carry();
      break;
    case 45: // SED
      set_decimal();
      break;
    case 46: // SEI
      set_interrupt_disable();
      break;
    case 47: // STA
      memory[address] = r_acc;
      break;
    case 48: // STX
      memory[address] = r_x;
      break;
    case 49: // STY
      memory[address] = r_y;
      break;
    case 50: // TAX
      r_x = r_acc;
      clear_zero();
      clear_negative();
      if (!r_x) { // zero flag
        set_zero();
      }
      if (r_x & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 51: // TAY
      r_y = r_acc;
      clear_zero();
      clear_negative();
      if (!r_y) { // zero flag
        set_zero();
      }
      if (r_y & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 52: // TSX
      r_x = sp;
      clear_zero();
      clear_negative();
      if (!r_x) { // zero flag
        set_zero();
      }
      if (r_x & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 53: // TXA
      r_acc = r_x;
      clear_zero();
      clear_negative();
      if (!r_acc) { // zero flag
        set_zero();
      }
      if (r_acc & 0x80) { // negative flag
        set_negative();
      }
      break;
    case 54: // TXS
      sp = r_x;
      break;
    case 55: // TYA
      r_acc = r_y;
      clear_zero();
      clear_negative();
      if (!r_acc) { // zero flag
        set_zero();
      }
      if (r_acc & 0x80) { // negative flag
        set_negative();
      }
      break;
    default:
      err = 1;
  }
  return err;
}

// get cpu cycle counter
uint64_t CPU::get_cycles() const {
  return cycles;
}

// set and get memory
uint8_t CPU::get_memory(uint16_t address) const {
  return memory[address];
}

void CPU::set_memory(uint16_t address, uint8_t value) {
  memory[address] = value;
}

// set and get program counter
uint16_t CPU::get_pc() const {
  return pc;
}

void CPU::set_pc(uint16_t address) {
  pc = address;
}

// get stack pointer
uint8_t CPU::get_sp() const {
  return sp;
}

void CPU::set_sp(uint8_t address) {
  sp = address;
}

// set and get Index Registers
uint8_t CPU::get_rx() const {
  return r_x;
}

void CPU::set_rx(uint8_t value) {
  r_x = value;
}

uint8_t CPU::get_ry() const {
  return r_y;
}

void CPU::set_ry(uint8_t value) {
  r_y = value;
}

// set and get accumulator
uint8_t CPU::get_acc() const {
  return r_acc;
}

void CPU::set_acc(uint8_t value) {
  r_acc = value;
}

// status register operations
// 7 6 5 4 3 2 1 0
// N V   B D I Z C
uint8_t CPU::get_st() const {
  return r_st;
}

// carry flag
int CPU::get_carry() const {
  return (r_st & 0x01) ? 1 : 0;
}

void CPU::set_carry() {
  r_st |= 0x01;
}

void CPU::clear_carry() {
  r_st &= 0xFE;
}

// zero flag
int CPU::get_zero() const {
  return (r_st & 0x02) ? 1 : 0;
}

void CPU::set_zero() {
  r_st |= 0x02;
}

void CPU::clear_zero() {
  r_st &= 0xFD;
}

// interrupt disable flag
int CPU::get_interrupt_disable() const {
  return (r_st & 0x04) ? 1 : 0;
}

void CPU::set_interrupt_disable() {
  r_st |= 0x04;
}

void CPU::clear_interrupt_disable() {
  r_st &= 0xFB;
}

// decimal flag
int CPU::get_decimal() const {
  return (r_st & 0x08) ? 1 : 0;
}

void CPU::set_decimal() {
  r_st |= 0x08;
}

void CPU::clear_decimal() {
  r_st &= 0xF7;
}

// break flag
void CPU::set_break() {
  r_st |= 0x10;
}

// overflow flag
int CPU::get_overflow() const {
  return (r_st & 0x40) ? 1 : 0;
}

void CPU::set_overflow() {
  r_st |= 0x40;
}

void CPU::clear_overflow() {
  r_st &= (0xBF);
}

// negative flag
int CPU::get_negative() const {
  return (r_st & 0x80) ? 1 : 0;
}

void CPU::set_negative() {
  r_st |= 0x80;
}

void CPU::clear_negative() {
  r_st &= 0x7F;
}

} // namespace nesemu
