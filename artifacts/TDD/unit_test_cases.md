# Unit Test Cases

Sources: `docs/preliminary_requirements.md` (R1–R7), `docs/clarifications.md` (C1–C29), `artifacts/SA/event_list.md`, `artifacts/SA/DFD/dfd_level_4_fsm.md` (T1–T25), `artifacts/SA/process_specification.md` (PS), `artifacts/SA/data_dictionary.md` (DD), `artifacts/SD/structure_chart.md` (SC).

## 1. Test Executables and Test Doubles

Every test starts from a clean state: each fixture's `SetUp()` resets its doubles and calls every module `init` it links, or calls `rvc_startup()` (TDD skill rule 9).

| Executable | Production code linked | Test doubles (link seams, in `tests/doubles/`) | Purpose |
|---|---|---|---|
| `test_afferent` | `left_sensor.c`, `right_sensor.c`, `dust_sensor.c`, `front_sensor.c`, `obstacle_location.c`, `dust_existence.c`, `detection.c` | `fake_port.c` (all six `rvc_port_*` functions) | Sensor interfaces, store modules, Detect Obstacle & Dust |
| `test_interfaces` | `motor_interface.c`, `cleaner_interface.c` | `fake_port.c` | C12 compare-and-send of the two library modules |
| `test_actions` | `move_forward.c`, `move_backward.c`, `turn_left.c`, `turn_right.c`, `clean_normal.c`, `clean_off.c`, `power_up_clean.c` | `fake_interfaces.c` (records every `motor_interface_send` / `cleaner_interface_send` call) | One command per Enable / Trigger, none on Disable (C24, C25) |
| `test_avoidance` | `avoidance.c` | — | Left-first avoidance rule (C4, R4) |
| `test_controller` | `controller.c`, `avoidance.c` | `fake_stores.c` (Obstacle Location, Dust Existence), `fake_actions.c` (records every Enable / Disable / Trigger call in order) | FSM T1–T25: exact signal sequence per transition (C26, FSM Issue 4) |
| `test_system` | all modules except `main.c` | `fake_port.c` | `rvc_startup()` / `rvc_tick()` end to end: exact Direction / Clean write sequence and count (TDD rule 8) |
| `check_call_hierarchy` (CTest, Python) | all `src/*.c` (static scan) | — | Caller–callee relationships match SC Section 3 / 5.1 |

`fake_port.c` records every `rvc_port_write_direction()` and `rvc_port_write_clean()` call in order. It also returns scripted values from the three read ports and counts the reads (TDD rule 8).

`main.c` is compiled (target `rvc_main_obj`) but not linked into tests. Its Tick loop has no end (SC Section 9 item 5), and tests use the same entry points `rvc_startup()` / `rvc_tick()` instead (SC Section 3.3).

## 2. Test Cases

Notation: `F, L, R, D` = store values read on the Tick. "Signals" = calls recorded by `fake_actions`. "Writes" = port writes recorded by `fake_port`.

### 2.1 Module Init (one row per stateful module)

| ID | Module | Test | Expected | Source |
|---|---|---|---|---|
| INIT-01 | Obstacle Location | `obstacle_location_init()` after the store was set | `get()` = {F=false, L=false, R=false} | C21 |
| INIT-02 | Dust Existence | `dust_existence_init()` after set(true) | `get()` = false | C21 |
| INIT-03 | Motor Interface | `init()` after a send, then send the same value again | value written again (nothing sent since init) | C27, PS 2.2 |
| INIT-04 | Cleaner Interface | as INIT-03 | value written again | C27, PS 2.3 |
| INIT-05 | Move Forward | `init()` produces no command | 0 motor commands | PS 2.1.2, SC 5.4 |
| INIT-06 | Move Backward | `init()` produces no command | 0 motor commands | PS 2.1.5, SC 5.4 |
| INIT-07 | Controller | `controller_init()` = T1 (see T1) | signals: Enable MF, Trigger CN | C27, T1 |
| INIT-08 | RVC System | `rvc_startup()` | writes: Direction=Forward, Clean=clean_on, once each; stores false | C16, C21, C22, C27 |

