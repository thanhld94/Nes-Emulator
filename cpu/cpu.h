#ifndef NESEMU_CPU_CPU_H_
#define NESEMU_CPU_CPU_H_

#include <iostream>

namespace nesemu {

class CPU {
  public:
    CPU();

    /* Cpu instructions */
    int step();
    int execute(int instruction, uint16_t address, int mode);

    /* Getters & Setters*/
    uint16_t get_pc() const;
    void set_pc(uint16_t value);

    uint8_t get_sp() const;
    void set_sp(uint8_t value);

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

    // set and clear break flag
    void set_break();

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

    /* Instruction execution helper */
    // get the memory address of the operand based on opcode
    uint16_t get_operand(uint8_t opcode) const;

    uint8_t memory[0x10000]; // System memory
};

} // namespace  nessim

#endif // NESEMU_CPU_CPU_H_
