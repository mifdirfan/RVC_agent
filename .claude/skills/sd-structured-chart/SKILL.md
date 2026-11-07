---
name: sd-structured-chart
description: Perform Structured Design using approved Structured Analysis artifacts. Apply Transform Analysis to identify Afferent Flow, Central Transformation, and Efferent Flow, then generate a hierarchical Structure Chart in Mermaid.
---

# Structured Design

## Inputs
- preliminary_requirements.md
- clarifications.md
- artifacts/SA/event_list.md
- artifacts/SA/context_diagram.md
- artifacts/SA/data_dictionary.md
- artifacts/SA/process_specification.md
- artifacts/SA/DFD/dfd_level_0.md
- artifacts/SA/DFD/dfd_level_1.md
- artifacts/SA/DFD/dfd_level_2.md
- artifacts/SA/DFD/dfd_level_3.md
- artifacts/SA/DFD/dfd_level_4_fsm.md
- artifacts/SA/sa_consistency_report.md

## Objective
Transform the functional decomposition
defined in Structured Analysis into a
hierarchical software module structure.

Perform Transform Analysis and generate
a Structure Chart using Mermaid.

## Procedure

### Step 1. Review SA Artifacts
1. Read all approved SA artifacts.
2. Identify system functions and their relationships.
3. Identify data processes and control processes.
4. Identify input/output data and control flows.
5. Identify Controller behavior from the FSM.
6. Identify process dependencies and interfaces.

### Step 2. Perform Transform Analysis
1. Analyze the DFD hierarchy, focusing on Level 3.
2. Identify the Afferent Flow:
   - Input acquisition
   - Sensor interfaces
   - Input processing
3. Identify the Central Transformation:
   - Main processing logic
   - Controller
   - Decision-making functions
4. Identify the Efferent Flow:
   - Output generation
   - Motor control
   - Cleaner control
5. Identify transform boundaries between the regions.
6. Map each relevant DFD process to a region.
7. Document the Transform Analysis results.
8. Follow templates/transform-analysis.md.


### Step 3. Design Control Hierarchy
1. Define `Main` as the root module,
   corresponding to the C `main()` function.
2. Place the `Controller` module under `Main`
   to coordinate the system behavior.
3. Organize input, processing, and output modules
   hierarchically under the appropriate callers.
4. Determine caller-callee relationships
   based on the approved SA artifacts.
5. Define execution order, conditional calls,
   and repeated calls.
6. Ensure Controller behavior is consistent
   with the FSM.
7. Distinguish module calls from DFD data flows.


### Step 4. Perform Module Decomposition
1. Derive modules from SA processes.
2. Group related functions into cohesive modules.
3. Separate independent responsibilities.
4. Identify reusable or library modules.
5. Apply information hiding and modularity.
6. Minimize coupling between modules.
7. Maintain traceability between SA processes
   and SD modules.

### Step 5. Define Module Interfaces
For each module:
1. Define its responsibility.
2. Identify input parameters.
3. Identify output or return values.
4. Identify data and control information exchanged.
5. Identify called modules.
6. Identify calling modules.
7. Identify corresponding SA processes.


### Step 6. Generate Structure Chart
1. Represent the module hierarchy using
   Mermaid flowchart TB.
2. Place `Main` at the top of the hierarchy.
3. Represent modules using rectangular nodes.
4. Represent caller-callee relationships
   using directed arrows.
5. Distinguish data and control information
   exchanged between modules.
6. Represent conditional and repeated calls.
7. Identify reusable or library modules.
8. Follow templates/structured-chart.md.
9. Controller -> action module arrows carry the control couple type from C24/C25:
   label "Enable/Disable" for Move Forward and Move Backward, "Trigger" for Turn Left,
   Turn Right, Clean Normal, Clean Off, Power-Up Clean.
10. Move Forward and Move Backward expose x_enable() / x_disable(); trigger modules expose x_trigger().
11. Motor Interface and Cleaner Interface are library modules [[ ]] that apply C12.
12. The order of calls inside Controller follows the FSM transition actions exactly.
13. Decide the counter representation (turn, reverse, power-up) and record it in the module interface table.


### Step 7. Validate Structured Design
1. Verify functional coverage of SA processes.
2. Verify module responsibilities and interfaces.
3. Verify caller-callee relationships.
4. Verify consistency with Process Specifications.
5. Verify Controller behavior against the FSM.
6. Verify SA-to-SD traceability.
7. Evaluate information hiding, modularity,
   low coupling, and high cohesion.
8. Report unresolved inconsistencies.

## Design Guidelines
- Use approved SA artifacts as the design basis.
- Perform Transform Analysis before creating
  the Structure Chart.
- Do not directly copy DFD data flows
  as module calls.
- Preserve all required system functionality.
- Each module must have a clear responsibility.
- Avoid unnecessary module dependencies.
- Do not invent unsupported behavior.
- Do not generate implementation code.
- Do not proceed to TDD without human approval.

## Formatting
- Follow templates/transform-analysis.md.
- Follow templates/structured-chart.md.
- Use Mermaid for all diagrams.

## Outputs
- artifacts/SD/transform_analysis.md
- artifacts/SD/structure_chart.md


## Completion Criteria
- Transform boundaries are identified.
- Afferent, Central, and Efferent regions are defined.
- `Main` is the root module of the Structure Chart.
- The Structure Chart represents the actual
  intended C function call hierarchy.
- Module interfaces and responsibilities are defined.
- SA-to-SD traceability is maintained.
- Design consistency is validated.
