# RVC System Test Scenarios

Generated from `simulator/scenario_defs.py` by `simulator/generate.py`. Sources: `docs/preliminary_requirements.md` (R1-R7), `docs/clarifications.md` (C1-C29), `artifacts/SA/DFD/dfd_level_4_fsm.md` (T1-T25), `artifacts/SA/event_list.md`, `artifacts/SA/process_specification.md`.

## 1. Test Design Policy

- 22 scenarios, each on the 10x10 grid, at most 100 ticks, with an explicit start pose, obstacles and dust.
- Each scenario evaluates at least 3 essential system functions (Section 3).
- Expected behaviour is written from the requirements, clarifications and the FSM, not from simulator output.
- The test oracle is the human evaluator. No PASS/FAIL is assigned automatically; every result starts as *Pending*.
- The full state is recorded at every tick (`simulator/traces/ST-xx.json`, and live runs in the simulator).

### Conventions

- Coordinates: x = column 0..9 (left to right), y = row 0..9 (top to bottom). Default start (4,4).
- Heading N = up (towards y-1). The requirements give no compass direction; the default start heading N is a simulator convention. Left of N is W, right of N is E.
- Walls: cells outside the grid count as obstacles for the sensors and the motion model.
- Map legend: `^ > v <` RVC start and heading, `#` obstacle, `*` dust, `.` empty. A start cell with dust is listed under Dust.
- Tick model: tick k senses at the pose reached after tick k-1 (F/L/R are the neighbour cells relative to the heading, dust is the current cell). The front interrupt is raised when F changes. Then `rvc_tick()` runs, cleaning removes the dust of the current cell if the cleaner is clean_on/clean_up, and the motor applies the held Direction: Forward/Backward move 1 cell; a turn changes the heading by 90 degrees at the end of its 3rd tick. Tick 0 is the state after `rvc_startup()`.

## 2. Scenarios

### ST-01 - Start-up and wall-following cleaning in an empty room

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: none
- Dust: none
- Maximum Ticks: 100

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . . . . . . .
  y2 . . . . . . . . . .
  y3 . . . . . . . . . .
  y4 . . . . ^ . . . . .
  y5 . . . . . . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-01 - Start-up outputs: Direction = Forward and Clean = clean_on sent once; stores false
- Essential Function 2: EF-02 - Straight forward movement with normal cleaning (clean_on)
- Essential Function 3: EF-03 - Front obstacle detection by interrupt (walls and obstacles); false only updates the store
- Essential Function 4: EF-06 - Avoidance: turn left, left has priority when both sides are free
- Essential Function 5: EF-08 - 3-tick turn, then resume forward with clean_on
- Essential Function 6: EF-09 - Cleaning stopped (clean_off) only while turning or reversing
- Essential Function 7: EF-17 - Direction and Clean are sent only when their value changes; signals only on transitions

#### Requirement Traceability
- Related Requirement IDs: R1, R2, R3, C1, C3, C4, C5, C8, C12, C13, C16, C21, C22, C27
- Related SA Artifacts: FSM rows T1, T2, T5, T12, T16 (`dfd_level_4_fsm.md`); Event List: Front Sensor Input, Left/Right Sensor Input, Tick, Direction, Clean; Process Specifications: 1.1 Front Sensor Interface, 1.5 Determine Obstacle Location, 2.1.1 Controller, 2.1.2 Move Forward, 2.1.3 Turn Left, 2.1.6 Clean Normal, 2.1.7 Clean Off, 2.2 Motor Interface, 2.3 Cleaner Interface

#### Expected Behavior
- Tick 0: Direction = Forward and Clean = clean_on are written once; state FwdNormal; all stores false (C21, C22, C27).
- Ticks 1-4: the RVC moves straight N from (4,4) to (4,0). No Direction or Clean is written (C12).
- Tick 5: at (4,0) the wall ahead raises the front interrupt (F = true). Left is free, so the RVC turns left: Direction = Turn Left, Clean = clean_off (R3, C4, C8).
- Ticks 5-7: the turn lasts 3 ticks; the heading becomes W at the end of tick 7 (C5).
- Tick 8: the front interrupt reports F = false; Direction = Forward and Clean = clean_on (R3).
- For the rest of the run the RVC follows the walls, turning left at every corner (3-tick turn each). It never turns right and never reverses. The right sensor is true along the walls and causes no reaction (C13).

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 0, 4, 5, 7, 8, 12, 15

### ST-02 - Straight cleaning through a corridor with side walls

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (3,1), (3,2), (3,3), (3,4), (3,5), (3,6), (5,1), (5,2), (5,3), (5,4), (5,5), (5,6)
- Dust: none
- Maximum Ticks: 20

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . # . # . . . .
  y2 . . . # . # . . . .
  y3 . . . # . # . . . .
  y4 . . . # ^ # . . . .
  y5 . . . # . # . . . .
  y6 . . . # . # . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-02 - Straight forward movement with normal cleaning (clean_on)
- Essential Function 2: EF-04 - Periodic left/right/dust sampling, once per tick
- Essential Function 3: EF-05 - Side-only obstacle with front clear causes no reaction
- Essential Function 4: EF-06 - Avoidance: turn left, left has priority when both sides are free
- Essential Function 5: EF-17 - Direction and Clean are sent only when their value changes; signals only on transitions

#### Requirement Traceability
- Related Requirement IDs: R2, R3, C2, C4, C12, C13, C17
- Related SA Artifacts: FSM rows T5, T2, T12, T16 (`dfd_level_4_fsm.md`); Event List: Left/Right Sensor Input, Front Sensor Input, Direction, Clean; Process Specifications: 1.2 Left Sensor Interface, 1.3 Right Sensor Interface, 1.5 Determine Obstacle Location, 2.1.1 Controller, 2.1.2 Move Forward, 2.1.3 Turn Left, 2.2 Motor Interface

#### Expected Behavior
- Ticks 1-4: left and right are both sampled true each tick while the front is clear. The RVC keeps moving forward with clean_on; the state stays FwdNormal and nothing is written (C13, C12).
- The RVC leaves the corridor at (4,0) after tick 4.
- Tick 5: wall ahead at (4,0), left (3,0) free: Turn Left + clean_off (R3, C4).
- Tick 8: Forward + clean_on heading W. From tick 9 the left side (S) is the corridor wall again; it causes no reaction (C13).

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 1, 4, 5, 8, 9

### ST-03 - Left-priority turn at a single front obstacle

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (4,1)
- Dust: none
- Maximum Ticks: 20

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . # . . . . .
  y2 . . . . . . . . . .
  y3 . . . . . . . . . .
  y4 . . . . ^ . . . . .
  y5 . . . . . . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-03 - Front obstacle detection by interrupt (walls and obstacles); false only updates the store
- Essential Function 2: EF-06 - Avoidance: turn left, left has priority when both sides are free
- Essential Function 3: EF-08 - 3-tick turn, then resume forward with clean_on
- Essential Function 4: EF-09 - Cleaning stopped (clean_off) only while turning or reversing
- Essential Function 5: EF-16 - Front interrupt during a turn or reverse is not acted on

