---
name: impl-c-implementation
description: Implement approved Structured Design artifacts in C11, preserving the module hierarchy, interfaces, and call relationships defined in the Structure Chart.
---

# C Implementation

## Inputs
- clarifications.md
- artifacts/SD/structure_chart.md
- artifacts/SD/transform_analysis.md
- artifacts/SA/DFD/dfd_level_4_fsm.md
- artifacts/SA/process_specification.md
- artifacts/SA/data_dictionary.md
- .claude/rules/c-coding.md

## Objective
Implement the approved Structure Chart in C11,
preserving its module responsibilities, interfaces,
and caller-callee relationships.

## Implementation Workflow

### Step 1. Review Approved Design
1. Read the approved Structure Chart.
2. Identify all modules and their responsibilities.
3. Identify module interfaces and call relationships.
4. Review the Controller FSM and Process Specifications.

### Step 2. Map Modules to C
1. Map each Structure Chart module to a C function
   or module with a clearly defined responsibility.
2. Define function interfaces based on the
   approved module interfaces.
3. Preserve the caller-callee relationships
   defined in the Structure Chart.
4. Organize source and header files according
   to module responsibilities.
5. Document any necessary design deviations
   and obtain human approval before implementation.

### Step 3. Perform TDD
1. Use the `impl-tdd-development` skill.
2. Derive unit tests from requirements,
   SA artifacts, and module interfaces.
3. Write failing tests before implementation.
4. Implement minimal C11 code to pass the tests.
5. Follow the Red-Green-Refactor cycle.

### Step 4. Build and Verify
1. Build the project using CMake and GCC.
2. Execute GoogleTest through CTest.
3. Fix implementation defects.
4. Measure line and branch coverage.
5. Verify that the implemented call hierarchy
   matches the approved Structure Chart.
6. Verify that Controller behavior matches the FSM.

### Step 5. Report Results
1. Document implemented modules and interfaces.
2. Report unit test and coverage results.
3. Report any differences from the Structure Chart.
4. Maintain SD-to-Implementation traceability.

## Implementation Constraints
- Implement production code in C11.
- Preserve the Structure Chart module hierarchy.
- Preserve approved caller-callee relationships.
- Do not bypass required modules.
- Do not introduce unapproved module dependencies.
- Keep module responsibilities consistent
  with the approved Structured Design.
- Do not modify approved SD artifacts
  without human approval.

## Outputs
- src/
- include/
- tests/
- CMakeLists.txt
- artifacts/Implementation/module_traceability.md
- artifacts/Implementation/test_report.md

## Completion Criteria
- All required Structure Chart modules are implemented.
- Module interfaces match the approved design.
- Caller-callee relationships match the Structure Chart.
- Unit tests pass.
- Test coverage is reported.
- Controller behavior is consistent with the FSM.
- SD-to-Implementation traceability is maintained.
