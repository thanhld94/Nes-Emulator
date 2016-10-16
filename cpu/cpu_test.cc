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

// Testing addressing mode table
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

TEST (AddressingModeTest, DEC_DecrementMemory) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xC6), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0xD6), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0xCE), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0xDE), ABSOLUTEX);
}

TEST (AddressingModeTest, DEX_DecrementXRegister) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xCA), IMPLIED); 
}

TEST (AddressingModeTest, DEY_DecrementYRegister) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x88), IMPLIED); 
}

TEST (AddressingModeTest, XOR_ExclusiveOR) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x49), IMMEDIATE);
  EXPECT_EQ(cpu.get_addressing_mode(0x45), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0x55), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x4D), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0x5D), ABSOLUTEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x59), ABSOLUTEY);
  EXPECT_EQ(cpu.get_addressing_mode(0x41), INDIRECTX);
  EXPECT_EQ(cpu.get_addressing_mode(0x51), INDIRECTY);
}

TEST (AddressingModeTest, INC_IncrementMemory) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xE6), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0xF6), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0xEE), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0xFE), ABSOLUTEX);
} 

TEST (AddressingModeTest, INX_IncrementXRegister) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xE8), IMPLIED);
}

TEST (AddressingModeTest, INY_IncrementYRegister) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xC8), IMPLIED);
}

TEST (AddressingModeTest, JMP_Jump) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x4C), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0x6C), INDIRECT);
}

TEST (AddressingModeTest, JSR_JumpToSubroutine) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x20), ABSOLUTE);
}

TEST (AddressingModeTest, LDA_LoadAccumulator) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xA9), IMMEDIATE);
  EXPECT_EQ(cpu.get_addressing_mode(0xA5), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0xB5), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0xAD), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0xBD), ABSOLUTEX);
  EXPECT_EQ(cpu.get_addressing_mode(0xB9), ABSOLUTEY);
  EXPECT_EQ(cpu.get_addressing_mode(0xA1), INDIRECTX);
  EXPECT_EQ(cpu.get_addressing_mode(0xB1), INDIRECTY);
}

TEST (AddressingModeTest, LDX_LoadXRegister) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xA2), IMMEDIATE);
  EXPECT_EQ(cpu.get_addressing_mode(0xA6), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0xB6), ZEROPAGEY);
  EXPECT_EQ(cpu.get_addressing_mode(0xAE), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0xBE), ABSOLUTEY);
}

TEST (AddressingModeTest, LDY_LoadYRegister) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xA0), IMMEDIATE);
  EXPECT_EQ(cpu.get_addressing_mode(0xA4), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0xB4), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0xAC), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0xBC), ABSOLUTEX);
}

TEST (AddressingModeTest, LSR_LogicalShiftRight) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x4A), ACCUMULATOR);
  EXPECT_EQ(cpu.get_addressing_mode(0x46), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0x56), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x4E), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0x5E), ABSOLUTEX);
}

TEST (AddressingModeTest, NOP_NoOpoeration) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xEA), IMPLIED);
}

TEST (AddressingModeTest, ORA_LogicalInculsiveOR) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x09), IMMEDIATE);
  EXPECT_EQ(cpu.get_addressing_mode(0x05), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0x15), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x0D), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0x1D), ABSOLUTEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x19), ABSOLUTEY);
  EXPECT_EQ(cpu.get_addressing_mode(0x01), INDIRECTX);
  EXPECT_EQ(cpu.get_addressing_mode(0x11), INDIRECTY);
}

TEST (AddressingModeTest, PHA_PushAccumulator) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x48), IMPLIED);
}

TEST (AddressingModeTest, PHP_PushProcessorStatus) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x08), IMPLIED);
}

TEST (AddressingModeTest, PLP_PullPorcessorStatus) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x28), IMPLIED);
}

TEST (AddressingModeTest, ROL_RotateLeft) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x2A), ACCUMULATOR);
  EXPECT_EQ(cpu.get_addressing_mode(0x26), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0x36), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x2E), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0x3E), ABSOLUTEX);
}

TEST (AddressingModeTest, ROR_RotateRight) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x6A), ACCUMULATOR);
  EXPECT_EQ(cpu.get_addressing_mode(0x66), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0x76), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x6E), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0x7E), ABSOLUTEX);
}

TEST (AddressingModeTest, RTI_ReturnFromInterrupt) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x40), IMPLIED);
}

TEST (AddressingModeTest, RTS_ReturnFromSubroutine) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x60), IMPLIED);
}

TEST (AddressingModeTest, SBC_SubtractWithCarry) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xE9), IMMEDIATE);
  EXPECT_EQ(cpu.get_addressing_mode(0xE5), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0xF5), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0xED), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0xFD), ABSOLUTEX);
  EXPECT_EQ(cpu.get_addressing_mode(0xF9), ABSOLUTEY);
  EXPECT_EQ(cpu.get_addressing_mode(0xE1), INDIRECTX);
  EXPECT_EQ(cpu.get_addressing_mode(0xF1), INDIRECTY);
}