#### Requirement Traceability
- Related Requirement IDs: R3, C1, C3, C4, C5, C8, C15, C18, C19, C20, C25
- Related SA Artifacts: FSM rows T2, T12, T16 (`dfd_level_4_fsm.md`); Event List: Front Sensor Input, Left/Right Sensor Input, Direction, Clean; Process Specifications: 1.1 Front Sensor Interface, 1.5 Determine Obstacle Location, 2.1.1 Controller, 2.1.2 Move Forward, 2.1.3 Turn Left, 2.1.6 Clean Normal, 2.1.7 Clean Off, 2.2 Motor Interface, 2.3 Cleaner Interface

#### Expected Behavior
- Ticks 1-2: forward to (4,2).
- Tick 3: obstacle (4,1) ahead: front interrupt F = true. Left (3,2) and right (5,2) are both free, so the RVC turns LEFT (C4): Disable Move Forward, Clean = clean_off, Direction = Turn Left.
- Ticks 4-5: still turning, no outputs; F stays true and is not acted on (C15). Heading W at end of tick 5.
- Tick 6: front interrupt F = false; Direction = Forward, Clean = clean_on. The RVC moves W along row 2.
- Tick 10: wall ahead at (0,2), left (S) free: another left turn.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 2, 3, 5, 6, 10

### ST-04 - Right turn when the left side is blocked

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (4,1), (3,2)
- Dust: none
- Maximum Ticks: 20

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . # . . . . .
  y2 . . . # . . . . . .
  y3 . . . . . . . . . .
  y4 . . . . ^ . . . . .
  y5 . . . . . . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-03 - Front obstacle detection by interrupt (walls and obstacles); false only updates the store
- Essential Function 2: EF-07 - Avoidance: turn right when the left is blocked
- Essential Function 3: EF-08 - 3-tick turn, then resume forward with clean_on
- Essential Function 4: EF-09 - Cleaning stopped (clean_off) only while turning or reversing

#### Requirement Traceability
- Related Requirement IDs: R3, C1, C4, C5, C8, C18
- Related SA Artifacts: FSM rows T3, T17, T21, T2 (`dfd_level_4_fsm.md`); Event List: Front Sensor Input, Left/Right Sensor Input, Direction, Clean; Process Specifications: 1.1 Front Sensor Interface, 1.2 Left Sensor Interface, 1.3 Right Sensor Interface, 1.5 Determine Obstacle Location, 2.1.1 Controller, 2.1.4 Turn Right, 2.1.6 Clean Normal, 2.1.7 Clean Off, 2.2 Motor Interface, 2.3 Cleaner Interface

#### Expected Behavior
- Tick 3: at (4,2) F = true (4,1), L = true (3,2), R = false (5,2): Direction = Turn Right, Clean = clean_off (R3).
- Ticks 3-5: turning right; heading E at end of tick 5 (C5).
- Tick 6: Forward + clean_on heading E; the RVC reaches (9,2) after tick 10.
- Tick 11: wall ahead, left (N) free: Turn Left (left priority again).

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 3, 5, 6, 11

### ST-05 - Boxed-in dead end: reverse 3 ticks, then turn left

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (4,1), (3,1), (3,2), (3,3), (5,1), (5,2), (5,3)
- Dust: none
- Maximum Ticks: 20

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . # # # . . . .
  y2 . . . # . # . . . .
  y3 . . . # . # . . . .
  y4 . . . . ^ . . . . .
  y5 . . . . . . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-05 - Side-only obstacle with front clear causes no reaction
- Essential Function 2: EF-10 - Boxed in (F, L, R): move backward for at least 3 ticks, continue while both sides are blocked
- Essential Function 3: EF-11 - Reverse ends with a turn, left first
- Essential Function 4: EF-09 - Cleaning stopped (clean_off) only while turning or reversing
- Essential Function 5: EF-16 - Front interrupt during a turn or reverse is not acted on
- Essential Function 6: EF-17 - Direction and Clean are sent only when their value changes; signals only on transitions

#### Requirement Traceability
- Related Requirement IDs: R4, C4, C8, C9, C13, C15, C19, C24, C25, C26
- Related SA Artifacts: FSM rows T4, T24, T22, T12, T16 (`dfd_level_4_fsm.md`); Event List: Front Sensor Input, Left/Right Sensor Input, Tick, Direction, Clean; Process Specifications: 1.1 Front Sensor Interface, 1.5 Determine Obstacle Location, 2.1.1 Controller, 2.1.2 Move Forward, 2.1.3 Turn Left, 2.1.5 Move Backward, 2.1.7 Clean Off, 2.1.6 Clean Normal, 2.2 Motor Interface, 2.3 Cleaner Interface

#### Expected Behavior
- Tick 2: at (4,3) L and R are true with the front clear: no reaction (C13).
- Tick 3: at (4,2) F, L and R are all true: Disable Move Forward, Clean = clean_off, Enable Move Backward -> Direction = Backward. move_forward_active becomes false and move_backward_active true (R4, C24).
- Ticks 3-5: moving backward (to (4,3), (4,4), (4,5)); reverse counter 0, 1, 2. At tick 4 the front interrupt reports F = false: not acted on (C15, C19).
- Tick 6: reverse has lasted 3 ticks and left is free: Disable Move Backward, Direction = Turn Left. No Clean write: clean_off continues (C9, C4).
- Tick 9: turn done, front clear: Enable Move Forward -> Direction = Forward, Clean = clean_on.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 2, 3, 4, 5, 6, 9

### ST-06 - Long dead end: reverse continues while both sides are blocked

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (4,1), (3,2), (3,3), (3,4), (3,5), (3,6), (3,7), (5,2), (5,3), (5,4), (5,5), (5,6), (5,7)
- Dust: none
- Maximum Ticks: 25

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . # . . . . .
  y2 . . . # . # . . . .
  y3 . . . # . # . . . .
  y4 . . . # ^ # . . . .
  y5 . . . # . # . . . .
  y6 . . . # . # . . . .
  y7 . . . # . # . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-10 - Boxed in (F, L, R): move backward for at least 3 ticks, continue while both sides are blocked
- Essential Function 2: EF-11 - Reverse ends with a turn, left first
- Essential Function 3: EF-16 - Front interrupt during a turn or reverse is not acted on
- Essential Function 4: EF-09 - Cleaning stopped (clean_off) only while turning or reversing

#### Requirement Traceability
- Related Requirement IDs: R4, C9, C15, C19, C4, C8
- Related SA Artifacts: FSM rows T4, T24, T25, T22 (`dfd_level_4_fsm.md`); Event List: Front Sensor Input, Left/Right Sensor Input, Tick, Direction; Process Specifications: 1.1 Front Sensor Interface, 1.2 Left Sensor Interface, 1.3 Right Sensor Interface, 1.5 Determine Obstacle Location, 2.1.1 Controller, 2.1.3 Turn Left, 2.1.5 Move Backward, 2.2 Motor Interface

#### Expected Behavior
- Tick 3: boxed in at (4,2): Direction = Backward, Clean = clean_off.
- Tick 4: front interrupt F = false while reversing: no action (C15).
- Ticks 6-8: the 3-tick minimum is reached but L and R are still both blocked, so reversing continues (C9). No outputs.
- Tick 9: at (4,8) the sides are free: Direction = Turn Left. The reverse lasted 6 ticks (3-8).
- Tick 12: Forward + clean_on heading W.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 3, 4, 6, 8, 9, 12

