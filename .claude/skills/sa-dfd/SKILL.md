---
name: sa-dfd
description: Generate hierarchical Data Flow Diagrams (DFD Levels 0-3) and a Level 4 Controller FSM using preliminary requirements, the Event List, and the System Context Diagram.
argument-hint: "[0|1|2|3|4]"
---

# Data Flow Diagram Generation

## Inputs
- preliminary_requirements.md
- clarifications.md
- artifacts/SA/event_list.md
- artifacts/SA/context_diagram.md
- artifacts/SA/data_dictionary.md(if available)
- artifacts/SA/DFD/dfd_level_0.md(if available)
- artifacts/SA/DFD/dfd_level_1.md(if available)
- artifacts/SA/DFD/dfd_level_2.md(if available)
- artifacts/SA/DFD/dfd_level_3.md(if available)


## Objective
Perform functional decomposition of the system
using hierarchical Data Flow Diagrams.

Generate DFD Levels 0 through 3 and represent
the Controller as an FSM at Level 4.

## Execution
Generate ONLY the level given in $ARGUMENTS, run balancing with the previous level, save, and stop.

## Reference Decomposition
L0: 0 RVC Control
L1: 1 Obstacle & Dust Detection, 2 Cleaner & Motor Control
L2-P1: 1.1-1.4 Sensor Interfaces, 1.5 Determine Obstacle Location, 1.6 Determine Dust Existence
L2-P2: 2.1 Main Control, 2.2 Motor Interface, 2.3 Cleaner Interface
L3-P2.1: 2.1.1 Controller (control process), 2.1.2 Move Forward, 2.1.3 Turn Left, 2.1.4 Turn Right, 2.1.5 Move Backward, 2.1.6 Clean Normal, 2.1.7 Clean Off, 2.1.8 Power-Up Clean

## Procedure

### Step 1. Review Inputs
1. Read the preliminary requirements.
2. Read the Event List.
3. Read the System Context Diagram.
4. Identify external entities and data flows.
5. Identify system functions and control events.

### Step 2. Generate DFD Level 0
1. Use the System Context Diagram as the basis.
2. Represent the entire system as Process 0.
3. Identify external entities interacting with the system.
4. Represent input and output data flows.
5. Add control flows when supported by requirements.
6. Ensure consistency with the Context Diagram.

### Step 3. Generate DFD Level 1
1. Decompose Process 0 into major system functions.
2. Assign process numbers 1, 2, 3, etc.
3. Identify data and control flows between processes.
4. Introduce Data Stores when necessary.
5. Preserve the external interfaces of Level 0.
6. Verify balancing between Level 0 and Level 1.

### Step 4. Generate DFD Level 2
1. Decompose Level 1 processes into sub-processes.
2. Use hierarchical numbering:
   1.1, 1.2, 1.3, 2.1, 2.2, etc.
3. Identify detailed functional responsibilities.
4. Identify data and control flows.
5. Identify required Data Stores.
6. Verify balancing with each parent process.
7. Create separate diagrams for distinct
   parent processes when necessary.
8. Process 1 diagram:
   - Front Sensor Input (interrupt, dashed) -> 1.1; Tick -> 1.2, 1.3, 1.4.
   - 1.1-1.3 send Front/Left/Right Obstacle -> 1.5 Determine Obstacle Location,
     which writes the data store "Obstacle Location" (Front + Left + Right).
   - 1.4 sends Dust -> 1.6 Determine Dust Existence, which writes the data store "Dust Existence".
9. Process 2 diagram:
   - Data stores Obstacle Location and Dust Existence and Tick -> 2.1 Main Control.
   - 2.1 -> Motor Command -> 2.2 Motor Interface -> Direction.
   - 2.1 -> Cleaner Command -> 2.3 Cleaner Interface -> Clean.
   - 2.2 and 2.3 apply C12 (send only when the value changes).
10. In the balancing table, document that the Level 1 flows Obstacle Location and
    Dust Existence become data stores at Level 2.

### Step 5. Generate DFD Level 3
1. Select the processes that require further decomposition.
2. Decompose them into lower-level functions.
3. Use hierarchical numbering:
   2.1.1, 2.1.2, 2.1.3, etc.
4. Separate control and data processing functions.
5. Identify detailed data and control flows.
6. Preserve the parent process interfaces.
7. Verify balancing with Level 2.
8. Continue until the Controller can be
   represented using an FSM.
9. Decompose ONLY 2.1 Main Control, using the Reference Decomposition (2.1.1-2.1.8).
   Keep 1.1-1.6, 2.2 and 2.3 as they are, and list them under "Processes Not Decomposed" with a reason.

