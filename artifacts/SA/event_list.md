# Event List

Sources: `preliminary_requirements.md` (lines cited as R1–R7) and `clarifications.md` (cited as C1–C28).

## Revision History

| Rev | Trigger | Changes |
|---|---|---|
| 1 | Initial generation | Event List from R1–R7 and C1–C8. |
| 2 | Consistency update after DFD Level 2 (approved) | Added citations to C9–C20. Refined event descriptions and output values with these clarifications. Marked Open Issues 1–7 as resolved. The set of events is unchanged: 5 inputs and 2 outputs. |
| 3 | SA consistency pass (C1–C28) | Added citations to C21–C28. Direction and Clean now state that they are sent once at start-up (C22, C27), then only on change. Clean (clean_up) and Dust Sensor Input now state the end of a power-up and a new power-up on later dust (C28). Added C21–C28 to the Traceability table. C21 and C23–C26 concern internal data and Level 3 control only; they add no event. The set of events is unchanged: 5 inputs and 2 outputs. |

| Input/Output Event | Description |
|---|---|
| Front Sensor Input | Input, control event (interrupt). Boolean signal raised immediately when an obstacle is detected in front of the RVC. It updates the obstacle situation straight away. On the next tick, the RVC stops cleaning and turns aside, or moves backward if left and right are also blocked (R3, R4, C1, C3, C20). A false value only updates the obstacle situation (C19). It is not acted on during a turn or reverse (C15). |
| Left Sensor Input | Input, flow-oriented, periodic. Boolean value reporting whether an obstacle is on the left side of the RVC. It is sampled once per tick. It is used to choose the turning direction and to detect the condition where front, left and right are all blocked (R3, R4, C2, C3, C4, C17, C18). A left obstacle with the front clear causes no reaction (C13). |
| Right Sensor Input | Input, flow-oriented, periodic. Boolean value reporting whether an obstacle is on the right side of the RVC. It is sampled once per tick. It is used to choose the turning direction and to detect the condition where front, left and right are all blocked (R3, R4, C2, C3, C4, C17, C18). A right obstacle with the front clear causes no reaction (C13). |
| Dust Sensor Input | Input, flow-oriented, periodic. Boolean value reporting whether dust is detected on the floor. It is sampled once per tick and triggers powered-up cleaning (R5, C2, C3, C17). It is ignored while turning or reversing (C10) and during an active power-up (C14). Dust detected on a later tick, after a power-up has ended, starts a new power-up (C28). |
| Tick | Input, temporal. Periodic time signal from the Digital Clock, and an abstract time unit (C17). It paces periodic sensor sampling and triggers the control decision after sensing (C20). It also measures the 3-tick rotation, the reverse of at least 3 ticks and the 3-tick cleaning power-up (R5, C2, C5, C6, C9). |
| Direction | Output. Movement command sent to the motor: Forward / Backward / Turn Left / Turn Right. Starts as Forward and is sent once at start-up; after that it is sent only when its value changes (R2, R3, R4, C4, C5, C9, C12, C16, C22, C27). |
| Clean | Output. Cleaning mode command sent to the cleaner (cleaning and mopping): clean_off / clean_on / clean_up. Starts as clean_on and is sent once at start-up; after that it is sent only when its value changes (R1, R3, R5, C6, C7, C8, C11, C12, C16, C22, C27, C28). |

## Event Classification

| Event | Direction | Category | Timing |
|---|---|---|---|
| Front Sensor Input | Input | Control event | Interrupt, asynchronous (C1); acted on at the next tick (C20) |
| Left Sensor Input | Input | Flow-oriented event | Periodic, once per tick (C2, C17) |
| Right Sensor Input | Input | Flow-oriented event | Periodic, once per tick (C2, C17) |
| Dust Sensor Input | Input | Flow-oriented event | Periodic, once per tick (C2, C17) |
| Tick | Input | Temporal event | Periodic (Digital Clock); abstract time unit (C17) |
| Direction | Output | Command | Once at start-up (C22, C27), then only when the movement changes (C12) |
| Clean | Output | Command | Once at start-up (C22, C27), then only when the cleaning mode changes (C12) |

## Input Data Values

| Event | Value | Meaning | Reference |
|---|---|---|---|
| Front / Left / Right Sensor Input | true | Obstacle detected on that side | R3, R4, C3 |
| Front / Left / Right Sensor Input | false | No obstacle on that side | R3, R4, C3 |
| Dust Sensor Input | true | Dust detected | R5, C3 |
| Dust Sensor Input | false | No dust detected | R5, C3 |

## Output Command Values

| Event | Value | Meaning | Reference |
|---|---|---|---|
| Direction | Forward | Move straight forward. Value at start-up, sent once. | R2, R3, R4, C16, C22, C27 |
| Direction | Backward | Move backward when front, left and right are all blocked. Lasts at least 3 ticks and continues while both sides are blocked. | R4, C9 |
| Direction | Turn Left | Turn aside left for 3 ticks; preferred when both sides are free | R3, R4, C4, C5 |
| Direction | Turn Right | Turn aside right for 3 ticks | R3, R4, C5 |
| Clean | clean_on | Normal cleaning and mopping while going forward. Value at start-up, sent once. | R1, R2, R3, C7, C16, C22, C27 |
| Clean | clean_off | Cleaning stopped; used only while reversing or rotating | R3, C7, C8 |
| Clean | clean_up | Powered-up cleaning for 3 ticks after dust detection. More dust does not restart it, and an obstacle cancels it. When it ends, the mode returns to clean_on on that tick; dust on a later tick starts a new power-up. | R5, C6, C7, C11, C14, C28 |

