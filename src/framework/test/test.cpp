#include <gtest/gtest.h>

#include <iostream>
#include <vector>

#include "src/framework/board.hpp"

TEST(BOARD, BoardEmpty) {
  dwc::Board b;
  for (char file : {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'}) {
    for (char rank : {'1', '2', '3', '4', '5', '6', '7', '8'}) {
      const char pos_c[2] = {file, rank};
      EXPECT_FALSE(b.get({pos_c}).has_value());
    }
  }
}

TEST(BOARD, BoardSettings) {
  struct test {
    const char* pos_c;
    dwc::Type type;
    dwc::Side side;
  };
  std::vector<test> tests{
      {"C8", dwc::Type::KING, dwc::Side::BLACK},
      {"A2", dwc::Type::PAWN, dwc::Side::WHITE},
      {"H8", dwc::Type::ROOK, dwc::Side::WHITE},
      {"H8", dwc::Type::BISHOP, dwc::Side::BLACK},
  };

  dwc::Board b;
  for (auto t : tests) {
    b.set({t.pos_c}, dwc::Piece{t.type, t.side});
    EXPECT_TRUE(b.get({t.pos_c}).has_value());
    EXPECT_EQ(b.get({t.pos_c}).value().side, t.side);
    EXPECT_EQ(b.get({t.pos_c}).value().type, t.type);
  }
}

TEST(BOARD, Positions) {
  struct test {
    const char* pos_c;
    int file;
    int rank;
  };
  std::vector<test> tests{
      {"b3", 1, 2},
      {"H8", 7, 7},
      {"a1", 0, 0},
  };
  for (auto t : tests) {
    dwc::Pos p{t.pos_c};
    EXPECT_EQ(p.file, t.file);
    EXPECT_EQ(p.rank, t.rank);
  }
}
