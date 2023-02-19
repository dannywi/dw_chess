#pragma once

#include <optional>

#include "basic_types.hpp"
#include "fen_lib.hpp"

namespace dwc {

class Board {
 private:
  dwc::State state_;

  void flip_turn() {
    if (!state_.turn.has_value()) return;
    state_.turn = state_.turn.value() == Side::WHITE ? Side::BLACK : Side::WHITE;
  }

  void check_move(Pos fr, Pos to) const;

  void set(Pos pos, Piece piece) { board_ut::set(pos, piece, state_.board); }
  void clear(Pos pos) { board_ut::clear(pos, state_.board); }

  void init(std::string_view fen_str) {
    dwc::fen::FenParser fp(fen_str);
    state_.board = fp.get_board_pos();
    state_.turn = fp.get_turn_side().value_or(Side::WHITE);
  }

 public:
  Board() {}
  Board(std::string_view fen_str) { init(fen_str); }

  std::optional<Piece> get(Pos pos) const { return board_ut::get(pos, state_.board); }

  void reset_position() { init("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w"); }

  void move(Move move) {
    check_move(move.fr, move.to);
    std::optional<Piece> piece = get(move.fr);
    clear(move.fr);
    // todo: keep captured piece, per side
    set(move.to, piece.value());

    flip_turn();
  }

  const State& get_state() const { return state_; }

  MovesT get_moves(Pos pos) const;

  template <class T, class... REST>
  void call_movers(Pos pos, MovesT& moves) const {
    auto piece = get(pos);
    if (!piece.has_value()) return;

    if (dwc::utils::contains(T::TargetTypes, piece->type)) {
      MovesT res = T::get_moves(*this, pos);
      moves.insert(end(moves), begin(res), end(res));
    }

    if constexpr (sizeof...(REST)) call_movers<REST...>(pos, moves);
  }
};
}  // namespace dwc
