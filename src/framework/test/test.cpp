#include <gtest/gtest.h>

#include <iostream>
#include <vector>

#include "src/framework/board.hpp"
#include "src/framework/display.hpp"  // delete

TEST(BOARD, BoardEmpty) {
  dwc::Board b;
  for (char file : {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'}) {
    for (char rank : {'1', '2', '3', '4', '5', '6', '7', '8'}) {
      std::string pos_s{file, rank};
      EXPECT_FALSE(b.get({pos_s}).has_value());
    }
  }
}

TEST(BOARD, BoardClear) {
  dwc::Board b;
  const char* pos_c = "c7";
  EXPECT_FALSE(b.get({pos_c}).has_value());
  dwc::Piece p{dwc::Type::BISHOP, dwc::Side::WHITE};
  b.set({pos_c}, p);
  EXPECT_TRUE(b.get({pos_c}).has_value());
  EXPECT_EQ(b.get({pos_c}).value(), p);
  b.clear({pos_c});
  EXPECT_FALSE(b.get({pos_c}).has_value());
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
    b.set({t.pos_c}, {t.type, t.side});
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

TEST(BOARD, BadPositions) {
  for (const char* pos_c : {"I1", "a0", "b11", "C9"}) { EXPECT_THROW(dwc::Pos{pos_c}, std::range_error); }
}

TEST(BOARD, BoardReset) {
  dwc::Board b;
  b.reset_position();
  EXPECT_EQ(b.get({"B2"}).value(), (dwc::Piece{dwc::Type::PAWN, dwc::Side::WHITE}));
  EXPECT_EQ(b.get({"E1"}).value(), (dwc::Piece{dwc::Type::KING, dwc::Side::WHITE}));
  EXPECT_EQ(b.get({"H8"}).value(), (dwc::Piece{dwc::Type::ROOK, dwc::Side::BLACK}));
  EXPECT_EQ(b.get({"B8"}).value(), (dwc::Piece{dwc::Type::KNIGHT, dwc::Side::BLACK}));

  for (char file : {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'}) {
    for (char rank : {'3', '4', '5', '6'}) {
      std::string pos_s{file, rank};
      EXPECT_FALSE(b.get({pos_s}).has_value());
    }
  }
}

TEST(BOARD, BoardMove) {
  dwc::Board b;
  b.reset_position();

  // King's Pawn opening
  b.move({"e2"}, {"e4"});
  EXPECT_FALSE(b.get({"e2"}).has_value());
  EXPECT_EQ(b.get({"e4"}).value(), (dwc::Piece{dwc::Type::PAWN, dwc::Side::WHITE}));

  // Caro Kann defense
  b.move({"c7"}, {"c6"});
  EXPECT_FALSE(b.get({"c7"}).has_value());
  EXPECT_EQ(b.get({"c6"}).value(), (dwc::Piece{dwc::Type::PAWN, dwc::Side::BLACK}));

  EXPECT_THROW(b.move({"h5"}, {"e2"}), std::range_error);
  dwc::display(b);
}

TEST(BOARD, TempDisplay) {
  dwc::Board b;
  b.reset_position();
  dwc::display(b);
  EXPECT_TRUE(1);
}
