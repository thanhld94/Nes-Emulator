#include <iostream>
#include <string>

class CPU {
  public:
    CPU();

    /* Addressing Mode */
    int get_addressing_mode(uint8_t opcode);

    /* Getters & Setters*/
    uint16_t get_pc();
    uint8_t get_sp();
    uint8_t get_rx();
    uint8_t get_ry();
    uint8_t get_acc();
    uint8_t get_st();

  private:
    uint16_t pc;
    uint8_t sp;
    uint8_t r_x;
    uint8_t r_y;
    uint8_t r_acc;
    uint8_t r_st;

    static const int mode_table[256]; 
};
