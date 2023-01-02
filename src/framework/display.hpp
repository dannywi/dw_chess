#pragma once

#include <iostream>
#include <type_traits>

#include "board.hpp"

namespace dwc {
namespace _inner {

const char* SEPARATOR = " ";
const char* EMPTY = ".";

struct GetPieceSymbol {
  const char* operator()(Type type, Side side) {
    // todo: make constexpr
    const char* map[cast_t(Type::SIZE)][cast_t(Side::SIZE)];

    map[cast_t(Type::PAWN)][cast_t(Side::WHITE)] = "P";
    map[cast_t(Type::KNIGHT)][cast_t(Side::WHITE)] = "N";
    map[cast_t(Type::BISHOP)][cast_t(Side::WHITE)] = "B";
    map[cast_t(Type::ROOK)][cast_t(Side::WHITE)] = "R";
    map[cast_t(Type::QUEEN)][cast_t(Side::WHITE)] = "Q";
    map[cast_t(Type::KING)][cast_t(Side::WHITE)] = "K";

    map[cast_t(Type::PAWN)][cast_t(Side::BLACK)] = "p";
    map[cast_t(Type::KNIGHT)][cast_t(Side::BLACK)] = "n";
    map[cast_t(Type::BISHOP)][cast_t(Side::BLACK)] = "b";
    map[cast_t(Type::ROOK)][cast_t(Side::BLACK)] = "r";
    map[cast_t(Type::QUEEN)][cast_t(Side::BLACK)] = "q";
    map[cast_t(Type::KING)][cast_t(Side::BLACK)] = "k";

    return map[cast_t(type)][cast_t(side)];
  }
};

struct GetPieceSymbolUni {
  const char* operator()(Type type, Side side) {
    // todo: make constexpr
    const char* map[cast_t(Type::SIZE)][cast_t(Side::SIZE)];

    map[cast_t(Type::PAWN)][cast_t(Side::WHITE)] = "♙";
    map[cast_t(Type::KNIGHT)][cast_t(Side::WHITE)] = "♘";
    map[cast_t(Type::BISHOP)][cast_t(Side::WHITE)] = "♗";
    map[cast_t(Type::ROOK)][cast_t(Side::WHITE)] = "♖";
    map[cast_t(Type::QUEEN)][cast_t(Side::WHITE)] = "♕";
    map[cast_t(Type::KING)][cast_t(Side::WHITE)] = "♔";

    map[cast_t(Type::PAWN)][cast_t(Side::BLACK)] = "♟︎";
    map[cast_t(Type::KNIGHT)][cast_t(Side::BLACK)] = "♞";
    map[cast_t(Type::BISHOP)][cast_t(Side::BLACK)] = "♝";
    map[cast_t(Type::ROOK)][cast_t(Side::BLACK)] = "♜";
    map[cast_t(Type::QUEEN)][cast_t(Side::BLACK)] = "♛";
    map[cast_t(Type::KING)][cast_t(Side::BLACK)] = "♚";

    return map[cast_t(type)][cast_t(side)];
  }
};

template <typename MAPPER, typename T = typename std::enable_if<std::is_invocable<MAPPER, dwc::Type, dwc::Side>::value>>
void display(const Board& board) {
  // todo: add option to remove file/rank marker
  // todo: add option to flip vertically and have black at bottom
  // assuming monospace
  std::cout << "┏━━━━━━━━━━━━━━━━━━━━┓\n";

  for (char rank = '8'; rank >= '1'; --rank) {
    std::cout << "┃ " << rank << "  ";
    for (char file = 'A'; file <= 'H'; ++file) {
      auto piece = board.get({std::string{file, rank}});
      std::cout << (piece.has_value() ? MAPPER()(piece.value().type, piece.value().side) : _inner::EMPTY);
      std::cout << _inner::SEPARATOR;
    }
    std::cout << "┃\n";
  }

  std::cout << "┃                    ┃\n";
  std::cout << "┃    A B C D E F G H ┃\n";
  std::cout << "┗━━━━━━━━━━━━━━━━━━━━┛\n";
}
}  // namespace _inner

void display(const Board& board) {
  _inner::display<_inner::GetPieceSymbolUni>(board);
}
}  // namespace dwc
