#include <gtest/gtest.h>

#include <iostream>
#include <map>
#include <vector>

#include "src/framework/display.hpp"  // delete
#include "src/framework/fen_lib.hpp"
#include "src/framework/legal_move.hpp"
#include "test_utils.hpp"

using namespace dwc;

TEST(BASIC, PieceMappable) {
  std::vector<Piece> pieces{
      {Type::PAWN, Side::WHITE},   {Type::PAWN, Side::BLACK},   {Type::KNIGHT, Side::WHITE},
      {Type::KNIGHT, Side::BLACK}, {Type::BISHOP, Side::WHITE}, {Type::BISHOP, Side::BLACK},
      {Type::ROOK, Side::WHITE},   {Type::ROOK, Side::BLACK},   {Type::QUEEN, Side::WHITE},
      {Type::QUEEN, Side::BLACK},  {Type::KING, Side::WHITE},   {Type::KING, Side::BLACK},
  };

  std::map<Piece, int> map;
  auto conv_i = [](auto i) { return i * 3; };
  for (size_t i = 0; i < pieces.size(); ++i) { map[pieces[i]] = conv_i(i); }

  for (size_t i = 0; i < pieces.size(); ++i) { EXPECT_EQ(map[pieces[i]], conv_i(i)); }
}

TEST(PieceMapping, PieceChar) {
  std::map<Piece, char> m = getPieceCharMap();
  EXPECT_EQ((m[{Type::BISHOP, Side::BLACK}]), 'b');
  EXPECT_EQ((m[{Type::QUEEN, Side::WHITE}]), 'Q');
}

TEST(PieceMapping, CharPiece) {
  std::map<char, Piece> m = getCharPieceMap();
  EXPECT_EQ(m['p'], (Piece{Type::PAWN, Side::BLACK}));
  EXPECT_EQ(m['N'], (Piece{Type::KNIGHT, Side::WHITE}));
}

