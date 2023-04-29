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
  dwc::Board b;
  b.reset_position();

  // pawn special move
  auto moves = b.get_moves({"h2"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"h2"}, {"h3"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"h2"}, {"h4"}}));

  moves = b.get_moves({"b7"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"b7"}, {"b6"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"b7"}, {"b5"}}));

  // most other pieces can't move
  EXPECT_EQ(b.get_moves({"e1"}).size(), 0);
  EXPECT_EQ(b.get_moves({"d8"}).size(), 0);

  // knights can move
  moves = b.get_moves({"b1"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"b1"}, {"a3"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"b1"}, {"c3"}}));

  // todo: after legal move checks turns, move a white piece to check this
  moves = b.get_moves({"g8"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"g8"}, {"h6"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"g8"}, {"f6"}}));
}

TEST(BOARD, LegalMoves02) {
  dwc::Board b;
  b.reset_position();
  b.move({{"e2"}, {"e4"}});
  b.move({{"e7"}, {"e6"}});

  // dwc::display(b);

  // white king can move
  auto moves = b.get_moves({"e1"});
  EXPECT_EQ(moves.size(), 1);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"e1"}, {"e2"}}));

  // white queen can move
  moves = b.get_moves({"d1"});
  EXPECT_EQ(moves.size(), 4);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d1"}, {"e2"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d1"}, {"f3"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d1"}, {"g4"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d1"}, {"h5"}}));

  // white bishop can move
  moves = b.get_moves({"f1"});
  EXPECT_EQ(moves.size(), 5);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"e2"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"d3"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"c4"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"b5"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"f1"}, {"a6"}}));
}

TEST(BOARD, LegalMoves03) {
  dwc::Board b;
  b.reset_position();
  b.move({{"e2"}, {"e4"}});
  b.move({{"d7"}, {"d5"}});
  // dwc::display(b);

  auto moves = b.get_moves({"e4"});
  EXPECT_EQ(moves.size(), 2);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"e4"}, {"e5"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"e4"}, {"d5"}}));

  b.move({{"c2"}, {"c4"}});
  // dwc::display(b);

  moves = b.get_moves({"d5"});
  EXPECT_EQ(moves.size(), 3);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d5"}, {"e4"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d5"}, {"d4"}}));
  EXPECT_TRUE(find_me(moves, dwc::Move{{"d5"}, {"c4"}}));

  b.move({{"c7"}, {"c5"}});
  // dwc::display(b);

  moves = b.get_moves({"c4"});
  EXPECT_EQ(moves.size(), 1);
  EXPECT_TRUE(find_me(moves, dwc::Move{{"c4"}, {"d5"}}));
}

TEST(BOARD, LegalMoves04) {
  dwc::Board b;
  b.reset_position();

  EXPECT_TRUE(dwc::legal_move::is_legal_move(b, {"h2"}, {{"h2"}, {"h3"}}));
  EXPECT_TRUE(dwc::legal_move::is_legal_move(b, {"h2"}, {{"h2"}, {"h4"}}));
  EXPECT_FALSE(dwc::legal_move::is_legal_move(b, {"h2"}, {{"h2"}, {"h5"}}));

  EXPECT_TRUE(dwc::legal_move::is_legal_move(b, {"b1"}, {{"b1"}, {"a3"}}));
  EXPECT_TRUE(dwc::legal_move::is_legal_move(b, {"b1"}, {{"b1"}, {"c3"}}));
  EXPECT_FALSE(dwc::legal_move::is_legal_move(b, {"b1"}, {{"b1"}, {"d3"}}));
}

namespace test {
template <bool EXP>
struct KingWhite {
  static bool check(const dwc::Board& b) { return dwc::legal_move::is_legal_move(b, {"e1"}, {{"e1"}, {"g1"}}) == EXP; }
};

template <bool EXP>
struct QueenWhite {
  static bool check(const dwc::Board& b) { return dwc::legal_move::is_legal_move(b, {"e1"}, {{"e1"}, {"c1"}}) == EXP; }
};

template <bool EXP>
struct KingBlack {
  static bool check(const dwc::Board& b) { return dwc::legal_move::is_legal_move(b, {"e8"}, {{"e8"}, {"g8"}}) == EXP; }
};

template <bool EXP>
struct QueenBlack {
  static bool check(const dwc::Board& b) { return dwc::legal_move::is_legal_move(b, {"e8"}, {{"e8"}, {"c8"}}) == EXP; }
};

template <typename... F>
void expect_true(const dwc::Board& b) {
  EXPECT_TRUE((F::check(b) && ...));
}

}  // namespace test

