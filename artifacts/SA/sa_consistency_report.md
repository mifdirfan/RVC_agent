# SA Consistency Report

Sources: `preliminary_requirements.md` (R1–R7), `clarifications.md` (C1–C29), and every SA artifact under `artifacts/SA/`:

| Artifact | Revision checked |
|---|---|
| `event_list.md` | Rev 3 |
| `context_diagram.md` | Rev 3 |
| `DFD/dfd_level_0.md` | current (Notes 1–4) |
| `DFD/dfd_level_1.md` | current (Notes 1–4) |
| `DFD/dfd_level_2.md` | current (Notes 1–7) |
| `DFD/dfd_level_3.md` | current (Notes 1–6) |
| `DFD/dfd_level_4_fsm.md` | current (Issues 1–6; Issue 6 = C29) |
| `data_dictionary.md` | Rev 8 |
| `process_specification.md` | Rev 8 (C29) |

## SA Approval Record

| Item | Value |
|---|---|
| Status | **Approved** by the project owner |
| Date | 2026-11-08 |
| Scope | All SA artifacts at the revisions listed above |
| Observations confirmed at approval | Section 7, items 2–6 |
| Next phase | Structured Design (`artifacts/SD/`). SD was approved on the same date |

## Overall result

**PASS.** All checks below pass. Every requirement R1–R7 and clarification C1–C29 is covered. No artifact has an open issue. Section 7 lists the observations confirmed at SA approval. None of them is a defect.

*C29 addendum (2026-11-08, text only).* C29 was added to `clarifications.md` after the consistency pass. It was applied in `dfd_level_4_fsm.md` (T16, T21–T23, Issue 6) and `process_specification.md` Rev 8. It makes explicit a behaviour that the artifacts already had, so no diagram, flow, store, data definition or transition changed. See Section 1a and the C29 row in Section 6.

## 1. Changes made in this consistency pass

Only text and citations were changed. No diagram, process name, process number, flow or store was changed.

| Artifact | Change |
|---|---|
| `event_list.md` | Rev 3. Cites C1–C28. Direction and Clean are sent once at start-up (C22, C27). Clean (clean_up) and Dust Sensor Input describe the end of a power-up (C28). Traceability now includes C21–C28. |
| `context_diagram.md` | Rev 3. Cites C1–C28. Motor and Cleaner rows cite C22 and C27, and the Cleaner and Digital Clock rows cite C28. Open Issues re-checked. |
| `dfd_level_0.md` | Cites C1–C28. Direction and Clean cite C16, C22, C27, and Clean also cites C28. New Note 4 (C21–C28: no effect on Level 0). |
| `dfd_level_1.md` | Cites C1–C28. Process 1 and 2 citations completed. Note 3: start-up send resolved (C16, C22, C27), plus C21/C23 and C28 items. Note 4 marked done. |
| `dfd_level_2.md` | Cites C1–C28. The 2.1 row now states the start-up commands (C27), the on-transition rule (C26) and the power-up end (C28). The 2.2 and 2.3 rows have no separate start-up send (C22, C27). The Data Stores table has a start-up value column (C21, C23). Note 5 resolved. New Note 6 (C24–C28). Note 7 (follow-ups) marked done. |
| `dfd_level_3.md` | Cites C1–C28. **S0 = Enable "Move Forward" + Trigger "Clean Normal" (C27).** Note 1 resolved (C24). Note 2 resolved (C27). The 2.2 and 2.3 "Processes Not Decomposed" rows no longer mention a start-up send. S4 and S5 cite C28. Note 6 marked done. |
| `dfd_level_4_fsm.md` | Cites C1–C28. **Issue 2 resolved (C28).** Issue 1 resolved (the follow-ups are done). T6 and T11 cite C28. C28 added to the clarification coverage table. |
| `data_dictionary.md` | Rev 8. C27 was already applied in Rev 7; re-checked with no change. C28 added to Clean (clean_up), Dust Existence, Cleaner Command, Trigger "Clean Normal", Trigger "Power-Up Clean" and the power-up tick count. Open Issue 13 added and resolved. Section 7 now lists no stale artifact. |
| `process_specification.md` | Rev 7. C27 was already applied in Rev 6; re-checked with no change. C28 added to the power-up logic of Processes 0, 2, 2.1 and 2.1.1 (timing rules, S4, S5), and to 2.1.6, 2.1.8 and the summary rows. Pseudocode comments only; the logic is unchanged. Open Issues 13 and 16 resolved. Rev 7 validation table added. |

## 1a. C29 re-check (after the consistency pass)

C29: "Dust on the tick a turn or reverse ends is ignored; it is acted on from the next tick if still present."

