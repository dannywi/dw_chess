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
- framework: plugin display func
- framework: provide APIs
- rule: legal moves per piece
- rule: consume Framework, provide all legal moves
- runner: console 2 player engine (command prompt)
- engine: random engine
- runner: console 1 player with random engine

### Cleanups
- include in tests, remove full paths
- add gha to do buildifier and clang format automatically on commit
