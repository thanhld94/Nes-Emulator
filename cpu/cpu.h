#ifndef NESEMU_CPU_CPU_H_
#define NESEMU_CPU_CPU_H_

#include <iostream>

namespace nesemu {

class CPU {
  public:
    CPU();

    /* Addressing Mode */

    // return the address mode correspond to opcode
    int get_addressing_mode(uint8_t opcode) const;

    // get the memory address of the opcode based on opcode
    uint16_t get_operand(uint8_t opcode) const;

    /* Cpu instructions */
    void Adc(uint16_t address);
    void And(uint16_t address);
    void Asl(uint16_t address, int mode);
    void Bcc(uint16_t address);
    void Bcs(uint16_t address);
    void Beq(uint16_t address);
    void Bit(uint16_t address);
    void Bmi(uint16_t address);
    void Bne(uint16_t address);
    void Bpl(uint16_t address);
    void Bvc(uint16_t address);
    void Bvs(uint16_t address);
    void Clc();
    void Cld();
    void Cli();
    void Clv();
    void Cmp(uint16_t address);
    void Cpx(uint16_t address);
    void Cpy(uint16_t address);
    void Dec(uint16_t address);
    void Dex();
    void Dey();
    void Eor(uint16_t address);
    void Inc(uint16_t address);
    void Inx();
    void Iny();

    /* Getters & Setters*/
    uint16_t get_pc() const;
    void set_pc(uint16_t value);

    uint8_t get_sp() const;

    uint8_t get_rx() const;
    void set_rx(uint8_t value);

    uint8_t get_ry() const;
    void set_ry(uint8_t value);

    uint8_t get_acc() const;
    void set_acc(uint8_t value);

    // Status register
    uint8_t get_st() const;

    // set and clear carry flag
    int get_carry() const;
    void set_carry();
    void clear_carry();

    // set and clear zero flag
    int get_zero() const;
    void set_zero();
    void clear_zero();

    // set and clear interrupt disable flag
    int get_interrupt_disable() const;
    void set_interrupt_disable();
    void clear_interrupt_disable();

    // set and clear decimal flag
    int get_decimal() const;
    void set_decimal();
    void clear_decimal();

    // set and clear overflow flag
    int get_overflow() const;
    void set_overflow();
    void clear_overflow();

    // set and clear negative flag
    int get_negative() const;
    void set_negative();
    void clear_negative();

    uint8_t get_memory(uint16_t address) const;
    void set_memory(uint16_t address, uint8_t value);

  private:
    uint16_t pc;
    uint8_t sp;
    uint8_t r_x;
    uint8_t r_y;
    uint8_t r_acc;
    uint8_t r_st;

    // System memory
    uint8_t memory[0x10000];

    static const int mode_table[256]; 
};

} // namespace  nessim

#endif // NESEMU_CPU_CPU_H_