## Traceability

| Reference | Statement | Events |
|---|---|---|
| R1 | Automatically cleans and mops | Clean |
| R2 | Goes straight forward while cleaning | Direction, Clean |
| R3 | Obstacle → stop cleaning, turn left/right, resume forward cleaning | Front/Left/Right Sensor Input, Direction, Clean |
| R4 | Front, left and right blocked → backward, turn, forward | Front/Left/Right Sensor Input, Direction |
| R5 | Dust → power up cleaning for a while | Dust Sensor Input, Tick, Clean |
| R6 | HW control details out of scope | Sensor, clock and actuator signals are treated as abstract events |
| R7 | Automatic cleaning function only | No user, charging or battery events are included |
| C1 | Front sensor is interrupt-driven | Front Sensor Input |
| C2 | Left, right and dust sensors are periodic | Left/Right/Dust Sensor Input, Tick |
| C3 | Sensor values are Boolean | Front/Left/Right/Dust Sensor Input |
| C4 | Left turn has priority when both turns are allowed | Left/Right Sensor Input, Direction |
| C5 | Rotation lasts 3 ticks | Tick, Direction |
| C6 | Cleaning power-up lasts 3 ticks | Tick, Clean |
| C7 | Cleaner modes are clean_off, clean_on, clean_up | Clean |
| C8 | clean_off only while reversing or rotating | Clean |
| C9 | Reversing lasts at least 3 ticks, continues while both sides are blocked, ends with a turn (left first) | Left/Right Sensor Input, Tick, Direction |
| C10 | Dust while turning or reversing is ignored | Dust Sensor Input, Clean |
| C11 | An obstacle during power-up cancels the power-up | Front Sensor Input, Clean |
| C12 | Direction and Clean are sent only on change | Direction, Clean |
| C13 | Side-only obstacle with the front clear causes no reaction | Left/Right Sensor Input |
| C14 | Dust during an active power-up does not restart it | Dust Sensor Input, Clean |
| C15 | Front interrupt during a turn or reverse is not acted on; front still blocked after a turn → new turn decision | Front Sensor Input, Direction |
| C16 | Start-up: Direction = Forward, Clean = clean_on | Direction, Clean |
| C17 | Tick is an abstract time unit; left, right and dust are sampled once per tick | Tick, Left/Right/Dust Sensor Input |
| C18 | Turn decision uses the latest stored left/right values | Left/Right Sensor Input, Direction |
| C19 | Front = false only updates the obstacle situation | Front Sensor Input |
| C20 | Front interrupt updates the obstacle situation at once; decisions are made on Tick, after sensing | Front Sensor Input, Tick |
| C21 | All stored sensor values start as false | Front/Left/Right/Dust Sensor Input (stored values only; no event added) |
| C22 | Direction = Forward and Clean = clean_on are sent once at start-up, then C12 applies | Direction, Clean |
| C23 | Left, right and dust start-up values are overwritten on the first tick | Left/Right/Dust Sensor Input, Tick |
| C24 | Move Forward / Move Backward are continuous processes (Enable / Disable) | None directly (Level 3 internal); affects how Direction is produced |
| C25 | Turn and cleaning actions are one-shot processes (Trigger) | None directly (Level 3 internal); affects how Direction and Clean are produced |
| C26 | Control signals are issued only on a state transition | None directly (Level 3 internal) |
| C27 | Start-up: Controller enables Move Forward and triggers Clean Normal; no separate start-up send | Direction, Clean |
| C28 | A power-up ends with a return to normal cleaning on that tick; later dust starts a new power-up | Dust Sensor Input, Tick, Clean |

## Open Issues

Each issue was re-checked against `clarifications.md` (C1–C28). No issue is open.

1. **Backward duration (R4). Resolved (C9).** Reversing lasts at least 3 ticks and continues while both left and right are blocked.
2. **Turn after backward (R4). Resolved (C9, C18).** Reversing ends with a turn when a side opens, left first. The decision uses the latest stored left and right values.
3. **Side-only obstacles (R3). Resolved (C13).** A left or right obstacle with the front clear causes no reaction; the RVC keeps moving forward.
4. **Dust during clean_off (R5, C8). Resolved (C10).** Dust detected while turning or reversing is ignored.
5. **Repeated dust detection (R5, C6). Resolved (C14).** Dust detected during an active power-up does not restart the 3-tick period.
6. **Tick period. Resolved (C17).** A tick is an abstract time unit. Left, right and dust are sampled once per tick. The real period is HW detail and out of scope (R6).
7. **Source file name. Resolved.** The requirements file is `preliminary_requirements.md`. All SA artifacts refer to it by this name, and line references R1–R7 apply to it.
