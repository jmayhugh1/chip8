#pragma once
#include "memConstants.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <random>
#include <stack>

class Chip8 {
private:
  friend class Chip8FuncTests;
  uint8_t registers[NUM_REGISTERS]{};
  uint8_t memory[MEMORY_SIZE]{};
  uint16_t index{};
  uint16_t pc{}; // holds 2 bytes, must index into memory
  std::stack<uint16_t> stack;
  uint8_t delayTimer{};
  uint8_t soundTimer{};
  uint8_t keypad[INPUT_KEYS]{};
  uint32_t video[SCREEN_WIDTH * SCREEN_HEIGHT]{};
  uint16_t opcode;

  std::default_random_engine randGen;
  std::uniform_int_distribution<uint8_t> randByte;

public:
  Chip8();
  void cls();               // 00E0 clear the display
  void ret();               // 00EE return from a subroutine
  void jump(uint16_t addr); // 1nnn jump to addr nnn
  void call(uint16_t addr); // 2nnn calls the subroutine at nnn
  void se_const(uint8_t vx,
                uint8_t kk); // 3xkk skip next instruction if register Vx == kk
  void sne_const(uint8_t vx,
                 uint8_t kk); // 4xkk skip next instruction if register Vx == kk
  void se_reg(uint8_t vx,
              uint8_t vy); // 5xy0 skip next instruction if register Vx == Vy

  void ld_const(uint8_t vx, uint8_t kk); // 6xkk load kk into register Vx
  void ld_reg(uint8_t vx, uint8_t vy);   // 8xy0 load kk into register Vx

  void add_const(
      uint8_t vx,
      uint8_t kk); // 7xkk  adds kk to the value of regiser Vx then store in vx

  void or_reg(uint8_t vx, uint8_t vy); // 8xy1 bitwise or of the elements of vx
  // and vy, stores the result in vx

  void and_reg(uint8_t vx,
               uint8_t vy); // 8xy2 bitwise and of the elements of vx
  // and vy, stores the result in vx
  void xor_reg(uint8_t vx,
               uint8_t vy); // 8xy3 bitwise xor of the elements of vx
  // and vy, stores the result in vx

  void add_reg(uint8_t vx,
               uint8_t vy); // 8xy4 add elements of vx
  // and vy, stores the result in vx

  void sub_reg(uint8_t vx, uint8_t vy); // 8xy5 subtract vy from vx, store in vx

  void shr_reg(uint8_t vx); // 8xy6 shift vx right by 1
  void subn_reg(uint8_t vx,
                uint8_t vy); // 8xy7 subtract vx from vy, store in vx
  void shl_reg(uint8_t vx);  // 8xyE shift vx left by 1
  void sne_reg(uint8_t vx,
               uint8_t vy); // 9xy0 skip next instruction if vx != vy

  void ld_i(uint16_t addr);         // Annn load index register with addr
  void jp_v0(uint16_t addr);        // Bnnn jump to addr + V0
  void rnd(uint8_t vx, uint8_t kk); // Cxkk set vx = random byte AND kk
  void drw(uint8_t vx, uint8_t vy,
           uint8_t height);  // Dxyn draw sprite at (vx, vy) with height
  void skp(uint8_t vx);      // Ex9E skip if key vx is pressed
  void sknp(uint8_t vx);     // ExA1 skip if key vx is not pressed
  void ld_vx_dt(uint8_t vx); // Fx07 set vx = delay timer
  void ld_vx_k(uint8_t vx);  // Fx0A wait for key press, store in vx
  void ld_dt_vx(uint8_t vx); // Fx15 set delay timer = vx
  void ld_st_vx(uint8_t vx); // Fx18 set sound timer = vx
  void add_i_vx(uint8_t vx); // Fx1E set I = I + vx
  void ld_f_vx(uint8_t vx);  // Fx29 set I = location of sprite for digit vx
  void ld_b_vx(uint8_t vx);  // Fx33 store BCD of vx in memory at I, I+1, I+2
  void ld_i_vx(uint8_t vx); // Fx55 store registers V0 through Vx in memory at I
  void
  ld_vx_i(uint8_t vx); // Fx65 read registers V0 through Vx from memory at I
};