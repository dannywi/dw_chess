#pragma once

#include <optional>

#include "basic_types.hpp"
#include "fen_lib.hpp"

namespace dwc {

class Board {
 private:
  BoardT board_;
  std::optional<Side> turn_;

  void flip_turn() {
    if (!turn_.has_value()) return;
    turn_ = turn_.value() == Side::WHITE ? Side::BLACK : Side::WHITE;
  }

  void check_move(Pos fr, Pos) {
    // board doesn't know chess rules, just having squares and pieces. These are "physical" validations.
    std::optional<Piece> piece = get(fr);
    if (!piece.has_value()) { throw std::logic_error("moving empty square"); }
    if (turn_.has_value() && piece.value().side != turn_.value()) { throw std::logic_error("moving wrong side"); }
  }

  void set(Pos pos, Piece piece) { board_ut::set(pos, piece, board_); }
  void clear(Pos pos) { board_ut::clear(pos, board_); }

  void init(std::string_view fen_str) {
    dwc::fen::FenParser fp(fen_str);
    board_ = fp.get_board_pos();
    turn_ = fp.get_turn_side().value_or(Side::WHITE);
  }

 public:
  Board() {}
  Board(std::string_view fen_str) { init(fen_str); }

  std::optional<Piece> get(Pos pos) const { return board_ut::get(pos, board_); }

  void reset_position() { init("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w"); }

  void move(Move move) {
    check_move(move.fr, move.to);
    std::optional<Piece> piece = get(move.fr);
    clear(move.fr);
    // todo: keep captured piece, per side
    set(move.to, piece.value());

    flip_turn();
  }
};
}  // namespace dwc