| Artifact | C29 status | Change needed |
|---|---|---|
| `dfd_level_4_fsm.md` | Cited on T16, T21, T22, T23, in the clarification coverage table and in Issue 6. None of these guards tests D | None |
| `process_specification.md` | Rev 8 cites C29 in Processes 0, 2, 2.1 and 2.1.1 (S6, S8, S9) and in 2.1.6 and 2.1.8. The pseudocode is unchanged, because the turn and reverse branches never tested D | None |
| `dfd_level_3.md` | S6, S8 and S9 list no dust condition, which matches C29. C29 is not cited by number | None. The behaviour already matches; the citation is optional |
| `data_dictionary.md` | No data name, type or value is affected. C29 is a timing rule of 2.1.1, not a data definition | None |
| `event_list.md`, `context_diagram.md`, `dfd_level_0.md` – `dfd_level_2.md` | The external interface is unchanged: there is no new event and no new flow | None |

Result: **PASS.** C29 is consistent with every artifact. No artifact other than this report was changed for this addendum.

## 2. DFD balancing (Context, L0, L1, L2, L3)

Flow labels were extracted from the Mermaid blocks and compared level by level.

| Boundary | Parent interface | Child interface | Result |
|---|---|---|---|
| Context → L0 | In: Front/Left/Right/Dust Sensor Input, Tick. Out: Direction, Clean | Same 5 in and 2 out on Process 0, with the same styles (Front and Tick dashed) | **PASS** |
| L0 → L1 (Process 0) | 5 in, 2 out | 1 receives the 4 sensor inputs and Tick; 2 receives Tick; 2 sends Direction and Clean. Tick is the same flow split to both. Obstacle Location and Dust Existence are internal. | **PASS** |
| L1 → L2 (Process 1) | In: 4 sensor inputs, Tick. Out: Obstacle Location, Dust Existence | Sensor inputs → 1.1–1.4; Tick → 1.2, 1.3, 1.4; 1.5 → Obstacle Location store; 1.6 → Dust Existence store. The output flows become stores with the same names. | **PASS** |
| L1 → L2 (Process 2) | In: Obstacle Location, Dust Existence, Tick. Out: Direction, Clean | Both stores → 2.1; Tick → 2.1; 2.2 → Direction; 2.3 → Clean. Motor Command and Cleaner Command are internal. | **PASS** |
| L2 → L3 (Process 2.1) | In: Obstacle Location, Dust Existence, Tick. Out: Motor Command, Cleaner Command | Both stores and Tick → 2.1.1; Motor Command from 2.1.2–2.1.5; Cleaner Command from 2.1.6–2.1.8. The 9 control signals are internal. | **PASS** |
| L2 primitives | 1.1–1.6, 2.2, 2.3 | Not decomposed, and listed as primitives in `dfd_level_3.md` | **PASS** |

External entities: the same 8 terminators appear at the Context level and at L0–L2. Only the Digital Clock appears at L3. None is added or removed.

## 3. FSM actions vs Level 3 control arrows

| Level 3 dashed arrow from 2.1.1 | FSM action | FSM transitions | Result |
|---|---|---|---|
| Enable → 2.1.2 Move Forward | Enable "Move Forward" | T1, T16, T21 | **PASS** |
| Disable → 2.1.2 Move Forward | Disable "Move Forward" | T2–T4, T7–T9 | **PASS** |
| Trigger → 2.1.3 Turn Left | Trigger "Turn Left" | T2, T7, T13, T19, T22 | **PASS** |
| Trigger → 2.1.4 Turn Right | Trigger "Turn Right" | T3, T8, T14, T18, T23 | **PASS** |
| Enable → 2.1.5 Move Backward | Enable "Move Backward" | T4, T9, T15, T20 | **PASS** |
| Disable → 2.1.5 Move Backward | Disable "Move Backward" | T22, T23 | **PASS** |
| Trigger → 2.1.6 Clean Normal | Trigger "Clean Normal" | T1, T11, T16, T21 | **PASS** |
| Trigger → 2.1.7 Clean Off | Trigger "Clean Off" | T2–T4, T7–T9 | **PASS** |
| Trigger → 2.1.8 Power-Up Clean | Trigger "Power-Up Clean" | T6 | **PASS** |

- There are 9 arrows and 9 distinct FSM actions, one to one. The FSM uses no other action.
- The FSM event is Tick only, which is the only event into 2.1.1. Init is the start-up pseudo-event.
- The FSM conditions are F, L, R, D and the three counters, the same as the L3 Controller Interface and DD Section 4.6.
- Every Level 3 situation S0–S9 maps to FSM transitions, and the no-signal ticks map to the stay rows T5, T10, T12, T17, T24 and T25. **S0 now matches T1 (C27).**
- No Trigger goes to a continuous process, and no Enable or Disable goes to a one-shot process (C24, C25).

