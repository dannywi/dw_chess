# CHESS GAME (WIP)
Chess game framework and engine library

## Run Tests
```bash
./run_tests.sh
```

## Format Files
```bash
./run_format.sh
```

## TODO
### Framework
Provide I/O functionalities:
- output board in fen + display
- execute move commands
- load preset boards (fen)
- persist board + history
- do rudimentary checks (board size, duplicated entries, etc.)
- track which side can move

### Rule Engine
- provide legal moves
- check is: mate, legal move

### Logic
- give best move given current board and side to move
- give score for each side

### Runner
- ensemble the board + engine + logic

## TODO
- [DONE] framework: setup board class(es) with internal reps
  - [DONE] setup position, square, piece classes
  - [DONE] setup board with initial position
- [DONE] framework: plugin display func
- [DONE] framework: provide APIs
  - [DONE] internal representation of a move (square 1 to square 2)
  - [DONE] assert orig square is not empty, replace target square if not empty
  - [DONE] add turn state to board, only allow move for correct side (if board is reset)
- rule: legal moves per piece
  - [DONE] namespace LegalMove
  - [DONE] move concepts
    - [DONE] direction: up/down/left/right/upleft/downleft/..../kn_upleft/kn_leftup -> map to move arithmetic
    - [DONE] limit: once/unlimited
  - [DONE] all types have move_characteristic: direction + limit
  - [DONE] get_legal_move: get piece type, for all directions/limit, move until limit
  - [DONE] ? move Pos / Type / Square to basic_types
  - [DONE] pass Board, piece (or pos), return list of Pos (some with eating enemy's piece)
  - [DONE] add pawn initially can move 2 squares
  - [DONE] add pawn can only take diagonally, and cannot capture with it's normal move
  - [DONE] on move, throw if it's not within legal move
  - [DONE] decouple State: create a State struct that has board, enpassant pos, castling condition, half move, turn
  - [DONE] move MovesT to basic_types
  - [DONE] special move abstraction:
    - [DONE] after each move, update state - castling: remove if castling, remove if moved, en passant: add if 2 squares move
    - [DONE] add legal move, pass state and position (separate classes for enpassant, castling, ...)
  - [DONE] first use the concept for pawn ahead move (doesn't need state) - remove from get_extra_moves_pawn
  - [DONE] use for pawn diagonal take, completely remove get_extra_moves_pawn
  - [DONE] Add "target piece" in each mover class, so call_movers can do the piece check, and empty check
  - [DONE] use for change turn
  - [DONE] use for basic Mover too, so legal_moves.hpp contains only these mover (stateless) classes
  - [DONE] use type_list, so mover updater and caller don't need to repeat the list (don't use tuple since it creates objects of the types)
  - support castling (another state in board)
    - [DONE] add state (class for KQkq)
    - [DONE] change signature in get_move to include const State
    - add move
      - [DONE] add helper structs to check each castling types
      - [DONE] list out test cases to fulfill
      - add restriction: cannot castle if under threat (is this King only?)
      - add restriction: cannot castle if the destination is under threat (is this King only?)
    - on move, remove the castling entry afterwards
  - support en passant (another state in board)
  - i.e. there are only 2 things that can add move, the normal mover, and this irregular mover
- move validation / state:
  - king is in check -> then the only legal moves are ones that make king not in check (need to copy board)
  - rook/king moved -> can't castle on that side
  - rook/king threatened -> can't castle on that side
- move: FEN notation
  - [DONE] support piece placement
  - [DONE] support multiple segments
  - [DONE] support turn
  - [DONE] support castling
  - support en passant
  - support half move, max 50
- move: pawn promotion
- state:
  - convert "turn" to state { turn_white, turn_black, draw, win_white, win_black }
  - support 50 moves rule (after pawn advance or capture)
  - support 3 repetition draw
- move: api for moves
- framework: api for setting board using FEN notation https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
  - [DONE] board
  - turn, castling, en passant, half move, full move
[DONE] - framework state: whose turn
- framework state: keep a tally of eaten pieces (clear on reset)
- add captured piece to state, each side
[DONE] - rule: consume Framework, provide all legal moves
- runner: console 2 player engine (command prompt)
- engine: random engine
- runner: console 1 player with random engine

### Cleanups
- [DONE] add github action for formatting and testing
- fix formatter in CI
  - install buildifier
  - use option to fix or check only
  - make CI fail on script failure
- use toolchain in bazel, so local and CI are aligned
- use bazelisk to control bazel version
- check if special moves like en passant, castling, and promotion can be abstracted
- split test cases per include file or namespace
- include statement in tests, remove full paths
- [DONE] add script to run buildifier and clang-format
- add gha to do buildifier and clang format automatically on commit
- display should be in it's own folder?
- replace pragma once with proper macros
- set different namespaces for the header libs
- [DONE] make RankT and FileT non-convertible
  - [DONE] create a base in utils, tests
  - [DONE] replace implementation in framework
- [DONE] make MoverDictT (Piece -> Mover) constexpr
  - [DONE] create an ordinal for piece (make hasher public)
  - [DONE] use the ordinal as key
  - [DONE] may need reverse ordinal to get piece from index
- [DONE] make MoveDiff constexpr mapping
- move the character mapping to similar structure
- make PiecesT detect the array size without passing it
- maybe separate files for each movers, and add unit tests
  - or, rename legal_moves to mover_updater.hpp
- refactor call_updater and call_movers, e.g. with apply func with type_list
- [DONE] add _t and _v versions of type_list utilities
- add edge case tests for type_list utils, e.g. with zero template param
- make FebLib receive a state object, remove its members
- allow castling to be done by moving rook
- make castle info compile time
- get_legal_move has the original position passed in twice, in pos and move.from, remove this redundancy

// LATEST TODO:
- because the static constexpr std::map in MoverCastling doesn't work (std::map cannot be constructed at compile time),
  - [DONE] create a compile time constructible map, in shared
  - create tests for it, using struct keys and values
  - use it in place of std::map in MoverCastling
  - add MoverCastling to board typelist
  - uncomment the prewritten test cases
