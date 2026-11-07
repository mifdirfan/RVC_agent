---
name: systemtesting-simulator
description: Build and validate a web-based RVC simulator with an editable 10x10 grid, tick-by-tick state inspection, at least 20 requirement-traceable test scenarios, and a maximum of 100 ticks per scenario.
---

# Web-Based RVC System Testing

## Inputs
- clarifications.md
- artifacts/SA/DFD/dfd_level_4_fsm.md
- artifacts/SA/event_list.md
- artifacts/SA/process_specification.md
- artifacts/SA/data_dictionary.md
- artifacts/SD/structure_chart.md
- include/rvc.h, include/rvc_port.h, include/front_sensor.h, include/rvc_types.h
- src/ (approved implementation, 84/84 tests passing)
- artifacts/Implementation/test_report.md

## Objective
Develop a web-based simulator to visualize
and verify the implemented RVC system.

Execute requirement-based test scenarios
on an editable 10x10 grid map.

Allow users to inspect the complete RVC
state at every simulation tick.

## Procedure

### Step 1. Review System Artifacts
1. Read the preliminary requirements.
2. Review the Controller FSM.
3. Review the Data Dictionary.
4. Review Process Specifications.
5. Review the approved Structure Chart.
6. Inspect the implemented C source code.
7. Identify observable states, inputs,
   outputs, timers, and control variables.
8. Extract requirement IDs for test traceability.

### Step 2. Design Simulator Architecture
1. Use the implemented C system as the
   source of RVC control behavior.
2. Design a web-based visualization interface.
3. Define the communication interface
   between the simulator and C implementation.
4. Separate map management, simulation
   execution, and visualization.
5. Define a deterministic tick execution model.
6. Do not replace the actual C controller
   with independently reimplemented logic.
7. Fixed architecture (do not re-decide):
   - simulator/runner/sim_runner.c links the real src/ files (not main.c) and implements the six
     rvc_port_* functions from the grid. Each tick: compute F/L/R relative to the heading
     (call front_sensor_on_interrupt() when F changes), read dust on the current cell, call rvc_tick(),
     apply the last Direction to the pose, and remove dust from cleaned cells.
     Input: scenario JSON. Output: trace JSON with the full state per tick.
   - Build it with the same MinGW gcc used for TDD.
   - simulator/server.py (Python standard library only) serves simulator/web/ and offers POST /run,
     which runs sim_runner on the posted map and returns the trace.
   - Pre-generate simulator/traces/ST-xx.json for every scenario.
   - simulator/web/: index.html + app.js + style.css, no framework.
   - Start command: python simulator/server.py, then open http://localhost:8000
8. Movement model: Forward/Backward = 1 cell per tick; a turn changes the heading by 90 degrees when
   the 3-tick turn completes. Moving into a wall or obstacle is blocked and flagged as a collision.
9. Debug state: if the controller state, counters or last commands cannot be observed through the
   ports, add a read-only rvc_debug_get() in a separate file under simulator/ (do not modify src/).

### Step 3. Implement Grid Map
1. Create a 10x10 grid map.
2. Support empty cells, obstacles, and dust.
3. Set the default RVC position to the center.
4. Define an explicit initial direction.
5. Prevent invalid initial placement.
6. Support user-defined map configurations.
7. Allow users to edit obstacle locations.
8. Allow users to edit dust locations.
9. Allow users to change the RVC start position
   and initial direction.
10. Validate the map before simulation.

### Step 4. Implement Simulation Controls
1. Provide Start, Pause, Step, Reset,
   and Restart controls.
2. Execute simulation in discrete ticks.
3. Allow a maximum of 100 ticks per scenario.
4. Allow tick-by-tick manual execution.
5. Support automatic execution with
   adjustable playback speed.
6. Stop automatically at the tick limit.
7. Preserve the execution history.
8. Allow users to inspect previous ticks.
9. Reset the controller and map state
   before starting a new scenario.

### Step 5. Visualize RVC Behavior
1. Render the complete 10x10 grid.
2. Display RVC position and direction.
3. Display obstacles and dust.
4. Update the map after every tick.
5. Highlight the current RVC position.
6. Optionally display movement history.
7. Display the current simulation tick.
8. Show the complete RVC state information.