TEST(BOARD, Castling01) {
  dwc::Board b;
  b.reset_position();

  using namespace test;
  expect_true<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<false>>(b);

  b.move({{"e2"}, {"e4"}});
  b.move({{"e7"}, {"e5"}});

  // castling king side both sides not allowed yet
  expect_true<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<false>>(b);

  b.move({{"g1"}, {"f3"}});
  b.move({{"g8"}, {"f6"}});

  // castling king side both sides not allowed yet
  expect_true<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<false>>(b);

  b.move({{"f1"}, {"c4"}});
  b.move({{"f8"}, {"c5"}});

  // can castle king side now, both sides
  expect_true<KingWhite<true>, QueenWhite<false>, KingBlack<true>, QueenBlack<false>>(b);

  // castling queen side both sides

  // move both sides, confirm position after move is correct

  // repeat, but move king or rook before, and confirm castling not allowed anymore
  // repeat, castle, then move until king is back into original position, confirm castling is not allowed
  // continue with moving rook back, confirm castling is not allowed

  // dwc::display(b);
}

TEST(BOARD, Castling02) {
  using namespace test;
  {
    dwc::Board b{"r3kbnr/pppq1ppp/2np4/4pbB1/8/2NP4/PPPQPPPP/R3KBNR w KQkq"};
    // dwc::display(b);
    expect_true<KingWhite<false>, QueenWhite<true>, KingBlack<false>, QueenBlack<true>>(b);
  }

  {
    // almost same setting, but rook not in place
    dwc::Board b{"p3kbnr/pppq1ppp/2np4/4pbB1/8/2NP4/PPPQPPPP/4KBNR w KQkq"};
    // dwc::display(b);
    expect_true<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<false>>(b);
  }
}

