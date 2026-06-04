#include "Chip8.h"
#include <chrono>
#include <cstdint>

Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
  pc = ROM_START_ADDRESS;

  randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

  for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
    memory[i + FONTSET_START_ADDRESS] = fontset[i];
  }
}

// Instructions
void Chip8::cls() { memset(video, 0, sizeof(video)); }

void Chip8::ret() {
  if (stack.empty()) {
    pc = ROM_START_ADDRESS;
  }
  pc = stack.top();
  stack.pop();
}
void Chip8::jump(uint16_t addr) { pc = addr; }

void Chip8::call(uint16_t addr) {
  stack.push(pc);
  pc = addr;
}

void Chip8::se_const(uint8_t vx, uint8_t kk) {
  if ((CONVERT_8TO4BITS(vx)) == kk)
    pc += 2;
}

void Chip8::sne_const(uint8_t vx, uint8_t kk) {
  if ((CONVERT_8TO4BITS(vx)) != kk)
    pc += 2;
}

void Chip8::se_reg(uint8_t vx, uint8_t vy) {
  if ((CONVERT_8TO4BITS(vx)) == (CONVERT_8TO4BITS(vy)))
    pc += 2;
}

void Chip8::ld_const(uint8_t vx, uint8_t kk) {
  registers[CONVERT_8TO4BITS(vx)] = kk;
}

void Chip8::ld_reg(uint8_t vx, uint8_t vy) {
  registers[CONVERT_8TO4BITS(vx)] = registers[CONVERT_8TO4BITS(vy)];
}

void Chip8::add_const(uint8_t vx, uint8_t kk) {
  registers[CONVERT_8TO4BITS(vx)] += kk;
}

void Chip8::or_reg(uint8_t vx, uint8_t vy) {
  registers[CONVERT_8TO4BITS(vx)] |= registers[CONVERT_8TO4BITS(vy)];
}

void Chip8::and_reg(uint8_t vx, uint8_t vy) {
  registers[CONVERT_8TO4BITS(vx)] &= registers[CONVERT_8TO4BITS(vy)];
}

void Chip8::xor_reg(uint8_t vx, uint8_t vy) {
  registers[CONVERT_8TO4BITS(vx)] ^= registers[CONVERT_8TO4BITS(vy)];
}

void Chip8::add_reg(uint8_t vx, uint8_t vy) {
  uint16_t sum = static_cast<uint16_t>(registers[vx]) + registers[vy];

  registers[CARRY_REG] = sum > 0xFF ? 1 : 0;
  registers[vx] = static_cast<uint8_t>(sum & 0xFF);
}

void Chip8::sub_reg(uint8_t vx, uint8_t vy) {
  registers[CARRY_REG] =
      registers[CONVERT_8TO4BITS(vx)] > registers[CONVERT_8TO4BITS(vy)] ? 1 : 0;

  registers[CONVERT_8TO4BITS(vx)] -= registers[CONVERT_8TO4BITS(vy)];
}

void Chip8::shr_reg(uint8_t vx) {
  registers[CARRY_REG] = registers[CONVERT_8TO4BITS(vx)] & 0x1;
  registers[CONVERT_8TO4BITS(vx)] >>= 1;
}

void Chip8::subn_reg(uint8_t vx, uint8_t vy) {
  registers[CARRY_REG] =
      registers[CONVERT_8TO4BITS(vy)] > registers[CONVERT_8TO4BITS(vx)] ? 1 : 0;

  registers[CONVERT_8TO4BITS(vx)] =
      registers[CONVERT_8TO4BITS(vy)] - registers[CONVERT_8TO4BITS(vx)];
}

void Chip8::shl_reg(uint8_t vx) {
  registers[CARRY_REG] = (registers[CONVERT_8TO4BITS(vx)] & 0x80) >> 7;
  registers[CONVERT_8TO4BITS(vx)] <<= 1;
}

void Chip8::sne_reg(uint8_t vx, uint8_t vy) {
  if (registers[CONVERT_8TO4BITS(vx)] != registers[CONVERT_8TO4BITS(vy)])
    pc += 2;
}

void Chip8::ld_i(uint16_t addr) { index = addr; }

void Chip8::jp_v0(uint16_t addr) { pc = registers[0] + addr; }

void Chip8::rnd(uint8_t vx, uint8_t kk) {
  registers[CONVERT_8TO4BITS(vx)] = randByte(randGen) & kk;
}