### 2.2 Afferent Branch (Process 1)

| ID | Test | Expected | Source |
|---|---|---|---|
| AFF-01 | `left_sensor_sample()` with port true / false | returns true / false; port read once per call | PS 1.2, C3 |
| AFF-02 | `right_sensor_sample()` with port true / false | returns true / false; read once | PS 1.3, C3 |
| AFF-03 | `dust_sensor_sample()` with port true / false | returns true / false; read once | PS 1.4, C3 |
| AFF-04 | `front_sensor_on_interrupt(true)` | store front = true; left, right unchanged | PS 1.1, 1.5, C1 |
| AFF-05 | `front_sensor_on_interrupt(false)` after true | store front = false (false is passed on) | C19 |
| AFF-06 | `obstacle_location_set_sides(l, r)` | sides updated; front unchanged | PS 1.5 |
| AFF-07 | `obstacle_location_set_front(f)` | front updated; sides unchanged | PS 1.5 |
| AFF-08 | `dust_existence_set(true/false)` | `get()` returns the value | PS 1.6 |
| AFF-09 | `detect_on_tick()` | each read port sampled exactly once; L, R, D stored; front unchanged | PS 1, C2, C17, C20 |

### 2.3 Library Modules (C12)

| ID | Test | Expected writes | Source |
|---|---|---|---|
| C12-01 | Motor: first send after init | written once | C22, C27 |
| C12-02 | Motor: same value twice | written once (no duplicate send) | C12 |
| C12-03 | Motor: Forward, Turn Left, Turn Left, Forward | Forward, Turn Left, Forward | C12 |
| C12-04 | Motor: all 4 values in turn | 4 writes with matching values | DD 4.4 |
| C12-05 | Cleaner: first send after init | written once | C22, C27 |
| C12-06 | Cleaner: same value twice | written once | C12 |
| C12-07 | Cleaner: clean_on, clean_off, clean_off, clean_up, clean_on | clean_on, clean_off, clean_up, clean_on | C12 |

### 2.4 Action Modules (Efferent, C24, C25)

| ID | Test | Expected interface calls | Source |
|---|---|---|---|
| ACT-01 | `move_forward_enable()` | 1 × motor send Forward | PS 2.1.2, C24 |
| ACT-02 | `move_forward_disable()` after enable | no further call | C24 |
| ACT-03 | `move_backward_enable()` | 1 × motor send Backward | PS 2.1.5, C24 |
| ACT-04 | `move_backward_disable()` after enable | no further call | C24 |
| ACT-05 | `turn_left_trigger()` twice | 2 × motor send Turn Left (one per Trigger) | PS 2.1.3, C25 |
| ACT-06 | `turn_right_trigger()` | 1 × motor send Turn Right | PS 2.1.4, C25 |
| ACT-07 | `clean_normal_trigger()` | 1 × cleaner send clean_on | PS 2.1.6, C25 |
| ACT-08 | `clean_off_trigger()` | 1 × cleaner send clean_off | PS 2.1.7, C25 |
| ACT-09 | `power_up_clean_trigger()` | 1 × cleaner send clean_up | PS 2.1.8, C25 |

### 2.5 Decide Avoidance

| ID | L | R | Expected | Source |
|---|---|---|---|---|
| AV-01 | false | false | `AVOID_TURN_LEFT` (left first) | C4 |
| AV-02 | false | true | `AVOID_TURN_LEFT` | C4, R3 |
| AV-03 | true | false | `AVOID_TURN_RIGHT` | R3 |
| AV-04 | true | true | `AVOID_BACKWARD` | R4 |

### 2.6 Controller FSM (one row per transition, `test_controller`)

Setup ("reach X"): drive the FSM into state X with Ticks and fake store values, then clear the signal log. "k ticks in state" = number of Ticks handled in the state after the entry Tick.

