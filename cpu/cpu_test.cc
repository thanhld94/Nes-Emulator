#include "cpu.h"

#include "gtest/gtest.h"

TEST (InitializeTest, FirstState) {
  CPU cpu;
  EXPECT_EQ(cpu.get_pc(), 0) << "initial program counter should be zero";
  EXPECT_EQ(cpu.get_sp(), 0) << "initial stack pointer should be zero";
  EXPECT_EQ(cpu.get_rx(), 0) << "initial value of Register X shoud be zero";
  EXPECT_EQ(cpu.get_ry(), 0) << "initial value of Register Y should be zero";
  EXPECT_EQ(cpu.get_acc(), 0) << "initial value of accumulator should be zero";
  EXPECT_EQ(cpu.get_st(), 0) << "inital state of status register should all be cleared";
}
