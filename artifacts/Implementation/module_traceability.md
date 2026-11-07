# Module Traceability (SD → Implementation)

Sources: `artifacts/SD/structure_chart.md` (SC, approved 2026-11-08), `artifacts/SD/transform_analysis.md`, `artifacts/SA/DFD/dfd_level_4_fsm.md`, `artifacts/SA/process_specification.md`, `artifacts/SA/data_dictionary.md`, `clarifications.md`, `.claude/rules/c-coding.md`.

## 1. Module → File → Function Mapping

Every Structure Chart module is one C translation unit with one header in `include/`. Each function signature is copied from SC Section 5.2.

| SC Module | SA Process | Header / Source | Functions (SC 5.2) | Hidden state (SC 5.4) |
|---|---|---|---|---|
| Main | 0 | — / `src/main.c` | `int main(void)` | — |
| RVC System | 0 | `include/rvc.h` / `src/rvc.c` | `rvc_startup`, `rvc_tick` | — |
| Detect Obstacle & Dust | 1 (Tick part) | `detection.h` / `detection.c` | `detect_on_tick` | — |
| Front Sensor Interface | 1.1 | `front_sensor.h` / `front_sensor.c` | `front_sensor_on_interrupt(bool)` | — |
| Left Sensor Interface | 1.2 | `left_sensor.h` / `left_sensor.c` | `left_sensor_sample` | — |
| Right Sensor Interface | 1.3 | `right_sensor.h` / `right_sensor.c` | `right_sensor_sample` | — |
| Dust Sensor Interface | 1.4 | `dust_sensor.h` / `dust_sensor.c` | `dust_sensor_sample` | — |
| Obstacle Location | 1.5 + store | `obstacle_location.h` / `obstacle_location.c` | `_init`, `_set_front`, `_set_sides`, `_get` | `static ObstacleLocation s_store` (all false) |
| Dust Existence | 1.6 + store | `dust_existence.h` / `dust_existence.c` | `_init`, `_set`, `_get` | `static bool s_store` (false) |
| Controller | 2.1.1 (+ L4 FSM) | `controller.h` / `controller.c` | `controller_init`, `controller_tick` | `static ControllerState s_state`; `static uint8_t s_turn_ticks, s_reverse_ticks, s_power_up_ticks` |
| Decide Avoidance | 2.1.1 (PS 2.1 `DecideAvoidance()`) | `avoidance.h` / `avoidance.c` | `decide_avoidance(bool, bool)` → `AvoidChoice` | none (pure) |
| Move Forward | 2.1.2 | `move_forward.h` / `move_forward.c` | `_init`, `_enable`, `_disable` | `static bool s_active` |
| Turn Left | 2.1.3 | `turn_left.h` / `turn_left.c` | `turn_left_trigger` | — |
| Turn Right | 2.1.4 | `turn_right.h` / `turn_right.c` | `turn_right_trigger` | — |
| Move Backward | 2.1.5 | `move_backward.h` / `move_backward.c` | `_init`, `_enable`, `_disable` | `static bool s_active` |
| Clean Normal | 2.1.6 | `clean_normal.h` / `clean_normal.c` | `clean_normal_trigger` | — |
| Clean Off | 2.1.7 | `clean_off.h` / `clean_off.c` | `clean_off_trigger` | — |
| Power-Up Clean | 2.1.8 | `power_up_clean.h` / `power_up_clean.c` | `power_up_clean_trigger` | — |
| Motor Interface [library] | 2.2 | `motor_interface.h` / `motor_interface.c` | `_init`, `_send(MotorCommand)` | `static bool s_sent_any; static MotorCommand s_last_sent` |
| Cleaner Interface [library] | 2.3 | `cleaner_interface.h` / `cleaner_interface.c` | `_init`, `_send(CleanerCommand)` | `static bool s_sent_any; static CleanerCommand s_last_sent` |
| Shared types | DD | `include/rvc_types.h` | `ObstacleLocation`, `MotorCommand`, `CleanerCommand`, `AvoidChoice` (SC 5.3) | — |
| Environment ports | terminators | `include/rvc_port.h` | 6 `rvc_port_*` declarations (SC 5.5). They are not defined in `src/`. The tests define them in `tests/doubles/fake_port.c`, and the simulator will define its own | — |

There are no global variables shared between modules. All state is `static` at file scope (SC 5.4, C rule "avoid unnecessary global variables").

## 2. Caller–Callee Relationships (verified by CH-01)

`tests/check_call_hierarchy.py` runs as CTest test `CallHierarchy.CH01_MatchesStructureChart`. For each `src/*.c` file, it checks two things:
- every call to a system function is in the approved callee set
- every approved call is present

Result: **PASS** (`logs/call_hierarchy.log`). A mutation check also confirmed that the script fails when the Controller calls `motor_interface_send()` directly, bypassing Turn Left.