TEST(BOARD, Castling03) {
  using namespace test;
  {  // move king
    dwc::Board b{"r3kbnr/pppq1ppp/2np4/4pbB1/8/2NP4/PPPQPPPP/R3KBNR w KQkq"};
    expect_true<KingWhite<false>, QueenWhite<true>, KingBlack<false>, QueenBlack<true>>(b);

    // move the kings
    b.move({{"e1"}, {"d1"}});
    // EXPECT_FALSE(test::is_legal_queen_side_white(b));
    expect_true<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<true>>(b);
    b.move({{"e8"}, {"e7"}});
    expect_true<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<false>>(b);

    // move them back, but should still not be able to castle
    b.move({{"d1"}, {"e1"}});
    expect_true<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<false>>(b);
    b.move({{"e7"}, {"e8"}});
    expect_true<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<false>>(b);
  }

  {  // move queen rook
    dwc::Board b{"r3k2r/pppq1ppp/2np4/4pbB1/8/2NP4/PPPQPPPP/R3K2R w KQkq"};
    expect_true<KingWhite<true>, QueenWhite<true>, KingBlack<true>, QueenBlack<true>>(b);

    // move queen rook
    b.move({{"a1"}, {"d1"}});
    expect_true<KingWhite<true>, QueenWhite<false>, KingBlack<true>, QueenBlack<true>>(b);
    b.move({{"a8"}, {"c8"}});
    expect_true<KingWhite<true>, QueenWhite<false>, KingBlack<true>, QueenBlack<false>>(b);

    // move them back, but should still not be able to castle
    b.move({{"d1"}, {"a1"}});
    expect_true<KingWhite<true>, QueenWhite<false>, KingBlack<true>, QueenBlack<false>>(b);
    b.move({{"c8"}, {"a8"}});
    expect_true<KingWhite<true>, QueenWhite<false>, KingBlack<true>, QueenBlack<false>>(b);
  }

  {  // move king rook
    dwc::Board b{"r3k2r/pppq1ppp/2np4/4pbB1/8/2NP4/PPPQPPPP/R3K2R w KQkq"};
    expect_true<KingWhite<true>, QueenWhite<true>, KingBlack<true>, QueenBlack<true>>(b);

    // move king rook
    b.move({{"h1"}, {"g1"}});
    expect_true<KingWhite<false>, QueenWhite<true>, KingBlack<true>, QueenBlack<true>>(b);
    b.move({{"h8"}, {"f8"}});
    expect_true<KingWhite<false>, QueenWhite<true>, KingBlack<false>, QueenBlack<true>>(b);

    // move them back, but should still not be able to castle
    b.move({{"g1"}, {"h1"}});
    expect_true<KingWhite<false>, QueenWhite<true>, KingBlack<false>, QueenBlack<true>>(b);
    b.move({{"f8"}, {"h8"}});
    expect_true<KingWhite<false>, QueenWhite<true>, KingBlack<false>, QueenBlack<true>>(b);
  }

  {  // take queen rook
    dwc::Board b{"r3k2r/pppq1ppp/1N1p4/4pbB1/8/1n1P4/PPPQPPPP/R3K2R w KQkq"};
    dwc::display(b);
    expect_true<KingWhite<true>, QueenWhite<true>, KingBlack<true>, QueenBlack<true>>(b);

    // take queen rook
    b.move({{"b6"}, {"a8"}});
    expect_true<KingWhite<true>, QueenWhite<true>, KingBlack<true>, QueenBlack<false>>(b);

    // take queen rook
    b.move({{"b3"}, {"a1"}});
    expect_true<KingWhite<true>, QueenWhite<false>, KingBlack<true>, QueenBlack<false>>(b);
  }

  // take king rook
}

TEST(BOARD, EnPassant01) {
  dwc::Board b{"rnbqkbnr/pp1ppppp/2p5/4P3/8/8/PPPP1PPP/RNBQKBNR b"};
  b.move({{"d7"}, {"d5"}});

  // dwc::display(b);

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

TEST(BOARD, FenBoardParser03) {
  using ResT = std::set<dwc::Piece>;
  dwc::Piece K{dwc::Type::KING, dwc::Side::WHITE};
  dwc::Piece Q{dwc::Type::QUEEN, dwc::Side::WHITE};
  dwc::Piece k{dwc::Type::KING, dwc::Side::BLACK};
  dwc::Piece q{dwc::Type::QUEEN, dwc::Side::BLACK};
  EXPECT_EQ(dwc::fen::_inner::parse_castling("K"), (ResT{{K}}));
  EXPECT_EQ(dwc::fen::_inner::parse_castling("KQ"), (ResT{{K, Q}}));
  EXPECT_EQ(dwc::fen::_inner::parse_castling("KQk"), (ResT{{K, Q, k}}));
  EXPECT_EQ(dwc::fen::_inner::parse_castling("KQkq"), (ResT{{K, Q, k, q}}));
  EXPECT_EQ(dwc::fen::_inner::parse_castling("qk"), (ResT{{k, q}}));
  EXPECT_EQ(dwc::fen::_inner::parse_castling(""), (ResT{}));

  EXPECT_THROW(dwc::fen::_inner::parse_castling("KK"), std::runtime_error);
  EXPECT_THROW(dwc::fen::_inner::parse_castling("kqq"), std::runtime_error);
  EXPECT_THROW(dwc::fen::_inner::parse_castling("KQkQ"), std::runtime_error);
  EXPECT_THROW(dwc::fen::_inner::parse_castling("KQkqR"), std::runtime_error);
  EXPECT_THROW(dwc::fen::_inner::parse_castling("KQR"), std::runtime_error);
  EXPECT_THROW(dwc::fen::_inner::parse_castling("R"), std::runtime_error);
}

// TEST(BOARD, TempDisplay) {
//   dwc::Board b;
//   b.reset_position();
//   dwc::display(b);
//   EXPECT_TRUE(1);
// }
