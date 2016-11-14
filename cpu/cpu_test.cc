#include "cpu.h"

#include "gtest/gtest.h"
#include "test_utils.h"
#include <string>

namespace nesemu {

TEST (InitializeTest, FirstState) {
  CPU cpu;
  EXPECT_EQ(cpu.get_pc(), 0x34);
  EXPECT_EQ(cpu.get_sp(), 0xFD); // stack is top-down, 0x01FF - 0x0100
  EXPECT_EQ(cpu.get_rx(), 0);
  EXPECT_EQ(cpu.get_ry(), 0);
  EXPECT_EQ(cpu.get_acc(), 0);
  EXPECT_EQ(cpu.get_st(), 0);
  EXPECT_EQ(cpu.get_cycles(), 0);
  for (int i = 0; ; i++) {
    EXPECT_EQ(cpu.get_memory(i), 0);
    if (i == 0xFFFF) {
      break;
    }
  }
}

//TODO change tests to test step() instead of execute

// Test flag clear function
TEST (StatusRegisterMethod, FlagClearFunctions) {
  CPU cpu;
  EXPECT_EQ(cpu.get_st(), 0);
  cpu.set_carry();
  EXPECT_EQ(cpu.get_st(), 0x01);
  cpu.clear_carry();
  EXPECT_EQ(cpu.get_st(), 0);

  cpu.set_zero();
  EXPECT_EQ(cpu.get_st(), 0x02);
  cpu.clear_zero();
  EXPECT_EQ(cpu.get_st(), 0);

  cpu.set_interrupt_disable();
  EXPECT_EQ(cpu.get_st(), 0x04);
  cpu.clear_interrupt_disable();
  EXPECT_EQ(cpu.get_st(), 0);

  cpu.set_decimal();
  EXPECT_EQ(cpu.get_st(), 0x08);
  cpu.clear_decimal();
  EXPECT_EQ(cpu.get_st(), 0);

  cpu.set_overflow();
  EXPECT_EQ(cpu.get_st(), 0x40);
  cpu.clear_overflow();
  EXPECT_EQ(cpu.get_st(), 0);

  cpu.set_negative();
  EXPECT_EQ(cpu.get_st(), 0x80);
  cpu.clear_negative();
  EXPECT_EQ(cpu.get_st(), 0);
}

// Single Instruction Tests
TEST (SingleInstructionTest, AllAddressingModeTest) {
  CPU cpu;
  uint16_t address = 0xD412;
  for (int instruction = 0; instruction < NUM_INSTRUCTIONS; instruction++) {
    for (int mode = 0; mode < NUM_AD_MODES; mode++) {
      cpu = CPU();
      address = (mode == 9 || mode == 11) ? 0 : 0xD412;
      int expected = 1 - VALID_MODE[instruction][mode];
      std::string message = (expected) ? "Error expected" : "No error expected";
      EXPECT_EQ(cpu.execute(instruction, address, mode), expected) << message;
    }
  }
}

TEST (SingleInstructionTest, BadInstructionCode) {
  CPU cpu;
  uint16_t address = 0x2134;
  for (int mode = 0; mode < NUM_AD_MODES; mode++) {
    cpu = CPU();
    EXPECT_EQ(cpu.execute(-1, address, mode), 1);
    EXPECT_EQ(cpu.execute(56, address, mode), 1);
  }
}

TEST (SingleInstructionTest, ADC_AddWithCarry) {
  CPU cpu;
  uint16_t address = 0x3412;
  cpu.set_memory(address, 0x14);
  cpu.set_carry();
  cpu.set_acc(0xFC);
  cpu.set_negative();
  cpu.set_zero();

  cpu.execute(ADC, address, ABSOLUTE);

  uint8_t expected = uint8_t(0x14 + 0xFC + 1);

  EXPECT_EQ(cpu.get_acc(), expected);
  EXPECT_EQ(cpu.get_carry(), 1);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, ADC_StatusRegisterCheck) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.clear_carry();
  cpu.set_acc(0);
  cpu.set_memory(address, 0);
  
  cpu.execute(ADC, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_acc(), 0);
  EXPECT_EQ(cpu.get_carry(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);

  cpu.set_acc(0x8F);
  cpu.set_memory(address, 0x34);
  cpu.execute(ADC, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_acc(), 0xC3);
  EXPECT_EQ(cpu.get_negative(), 1);
  EXPECT_EQ(cpu.get_carry(), 0);
  EXPECT_EQ(cpu.get_zero(), 0);
}

TEST (SingleInstructionTest, AND_LogicalAnd) {
  CPU cpu;
  uint16_t address = 0xD412;
  cpu.set_zero();
  cpu.set_negative();
  cpu.set_memory(address, 0xDF); //1101 1111
  cpu.set_acc(0x4A);             //0100 1010

  uint8_t expected = 0xDF & 0x4A;
  cpu.execute(AND, address, ABSOLUTE);

  EXPECT_EQ(cpu.get_acc(), expected);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 0);

  cpu = CPU();
  cpu.clear_zero();
  cpu.clear_negative();
  cpu.set_memory(address, 0xDF); //1101 1111
  cpu.set_acc(0xCA);             //1100 1010

  expected = 0xDF & 0xCA;
  cpu.execute(AND, address, ABSOLUTE);

