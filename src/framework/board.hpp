#pragma once

#include <optional>

#include "basic_types.hpp"
#include "fen_lib.hpp"
#include "src/shared/type_list.hpp"

namespace dwc {

namespace legal_move {
class MoverBasic;
class UpdaterTurn;
class MoverPawnAhead;
class MoverPawnTake;
class MoverCastling;
}  // namespace legal_move

class Board {
 private:
  dwc::State state_;

  void check_move(Pos fr, Pos to) const;

  void set(Pos pos, Piece piece) { board_ut::set(pos, piece, state_.board); }
  void clear(Pos pos) { board_ut::clear(pos, state_.board); }

  void init(std::string_view fen_str) {
    dwc::fen::FenParser fp(fen_str);
    state_.board = fp.get_board_pos();
    state_.turn = fp.get_turn_side().value_or(Side::WHITE);
    state_.castling = fp.get_castling();
  }

  using MoverUpdaterList = utils::type_list<legal_move::MoverBasic, legal_move::UpdaterTurn, legal_move::MoverPawnAhead,
                                            legal_move::MoverPawnTake, legal_move::MoverCastling>;
  // todo: static check here for no duplicated types
 public:
  Board() {}
  Board(std::string_view fen_str) { init(fen_str); }

  std::optional<Piece> get(Pos pos) const { return board_ut::get(pos, state_.board); }

  void reset_position() { init("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq"); }

  void move(Move move);
  MovesT get_moves(Pos pos) const;
  void dump_moves(Pos pos) const;
  const State& get_state() const { return state_; }

  template <typename TL>
  void call_movers(Pos pos, MovesT& moves) const {
    auto piece = get(pos);
    if (!piece.has_value()) return;

    using T = dwc::utils::head_t<TL>;
    if (dwc::utils::contains(T::TargetTypes, piece->type)) {
      MovesT res = T::get_moves(*this, state_, pos);
      moves.insert(end(moves), begin(res), end(res));
    }

    using TAIL = dwc::utils::tail_t<TL>;
    if constexpr (dwc::utils::size_v < TAIL >> 0) call_movers<TAIL>(pos, moves);
  }

  template <typename TL>
  void call_updaters(State& state, Move move) const {
    auto piece = get(move.to);
    if (!piece.has_value()) return;

    using T = dwc::utils::head_t<TL>;
    if (dwc::utils::contains(T::TargetTypes, piece->type)) { T::update_state(state, *piece, move); }

    using TAIL = dwc::utils::tail_t<TL>;
    if constexpr (dwc::utils::size_v < TAIL >> 0) call_updaters<TAIL>(state, move);
  }
};
}  // namespace dwc
