#include <gtest/gtest.h>

#include <iostream>
#include <map>
#include <vector>

#include "src/framework/board.hpp"
#include "src/framework/display.hpp"  // delete
#include "src/framework/legal_move.hpp"

TEST(BASIC, PieceMappable) {
  std::vector<dwc::Piece> pieces{
      {dwc::Type::PAWN, dwc::Side::WHITE},   {dwc::Type::PAWN, dwc::Side::BLACK},
      {dwc::Type::KNIGHT, dwc::Side::WHITE}, {dwc::Type::KNIGHT, dwc::Side::BLACK},
      {dwc::Type::BISHOP, dwc::Side::WHITE}, {dwc::Type::BISHOP, dwc::Side::BLACK},
      {dwc::Type::ROOK, dwc::Side::WHITE},   {dwc::Type::ROOK, dwc::Side::BLACK},
      {dwc::Type::QUEEN, dwc::Side::WHITE},  {dwc::Type::QUEEN, dwc::Side::BLACK},
      {dwc::Type::KING, dwc::Side::WHITE},   {dwc::Type::KING, dwc::Side::BLACK},
  };

  std::map<dwc::Piece, int> map;
  auto conv_i = [](auto i) { return i * 3; };
  for (size_t i = 0; i < pieces.size(); ++i) { map[pieces[i]] = conv_i(i); }

  for (size_t i = 0; i < pieces.size(); ++i) { EXPECT_EQ(map[pieces[i]], conv_i(i)); }
}

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
  for (const char* pos_c : {"I1", "a0", "b11", "C9"}) { EXPECT_THROW(dwc::Pos{pos_c}, std::logic_error); }
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
  b.move({{"e2"}, {"e4"}});
  EXPECT_FALSE(b.get({"e2"}).has_value());
  EXPECT_EQ(b.get({"e4"}).value(), (dwc::Piece{dwc::Type::PAWN, dwc::Side::WHITE}));

  // Caro Kann defense
  b.move({{"c7"}, {"c6"}});
  EXPECT_FALSE(b.get({"c7"}).has_value());
  EXPECT_EQ(b.get({"c6"}).value(), (dwc::Piece{dwc::Type::PAWN, dwc::Side::BLACK}));

  EXPECT_THROW(b.move({{"h5"}, {"e2"}}), std::logic_error);
  EXPECT_THROW(b.move({{"d7"}, {"d5"}}), std::logic_error);

  dwc::display(b);
}

template <typename T, typename CONT>
bool find_me(CONT cont, T v) {
  for (auto c : cont) {
    if (c == v) return true;
  }
  return false;
}

TEST(BOARD, LegalMoves01) {
  using dwc::legal_move::get_moves;

  dwc::Board b;
  b.reset_position();

  // todo: correct this with inital pawn move
  auto moves = get_moves(b, {"h2"});
  EXPECT_EQ(moves.size(), 1);

  // most other pieces can't move
  EXPECT_EQ(get_moves(b, {"e1"}).size(), 0);
  EXPECT_EQ(get_moves(b, {"d8"}).size(), 0);

  // knights can move
  moves = get_moves(b, {"b1"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"b1"}, {"a3"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"b1"}, {"c3"}}));

  // todo: after legal move checks turns, move a white piece to check this
  moves = get_moves(b, {"g8"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"g8"}, {"h6"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"g8"}, {"f6"}}));
}

TEST(BOARD, LegalMoves02) {
  using dwc::legal_move::get_moves;

  dwc::Board b;
  b.reset_position();
  b.move({{"e2"}, {"e4"}});
  b.move({{"e7"}, {"e6"}});

  dwc::display(b);

  // white king can move
  auto moves = get_moves(b, {"e1"});
  EXPECT_EQ(moves.size(), 1);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"e1"}, {"e2"}}));

  // white queen can move
  moves = get_moves(b, {"d1"});
  EXPECT_EQ(moves.size(), 4);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d1"}, {"e2"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d1"}, {"f3"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d1"}, {"g4"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d1"}, {"h5"}}));

  std::cout << "====== BISHIP ====== " << std::endl;
  // white bishop can move
  moves = get_moves(b, {"f1"});
  EXPECT_EQ(moves.size(), 5);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"e2"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"d3"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"c4"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"b5"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"a6"}}));
}

// TEST(BOARD, TempDisplay) {
//   dwc::Board b;
//   b.reset_position();
//   dwc::display(b);
//   EXPECT_TRUE(1);
// }