| Caller | Callees found in code | SC reference |
|---|---|---|
| `main.c` | `rvc_startup`, `rvc_port_wait_tick`, `rvc_tick` | 3.1, 3.3 |
| `rvc.c` | `obstacle_location_init`, `dust_existence_init`, `motor_interface_init`, `cleaner_interface_init`, `move_forward_init`, `move_backward_init`, `controller_init` (steps 1–7, in order); `detect_on_tick`, `controller_tick` | 3.2, 3.3 |
| `detection.c` | `left_sensor_sample`, `right_sensor_sample`, `obstacle_location_set_sides`, `dust_sensor_sample`, `dust_existence_set` | 3.3 rows 1.1–1.3 |
| `front_sensor.c` | `obstacle_location_set_front` | 3.1 |
| `left/right/dust_sensor.c` | `rvc_port_read_*_sensor` | 5.5 |
| `controller.c` | `obstacle_location_get`, `dust_existence_get`, `decide_avoidance`, the 2 Enable/Disable pairs, the 5 Triggers | 3.1, 7 |
| 4 motor action modules | `motor_interface_send` | 3.1 |
| 3 cleaner action modules | `cleaner_interface_send` | 3.1 |
| `motor_interface.c` / `cleaner_interface.c` | `rvc_port_write_direction` / `rvc_port_write_clean` | 5.5 |
| `obstacle_location.c`, `dust_existence.c`, `avoidance.c` | none | 4 |

## 3. Controller FSM → Code (SC Section 7)

| FSM | Code location in `controller.c` | Signals issued (order) |
|---|---|---|
| T1 | `controller_init()` → `enter_forward_normal()` | `move_forward_enable`; `clean_normal_trigger` |
| T2–T4 | `on_fwd_normal()` → `avoid_from_forward()` → `start_avoidance()` | `move_forward_disable`; `clean_off_trigger`; `turn_left_trigger` / `turn_right_trigger` / `move_backward_enable` |
| T5 | `on_fwd_normal()`, front clear, no dust | none |
| T6 | `on_fwd_normal()`, front clear, dust | `power_up_clean_trigger` |
| T7–T9 | `on_fwd_power_up()` → `avoid_from_forward()` | as T2–T4 |
| T10 / T11 | `on_fwd_power_up()`, `s_power_up_ticks` < 3 / == 3 | none / `clean_normal_trigger` |
| T12, T17 | `on_turn()`, `s_turn_ticks` != 3 | none |
| T13–T15, T18–T20 | `on_turn()`, turn done, front blocked → `start_avoidance(decide_avoidance(L, R))` | `turn_left_trigger` / `turn_right_trigger` / `move_backward_enable` |
| T16, T21 | `on_turn()`, turn done, front clear → `enter_forward_normal()` | `move_forward_enable`; `clean_normal_trigger` |
| T24 | `on_move_backward()`, `s_reverse_ticks` < 3 | none |
| T22, T23 | `on_move_backward()`, revMin, choice ≠ backward | `move_backward_disable`; `turn_left_trigger` / `turn_right_trigger` |
| T25 | `on_move_backward()`, revMin, choice = backward | none |

The counters follow SC 5.4:
- three `uint8_t` counters
- reset to 0 on entry, including re-entry by T13 and T18
- incremented at the start of each later Tick in the timed state
- saturating at 3 (`count_tick()`)

`decide_avoidance()` is called only in the guard cases SC 7 lists. In MoveBackward, F is never tested (C15).

## 4. Design Deviations

**None.** All modules, interfaces, signatures, types, hidden state and call relationships match the approved Structure Chart. Two implementation choices sit inside the SD's freedom and change no interface or behaviour:

1. **Controller helper functions.** `controller.c` contains private `static` helpers: `count_tick`, `start_avoidance`, `avoid_from_forward`, `enter_forward_normal`, and one `on_<state>` handler per state. They are internal to the Controller module and are not modules of their own. They also remove repeated code: the same entry signals appear in T2/T7, T13/T19/T22 and others.
2. **Value parameters.** The helpers take the `ObstacleLocation` snapshot by value rather than by pointer, so no pointer is dereferenced (C rule "check pointers before dereferencing").

Build-level notes (not design changes):
- `main.c` is compiled with the same warnings (target `rvc_main_obj`) but is not linked into a test executable. Its Tick loop never ends (SC 9 item 5), and the ports are platform-defined. Tests use the same `rvc_startup()` / `rvc_tick()` entry points (SC 3.3).
- Test doubles replace modules at link time (`tests/doubles/`). Production code has no test hooks.

## 5. Coding Rules Check (`.claude/rules/c-coding.md`)

| Rule | How it is met |
|---|---|
| Follow C11 | `CMAKE_C_STANDARD 11`, `CMAKE_C_EXTENSIONS OFF`, with `-Wall -Wextra -Wpedantic -Werror`. GCC 11.2 builds it with no warnings |
| Explicit function declarations | Every public function is declared in its header, and every `.c` includes its own header. Private helpers are `static` and defined before use |
| Check pointers before dereferencing | Production code has no pointer parameters and no dereferences |
| Avoid unnecessary globals | Only file-scope `static` module state (SC 5.4). Nothing is `extern` |
| Separate interfaces into headers | 19 module headers plus `rvc_types.h` and `rvc_port.h` in `include/` |