## 4. Data Dictionary coverage of flows and stores

| DFD element | Levels | DD entry | Result |
|---|---|---|---|
| Front / Left / Right / Dust Sensor Input | Ctx, 0, 1, 2 | Section 1, 4.1 | **PASS** |
| Tick | Ctx, 0–3 | Section 1, 4.1 | **PASS** |
| Direction, Clean | Ctx, 0, 1, 2 | Section 1, 4.1 | **PASS** |
| Obstacle Location (flow) | 1, 2, 3 | Section 2, 4.2 | **PASS** |
| Dust Existence (flow) | 1, 2, 3 | Section 2, 4.2 | **PASS** |
| Front / Left / Right Obstacle | 1 (component), 2 | Section 2, 4.2 | **PASS** |
| Dust | 2 | Section 2, 4.2 | **PASS** |
| Motor Command, Cleaner Command | 2, 3 | Section 2, 4.2, 4.4 | **PASS** |
| 9 control signals (Enable / Disable / Trigger) | 3 | Section 2.1, 4.5 | **PASS** |
| Obstacle Location (store) | 2, 3 | Section 3, 4.3 | **PASS** |
| Dust Existence (store) | 2, 3 | Section 3, 4.3 | **PASS** |

All 18 distinct labels in the DFD diagrams (with Enable, Disable and Trigger resolved to their 9 named signals) and both stores have entries. The DD has no entry that is missing from the diagrams. Controller-internal data (movement, mode and counters) is in DD Section 4.6 and is correctly not shown as a store.

## 5. Process Specification coverage of L0–L3 processes

| Level | Processes in diagram | PS entry | Result |
|---|---|---|---|
| 0 | 0 | Process 0 | **PASS** |
| 1 | 1, 2 | Processes 1, 2 | **PASS** |
| 2 | 1.1–1.6, 2.1–2.3 | Processes 1.1–1.6, 2.1–2.3 | **PASS** |
| 3 | 2.1.1–2.1.8 | Processes 2.1.1–2.1.8 | **PASS** |
| 4 | FSM of 2.1.1 | None, as the project rules require | **PASS** |

There are 20 processes in the diagrams and 20 specifications, with the same IDs and names. The inputs and outputs of each specification match its diagram arrows.

## 6. Coverage of requirements and clarifications

"Where" names the main places each item is applied. Every item is also cited in the DD and PS.

### Requirements

| ID | Statement | Where | Result |
|---|---|---|---|
| R1 | Automatically cleans and mops | Clean output; clean_on / clean_up modes; 2.1.6, 2.1.8 | **PASS** |
| R2 | Goes straight forward while cleaning | FwdNormal; Enable Move Forward; 2.1.2 | **PASS** |
| R3 | Obstacle → stop cleaning, turn left/right, resume forward cleaning | S1, S2, S6; T2, T3, T7, T8, T16, T21; Clean Off | **PASS** |
| R4 | Front, left and right blocked → backward, turn, forward | S3, S8, S9; T4, T9, T15, T20, T22, T23; 2.1.5 | **PASS** |
| R5 | Dust → power up cleaning for a while | S4, S5; T6, T11; 2.1.8 | **PASS** |
| R6 | HW control details out of scope | Sensors, clock and actuators are abstract terminators; tick period out of scope (C17) | **PASS** |
| R7 | Automatic cleaning function only | No user, charging or battery events (Event List, Context Diagram) | **PASS** |

### Clarifications