### ST-07 - Reverse ends with a right turn (left still blocked)

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (4,1), (3,2), (3,3), (3,4), (3,5), (3,6), (3,7), (3,8), (3,9), (5,2), (5,3), (5,4), (5,5), (5,6)
- Dust: none
- Maximum Ticks: 25

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . # . . . . .
  y2 . . . # . # . . . .
  y3 . . . # . # . . . .
  y4 . . . # ^ # . . . .
  y5 . . . # . # . . . .
  y6 . . . # . # . . . .
  y7 . . . # . . . . . .
  y8 . . . # . . . . . .
  y9 . . . # . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-10 - Boxed in (F, L, R): move backward for at least 3 ticks, continue while both sides are blocked
- Essential Function 2: EF-11 - Reverse ends with a turn, left first
- Essential Function 3: EF-07 - Avoidance: turn right when the left is blocked
- Essential Function 4: EF-08 - 3-tick turn, then resume forward with clean_on

#### Requirement Traceability
- Related Requirement IDs: R4, C4, C5, C9, C18
- Related SA Artifacts: FSM rows T4, T24, T25, T23, T17, T21 (`dfd_level_4_fsm.md`); Event List: Left/Right Sensor Input, Tick, Direction, Clean; Process Specifications: 1.2 Left Sensor Interface, 1.3 Right Sensor Interface, 1.5 Determine Obstacle Location, 2.1.1 Controller, 2.1.4 Turn Right, 2.1.5 Move Backward, 2.2 Motor Interface, 2.3 Cleaner Interface

#### Expected Behavior
- Tick 3: boxed in at (4,2): Backward + clean_off.
- Ticks 6-7: minimum reached but both sides blocked: keep reversing.
- Tick 8: at (4,7) L = true (3,7), R = false (5,7): Disable Move Backward, Direction = Turn Right (C9, C4).
- Ticks 8-10: turning right; heading E at end of tick 10.
- Tick 11: Forward + clean_on heading E.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 3, 6, 7, 8, 10, 11

### ST-08 - Custom start on the left wall, right turn in the corner

#### Initial Configuration
- Grid: 10x10
- RVC Start: (0,4) (custom start)
- Initial Direction: N
- Obstacles: none
- Dust: none
- Maximum Ticks: 20

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . . . . . . .
  y2 . . . . . . . . . .
  y3 . . . . . . . . . .
  y4 ^ . . . . . . . . .
  y5 . . . . . . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-05 - Side-only obstacle with front clear causes no reaction
- Essential Function 2: EF-03 - Front obstacle detection by interrupt (walls and obstacles); false only updates the store
- Essential Function 3: EF-07 - Avoidance: turn right when the left is blocked
- Essential Function 4: EF-08 - 3-tick turn, then resume forward with clean_on

#### Requirement Traceability
- Related Requirement IDs: R2, R3, C1, C4, C5, C13
- Related SA Artifacts: FSM rows T5, T3, T17, T21 (`dfd_level_4_fsm.md`); Event List: Front Sensor Input, Left Sensor Input, Direction, Clean; Process Specifications: 1.1 Front Sensor Interface, 1.2 Left Sensor Interface, 1.5 Determine Obstacle Location, 2.1.1 Controller, 2.1.4 Turn Right, 2.2 Motor Interface, 2.3 Cleaner Interface

#### Expected Behavior
- Start is not the centre (custom start (0,4) facing N).
- Ticks 1-4: the wall on the left gives L = true each tick; no reaction (C13). The RVC moves to (0,0).
- Tick 5: F = true (wall) and L = true (wall), R = false: Direction = Turn Right, Clean = clean_off.
- Tick 8: Forward + clean_on heading E; L stays true along the top wall without reaction.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 1, 4, 5, 8

### ST-09 - Single dust spot: 3-tick power-up, then normal cleaning

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: S
- Obstacles: none
- Dust: (4,5)
- Maximum Ticks: 20

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . . . . . . .
  y2 . . . . . . . . . .
  y3 . . . . . . . . . .
  y4 . . . . v . . . . .
  y5 . . . . * . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-04 - Periodic left/right/dust sampling, once per tick
- Essential Function 2: EF-12 - Dust: power-up (clean_up) for 3 ticks, then back to clean_on
- Essential Function 3: EF-02 - Straight forward movement with normal cleaning (clean_on)
- Essential Function 4: EF-17 - Direction and Clean are sent only when their value changes; signals only on transitions

#### Requirement Traceability
- Related Requirement IDs: R1, R5, C2, C6, C7, C12, C28
- Related SA Artifacts: FSM rows T6, T10, T11, T2 (`dfd_level_4_fsm.md`); Event List: Dust Sensor Input, Tick, Clean; Process Specifications: 1.4 Dust Sensor Interface, 1.6 Determine Dust Existence, 2.1.1 Controller, 2.1.8 Power-Up Clean, 2.1.6 Clean Normal, 2.3 Cleaner Interface

#### Expected Behavior
- Tick 2: at (4,5) dust = true and front clear: Clean = clean_up; no Direction write (motion continues forward) (R5).
- Ticks 2-4: power-up lasts 3 ticks (power-up counter 0, 1, 2) (C6).
- Tick 5: power-up over: Clean = clean_on (C28).
- Tick 6: wall ahead at (4,9): Turn Left + clean_off.
- The dust at (4,5) is gone after tick 2.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 1, 2, 4, 5, 6

### ST-10 - Dust trail: no restart during power-up, new power-up afterwards

#### Initial Configuration
- Grid: 10x10
- RVC Start: (0,4) (custom start)
- Initial Direction: E
- Obstacles: none
- Dust: (1,4), (2,4), (3,4), (4,4), (5,4)
- Maximum Ticks: 20

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . . . . . . .
  y2 . . . . . . . . . .
  y3 . . . . . . . . . .
  y4 > * * * * * . . . .
  y5 . . . . . . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-12 - Dust: power-up (clean_up) for 3 ticks, then back to clean_on
- Essential Function 2: EF-13 - Dust during an active power-up does not restart it; dust after the end starts a new one
- Essential Function 3: EF-04 - Periodic left/right/dust sampling, once per tick
- Essential Function 4: EF-17 - Direction and Clean are sent only when their value changes; signals only on transitions

#### Requirement Traceability
- Related Requirement IDs: R5, C6, C12, C14, C17, C28
- Related SA Artifacts: FSM rows T6, T10, T11, T6, T2 (`dfd_level_4_fsm.md`); Event List: Dust Sensor Input, Tick, Clean; Process Specifications: 1.4 Dust Sensor Interface, 1.6 Determine Dust Existence, 2.1.1 Controller, 2.1.8 Power-Up Clean, 2.1.6 Clean Normal, 2.3 Cleaner Interface