void Chip8::drw(uint8_t vx, uint8_t vy, uint8_t height) {
  uint8_t xPos = registers[CONVERT_8TO4BITS(vx)] % SCREEN_WIDTH;
  uint8_t yPos = registers[CONVERT_8TO4BITS(vy)] % SCREEN_HEIGHT;

  registers[CARRY_REG] = 0;

  for (unsigned int row = 0; row < height; ++row) {
    uint8_t spriteByte = memory[index + row];

    for (unsigned int col = 0; col < 8; ++col) {
      uint8_t spritePixel = spriteByte & (0x80 >> col);
      uint32_t *screenPixel =
          &video[(yPos + row) * SCREEN_WIDTH + (xPos + col)];

      if (spritePixel) {
        if (*screenPixel == 0xFFFFFFFF) {
          registers[CARRY_REG] = 1;
        }
        *screenPixel ^= 0xFFFFFFFF;
      }
    }
  }
}

void Chip8::skp(uint8_t vx) {
  uint8_t key = registers[CONVERT_8TO4BITS(vx)];
  if (keypad[key])
    pc += 2;
}

void Chip8::sknp(uint8_t vx) {
  uint8_t key = registers[CONVERT_8TO4BITS(vx)];
  if (!keypad[key])
    pc += 2;
}

void Chip8::ld_vx_dt(uint8_t vx) {
  registers[CONVERT_8TO4BITS(vx)] = delayTimer;
}

void Chip8::ld_vx_k(uint8_t vx) {
  if (keypad[0]) {
    registers[CONVERT_8TO4BITS(vx)] = 0;
  } else if (keypad[1]) {
    registers[CONVERT_8TO4BITS(vx)] = 1;
  } else if (keypad[2]) {
    registers[CONVERT_8TO4BITS(vx)] = 2;
  } else if (keypad[3]) {
    registers[CONVERT_8TO4BITS(vx)] = 3;
  } else if (keypad[4]) {
    registers[CONVERT_8TO4BITS(vx)] = 4;
  } else if (keypad[5]) {
    registers[CONVERT_8TO4BITS(vx)] = 5;
  } else if (keypad[6]) {
    registers[CONVERT_8TO4BITS(vx)] = 6;
  } else if (keypad[7]) {
    registers[CONVERT_8TO4BITS(vx)] = 7;
  } else if (keypad[8]) {
    registers[CONVERT_8TO4BITS(vx)] = 8;
  } else if (keypad[9]) {
    registers[CONVERT_8TO4BITS(vx)] = 9;
  } else if (keypad[10]) {
    registers[CONVERT_8TO4BITS(vx)] = 10;
  } else if (keypad[11]) {
    registers[CONVERT_8TO4BITS(vx)] = 11;
  } else if (keypad[12]) {
    registers[CONVERT_8TO4BITS(vx)] = 12;
  } else if (keypad[13]) {
    registers[CONVERT_8TO4BITS(vx)] = 13;
  } else if (keypad[14]) {
    registers[CONVERT_8TO4BITS(vx)] = 14;
  } else if (keypad[15]) {
    registers[CONVERT_8TO4BITS(vx)] = 15;
  } else {
    pc -= 2;
  }
}

void Chip8::ld_dt_vx(uint8_t vx) {
  delayTimer = registers[CONVERT_8TO4BITS(vx)];
}

void Chip8::ld_st_vx(uint8_t vx) {
  soundTimer = registers[CONVERT_8TO4BITS(vx)];
}

void Chip8::add_i_vx(uint8_t vx) { index += registers[CONVERT_8TO4BITS(vx)]; }

void Chip8::ld_f_vx(uint8_t vx) {
  uint8_t digit = registers[CONVERT_8TO4BITS(vx)];
  index = FONTSET_START_ADDRESS + (5 * digit);
}

void Chip8::ld_b_vx(uint8_t vx) {
  uint8_t value = registers[CONVERT_8TO4BITS(vx)];

  memory[index + 2] = value % 10;
  value /= 10;

  memory[index + 1] = value % 10;
  value /= 10;

  memory[index] = value % 10;
}

void Chip8::ld_i_vx(uint8_t vx) {
  for (uint8_t i = 0; i <= (CONVERT_8TO4BITS(vx)); ++i) {
    memory[index + i] = registers[i];
  }
}

void Chip8::ld_vx_i(uint8_t vx) {
  for (uint8_t i = 0; i <= (CONVERT_8TO4BITS(vx)); ++i) {
    registers[i] = memory[index + i];
  }
}