TEST (AddressingModeTest, SEC_SetCarryFlag) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x38), IMPLIED);
}

TEST (AddressingModeTest, SED_SetDecimalFlag) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xF8), IMPLIED);
}

TEST (AddressingModeTest, SEI_SetInterruptDecimal) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x78), IMPLIED);
}

TEST (AddressingModeTest, STA_StoreAccumulator) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x85), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0x95), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x8D), ABSOLUTE);
  EXPECT_EQ(cpu.get_addressing_mode(0x9D), ABSOLUTEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x99), ABSOLUTEY);
  EXPECT_EQ(cpu.get_addressing_mode(0x81), INDIRECTX);
  EXPECT_EQ(cpu.get_addressing_mode(0x91), INDIRECTY);
}

TEST (AddressingModeTest, STX_StoreXRegister) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x86), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0x96), ZEROPAGEY);
  EXPECT_EQ(cpu.get_addressing_mode(0x8E), ABSOLUTE);
}

TEST (AddressingModeTest, STY_StoreYRegister) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x84), ZEROPAGE);
  EXPECT_EQ(cpu.get_addressing_mode(0x94), ZEROPAGEX);
  EXPECT_EQ(cpu.get_addressing_mode(0x8C), ABSOLUTE);
}

TEST (AddressingModeTest, TAX_TransferAccumulatorToX) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xAA), IMPLIED);
}

TEST (AddressingModeTest, TAY_TransferAccumulatorToY) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xA8), IMPLIED);
}

TEST (AddressingModeTest, TSX_TransferStackPointerToX) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0xBA), IMPLIED);
}

TEST (AddressingModeTest, TXA_TransferXToAccumulator) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x8A), IMPLIED);
}

TEST (AddressingModeTest, TXS_TransferXToStackPointer) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x9A), IMPLIED);
}

TEST (AddressingModeTest, TYA_TransferYToAccumulator) {
  CPU cpu;
  EXPECT_EQ(cpu.get_addressing_mode(0x98), IMPLIED);
}


// Fetching target based on addressing mode 
TEST (AddressingModeOperand, GetOperandImmediateAddress) {
  CPU cpu;
  uint16_t imm_op = cpu.get_operand(0x69);
  EXPECT_EQ(imm_op, cpu.get_pc() + 1);
}

TEST (AddressingModeOperand, GetOperandZeroPageAddress) {
  CPU cpu;
  cpu.set_memory(cpu.get_pc() + 1, 0x43);
  uint16_t zp_op = cpu.get_operand(0x65);
  EXPECT_EQ(zp_op, 0x43);
}

TEST (AddressingModeOperand, GetOperandZeroPageXAddress) {
  CPU cpu;
  cpu.set_memory(cpu.get_pc() + 1, 0xF4);
  cpu.set_rx(0x31);
  uint16_t zpx_op = cpu.get_operand(0x75);
  EXPECT_EQ(zpx_op, 0x25);
}

TEST (AddressingModeOperand, GetOperandZeroPageYAddress) {
  CPU cpu;
  cpu.set_memory(cpu.get_pc() + 1, 0x88);
  cpu.set_ry(0xA4);
  uint16_t zpy_op = cpu.get_operand(0xB6);
  EXPECT_EQ(zpy_op, 0x2C);
}

TEST (AddressingModeOperand, GetOperandAbsoluteAddress) {
  CPU cpu;
  cpu.set_memory(cpu.get_pc() + 1, 0x34);
  cpu.set_memory(cpu.get_pc() + 2, 0x12);
  uint16_t abs_op = cpu.get_operand(0x6D);
  EXPECT_EQ(abs_op, 0x1234);
}

TEST (AddressingModeOperand, GetOperandAbsoluteXAddress) {
  CPU cpu;
  cpu.set_memory(cpu.get_pc() + 1, 0x00);
  cpu.set_memory(cpu.get_pc() + 2, 0x12);
  cpu.set_rx(0xF3);
  uint16_t absx_op = cpu.get_operand(0x7D);
  EXPECT_EQ(absx_op, 0x12F3);
}

TEST (AddressingModeOperand, GetOperandAbsoluteYAddress) {
  CPU cpu;
  cpu.set_memory(cpu.get_pc() + 1, 0);
  cpu.set_memory(cpu.get_pc() + 2, 0x12);
  cpu.set_ry(0xFF);
  uint16_t absy_op = cpu.get_operand(0x79);
  EXPECT_EQ(absy_op, 0x12FF);
}

