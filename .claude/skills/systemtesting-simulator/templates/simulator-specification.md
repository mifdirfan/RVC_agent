# Web-Based RVC Simulator Specification

## 1. Simulator Overview
Build an interactive web-based simulator
for the implemented RVC system.

The simulator must visualize RVC behavior
and support requirement-based system testing.

## 2. Execution Architecture

- Use the actual C controller implementation.
- Do not duplicate controller decisions in JavaScript.
- Connect the web UI to C through WebAssembly
  or a local backend process.
- Use a deterministic step-based execution interface.
- Keep the simulation environment separate
  from the controller implementation.

Required execution interface:
- initialize(configuration)
- step(sensor_inputs)
- get_state()
- reset()

Adapt these interfaces to the actual C API.

## 3. Grid Map

### Grid Configuration
- Width: 10 cells
- Height: 10 cells
- Coordinates: x = 0..9, y = 0..9
- Default start position: (4, 4)
- Default direction: derived from requirements

The 10x10 grid has four central cells.
Use (4, 4) as the default center convention.

### Cell Types
- Empty
- Obstacle
- Dust
- RVC

Obstacle and dust information must be
maintained independently where applicable.

### Map Editing
Users must be able to:
- Add or remove obstacles.
- Add or remove dust.
- Set the RVC initial position.
- Set the RVC initial direction.
- Clear the map.
- Reset to a predefined map.
- Save and load custom configurations.

### Validation
- Keep the RVC within grid boundaries.
- Prevent an obstacle from overlapping
  the initial RVC position.
- Reject invalid map configurations.
- Do not silently modify user-created maps.

## 4. Visualization

Display:
- A 10x10 grid
- RVC position
- RVC heading
- Obstacles
- Dust locations
- Current tick
- Current controller state
- Motor and cleaner status

Use a directional marker to show
the RVC heading.

Update the visualization after each tick.

## 5. Simulation Controls

Provide:
- Start
- Pause
- Step Forward
- Step Backward (history inspection)
- Reset
- Restart
- Playback Speed
- Scenario Selection
- Map Edit Mode

Maximum execution length:
100 ticks per scenario.

Step Backward must inspect saved snapshots
without reversing the actual C controller.

Reset must restore:
- Controller state
- Internal timers
- Initial RVC position
- Initial RVC direction
- Initial map configuration

## 6. Tick Execution

Define one simulation tick as one
complete system update.

At each tick:
1. Read the current map and RVC position.
2. Determine sensor inputs.
3. Execute one controller update.
4. Obtain controller outputs.
5. Apply movement and cleaning effects.
6. Update the map state.
7. Record a complete state snapshot.
8. Evaluate applicable test assertions.
9. Render the updated state.

Preserve the exact timing and output
semantics of the actual C implementation.

If the controller requires multiple
clock edges per logical update,
document the mapping explicitly.

## 7. State Inspection

Provide a State Inspector panel.

Display every relevant state variable
defined in the C implementation.

At minimum, when supported:
- Tick
- Position X
- Position Y
- Direction
- Controller State
- Front Sensor
- Left Sensor
- Right Sensor
- Dust Sensor
- Motor Output
- Cleaner Output
- Power Timer
- Turn Timer
- Internal Control Flags
- Previous State
- Current State
- Transition/Event

Do not fabricate unavailable state values.

Expose the necessary internal state through
a read-only interface if required.

## 8. Execution History

Record a complete snapshot for each tick.

Include:
- Initial state (Tick 0)
- Input values
- Controller state
- Internal variables
- Output values
- Map changes
- Test assertions and results

Provide:
- Tick navigation
- State history table
- State comparison
- Exportable execution trace

## 9. Test Execution

Support:
- Predefined test scenarios
- User-created maps
- Requirement-based assertions
- PASS / FAIL / INCONCLUSIVE results
- First failing tick
- Expected and actual behavior
- Requirement traceability

A scenario passes only when its defined
verification conditions are satisfied.

Reaching Tick 100 without a violation
does not automatically prove success.

## 10. User Interface Layout

The main screen should contain:

### Map Panel
- Editable 10x10 grid
- RVC position and direction
- Obstacle and dust visualization

### Control Panel
- Start / Pause / Step / Reset
- Playback speed
- Tick counter

### State Inspector
- Complete controller state
- Sensor inputs
- Motor output
- Cleaner output
- Internal timers and variables

### Scenario Panel
- Scenario list
- Requirements under test
- Expected behavior
- Test execution status

### History Panel
- Tick-by-tick execution trace
- State transitions
- Assertion failures

## 11. Implementation Constraints

- The production controller must remain in C11.
- The web simulator may use HTML, CSS,
  and JavaScript or TypeScript.
- Use GCC for native C builds.
- Use an appropriate WebAssembly toolchain
  if running C directly in the browser.
- Preserve the original controller logic.
- Keep map and visualization logic separate.
- Do not hardcode expected outputs
  as simulated controller behavior.
