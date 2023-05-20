#include <gtest/gtest.h>

#include <iostream>
#include <map>
#include <vector>

#include "src/framework/board.hpp"
#include "src/framework/display.hpp"  // delete
#include "src/framework/fen_lib.hpp"
#include "src/framework/legal_move.hpp"

using namespace dwc;

namespace {
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

// do in C++11 style, somehow C++17 fold fails to call static function of the types
template <typename... F>
struct And;

template <typename F, typename... REST>
struct And<F, REST...> {
  static bool fold(const Board& b) { return F::check(b) && And<REST...>::fold(b); }
};

template <>
struct And<> {
  static bool fold(const Board& b) { return true; }
};

}  // namespace

TEST(BOARD, Castling01) {
  Board b;
  b.reset_position();

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

TEST(BOARD, Castling03_MoveKing) {
  Board b{"r3kbnr/pppq1ppp/2np4/4pb2/5B2/2NP4/PPPQPPPP/R3KBNR w KQkq"};
  expect_true<KingWhite<false>, QueenWhite<true>, KingBlack<false>, QueenBlack<true>>(b);

  // move the kings
  b.move({{"e1"}, {"d1"}});
  expect_true<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<true>>(b);
  b.move({{"e8"}, {"e7"}});
  expect_true<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<false>>(b);

  // move them back, but should still not be able to castle
  b.move({{"d1"}, {"e1"}});
  expect_true<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<false>>(b);
  b.move({{"e7"}, {"e8"}});
  expect_true<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<false>>(b);
}

TEST(BOARD, Castling03_MoveQueenRook) {
  Board b{"r3k2r/pppq1ppp/2np4/4pbB1/8/2NP4/PPPQPPPP/R3K2R w KQkq"};
  expect_true<KingWhite<true>, QueenWhite<true>, KingBlack<true>, QueenBlack<true>>(b);

  // move queen rooks
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

TEST(BOARD, Castling03_MoveKingRook) {
  Board b{"r3k2r/pppq1ppp/2np4/4pbB1/8/2NP4/PPPQPPPP/R3K2R w KQkq"};
  expect_true<KingWhite<true>, QueenWhite<true>, KingBlack<true>, QueenBlack<true>>(b);

  // move king rooks
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

TEST(BOARD, Castling04_KingDestThreatened) {
  Board b{"r3k2r/ppp2ppp/3p4/4pB2/8/3P4/nPPQPPPP/R3K2R w KQkq"};
  // queen side is threatened, cannot castle
  EXPECT_TRUE((And<KingWhite<true>, QueenWhite<false>, KingBlack<true>, QueenBlack<false>>::fold(b)));
}

// todo: add king dest threatened by pinned piece

TEST(BOARD, Castling05_KingThreatened) {
  Board b{"r3k2r/ppp2ppp/3p4/1B2p3/8/6b1/PPPQP1PP/R3K2R w KQkq"};
  // white king is threatened, cannot castle
  EXPECT_TRUE((And<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<false>>::fold(b)));
}

TEST(BOARD, Castling05_KingThreatenedByPinnedPiece) {
  Board b{"r6r/ppp2ppp/3p4/1B2p2k/7b/7Q/PPP1P1PP/R3K2R w KQ"};
  // white king is threatened by a pinned pieace, still cannot castle
  EXPECT_TRUE((And<KingWhite<false>, QueenWhite<false>, KingBlack<false>, QueenBlack<false>>::fold(b)));
}

TEST(BOARD, Castling06_RookThreatened) {
  Board b{"r3k2r/ppp1pp1p/3p1B2/4p3/8/2b5/P1PQP1PP/R3K2R w KQkq"};
  // the rooks are threatened, cannot castle
  EXPECT_TRUE((And<KingWhite<true>, QueenWhite<false>, KingBlack<false>, QueenBlack<true>>::fold(b)));
}

// TEST(BOARD, Castling06_RookDestThreatened) {
//   Board b{"r3k2r/ppp1pp1p/3p3B/4p3/8/1b6/PP1QP1PP/R3K2R w KQkq"};
//   display(b);
//   // the rooks are threatened, cannot castle
//   EXPECT_TRUE((And<QueenWhite<false>>::fold(b)));
//   //EXPECT_TRUE((And<KingWhite<true>, QueenWhite<false>, KingBlack<false>, QueenBlack<true>>::fold(b)));
// }
