# Web-Based RVC Simulator — Specification

Start: `python simulator/server.py`, then open http://localhost:8000.
The runner is built automatically on first start, using `C:\MinGW\bin\gcc.exe` (the GCC 11.2.0 used for TDD) or `gcc` on the PATH.

## 1. Simulator Overview

The simulator shows the behaviour of the **implemented C controller** (`src/`, 84/84 unit tests passing) on an editable 10x10 grid, and supports requirement-based system testing with a human evaluator as the oracle. It provides:
- 22 predefined scenarios (`scenarios/`)
- custom maps
- tick-by-tick inspection of the complete RVC state
- an execution history
- recording of PASS / FAIL / INCONCLUSIVE verdicts

## 2. Execution Architecture

```
 browser (web/)                server.py (stdlib)              runner/sim_runner.exe (C11, GCC)
 ┌──────────────────┐  POST /run ┌───────────────┐  scenario.json ┌───────────────────────────────────┐
 │ map editor       │ ─────────▶ │ validates     │ ─────────────▶ │ sim_world   grid, sensors, motion │
 │ playback/inspect │ ◀───────── │ size, runs    │ ◀───────────── │ sim_ports   rvc_port_* (6 ports)  │
 │ evaluation form  │   trace    │ the runner    │   trace.json   │ src/*.c     REAL controller       │
 └──────────────────┘            └───────────────┘                │ debug/      read-only rvc_debug_get│
                                                                  └───────────────────────────────────┘
```

| Concern | Where | Notes |
|---|---|---|
| Control behaviour | `src/*.c`, unmodified | Every module except `main.c`, whose tick loop never ends. Nothing is re-implemented in JS or Python. |
| Environment ports | `runner/sim_ports.c` | Defines the six `rvc_port_*` functions of `include/rvc_port.h` from the grid. |
| Map, sensors, motion | `runner/sim_world.c` | Contains no control decisions. |
| Hidden state (read-only) | `runner/debug/` | See below. |
| Scenario parsing and validation | `runner/sim_scenario.c` | Rejects invalid maps, never alters them. |
| Visualisation | `web/` (`index.html`, `app.js`, `style.css`, no framework) | Plays back the trace. Decides nothing. |
| Serving and storage | `server.py` | Serves the UI, runs the runner, stores custom scenarios and evaluations. |

**Hidden state.** The FSM state, the three counters and the Move Forward / Move Backward active flags are `static` inside `src/`, so the ports can't see them. `runner/debug/dbg_controller.c`, `dbg_move_forward.c` and `dbg_move_backward.c` each `#include` the **unmodified** `src/<module>.c` into their own translation unit and add a read-only getter. The runner links them *instead of* those three source files, so the controller code that runs is exactly the approved code. `src/` is not modified.

**Build.** `python simulator/build_runner.py` compiles with `-std=c11 -Wall -Wextra -Wpedantic -Werror`; it currently builds with no warnings.

**Required execution interface, mapped to the C API:**

| Template interface | Implementation |
|---|---|
| `initialize(configuration)` | The scenario JSON is parsed and validated (`scenario_parse`), then `rvc_startup()` runs. That gives tick 0. |
| `step(sensor_inputs)` | One tick of the loop in `sim_runner.c`: the sensor frame comes from the map, `front_sensor_on_interrupt()` is called if F changed, then `rvc_tick()`. |
| `get_state()` | `rvc_debug_get()` (controller state, counters, motor-process flags, stores), plus the port log (reads and writes) and the world (pose, dust). |
| `reset()` | Every run is a new `sim_runner` process, so all static state starts from its initial value, then `rvc_startup()`. **Run** and **Restart** in the UI always do this. |

Execution is deterministic. The runner computes the whole run (up to 100 ticks) in one go; the UI then steps through the recorded snapshots. **Step back** only inspects saved snapshots and never reverses the controller. Validation check V12 confirms that two runs give identical traces.

## 3. Grid Map

- 10x10 grid. x = 0..9 is the column (left to right) and y = 0..9 is the row (top to bottom).
- Default start is (4,4). Default heading is **N** (up). The requirements give no compass heading, so the heading is an explicit simulator convention.
- Cells outside the grid count as walls (obstacles).
- Obstacles and dust are kept in separate sets.

**Map editing** (the **Edit map** button):
- tools: obstacle, dust, start cell, erase (click or drag)
- heading, max ticks
- clear map, reset to scenario, start at centre
- validate, save as scenario (`scenarios/custom/<id>.json`)
- import / export map JSON

**Validation**, done the same way in the UI and in `sim_runner`:
- the start is inside the grid and not on an obstacle
- the heading is N, E, S or W
- every cell is inside the grid
- no duplicate cells
- no cell is both an obstacle and dust
- max ticks is 1..100

