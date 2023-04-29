#include <gtest/gtest.h>

#include <iostream>
#include <map>
#include <vector>

#include "src/framework/board.hpp"
#include "src/framework/display.hpp"  // delete
#include "src/framework/fen_lib.hpp"
#include "src/framework/legal_move.hpp"

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

namespace test {
void check_init_pos(const Board& b) {
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
  Board b{"8/4k3/8/8/2K1r3/8/8/8 w"};
  EXPECT_TRUE(b.is_king_threatened(Side::WHITE));
  EXPECT_FALSE(b.is_king_threatened(Side::BLACK));
}

namespace test {
template <bool EXP>
struct KingWhite {
  static bool check(const Board& b) { return legal_move::is_legal_move(b, {"e1"}, {{"e1"}, {"g1"}}) == EXP; }
};

template <bool EXP>
struct QueenWhite {
  static bool check(const Board& b) { return legal_move::is_legal_move(b, {"e1"}, {{"e1"}, {"c1"}}) == EXP; }
};

template <bool EXP>
struct KingBlack {
  static bool check(const Board& b) { return legal_move::is_legal_move(b, {"e8"}, {{"e8"}, {"g8"}}) == EXP; }
};

template <bool EXP>
struct QueenBlack {
  static bool check(const Board& b) { return legal_move::is_legal_move(b, {"e8"}, {{"e8"}, {"c8"}}) == EXP; }
};

template <typename... F>
void expect_true(const Board& b) {
  EXPECT_TRUE((F::check(b) && ...));
}

}  // namespace test

TEST(BOARD, Castling01) {
  Board b;
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

  b.move({{"d2"}, {"d3"}});
  b.move({{"d7"}, {"d6"}});
  // still cannot castle queen side
  expect_true<KingWhite<true>, QueenWhite<false>, KingBlack<true>, QueenBlack<false>>(b);

  b.move({{"c1"}, {"g5"}});
  b.move({{"c8"}, {"g4"}});
  // still cannot castle queen side
  expect_true<KingWhite<true>, QueenWhite<false>, KingBlack<true>, QueenBlack<false>>(b);

  b.move({{"d1"}, {"d2"}});
  b.move({{"d8"}, {"d7"}});
  // still cannot castle queen side
  expect_true<KingWhite<true>, QueenWhite<false>, KingBlack<true>, QueenBlack<false>>(b);

  b.move({{"b1"}, {"c3"}});
  b.move({{"b8"}, {"a6"}});
  // still cannot castle queen side
  expect_true<KingWhite<true>, QueenWhite<true>, KingBlack<true>, QueenBlack<true>>(b);
}

TEST(BOARD, Castling02) {
  using namespace test;
  {
    Board b{"r3kbnr/pppq1ppp/2np4/4pbB1/8/2NP4/PPPQPPPP/R3KBNR w KQkq"};
    expect_true<KingWhite<false>, QueenWhite<true>, KingBlack<false>, QueenBlack<true>>(b);
  }

  {
    // almost same setting, but rook not in place
    Board b{"p3kbnr/pppq1ppp/2np4/4pbB1/8/2NP4/PPPQPPPP/4KBNR w KQkq"};
    expect_true<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<false>>(b);
  }
}

TEST(BOARD, Castling03) {
  using namespace test;
  {  // move king
    Board b{"r3kbnr/pppq1ppp/2np4/4pbB1/8/2NP4/PPPQPPPP/R3KBNR w KQkq"};
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
    Board b{"r3k2r/pppq1ppp/2np4/4pbB1/8/2NP4/PPPQPPPP/R3K2R w KQkq"};
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
    Board b{"r3k2r/pppq1ppp/2np4/4pbB1/8/2NP4/PPPQPPPP/R3K2R w KQkq"};
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
    Board b{"r3k2r/pppq1ppp/1N1p4/4pbB1/8/1n1P4/PPPQPPPP/R3K2R w KQkq"};
    expect_true<KingWhite<true>, QueenWhite<true>, KingBlack<true>, QueenBlack<true>>(b);

    // black side
    b.move({{"b6"}, {"a8"}});
    expect_true<KingWhite<true>, QueenWhite<true>, KingBlack<true>, QueenBlack<false>>(b);

    // white side
    b.move({{"b3"}, {"a1"}});
    expect_true<KingWhite<true>, QueenWhite<false>, KingBlack<true>, QueenBlack<false>>(b);
  }

  // take king rook
  {  // take queen rook
    Board b{"r3k2r/pppq1ppp/1N1p2N1/4pbB1/8/1n1P2n1/PPPQPPPP/R3K2R w KQkq"};
    expect_true<KingWhite<true>, QueenWhite<true>, KingBlack<true>, QueenBlack<true>>(b);

    // black side
    b.move({{"g6"}, {"h8"}});
    expect_true<KingWhite<true>, QueenWhite<true>, KingBlack<false>, QueenBlack<true>>(b);

    // white side
    b.move({{"g3"}, {"h1"}});
    expect_true<KingWhite<false>, QueenWhite<true>, KingBlack<false>, QueenBlack<true>>(b);
  }
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

TEST(BOARD, FenBoardParserException01) {
  EXPECT_THROW(Board{"rnbqkbnrr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"}, std::runtime_error);
  EXPECT_THROW(Board{"rnbqkbnrr/pppppppp/9/8/8/8/PPPPPPPP/RNBQKBNR"}, std::runtime_error);
  EXPECT_THROW(Board{"rnbqkbnrr/pppppppp/"}, std::runtime_error);
  EXPECT_THROW(Board{"rnbqkbnrr/pppppppp/4p4/8/8/8/PPPPPPPP/RNBQKBNR"}, std::runtime_error);
  EXPECT_NO_THROW(Board{"8/8/8/4p3/2K2Q2/8/8/8"});
}

TEST(BOARD, FenBoardParserException02) {
  EXPECT_THROW(Board{"8/8/8/4p3/2K2Q2/8/8/8 w 123"}, std::runtime_error);  // more than available parser
}

TEST(BOARD, FenBoardParserException03) {
  Board b{"rnbqkbnr/pp1ppppp/2p5/4P3/8/8/PPPP1PPP/RNBQKBNR w"};
  EXPECT_THROW(b.move({{"d7"}, {"d5"}}), std::logic_error);  // wrong side move
}

TEST(BOARD, FenBoardParser01) {
  Board b{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"};
  test::check_init_pos(b);
}

TEST(BOARD, FenBoardParser02) {
  EXPECT_EQ(fen::_inner::parse_side("w").value(), Side::WHITE);
  EXPECT_EQ(fen::_inner::parse_side("b").value(), Side::BLACK);
  EXPECT_THROW(fen::_inner::parse_side("bw").value(), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_side("a").value(), std::runtime_error);
}

TEST(BOARD, FenBoardParser03) {
  using ResT = std::set<Piece>;
  Piece K{Type::KING, Side::WHITE};
  Piece Q{Type::QUEEN, Side::WHITE};
  Piece k{Type::KING, Side::BLACK};
  Piece q{Type::QUEEN, Side::BLACK};
  EXPECT_EQ(fen::_inner::parse_castling("K"), (ResT{{K}}));
  EXPECT_EQ(fen::_inner::parse_castling("KQ"), (ResT{{K, Q}}));
  EXPECT_EQ(fen::_inner::parse_castling("KQk"), (ResT{{K, Q, k}}));
  EXPECT_EQ(fen::_inner::parse_castling("KQkq"), (ResT{{K, Q, k, q}}));
  EXPECT_EQ(fen::_inner::parse_castling("qk"), (ResT{{k, q}}));
  EXPECT_EQ(fen::_inner::parse_castling(""), (ResT{}));

  EXPECT_THROW(fen::_inner::parse_castling("KK"), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_castling("kqq"), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_castling("KQkQ"), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_castling("KQkqR"), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_castling("KQR"), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_castling("R"), std::runtime_error);
}

// TEST(BOARD, TempDisplay) {
//   Board b;
//   b.reset_position();
//   display(b);
//   EXPECT_TRUE(1);
// }