| ID | Topic | Where | Result |
|---|---|---|---|
| C1 | Front sensor is interrupt-driven | Dashed Front Sensor Input; 1.1; front part of the store written at once | **PASS** |
| C2 | Left, right, dust are periodic | 1.2–1.4 on Tick | **PASS** |
| C3 | Sensor values are Boolean | DD types; 1.1–1.4 pass values unchanged | **PASS** |
| C4 | Left turn has priority | `!L` tested before `L && !R` in every avoidance choice | **PASS** |
| C5 | Rotation lasts 3 ticks | `turnDone`; T12, T17 stay | **PASS** |
| C6 | Power-up lasts 3 ticks (interpretation recorded in C6) | `upDone`; T10, T11 | **PASS** |
| C7 | Modes clean_off / clean_on / clean_up | Clean and Cleaner Command value sets | **PASS** |
| C8 | clean_off only when reversing or rotating | Clean Off only on entry to a turn or reverse; Clean Normal on every exit to forward | **PASS** |
| C9 | Reverse ≥ 3 ticks, while both sides blocked, ends with a turn (left first) | `revMin`; T22–T25 | **PASS** |
| C10 | Dust ignored while turning or reversing | No D guard in TurnLeft, TurnRight or MoveBackward | **PASS** |
| C11 | Obstacle cancels power-up | T7–T9 with Clean Off | **PASS** |
| C12 | Direction and Clean sent only on change | 2.2 and 2.3 compare-and-send | **PASS** |
| C13 | Side-only obstacle: no reaction | T5, T10 | **PASS** |
| C14 | Dust during power-up does not restart it | T10 (D not tested) | **PASS** |
| C15 | Front not acted on during turn/reverse; new decision if still blocked when a turn ends | T12, T17, T24; T13–T15, T18–T20 | **PASS** |
| C16 | Start-up Direction = Forward, Clean = clean_on | FwdNormal initial state; T1 | **PASS** |
| C17 | Abstract tick; sampled once per tick | Tick definition; 1.2–1.4 | **PASS** |
| C18 | Turn decision uses latest stored left/right | 2.1.1 reads the Obstacle Location store on Tick | **PASS** |
| C19 | Front = false only updates the store | 1.1, 1.5; no FSM action on F = false | **PASS** |
| C20 | Front updates store at once; decisions on Tick after sensing | L2 ordering rule; 2.1.1 reads the store on Tick | **PASS** |
| C21 | All stored sensor values start false | DD Section 4.3; L2 Data Stores table; 1.5, 1.6 INIT | **PASS** |
| C22 | Forward and clean_on sent once at start-up, then C12 | 2.2 and 2.3: first command always sent | **PASS** |
| C23 | Left/right/dust start values overwritten on first tick | DD Section 4.3; L2 Data Stores table | **PASS** |
| C24 | Move Forward / Move Backward continuous (Enable / Disable), command once on Enable | 2.1.2, 2.1.5; L3 Signal Type Rules | **PASS** |
| C25 | Turn and cleaning actions one-shot (Trigger) | 2.1.3, 2.1.4, 2.1.6–2.1.8 | **PASS** |
| C26 | Signals only on a state transition | FSM stay rows have no actions; L2 2.1 row | **PASS** |
| C27 | Start-up: Enable Move Forward + Trigger Clean Normal; no separate start-up send | L3 S0; FSM T1; DD Sections 2.1, 4.4; PS 2.1, 2.1.1, 2.2, 2.3 | **PASS** |
| C28 | Power-up ends with clean_on on that tick; later dust starts a new power-up | FSM T11, T6; L3 S4, S5; DD and PS power-up text | **PASS** |
| C29 | Dust on the tick a turn or reverse ends is ignored; acted on from the next tick if still present | FSM T16, T21, T22, T23 (no D guard), Issue 6; L3 S6, S8, S9; PS Rev 8 (0, 2, 2.1, 2.1.1, 2.1.6, 2.1.8) | **PASS** |

## 7. Still open

**No open issues remain in any SA artifact.** These items were acknowledged at SA approval (2026-11-08). None of them blocked approval.

1. **SA approval. Done (2026-11-08).** The project owner approved the SA artifacts (see SA Approval Record). *(Was: "SA approval is pending".)*
2. **Start-up wording at Levels 0 and 1.** The Process 0 and Process 2 pseudocode still writes `send Direction(Forward); send Clean(clean_on)` at INIT, with the comment "at Level 3 via S0 (C27)". This is deliberate: at those levels the observable behaviour (each value sent once at start-up, C22) is the same. The mechanism (no separate send in 2.2 and 2.3) is stated at Levels 2 and 3. Please confirm this level of abstraction is acceptable.
3. **Dust on the tick a turn ends.** T16 and T21 return to FwdNormal without testing D, so dust on that tick is ignored and acted on from the next tick if still present. This follows C10 (the RVC is still turning when that tick's decision is made) and matches the C28 pattern. **Now stated by C29** (see Section 1a). *(Was: "No clarification states it explicitly".)*
4. **Signal order within one transition.** The FSM fixes an order: Disable first, then the cleaner Trigger, then the movement Trigger or Enable (FSM Issue 4). The requirements give no order, and no output depends on it. This order carries into SD.
5. **Unbounded repetition by design.** Reversing continues while both sides are blocked (T25, C9), and turns repeat while the front stays blocked (T13, T18, C15). The requirements give no limit. System test scenarios should cover these cases.
6. **Counter representation.** The width, saturation and reset points of the turn, reverse and power-up counters are left to SD (DD Section 4.6, FSM Issue 5).
