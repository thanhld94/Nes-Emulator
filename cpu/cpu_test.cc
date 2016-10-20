#include "cpu.h"

#include "gtest/gtest.h"
#include "test_utils.h"

namespace nesemu {

TEST (InitializeTest, FirstState) {
  CPU cpu;
  EXPECT_EQ(cpu.get_pc(), 0x34);
  EXPECT_EQ(cpu.get_sp(), 0xFD); // stack is top-down, 0x01FF - 0x0100
  EXPECT_EQ(cpu.get_rx(), 0);
  EXPECT_EQ(cpu.get_ry(), 0);
  EXPECT_EQ(cpu.get_acc(), 0);
  EXPECT_EQ(cpu.get_st(), 0);
  for (int i = 0x00; i <= 0xFFFF; i++) {
    EXPECT_EQ(cpu.get_memory(i), 0);
  }
}

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
//TODO loop test through all addressing modes
//TODO test bad inputs
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
  uint16_t address = 0x1412;
  cpu.set_zero();
  cpu.set_negative();
  cpu.set_memory(address, 0xDF); //1101 1111
  cpu.set_acc(0x4A);             //0100 1010

  uint8_t expected = 0xDF & 0x4A;
  cpu.execute(AND, address, ABSOLUTE);

  EXPECT_EQ(cpu.get_acc(), expected);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 0);
}

TEST (SingleInstructionTest, AND_StatusRegisterCheck) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.clear_zero();
  cpu.clear_negative();
  cpu.set_memory(address, 0xDF); //1101 1111
  cpu.set_acc(0xCA);             //1100 1010

  uint8_t expected = 0xDF & 0xCA;
  cpu.execute(AND, address, ABSOLUTE);

  EXPECT_EQ(cpu.get_acc(), expected);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  cpu.set_acc(0xCA);             //1100 1010
  cpu.set_memory(address, 0x35); //0011 0101 

  cpu.execute(AND, address, ACCUMULATOR);

  EXPECT_EQ(cpu.get_acc(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);
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
  cpu.execute(BCC, address, ABSOLUTE);
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

//TODO BRK

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

//TODO JSR

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

//TODO RTI
//TODO RTS

//TODO SBC
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

//TODO SEC
//TODO SED
//TODO SEI
//TODO STA
//TODO STX
//TODO STY
//TODO TAX
//TODO TAY
//TODO TSX
//TODO TXA
//TODO TXS
//TODO TYA

} // namespace nesemu
