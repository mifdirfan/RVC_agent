
# RVC Project Instructions

## Project Overview
- Develop a Robotic Vacuum Cleaner (RVC) system.
- Use SASD-Guided Spec-Driven Development (SDD).
- Use AI Coding Agents to support the development process.

## Development Process
- Follow the SASD-Guided SDD workflow.
- Complete Structured Analysis (SA) before Structured Design (SD).
- Complete SD before implementation.
- Follow Test-Driven Development (TDD) during implementation.
- Perform system testing after implementation.

## Implementation Constraints
- Implement production code in C.
- Use C11 as the language standard.
- Use GCC for compilation.

## Agent Responsibilities
- Follow the defined development workflow.
- Use approved artifacts from previous phases.
- Maintain consistency between development artifacts.
- Do not proceed to the next phase without human approval.
- Report missing or ambiguous requirements.
- Do not claim verification success without executing tests.


## Structured Analysis Guidelines
- Generate SA artifacts using the corresponding skills.
- Generate the Event List before the System Context Diagram.
- Use preliminary_requirements.md as the primary source.
- Ensure consistency between related SA artifacts.
- Do not invent requirements or unsupported behavior.
- Save all SA artifacts under artifacts/SA/.


## Structured Analysis Workflow

1. Generate the Event List from preliminary requirements.
2. Generate the System Context Diagram from the Event List.
3. Generate DFD Levels 0 through 3 using hierarchical functional decomposition, incrementally updating the Data Dictionary and Process Specifications at each level.
4. Generate the Controller FSM as DFD Level 4.
5. Verify consistency across all SA artifacts.
6. Present the completed SA artifacts for human approval before proceeding to SD.


## Structured Analysis Constraints
- Use `preliminary_requirements.md` as the primary source.
- Preserve consistency between the Event List and Context Diagram.
- Maintain DFD balancing between adjacent levels.
- Use consistent process numbering and flow names.
- Update the Data Dictionary and Process Specifications after each DFD level (0-3).
- Describe every process in DFD Levels 0-3.
- Do not generate Process Specifications for the Level 4 FSM.
- Use Mermaid for DFD and FSM diagrams.
- Do not invent unsupported requirements or behavior.



## Structured Design Guidelines
- Generate SD artifacts using the corresponding skills.
- Perform Transform Analysis before generating the Structure Chart.
- Use approved SA artifacts as the primary source.
- Derive the module hierarchy from SA functional decomposition.
- Apply information hiding, modularity, low coupling, and high cohesion.
- Ensure consistency and traceability between SA and SD artifacts.
- Do not introduce unsupported functionality or behavior.
- Save all SD artifacts under artifacts/SD/.


## Structured Design Workflow

1. Perform Transform Analysis using approved SA artifacts.
2. Identify Afferent Flow, Central Transformation, and Efferent Flow.
3. Derive a hierarchical module structure from the functional decomposition.
4. Generate the Structure Chart using Mermaid.
5. Verify consistency with SA artifacts and obtain human approval before TDD.

## Structured Design Constraints
- Use approved SA artifacts as the design basis.
- Distinguish module calls from DFD data flows.
- Apply information hiding, modularity, low coupling, and high cohesion.
- Define clear module responsibilities and interfaces.
- Maintain traceability between SA processes and SD modules.
- Do not introduce unsupported system behavior.
- Do not proceed to TDD without human approval.



## Required Deliverables

### Structured Analysis (SA)
- Context Diagram
- Event List
- Data Flow Diagrams (DFD Levels 0-4)
- Finite State Machine (FSM)
- Data Dictionary
- Process Specifications

### Structured Design (SD)
- Transform Analysis
- Structure Chart

### Test-Driven Development (TDD)
- Unit Test Cases
- Test Execution Results
- Test Coverage Report
- Implementation Source Code

### System Testing
- Web-based RVC Simulator
- System Test Scenarios

## Testing Policy
- Derive test cases from requirements and approved SA artifacts.
- Follow the Red-Green-Refactor cycle.
- Measure and report test coverage.
- Do not proceed when required tests fail.
- Execute system tests using a web-based RVC simulator.