A forbidden edit is **refused with a message**. The map is never changed silently.

## 4. Visualization

- The 10x10 grid with axis labels.
- The RVC as a disc with a heading arrow. Its ring colour shows the cleaner mode: green clean_on, amber clean_up, grey clean_off.
- During a turn, a badge shows the rotation progress, e.g. `↺ 2/3`. On a blocked move the disc turns red and shows `collision`.
- Obstacles, the dust remaining at the shown tick, and the visited cells up to that tick.
- The current tick, the trace source (`pre-generated` or `live run`), the controller state, and the motor and cleaner status.

## 5. Simulation Controls

| Control | Effect |
|---|---|
| Run C controller | Executes `sim_runner` on the current map from reset and shows tick 0. |
| ▶ Start | Plays automatically at 1–20 ticks/s and stops at the last tick (the tick limit). Runs the controller first if there is no trace. |
| ❚❚ Pause | Stops playback. |
| Step ▶ / ◀ Step back | Moves one recorded tick forward or back. Also the → and ← keys; Space plays or pauses. |
| Reset | Back to tick 0: the initial map, pose and controller state. |
| Restart | Re-runs the C controller from reset and plays. |
| Tick slider, history rows, key-tick chips | Jump to any recorded tick. |
| Scenario list | Built-in scenarios and saved custom scenarios. |
| Deep link | `#scenario=ST-05&tick=6` (optional `&theme=light`) opens a scenario at a tick. |

The maximum is 100 ticks per scenario. `sim_runner` rejects `max_ticks` > 100 (check V9).

## 6. Tick Execution

One tick is one complete system update. For tick k = 1..max_ticks:

1. Read the pose reached after tick k-1 (shown as "Sensed at").
2. Sensor inputs: F, L and R are the neighbour cells relative to the heading (a wall or obstacle gives true). D is dust on the current cell.
3. If F differs from the last front value delivered, raise the front interrupt: `front_sensor_on_interrupt(F)`. The interrupt is event-driven (C1) and only reports changes. The store starts false (C21).
4. `rvc_tick()`: the controller reads L, R and D once through the ports (C17), then decides (C20).
5. Outputs: any `rvc_port_write_direction` / `rvc_port_write_clean` calls are logged. The actuators keep the last value written.
6. Cleaning: if the cleaner holds clean_on or clean_up, dust on the current cell is removed. clean_off removes nothing.
7. Motion, from the held Direction:
   - Forward or Backward moves 1 cell. If the target is a wall or obstacle, the move is blocked, the pose does not change and a collision is flagged.
   - Turn Left or Turn Right: the heading changes by 90° at the end of the turn's 3rd tick (C5). The rotation progress restarts whenever a new Direction is written.
8. A complete snapshot is recorded. The UI renders it and the human evaluates it. There are no automatic assertions.

Tick 0 is the snapshot right after `rvc_startup()` (C21, C22, C27).

**Clock-edge mapping:** one simulator tick is one `rvc_tick()` call, which is one Tick event of the FSM. The front interrupt is delivered inside the same tick, before `rvc_tick()`, which matches C20: the interrupt updates the store at once, and the decision is made on the Tick.

## 7. State Inspection

The State Inspector shows, for the selected tick, each value with its previous value highlighted when it changed. Every value comes from the trace; none is made up.

| Group | Fields |
|---|---|
| Tick & pose | tick, phase, sensed-at position and heading, position after the tick (x, y), heading after the tick, turn progress (motor) |
| Sensor inputs | front, left, right, dust; front interrupt raised (and its value); number of port reads of L / R / D |
| Data stores | Obstacle Location front / left / right; Dust Existence |
| Controller (2.1.1) | previous state, current state, FSM row (T1–T25), whether a transition happened, turn / reverse / power-up tick counters, Move Forward active, Move Backward active |
| Outputs | Direction and Clean written this tick (C12: only on change); motor and cleaner outputs held |
| Environment effect | motion, collision, cell cleaned, dust cells remaining |

The FSM row label is worked out from the *observed* change of state and counters (`fsm_row()` in `sim_runner.c`). It is a reading aid for the evaluator and feeds nothing back into the run.

## 8. Execution History

Each tick's snapshot, from tick 0 to the end, is in the trace and in the history table.
- **Rows:** tick-0 row, transition rows and collision rows are highlighted. A filter shows only ticks with a transition, a write or a collision. Clicking a row jumps to that tick.
- **Comparison:** the inspector shows each changed value next to its previous value.
- **Export trace** downloads the full JSON.

