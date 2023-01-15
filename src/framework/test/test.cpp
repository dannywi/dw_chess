#include <gtest/gtest.h>

#include <iostream>
#include <map>
#include <vector>

#include "src/framework/board.hpp"
#include "src/framework/display.hpp"  // delete
#include "src/framework/fen_lib.hpp"
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

TEST(PieceMapping, PieceChar) {
  std::map<dwc::Piece, char> m = dwc::getPieceCharMap();
  EXPECT_EQ((m[{dwc::Type::BISHOP, dwc::Side::BLACK}]), 'b');
  EXPECT_EQ((m[{dwc::Type::QUEEN, dwc::Side::WHITE}]), 'Q');
}

TEST(PieceMapping, CharPiece) {
  std::map<char, dwc::Piece> m = dwc::getCharPieceMap();
  EXPECT_EQ(m['p'], (dwc::Piece{dwc::Type::PAWN, dwc::Side::BLACK}));
  EXPECT_EQ(m['N'], (dwc::Piece{dwc::Type::KNIGHT, dwc::Side::WHITE}));
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
  dwc::BoardT board;
  const char* pos_c = "c7";
  EXPECT_FALSE(dwc::board_ut::get({pos_c}, board).has_value());
  dwc::Piece p{dwc::Type::BISHOP, dwc::Side::WHITE};
  dwc::board_ut::set({pos_c}, p, board);
  EXPECT_TRUE(dwc::board_ut::get({pos_c}, board).has_value());
  EXPECT_EQ(dwc::board_ut::get({pos_c}, board).value(), p);
  dwc::board_ut::clear({pos_c}, board);
  EXPECT_FALSE(dwc::board_ut::get({pos_c}, board).has_value());
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

  dwc::BoardT board;
  for (auto t : tests) {
    dwc::board_ut::set({t.pos_c}, {t.type, t.side}, board);
    EXPECT_TRUE(dwc::board_ut::get({t.pos_c}, board).has_value());
    EXPECT_EQ(dwc::board_ut::get({t.pos_c}, board).value().side, t.side);
    EXPECT_EQ(dwc::board_ut::get({t.pos_c}, board).value().type, t.type);
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

namespace test {
void check_init_pos(const dwc::Board& b) {
  auto mk_pos = [](char file, char rank) { return dwc::Pos(std::string{file, rank}); };

  for (auto [rank, side] : std::vector<std::tuple<char, dwc::Side>>{{'1', dwc::Side::WHITE}, {'8', dwc::Side::BLACK}}) {
    EXPECT_EQ(b.get(mk_pos('A', rank)).value(), (dwc::Piece{dwc::Type::ROOK, side}));
    EXPECT_EQ(b.get(mk_pos('B', rank)).value(), (dwc::Piece{dwc::Type::KNIGHT, side}));
    EXPECT_EQ(b.get(mk_pos('C', rank)).value(), (dwc::Piece{dwc::Type::BISHOP, side}));
    EXPECT_EQ(b.get(mk_pos('D', rank)).value(), (dwc::Piece{dwc::Type::QUEEN, side}));
    EXPECT_EQ(b.get(mk_pos('E', rank)).value(), (dwc::Piece{dwc::Type::KING, side}));
    EXPECT_EQ(b.get(mk_pos('F', rank)).value(), (dwc::Piece{dwc::Type::BISHOP, side}));
    EXPECT_EQ(b.get(mk_pos('G', rank)).value(), (dwc::Piece{dwc::Type::KNIGHT, side}));
    EXPECT_EQ(b.get(mk_pos('H', rank)).value(), (dwc::Piece{dwc::Type::ROOK, side}));
  }

  for (char file : {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'}) {
    for (char rank : {'3', '4', '5', '6'}) {
      std::string pos_s{file, rank};
      EXPECT_FALSE(b.get({pos_s}).has_value());
    }
    EXPECT_EQ(b.get(mk_pos(file, '2')).value(), (dwc::Piece{dwc::Type::PAWN, dwc::Side::WHITE}));
    EXPECT_EQ(b.get(mk_pos(file, '7')).value(), (dwc::Piece{dwc::Type::PAWN, dwc::Side::BLACK}));
  }
}
}  // namespace test

TEST(BOARD, BoardReset) {
  dwc::Board b;
  b.reset_position();
  test::check_init_pos(b);
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

  // dwc::display(b);
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

  // pawn special move
  auto moves = get_moves(b, {"h2"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"h2"}, {"h3"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"h2"}, {"h4"}}));

  moves = get_moves(b, {"b7"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"b7"}, {"b6"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"b7"}, {"b5"}}));

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

  // dwc::display(b);

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

  // white bishop can move
  moves = get_moves(b, {"f1"});
  EXPECT_EQ(moves.size(), 5);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"e2"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"d3"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"c4"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"b5"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"a6"}}));
}