#### Signal Type Rules (C24, C25, C26).
| Process | Signal from 2.1.1 | Why |
|---|---|---|
| 2.1.2 Move Forward | Enable / Disable | Continuous until stopped |
| 2.1.3 Turn Left | Trigger | One-shot, lasts 3 ticks (C5) |
| 2.1.4 Turn Right | Trigger | One-shot, lasts 3 ticks (C5) |
| 2.1.5 Move Backward | Enable / Disable (C24) | Continuous, at least 3 ticks, until a side opens (C9) |
| 2.1.6 Clean Normal | Trigger (C25) | One-shot: sends clean_on once |
| 2.1.7 Clean Off | Trigger (C25) | One-shot: sends clean_off once |
| 2.1.8 Power-Up Clean | Trigger (C25) | One-shot: sends clean_up once (lasts 3 ticks, C6) |

- 2.1.1 Controller is the ONLY control process. Draw it as a dashed circle:
  `(("2.1.1<br/>Controller")):::control` and `classDef control stroke-dasharray: 5 5`.
- Draw Enable/Disable as TWO dashed arrows from 2.1.1, labeled `Enable` and `Disable`.
- Draw Trigger as ONE dashed arrow from 2.1.1, labeled `Trigger`.
- Never draw a Trigger to Move Forward or Move Backward.
  Never draw Enable/Disable to Clean Normal, Clean Off, or Power-Up Clean.
- Inputs to 2.1.1: data stores Obstacle Location and Dust Existence, and Tick.
  Tick goes ONLY to 2.1.1: the Controller times all durations (turn 3, reverse >= 3, power-up 3),
  because Trigger processes are one-shot.
- 2.1.2-2.1.5 send `Motor Command` to Process 2.2.
  2.1.6-2.1.8 send `Cleaner Command` to Process 2.3.
- Signals are issued only on a state transition (C26).
  Therefore Motor Command and Cleaner Command are produced only when the decision changes.
- After the diagram, add the Signal Type table above, plus a "Controller Interface" table
  (Events in | Conditions in | Control signals out). Level 4 will use it.
- Verify balancing with 2.1: its inputs (Obstacle Location, Dust Existence, Tick) and
  outputs (Motor Command, Cleaner Command) are unchanged.

### Step 6. Generate DFD Level 4 (Controller FSM)
1. Identify the Controller process from Level 3.
2. Read its inputs, outputs, and control flows.
3. Identify controller states.
4. Identify triggering events and transition conditions.
5. Identify actions associated with transitions.
6. Define the initial state.
7. Define all required state transitions.
8. Represent the FSM using Mermaid stateDiagram-v2.
9. Ensure consistency with the Level 3 Controller.
10. Use exactly these states: Move Forward (Normal Cleaning) [initial], Move Forward (Power-Up Cleaning),
    Turn Left, Turn Right, Move Backward. Do not add a Stop or Idle state.
11. The initial transition follows C27.
12. Cover every clarification that affects control and cite it in the Source column:
    turn 3 ticks (C5), power-up 3 ticks with no restart (C6, C14), reverse >= 3 ticks until a side opens (C9),
    dust ignored while turning/reversing (C10), obstacle cancels power-up (C11), side-only obstacle causes
    no transition (C13), front still blocked when a turn ends -> new avoidance decision (C15),
    clean_off only while turning/reversing (C8), left before right (C4), front before dust.
13. Before any Enable of a motor process, Disable the previously enabled one.
14. Check guard completeness: for each state on Tick, the guards over F/L/R/Dust/timer cover every case
    (state explicitly when the FSM stays in the same state).
15. Save the Mermaid diagram, the Transition Table, the State Table, and a Validation section to
    artifacts/SA/DFD/dfd_level_4_fsm.md.

## DFD Notation
- Data Process: Circular node.
- Control Process: Dashed circular node.
- Terminator: Rectangular node.
- Data Store: Data Store node.
- Data Flow: Solid directed arrow.
- Control Flow: Dashed directed arrow.

## Decomposition Rules
- Each lower level must refine a process
  from the immediately preceding level.
- Preserve the parent process input/output interface.
- Do not introduce unexplained external flows.
- Every process must have a meaningful function.
- Use consistent process numbering.
- Do not decompose functions without justification.
- Do not invent unsupported system behavior.

## Mermaid Guidelines
- Use flowchart LR for DFD Levels 0-3.
- Use stateDiagram-v2 for the Controller FSM.
- Follow templates/dfd-template.md.
- Follow templates/fsm-template.md.
- Label all data and control flows.
- Distinguish control flows from data flows.
- Use valid Mermaid syntax.

## Validation
- Verify DFD balancing between adjacent levels.
- Verify process numbering consistency.
- Verify input/output consistency.
- Verify data and control flow labels.
- Verify Controller FSM consistency with Level 3.
- Report missing or ambiguous requirements.
- Do not silently resolve inconsistencies.

## Outputs
- artifacts/SA/DFD/dfd_level_0.md
- artifacts/SA/DFD/dfd_level_1.md
- artifacts/SA/DFD/dfd_level_2.md
- artifacts/SA/DFD/dfd_level_3.md
- artifacts/SA/DFD/dfd_level_4_fsm.md
