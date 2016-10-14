#include "cpu.h"

#include <iostream>
#include <string>

CPU::CPU() {
  pc = 0;
  sp = 0;
  r_x = 0;
  r_y = 0;
  r_acc = 0;
  r_st = 0;
}

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
const int CPU::mode_table[256] = {
//0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F 
 11, 7, 0, 0, 0, 1, 1, 0,11, 0, 9, 0, 0, 4, 4, 0, //0
 10, 8, 0, 0, 0, 2, 2, 0,11, 6, 0, 0, 0, 5, 5, 0, //1
  4, 7, 0, 0, 1, 1, 1, 0,11, 0, 0, 0, 4, 4, 4, 0, //2
 10, 8, 0, 0, 0, 2, 2, 0,11, 6, 0, 0, 0, 5, 5, 0, //3
 11, 7, 0, 0, 0, 1, 1, 0,11, 0, 9, 0, 4, 4, 4, 0, //4
 10, 8, 0, 0, 0, 2, 2, 0,11, 6, 0, 0, 0, 5, 5, 0, //5
 11, 7, 0, 0, 0, 1, 1, 0,11, 0, 0, 0,12, 4, 4, 0, //6
 10, 8, 0, 0, 0, 2, 2, 0,11, 6, 0, 0, 0, 5, 5, 0, //7
  0, 7, 0, 0, 1, 1, 1, 0,11, 0,11, 0, 4, 4, 4, 0, //8
 10, 8, 0, 0, 2, 2, 3, 0,11, 6,11, 0, 0, 5, 0, 0, //9
  0, 7, 0, 0, 1, 1, 1, 0,11, 0,11, 0, 4, 4, 4, 0, //A
 10, 8, 0, 0, 2, 2, 3, 0,11, 6,11, 0, 5, 5, 6, 0, //B
  0, 7, 0, 0, 1, 1, 1, 0,11, 0,11, 0, 4, 4, 4, 0, //C
 10, 8, 0, 0, 0, 2, 2, 0,11, 6, 0, 0, 0, 5, 5, 0, //D
  0, 7, 0, 0, 1, 1, 1, 0,11, 0,11, 0, 4, 4, 4, 0, //E
 10, 8, 0, 0, 0, 2, 2, 0,11, 6, 0, 0, 0, 5, 5, 0  //F
};

int CPU::get_addressing_mode(uint8_t opcode) {
  return mode_table[opcode];
}

/* Getter & Setter*/

uint16_t CPU::get_pc() {
  return pc;
}

uint8_t CPU::get_sp() {
  return sp;
}

uint8_t CPU::get_rx() {
  return r_x;
}

uint8_t CPU::get_ry() {
  return r_y;
}

uint8_t CPU::get_acc() {
  return r_acc;
}

uint8_t CPU::get_st() {
  return r_st;
}
