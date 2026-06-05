#include "Chip8.h"
#include "memConstants.h"
#include <cstdint>
#include <gtest/gtest.h>
#include <limits>

// Test fixture class for accessing private members
class Chip8FuncTests : public ::testing::Test {

protected:
  const uint8_t REG_0 = 0;
  const uint8_t REG_1 = 1;
  const uint8_t KK_0 = 4;
  const uint8_t KK_1 = 5;
  const uint8_t MAX_8_BIT = std::numeric_limits<uint8_t>::max();

  Chip8 chip8;
  uint8_t &reg(int i) { return chip8.registers[i]; }
  uint8_t &mem(int i) { return chip8.memory[i]; }
  uint16_t &pc() { return chip8.pc; }
  std::stack<uint16_t> &stack() { return chip8.stack; }
};

TEST_F(Chip8FuncTests, testConverionMacros) {
  EXPECT_EQ(CONVERT_8TO4BITS(MAX_8_BIT), 15);
}
TEST_F(Chip8FuncTests, testCallJumpRet) {
  uint16_t subroutineAddr = ROM_START_ADDRESS;
  uint16_t jumpAddr = ROM_START_ADDRESS + 4;

  chip8.jump(jumpAddr);
  EXPECT_EQ(pc(), jumpAddr);

  chip8.call(subroutineAddr);
  EXPECT_EQ(stack().top(), jumpAddr);
  EXPECT_EQ(pc(), subroutineAddr);
}

TEST_F(Chip8FuncTests, testSkipInstructions) {
  EXPECT_EQ(pc(), ROM_START_ADDRESS);
  chip8.ld_const(REG_0, KK_0);
  EXPECT_EQ(reg(REG_0), KK_0);

  chip8.se_const(REG_0, KK_0);
  EXPECT_EQ(ROM_START_ADDRESS + 2, pc());

  chip8.sne_const(REG_0, KK_0);
  EXPECT_EQ(ROM_START_ADDRESS + 2, pc());

  chip8.ld_const(REG_1, KK_1);

  chip8.se_reg(REG_0, REG_1);
  EXPECT_EQ(ROM_START_ADDRESS + 2, pc());

  chip8.sne_reg(REG_0, REG_1);
  EXPECT_EQ(ROM_START_ADDRESS + 4, pc());
}

TEST_F(Chip8FuncTests, testLoads) {

  // load maximum bit should just go to last thing
  chip8.ld_const(MAX_8_BIT, KK_0);
  EXPECT_EQ(reg(NUM_REGISTERS - 1), KK_0);

  chip8.ld_reg(REG_0, NUM_REGISTERS - 1);
  EXPECT_EQ(reg(NUM_REGISTERS - 1), KK_0);

  chip8.ld_reg(REG_1, REG_0);
  EXPECT_EQ(reg(REG_0), reg(REG_0));
}

TEST_F(Chip8FuncTests, testBoolOps) {
  chip8.ld_const(REG_1, KK_0);
  chip8.add_const(REG_1, KK_1);

  EXPECT_EQ(KK_0 + KK_1, reg(REG_1));

  chip8.ld_const(REG_0, KK_0);
  chip8.ld_const(REG_1, KK_1);
  chip8.or_reg(REG_0, REG_1);

  EXPECT_EQ(reg(REG_0), KK_0 | KK_1);

  chip8.ld_const(REG_0, KK_0);
  chip8.ld_const(REG_1, KK_1);
  chip8.and_reg(REG_0, REG_1);

  EXPECT_EQ(reg(REG_0), KK_0 & KK_1);

  chip8.ld_const(REG_0, KK_0);
  chip8.ld_const(REG_1, KK_1);
  chip8.xor_reg(REG_0, REG_1);

  EXPECT_EQ(reg(REG_0), KK_0 ^ KK_1);
}

TEST_F(Chip8FuncTests, testArithemeticOps) {
  chip8.ld_const(REG_0, KK_0);
  chip8.ld_const(REG_1, KK_1);
  chip8.add_reg(REG_0, REG_1);

  EXPECT_EQ(reg(REG_0), KK_0 + KK_1);

  chip8.ld_const(REG_0, KK_0);
  chip8.ld_const(REG_1, KK_1);
  chip8.sub_reg(REG_0, REG_1);

  EXPECT_EQ(reg(REG_0), static_cast<uint8_t>(KK_0 - KK_1));
  EXPECT_EQ(reg(CARRY_REG), 0);
}