Trace format `rvc-trace/1`: `scenario` (start, obstacles, dust, max_ticks), `ticks[]` (the fields in Section 7, plus the dust list after each tick) and `summary`.

## 9. Test Execution

- **Scenarios:** 22 predefined scenarios (`artifacts/SystemTest/test_scenarios.md`). Each has at least 3 essential functions, requirement IDs, FSM rows, Event List events and Process Specifications. The expected behaviour is written from the requirements and shown beside the map.
- **Human verdicts:** the evaluator selects a scenario, inspects it, then records PASS / FAIL / INCONCLUSIVE with their name, observations and related ticks.
- **Where verdicts are saved:** `results/evaluations.json`, with earlier verdicts kept as history. The evaluated trace is saved as `results/traces/<id>_<timestamp>.json`, and `artifacts/SystemTest/test_results.md` is regenerated.
- **No automatic verdicts.** The simulator never assigns PASS or FAIL. Reaching the last tick is not counted as success.
- **Custom maps:** an edited map must be saved as a scenario before it can be evaluated, so that each verdict refers to a stored map.

## 10. User Interface Layout

| Panel | Contents |
|---|---|
| Scenario panel | Scenario list, configuration, requirements, FSM rows, key-tick chips, essential functions, expected behaviour, human evaluation form and the recorded verdict |
| Map panel | Editable grid, legend, edit bar, controls, speed, tick slider |
| State inspector | All the fields in Section 7, with change highlighting |
| History panel | Tick-by-tick table of inputs, state, FSM row, counters, motor-process flags, writes, outputs, motion and cleaning |

The layout adapts from three columns down to one column on narrow screens, and supports light and dark themes.

## 11. Implementation Constraints and Files

The production controller is unchanged C11 code, built with GCC.

```
simulator/
  server.py              web server + POST /run (stdlib only)
  build_runner.py        builds runner/sim_runner.exe with GCC
  scenario_defs.py       scenario definitions (single source of truth)
  generate.py            writes scenarios/, traces/, test_scenarios.md, requirement_traceability.md
  results_report.py      evaluation storage + test_results.md
  runner/                sim_runner.c, sim_world.*, sim_ports.*, sim_scenario.*, debug/
  web/                   index.html, app.js, style.css
  scenarios/ST-xx.json   22 built-in scenarios; scenarios/custom/ for saved custom maps
  traces/ST-xx.json      pre-generated traces (real C controller)
  results/               created when the first evaluation is saved
  validation/            validate_simulator.py, ui_check.mjs (Step 10)
```

Regenerating after a change to `src/` or to the scenarios: `python simulator/generate.py`, then `python simulator/validation/validate_simulator.py`. Check V12 fails if the pre-generated traces are out of date.

## 12. Validation (Step 10)

| Tool | What it checks |
|---|---|
| `python simulator/validation/validate_simulator.py` | Tick advancement, sensors against the map, grid update against the motor output, interrupt and store wiring, dust removal, held outputs, complete fields; determinism and freshness of the traces; the 100-tick limit; map validation and custom starts; traceability |
| `node --experimental-websocket simulator/validation/ui_check.mjs http://127.0.0.1:8000` | Drives headless Chrome through the controls, editing, history navigation, saving a custom scenario, recording an evaluation, and deep links. It removes its test data afterwards. |

Results: `artifacts/SystemTest/simulator_validation.md`.

## 13. Limitations

1. **Static maps only.** The FSM rows T13, T14, T15, T18, T19 and T20 cannot be reached. After a turn, the new front cell is the side cell that was sensed free when the turn was chosen (C18), so the front is always clear when a turn ends. These rows are covered by the unit tests `Controller.T13_*` … `Controller.T20_*`.
2. **Abstract sensor model.** Each sensor looks only at the neighbouring cell. Real sensor range, noise and timing are hardware details and out of scope (R6). The front interrupt fires only when F changes between ticks, not in the middle of a tick.
3. **No rear sensor.** The requirements define none, so Backward can move into an obstacle. The simulator flags this as a collision and leaves the pose unchanged. ST-16 shows this case; it is reported as an observation, not judged.
4. **Motion model is a simulator assumption.** One cell per tick, and a 90° turn completed in 3 ticks. The requirements fix only the turn duration (C5).
5. **Cleaning model is a simulator assumption.** A cell's dust is removed when the RVC spends a tick on it with clean_on or clean_up. The requirements do not define how much dust is removed or how quickly.
6. **Batch execution.** The run is computed in full before playback. Because the map is static and the runner is deterministic, this is the same as executing tick by tick.
7. **Local use only.** The server binds to 127.0.0.1, has no authentication, and handles one runner process per request.
