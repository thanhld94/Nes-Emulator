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
    // 7 6 5 4 3 2 1 0
    // N V   B D I Z C
    uint8_t get_st() const;

    // set and clear carry flag
    int get_carry() const;
    void set_carry();
    void clear_carry();

    // set and cleaer negative flag
    int get_negative() const;
    void set_negative();
    void clear_negative();

    // set and clear zero flag
    int get_zero() const;
    void set_zero();
    void clear_zero();

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