#### Expected Behavior
- Custom start (0,4) facing E.
- Tick 2: dust at (1,4): Clean = clean_up.
- Ticks 3-4: dust is detected again but the power-up is not restarted: no Clean write, counter keeps counting (C14).
- Tick 5: power-up ends: Clean = clean_on, even though dust is detected on that tick (C28).
- Tick 6: dust at (5,4) on a later tick: a NEW power-up starts, Clean = clean_up (C28).
- Tick 9: Clean = clean_on. Tick 10: wall at (9,4): Turn Left + clean_off.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 2, 3, 4, 5, 6, 9, 10

### ST-11 - Obstacle cancels the power-up

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (4,2)
- Dust: (4,4), (3,3)
- Maximum Ticks: 20

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . . . . . . .
  y2 . . . . # . . . . .
  y3 . . . * . . . . . .
  y4 . . . . ^ . . . . .
  y5 . . . . . . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-12 - Dust: power-up (clean_up) for 3 ticks, then back to clean_on
- Essential Function 2: EF-14 - An obstacle during power-up cancels the power-up
- Essential Function 3: EF-06 - Avoidance: turn left, left has priority when both sides are free
- Essential Function 4: EF-09 - Cleaning stopped (clean_off) only while turning or reversing
- Essential Function 5: EF-18 - Front obstacle has priority over dust

#### Requirement Traceability
- Related Requirement IDs: R3, R5, C4, C6, C8, C11, C25, C28
- Related SA Artifacts: FSM rows T6, T7, T12, T16, T10 (`dfd_level_4_fsm.md`); Event List: Dust Sensor Input, Front Sensor Input, Clean, Direction; Process Specifications: 1.1 Front Sensor Interface, 1.4 Dust Sensor Interface, 2.1.1 Controller, 2.1.3 Turn Left, 2.1.6 Clean Normal, 2.1.7 Clean Off, 2.1.8 Power-Up Clean, 2.2 Motor Interface, 2.3 Cleaner Interface

#### Expected Behavior
- Tick 1: dust on the start cell: Clean = clean_up.
- Tick 2: at (4,3) obstacle (4,2) ahead: Disable Move Forward, Clean = clean_off, Turn Left. The power-up is cancelled after 1 tick (C11).
- Tick 5: turn done: Forward + clean_on (the cancelled power-up is not resumed).
- Tick 6: dust at (3,3): a new power-up, Clean = clean_up.
- Tick 9: wall ahead at (0,3): the front is checked first, so Turn Left + clean_off (front before the power-up end).

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 1, 2, 5, 6, 9

### ST-12 - Dust on the turning cell is ignored; next-tick dust is acted on

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (4,1)
- Dust: (4,2), (3,2)
- Maximum Ticks: 20

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . # . . . . .
  y2 . . . * * . . . . .
  y3 . . . . . . . . . .
  y4 . . . . ^ . . . . .
  y5 . . . . . . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-18 - Front obstacle has priority over dust
- Essential Function 2: EF-15 - Dust ignored while turning/reversing and on the tick a turn/reverse ends
- Essential Function 3: EF-06 - Avoidance: turn left, left has priority when both sides are free
- Essential Function 4: EF-12 - Dust: power-up (clean_up) for 3 ticks, then back to clean_on

#### Requirement Traceability
- Related Requirement IDs: R3, R5, C10, C29, C4, C6
- Related SA Artifacts: FSM rows T2, T12, T16, T6 (`dfd_level_4_fsm.md`); Event List: Front Sensor Input, Dust Sensor Input, Direction, Clean; Process Specifications: 1.1 Front Sensor Interface, 1.4 Dust Sensor Interface, 1.6 Determine Dust Existence, 2.1.1 Controller, 2.1.3 Turn Left, 2.1.7 Clean Off, 2.1.8 Power-Up Clean, 2.3 Cleaner Interface

#### Expected Behavior
- Tick 3: at (4,2) F = true and dust = true: front first -> Turn Left + clean_off; no clean_up.
- Ticks 4-5: dust still detected while turning: ignored (C10).
- Tick 6: turn ends with the front clear: Forward + clean_on; dust on this tick is ignored (C29).
- Tick 7: at (3,2) dust detected: Clean = clean_up (acted on from the next tick, C29).

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 3, 4, 5, 6, 7

### ST-13 - Dust ignored while reversing and on the reverse/turn end ticks

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (4,1), (3,2), (5,2)
- Dust: (4,2), (4,5)
- Maximum Ticks: 20

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . # . . . . .
  y2 . . . # * # . . . .
  y3 . . . . . . . . . .
  y4 . . . . ^ . . . . .
  y5 . . . . * . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-10 - Boxed in (F, L, R): move backward for at least 3 ticks, continue while both sides are blocked
- Essential Function 2: EF-11 - Reverse ends with a turn, left first
- Essential Function 3: EF-15 - Dust ignored while turning/reversing and on the tick a turn/reverse ends
- Essential Function 4: EF-18 - Front obstacle has priority over dust
- Essential Function 5: EF-09 - Cleaning stopped (clean_off) only while turning or reversing

#### Requirement Traceability
- Related Requirement IDs: R4, R5, C8, C9, C10, C29
- Related SA Artifacts: FSM rows T4, T24, T22, T12, T16 (`dfd_level_4_fsm.md`); Event List: Front/Left/Right Sensor Input, Dust Sensor Input, Direction, Clean; Process Specifications: 1.4 Dust Sensor Interface, 1.6 Determine Dust Existence, 2.1.1 Controller, 2.1.5 Move Backward, 2.1.3 Turn Left, 2.1.7 Clean Off, 2.3 Cleaner Interface

#### Expected Behavior
- Tick 3: at (4,2) F, L, R and dust are all true: Backward + clean_off (front before dust). Dust (4,2) stays (cleaning is off).
- Tick 6: at (4,5) dust = true, reverse ends with left free: Turn Left; no Clean write, dust ignored (C29).
- Ticks 7-8: dust still detected while turning: ignored (C10).
- Tick 9: turn ends: Forward + clean_on; dust ignored on this tick (C29). No clean_up occurs in this run window.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 3, 6, 7, 8, 9

### ST-14 - Power-up cancelled by a right turn

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (4,2), (3,3)
- Dust: (4,4)
- Maximum Ticks: 15

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . . . . . . .
  y2 . . . . # . . . . .
  y3 . . . # . . . . . .
  y4 . . . . ^ . . . . .
  y5 . . . . . . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-12 - Dust: power-up (clean_up) for 3 ticks, then back to clean_on
- Essential Function 2: EF-14 - An obstacle during power-up cancels the power-up
- Essential Function 3: EF-07 - Avoidance: turn right when the left is blocked
- Essential Function 4: EF-09 - Cleaning stopped (clean_off) only while turning or reversing

#### Requirement Traceability
- Related Requirement IDs: R3, R5, C6, C8, C11
- Related SA Artifacts: FSM rows T6, T8, T17, T21 (`dfd_level_4_fsm.md`); Event List: Dust Sensor Input, Front/Left Sensor Input, Direction, Clean; Process Specifications: 1.1 Front Sensor Interface, 1.4 Dust Sensor Interface, 2.1.1 Controller, 2.1.4 Turn Right, 2.1.7 Clean Off, 2.1.8 Power-Up Clean, 2.2 Motor Interface, 2.3 Cleaner Interface

