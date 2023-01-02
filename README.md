# CHESS GAME
Chess game framework and engine

## Run Tests
```bash
./run_tests.sh
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
DONE - framework: setup board class(es) with internal reps
  DONE - setup position, square, piece classes
  DONE - setup board with initial position
DONE - framework: plugin display func
DONE - framework: provide APIs
  DONE - internal representation of a move (square 1 to square 2)
  DONE - assert orig square is not empty, replace target square if not empty
  DONE - add turn state to board, only allow move for correct side (if board is reset)
- rule: legal moves per piece
  DONE - namespace LegalMove
  DONE - move concepts
    DONE - direction: up/down/left/right/upleft/downleft/..../kn_upleft/kn_leftup -> map to move arithmetic
    DONE - limit: once/unlimited
  DONE - all types have move_characteristic: direction + limit
  DONE - get_legal_move: get piece type, for all directions/limit, move until limit
  DONE - ? move Pos / Type / Square to basic_types
  DONE - pass Board, piece (or pos), return list of Pos (some with eating enemy's piece)
  - add pawn initially can move 2 squares
  - state validation: king is in check -> then the only legal moves are ones that make king not in check (need to copy board)
  - support en passant (another state in board)
  - support castling (another state in board, queen side + king side)
  - support 50 moves rule (after pawn advance or capture)
  - support 3 repetition draw
- move: api for moves
- framework: api for setting board using FEN notation
- framework state: whose turn
- framework state: keep a tally of eaten pieces (clear on reset)
- rule: consume Framework, provide all legal moves
- runner: console 2 player engine (command prompt)
- engine: random engine
- runner: console 1 player with random engine

### Cleanups
- include in tests, remove full paths
- add gha to do buildifier and clang format automatically on commit