  EXPECT_EQ(cpu.get_acc(), expected);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);
}

TEST (SingleInstructionTest, ASL_ArithmeticShiftLeft) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.set_zero();
  cpu.set_negative();
  cpu.set_carry();

  // Non accumulator mode
  for (int i = 0; i < 4; i++) {
    cpu.set_memory(address, 0x3A); //0011 1010

    uint8_t expected = 0x74; //0111 0100
    int modes[4] = {ZEROPAGE, ZEROPAGEX, ABSOLUTE, ABSOLUTEX};
    int addressing_mode = modes[i];
    cpu.execute(ASL, address, addressing_mode);
    
    EXPECT_EQ(cpu.get_memory(address), expected);
    EXPECT_EQ(cpu.get_zero(), 0);
    EXPECT_EQ(cpu.get_negative(), 0);
    EXPECT_EQ(cpu.get_carry(), 0);
  }

  // Accumulator mode
  address = 0;
  cpu.set_zero();
  cpu.set_negative();
  cpu.set_carry();
  cpu.set_acc(0x3A);       //0011 1010

  uint8_t expected = 0x74; //0111 0100
  cpu.execute(ASL, address, ACCUMULATOR);

  EXPECT_EQ(cpu.get_acc(), expected);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 0);
  EXPECT_EQ(cpu.get_carry(), 0);
}

TEST (SingleInstructionTest, BCC_BranchIfCarryClear) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.clear_carry();
  cpu.execute(BCC, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), address);

  cpu.set_pc(0x2324);
  cpu.set_carry();
  cpu.execute(BCC, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), 0x2324);
}

TEST (SingleInstructionTest, BCS_BranchIfCarrySet) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.clear_carry();
  cpu.set_pc(0x2345);
  cpu.execute(BCS, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), 0x2345);

  cpu.set_carry();
  cpu.execute(BCS, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), 0x1412);
}

TEST (SingleInstructionTest, BEQ_BranchIfEqual) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.clear_zero();
  cpu.set_pc(0x5678);
  cpu.execute(BEQ, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), 0x5678);

  cpu.set_zero();
  cpu.execute(BEQ, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), 0x1412);
}

TEST (SingleInstructionTest, BIT_BitTest) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.clear_zero();
  cpu.clear_negative();
  cpu.clear_overflow();
  cpu.set_memory(address, 0x6A); //0110 1010
  cpu.set_acc(0x95);             //1001 0101
  cpu.execute(BIT, address, ABSOLUTE);

  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
  EXPECT_EQ(cpu.get_overflow(), 1);

  cpu.clear_zero();
  cpu.clear_overflow();
  cpu.clear_negative();
  cpu.set_memory(address, 0xBD); //1011 1101
  cpu.execute(BIT, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);
  EXPECT_EQ(cpu.get_overflow(), 0);
}

TEST (SingleInstructionTest, BMI_BranchIfMinus) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.clear_negative();
  cpu.set_pc(0x2428);
  cpu.execute(BMI, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), 0x2428);

  cpu.set_negative();
  cpu.execute(BMI, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), address);
}

TEST (SingleInstructionTest, BNE_BranchIfNotEqual) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.set_zero();
  cpu.set_pc(0x2428);
  cpu.execute(BNE, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), 0x2428);

  cpu.clear_zero();
  cpu.execute(BNE, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), address);
}

TEST (SingleInstructionTest, BPL_BranchIfPositive) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.set_negative();
  cpu.set_pc(0x2429);
  cpu.execute(BPL, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), 0x2429);

  cpu.clear_negative();
  cpu.execute(BPL, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), address);
}

TEST (SingleInstructionTest, BRK_BreakInterrupt) {
  CPU cpu;
  cpu.set_memory(0xFFFE, 0xA2);
  cpu.set_memory(0xFFFF, 0xFC);
  cpu.set_pc(0xD412);
  cpu.set_carry();
  cpu.set_zero();
  cpu.set_overflow();
  cpu.set_negative(); // status reg = 1100 0011 = 0xC3

  uint8_t expectedsp = cpu.get_sp() + 3;
  uint8_t expectedst = 0xD3; // 1101 0011
  cpu.execute(BRK, 0, IMPLIED);

  EXPECT_EQ(cpu.get_pc(), 0xFCA2);
  EXPECT_EQ(cpu.get_st(), expectedst);
  EXPECT_EQ(cpu.get_sp(), expectedsp);
  uint16_t pc_low_ad = 0x0100 + uint8_t(expectedsp - 3);
  uint16_t pc_high_ad = 0x0100 + uint8_t(expectedsp - 2);
  uint16_t old_st_ad = 0x0100 + uint8_t(expectedsp - 1);
  EXPECT_EQ(cpu.get_memory(pc_low_ad), 0x13); // old pc low
  EXPECT_EQ(cpu.get_memory(pc_high_ad), 0xD4); // old pc high
  EXPECT_EQ(cpu.get_memory(old_st_ad), 0xC3); // old sp
}

TEST (SingleInstructionTest, BVC_BranchIfOverflowClear) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.set_overflow();
  cpu.set_pc(0x2428);
  cpu.execute(BVC, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), 0x2428);

  cpu.clear_overflow();
  cpu.execute(BVC, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), address);
}