| ID | From | Store values on the Tick | Expected signals (in order) | Next-state check | Source |
|---|---|---|---|---|---|
| T1 | [*] | `controller_init()` | Enable MF; Trigger CN | FwdNormal (T6 possible next) | C16, C27 |
| T2 | FwdNormal | F, !L | Disable MF; Trigger CO; Trigger TL | TurnLeft | R3, C4, C8 |
| T3 | FwdNormal | F, L, !R | Disable MF; Trigger CO; Trigger TR | TurnRight | R3, C4 |
| T4 | FwdNormal | F, L, R | Disable MF; Trigger CO; Enable MB | MoveBackward | R4, C9 |
| T5 | FwdNormal | !F, !D, all four L/R combinations | none | FwdNormal | C13, C26 |
| T6 | FwdNormal | !F, D | Trigger PU | FwdPowerUp | R5 |
| T7 | FwdPowerUp | F, !L | Disable MF; Trigger CO; Trigger TL | TurnLeft | C11 |
| T8 | FwdPowerUp | F, L, !R | Disable MF; Trigger CO; Trigger TR | TurnRight | C11 |
| T9 | FwdPowerUp | F, L, R | Disable MF; Trigger CO; Enable MB | MoveBackward | C11 |
| T10 | FwdPowerUp, 1st and 2nd tick | !F, D or !D, any L/R | none | FwdPowerUp | C6, C13, C14 |
| T11 | FwdPowerUp, 3rd tick | !F (D true) | Trigger CN | FwdNormal | C6, C28 |
| T12 | TurnLeft, ticks 1–2 | F, D (ignored) | none | TurnLeft | C5, C10, C15 |
| T13 | TurnLeft, tick 3 | F, !L | Trigger TL; then 2 silent ticks; ends on the 3rd | TurnLeft (counter reset) | C5, C15 |
| T14 | TurnLeft, tick 3 | F, L, !R | Trigger TR | TurnRight | C15 |
| T15 | TurnLeft, tick 3 | F, L, R | Enable MB | MoveBackward | R4, C15 |
| T16 | TurnLeft, tick 3 | !F, D | Enable MF; Trigger CN (D ignored) | FwdNormal | R3, C29 |
| T17 | TurnRight, ticks 1–2 | F, D | none | TurnRight | C5, C10, C15 |
| T18 | TurnRight, tick 3 | F, L, !R | Trigger TR; then 2 silent ticks | TurnRight (counter reset) | C15 |
| T19 | TurnRight, tick 3 | F, !L | Trigger TL | TurnLeft | C4, C15 |
| T20 | TurnRight, tick 3 | F, L, R | Enable MB | MoveBackward | R4 |
| T21 | TurnRight, tick 3 | !F, D | Enable MF; Trigger CN (D ignored) | FwdNormal | R3, C29 |
| T22 | MoveBackward, tick ≥3 | !L | Disable MB; Trigger TL | TurnLeft | R4, C4, C9 |
| T23 | MoveBackward, tick ≥3 | L, !R | Disable MB; Trigger TR | TurnRight | R4, C9 |
| T24 | MoveBackward, ticks 1–2 | !L, !R, F, D (ignored) | none | MoveBackward | C9, C10, C15 |
| T25 | MoveBackward, tick ≥3 | L, R | none | MoveBackward | C9 |

### 2.7 Clarification-Specific and Counter Tests

| ID | Test | Expected | Source |
|---|---|---|---|
| CL-C13 | FwdNormal and FwdPowerUp, F false, L/R true | no signal | C13 |
| CL-C14 | Dust on every tick of a power-up | power-up ends after 3 ticks; no second Trigger PU during it | C14 |
| CL-C28 | Dust still true after the power-up ends | T11 on tick 3 (Trigger CN); T6 (Trigger PU) on the next tick | C28 |
| CL-C29 | Dust true on the tick a turn ends with front clear | T16 only (no PU); PU on the next tick | C29 |
| CL-C10 | Dust during reverse, then reverse ends | no PU in MoveBackward or the following turn | C10 |
| CL-C11 | Obstacle on tick 1 of power-up; later dust | power-up cancelled (T7); a new power-up after returning lasts the full 3 ticks (counter reset on T6) | C11, SC 5.4 |
| CNT-01 | Turn lasts exactly 3 ticks | entry tick k; exit on tick k+3, not before | C5 |
| CNT-02 | Power-up lasts exactly 3 ticks | exit on tick k+3 | C6 |
| CNT-03 | Reverse minimum 3 ticks | side opens at tick 1: exit on tick k+3 | C9 |
| CNT-04 | Reverse counter saturation | 300 ticks with L and R blocked (beyond `uint8_t` range), then L opens | T22 still taken (revMin stays true); no signal during the 300 ticks | SC 5.4 |
| CNT-05 | Turn counter reset on re-entry | after T13 the new turn lasts a full 3 ticks | SC 5.4 |

