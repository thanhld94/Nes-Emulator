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