TEST (SingleInstructionTest, BVS_BranchIfOverFlowSet) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.clear_overflow();
  cpu.set_pc(0x2428);
  cpu.execute(BVS, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), 0x2428);

  cpu.set_overflow();
  cpu.execute(BVS, address, RELATIVE);
  EXPECT_EQ(cpu.get_pc(), address);
}

TEST (SingleInstructionTest, CLC_ClearCarryFlag) {
  CPU cpu;
  cpu.set_carry();
  cpu.execute(CLC, 0, IMPLIED);
  EXPECT_EQ(cpu.get_carry(), 0);

  cpu.clear_carry();
  cpu.execute(CLC, 0, IMPLIED);
  EXPECT_EQ(cpu.get_carry(), 0);
}

TEST (SingleInstructionTest, CLD_ClearDecimalMode) {
  CPU cpu;
  cpu.set_decimal();
  cpu.execute(CLD, 0, IMPLIED);
  EXPECT_EQ(cpu.get_decimal(), 0);

  cpu.clear_decimal();
  cpu.execute(CLD, 0, IMPLIED);
  EXPECT_EQ(cpu.get_decimal(), 0);
}

TEST (SingleInstructionTest, CLI_ClearInterruptDisable) {
  CPU cpu;
  cpu.set_interrupt_disable();
  cpu.execute(CLI, 0, IMPLIED);
  EXPECT_EQ(cpu.get_interrupt_disable(), 0);

  cpu.clear_interrupt_disable();
  cpu.execute(CLI, 0, IMPLIED);
  EXPECT_EQ(cpu.get_interrupt_disable(), 0);
}

TEST (SingleInstructionTest, CLV_ClearOverflowFlag) {
  CPU cpu;
  cpu.set_overflow();
  cpu.execute(CLV, 0, IMPLIED);
  EXPECT_EQ(cpu.get_overflow(), 0);

  cpu.clear_overflow();
  cpu.execute(CLV, 0, IMPLIED);
  EXPECT_EQ(cpu.get_overflow(), 0);
}

TEST (SingleInstructionTest, CMP_Compare) {
  //Test equal
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.set_memory(address, 0x6E); // 0110 1110
  cpu.set_acc(0x6E);    // 0110 1110
  cpu.execute(CMP, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_carry(), 1); 
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);

  //Test greater than
  cpu = CPU();
  cpu.set_memory(address, 0x6E); // 0110 1110
  cpu.set_acc(0x8D);    // 1000 1101
  cpu.execute(CMP, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_carry(), 1);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 0);

  //Test negative flag
  cpu = CPU();
  cpu.set_memory(address, 0x21); // 0010 0001
  cpu.set_acc(0xF7);    // 1111 0111
  cpu.execute(CMP, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_carry(), 1);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  // Test less than
  cpu = CPU();
  cpu.set_memory(address, 0x77); // 0111 0111
  cpu.set_acc(0x21);             // 0010 0001
  cpu.execute(CMP, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_carry(), 0);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);
}

TEST (SingleInstructionTest, CPX_CompareXRegister) {
  // test equals
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.set_memory(address, 0x48); //0100 1000
  cpu.set_rx(0x48);
  cpu.execute(CPX, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_carry(), 1);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);

  // test strictly greater than
  cpu = CPU();
  cpu.set_memory(address, 0x72); // 0111 0010
  cpu.set_rx(0x7D);              // 0111 1101
  cpu.execute(CPX, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_carry(), 1);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 0);

  // tst less than
  cpu = CPU();
  cpu.set_memory(address, 0x7F); // 0111 1111
  cpu.set_rx(0x4D);              // 0100 1101
  cpu.execute(CPX, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_carry(), 0);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  // test greater neg flag
  cpu = CPU();
  cpu.set_memory(address, 0x4D); // 0100 1101
  cpu.set_rx(0xFD);              // 1111 1101
  cpu.execute(CPX, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_carry(), 1);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);
}

TEST (SingleInstructionTest, CPY_CompareYRegister) {
  // test equals
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.set_memory(address, 0x48); //0100 1000
  cpu.set_ry(0x48);
  cpu.execute(CPY, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_carry(), 1);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);

  // test strictly greater than
  cpu = CPU();
  cpu.set_memory(address, 0x72); // 0111 0010
  cpu.set_ry(0x7D);              // 0111 1101
  cpu.execute(CPY, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_carry(), 1);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 0);

  // tst less than
  cpu = CPU();
  cpu.set_memory(address, 0x7F); // 0111 1111
  cpu.set_ry(0x4D);              // 0100 1101
  cpu.execute(CPY, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_carry(), 0);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  // test greater neg flag
  cpu = CPU();
  cpu.set_memory(address, 0x4D); // 0100 1101
  cpu.set_ry(0xFD);              // 1111 1101
  cpu.execute(CPY, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_carry(), 1);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);
}