TEST (AddressingModeOperand, GetOperandIndirectXAddress) {
  CPU cpu;
  cpu.set_memory(cpu.get_pc() + 1, 0x45);
  cpu.set_memory(0x45, 0x68);
  cpu.set_rx(0xD3);
  cpu.set_memory(0x3B, 0x34);
  cpu.set_memory(0x3C, 0x12);
  uint16_t indirx_op = cpu.get_operand(0xA1);
  EXPECT_EQ(indirx_op, 0x1234);
}

TEST (AddressingModeOperand, GetOperandIndirectYAddress) {
  CPU cpu;
  cpu.set_memory(cpu.get_pc() + 1, 0x45);
  cpu.set_memory(0x45, 0x34);
  cpu.set_memory(0x46, 0x12);
  cpu.set_ry(0x45);
  uint16_t indiry_op = cpu.get_operand(0xB1);
  EXPECT_EQ(indiry_op, 0x1279);
}

TEST (AddressingModeOperand, GetOperandAccumulatorAddress) {
  CPU cpu;
  EXPECT_EQ(cpu.get_operand(0x0A), 0); 
}

TEST (AddressingModeOperand, GetOperandRelativeAddress) {
  CPU cpu;
  cpu.set_memory(cpu.get_pc() + 1, 0x3F);
  uint16_t rel_op = cpu.get_operand(0x90);
  EXPECT_EQ(rel_op, cpu.get_pc() + 2 + 0x3F);
}

TEST (AddressingModeOperand, GetOperandRelativeNegativeAddress) {
  CPU cpu;
  cpu.set_memory(cpu.get_pc() + 1, int8_t(-3));
  uint16_t rel_op = cpu.get_operand(0x90);
  EXPECT_EQ(rel_op, cpu.get_pc() - 1);
}

TEST (AddressingModeOperand, GetOperandImpliedAddress) {
  CPU cpu;
  EXPECT_EQ(cpu.get_operand(0x18), 0);
}

TEST (AddressingModeOperand, GetOperandIndirectAddress) {
  CPU cpu;
  cpu.set_memory(cpu.get_pc() + 1, 0x34);
  cpu.set_memory(cpu.get_pc() + 2, 0x12);
  cpu.set_memory(0x1234, 0x76);
  cpu.set_memory(0x1235, 0x98);
  uint16_t indir_op = cpu.get_operand(0x6C);
  EXPECT_EQ(indir_op, 0x9876);
}

// Single Instruction Tests
TEST (SingleInstructionTest, ADC_AddWithCarry) {
  CPU cpu;
  uint16_t address = 0x3412;
  cpu.set_memory(address, 0x14);
  cpu.set_carry();
  cpu.set_acc(0xFC);
  cpu.set_negative();
  cpu.set_zero();

  cpu.Adc(address);
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
  
  cpu.Adc(address);
  EXPECT_EQ(cpu.get_acc(), 0);
  EXPECT_EQ(cpu.get_carry(), 0);
  EXPECT_EQ(cpu.get_zero(), 1);
  EXPECT_EQ(cpu.get_negative(), 0);

  cpu.set_acc(0x8F);
  cpu.set_memory(address, 0x34);
  cpu.Adc(address);
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
  cpu.And(address);

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
  cpu.And(address);

  EXPECT_EQ(cpu.get_acc(), expected);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 1);

  cpu.set_acc(0xCA);             //1100 1010
  cpu.set_memory(address, 0x35); //0011 0101 

  cpu.And(address);

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
    cpu.Asl(address, addressing_mode);
    
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
  cpu.Asl(address, ACCUMULATOR);

  EXPECT_EQ(cpu.get_acc(), expected);
  EXPECT_EQ(cpu.get_zero(), 0);
  EXPECT_EQ(cpu.get_negative(), 0);
  EXPECT_EQ(cpu.get_carry(), 0);
}

//TODO BCC
TEST (SingleInstructionTest, BCC_BranchIfCarryClear) {
  CPU cpu;
  uint16_t address = 0x1412;
  cpu.clear_carry();
  cpu.Bcc(address);
  EXPECT_EQ(cpu.get_pc(), address);

  cpu.set_pc(0x2324);
  cpu.set_carry();
  cpu.Bcc(address);
  EXPECT_EQ(cpu.get_pc(), 0x2324);
}

//TODO BCS
//TODO BEQ
//TODO BIT
//TODO BMI
//TODO BNE
//TODO BPL
//TODO BRK
//TODO BVC
//TODO BVS
//TODO CLC
//TODO CLD
//TODO CLI
//TODO CLV
//TODO CMP
//TODO CPX
//TODO CPY
//TODO DEC
//TODO DEX
//TODO DEY
//TODO EOR
//TODO INC
//TODO INX
//TODO INY
//TODO JMP
//TODO JSR
//TODO LDA
//TODO LDX
//TODO LDY
//TODO LSR
//TODO NOP
//TODO ORA
//TODO PHA
//TODO PHP
//TODO PLA
//TODO PLP
//TODO ROL
//TODO ROR
//TODO RTI
//TODO RTS
//TODO SBC
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
