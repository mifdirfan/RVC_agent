# Process Specification

## Process Identification

| Field | Description |
|---|---|
| Process ID | DFD Process Number |
| Process Name | DFD Process Name |
| DFD Level | Level 0, 1, 2, or 3 |
| Parent Process | Parent Process ID or None |

## Purpose
Describe the primary function of the process.

## Inputs
List all input data and control flows.

## Outputs
List all output data and control flows.

## Trigger / Execution Condition
Describe when the process is executed.

## Processing Logic
Describe the processing procedure
using numbered steps or pseudocode.

## Relevant Requirements
List the requirements supporting this process.

---

# Example: Front Sensor Interface

## Process Identification

| Field | Description |
|---|---|
| Process ID | 1.1 |
| Process Name | Front Sensor Interface |
| DFD Level | 2 |
| Parent Process | 1 |

## Purpose
Receive the front sensor input and
determine whether an obstacle is detected.

## Inputs
- Front Sensor Input

## Outputs
- Front Obstacle

## Trigger / Execution Condition
- When the front sensor input is received.

## Processing Logic
1. Receive the Front Sensor Input.
2. Interpret the sensor value.
3. Determine whether a front obstacle exists.
4. Output the Front Obstacle result.

## Pseudocode

IF Front Sensor Input == TRUE THEN
    Front Obstacle = TRUE
ELSE
    Front Obstacle = FALSE
END IF

## Relevant Requirements
- Reference the corresponding requirement ID.

---

# Formatting Rules
- Use one section for each DFD process.
- Group specifications by DFD level.
- Preserve hierarchical process numbering.
- Describe processing logic explicitly.
- Match all input/output names with the DFD.
- Use pseudocode when the logic includes conditions.
- Do not introduce behavior not supported
  by requirements or DFD artifacts.
- Do not generate specifications for Level 4 FSM.
