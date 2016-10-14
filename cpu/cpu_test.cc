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

TEST (AddressingModeTest, ASL_ArithmeticShiftLeft) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x0A), ACCUMULATOR);
  EXPECT_EQ(cpu.get_addressing_mode(0x06), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0x16), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x0E), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0x1E), ABSOLUTEX);
}

TEST (AddressingModeTest, BCC_BranchIfCarryClear) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x90), RELATIVE);
}

TEST (AddressingModeTest, BCS_BranchIfCarrySet) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xB0), RELATIVE);
}

TEST (AddressingModeTest, BEQ_BranchIfEqual) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xF0), RELATIVE);
}

TEST (AddressingModeTest, BIT_BitTest) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x24), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0x2C), ABSOLUTE);
}

TEST (AddressingModeTest, BMI_BranchIfMinus) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x30), RELATIVE);
}

TEST (AddressingModeTest, BNE_BranchIfNotEqual) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xD0), RELATIVE);
}

TEST (AddressingModeTest, BRK_ForceInterrupt) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x00), IMPLIED);
}

TEST (AddressingModeTest, BVC_BranchIfOverflowClear) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x50), RELATIVE);
}

TEST (AddressingModeTest, BVS_BranchIfOverFlowSet) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x70), RELATIVE);
}

TEST (AddressingModeTest, CLC_ClearCarryFlag) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x18), IMPLIED);
}

TEST (AddressingModeTest, CLD_ClearDecimalMode) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xD8), IMPLIED);
}

TEST (AddressingModeTest, CLI_ClearInterruptDisable) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x58), IMPLIED);
}

TEST (AddressingModeTest, CLV_ClearOverflowFlag) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xB8), IMPLIED);
}

TEST (AddressingModeTest, CMP_Compare) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xC9), IMMEDIATE);
  EXPECT_EQ(cpu.get_addressing_mode(0xC5), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0xD5), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0xCD), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0xDD), ABSOLUTEX);
  EXPECT_EQ(cpu.get_addressing_mode(0xD9), ABSOLUTEY);
  EXPECT_EQ(cpu.get_addressing_mode(0xC1), INDIRECTX);
  EXPECT_EQ(cpu.get_addressing_mode(0xD1), INDIRECTY);
}

TEST (AddressingModeTest, CPX_CompareXRegister) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xE0), IMMEDIATE);
  EXPECT_EQ(cpu.get_addressing_mode(0xE4), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0xEC), ABSOLUTE);
}

TEST (AddressingModeTest, CPY_CompareYRegister) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xC0), IMMEDIATE);
  EXPECT_EQ(cpu.get_addressing_mode(0xC4), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0xCC), ABSOLUTE);

}
