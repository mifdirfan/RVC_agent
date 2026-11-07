# Representative System Test Scenarios

Five scenarios chosen to represent the system test. The full definitions are in `test_scenarios.md`; the complete per-tick state is in `simulator/traces/ST-xx.json` and in the simulator.

- **How to reproduce a view:** start `python simulator/server.py` and open `http://localhost:8000/#scenario=ST-05&tick=3`. Change the scenario and tick in the link as needed.
- **Screenshots:** in `screenshots/`. They were taken with headless Chrome, because Playwright is not installed. Each one shows the simulator at the listed tick: the map, the full state inspector (values that changed since the previous tick are highlighted, with the previous value) and the expected behaviour.
- **Position shown:** the map shows the RVC at its position *after* the tick. The decision of tick *k* is made at "Sensed at", which is the position after tick *k*-1.

**Verdicts are left empty on purpose.** They are for the human evaluator, who enters them in the simulator, and those are recorded in `test_results.md`.

| # | Scenario | Represents |
|---|---|---|
| 1 | ST-02 | Straight cleaning with side walls (C13) |
| 2 | ST-03 | Left-priority turn (C4) |
| 3 | ST-05 | Boxed in: reverse, then turn (C9; Enable/Disable of Move Forward / Move Backward, C24) |
| 4 | ST-10 | Dust power-up, no restart, new power-up (C6, C14, C28) |
| 5 | ST-11 | Obstacle cancels the power-up (C11) |

---

## 1. ST-02: Straight cleaning through a corridor with side walls

Requirements: R2, R3, C2, C4, C12, C13, C17. FSM rows: T5, T2, T12, T16.

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
Start (4,4) facing N. Corridor walls at x = 3 and x = 5, y = 1..6. No dust. 20 ticks.

### Key ticks

| Tick | What to observe | Screenshot |
|---|---|---|
| 1–4 | Left and right sensors are true every tick and the front is clear. The state stays FwdNormal (row T5). Nothing is written to Direction or Clean. | `screenshots/ST-02_tick02.png` |
| 5 | Wall ahead at (4,0) and left (3,0) is free. Front interrupt true, so Turn Left and clean_off (T2). | `screenshots/ST-02_tick05.png` |
| 8 | Turn done: Forward and clean_on, heading W (T16). | — |

### Expected behaviour
- A side obstacle alone causes no reaction while the front is clear. The RVC keeps going straight with clean_on (C13, R2).
- Outputs are written only when they change (C12).
- At the corridor exit the RVC turns left at the wall.

### Human verdict
- Result:
- Evaluator:
- Observations:

---

## 2. ST-03: Left-priority turn at a single front obstacle

Requirements: R3, C1, C3, C4, C5, C8, C15, C18, C19, C20, C25. FSM rows: T2, T12, T16.

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . # . . . . .
  y2 . . . . . . . . . .
  y3 . . . . . . . . . .
  y4 . . . . ^ . . . . .
  y5 . . . . . . . . . .
  ...  (rows 5-9 empty)
```
Start (4,4) facing N. Obstacle at (4,1). No dust. 20 ticks.

### Key ticks

| Tick | What to observe | Screenshot |
|---|---|---|
| 3 | At (4,2): front interrupt raised (F = true), L = false, R = false. Both turns are allowed, so the turn is **left** (C4). Move Forward becomes inactive; Turn Left and clean_off are written (T2). | `screenshots/ST-03_tick03.png` |
| 4–5 | Still turning (turn counter 1, 2; motor progress 2/3, then 3/3). F stays true and is not acted on (C15). | — |
| 6 | Front interrupt false. Forward and clean_on, heading W (T16). | `screenshots/ST-03_tick06.png` |

### Expected behaviour
- On a front obstacle, cleaning stops and the RVC turns aside (R3). When both sides are free, it turns left (C4).
- The turn lasts 3 ticks (C5). After it, the RVC goes forward with cleaning again.

### Human verdict
- Result:
- Evaluator:
- Observations:

---

## 3. ST-05: Boxed-in dead end, reverse 3 ticks, then turn left

Requirements: R4, C4, C8, C9, C13, C15, C19, C24, C25, C26. FSM rows: T4, T24, T22, T12, T16.

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . # # # . . . .
  y2 . . . # . # . . . .
  y3 . . . # . # . . . .
  y4 . . . . ^ . . . . .
  ...  (rows 5-9 empty)
```
Start (4,4) facing N. A dead-end pocket at x = 3..5, y = 1..3. No dust. 20 ticks.

