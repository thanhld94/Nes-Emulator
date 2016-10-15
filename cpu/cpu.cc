#include "cpu.h"

#include <iostream>
#include <string>

namespace nesemu {

CPU::CPU() {
  pc = 0x34;
  sp = 0xFD;
  r_x = 0;
  r_y = 0;
  r_acc = 0;
  r_st = 0;
  memset(memory, 0, sizeof memory);
}

/* Addressing Mode
  0 - Immediate
  1 - ZeroPage
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
const int CPU::mode_table[256] = {
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
  0, 7, 0, 0, 1, 1, 1,-1,11, 0,11,-1, 4, 4, 4,-1, //A
 10, 8,-1,-1, 2, 2, 3,-1,11, 6,11,-1, 5, 5, 6,-1, //B
  0, 7,-1,-1, 1, 1, 1,-1,11, 0,11,-1, 4, 4, 4,-1, //C
 10, 8,-1,-1,-1, 2, 2,-1,11, 6,-1,-1,-1, 5, 5,-1, //D
  0, 7,-1,-1, 1, 1, 1,-1,11, 0,11,-1, 4, 4, 4,-1, //E
 10, 8,-1,-1,-1, 2, 2,-1,11, 6,-1,-1,-1, 5, 5,-1  //F
};

int CPU::get_addressing_mode(uint8_t opcode) const {
  return mode_table[opcode];
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

void CPU::Adc(uint8_t opcode) {
  int mode = mode_table[opcode];
  uint8_t operand1;

  //TODO get rid of magic numbers
  switch (mode) {
    case 0: // Immediate mode 
      operand1 = memory[pc + 1];
      r_acc += operand1;
      break;

    case 1: // Page Zero
      operand1 = memory[pc + 1];
      r_acc += memory[operand1];
      break;

    case 2: // Page Zero, X
      operand1 = r_x + memory[pc + 1];
      r_acc += memory[operand1];
      break;

    default:
      std::cerr << "Bad opcode" << std::endl;
  }
}

/* Getter & Setter*/

uint8_t CPU::get_memory(uint16_t address) const {
  return memory[address];
}

void CPU::set_memory(uint16_t address, uint8_t value) {
  memory[address] = value;
}

uint16_t CPU::get_pc() const {
  return pc;
}

void CPU::set_pc(uint16_t address) {
  pc = address;
}

uint8_t CPU::get_sp() const {
  return sp;
}

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

uint8_t CPU::get_acc() const {
  return r_acc;
}

uint8_t CPU::get_st() const {
  return r_st;
}

} // namespace nesemu
