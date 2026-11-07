
# Controller FSM Template

## Purpose
Represent the Controller process from
DFD Level 3 as a Finite State Machine.

## Inputs
- DFD Level 3 Controller inputs
- Control events and conditions
- Preliminary requirements

## FSM Notation
- State: Rounded rectangle in stateDiagram-v2.
- Initial State: [*]
- Transition: Directed arrow
- Transition Label: Event [Condition] / Action

## Mermaid Format

```mermaid
stateDiagram-v2
    FwdNormal : Move Forward (Normal Cleaning)
    FwdPowerUp : Move Forward (Power-Up Cleaning)
    TurnLeft : Turn Left
    TurnRight : Turn Right
    MoveBackward : Move Backward

    [*] --> FwdNormal : Init / Enable "Move Forward"; Trigger "Clean Normal"
    FwdNormal --> TurnLeft : Tick [F && !L] / Disable "Move Forward"; Trigger "Clean Off"; Trigger "Turn Left"
    FwdNormal --> MoveBackward : Tick [F && L && R] / Disable "Move Forward"; Trigger "Clean Off"; Enable "Move Backward"
    TurnLeft --> FwdNormal : Tick x3 [!F] / Enable "Move Forward"; Trigger "Clean Normal"
```

## Required Elements
1. Initial state
2. Controller states
3. Input events
4. Transition conditions
5. Output actions
6. State transitions

## Transition Description

Use the following format:

Event [Condition] / Action

For example:
Tick [F && L && R] / Disable "Move Forward"; Trigger "Clean Off"; Enable "Move Backward"

## Transition Table

| # | Current State | Event | Condition | Actions (in order) | Next State | Source |
|---|---|---|---|---|---|---|
| T1 | [*] | Init | - | Enable "Move Forward"; Trigger "Clean Normal" | FwdNormal | C22, C27 |
| T2 | FwdNormal | Tick | F && !L | Disable "Move Forward"; Trigger "Clean Off"; Trigger "Turn Left" | TurnLeft | R3, C4, C8 |

Also add a State Table: State | Active motor process | Cleaner mode | Timer used.

## Validation Rules
- All states must be defined.
- Every transition must have a valid source and target.
- All input events must originate from requirements or DFD.
- All output actions must match Controller outputs.
- Check for conflicting transition conditions.
- Check for missing transition conditions.
- Define priority when multiple conditions overlap.
- Identify unreachable states.
- Identify states with no valid outgoing transition.
- Do not invent unspecified transitions.
- Allowed actions only: Enable "X", Disable "X", Trigger "X".
- Never Trigger "Move Backward"; never Enable/Disable a Clean process.
- Durations come from clarifications.md.

## Output
Save the FSM and Transition Table to:
artifacts/SA/DFD/dfd_level_4_fsm.md