#### Expected Behavior
- Tick 1: dust on the start cell: Clean = clean_up.
- Tick 2: at (4,3) F = true, L = true (3,3), R = false: Disable Move Forward, clean_off, Turn Right (C11).
- Tick 5: Forward + clean_on heading E.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 1, 2, 4, 5

### ST-15 - Power-up cancelled by a dead end (reverse)

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (4,1), (3,2), (5,2)
- Dust: (4,4)
- Maximum Ticks: 15

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . # . . . . .
  y2 . . . # . # . . . .
  y3 . . . . . . . . . .
  y4 . . . . ^ . . . . .
  y5 . . . . . . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-12 - Dust: power-up (clean_up) for 3 ticks, then back to clean_on
- Essential Function 2: EF-14 - An obstacle during power-up cancels the power-up
- Essential Function 3: EF-10 - Boxed in (F, L, R): move backward for at least 3 ticks, continue while both sides are blocked
- Essential Function 4: EF-11 - Reverse ends with a turn, left first

#### Requirement Traceability
- Related Requirement IDs: R4, R5, C9, C11, C6
- Related SA Artifacts: FSM rows T6, T10, T9, T24, T22, T16 (`dfd_level_4_fsm.md`); Event List: Dust Sensor Input, Front/Left/Right Sensor Input, Direction, Clean; Process Specifications: 1.4 Dust Sensor Interface, 2.1.1 Controller, 2.1.5 Move Backward, 2.1.3 Turn Left, 2.1.7 Clean Off, 2.1.8 Power-Up Clean, 2.2 Motor Interface, 2.3 Cleaner Interface

#### Expected Behavior
- Tick 1: dust on the start cell: Clean = clean_up.
- Tick 2: power-up continues (front clear).
- Tick 3: at (4,2) F, L, R all true: Disable Move Forward, clean_off, Enable Move Backward (C11, R4).
- Tick 6: reverse of 3 ticks done, left free: Turn Left.
- Tick 9: Forward + clean_on.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 1, 2, 3, 6, 9

### ST-16 - Fully enclosed start cell with dust

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (4,3), (3,4), (5,4), (4,5)
- Dust: (4,4)
- Maximum Ticks: 15

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . . . . . . .
  y2 . . . . . . . . . .
  y3 . . . . # . . . . .
  y4 . . . # ^ # . . . .
  y5 . . . . # . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-18 - Front obstacle has priority over dust
- Essential Function 2: EF-10 - Boxed in (F, L, R): move backward for at least 3 ticks, continue while both sides are blocked
- Essential Function 3: EF-15 - Dust ignored while turning/reversing and on the tick a turn/reverse ends
- Essential Function 4: EF-09 - Cleaning stopped (clean_off) only while turning or reversing

#### Requirement Traceability
- Related Requirement IDs: R4, R5, C9, C10, C8
- Related SA Artifacts: FSM rows T4, T24, T25 (`dfd_level_4_fsm.md`); Event List: Front/Left/Right Sensor Input, Dust Sensor Input, Direction, Clean; Process Specifications: 1.1 Front Sensor Interface, 1.6 Determine Dust Existence, 2.1.1 Controller, 2.1.5 Move Backward, 2.1.7 Clean Off, 2.2 Motor Interface, 2.3 Cleaner Interface

#### Expected Behavior
- Tick 1: F (interrupt), L, R and dust are all true: front first -> Backward + clean_off; no clean_up.
- From tick 4 the 3-tick minimum is reached, but L and R stay blocked, so the RVC keeps reversing for the whole run (C9). Dust stays ignored (C10) and is never cleaned.
- The cell behind is blocked: the simulator flags a collision on every tick and the pose does not change. The requirements define no rear sensor; record what you observe.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 1, 3, 4, 15

### ST-17 - Front blocked on tick 1 with dust on the start cell

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: E
- Obstacles: (5,4)
- Dust: (4,4)
- Maximum Ticks: 15

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . . . . . . .
  y2 . . . . . . . . . .
  y3 . . . . . . . . . .
  y4 . . . . > # . . . .
  y5 . . . . . . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-03 - Front obstacle detection by interrupt (walls and obstacles); false only updates the store
- Essential Function 2: EF-18 - Front obstacle has priority over dust
- Essential Function 3: EF-15 - Dust ignored while turning/reversing and on the tick a turn/reverse ends
- Essential Function 4: EF-06 - Avoidance: turn left, left has priority when both sides are free
- Essential Function 5: EF-04 - Periodic left/right/dust sampling, once per tick

#### Requirement Traceability
- Related Requirement IDs: R3, R5, C1, C10, C20, C23, C29
- Related SA Artifacts: FSM rows T2, T12, T16, T5 (`dfd_level_4_fsm.md`); Event List: Front Sensor Input, Dust Sensor Input, Direction, Clean; Process Specifications: 1.1 Front Sensor Interface, 1.4 Dust Sensor Interface, 1.5 Determine Obstacle Location, 1.6 Determine Dust Existence, 2.1.1 Controller, 2.1.3 Turn Left, 2.1.7 Clean Off, 2.1.6 Clean Normal

#### Expected Behavior
- Tick 1: the first sensing tick: front interrupt F = true (5,4), dust = true: front first -> Turn Left + clean_off (no power-up).
- Ticks 2-3: dust ignored while turning (C10). Heading N at end of tick 3.
- Tick 4: front interrupt F = false: Forward + clean_on; dust on this tick ignored (C29).
- No clean_up at any tick.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 1, 2, 3, 4

### ST-18 - Successive left turns around a set of obstacles

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (4,1), (2,2), (3,6)
- Dust: none
- Maximum Ticks: 30

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . # . . . . .
  y2 . . # . . . . . . .
  y3 . . . . . . . . . .
  y4 . . . . ^ . . . . .
  y5 . . . . . . . . . .
  y6 . . . # . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-03 - Front obstacle detection by interrupt (walls and obstacles); false only updates the store
- Essential Function 2: EF-06 - Avoidance: turn left, left has priority when both sides are free
- Essential Function 3: EF-08 - 3-tick turn, then resume forward with clean_on
- Essential Function 4: EF-09 - Cleaning stopped (clean_off) only while turning or reversing
- Essential Function 5: EF-17 - Direction and Clean are sent only when their value changes; signals only on transitions

#### Requirement Traceability
- Related Requirement IDs: R3, C1, C4, C5, C8, C12
- Related SA Artifacts: FSM rows T2, T12, T16, T5 (`dfd_level_4_fsm.md`); Event List: Front Sensor Input, Direction, Clean; Process Specifications: 1.1 Front Sensor Interface, 2.1.1 Controller, 2.1.2 Move Forward, 2.1.3 Turn Left, 2.1.6 Clean Normal, 2.1.7 Clean Off, 2.2 Motor Interface, 2.3 Cleaner Interface

#### Expected Behavior
- Tick 3: (4,1) ahead at (4,2): Turn Left. Tick 6: Forward heading W.
- Tick 7: (2,2) ahead at (3,2): Turn Left again. Tick 10: Forward heading S.
- Tick 13: (3,6) ahead at (3,5): Turn Left. Tick 16: Forward heading E.
- Tick 22: wall ahead at (9,5): Turn Left.
- Every turn lasts 3 ticks with clean_off and is followed by Forward + clean_on.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 3, 6, 7, 10, 13, 16, 22

