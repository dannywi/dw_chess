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
  - add turn state to board, only allow move for correct side ()
- rule: legal moves per piece -> pass board, piece (or pos), return list of Pos (some with eating enemy's piece)
- move: api for moves
- framework state: whose turn
- framework state: keep a tally of eaten pieces (clear on reset)
- rule: consume Framework, provide all legal moves
- runner: console 2 player engine (command prompt)
- engine: random engine
- runner: console 1 player with random engine

### Cleanups
- include in tests, remove full paths
- add gha to do buildifier and clang format automatically on commit
