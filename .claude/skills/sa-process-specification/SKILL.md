---
name: sa-process-specification
description: Incrementally generate and refine Process Specifications after each DFD level (0-3), preserving existing definitions and maintaining consistency with requirements and the Data Dictionary.
---

# Process Specification Generation

## Inputs
- docs/preliminary_requirements.md
- docs/clarifications.md
- artifacts/SA/event_list.md
- artifacts/SA/context_diagram.md
- artifacts/SA/data_dictionary.md (if available)
- artifacts/SA/process_specification.md (if available)
- artifacts/SA/DFD/dfd_level_0.md (if available)
- artifacts/SA/DFD/dfd_level_1.md (if available)
- artifacts/SA/DFD/dfd_level_2.md (if available)
- artifacts/SA/DFD/dfd_level_3.md (if available)

## Objective
Describe the detailed behavior of every
process defined in DFD Levels 0 through 3.

Do not generate a separate Process Specification
for the Level 4 Controller FSM.


## Procedure

### Step 1. Identify Processes
1. Read the newly generated DFD level.
2. Read the existing Process Specifications (if available).
3. Identify newly introduced processes and their numbers.
4. Identify parent-child relationships.
5. Identify all input and output flows.

### Step 2. Analyze Process Behavior
For each process:
1. Identify its purpose.
2. Identify input data and control signals.
3. Identify output data and control signals.
4. Determine the processing logic.
5. Identify execution conditions and triggering events.
6. Identify interactions with Data Stores.

### Step 3. Generate or Update Process Specifications
1. Initialize Process Specifications if they do not exist.
2. Add specifications for newly introduced processes.
3. Refine existing specifications when additional details become available.
4. Preserve previously documented specifications.
5. Save the updated Process Specifications.

For each process, document:
- Process ID
- Process Name
- Parent Process
- Purpose
- Inputs
- Outputs
- Trigger or Execution Condition
- Processing Logic
- Relevant Requirements

### Step 4. Validate Hierarchical Consistency
1. Verify that each child process refines its parent process.
2. Verify that process inputs and outputs match the DFD.
3. Verify consistency with the Data Dictionary.
4. Verify consistency with the preliminary requirements.


## Guidelines
- Update Process Specifications after each DFD level (0-3).
- Preserve existing specifications during incremental updates.
- Use the same process names and numbers as the DFD.
- Describe processing logic explicitly, using pseudocode when appropriate.
- Maintain traceability to the preliminary requirements.
- Do not invent unspecified behavior.
- Report missing or ambiguous requirements.
- Do not generate Process Specifications for the Level 4 Controller FSM.
- Use the lecture table format: Ref. No. | Name | Input | Output | Process Description.
- Re-check every Open Issue against clarifications.md. If a clarification resolves it, mark it "Resolved (Cn)", update the affected definitions/logic, and keep it listed for traceability.
- Level 3 signal-type rules (C24, C25, C26):
  - Enable/Disable processes (2.1.2 Move Forward, 2.1.5 Move Backward):
    Trigger/Execution Condition = "Active from Enable until Disable."
    Logic = "On Enable, produce Motor Command once (Forward or Backward). Stay active,
    producing nothing further, until Disable."
    For 2.1.5, state that reversing continues until 2.1.1 disables it,
    after at least 3 ticks, when a side opens (C9).
  - Trigger processes (2.1.3, 2.1.4, 2.1.6, 2.1.7, 2.1.8):
    Trigger/Execution Condition = "Runs once per Trigger."
    Logic = "Produce one command, then end." Commands:
    Turn Left -> Motor Command Turn Left; Turn Right -> Motor Command Turn Right;
    Clean Normal -> Cleaner Command clean_on; Clean Off -> Cleaner Command clean_off;
    Power-Up Clean -> Cleaner Command clean_up.
  - 2.1.1 Controller: list its inputs and its control signals out, the timing rules
    (C5, C6, C9), the priority rules (front before dust, left before right, C4),
    and "issues signals only on a state transition". Refer to the Level 4 FSM
    for state logic; do not write a separate spec for the FSM.


## Formatting
Follow templates/process-specification.md.


## Validation
- Every process in the generated DFD Levels 0-3 must have a corresponding specification.
- Process IDs, names, inputs, and outputs must match the DFD.
- Child process behavior must be consistent with its parent process.
- Processing logic must be consistent with the requirements and Data Dictionary.
- No existing process specifications may be lost during an update.


## Output
- artifacts/SA/process_specification.md
