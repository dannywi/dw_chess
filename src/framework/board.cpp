#include "board.hpp"

#include <iostream>

#include "legal_move.hpp"

namespace dwc {
void Board::check_move(Pos fr, Pos to) const {
  std::optional<Piece> piece = get(fr);
  if (!piece.has_value()) { throw std::logic_error("moving empty square"); }
  if (state_.turn.has_value() && piece.value().side != state_.turn.value()) {
    throw std::logic_error("moving wrong side");
  }
  if (!legal_move::is_legal_move(*this, fr, {fr, to})) {
    std::ostringstream oss;
    oss << "not legal move: " << fr << " -> " << to;
    throw std::logic_error(oss.str());
  }
}

MovesT Board::get_moves(Pos pos) const {
  MovesT moves;
  call_movers<MoverUpdaterList>(pos, moves);

  // pinned piece can still threaten castling or king, so don't check its king
  if (!is_checking_threats()) {
    auto king_is_threatened_after = [this](const Move& move) {
      auto piece = this->get(move.fr);
      Side side = piece->side;
      Board b_copy = *this;
      b_copy.move_internal(move, *piece);
      return b_copy.is_king_threatened(side);
    };
    auto new_end = std::remove_if(moves.begin(), moves.end(), king_is_threatened_after);
    moves.erase(new_end, moves.end());
  }

  return moves;
}

void Board::dump_moves(Pos pos) const {
  auto moves = get_moves(pos);
  std::cout << "Available Moves " << pos << std::endl;
  for (const auto& i : moves) { std::cout << "- " << i << std::endl; }
}

void Board::move(Move move) {
  check_move(move.fr, move.to);
  std::optional<Piece> piece = get(move.fr);
  move_internal(move, piece.value());
  call_updaters<MoverUpdaterList>(state_, move);
}

bool Board::is_threatened(Pos pos) const {
  auto tgt = get(pos);
  if (!tgt.has_value()) { return false; }
  return is_threatened(pos, tgt->side);
}

bool Board::is_threatened(Pos pos, Side side) const {
  // if necessary move this to util lib with added features
  struct set_true {
    bool& b;
    set_true(bool& b) : b(b) { b = true; }
    ~set_true() { b = false; }
  } g{is_checking_threats_};

  // todo: create an iterator, using generator coroutine if possible
  for (char rank = '8'; rank >= '1'; --rank) {
    for (char file = 'A'; file <= 'H'; ++file) {
      Pos curr_pos{std::string{file, rank}};
      if (pos == curr_pos) { continue; }
      auto curr_pcs = get(curr_pos);
      if (!curr_pcs.has_value() || curr_pcs->side == side) { continue; }
      auto moves = get_moves(curr_pos);
      for (const auto& move : moves) {
        if (move.to == pos) { return true; }
      }
    }
  }

  return false;
}

bool Board::is_king_threatened(Side side) const {
  std::optional<Pos> pos_king;
  // todo: create an iterator, using generator coroutine if possible
  for (char rank = '8'; rank >= '1'; --rank) {
    for (char file = 'A'; file <= 'H'; ++file) {
      Pos curr_pos{std::string{file, rank}};
      auto curr_pcs = get(curr_pos);
      if (curr_pcs.has_value() && curr_pcs->type == Type::KING && curr_pcs->side == side) {
        pos_king = curr_pos;
        break;
      }
    }
  }

  if (!pos_king.has_value()) throw std::logic_error("this side has no king on board");

  return is_threatened(*pos_king);
}

}  // namespace dwc
