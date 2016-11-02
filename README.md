# Nes-Emulator
Nintendo Entertainment System Emulator

## Goals ##
The final goal is creating an emulator that can support c++ plugins to control and play real NES games.
The purpose is that AI developers can plugin AI agents to train and play real NES games.

## Design ##
5 main objects:
- The Console
- The Central Processing Unit (CPU)
- The Picture Processing Unit (PPU)
- The Cartridge
- The Controller

### The CPU ###
- Emulate the control of the 6502 CPU used in the NES.
- Implement the complete instruction set.
  + Addressing modes
  + Instruction cycles

#### Attributes ####
- An array 8-bit integers represent the memory of the CPU.
- 16-bit Program Counter (PC)
- 8-bit Stack Pointer (SP)
- 8-bit Accumulator
- 2 8-bit index-registers (X and Y, where X is the one that can do stack operations)
- 8-bit processor status (P)

#### Methods ####
- step(): executing the next instruction, updating all registers and memory accordingly

- helper methods:
  + execute(instruction, address_mode) : executing a specific instruction (This method is currently use mainly for testing)
