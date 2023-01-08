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
  - support en passant (another state in board)
  - support en passant FEN input
  - support castling (another state in board)
  - support castling FEN input
- move validation / state:
  - king is in check -> then the only legal moves are ones that make king not in check (need to copy board)
  - rook/king moved -> can't castle on that side
  - rook/king threatened -> can't castle on that side
- move: FEN notation
- move: pawn promotion
- state:
  - convert "turn" to state { turn_white, turn_black, draw, win_white, win_black }
  - support 50 moves rule (after pawn advance or capture)
  - support 3 repetition draw
- move: api for moves
- framework: api for setting board using FEN notation https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
  - [DONE] board
  - turn, castling, en passant, half move, full move
- framework state: whose turn
- framework state: keep a tally of eaten pieces (clear on reset)
- rule: consume Framework, provide all legal moves
- runner: console 2 player engine (command prompt)
- engine: random engine
- runner: console 1 player with random engine

### Cleanups
- include statement in tests, remove full paths
- [DONE] add script to run buildifier and clang-format
- add gha to do buildifier and clang format automatically on commit
- display should be in it's own folder?
- replace pragma once with proper macros
- set different namespaces for the header libs
- [DONE] make RankT and FileT non-convertible
  - [DONE] create a base in utils, tests
  - [DONE] replace implementation in framework
- make MoverDictT (Piece -> Mover) constexpr
  - create an ordinal for piece (make hasher public)
  - use the ordinal as key
  - may need reverse ordinal to get piece from index
- make MoveDiff constexpr mapping
- move the character mapping to similar structure