### ST-19 - Side obstacles during an active power-up

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: S
- Obstacles: (5,5), (3,6)
- Dust: (4,5), (4,6)
- Maximum Ticks: 15

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . . . . . . .
  y2 . . . . . . . . . .
  y3 . . . . . . . . . .
  y4 . . . . v . . . . .
  y5 . . . . * # . . . .
  y6 . . . # * . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-05 - Side-only obstacle with front clear causes no reaction
- Essential Function 2: EF-12 - Dust: power-up (clean_up) for 3 ticks, then back to clean_on
- Essential Function 3: EF-13 - Dust during an active power-up does not restart it; dust after the end starts a new one
- Essential Function 4: EF-04 - Periodic left/right/dust sampling, once per tick

#### Requirement Traceability
- Related Requirement IDs: R5, C6, C13, C14, C17
- Related SA Artifacts: FSM rows T6, T10, T11, T2 (`dfd_level_4_fsm.md`); Event List: Left/Right Sensor Input, Dust Sensor Input, Clean; Process Specifications: 1.2 Left Sensor Interface, 1.3 Right Sensor Interface, 1.4 Dust Sensor Interface, 2.1.1 Controller, 2.1.8 Power-Up Clean, 2.1.6 Clean Normal, 2.3 Cleaner Interface

#### Expected Behavior
- Heading S: left is E, right is W.
- Tick 2: at (4,5) dust = true, L = true (5,5): Clean = clean_up; the side obstacle causes no reaction (C13).
- Tick 3: at (4,6) dust = true, R = true (3,6): no restart (C14) and no turn (C13).
- Tick 5: Clean = clean_on. Tick 6: wall at (4,9): Turn Left + clean_off.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 2, 3, 5, 6

### ST-20 - Furnished room (integration, 100 ticks)

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (0,7), (0,8), (3,9), (4,2), (4,9), (5,8), (7,8), (8,1), (8,3), (8,4), (8,5), (9,0)
- Dust: (1,8), (2,5), (3,6), (5,5), (6,1), (6,3), (6,8), (9,3)
- Maximum Ticks: 100

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . #
  y1 . . . . . . * . # .
  y2 . . . . # . . . . .
  y3 . . . . . . * . # *
  y4 . . . . ^ . . . # .
  y5 . . * . . * . . # .
  y6 . . . * . . . . . .
  y7 # . . . . . . . . .
  y8 # * . . . # * # . .
  y9 . . . # # . . . . .
```

#### Test Objectives
- Essential Function 1: EF-02 - Straight forward movement with normal cleaning (clean_on)
- Essential Function 2: EF-06 - Avoidance: turn left, left has priority when both sides are free
- Essential Function 3: EF-07 - Avoidance: turn right when the left is blocked
- Essential Function 4: EF-10 - Boxed in (F, L, R): move backward for at least 3 ticks, continue while both sides are blocked
- Essential Function 5: EF-11 - Reverse ends with a turn, left first
- Essential Function 6: EF-12 - Dust: power-up (clean_up) for 3 ticks, then back to clean_on
- Essential Function 7: EF-14 - An obstacle during power-up cancels the power-up
- Essential Function 8: EF-09 - Cleaning stopped (clean_off) only while turning or reversing

#### Requirement Traceability
- Related Requirement IDs: R1, R2, R3, R4, R5, C4, C5, C6, C8, C9, C11, C13, C14
- Related SA Artifacts: FSM rows T2, T3, T4, T6, T7, T9, T11, T16, T21, T22, T23, T25 (`dfd_level_4_fsm.md`); Event List: All input and output events; Process Specifications: 1.1 Front Sensor Interface, 1.2 Left Sensor Interface, 1.3 Right Sensor Interface, 1.4 Dust Sensor Interface, 2.1.1 Controller, 2.1.2 Move Forward, 2.1.3 Turn Left, 2.1.4 Turn Right, 2.1.5 Move Backward, 2.1.6 Clean Normal, 2.1.7 Clean Off, 2.1.8 Power-Up Clean, 2.2 Motor Interface, 2.3 Cleaner Interface

#### Expected Behavior
- At every tick where F becomes true while moving forward: left free -> Turn Left; left blocked and right free -> Turn Right; all three blocked -> Backward. Cleaning is clean_off during every turn and reverse.
- Every turn lasts 3 ticks; a reverse lasts at least 3 ticks and continues while both sides are blocked, then turns (left first).
- Dust while moving forward with clean_on starts a 3-tick clean_up; further dust does not restart it; an obstacle cancels it.
- A side obstacle alone never changes the movement.
- Direction / Clean are written only when they change.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested all avoidance and dust events

### ST-21 - Corner start: front and left walls on tick 1

#### Initial Configuration
- Grid: 10x10
- RVC Start: (0,0) (custom start)
- Initial Direction: N
- Obstacles: none
- Dust: none
- Maximum Ticks: 15

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 ^ . . . . . . . . .
  y1 . . . . . . . . . .
  y2 . . . . . . . . . .
  y3 . . . . . . . . . .
  y4 . . . . . . . . . .
  y5 . . . . . . . . . .
  y6 . . . . . . . . . .
  y7 . . . . . . . . . .
  y8 . . . . . . . . . .
  y9 . . . . . . . . . .
```

#### Test Objectives
- Essential Function 1: EF-01 - Start-up outputs: Direction = Forward and Clean = clean_on sent once; stores false
- Essential Function 2: EF-03 - Front obstacle detection by interrupt (walls and obstacles); false only updates the store
- Essential Function 3: EF-07 - Avoidance: turn right when the left is blocked
- Essential Function 4: EF-04 - Periodic left/right/dust sampling, once per tick
- Essential Function 5: EF-05 - Side-only obstacle with front clear causes no reaction

#### Requirement Traceability
- Related Requirement IDs: C1, C4, C13, C16, C20, C21, C22, C23
- Related SA Artifacts: FSM rows T1, T3, T17, T21, T5 (`dfd_level_4_fsm.md`); Event List: Front Sensor Input, Left Sensor Input, Direction, Clean; Process Specifications: 1.1 Front Sensor Interface, 1.2 Left Sensor Interface, 1.5 Determine Obstacle Location, 2.1.1 Controller, 2.1.4 Turn Right, 2.2 Motor Interface, 2.3 Cleaner Interface

#### Expected Behavior
- Tick 0: Forward + clean_on written once; front store false (C21) although a wall is ahead.
- Tick 1: the front interrupt sets F = true; L = true is sampled on this tick (overwriting the start-up false, C23); R free: Turn Right + clean_off.
- Tick 4: Forward + clean_on heading E; L = true along the top wall causes no reaction.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested 0, 1, 3, 4

### ST-22 - Dusty room (integration, 100 ticks)

