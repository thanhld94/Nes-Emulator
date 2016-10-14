#include "cpu.h"

#include "gtest/gtest.h"

#define IMMEDIATE   0
#define ZEROPAGE    1
#define ZEROPAGEX   2
#define ZEROPAGEY   3
#define ABSOLUTE    4
#define ABSOLUTEX   5
#define ABSOLUTEY   6
#define INDIRECTX   7
#define INDIRECTY   8
#define ACCUMULATOR 9
#define RELATIVE    10
#define IMPLIED     11
#define INDIRECT    12

TEST (InitializeTest, FirstState) {
  CPU cpu;
  EXPECT_EQ(cpu.get_pc(), 0) << "initial program counter should be zero";
  EXPECT_EQ(cpu.get_sp(), 0) << "initial stack pointer should be zero";
  EXPECT_EQ(cpu.get_rx(), 0) << "initial value of Register X shoud be zero";
  EXPECT_EQ(cpu.get_ry(), 0) << "initial value of Register Y should be zero";
  EXPECT_EQ(cpu.get_acc(), 0) << "initial value of accumulator should be zero";
  EXPECT_EQ(cpu.get_st(), 0) << "inital state of status register should all be cleared";
}

TEST (AddressingModeTest, ADC_AddWithCarry) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x69), IMMEDIATE);
  EXPECT_EQ(cpu.get_addressing_mode(0x65), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0x75), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x6D), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0x7D), ABSOLUTEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x79), ABSOLUTEY);
  EXPECT_EQ(cpu.get_addressing_mode(0x61), INDIRECTX);
  EXPECT_EQ(cpu.get_addressing_mode(0x71), INDIRECTY);
}

TEST (AddressingModeTest, AND_LogicalAnd) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x29), IMMEDIATE);
  EXPECT_EQ(cpu.get_addressing_mode(0x25), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0x35), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x2D), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0x3D), ABSOLUTEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x39), ABSOLUTEY);
  EXPECT_EQ(cpu.get_addressing_mode(0x21), INDIRECTX);
  EXPECT_EQ(cpu.get_addressing_mode(0x31), INDIRECTY);
}