TEST (SingleInstructionTest, DEC_DecrementMemory) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.set_memory(address, 0xF8); // 1111 1000
  cpu.execute(DEC, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_memory(address), 0xF7); // 1111 0111
  EXPECT_EQ(cpu.get_negative(), 1); 
  EXPECT_EQ(cpu.get_zero(), 0);

  // result = 0 case
  cpu.set_memory(address, 0x01); // 0000 0001
  cpu.execute(DEC, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_memory(address), 0);
  EXPECT_EQ(cpu.get_negative(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
}

TEST (SingleInstructionTest, DEX_DecrementXRegister) {
  CPU cpu;
  cpu.set_rx(0xF4); // 1111 0100
  cpu.execute(DEX, 0, IMPLIED);
  EXPECT_EQ(cpu.get_rx(), 0xF3); // 1111 0011
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  cpu.set_rx(0x01);
  cpu.execute(DEX, 0, IMPLIED);
  EXPECT_EQ(cpu.get_rx(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, DEY_DecrementYRegister) {
  CPU cpu;
  cpu.set_ry(0xF4); // 1111 0100
  cpu.execute(DEY, 0, IMPLIED);
  EXPECT_EQ(cpu.get_ry(), 0xF3); // 1111 0011
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  cpu.set_ry(0x01);
  cpu.execute(DEY, 0, IMPLIED);
  EXPECT_EQ(cpu.get_ry(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, EOR_ExclusiveOR) {
  CPU cpu;
  uint16_t address = 0xD412;
  cpu.set_memory(address, 0xA4); // 1010 0100
  cpu.set_acc(0x4C);             // 0100 1100
  cpu.execute(EOR, address, ABSOLUTE);
  uint8_t expected = 0xE8;       // 1110 1000
  EXPECT_EQ(cpu.get_acc(), expected);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  //zero flag test
  cpu.set_memory(address, 0xA4);
  cpu.set_acc(0xA4);
  cpu.execute(EOR, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_acc(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, INC_IncrementMemory) {
  CPU cpu;
  uint16_t address = 0xD412;
  cpu.set_memory(address, 0xF8); // 1111 1000
  cpu.execute(INC, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_memory(address), 0xF9); // 1111 1001
  EXPECT_EQ(cpu.get_negative(), 1); 
  EXPECT_EQ(cpu.get_zero(), 0);

  // result = 0 case
  cpu.set_memory(address, 0xFF); // 1111 1111
  cpu.execute(INC, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_memory(address), 0);
  EXPECT_EQ(cpu.get_negative(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
}

TEST (SingleInstructionTest, INX_IncrementXRegister) {
  CPU cpu;
  cpu.set_rx(0xF4); // 1111 0100
  cpu.execute(INX, 0, IMPLIED);
  EXPECT_EQ(cpu.get_rx(), 0xF5); // 1111 0101
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  // zero flag test
  cpu.set_rx(0xFF);
  cpu.execute(INX, 0, IMPLIED);
  EXPECT_EQ(cpu.get_rx(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, INY_IncrementYRegister) {
  CPU cpu;
  cpu.set_ry(0xF4); // 1111 0100
  cpu.execute(INY, 0, IMPLIED);
  EXPECT_EQ(cpu.get_ry(), 0xF5); // 1111 0101
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  // zero flag test
  cpu.set_ry(0xFF);
  cpu.execute(INY, 0, IMPLIED);
  EXPECT_EQ(cpu.get_ry(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, JMP_Jump) {
  CPU cpu;
  uint16_t address = 0xD428;
  cpu.execute(JMP, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_pc(), address);
}

TEST (SingleInstructionTest, JSR_JumpToSubroutine) {
  CPU cpu;
  uint16_t address = 0xD412;
  cpu.set_memory(address, 0x21);
  cpu.set_memory(address + 1, 0x43);
  cpu.set_pc(0xAFC1);

  uint8_t expected_pc_low = 0xC0;
  uint8_t expected_pc_high = 0xAF;
  uint16_t low_pc_add = 0x0100 + cpu.get_sp();
  uint16_t high_pc_add = 0x0100 + uint8_t(cpu.get_sp() + 1);

  uint8_t expected_sp = cpu.get_sp() + 2;
  cpu.execute(JSR, address, ABSOLUTE);

  EXPECT_EQ(cpu.get_pc(), 0x4321);
  EXPECT_EQ(cpu.get_sp(), expected_sp);
  EXPECT_EQ(cpu.get_memory(low_pc_add), expected_pc_low);
  EXPECT_EQ(cpu.get_memory(high_pc_add), expected_pc_high);
}

TEST (SingleInstructionTest, LDA_LoadAccumulator) {
  CPU cpu;
  uint16_t address = 0xD412;
  cpu.set_memory(address, 0xD4); // 1101 0100
  cpu.execute(LDA, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_acc(), 0xD4);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  // zero flag
  cpu = CPU();
  cpu.set_memory(address, 0);
  cpu.execute(LDA, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_acc(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, LDX_LoadXRegister) {
  CPU cpu;
  uint16_t address = 0xD412;
  cpu.set_memory(address, 0xD4); // 1101 0100
  cpu.execute(LDX, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_rx(), 0xD4);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  // zero flag
  cpu = CPU();
  cpu.set_memory(address, 0);
  cpu.execute(LDX, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_rx(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, LDY_LoadYRegister) {
  CPU cpu;
  uint16_t address = 0xD412;
  cpu.set_memory(address, 0xD4); // 1101 0100
  cpu.execute(LDY, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_ry(), 0xD4);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  // zero flag
  cpu = CPU();
  cpu.set_memory(address, 0);
  cpu.execute(LDY, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_ry(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, LSR_LogicalShiftRight) {
  CPU cpu;
  // accumulator mode
  cpu.set_acc(0xDA); // 1101 1010
  cpu.execute(LSR, 0, ACCUMULATOR);
  EXPECT_EQ(cpu.get_acc(), 0x6D); // 0110 1101
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 0);
  EXPECT_EQ(cpu.get_carry(), 0);

  // non accumulator mode
  uint16_t address = 0xD412;
  cpu.set_memory(address, 0x01);
  cpu.execute(LSR, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_memory(address), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
  EXPECT_EQ(cpu.get_carry(), 1);
}

TEST (SingleInstructionTest, ORA_LogicalInclusiveOr) {
  CPU cpu;
  uint16_t address = 0xD412;
  cpu.set_memory(address, 0x7A); // 0111 1010
  cpu.set_acc(0xA4);             // 1010 0100
  cpu.execute(ORA, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_acc(), 0xFE);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  // zero test
  cpu = CPU();
  cpu.set_memory(address, 0);
  cpu.set_acc(0);
  cpu.execute(ORA, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_acc(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, PHA_PushAccumulator) {
  CPU cpu;
  cpu.set_acc(0x45);
  uint8_t current_sp = cpu.get_sp();
  cpu.execute(PHA, 0, IMPLIED);
  EXPECT_EQ(cpu.get_memory(0x0100 + current_sp), 0x45);
  EXPECT_EQ(cpu.get_sp(), current_sp - 1);

  // stack overflow
  cpu = CPU();
  cpu.set_acc(0xAB);
  cpu.set_sp(0x00);
  cpu.execute(PHA, 0, IMPLIED);
  EXPECT_EQ(cpu.get_memory(0x0100), 0xAB);
  EXPECT_EQ(cpu.get_sp(), 0xFF);
}

TEST (SingleInstructionTest, PHP_PushProcessorStatus) {
  CPU cpu;
  cpu.set_carry();
  cpu.set_zero();
  cpu.set_interrupt_disable();
  cpu.set_overflow();
  cpu.set_negative();
  uint8_t expected = 0xC7;
  uint8_t current_sp = cpu.get_sp();
  cpu.execute(PHP, 0, IMPLIED);
  EXPECT_EQ(cpu.get_memory(0x0100 + current_sp), expected);
  EXPECT_EQ(cpu.get_sp(), current_sp - 1);
}

TEST (SingleInstructionTest, PLA_PullAccumulator) {
  CPU cpu;
  cpu.set_memory(0x0100 + cpu.get_sp() + 1, 0xD4); // 1101 0100
  uint8_t current_sp = cpu.get_sp();
  cpu.execute(PLA, 0, IMPLIED);
  EXPECT_EQ(cpu.get_acc(), 0xD4);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);
  EXPECT_EQ(cpu.get_sp(), current_sp + 1);

  // zero flag test
  cpu = CPU();
  cpu.set_memory(0x0100 + cpu.get_sp() + 1, 0);
  current_sp = cpu.get_sp();
  cpu.execute(PLA, 0, IMPLIED);
  EXPECT_EQ(cpu.get_acc(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
  EXPECT_EQ(cpu.get_sp(), current_sp + 1);
}

TEST (SingleInstructionTest, PLP_PullProcessorStatus) {
  CPU cpu;
  cpu.set_memory(0x0100 + cpu.get_sp() + 1, 0xC7);
  uint8_t current_sp = cpu.get_sp();
  cpu.execute(PLP, 0, IMPLIED);
  EXPECT_EQ(cpu.get_st(), 0xC7); //1100 0111
  EXPECT_EQ(cpu.get_sp(), current_sp + 1);
}

TEST (SingleInstructionTest, ROL_RotateLeft) {
  CPU cpu;
  // accumulator mode
  cpu.set_acc(0xFA); // 1111 1010
  cpu.set_carry();
  cpu.execute(ROL, 0, ACCUMULATOR);
  EXPECT_EQ(cpu.get_acc(), 0xF5); // 1111 0101
  EXPECT_EQ(cpu.get_carry(), 1);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  // other modes
  cpu = CPU();
  uint16_t address = 0xD412;
  cpu.clear_carry();
  cpu.set_memory(address, 0x80); // 1000 0000 
  cpu.execute(ROL, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_memory(address), 0);
  EXPECT_EQ(cpu.get_carry(), 1);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, ROR_RotateRight) {
  CPU cpu;
  // accumulator mode
  cpu.set_acc(0xA4); // 1010 0100 
  cpu.set_carry();
  cpu.execute(ROR, 0, ACCUMULATOR);
  EXPECT_EQ(cpu.get_acc(), 0xD2); // 1101 0010
  EXPECT_EQ(cpu.get_carry(), 0);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  // other modes
  cpu = CPU();
  uint16_t address = 0xD412;
  cpu.clear_carry();
  cpu.set_memory(address, 0x01); // 0000 0001
  cpu.execute(ROR, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_memory(address), 0);
  EXPECT_EQ(cpu.get_carry(), 1);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);

}

TEST (SingleInstructionTest, RTI_ReturnFromInterrupt) {
  CPU cpu;
  uint8_t old_st = 0xDA; // 1101 1010
  cpu.set_memory(0x0100 + uint8_t(cpu.get_sp() - 1), old_st);
  cpu.set_memory(0x0100 + uint8_t(cpu.get_sp() - 2), 0x12);
  cpu.set_memory(0x0100 + uint8_t(cpu.get_sp() - 3), 0x34);

  uint16_t old_pc = 0x1234;
  uint8_t expected_sp = cpu.get_sp() - 3;
  cpu.execute(RTI, 0, IMPLIED);

  EXPECT_EQ(cpu.get_pc(), old_pc);
  EXPECT_EQ(cpu.get_st(), old_st);
  EXPECT_EQ(cpu.get_sp(), expected_sp);
}

TEST (SingleInstructionTest, RTS_ReturnFromSubroutine) {
  CPU cpu;
  cpu.set_memory(0x0100 + uint8_t(cpu.get_sp() - 1), 0x012);
  cpu.set_memory(0x0100 + uint8_t(cpu.get_sp() - 2), 0x033);

  uint16_t old_pc = 0x01234;
  uint8_t expected_sp = cpu.get_sp() - 2;
  cpu.execute(RTS, 0, IMPLIED);

  EXPECT_EQ(cpu.get_pc(), old_pc);
  EXPECT_EQ(cpu.get_sp(), expected_sp);
}

TEST (SingleInstructionTest, SBC_SubtractWithCarry) {
  CPU cpu;
  uint16_t address = 0xD412;
  cpu.set_memory(address, 0x45); // 0100 0101
  cpu.set_carry();
  cpu.set_acc(0x2F);    // 0010 1111
  cpu.execute(SBC, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_acc(), 0xE9); // 1110 1001 
  EXPECT_EQ(cpu.get_carry(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);
  EXPECT_EQ(cpu.get_zero(), 0);

  // zero / carry test
  cpu = CPU();
  cpu.set_memory(address, 0x45);
  cpu.clear_carry();
  cpu.set_acc(0x45);
  cpu.execute(SBC, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_acc(), 0);
  EXPECT_EQ(cpu.get_carry(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
}

TEST (SingleInstructionTest, SEC_SetCarryFlag) {
  CPU cpu;
  cpu.clear_carry();
  cpu.execute(SEC, 0, IMPLIED);
  EXPECT_EQ(cpu.get_carry(), 1);

  cpu.set_carry();
  cpu.execute(SEC, 0, IMPLIED);
  EXPECT_EQ(cpu.get_carry(), 1);
}

TEST (SingleInstructionTest, SED_SetDecimalFlag) {
  CPU cpu;
  cpu.clear_decimal();
  cpu.execute(SED, 0, IMPLIED);
  EXPECT_EQ(cpu.get_decimal(), 1);

  cpu.set_decimal();
  cpu.execute(SED, 0, IMPLIED);
  EXPECT_EQ(cpu.get_decimal(), 1);
}

TEST (SingleInstructionTest, SEI_SetInterruptDisable) {
  CPU cpu;
  cpu.clear_interrupt_disable();
  cpu.execute(SEI, 0, IMPLIED);
  EXPECT_EQ(cpu.get_interrupt_disable(), 1);

  cpu.set_interrupt_disable();
  cpu.execute(SEI, 0, IMPLIED);
  EXPECT_EQ(cpu.get_interrupt_disable(), 1);
}

TEST (SingleInstructionTest, STA_StoreAccumulator) {
  CPU cpu;
  uint16_t address = 0xD412;
  cpu.set_acc(0xFC);
  cpu.execute(STA, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_memory(address), 0xFC);
}

TEST (SingleInstructionTest, STX_StoreXRegister) {
  CPU cpu;
  uint16_t address = 0xD412;
  cpu.set_rx(0xFC);
  cpu.execute(STX, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_memory(address), 0xFC);
}

TEST (SingleInstructionTest, STY_StoreYRegister) {
  CPU cpu;
  uint16_t address = 0xD412;
  cpu.set_ry(0xFC);
  cpu.execute(STY, address, ABSOLUTE);
  EXPECT_EQ(cpu.get_memory(address), 0xFC);
}

TEST (SingleInstructionTest, TAX_TransferAccumulatorToX) {
  CPU cpu;
  cpu.set_acc(0xFD);
  cpu.execute(TAX, 0, IMPLIED);
  EXPECT_EQ(cpu.get_rx(), 0xFD);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  // zero flag test
  cpu = CPU();
  cpu.set_acc(0);
  cpu.execute(TAX, 0, IMPLIED);
  EXPECT_EQ(cpu.get_rx(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, TAY_TransferAccumulatorToY) {
  CPU cpu;
  cpu.set_acc(0xFD);
  cpu.execute(TAY, 0, IMPLIED);
  EXPECT_EQ(cpu.get_ry(), 0xFD);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  // zero flag test
  cpu = CPU();
  cpu.set_acc(0);
  cpu.execute(TAY, 0, IMPLIED);
  EXPECT_EQ(cpu.get_ry(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, TSX_TransferStackPointerToX) {
  CPU cpu;
  cpu.set_sp(0xDF);
  cpu.execute(TSX, 0, IMPLIED);
  EXPECT_EQ(cpu.get_rx(), 0xDF);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  //zero flag test
  cpu = CPU();
  cpu.set_sp(0);
  cpu.execute(TSX, 0, IMPLIED);
  EXPECT_EQ(cpu.get_rx(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, TXA_TransferXToAccumulator) {
  CPU cpu;
  cpu.set_rx(0xDF);
  cpu.execute(TXA, 0, IMPLIED);
  EXPECT_EQ(cpu.get_acc(), 0xDF);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  //zero flag test
  cpu = CPU();
  cpu.set_rx(0);
  cpu.execute(TXA, 0, IMPLIED);
  EXPECT_EQ(cpu.get_acc(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, TXS_TransferXToStackPointer) {
  CPU cpu;
  cpu.set_rx(0xDF);
  cpu.execute(TXS, 0, IMPLIED);
  EXPECT_EQ(cpu.get_sp(), 0xDF);
}

TEST (SingleInstructionTest, TYA_TransferYToAccumulator) {
  CPU cpu;
  cpu.set_ry(0xDF);
  cpu.execute(TYA, 0, IMPLIED);
  EXPECT_EQ(cpu.get_acc(), 0xDF);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  //zero flag test
  cpu = CPU();
  cpu.set_ry(0);
  cpu.execute(TYA, 0, IMPLIED);
  EXPECT_EQ(cpu.get_acc(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
}

// Test status of pc, cycles after execution
TEST (SingleExecutionTest, ProgramCounterTest) {
  for (uint8_t opcode = 0; ; opcode++) {
    CPU cpu;
    uint16_t pc = cpu.get_pc();
    cpu.set_memory(pc, opcode);
    int error = cpu.step();
    int expected_error = 0;
    if (instruction_size[opcode] == -1) 
      expected_error = 1;

    EXPECT_EQ(error, expected_error); // error check
    
    if (expected_error == 0) {
      // don't check for brach and jump instructions
      if (opcode == 0x90 || opcode == 0xB0 || opcode == 0xF0 ||
        opcode == 0x30 || opcode == 0xD0 || opcode == 0x10 ||
        opcode == 0x50 || opcode == 0x4C || opcode == 0x6C ||
        opcode == 0x20 || opcode == 0x40 || opcode == 0x60 ||
        opcode == 0x00) {
        continue;
      }
      EXPECT_EQ(cpu.get_pc(), pc + instruction_size[opcode])
        << " instruction size = " << instruction_size[opcode] 
        << " old pc = " << std::hex << unsigned(pc)
        << " opcode = " << std::hex << unsigned(opcode);
    }

    if (opcode == 0xFF) break;
  }
}

//TODO handle instruction cycles
TEST (ClockCycleTest, ADC_AddWithCarry) {
  {// immediate mode
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x69); //adc immediate
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Zero page
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x65); //adc zero page
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Zero page X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x75); //adc zero page X
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x6D); //adc absolute
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x7D); //adc absolute X
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute Y
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x79); //adc absolute Y
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Indirect X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x61); //adc indirect X
    int expected = cpu.get_cycles() + 6;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Indirect Y
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x71); //adc indirect Y
    int expected = cpu.get_cycles() + 5;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, AND_LogicalAnd) {
  {// Immediate
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x29); // immediate
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Zero page
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x25); // zero page
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Zero page X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x35); // zero page X
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x2D); // absolute
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x3D); // absolute X
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute Y
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x39); // absolute Y
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Indirect X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x21); //adc indirect X
    int expected = cpu.get_cycles() + 6;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Indirect Y
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x31); //adc indirect Y
    int expected = cpu.get_cycles() + 5;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, ASL_ArithmeticShiftLeft) {
  {// Accumulator
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x0A); // accumulator
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Zero page
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x06); // zero page
    int expected = cpu.get_cycles() + 5;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Zero page X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x16); // zero page X
    int expected = cpu.get_cycles() + 6;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x0E); // absolute
    int expected = cpu.get_cycles() + 6;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x1E); // absolute X
    int expected = cpu.get_cycles() + 7;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, BCC_BranchIfCarryClear) {
  {// unsuccessful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_carry();
    cpu.set_memory(pc, 0x90); // bcc
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// successful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.clear_carry();
    cpu.set_memory(pc, 0x90); // bcc
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, BCS_BranchIfCarrySet) {
  {// unsuccessful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.clear_carry();
    cpu.set_memory(pc, 0xB0); // bcs
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// successful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_carry();
    cpu.set_memory(pc, 0xB0); // bcs
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, BEQ_BranchIfEqual) {
  {// unsuccessful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.clear_zero();
    cpu.set_memory(pc, 0xF0); // beq
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// successful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_zero();
    cpu.set_memory(pc, 0xF0); // beq
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, BIT_BitTest) {
  {// zero page
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x24); // bit zero page
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// absolute
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x2C); // bit absolute
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, BMI_BranchIfMinus) {
  {// unsuccessful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.clear_negative();
    cpu.set_memory(pc, 0x30); // bmi
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// successful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_negative();
    cpu.set_memory(pc, 0x30); // bmi
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, BNE_BranchIfNotEqual) {
  {// unsuccessful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_zero();
    cpu.set_memory(pc, 0xD0); // bne
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// successful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.clear_zero();
    cpu.set_memory(pc, 0xD0); // bne
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, BPL_BranchIfPositive) {
  {// unsuccessful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_negative();
    cpu.set_memory(pc, 0x10); // bpl
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// successful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.clear_negative();
    cpu.set_memory(pc, 0x10); // bpl
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, BRK_BreakInterrupt) {
  CPU cpu;
  int pc = cpu.get_pc();
  cpu.set_memory(pc, 0x00); // brk
  int expected = cpu.get_cycles() + 7;
  cpu.step();
  EXPECT_EQ(cpu.get_cycles(), expected);
}

TEST (ClockCycleTest, BVC_BranchIfOverflowClear) {
  {// unsuccessful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_overflow();
    cpu.set_memory(pc, 0x50); // bvc
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// successful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.clear_overflow();
    cpu.set_memory(pc, 0x50); // bvc
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, BVS_BranchIfOverFlowSet) {
  {// unsuccessful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.clear_overflow();
    cpu.set_memory(pc, 0x70); // bvs
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// successful branch
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_overflow();
    cpu.set_memory(pc, 0x70); // bvs
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, CLI_ClearInterruptDisable) {
  CPU cpu;
  int pc = cpu.get_pc();
  cpu.set_memory(pc, 0x58); // cli
  int expected = cpu.get_cycles() + 2;
  cpu.step();
  EXPECT_EQ(cpu.get_cycles(), expected);
}

TEST (ClockCycleTest, CLV_ClearOverflowFlag) {
  CPU cpu;
  int pc = cpu.get_pc();
  cpu.set_memory(pc, 0xB8); // clv
  int expected = cpu.get_cycles() + 2;
  cpu.step();
  EXPECT_EQ(cpu.get_cycles(), expected);
}

TEST (ClockCycleTest, CMP_Compare) {
  {// Immediate
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xC9); // immediate
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Zero page
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xC5); // zero page
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Zero page X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xD5); // zero page X
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xCD); // absolute
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xDD); // absolute X
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute Y
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xD9); // absolute Y
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Indirect X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xC1); //adc indirect X
    int expected = cpu.get_cycles() + 6;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Indirect Y
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xD1); //adc indirect Y
    int expected = cpu.get_cycles() + 5;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, CPX_CompareXRegister) {
  {// Immediate
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xE0); // immediate
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Zero page
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xE4); // zero page
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xEC); // absolute
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, CPY_CompareYRegister) {
  {// Immediate
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xC0); // immediate
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Zero page
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xC4); // zero page
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xCC); // absolute
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, DEC_DecrementMemory) {
  {// zero page
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xC6); // zero page
    int expected = cpu.get_cycles() + 5;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// zero page, X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xD6); // zero page X
    int expected = cpu.get_cycles() + 6;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xCE); // absolute
    int expected = cpu.get_cycles() + 6;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xDE); // absolute X
    int expected = cpu.get_cycles() + 7;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, DEX_DecrementXRegister) {
  CPU cpu;
  int pc = cpu.get_pc();
  cpu.set_memory(pc, 0xCA); // dex implied
  int expected = cpu.get_cycles() + 2;
  cpu.step();
  EXPECT_EQ(cpu.get_cycles(), expected);
}

TEST (ClockCycleTest, DEY_DecrementYRegister) {
  CPU cpu;
  int pc = cpu.get_pc();
  cpu.set_memory(pc, 0x88); // dey implied
  int expected = cpu.get_cycles() + 2;
  cpu.step();
  EXPECT_EQ(cpu.get_cycles(), expected);
}

TEST (ClockCycleTest, EOR_ExclusiveOR) {
  {// Immediate
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x49); // immediate
    int expected = cpu.get_cycles() + 2;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Zero page
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x45); // zero page
    int expected = cpu.get_cycles() + 3;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Zero page X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x55); // zero page X
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x4D); // absolute
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x5D); // absolute X
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute Y
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x59); // absolute Y
    int expected = cpu.get_cycles() + 4;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Indirect X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x41); //adc indirect X
    int expected = cpu.get_cycles() + 6;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Indirect Y
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0x51); //adc indirect Y
    int expected = cpu.get_cycles() + 5;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, INC_IncrementMemory) {
  {// zero page
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xE6); // zero page
    int expected = cpu.get_cycles() + 5;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// zero page, X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xF6); // zero page X
    int expected = cpu.get_cycles() + 6;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xEE); // absolute
    int expected = cpu.get_cycles() + 6;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
  {// Absolute X
    CPU cpu;
    int pc = cpu.get_pc();
    cpu.set_memory(pc, 0xFE); // absolute X
    int expected = cpu.get_cycles() + 7;
    cpu.step();
    EXPECT_EQ(cpu.get_cycles(), expected);
  }
}

TEST (ClockCycleTest, INX_IncrementXRegister) {
  CPU cpu;
  int pc = cpu.get_pc();
  cpu.set_memory(pc, 0xE8); // inx implied
  int expected = cpu.get_cycles() + 2;
  cpu.step();
  EXPECT_EQ(cpu.get_cycles(), expected);
}

TEST (ClockCycleTest, INY_IncrementYRegister) {
  CPU cpu;
  int pc = cpu.get_pc();
  cpu.set_memory(pc, 0xC8); // iny implied
  int expected = cpu.get_cycles() + 2;
  cpu.step();
  EXPECT_EQ(cpu.get_cycles(), expected);
}

//TODO hande instruction cycles with page different

} // namespace nesemu
