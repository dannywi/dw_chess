#include <gtest/gtest.h>

#include <iostream>
#include <map>
#include <vector>

#include "src/framework/display.hpp"  // delete
#include "src/framework/fen_lib.hpp"
#include "test_utils.hpp"

using namespace dwc;

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
  EXPECT_EQ(fen::_inner::parse_castling("-"), (ResT{}));
  EXPECT_EQ(fen::_inner::parse_castling(""), (ResT{}));

  EXPECT_THROW(fen::_inner::parse_castling("KK"), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_castling("kqq"), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_castling("KQkQ"), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_castling("KQkqR"), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_castling("KQR"), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_castling("R"), std::runtime_error);
}

TEST(BOARD, FenBoardParser04) {
  EXPECT_EQ(fen::_inner::parse_en_passant("-", Side::BLACK), std::optional<dwc::Pos>{});
  EXPECT_EQ(fen::_inner::parse_en_passant("-", Side::WHITE), std::optional<dwc::Pos>{});
  EXPECT_EQ(fen::_inner::parse_en_passant("-", Side::SIZE), std::optional<dwc::Pos>{});

  EXPECT_EQ(fen::_inner::parse_en_passant("e3", Side::BLACK), std::optional<dwc::Pos>{"e3"});
  EXPECT_EQ(fen::_inner::parse_en_passant("b3", Side::BLACK), std::optional<dwc::Pos>{"b3"});
  EXPECT_EQ(fen::_inner::parse_en_passant("a6", Side::WHITE), std::optional<dwc::Pos>{"a6"});
  EXPECT_EQ(fen::_inner::parse_en_passant("g6", Side::WHITE), std::optional<dwc::Pos>{"g6"});

  EXPECT_THROW(fen::_inner::parse_en_passant("c6", Side::BLACK), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_en_passant("d7", Side::BLACK), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_en_passant("e8", Side::BLACK), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_en_passant("c3", Side::WHITE), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_en_passant("b2", Side::WHITE), std::runtime_error);
  EXPECT_THROW(fen::_inner::parse_en_passant("a1", Side::WHITE), std::runtime_error);
}
