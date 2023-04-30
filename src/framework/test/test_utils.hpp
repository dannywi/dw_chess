#pragma once

#include <gtest/gtest.h>

#include "src/framework/board.hpp"

namespace test {
void check_init_pos(const dwc::Board& b) {
  using namespace dwc;
  auto mk_pos = [](char file, char rank) { return Pos(std::string{file, rank}); };

  for (auto [rank, side] : std::vector<std::tuple<char, Side>>{{'1', Side::WHITE}, {'8', Side::BLACK}}) {
    EXPECT_EQ(b.get(mk_pos('A', rank)).value(), (Piece{Type::ROOK, side}));
    EXPECT_EQ(b.get(mk_pos('B', rank)).value(), (Piece{Type::KNIGHT, side}));
    EXPECT_EQ(b.get(mk_pos('C', rank)).value(), (Piece{Type::BISHOP, side}));
    EXPECT_EQ(b.get(mk_pos('D', rank)).value(), (Piece{Type::QUEEN, side}));
    EXPECT_EQ(b.get(mk_pos('E', rank)).value(), (Piece{Type::KING, side}));
    EXPECT_EQ(b.get(mk_pos('F', rank)).value(), (Piece{Type::BISHOP, side}));
    EXPECT_EQ(b.get(mk_pos('G', rank)).value(), (Piece{Type::KNIGHT, side}));
    EXPECT_EQ(b.get(mk_pos('H', rank)).value(), (Piece{Type::ROOK, side}));
  }

  for (char file : {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'}) {
    for (char rank : {'3', '4', '5', '6'}) {
      std::string pos_s{file, rank};
      EXPECT_FALSE(b.get({pos_s}).has_value());
    }
    EXPECT_EQ(b.get(mk_pos(file, '2')).value(), (Piece{Type::PAWN, Side::WHITE}));
    EXPECT_EQ(b.get(mk_pos(file, '7')).value(), (Piece{Type::PAWN, Side::BLACK}));
  }
}
}  // namespace test