#### Initial Configuration
- Grid: 10x10
- RVC Start: (4,4) (default centre)
- Initial Direction: N
- Obstacles: (1,0), (1,3), (2,3), (3,8), (5,6), (9,1)
- Dust: (0,4), (0,7), (1,5), (2,1), (2,5), (3,0), (4,9), (5,0), (5,4), (5,5), (5,7), (6,2), (6,5), (7,7), (7,8), (8,3), (8,6), (9,4)
- Maximum Ticks: 100

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . # . * . * . . . .
  y1 . . * . . . . . . #
  y2 . . . . . . * . . .
  y3 . # # . . . . . * .
  y4 * . . . ^ * . . . *
  y5 . * * . . * * . . .
  y6 . . . . . # . . * .
  y7 * . . . . * . * . .
  y8 . . . # . . . * . .
  y9 . . . . * . . . . .
```

#### Test Objectives
- Essential Function 1: EF-12 - Dust: power-up (clean_up) for 3 ticks, then back to clean_on
- Essential Function 2: EF-13 - Dust during an active power-up does not restart it; dust after the end starts a new one
- Essential Function 3: EF-14 - An obstacle during power-up cancels the power-up
- Essential Function 4: EF-15 - Dust ignored while turning/reversing and on the tick a turn/reverse ends
- Essential Function 5: EF-06 - Avoidance: turn left, left has priority when both sides are free
- Essential Function 6: EF-07 - Avoidance: turn right when the left is blocked
- Essential Function 7: EF-10 - Boxed in (F, L, R): move backward for at least 3 ticks, continue while both sides are blocked

#### Requirement Traceability
- Related Requirement IDs: R1, R3, R4, R5, C6, C10, C11, C14, C28, C29
- Related SA Artifacts: FSM rows T2, T3, T4, T6, T7, T8, T11, T16, T21, T23, T25 (`dfd_level_4_fsm.md`); Event List: All input and output events; Process Specifications: 1.4 Dust Sensor Interface, 1.6 Determine Dust Existence, 2.1.1 Controller, 2.1.3 Turn Left, 2.1.4 Turn Right, 2.1.5 Move Backward, 2.1.6 Clean Normal, 2.1.7 Clean Off, 2.1.8 Power-Up Clean, 2.3 Cleaner Interface

#### Expected Behavior
- Each dust detection while in FwdNormal starts a 3-tick clean_up; dust during a power-up does not extend it; dust on a later tick after it ends starts a new one.
- Dust detected while turning or reversing, or on the tick a turn/reverse ends, causes no clean_up.
- A front obstacle during clean_up cancels it (clean_off, then clean_on after the turn).
- Avoidance follows left-first; reverse when boxed in.
- Dust cells passed with clean_off stay on the map.

#### Human Evaluation
- Result: Pending (PASS / FAIL / INCONCLUSIVE)
- Evaluator:
- Observations:
- Related Tick(s): suggested all avoidance and dust events

## 3. Essential System Functions

| ID | Function | Requirements | Scenarios |
|---|---|---|---|
| EF-01 | Start-up outputs: Direction = Forward and Clean = clean_on sent once; stores false | C16, C21, C22, C27 | ST-01, ST-21 |
| EF-02 | Straight forward movement with normal cleaning (clean_on) | R1, R2 | ST-01, ST-02, ST-09, ST-20 |
| EF-03 | Front obstacle detection by interrupt (walls and obstacles); false only updates the store | C1, C3, C19, C20 | ST-01, ST-03, ST-04, ST-08, ST-17, ST-18, ST-21 |
| EF-04 | Periodic left/right/dust sampling, once per tick | C2, C17, C23 | ST-02, ST-09, ST-10, ST-17, ST-19, ST-21 |
| EF-05 | Side-only obstacle with front clear causes no reaction | C13 | ST-02, ST-05, ST-08, ST-19, ST-21 |
| EF-06 | Avoidance: turn left, left has priority when both sides are free | R3, C4, C18 | ST-01, ST-02, ST-03, ST-11, ST-12, ST-17, ST-18, ST-20, ST-22 |
| EF-07 | Avoidance: turn right when the left is blocked | R3, C4, C18 | ST-04, ST-07, ST-08, ST-14, ST-20, ST-21, ST-22 |
| EF-08 | 3-tick turn, then resume forward with clean_on | R3, C5, C7 | ST-01, ST-03, ST-04, ST-07, ST-08, ST-18 |
| EF-09 | Cleaning stopped (clean_off) only while turning or reversing | R3, C7, C8 | ST-01, ST-03, ST-04, ST-05, ST-06, ST-11, ST-13, ST-14, ST-16, ST-18, ST-20 |
| EF-10 | Boxed in (F, L, R): move backward for at least 3 ticks, continue while both sides are blocked | R4, C9 | ST-05, ST-06, ST-07, ST-13, ST-15, ST-16, ST-20, ST-22 |
| EF-11 | Reverse ends with a turn, left first | R4, C4, C9 | ST-05, ST-06, ST-07, ST-13, ST-15, ST-20 |
| EF-12 | Dust: power-up (clean_up) for 3 ticks, then back to clean_on | R5, C6, C7, C28 | ST-09, ST-10, ST-11, ST-12, ST-14, ST-15, ST-19, ST-20, ST-22 |
| EF-13 | Dust during an active power-up does not restart it; dust after the end starts a new one | C14, C28 | ST-10, ST-19, ST-22 |
| EF-14 | An obstacle during power-up cancels the power-up | C11 | ST-11, ST-14, ST-15, ST-20, ST-22 |
| EF-15 | Dust ignored while turning/reversing and on the tick a turn/reverse ends | C10, C29 | ST-12, ST-13, ST-16, ST-17, ST-22 |
| EF-16 | Front interrupt during a turn or reverse is not acted on | C15, C19 | ST-03, ST-05, ST-06 |
| EF-17 | Direction and Clean are sent only when their value changes; signals only on transitions | C12, C24, C25, C26 | ST-01, ST-02, ST-05, ST-09, ST-10, ST-18 |
| EF-18 | Front obstacle has priority over dust | R3, R5 | ST-11, ST-12, ST-13, ST-16, ST-17 |

## 4. Test Execution and Evaluation

1. `python simulator/server.py`, open http://localhost:8000.
2. Select a scenario. Its pre-generated trace loads; **Run C controller** re-executes the real controller from reset.
3. Use Start / Pause / Step / Step back / Reset / Restart and the history table to inspect every tick.
4. Assign PASS / FAIL / INCONCLUSIVE, enter evaluator, observations and related ticks, and press **Save evaluation**. The verdict and the trace are stored in `simulator/results/` and `artifacts/SystemTest/test_results.md` is regenerated.

## 5. Requirement Traceability

| Scenario ID | Essential Functions | Requirement IDs | SA Artifacts | Human Result |
|---|---|---|---|---|
| ST-01 | EF-01, EF-02, EF-03, EF-06, EF-08, EF-09, EF-17 | R1, R2, R3, C1, C3, C4, C5, C8, C12, C13, C16, C21, C22, C27 | FSM T1, T2, T5, T12, T16; PS 1.1, 1.5, 2.1.1, 2.1.2, 2.1.3, 2.1.6, 2.1.7, 2.2, 2.3 | Pending |
| ST-02 | EF-02, EF-04, EF-05, EF-06, EF-17 | R2, R3, C2, C4, C12, C13, C17 | FSM T5, T2, T12, T16; PS 1.2, 1.3, 1.5, 2.1.1, 2.1.2, 2.1.3, 2.2 | Pending |
| ST-03 | EF-03, EF-06, EF-08, EF-09, EF-16 | R3, C1, C3, C4, C5, C8, C15, C18, C19, C20, C25 | FSM T2, T12, T16; PS 1.1, 1.5, 2.1.1, 2.1.2, 2.1.3, 2.1.6, 2.1.7, 2.2, 2.3 | Pending |
| ST-04 | EF-03, EF-07, EF-08, EF-09 | R3, C1, C4, C5, C8, C18 | FSM T3, T17, T21, T2; PS 1.1, 1.2, 1.3, 1.5, 2.1.1, 2.1.4, 2.1.6, 2.1.7, 2.2, 2.3 | Pending |
| ST-05 | EF-05, EF-10, EF-11, EF-09, EF-16, EF-17 | R4, C4, C8, C9, C13, C15, C19, C24, C25, C26 | FSM T4, T24, T22, T12, T16; PS 1.1, 1.5, 2.1.1, 2.1.2, 2.1.3, 2.1.5, 2.1.7, 2.1.6, 2.2, 2.3 | Pending |
| ST-06 | EF-10, EF-11, EF-16, EF-09 | R4, C9, C15, C19, C4, C8 | FSM T4, T24, T25, T22; PS 1.1, 1.2, 1.3, 1.5, 2.1.1, 2.1.3, 2.1.5, 2.2 | Pending |
| ST-07 | EF-10, EF-11, EF-07, EF-08 | R4, C4, C5, C9, C18 | FSM T4, T24, T25, T23, T17, T21; PS 1.2, 1.3, 1.5, 2.1.1, 2.1.4, 2.1.5, 2.2, 2.3 | Pending |
| ST-08 | EF-05, EF-03, EF-07, EF-08 | R2, R3, C1, C4, C5, C13 | FSM T5, T3, T17, T21; PS 1.1, 1.2, 1.5, 2.1.1, 2.1.4, 2.2, 2.3 | Pending |
| ST-09 | EF-04, EF-12, EF-02, EF-17 | R1, R5, C2, C6, C7, C12, C28 | FSM T6, T10, T11, T2; PS 1.4, 1.6, 2.1.1, 2.1.8, 2.1.6, 2.3 | Pending |
| ST-10 | EF-12, EF-13, EF-04, EF-17 | R5, C6, C12, C14, C17, C28 | FSM T6, T10, T11, T6, T2; PS 1.4, 1.6, 2.1.1, 2.1.8, 2.1.6, 2.3 | Pending |
| ST-11 | EF-12, EF-14, EF-06, EF-09, EF-18 | R3, R5, C4, C6, C8, C11, C25, C28 | FSM T6, T7, T12, T16, T10; PS 1.1, 1.4, 2.1.1, 2.1.3, 2.1.6, 2.1.7, 2.1.8, 2.2, 2.3 | Pending |
| ST-12 | EF-18, EF-15, EF-06, EF-12 | R3, R5, C10, C29, C4, C6 | FSM T2, T12, T16, T6; PS 1.1, 1.4, 1.6, 2.1.1, 2.1.3, 2.1.7, 2.1.8, 2.3 | Pending |
| ST-13 | EF-10, EF-11, EF-15, EF-18, EF-09 | R4, R5, C8, C9, C10, C29 | FSM T4, T24, T22, T12, T16; PS 1.4, 1.6, 2.1.1, 2.1.5, 2.1.3, 2.1.7, 2.3 | Pending |
| ST-14 | EF-12, EF-14, EF-07, EF-09 | R3, R5, C6, C8, C11 | FSM T6, T8, T17, T21; PS 1.1, 1.4, 2.1.1, 2.1.4, 2.1.7, 2.1.8, 2.2, 2.3 | Pending |
| ST-15 | EF-12, EF-14, EF-10, EF-11 | R4, R5, C9, C11, C6 | FSM T6, T10, T9, T24, T22, T16; PS 1.4, 2.1.1, 2.1.5, 2.1.3, 2.1.7, 2.1.8, 2.2, 2.3 | Pending |
| ST-16 | EF-18, EF-10, EF-15, EF-09 | R4, R5, C9, C10, C8 | FSM T4, T24, T25; PS 1.1, 1.6, 2.1.1, 2.1.5, 2.1.7, 2.2, 2.3 | Pending |
| ST-17 | EF-03, EF-18, EF-15, EF-06, EF-04 | R3, R5, C1, C10, C20, C23, C29 | FSM T2, T12, T16, T5; PS 1.1, 1.4, 1.5, 1.6, 2.1.1, 2.1.3, 2.1.7, 2.1.6 | Pending |
| ST-18 | EF-03, EF-06, EF-08, EF-09, EF-17 | R3, C1, C4, C5, C8, C12 | FSM T2, T12, T16, T5; PS 1.1, 2.1.1, 2.1.2, 2.1.3, 2.1.6, 2.1.7, 2.2, 2.3 | Pending |
| ST-19 | EF-05, EF-12, EF-13, EF-04 | R5, C6, C13, C14, C17 | FSM T6, T10, T11, T2; PS 1.2, 1.3, 1.4, 2.1.1, 2.1.8, 2.1.6, 2.3 | Pending |
| ST-20 | EF-02, EF-06, EF-07, EF-10, EF-11, EF-12, EF-14, EF-09 | R1, R2, R3, R4, R5, C4, C5, C6, C8, C9, C11, C13, C14 | FSM T2, T3, T4, T6, T7, T9, T11, T16, T21, T22, T23, T25; PS 1.1, 1.2, 1.3, 1.4, 2.1.1, 2.1.2, 2.1.3, 2.1.4, 2.1.5, 2.1.6, 2.1.7, 2.1.8, 2.2, 2.3 | Pending |
| ST-21 | EF-01, EF-03, EF-07, EF-04, EF-05 | C1, C4, C13, C16, C20, C21, C22, C23 | FSM T1, T3, T17, T21, T5; PS 1.1, 1.2, 1.5, 2.1.1, 2.1.4, 2.2, 2.3 | Pending |
| ST-22 | EF-12, EF-13, EF-14, EF-15, EF-06, EF-07, EF-10 | R1, R3, R4, R5, C6, C10, C11, C14, C28, C29 | FSM T2, T3, T4, T6, T7, T8, T11, T16, T21, T23, T25; PS 1.4, 1.6, 2.1.1, 2.1.3, 2.1.4, 2.1.5, 2.1.6, 2.1.7, 2.1.8, 2.3 | Pending |

Full matrices: `requirement_traceability.md`.

## 6. Custom Map Testing

In the simulator, **Edit map** lets the user place or remove obstacles and dust, set the start cell and heading, clear the map or reset it to the selected scenario. The map is validated (start inside the grid and not on an obstacle, no cell both obstacle and dust, 1-100 ticks) before the C controller is run from reset. Every tick is recorded and inspectable, a human verdict can be recorded, and **Save as scenario** stores the map in `simulator/scenarios/custom/` so it appears in the scenario list.