### 2.8 System Tests through `rvc_startup()` / `rvc_tick()` (`test_system`)

| ID | Scenario | Expected writes (exact sequence) | Source |
|---|---|---|---|
| SYS-01 | Start-up, then 10 clear ticks | Dir Forward; Clean clean_on; nothing else | C12, C16, C22 |
| SYS-02 | Front interrupt (true), next tick, L, R free | Clean clean_off, Dir Turn Left; turn 3 ticks; front cleared → Dir Forward, Clean clean_on | R3, C4, C5, C8, C20 |
| SYS-03 | Interrupt arrives between ticks; acted on only at the next tick | no write until the next `rvc_tick()` | C1, C20 |
| SYS-04 | Left sampled on the same tick as the decision | left blocked set by the port on that tick → Turn Right | C18, C20 |
| SYS-05 | Front, left and right blocked | clean_off, Backward ≥3 ticks, then Turn Left when left opens, then Forward, clean_on | R4, C9 |
| SYS-06 | Turn ends with front still blocked, left free | no duplicate Turn Left write; a further 3 ticks of turning | C12, C15, T13 |
| SYS-07 | Dust | clean_up for 3 ticks, then clean_on | R5, C6 |
| SYS-08 | Obstacle during power-up | clean_off, Turn Left (no clean_on in between) | C11 |
| SYS-09 | Front false interrupt only | no write | C19 |
| SYS-10 | Turn Right then T19 to Turn Left | writes Turn Right, then Turn Left | C4, T19 |

### 2.9 Structure Check

| ID | Test | Expected | Source |
|---|---|---|---|
| CH-01 | Static scan of the calls in each `src/*.c` file | Each module calls only the callees in SC Sections 3.1, 3.2 and 5.1. Every chart call exists | SC 3, 5.1 |

## 3. Requirement-to-Test Traceability

| Req. | Tests |
|---|---|
| R1 | INIT-08, SYS-01, ACT-07 |
| R2 | SYS-01, T5 |
| R3 | T2, T3, T7, T8, T16, T21, SYS-02 |
| R4 | AV-04, T4, T9, T15, T20, T22, T23, SYS-05 |
| R5 | T6, T11, SYS-07 |
| R6, R7 | Port abstraction (`fake_port.c`); no extra behaviour tested |
| C1 | AFF-04, SYS-03 |
| C2, C17 | AFF-09 |
| C3 | AFF-01 – AFF-03 |
| C4 | AV-01, AV-02, T2, T13, T19, T22, SYS-02, SYS-10 |
| C5 | T12, T13, CNT-01, CNT-05 |
| C6 | T10, T11, CNT-02, SYS-07 |
| C7, C8 | ACT-07 – ACT-09, T2 – T4, SYS-02 |
| C9 | T22 – T25, CNT-03, CNT-04, SYS-05 |
| C10 | T12, T17, T24, CL-C10 |
| C11 | T7 – T9, CL-C11, SYS-08 |
| C12 | C12-01 – C12-07, SYS-01, SYS-06 |
| C13 | T5, T10, CL-C13 |
| C14 | T10, CL-C14 |
| C15 | T12 – T15, T17 – T20, SYS-06 |
| C16, C22, C27 | T1, INIT-07, INIT-08, SYS-01 |
| C18, C20 | AFF-09, SYS-03, SYS-04 |
| C19 | AFF-05, SYS-09 |
| C21, C23 | INIT-01, INIT-02, INIT-08 |
| C24, C25 | ACT-01 – ACT-09 |
| C26 | T5, T10, T12, T17, T24, T25 |
| C28 | T11, CL-C28 |
| C29 | T16, T21, CL-C29 |
