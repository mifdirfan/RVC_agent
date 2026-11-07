# RVC System Test Scenarios

## 1. Test Design Policy

Generate at least 20 test scenarios based on
preliminary requirements and approved SA artifacts.

Each scenario must:
- Use a 10x10 grid.
- Execute for a maximum of 100 ticks.
- Define the initial RVC position and direction.
- Define obstacle and dust configurations.
- Evaluate at least 3 essential system functions.
- Identify related requirements and SA artifacts.
- Describe expected behavior for human evaluation.
- Record complete state traces at every tick.

The test oracle is the human evaluator,
not an automated assertion system.

## 2. Scenario Format

For each scenario, document:

### Scenario ID and Name
- ID: ST-XX
- Name: Descriptive scenario name

### Initial Configuration
- Grid: 10x10 and RVC Start: (4,4) → (4,4), the center convention for a 0-9 grid
- RVC Start: (4,4) unless overridden
- Initial Direction:
- Obstacles: Explicit coordinates
- Dust: Explicit coordinates
- Maximum Ticks: 100

### Test Objectives
- Essential Function 1:
- Essential Function 2:
- Essential Function 3:

### Requirement Traceability
- Related Requirement IDs:
- Related SA Artifacts:

### Expected Behavior
Describe the expected RVC behavior based on
requirements and approved SA artifacts.

### Human Evaluation
- Result: PASS / FAIL / INCONCLUSIVE
- Evaluator:
- Observations:
- Related Tick(s):

## 3. Scenario Generation

1. Identify essential system functions from
   requirements and SA artifacts.
2. Generate at least 20 distinct scenarios.
3. Ensure each scenario evaluates at least
   3 essential functions.
4. Define map configurations that exercise
   the selected functions.
5. Describe expected behavior without
   inventing unsupported functionality.
6. Maintain requirement traceability.

## 4. Test Execution and Evaluation

1. Load the selected scenario.
2. Execute the simulation for up to 100 ticks.
3. Display the RVC's complete state at every tick.
4. Allow the evaluator to inspect the grid,
   state transitions, sensor inputs, and outputs.
5. Allow the human evaluator to assign
   PASS, FAIL, or INCONCLUSIVE.
6. Record evaluation results and observations.
7. Preserve the complete execution history.

Do not automatically assign PASS or FAIL.

## 5. Requirement Traceability

| Scenario ID | Essential Functions | Requirement IDs | SA Artifacts | Human Result |
|---|---|---|---|---|
| ST-XX | Function 1, 2, 3 | REQ-XX | FSM / Event List / Process Specification | Pending |

Use actual requirement identifiers.
Do not invent requirement references.

## 6. Custom Map Testing

Allow users to:
1. Modify obstacle and dust locations.
2. Change the RVC start position and direction.
3. Execute the modified map for up to 100 ticks.
4. Inspect all RVC states and outputs.
5. Evaluate behavior manually.
6. Save the modified map as a new scenario.

## 7. Completion Criteria

- At least 20 distinct scenarios are provided.
- Each scenario evaluates at least 3 essential functions.
- All scenarios have requirement traceability.
- Each scenario executes for at most 100 ticks.
- Complete tick-by-tick state inspection is available.
- Human evaluators can record PASS, FAIL,
  or INCONCLUSIVE with observations.
- Custom maps can be tested and saved.
