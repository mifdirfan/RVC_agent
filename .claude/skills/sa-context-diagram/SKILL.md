---
name: sa-context-diagram
description: Generate a Mermaid System Context Diagram using preliminary requirements and the Event List.
---

# System Context Diagram Generation

## Input
- preliminary_requirements.md
- clarifications.md
- artifacts/SA/event_list.md

## Procedure
1. Read the preliminary requirements.
2. Check whether artifacts/SA/event_list.md exists.
3. If the Event List does not exist, invoke the sa-event-list skill first.
4. Identify the system boundary.
5. Identify external entities interacting with the system.
6. Represent the entire system as a single process.
7. Identify input and output data flows.
8. Use event names from the Event List as arrow labels.
9. Follow the Mermaid format defined in templates/context-diagram.md.
10. Save the diagram to artifacts/SA/context_diagram.md.

## Guidelines
- Represent the entire system as a single process.
- Represent external entities as rectangular nodes.
- Represent the system process as a circular node.
- Use directed arrows for data flows.
- Label each arrow using the corresponding Event List entry.
- Ensure arrow directions match input/output relationships.
- Do not introduce additional internal processes.
- Do not invent external entities or data flows.
- Ensure consistency between the Event List and Context Diagram.
- Cite all clarifications currently in clarifications.md. If an artifact you read cites
  an older set or lists an Open Issue now resolved, report it in a "Stale artifacts" section.

## Output
- artifacts/SA/context_diagram.md