### Step 6. Implement State Inspection
At every tick, record and display:
- Tick number
- RVC position (x, y)
- RVC direction
- Controller state
- Sensor inputs
- Motor output
- Cleaner output
- Timer values
- Relevant internal control variables
- State transition information

Include all additional state variables
defined in the implementation and FSM.


### Step 7. Generate Test Scenarios
1. Identify essential system functions from
   requirements and approved SA artifacts.
2. Generate at least 20 distinct test scenarios.
3. Ensure each scenario evaluates at least
   3 essential system functions.
4. Define a 10x10 map for each scenario.
5. Define the initial RVC position and direction.
6. Define obstacle and dust configurations.
7. Describe expected behavior based on requirements.
8. Identify related requirement IDs and SA artifacts.
9. Limit each scenario to a maximum of 100 ticks.
10. Follow templates/test-scenarios.md.


### Step 8. Execute System Tests
1. Load the selected test scenario.
2. Reset the RVC implementation.
3. Execute the scenario for up to 100 ticks.
4. Record the complete RVC state at every tick.
5. Display state transitions, sensor inputs,
   motor outputs, and cleaner outputs.
6. Allow the human evaluator to inspect
   the simulation and execution history.
7. Allow the evaluator to assign
   PASS, FAIL, or INCONCLUSIVE.
8. Record evaluation results and observations.
9. Save test results and execution traces.


### Step 9. Support Custom Map Testing
1. Allow users to modify the grid map.
2. Allow users to change the initial
   RVC position and direction.
3. Validate the modified configuration.
4. Reset the system before execution.
5. Execute the actual C controller.
6. Record complete state information
   at every tick.
7. Allow users to inspect and evaluate
   RVC behavior manually.
8. Record human evaluation results.
9. Allow modified maps to be saved
   as new test scenarios.


### Step 10. Validate Simulator
1. Verify that grid updates match RVC movement.
2. Verify that displayed states match
   actual controller states.
3. Verify that sensor inputs match map conditions.
4. Verify correct tick advancement.
5. Verify the 100-tick execution limit.
6. Verify map editing and custom start positions.
7. Verify complete state history recording.
8. Verify human evaluation and result recording.
9. Verify requirement traceability.
10. Report simulator limitations.

### Step 11. Representative scenarios for the report
- Choose 5 scenarios: straight cleaning with side walls (C13), left-priority turn (C4),
boxed-in reverse then turn (C9, Enable/Disable), dust power-up / no restart / new power-up (C14, C28),
- obstacle cancels power-up (C11). For each, write artifacts/SystemTest/representative.md with the map,
- the key ticks, the expected behaviour and an empty human verdict. If Playwright is available,
save screenshots to artifacts/SystemTest/screenshots/; otherwise list which tick to capture.
- Do not assign PASS/FAIL yourself.


## Constraints
- Use a 10x10 grid.
- Use the center as the default RVC start position.
- Allow user-defined start positions on custom maps.
- Limit each scenario to a maximum of 100 ticks.
- Provide at least 20 distinct test scenarios.
- Evaluate at least 3 essential system functions
  in each scenario.
- Support interactive map editing.
- Display complete RVC state at every tick.
- Execute the implemented C controller.
- Maintain requirement-to-test traceability.
- Use human evaluation as the test oracle.
- Do not automatically assign PASS or FAIL.
- Do not invent expected behavior
  unsupported by requirements.


## Formatting
- Follow templates/simulator-specification.md.
- Follow templates/test-scenarios.md.

## Outputs
- simulator/
- artifacts/SystemTest/test_scenarios.md
- artifacts/SystemTest/test_results.md
- artifacts/SystemTest/requirement_traceability.md


## Completion Criteria
- The simulator runs in a web browser.
- The 10x10 grid is rendered correctly.
- Users can edit maps and start positions.
- All RVC state variables are inspectable.
- At least 20 test scenarios are implemented.
- Each scenario evaluates at least
  3 essential system functions.
- Each scenario runs for at most 100 ticks.
- Complete execution history is recorded.
- Human evaluators can assign PASS, FAIL,
  or INCONCLUSIVE and record observations.
- Every scenario references relevant requirements.
- Custom maps can be tested and saved.