TEST(BOARD, LegalMoves03) {
  using dwc::legal_move::get_moves;

  dwc::Board b;
  b.reset_position();
  b.move({{"e2"}, {"e4"}});
  b.move({{"d7"}, {"d5"}});
  // dwc::display(b);

  auto moves = get_moves(b, {"e4"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"e4"}, {"e5"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"e4"}, {"d5"}}));

  b.move({{"c2"}, {"c4"}});
  // dwc::display(b);

  moves = get_moves(b, {"d5"});
  EXPECT_EQ(moves.size(), 3);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d5"}, {"e4"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d5"}, {"d4"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d5"}, {"c4"}}));

  b.move({{"c7"}, {"c5"}});
  // dwc::display(b);

  moves = get_moves(b, {"c4"});
  EXPECT_EQ(moves.size(), 1);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"c4"}, {"d5"}}));
}

TEST(BOARD, LegalMoves04) {
  using dwc::legal_move::get_moves;

  dwc::Board b;
  b.reset_position();

  EXPECT_TRUE(dwc::legal_move::is_legal_move(b, {"h2"}, {{"h2"}, {"h3"}}));
  EXPECT_TRUE(dwc::legal_move::is_legal_move(b, {"h2"}, {{"h2"}, {"h4"}}));
  EXPECT_FALSE(dwc::legal_move::is_legal_move(b, {"h2"}, {{"h2"}, {"h5"}}));

  EXPECT_TRUE(dwc::legal_move::is_legal_move(b, {"b1"}, {{"b1"}, {"a3"}}));
  EXPECT_TRUE(dwc::legal_move::is_legal_move(b, {"b1"}, {{"b1"}, {"c3"}}));
  EXPECT_FALSE(dwc::legal_move::is_legal_move(b, {"b1"}, {{"b1"}, {"d3"}}));
}

TEST(BOARD, EnPassant01) {
  using dwc::legal_move::get_moves;
  dwc::Board b{"rnbqkbnr/pp1ppppp/2p5/4P3/8/8/PPPP1PPP/RNBQKBNR b"};
  b.move({{"d7"}, {"d5"}});
  dwc::display(b);

  // en passant available
  // auto moves = get_moves(b, {"e5"});
  // EXPECT_EQ(moves.size(), 2);
  // EXPECT_TRUE(find_me(moves, dwc::Move{{"e5"}, {"e6"}}));
  // EXPECT_TRUE(find_me(moves, dwc::Move{{"e5"}, {"d6"}}));  // en passant
}

TEST(BOARD, FenBoardParserException01) {
  EXPECT_THROW(dwc::Board{"rnbqkbnrr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"}, std::runtime_error);
  EXPECT_THROW(dwc::Board{"rnbqkbnrr/pppppppp/9/8/8/8/PPPPPPPP/RNBQKBNR"}, std::runtime_error);
  EXPECT_THROW(dwc::Board{"rnbqkbnrr/pppppppp/"}, std::runtime_error);
  EXPECT_THROW(dwc::Board{"rnbqkbnrr/pppppppp/4p4/8/8/8/PPPPPPPP/RNBQKBNR"}, std::runtime_error);
  EXPECT_NO_THROW(dwc::Board{"8/8/8/4p3/2K2Q2/8/8/8"});
}

TEST(BOARD, FenBoardParserException02) {
  EXPECT_THROW(dwc::Board{"8/8/8/4p3/2K2Q2/8/8/8 w 123"}, std::runtime_error);  // more than available parser
}

TEST(BOARD, FenBoardParserException03) {
  dwc::Board b{"rnbqkbnr/pp1ppppp/2p5/4P3/8/8/PPPP1PPP/RNBQKBNR w"};
  EXPECT_THROW(b.move({{"d7"}, {"d5"}}), std::logic_error);  // wrong side move
}

TEST(BOARD, FenBoardParser01) {
  dwc::Board b{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"};
  // dwc::display(b);
  test::check_init_pos(b);
}

TEST(BOARD, FenBoardParser02) {
  EXPECT_EQ(dwc::fen::_inner::parse_side("w").value(), dwc::Side::WHITE);
  EXPECT_EQ(dwc::fen::_inner::parse_side("b").value(), dwc::Side::BLACK);
  EXPECT_THROW(dwc::fen::_inner::parse_side("bw").value(), std::runtime_error);
  EXPECT_THROW(dwc::fen::_inner::parse_side("a").value(), std::runtime_error);
}

// TEST(BOARD, TempDisplay) {
//   dwc::Board b;
//   b.reset_position();
//   dwc::display(b);
//   EXPECT_TRUE(1);
// }
