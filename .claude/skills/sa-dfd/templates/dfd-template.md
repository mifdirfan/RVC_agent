# Data Flow Diagram Template

## Notation

| Element | Mermaid Representation |
|---|---|
| Data Process | Circular node (( )) |
| Control Process | Dashed circular node |
| Terminator | Rectangle [ ] |
| Data Store | Subroutine node [[ ]] |
| Data Flow | Solid arrow --> |
| Control Flow | Dashed arrow -.-> |
| Enable / Disable | Two dashed arrows: `C -.->\|Enable\| P` and `C -.->\|Disable\| P` |
| Trigger | One dashed arrow: `C -.->\|Trigger\| P` |
| Sibling process outside the diagram | Rectangle `["Process 2.2"]`; say under the diagram that it is not a terminator |

Note:
Mermaid does not provide an exact native
notation for traditional two-line DFD Data Stores.
Use a subroutine node as a consistent approximation.

## Process Numbering
- Level 0: 0
- Level 1: 1, 2, 3, ...
- Level 2: 1.1, 1.2, 2.1, 2.2, ...
- Level 3: 2.1.1, 2.1.2, 2.1.3, ...


## Formatting Rules
- Every diagram must use a Mermaid code block.
- Every process must have a name and number.
- Every flow must have a label.
- Use the same flow names across DFD levels.
- Distinguish control flows from data flows.
- Data Stores must have meaningful names.
- Use separate diagrams for different
  parent processes when appropriate.
- Do not copy example processes unless supported
  by the actual requirements.

## Balancing Rules
- Parent inputs must be preserved in the child DFD.
- Parent outputs must be preserved in the child DFD.
- Internal flows may be introduced during decomposition.
- If a parent flow is decomposed into multiple flows,
  document the mapping explicitly.
- Do not add unexplained external inputs or outputs.