### Key ticks

| Tick | What to observe | Screenshot |
|---|---|---|
| 2 | At (4,3), L and R are true with the front clear: no reaction (C13). | — |
| 3 | At (4,2), F, L and R are all true. The inspector shows: Move Forward active true → **false**, Move Backward active false → **true**, Backward and clean_off written (T4, C24). | `screenshots/ST-05_tick03.png` |
| 3–5 | Moving backward to (4,3), (4,4), (4,5). Reverse counter 0, 1, 2. At tick 4 the front interrupt reports false and is not acted on (C15, C19). | — |
| 6 | Reverse counter 3 (revMin) and left is free. Move Backward active → **false**; Turn Left written; **no** Clean write, so clean_off continues (T22, C9). | `screenshots/ST-05_tick06.png` |
| 9 | Turn done. Move Forward active → **true**; Forward and clean_on written (T16). | `screenshots/ST-05_tick09.png` |

### Expected behaviour
- With obstacles in front, left and right, the RVC moves backward, then turns aside and goes forward (R4).
- The reverse lasts at least 3 ticks and ends with a turn when a side opens, left first (C9).
- At most one continuous motor process is active at a time. Move Backward is disabled before the turn starts.

### Human verdict
- Result:
- Evaluator:
- Observations:

---

## 4. ST-10: Dust trail, no restart during the power-up, new power-up afterwards

Requirements: R5, C6, C12, C14, C17, C28. FSM rows: T6, T10, T11, T6, T2.

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  ...
  y4 > * * * * * . . . .
  ...  (all other cells empty)
```
Custom start (0,4) facing E. Dust at (1,4) to (5,4). No obstacles. 20 ticks.

### Key ticks

| Tick | What to observe | Screenshot |
|---|---|---|
| 2 | Dust at (1,4): clean_up written, no Direction write (T6). The power-up counter starts at 0. | `screenshots/ST-10_tick02.png` |
| 3–4 | Dust is detected again, but there is no Clean write and the counter keeps counting 1, 2: **no restart** (T10, C14). | `screenshots/ST-10_tick04.png` |
| 5 | Counter 3: clean_on written although dust is detected on this tick (T11, C28). | `screenshots/ST-10_tick05.png` |
| 6 | Dust at (5,4) on a later tick: **new** power-up, clean_up written (T6, C28). | `screenshots/ST-10_tick06.png` |
| 9, 10 | clean_on at tick 9. At tick 10 the wall at (9,4) causes Turn Left and clean_off. | — |

### Expected behaviour
- Detected dust powers up the cleaning for a while: 3 ticks (R5, C6).
- Dust during an active power-up does not restart it (C14).
- When the power-up ends, the mode returns to clean_on on that tick. Dust on a later tick starts a new power-up (C28).

### Human verdict
- Result:
- Evaluator:
- Observations:

---

## 5. ST-11: Obstacle cancels the power-up

Requirements: R3, R5, C4, C6, C8, C11, C25, C28. FSM rows: T6, T7, T12, T16, T10.

```text
    x 0 1 2 3 4 5 6 7 8 9
  y0 . . . . . . . . . .
  y1 . . . . . . . . . .
  y2 . . . . # . . . . .
  y3 . . . * . . . . . .
  y4 . . . . ^ . . . . .     (dust also on the start cell (4,4))
  ...  (rows 5-9 empty)
```
Start (4,4) facing N. Obstacle at (4,2). Dust at (4,4) and (3,3). 20 ticks.

### Key ticks

| Tick | What to observe | Screenshot |
|---|---|---|
| 1 | Dust on the start cell: clean_up (T6). | `screenshots/ST-11_tick01.png` |
| 2 | At (4,3) the obstacle is ahead: Move Forward inactive, clean_off and Turn Left written. The power-up is **cancelled** after 1 tick (T7, C11). | `screenshots/ST-11_tick02.png` |
| 5 | Turn done: Forward and **clean_on**, not clean_up. The cancelled power-up is not resumed (T16). | `screenshots/ST-11_tick05.png` |
| 6 | Dust at (3,3): a new power-up, clean_up (T6). | `screenshots/ST-11_tick06.png` |
| 9 | Wall ahead at (0,3) while in power-up: the front is checked first, so Turn Left and clean_off (T7). | — |

### Expected behaviour
- An obstacle during power-up cleaning cancels the power-up (C11). Cleaning stops while turning (C8) and resumes as normal cleaning afterwards (R3).

### Human verdict
- Result:
- Evaluator:
- Observations:
