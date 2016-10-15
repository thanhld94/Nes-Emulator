#ifndef NESEMU_CPU_CPU_H_
#define NESEMU_CPU_CPU_H_

#include <iostream>
#include <string>

namespace nesemu {

class CPU {
  public:
    CPU();

    /* Addressing Mode */
    int get_addressing_mode(uint8_t opcode) const;
    uint8_t get_operand(uint8_t opcode) const;

    /* Cpu instructions */
    void Adc(uint8_t opcode);

    /* Getters & Setters*/
    uint16_t get_pc() const;
    void set_pc(uint16_t value);

    uint8_t get_sp() const;

    uint8_t get_rx() const;
    void set_rx(uint8_t value);

    uint8_t get_ry() const;
    void set_ry(uint8_t value);

    uint8_t get_acc() const;
    uint8_t get_st() const;

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
