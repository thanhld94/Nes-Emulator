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

    /* Getters & Setters*/
    uint16_t get_pc() const;
    uint8_t get_sp() const;
    uint8_t get_rx() const;
    uint8_t get_ry() const;
    uint8_t get_acc() const;
    uint8_t get_st() const;

  private:
    uint16_t pc;
    uint8_t sp;
    uint8_t r_x;
    uint8_t r_y;
    uint8_t r_acc;
    uint8_t r_st;

    static const int mode_table[256]; 
};

} // namespace  nessim

#endif // NESEMU_CPU_CPU_H_