TEST(BOARD, BoardEmpty) {
  Board b;
  for (char file : {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'}) {
    for (char rank : {'1', '2', '3', '4', '5', '6', '7', '8'}) {
      std::string pos_s{file, rank};
      EXPECT_FALSE(b.get({pos_s}).has_value());
    }
  }
}

TEST(BOARD, BoardClear) {
  BoardT board;
  const char* pos_c = "c7";
  EXPECT_FALSE(board_ut::get({pos_c}, board).has_value());
  Piece p{Type::BISHOP, Side::WHITE};
  board_ut::set({pos_c}, p, board);
  EXPECT_TRUE(board_ut::get({pos_c}, board).has_value());
  EXPECT_EQ(board_ut::get({pos_c}, board).value(), p);
  board_ut::clear({pos_c}, board);
  EXPECT_FALSE(board_ut::get({pos_c}, board).has_value());
}

TEST(BOARD, BoardSettings) {
  struct test {
    const char* pos_c;
    Type type;
    Side side;
  };
  std::vector<test> tests{
      {"C8", Type::KING, Side::BLACK},
      {"A2", Type::PAWN, Side::WHITE},
      {"H8", Type::ROOK, Side::WHITE},
      {"H8", Type::BISHOP, Side::BLACK},
  };

  BoardT board;
  for (auto t : tests) {
    board_ut::set({t.pos_c}, {t.type, t.side}, board);
    EXPECT_TRUE(board_ut::get({t.pos_c}, board).has_value());
    EXPECT_EQ(board_ut::get({t.pos_c}, board).value().side, t.side);
    EXPECT_EQ(board_ut::get({t.pos_c}, board).value().type, t.type);
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
    Pos p{t.pos_c};
    EXPECT_EQ(p.file, t.file);
    EXPECT_EQ(p.rank, t.rank);
  }
}

TEST(BOARD, BadPositions) {
  for (const char* pos_c : {"I1", "a0", "b11", "C9"}) { EXPECT_THROW(Pos{pos_c}, std::logic_error); }
}

TEST(BOARD, BoardReset) {
  Board b;
  b.reset_position();
  test::check_init_pos(b);
}

TEST(BOARD, BoardMove) {
  Board b;
  b.reset_position();

  // King's Pawn opening
  b.move({{"e2"}, {"e4"}});
  EXPECT_FALSE(b.get({"e2"}).has_value());
  EXPECT_EQ(b.get({"e4"}).value(), (Piece{Type::PAWN, Side::WHITE}));

  // Caro Kann defense
  b.move({{"c7"}, {"c6"}});
  EXPECT_FALSE(b.get({"c7"}).has_value());
  EXPECT_EQ(b.get({"c6"}).value(), (Piece{Type::PAWN, Side::BLACK}));

  EXPECT_THROW(b.move({{"h5"}, {"e2"}}), std::logic_error);
  EXPECT_THROW(b.move({{"d7"}, {"d5"}}), std::logic_error);
}

template <typename T, typename CONT>
bool find_me(CONT cont, T v) {
  for (auto c : cont) {
    if (c == v) return true;
  }
  return false;
}

TEST(BOARD, LegalMoves01) {
  Board b;
  b.reset_position();

  // pawn special move
  auto moves = b.get_moves({"h2"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, Move{{"h2"}, {"h3"}}));
  EXPECT_TRUE(find_me(moves, Move{{"h2"}, {"h4"}}));

  moves = b.get_moves({"b7"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, Move{{"b7"}, {"b6"}}));
  EXPECT_TRUE(find_me(moves, Move{{"b7"}, {"b5"}}));

  // most other pieces can't move
  EXPECT_EQ(b.get_moves({"e1"}).size(), 0);
  EXPECT_EQ(b.get_moves({"d8"}).size(), 0);

  // knights can move
  moves = b.get_moves({"b1"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, Move{{"b1"}, {"a3"}}));
  EXPECT_TRUE(find_me(moves, Move{{"b1"}, {"c3"}}));

  // todo: after legal move checks turns, move a white piece to check this
  moves = b.get_moves({"g8"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, Move{{"g8"}, {"h6"}}));
  EXPECT_TRUE(find_me(moves, Move{{"g8"}, {"f6"}}));
}

TEST(BOARD, LegalMoves02) {
  Board b;
  b.reset_position();
  b.move({{"e2"}, {"e4"}});
  b.move({{"e7"}, {"e6"}});

  // white king can move
  auto moves = b.get_moves({"e1"});
  EXPECT_EQ(moves.size(), 1);
  EXPECT_TRUE(find_me(moves, Move{{"e1"}, {"e2"}}));

  // white queen can move
  moves = b.get_moves({"d1"});
  EXPECT_EQ(moves.size(), 4);
  EXPECT_TRUE(find_me(moves, Move{{"d1"}, {"e2"}}));
  EXPECT_TRUE(find_me(moves, Move{{"d1"}, {"f3"}}));
  EXPECT_TRUE(find_me(moves, Move{{"d1"}, {"g4"}}));
  EXPECT_TRUE(find_me(moves, Move{{"d1"}, {"h5"}}));

  // white bishop can move
  moves = b.get_moves({"f1"});
  EXPECT_EQ(moves.size(), 5);
  EXPECT_TRUE(find_me(moves, Move{{"f1"}, {"e2"}}));
  EXPECT_TRUE(find_me(moves, Move{{"f1"}, {"d3"}}));
  EXPECT_TRUE(find_me(moves, Move{{"f1"}, {"c4"}}));
  EXPECT_TRUE(find_me(moves, Move{{"f1"}, {"b5"}}));
  EXPECT_TRUE(find_me(moves, Move{{"f1"}, {"a6"}}));
}

TEST(BOARD, LegalMoves03) {
  Board b;
  b.reset_position();
  b.move({{"e2"}, {"e4"}});
  EXPECT_FALSE(b.is_threatened({"e4"}));
  b.move({{"d7"}, {"d5"}});

  auto moves = b.get_moves({"e4"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, Move{{"e4"}, {"e5"}}));
  EXPECT_TRUE(find_me(moves, Move{{"e4"}, {"d5"}}));
  EXPECT_TRUE(b.is_threatened({"e4"}));
  EXPECT_TRUE(b.is_threatened({"d5"}));

  b.move({{"c2"}, {"c4"}});

  moves = b.get_moves({"d5"});
  EXPECT_EQ(moves.size(), 3);
  EXPECT_TRUE(find_me(moves, Move{{"d5"}, {"e4"}}));
  EXPECT_TRUE(find_me(moves, Move{{"d5"}, {"d4"}}));
  EXPECT_TRUE(find_me(moves, Move{{"d5"}, {"c4"}}));
  EXPECT_TRUE(b.is_threatened({"e4"}));
  EXPECT_TRUE(b.is_threatened({"d5"}));
  EXPECT_TRUE(b.is_threatened({"c4"}));

  b.move({{"c7"}, {"c5"}});

  moves = b.get_moves({"c4"});
  EXPECT_EQ(moves.size(), 1);
  EXPECT_TRUE(find_me(moves, Move{{"c4"}, {"d5"}}));
  EXPECT_FALSE(b.is_threatened({"c5"}));

  // other checks
  EXPECT_FALSE(b.is_threatened({"a2"}));
  EXPECT_FALSE(b.is_threatened({"b8"}));
  EXPECT_FALSE(b.is_threatened({"h5"}));
}

TEST(BOARD, LegalMoves04) {
  Board b;
  b.reset_position();

  EXPECT_TRUE(legal_move::is_legal_move(b, {"h2"}, {{"h2"}, {"h3"}}));
  EXPECT_TRUE(legal_move::is_legal_move(b, {"h2"}, {{"h2"}, {"h4"}}));
  EXPECT_FALSE(legal_move::is_legal_move(b, {"h2"}, {{"h2"}, {"h5"}}));

  EXPECT_TRUE(legal_move::is_legal_move(b, {"b1"}, {{"b1"}, {"a3"}}));
  EXPECT_TRUE(legal_move::is_legal_move(b, {"b1"}, {{"b1"}, {"c3"}}));
  EXPECT_FALSE(legal_move::is_legal_move(b, {"b1"}, {{"b1"}, {"d3"}}));
}

TEST(BOARD, IsThreatened01) {
  Board b;
  b.reset_position();
  b.move({{"e2"}, {"e3"}});
  EXPECT_FALSE(b.is_threatened({"e3"}));
  EXPECT_FALSE(b.is_king_threatened(Side::WHITE));
  EXPECT_FALSE(b.is_king_threatened(Side::BLACK));
}

TEST(BOARD, IsThreatened02) {
  Board b{"8/4k3/8/8/2K5/8/8/8 w"};
  EXPECT_FALSE(b.is_king_threatened(Side::WHITE));
  EXPECT_FALSE(b.is_king_threatened(Side::BLACK));
}

TEST(BOARD, IsThreatened03) {
  Board b{"8/4k3/8/8/2K1R3/8/8/8 w"};
  EXPECT_FALSE(b.is_king_threatened(Side::WHITE));
  EXPECT_TRUE(b.is_king_threatened(Side::BLACK));
}

TEST(BOARD, IsThreatened04) {
  Board b{"8/4k3/8/Q7/2K1r3/P7/2B5/8 w"};
  EXPECT_TRUE(b.is_king_threatened(Side::WHITE));
  EXPECT_FALSE(b.is_king_threatened(Side::BLACK));

  // bishop can move to take the threatening rook
  auto moves = b.get_moves({"c2"});
  EXPECT_EQ(moves.size(), 1);
  EXPECT_TRUE(find_me(moves, Move{{"c2"}, {"e4"}}));

  // other pieces can't move
  EXPECT_TRUE(b.get_moves({"a3"}).empty());
  EXPECT_TRUE(b.get_moves({"a5"}).empty());
}

TEST(BOARD, Pinned01) {
  Board b{"8/k7/8/8/8/4r3/4N3/4K3 w"};
  EXPECT_FALSE(b.is_king_threatened(Side::WHITE));
  EXPECT_FALSE(b.is_king_threatened(Side::BLACK));

  auto moves = b.get_moves({"e2"});
  EXPECT_TRUE(moves.empty());
}

TEST(BOARD, Pinned02) {
  Board b{"q7/k7/8/8/3P4/4r3/6B1/7K w"};
  EXPECT_FALSE(b.is_king_threatened(Side::WHITE));
  EXPECT_FALSE(b.is_king_threatened(Side::BLACK));

  // bishop can only move towards the queen
  auto moves = b.get_moves({"g2"});
  EXPECT_EQ(moves.size(), 6);
  EXPECT_TRUE(find_me(moves, Move{{"g2"}, {"f3"}}));
  EXPECT_TRUE(find_me(moves, Move{{"g2"}, {"e4"}}));
  EXPECT_TRUE(find_me(moves, Move{{"g2"}, {"d5"}}));
  EXPECT_TRUE(find_me(moves, Move{{"g2"}, {"c6"}}));
  EXPECT_TRUE(find_me(moves, Move{{"g2"}, {"b7"}}));
  EXPECT_TRUE(find_me(moves, Move{{"g2"}, {"a8"}}));

  // pawn can move
  moves = b.get_moves({"d4"});
  EXPECT_EQ(moves.size(), 1);
  EXPECT_TRUE(find_me(moves, Move{{"d4"}, {"d5"}}));
}

TEST(BOARD, Pinned03) {
  Board b{"8/k7/8/8/8/4r3/4B3/4K3 b"};
  display(b);
  EXPECT_FALSE(b.is_king_threatened(Side::WHITE));
  EXPECT_FALSE(b.is_king_threatened(Side::BLACK));

  // king cannot move to a threatened position, eventhough the threatening bishop is pinned
  auto moves = b.get_moves({"a7"});
  EXPECT_EQ(moves.size(), 4);
  EXPECT_FALSE(find_me(moves, Move{{"a7"}, {"a6"}}));
}

TEST(BOARD, Pinned04) {
  Board b{"8/8/k7/1n6/8/4r3/4B3/4K3 b"};
  EXPECT_FALSE(b.is_king_threatened(Side::WHITE));
  EXPECT_FALSE(b.is_king_threatened(Side::BLACK));

  // knight cannot move, eventhough the threatening bishop is pinned
  auto moves = b.get_moves({"b5"});
  EXPECT_TRUE(moves.empty());
}

TEST(BOARD, EnPassant01) {
  Board b{"rnbqkbnr/pp1ppppp/2p5/4P3/8/8/PPPP1PPP/RNBQKBNR b"};
  b.move({{"d7"}, {"d5"}});

  // display(b);

  // en passant available
  // auto moves = get_moves(b, {"e5"});
  // EXPECT_EQ(moves.size(), 2);
  // EXPECT_TRUE(find_me(moves, Move{{"e5"}, {"e6"}}));
  // EXPECT_TRUE(find_me(moves, Move{{"e5"}, {"d6"}}));  // en passant
}
