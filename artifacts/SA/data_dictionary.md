# Data Dictionary

Sources: `preliminary_requirements.md` (lines cited as R1–R7), `clarifications.md` (C1–C28), `artifacts/SA/event_list.md`, `artifacts/SA/context_diagram.md`, `artifacts/SA/DFD/dfd_level_0.md`, `artifacts/SA/DFD/dfd_level_1.md`, `artifacts/SA/DFD/dfd_level_2.md`, `artifacts/SA/DFD/dfd_level_3.md`, `artifacts/SA/DFD/dfd_level_4_fsm.md` (read for consistency only), `artifacts/SA/process_specification.md` (Rev 7).

## Revision History

| Rev | Trigger | Changes |
|---|---|---|
| 1 | Initialization from Event List | Defined the 7 external input/output events with names, descriptions, types and values taken from the Event List. |
| 2 | DFD Level 0 generated | Added Source/Destination for Process 0 RVC Control and recorded DFD Level 0 for all 7 external flows. Added C9–C12 references cited by Level 0. Level 0 has no internal flows and no data stores. |
| 3 | DFD Level 1 generated | Added Level 1 endpoints for the 7 external flows. Inputs now go to 1 Obstacle & Dust Detection, outputs come from 2 Cleaner & Motor Control, and Tick goes to both. Added the internal flows Obstacle Location and Dust Existence, and the components of Obstacle Location (Front Obstacle, Left Obstacle, Right Obstacle). Level 1 has no data stores. Level 0 definitions are kept. |
| 4 | DFD Level 2 generated | Added Level 2 endpoints for the 7 external flows (sensor inputs → 1.1–1.4; Tick → 1.2, 1.3, 1.4, 2.1; Direction from 2.2; Clean from 2.3). Front, Left and Right Obstacle are now separate flows (1.1/1.2/1.3 → 1.5) as well as components of Obstacle Location. Added the new internal flows Dust, Motor Command and Cleaner Command. Added the data stores Obstacle Location and Dust Existence, which replace the Level 1 flows of the same name. Added C13–C20 references and refined timing, start-up values (C16) and sampling (C17). Re-checked the open issues against `clarifications.md` and marked the resolved ones. Level 0–1 definitions are kept. |
| 4a | Consistency update (approved) | Marked Open Issues 1, 2 and 6 as resolved after the Event List, Context Diagram and DFD Levels 0–2 were updated. No definitions changed. |
| 5 | Re-check against `clarifications.md` C21–C26 (no new DFD level) | Open Issue 7 is now resolved (C21, C22, C23). Both data stores start with all values false (C21). Direction = Forward and Clean = clean_on are sent once at start-up (C22). Open Issue 10 is mostly resolved (C24–C26); one detail is carried to Level 3. Added explicit value sets for Motor Command and Cleaner Command and their one-to-one mapping to Direction and Clean (Section 4.4). Added the provisional Level 3 control signals (Enable / Disable / Trigger) defined by C24–C26 (Section 2.1); their process numbers wait for DFD Level 3. Added Section 7, Stale artifacts. All Rev 4a definitions are kept. |
| 6 | DFD Level 3 generated (decomposition of 2.1 Main Control) | Section 2.1 is no longer provisional. The 9 control signals now have process numbers (2.1.1 Controller → 2.1.2–2.1.8) and DFD Level 3, and they match `dfd_level_3.md` one-to-one. Added Level 3 endpoints: Tick → 2.1.1 only; the two store reads → 2.1.1; Motor Command from 2.1.2–2.1.5; Cleaner Command from 2.1.6–2.1.8. Front, Left and Right Obstacle are recorded as the Controller conditions F, L and R. Section 4.4 now names the Level 3 producers. Added Section 4.5 (control signal details) and Section 4.6 (data kept inside 2.1.1, which is not a data store). Open Issue 10 is resolved: a continuous process produces its Motor Command once on Enable (DFD L3 Note 1, PS Issue 10). Open Issue 11 is resolved. Added Open Issue 12 for the start-up Enable of Move Forward, which needs a decision (DFD L3 Note 2). Level 3 adds no data stores. Section 7 is refreshed. All Rev 5 definitions are kept. |
| 7 | Re-check against `clarifications.md` C27 (no new DFD level) | Open Issue 12 is resolved (C27): at start-up 2.1.1 enables Move Forward **and** triggers Clean Normal. Together these produce the start-up Direction = Forward and Clean = clean_on of C22, and 2.2/2.3 have no separate start-up send. Trigger "Clean Normal" now lists S0 (start-up). Direction, Clean, Section 3, Section 4.4 and Section 4.6 now give the start-up path through Motor/Cleaner Command. 2.2 and 2.3 have sent nothing yet at start-up, so the first command passes the C12 compare. The remark on Open Issue 10 is removed, because C24 now states "produces its Motor Command once on Enable" itself. No data name, type or value set changed, and no flow or store was added or removed. Section 7 is refreshed against `dfd_level_4_fsm.md` and `process_specification.md` Rev 5. All Rev 6 definitions are kept. |
| 8 | SA consistency pass (C1–C28) | C27 (FSM Issue 1) was already applied in Rev 7; re-checked, no change. Added C28 to the power-up definitions: Clean (clean_up), Dust Existence, Trigger "Power-Up Clean", Trigger "Clean Normal", Cleaner Command and the power-up tick count (Section 4.6). When a power-up ends, the mode returns to clean_on on that tick; dust on a later tick starts a new power-up, which is not a restart under C14. Added Open Issue 13 (resolved by C28). Section 2.1 and Section 7 refreshed: all listed artifacts are now up to date. No data name, type, value set, flow or store changed. All Rev 7 definitions are kept. |

## 1. External Input/Output Events

Source and Destination give the endpoint at each DFD level. Each level's endpoint is a child of the endpoint at the level above.

| Input/Output Event | Description | Format / Type | Flow Category | Source | Destination | DFD Level | Requirement Reference |
|---|---|---|---|---|---|---|---|
| Front Sensor Input | Reports an obstacle in front of the RVC. Raised immediately (interrupt). | Boolean | Control (interrupt) | Front Sensor | L0: 0 RVC Control; L1: 1 Obstacle & Dust Detection; L2: 1.1 Front Sensor Interface | Context, 0, 1, 2 | R3, R4, C1, C3, C19, C20 |
| Left Sensor Input | Reports an obstacle on the left side of the RVC. Sampled once per tick. | Boolean | Data (periodic) | Left Sensor | L0: 0 RVC Control; L1: 1 Obstacle & Dust Detection; L2: 1.2 Left Sensor Interface | Context, 0, 1, 2 | R3, R4, C2, C3, C4, C17 |
| Right Sensor Input | Reports an obstacle on the right side of the RVC. Sampled once per tick. | Boolean | Data (periodic) | Right Sensor | L0: 0 RVC Control; L1: 1 Obstacle & Dust Detection; L2: 1.3 Right Sensor Interface | Context, 0, 1, 2 | R3, R4, C2, C3, C4, C17 |
| Dust Sensor Input | Reports dust on the floor. Sampled once per tick. | Boolean | Data (periodic) | Dust Sensor | L0: 0 RVC Control; L1: 1 Obstacle & Dust Detection; L2: 1.4 Dust Sensor Interface | Context, 0, 1, 2 | R5, C2, C3, C17 |
| Tick | Periodic time signal. Paces sampling and control decisions, and measures the 3-tick durations. | Event (no value) | Control (temporal) | Digital Clock | L0: 0 RVC Control; L1: 1 and 2; L2: 1.2, 1.3, 1.4 and 2.1 Main Control; L3: 2.1.1 Controller only | Context, 0, 1, 2, 3 | C2, C5, C6, C9, C17, C20 |
| Direction | Movement command sent to the Motor. | Enum | Data (output command) | L0: 0 RVC Control; L1: 2 Cleaner & Motor Control; L2: 2.2 Motor Interface | Motor | Context, 0, 1, 2 | R2, R3, R4, C4, C5, C9, C12, C16, C22, C27 |
| Clean | Cleaning mode command sent to the Cleaner (cleaning and mopping). | Enum | Data (output command) | L0: 0 RVC Control; L1: 2 Cleaner & Motor Control; L2: 2.3 Cleaner Interface | Cleaner | Context, 0, 1, 2 | R1, R3, R5, C6, C7, C8, C11, C12, C16, C22, C27, C28 |

## 2. Internal Data and Control Flows

| Data Name | Description | Format / Type | Flow Category | Source | Destination | DFD Level | Requirement Reference |
|---|---|---|---|---|---|---|---|
| Obstacle Location | Where obstacles are around the RVC: front, left and right. At Level 2 it is the name of a data store and of the flows that write and read it. | Composite = Front Obstacle + Left Obstacle + Right Obstacle | Data | L1: 1 Obstacle & Dust Detection; L2: 1.5 (write) / Obstacle Location store (read); L3: Obstacle Location store (read) | L1: 2 Cleaner & Motor Control; L2: Obstacle Location store (write) / 2.1 (read); L3: 2.1.1 Controller (read) | 1, 2, 3 | R3, R4, C1, C2, C3, C4, C9, C18, C20 |
| Front Obstacle | An obstacle is in front. Component of Obstacle Location, and at Level 2 also a separate flow. At Level 3 it is the Controller condition **F**. | Boolean | Data | L1: 1 (inside Obstacle Location); L2: 1.1 Front Sensor Interface; L3: Obstacle Location store | L1: 2 (inside Obstacle Location); L2: 1.5 Determine Obstacle Location; L3: 2.1.1 (condition F) | 1 (component), 2, 3 (component) | R3, R4, C1, C3, C15, C19, C20 |
| Left Obstacle | An obstacle is on the left. Component of Obstacle Location, and at Level 2 also a separate flow. At Level 3 it is the Controller condition **L**. | Boolean | Data | L1: 1 (inside Obstacle Location); L2: 1.2 Left Sensor Interface; L3: Obstacle Location store | L1: 2 (inside Obstacle Location); L2: 1.5 Determine Obstacle Location; L3: 2.1.1 (condition L) | 1 (component), 2, 3 (component) | R3, R4, C2, C3, C4, C9, C17, C18 |
| Right Obstacle | An obstacle is on the right. Component of Obstacle Location, and at Level 2 also a separate flow. At Level 3 it is the Controller condition **R**. | Boolean | Data | L1: 1 (inside Obstacle Location); L2: 1.3 Right Sensor Interface; L3: Obstacle Location store | L1: 2 (inside Obstacle Location); L2: 1.5 Determine Obstacle Location; L3: 2.1.1 (condition R) | 1 (component), 2, 3 (component) | R3, R4, C2, C3, C4, C9, C17, C18 |
| Dust Existence | Dust is present at the latest dust sensor sample. At Level 2 it is the name of a data store and of the flows that write and read it. At Level 3 it is the Controller condition **D**. | Boolean | Data | L1: 1 Obstacle & Dust Detection; L2: 1.6 (write) / Dust Existence store (read); L3: Dust Existence store (read) | L1: 2 Cleaner & Motor Control; L2: Dust Existence store (write) / 2.1 (read); L3: 2.1.1 Controller (read, condition D) | 1, 2, 3 | R5, C2, C3, C10, C14, C28 |
| Dust | Dust sensor value sampled on the current tick. | Boolean | Data | 1.4 Dust Sensor Interface | 1.6 Determine Dust Existence | 2 | R5, C2, C3, C17 |
| Motor Command | Movement that Main Control decides on. | Enum {Forward, Backward, Turn Left, Turn Right}; maps 1:1 to Direction (Section 4.4) | Data | L2: 2.1 Main Control; L3: 2.1.2 Move Forward (Forward), 2.1.3 Turn Left (Turn Left), 2.1.4 Turn Right (Turn Right), 2.1.5 Move Backward (Backward) | 2.2 Motor Interface | 2, 3 | R2, R3, R4, C4, C5, C9, C13, C15, C24, C25, C26 |
| Cleaner Command | Cleaning mode that Main Control decides on. | Enum {clean_on, clean_off, clean_up}; maps 1:1 to Clean (Section 4.4) | Data | L2: 2.1 Main Control; L3: 2.1.6 Clean Normal (clean_on), 2.1.7 Clean Off (clean_off), 2.1.8 Power-Up Clean (clean_up) | 2.3 Cleaner Interface | 2, 3 | R1, R3, R5, C6, C7, C8, C10, C11, C14, C25, C26, C27, C28 |

### 2.1 Level 3 Control Signals (from 2.1.1 Controller)

These are the 9 control signals drawn in `dfd_level_3.md`. They were recorded in advance in Rev 5 from C24 and C25. Rev 6 adds the process numbers and checks each one against the diagram: every entry below is one dashed arrow, and every dashed arrow from 2.1.1 is one entry below. No signal was added beyond C24 and C25. They stay inside 2.1, so they do not affect Level 2 balancing.

| Data Name | Description | Format / Type | Flow Category | Semantics | Source | Destination | DFD Level | Requirement Reference |
|---|---|---|---|---|---|---|---|---|
| Enable "Move Forward" | Starts forward movement: at start-up (S0, C27) and after a completed turn with the front clear (S6). | Event (no data value) | Control (Enable/Disable) | The process stays active until it is disabled. On Enable it produces Motor Command = Forward once. | 2.1.1 Controller | 2.1.2 Move Forward | 3 | R2, R3, C5, C16, C22, C24, C26, C27 |
| Disable "Move Forward" | Stops forward movement when a front obstacle is acted on (S1–S3). | Event (no data value) | Control (Enable/Disable) | Deactivates the process. Produces no command. | 2.1.1 Controller | 2.1.2 Move Forward | 3 | R3, R4, C24, C26 |
| Enable "Move Backward" | Starts reversing when front, left and right are all blocked (S3, or S7 at the end of a turn). | Event (no data value) | Control (Enable/Disable) | The process stays active until it is disabled (C9). On Enable it produces Motor Command = Backward once. | 2.1.1 Controller | 2.1.5 Move Backward | 3 | R4, C9, C15, C24, C26 |
| Disable "Move Backward" | Ends reversing after at least 3 ticks, once a side opens (S8, S9). | Event (no data value) | Control (Enable/Disable) | Deactivates the process. Produces no command. | 2.1.1 Controller | 2.1.5 Move Backward | 3 | R4, C9, C24, C26 |
| Trigger "Turn Left" | Requests one left turn: front blocked with left free (S1), at the end of a turn (S7) or at the end of reversing (S8). | Event (no data value) | Control (Trigger) | The process runs once and produces Motor Command = Turn Left once. The 3-tick turn is timed by 2.1.1. | 2.1.1 Controller | 2.1.3 Turn Left | 3 | R3, R4, C4, C5, C9, C15, C18, C25, C26 |
| Trigger "Turn Right" | Requests one right turn: front and left blocked with right free (S2), at the end of a turn (S7) or at the end of reversing (S9). | Event (no data value) | Control (Trigger) | The process runs once and produces Motor Command = Turn Right once. The 3-tick turn is timed by 2.1.1. | 2.1.1 Controller | 2.1.4 Turn Right | 3 | R3, R4, C4, C5, C9, C15, C18, C25, C26 |
| Trigger "Clean Normal" | Requests normal cleaning: at start-up (S0, C27), after a completed turn with the front clear (S6), or when the 3-tick power-up ends (S5), on that same tick (C28). | Event (no data value) | Control (Trigger) | The process runs once and produces Cleaner Command = clean_on once. | 2.1.1 Controller | 2.1.6 Clean Normal | 3 | R1, R2, R3, C6, C7, C16, C22, C25, C26, C27, C28 |
| Trigger "Clean Off" | Stops cleaning when a front obstacle is acted on (S1–S3). Cancels a power-up in progress (C11). | Event (no data value) | Control (Trigger) | The process runs once and produces Cleaner Command = clean_off once. | 2.1.1 Controller | 2.1.7 Clean Off | 3 | R3, C7, C8, C11, C25, C26 |
| Trigger "Power-Up Clean" | Starts powered-up cleaning when dust is found while moving forward with clean_on (S4). This includes dust on a tick after a power-up has ended, which starts a new power-up (C28). | Event (no data value) | Control (Trigger) | The process runs once and produces Cleaner Command = clean_up once. The 3-tick power-up is timed by 2.1.1. | 2.1.1 Controller | 2.1.8 Power-Up Clean | 3 | R5, C6, C7, C10, C14, C25, C26, C28 |

Issuing rule (C26): the Controller sends Enable, Disable or Trigger only on a state transition, never on every tick. S0–S9 are the situations listed in `dfd_level_3.md`, "Signals per situation". S0 (start-up) is the initial transition (`dfd_level_4_fsm.md` T1). It issues Enable "Move Forward" and Trigger "Clean Normal" (C27). `dfd_level_3.md` S0 lists the same two signals (updated in the SA consistency pass).

## 3. Data Store Definitions

| Data Store | Description | Stored Data | Format / Type | Written by | Read by | DFD Level | Requirement Reference |
|---|---|---|---|---|---|---|---|
| Obstacle Location | Latest known obstacle situation around the RVC. Replaces the Level 1 flow of the same name. | Front Obstacle, Left Obstacle, Right Obstacle | Composite of 3 Booleans; all false at start-up (C21) | 1.5 Determine Obstacle Location | L2: 2.1 Main Control; L3: 2.1.1 Controller | 2, 3 | R3, R4, C1–C4, C17, C18, C20, C21, C23 |
| Dust Existence | Latest dust status. Replaces the Level 1 flow of the same name. | Dust Existence | Boolean; false at start-up (C21) | 1.6 Determine Dust Existence | L2: 2.1 Main Control; L3: 2.1.1 Controller | 2, 3 | R5, C2, C3, C17, C21, C23 |

Levels 0 and 1 have no data stores. Level 3 adds none; it reads the two Level 2 stores. Values used by only one process are kept inside that process and are not data stores (see `dfd_level_2.md`, Note 4, and `dfd_level_3.md`, Data Stores):
- the last Direction and Clean values sent, kept in 2.2 and 2.3. At start-up nothing has been sent yet, so the first Motor Command and Cleaner Command (Forward and clean_on, from S0) are always sent (C12, C22, C27);
- the current movement, the cleaning mode and the turn, reverse and power-up tick counters, kept in 2.1 at Level 2 and in 2.1.1 Controller at Level 3 (Section 4.6).

## 4. Detailed Data Definitions

### 4.1 External Flows

#### Front Sensor Input
- Description: Obstacle detection signal from the front sensor. It is interrupt-driven, so it reaches the system as soon as an obstacle is detected. It updates Obstacle Location straight away. The control decision to stop cleaning and turn aside, or to move backward if left and right are also blocked, is made on the next Tick (C20).
- Format / Type: Boolean
- Possible Values: `true` = obstacle detected in front; `false` = no obstacle in front. A `false` value only updates Obstacle Location and causes no action (C19).
- Flow Category: Control (interrupt, asynchronous)
- Source: Front Sensor (terminator)
- Destination: L0: 0 RVC Control; L1: 1 Obstacle & Dust Detection; L2: 1.1 Front Sensor Interface
- DFD Level: Context, 0, 1, 2
- Requirement Reference: R3, R4, C1, C3, C19, C20
- Notes: At Level 1 its value is passed on as Front Obstacle inside Obstacle Location. At Level 2, 1.1 passes it to 1.5 as the flow Front Obstacle. A front interrupt during a turn or reverse is not acted on (C15).

#### Left Sensor Input
- Description: Obstacle detection value from the left sensor. Used to choose the turn direction and to detect the condition where front, left and right are all blocked.
- Format / Type: Boolean
- Possible Values: `true` = obstacle on the left; `false` = no obstacle on the left
- Flow Category: Data (periodic)
- Source: Left Sensor (terminator)
- Destination: L0: 0 RVC Control; L1: 1 Obstacle & Dust Detection; L2: 1.2 Left Sensor Interface
- DFD Level: Context, 0, 1, 2
- Requirement Reference: R3, R4, C2, C3, C4, C13, C17
- Notes: Sampled once per tick. The real period is HW detail and out of scope (C17, R6). A left obstacle with the front clear causes no reaction (C13). At Level 2, 1.2 passes it to 1.5 as the flow Left Obstacle.

#### Right Sensor Input
- Description: Obstacle detection value from the right sensor. Used to choose the turn direction and to detect the condition where front, left and right are all blocked.
- Format / Type: Boolean
- Possible Values: `true` = obstacle on the right; `false` = no obstacle on the right
- Flow Category: Data (periodic)
- Source: Right Sensor (terminator)
- Destination: L0: 0 RVC Control; L1: 1 Obstacle & Dust Detection; L2: 1.3 Right Sensor Interface
- DFD Level: Context, 0, 1, 2
- Requirement Reference: R3, R4, C2, C3, C4, C13, C17
- Notes: Sampled once per tick (C17). A right obstacle with the front clear causes no reaction (C13). At Level 2, 1.3 passes it to 1.5 as the flow Right Obstacle.

#### Dust Sensor Input
- Description: Dust detection value from the dust sensor. Triggers powered-up cleaning. Dust detected while turning or reversing is ignored (C10).
- Format / Type: Boolean
- Possible Values: `true` = dust detected; `false` = no dust detected
- Flow Category: Data (periodic)
- Source: Dust Sensor (terminator)
- Destination: L0: 0 RVC Control; L1: 1 Obstacle & Dust Detection; L2: 1.4 Dust Sensor Interface
- DFD Level: Context, 0, 1, 2
- Requirement Reference: R5, C2, C3, C10, C17
- Notes: Sampled once per tick (C17). At Level 1 its value is passed on as Dust Existence. At Level 2, 1.4 passes it to 1.6 as the flow Dust.

#### Tick
- Description: Periodic time signal from the Digital Clock. It paces sampling of the periodic sensors. It also triggers the control decision and measures the 3-tick rotation (C5), the 3-tick cleaning power-up (C6) and the minimum 3-tick reversing period (C9).
- Format / Type: Event (signal with no data value)
- Possible Values: N/A (occurrence only)
- Flow Category: Control (temporal)
- Source: Digital Clock (terminator)
- Destination: L0: 0 RVC Control; L1: 1 Obstacle & Dust Detection (sampling, C2) and 2 Cleaner & Motor Control (durations, C5, C6, C9); L2: 1.2, 1.3, 1.4 (sampling) and 2.1 Main Control (decisions and durations); L3: 2.1.1 Controller only
- DFD Level: Context, 0, 1, 2, 3
- Requirement Reference: C2, C5, C6, C9, C17, C20
- Notes: A tick is an abstract time unit. The real period is HW detail and out of scope (C17, R6). Within a tick, sensing (1.2–1.6) comes before the decision (2.1, and 2.1.1 at Level 3) (C20). At Levels 1 and 2 the same flow is drawn to every process that uses it. This is a split of one flow, not a new external flow. At Level 3 Tick is the only event of 2.1.1. The action processes 2.1.2–2.1.8 get no Tick, so 2.1.1 times every duration.

#### Direction
- Description: Movement command sent to the Motor. Sent only when its value changes (C12).
- Format / Type: Enum
- Possible Values:

| Value | Meaning | Reference |
|---|---|---|
| Forward | Move straight forward. Value at start-up (C16). | R2, R3, R4, C16 |
| Backward | Move backward while front, left and right are all blocked; lasts at least 3 ticks | R4, C9 |
| Turn Left | Turn aside left for 3 ticks; preferred when both sides are free | R3, R4, C4, C5 |
| Turn Right | Turn aside right for 3 ticks | R3, R4, C5 |

- Flow Category: Data (output command)
- Source: L0: 0 RVC Control; L1: 2 Cleaner & Motor Control; L2: 2.2 Motor Interface
- Destination: Motor (terminator)
- DFD Level: Context, 0, 1, 2
- Requirement Reference: R2, R3, R4, C4, C5, C9, C12, C16, C22, C27
- Notes: At start-up, Direction = Forward is sent once (C22). It comes from Enable "Move Forward" at start-up, which makes 2.1.2 produce Motor Command = Forward. 2.2 has sent nothing yet, so it passes this command on as Direction. 2.2 has no separate start-up send (C27). After that, Direction is sent only on change (C12). At Level 2, 2.2 sends Direction when Motor Command differs from the last value sent.

#### Clean
- Description: Cleaning mode command sent to the Cleaner, which performs cleaning and mopping. Sent only when its value changes (C12).
- Format / Type: Enum
- Possible Values:

| Value | Meaning | Reference |
|---|---|---|
| clean_on | Normal cleaning and mopping while going forward. Value at start-up (C16). | R1, R2, R3, C7, C16 |
| clean_off | Cleaning stopped; used only while reversing or rotating | R3, C7, C8 |
| clean_up | Powered-up cleaning for 3 ticks after dust detection; not restarted by more dust (C14); cancelled by an obstacle (C11). When it ends, the mode returns to clean_on on that tick; dust on a later tick starts a new power-up (C28) | R5, C6, C7, C11, C14, C28 |

- Flow Category: Data (output command)
- Source: L0: 0 RVC Control; L1: 2 Cleaner & Motor Control; L2: 2.3 Cleaner Interface
- Destination: Cleaner (terminator)
- DFD Level: Context, 0, 1, 2
- Requirement Reference: R1, R3, R5, C6, C7, C8, C11, C12, C14, C16, C22, C27, C28
- Notes: At start-up, Clean = clean_on is sent once (C22). It comes from Trigger "Clean Normal" at start-up, which makes 2.1.6 produce Cleaner Command = clean_on. 2.3 has sent nothing yet, so it passes this command on as Clean. 2.3 has no separate start-up send (C27). After that, Clean is sent only on change (C12). At Level 2, 2.3 sends Clean when Cleaner Command differs from the last value sent.

### 4.2 Internal Flows

#### Obstacle Location
- Description: The obstacle situation around the RVC. Main Control uses it to decide whether to turn left, turn right or reverse, and when reversing may end.
- Format / Type: Composite. `Obstacle Location = Front Obstacle + Left Obstacle + Right Obstacle`
- Possible Values: Each component is `true` (obstacle) or `false` (no obstacle). There are 8 combinations.
- Flow Category: Data
- Source: L1: 1 Obstacle & Dust Detection; L2: 1.5 Determine Obstacle Location (writes the store); the Obstacle Location store (read by 2.1)
- Destination: L1: 2 Cleaner & Motor Control; L2: the Obstacle Location store (write); 2.1 Main Control (read); L3: 2.1.1 Controller (read)
- DFD Level: 1 (flow), 2 (data store and its write/read flows), 3 (store read by 2.1.1)
- Requirement Reference: R3, R4, C1, C2, C3, C4, C9, C18, C20
- Update timing:
  - Front Obstacle is updated as soon as Front Sensor Input arrives (interrupt, C1, C20).
  - Left Obstacle and Right Obstacle are updated on each Tick (C2, C17).
- Read timing: 2.1 Main Control (2.1.1 Controller at Level 3) reads it on each Tick, after that tick's sensing (C20). The turn decision uses the latest stored left and right values (C18). At Level 3 its components are the Controller conditions F, L and R.
- Notes: At Level 2 the Level 1 flow becomes a data store with the same name and contents (see Section 3). This is a refinement, not a new interface.

#### Front Obstacle
- Description: An obstacle is in front of the RVC. At Level 1 it is a component of Obstacle Location. At Level 2 it is also the flow from 1.1 to 1.5.
- Format / Type: Boolean
- Possible Values: `true` = obstacle in front; `false` = front clear
- Flow Category: Data
- Source: L1: 1 Obstacle & Dust Detection (inside Obstacle Location); L2: 1.1 Front Sensor Interface
- Destination: L1: 2 Cleaner & Motor Control (inside Obstacle Location); L2: 1.5 Determine Obstacle Location
- DFD Level: 1 (component), 2 (flow and store component)
- Requirement Reference: R3, R4, C1, C3, C19, C20
- Update timing: As soon as Front Sensor Input arrives (C1, C20). A `false` value only updates the store (C19).
- Level 3: read from the Obstacle Location store by 2.1.1 as condition F, on Tick (C20). F = true is not acted on during a turn or reverse until the period ends (C15).

#### Left Obstacle
- Description: An obstacle is on the left of the RVC. At Level 1 it is a component of Obstacle Location. At Level 2 it is also the flow from 1.2 to 1.5.
- Format / Type: Boolean
- Possible Values: `true` = left blocked; `false` = left free
- Flow Category: Data
- Source: L1: 1 Obstacle & Dust Detection (inside Obstacle Location); L2: 1.2 Left Sensor Interface
- Destination: L1: 2 Cleaner & Motor Control (inside Obstacle Location); L2: 1.5 Determine Obstacle Location; L3: 2.1.1 (condition L)
- DFD Level: 1 (component), 2 (flow and store component), 3 (store component)
- Requirement Reference: R3, R4, C2, C3, C4, C9, C17, C18
- Update timing: Once per Tick (C2, C17).
- Level 3: condition L of 2.1.1. It selects Turn Left (L = false) before Turn Right (C4), and ends reversing when false (C9).

#### Right Obstacle
- Description: An obstacle is on the right of the RVC. At Level 1 it is a component of Obstacle Location. At Level 2 it is also the flow from 1.3 to 1.5.
- Format / Type: Boolean
- Possible Values: `true` = right blocked; `false` = right free
- Flow Category: Data
- Source: L1: 1 Obstacle & Dust Detection (inside Obstacle Location); L2: 1.3 Right Sensor Interface
- Destination: L1: 2 Cleaner & Motor Control (inside Obstacle Location); L2: 1.5 Determine Obstacle Location; L3: 2.1.1 (condition R)
- DFD Level: 1 (component), 2 (flow and store component), 3 (store component)
- Requirement Reference: R3, R4, C2, C3, C4, C9, C17, C18
- Update timing: Once per Tick (C2, C17).
- Level 3: condition R of 2.1.1. With L = true, R = false selects Turn Right; L = R = true selects Move Backward (R4, C9).

#### Dust Existence
- Description: Whether dust is present at the latest dust sensor sample. Main Control uses it to start powered-up cleaning while moving forward with clean_on. It is ignored while turning or reversing (C10) and during an active power-up (C14).
- Format / Type: Boolean
- Possible Values: `true` = dust present; `false` = no dust
- Flow Category: Data
- Source: L1: 1 Obstacle & Dust Detection; L2: 1.6 Determine Dust Existence (writes the store); the Dust Existence store (read by 2.1)
- Destination: L1: 2 Cleaner & Motor Control; L2: the Dust Existence store (write); 2.1 Main Control (read); L3: 2.1.1 Controller (read, condition D)
- DFD Level: 1 (flow), 2 (data store and its write/read flows), 3 (store read by 2.1.1)
- Requirement Reference: R5, C2, C3, C6, C10, C14, C17, C28
- Update timing: Written once per Tick (C2, C17). Read by 2.1 (2.1.1 at Level 3) on each Tick, after that tick's sensing (C20).
- Use at the end of a power-up: it is not tested on the tick the power-up ends, when the mode returns to clean_on. If it is still true on a later tick, a new power-up starts (C28).
- Notes: At Level 2 the Level 1 flow becomes a data store with the same name and contents (see Section 3).

#### Dust
- Description: The dust sensor value sampled on the current tick, passed from the sensor interface to the process that updates Dust Existence.
- Format / Type: Boolean
- Possible Values: `true` = dust detected; `false` = no dust detected
- Flow Category: Data
- Source: 1.4 Dust Sensor Interface
- Destination: 1.6 Determine Dust Existence
- DFD Level: 2
- Requirement Reference: R5, C2, C3, C17
- Update timing: Once per Tick.

#### Motor Command
- Description: The movement that Main Control decides on. 2.2 Motor Interface turns it into Direction, and sends it only when the value changes (C12).
- Format / Type: Enum. `Motor Command = {Forward, Backward, Turn Left, Turn Right}`
- Possible Values: Forward / Backward / Turn Left / Turn Right. Each value maps one-to-one to the Direction value of the same name (Section 4.4).
- Flow Category: Data
- Source: L2: 2.1 Main Control; L3: 2.1.2 Move Forward, 2.1.3 Turn Left, 2.1.4 Turn Right, 2.1.5 Move Backward (one flow with four producers; each producer makes one value, Section 4.4)
- Destination: 2.2 Motor Interface
- DFD Level: 2, 3
- Requirement Reference: R2, R3, R4, C4, C5, C9, C13, C15, C18, C24, C25, C26, C27
- Notes: Issued only when a new movement is decided, that is, on a state transition (C26). The first one, Forward, comes from the start-up Enable "Move Forward" (C27). Turn Left and Turn Right each produce one command per Trigger (C25). Move Forward and Move Backward are continuous processes that stay active from Enable to Disable (C24). They produce their command once, on Enable, and nothing on Disable. C24 states this directly, and it fits Level 3, where they have no Tick input (Open Issue 10, resolved). A repeated turn at the end of a turn (C15) is a new Trigger, so it produces a command with the same value, which 2.2 filters (C12).

#### Cleaner Command
- Description: The cleaning mode that Main Control decides on. 2.3 Cleaner Interface turns it into Clean, and sends it only when the value changes (C12).
- Format / Type: Enum. `Cleaner Command = {clean_on, clean_off, clean_up}`
- Possible Values: clean_on / clean_off / clean_up. Each value maps one-to-one to the Clean value of the same name (Section 4.4).
- Flow Category: Data
- Source: L2: 2.1 Main Control; L3: 2.1.6 Clean Normal, 2.1.7 Clean Off, 2.1.8 Power-Up Clean (one flow with three producers; each producer makes one value, Section 4.4)
- Destination: 2.3 Cleaner Interface
- DFD Level: 2, 3
- Requirement Reference: R1, R3, R5, C6, C7, C8, C10, C11, C14, C25, C26, C27
- Notes: At Level 3 it comes from the one-shot processes 2.1.6 Clean Normal (clean_on), 2.1.7 Clean Off (clean_off) and 2.1.8 Power-Up Clean (clean_up). Each one produces one command per Trigger (C25), and Triggers are sent only on a state transition (C26). The first one, clean_on, comes from the start-up Trigger "Clean Normal" (C27).

### 4.3 Data Stores

#### Obstacle Location (data store)
- Description: Holds the latest known obstacle situation. Its writer (1.5) is driven partly by an interrupt and partly by the tick, and its reader (2.1) is driven by the tick. The store separates the two.
- Format / Type: Composite = Front Obstacle + Left Obstacle + Right Obstacle (each Boolean)
- Possible Values: 8 combinations of the three Booleans
- Written by: 1.5 Determine Obstacle Location (front part on the interrupt; left and right parts on each tick)
- Read by: 2.1 Main Control at Level 2; 2.1.1 Controller at Level 3 (on each Tick, after sensing; C20)
- DFD Level: 2, 3
- Requirement Reference: R3, R4, C1–C4, C17, C18, C20, C21, C23
- Initial value: Front Obstacle = false, Left Obstacle = false and Right Obstacle = false (C21). Left and Right are overwritten on the first tick before 2.1 reads them (C20, C23). Front stays false until the first front interrupt. *(Was TBD before Rev 5.)*

#### Dust Existence (data store)
- Description: Holds the latest dust status.
- Format / Type: Boolean
- Possible Values: `true` = dust present; `false` = no dust
- Written by: 1.6 Determine Dust Existence (once per tick)
- Read by: 2.1 Main Control at Level 2; 2.1.1 Controller at Level 3 (on each Tick, after sensing; C20)
- DFD Level: 2, 3
- Requirement Reference: R5, C2, C3, C17, C21, C23
- Initial value: false (C21). It is overwritten on the first tick before 2.1 reads it (C20, C23). C21 keeps the value fixed so that behaviour is deterministic. *(Was TBD before Rev 5.)*

### 4.4 Command-to-Output Mapping

2.2 and 2.3 do not change the values: each command value maps to the output value of the same name. The only filtering they do is the send-on-change rule (C12). They have no separate start-up send. The start-up Forward and clean_on of C22 are the first Motor Command and Cleaner Command, produced by the S0 Enable "Move Forward" and Trigger "Clean Normal" (C27). Because nothing has been sent before them, both pass the C12 compare.

| Motor Command | Direction (to Motor) | Level 3 producer | Activated by | Reference |
|---|---|---|---|---|
| Forward | Forward | 2.1.2 Move Forward (continuous, C24) | Enable "Move Forward" (once on Enable; also at start-up, C27) | R2, C16, C24, C27 |
| Backward | Backward | 2.1.5 Move Backward (continuous, C24) | Enable "Move Backward" (once on Enable) | R4, C9, C24 |
| Turn Left | Turn Left | 2.1.3 Turn Left (one-shot, C25) | Trigger "Turn Left" | R3, R4, C4, C5, C25 |
| Turn Right | Turn Right | 2.1.4 Turn Right (one-shot, C25) | Trigger "Turn Right" | R3, R4, C5, C25 |

| Cleaner Command | Clean (to Cleaner) | Level 3 producer | Activated by | Reference |
|---|---|---|---|---|
| clean_on | clean_on | 2.1.6 Clean Normal (one-shot, C25) | Trigger "Clean Normal" (also at start-up, C27) | R1, R2, C7, C16, C25, C27 |
| clean_off | clean_off | 2.1.7 Clean Off (one-shot, C25) | Trigger "Clean Off" | R3, C7, C8, C25 |
| clean_up | clean_up | 2.1.8 Power-Up Clean (one-shot, C25) | Trigger "Power-Up Clean" | R5, C6, C7, C25 |

No command value lacks an output value, and no output value lacks a command value. Each value has exactly one Level 3 producer, and each producer makes exactly one value. Disable "Move Forward" and Disable "Move Backward" produce no command.

### 4.5 Level 3 Control Signals

The 9 signals in Section 2.1 share one definition pattern:

- Format / Type: Event (no data value). Occurrence only.
- Possible Values: N/A.
- Flow Category: Control (Enable/Disable) for 2.1.2 and 2.1.5; Control (Trigger) for 2.1.3, 2.1.4 and 2.1.6–2.1.8 (C24, C25).
- Source: 2.1.1 Controller (control process).
- DFD Level: 3. The signals stay inside 2.1 and do not appear at Levels 0–2.
- Issuing: only on a state transition, on Tick (C20, C26).
- Semantics:
  - **Enable**: the destination becomes active and stays active until the matching Disable. On Enable it produces its Motor Command once.
  - **Disable**: the destination becomes inactive. No command is produced.
  - **Trigger**: the destination runs once and produces one command (C25). Its duration, if any (turn 3 ticks, power-up 3 ticks), is timed by 2.1.1, not by the destination.

| Signal | Destination | Signal pair / type | Command produced | Situations (`dfd_level_3.md`) |
|---|---|---|---|---|
| Enable "Move Forward" | 2.1.2 | Enable/Disable pair | Motor Command = Forward | S0 (C27), S6 |
| Disable "Move Forward" | 2.1.2 | Enable/Disable pair | none | S1, S2, S3 |
| Trigger "Turn Left" | 2.1.3 | Trigger | Motor Command = Turn Left | S1, S7, S8 |
| Trigger "Turn Right" | 2.1.4 | Trigger | Motor Command = Turn Right | S2, S7, S9 |
| Enable "Move Backward" | 2.1.5 | Enable/Disable pair | Motor Command = Backward | S3, S7 |
| Disable "Move Backward" | 2.1.5 | Enable/Disable pair | none | S8, S9 |
| Trigger "Clean Normal" | 2.1.6 | Trigger | Cleaner Command = clean_on | S0 (C27), S5, S6 |
| Trigger "Clean Off" | 2.1.7 | Trigger | Cleaner Command = clean_off | S1, S2, S3 |
| Trigger "Power-Up Clean" | 2.1.8 | Trigger | Cleaner Command = clean_up | S4 |

The order of several signals issued on the same tick (for example S1: Disable Move Forward, Trigger Clean Off, Trigger Turn Left) is not stated in the requirements and does not change any output (`dfd_level_3.md` Note 4). It is not a data definition; it is left to the Level 4 FSM and SD.

### 4.6 Data Kept Inside 2.1.1 Controller (not data stores)

These items are used only by 2.1.1. They are hidden inside it (information hiding) and are not flows or stores. They are listed so that the Level 4 FSM and SD use the same names. Their values come only from the stated clarifications.

| Item | Description | Format / Type | Values | Reference |
|---|---|---|---|---|
| Current movement | The movement 2.1.1 has activated. | Enum, same values as Motor Command | Forward / Backward / Turn Left / Turn Right; Forward at start-up, set by the S0 Enable "Move Forward" | R2–R4, C16, C27 |
| Cleaning mode | The cleaning mode 2.1.1 has requested. | Enum, same values as Cleaner Command | clean_on / clean_off / clean_up; clean_on at start-up, set by the S0 Trigger "Clean Normal" | R1, R3, R5, C7, C16, C27 |
| Turn tick count | Ticks elapsed in the current turn. | Non-negative integer count of Ticks | The turn ends after 3 ticks | C5 |
| Reverse tick count | Ticks elapsed in the current reverse. | Non-negative integer count of Ticks | Reversing may end once 3 ticks have elapsed and a side is free | C9 |
| Power-up tick count | Ticks elapsed in the current power-up. | Non-negative integer count of Ticks | The power-up ends after 3 ticks, and the mode returns to clean_on on that tick (C28); not restarted by dust (C14); cancelled by an obstacle (C11). A new power-up after one has ended starts a new count (C28) | C6, C11, C14, C28 |

Representation, counter width and reset points are SD/implementation detail and are TBD here. The Level 4 FSM shows the movement and mode as states and the counts as timing conditions.

## 5. Validation

### After DFD Level 0

| Check | Result |
|---|---|
| Every DFD Level 0 flow is defined | Yes. All 7 flows (5 in, 2 out) are defined. |
| Every DFD Level 0 data store is defined | N/A. Level 0 has no data stores. |
| Names match the Event List | Yes. All 7 Event List names are kept unchanged. |
| Names, sources and destinations match the Context Diagram and Level 0 | Yes. |
| Flow categories match the Context Diagram / Level 0 styles | Yes. Front Sensor Input and Tick are control flows (dashed). The others are data flows (solid). |
| Duplicate or conflicting definitions | None. |

### After DFD Level 1

| Check | Result |
|---|---|
| Every DFD Level 1 flow is defined | Yes. 7 external flows and 2 internal flows (Obstacle Location, Dust Existence). |
| Composite flows decomposed | Yes. Obstacle Location = Front Obstacle + Left Obstacle + Right Obstacle. Each component is defined. |
| Every DFD Level 1 data store is defined | N/A. Level 1 has no data stores. |
| Level 1 endpoints refine Level 0 endpoints | Yes. 1 and 2 are children of 0, and the Tick split is documented. |
| Flow categories match the Level 1 styles | Yes. Front Sensor Input and Tick are dashed (control). All other flows, including the two internal ones, are solid (data). |
| Names match the Event List and earlier levels | Yes. No external flow was renamed. |
| Level 0 definitions preserved | Yes. They are kept as the "L0" endpoints. |
| Duplicate or conflicting definitions | None. |

### After DFD Level 2

| Check | Result |
|---|---|
| Every DFD Level 2 flow is defined | Yes. 7 external flows; internal flows Front/Left/Right Obstacle, Dust, Motor Command and Cleaner Command; and the store write and read flows Obstacle Location and Dust Existence. |
| Every DFD Level 2 data store is defined | Yes. Obstacle Location and Dust Existence (Sections 3 and 4.3). |
| Level 2 endpoints refine Level 1 endpoints | Yes. 1.1–1.6 are children of 1, and 2.1–2.3 are children of 2. The Tick split to 1.2, 1.3, 1.4 and 2.1 is documented. |
| Flow-to-store change documented | Yes. The Level 1 flows Obstacle Location and Dust Existence become data stores with the same name and contents, as recorded in `dfd_level_2.md` balancing. |
| Flow categories match the Level 2 styles | Yes. Front Sensor Input and Tick are dashed (control). All other flows are solid (data). |
| Names match the Event List and earlier levels | Yes. No existing name was changed. The new names (Dust, Motor Command, Cleaner Command) do not clash with existing names. |
| Level 0–1 definitions preserved | Yes. They are kept as the "L0" and "L1" endpoints. |
| Value sets consistent | Yes. Motor Command uses the Direction values and Cleaner Command uses the Clean values. No new values were added. |
| Duplicate or conflicting definitions | None in this dictionary. There is a timing conflict between `dfd_level_1.md` and C20; see Section 6, item 6. |

### Rev 5 Re-check (C21–C26)

| Check | Result |
|---|---|
| All clarifications cited | Yes. C1–C26 are cited. C21–C23 appear on Direction, Clean and both data stores. C24–C26 appear on Motor Command, Cleaner Command and the Section 2.1 signals. |
| Every DFD Level 0–2 flow and store still defined | Yes. No flow or store was added or removed. |
| Motor/Cleaner Command value sets explicit and mapped | Yes (Section 4.4). Each mapping is one-to-one, and no new values were added. |
| Level 3 signals consistent with C24–C26 | Yes. Move Forward and Move Backward use Enable/Disable. The 5 one-shot processes use Trigger. No other signals are defined. |
| Conflicting definitions | None. The TBD initial store values were replaced by the C21 values, not overwritten with a conflicting value. |

### After DFD Level 3

| Check | Result |
|---|---|
| Every DFD Level 3 flow is defined | Yes. Store reads Obstacle Location and Dust Existence, Tick, the 9 control signals (Section 2.1, 4.5), Motor Command and Cleaner Command. 14 flow labels in `dfd_level_3.md`, 14 entries here. |
| Every control flow from 2.1.1 is defined | Yes. 9 dashed arrows in the diagram: Enable and Disable to 2.1.2 and 2.1.5, Trigger to 2.1.3, 2.1.4, 2.1.6, 2.1.7 and 2.1.8. Each has its own entry with Data Name, Event type, Flow Category, Semantics, Source 2.1.1 and a numbered Destination. |
| Signal types match C24 and C25 | Yes. Enable/Disable only to the continuous processes 2.1.2 and 2.1.5. Trigger only to the one-shot processes 2.1.3, 2.1.4 and 2.1.6–2.1.8. |
| Every DFD Level 3 data store is defined | Yes. Level 3 adds no stores. The two Level 2 stores now list 2.1.1 as their Level 3 reader. |
| Level 3 endpoints refine Level 2 endpoints | Yes. 2.1.1–2.1.8 are children of 2.1. Tick, the store reads and both commands keep their names, styles and value sets. Motor Command now has 4 producers and Cleaner Command has 3, each making one value (Section 4.4). |
| Motor/Cleaner Command value sets and mapping | Yes. Motor Command = {Forward, Backward, Turn Left, Turn Right} and Cleaner Command = {clean_on, clean_off, clean_up}, mapped 1:1 to Direction and Clean. No new values. |
| Names match the Event List and earlier levels | Yes. No name was changed. The new signal names match the `dfd_level_3.md` Flows table (`Enable (Move Forward)` there is `Enable "Move Forward"` here: same signal, dictionary naming style). |
| Level 0–2 definitions preserved | Yes. Level 3 endpoints were appended; no earlier endpoint was removed. |
| Hidden Controller data not mistaken for stores | Yes. Movement, mode and the three counters are in Section 4.6 and are not stores. |
| All clarifications cited | Yes. C1–C26. |
| Duplicate or conflicting definitions | None inside this dictionary. One conflict between `dfd_level_3.md` and `process_specification.md` is reported as Open Issue 12. |

### Rev 7 Re-check (C27)

| Check | Result |
|---|---|
| All clarifications cited | Yes. C1–C27. C27 appears on Direction, Clean, Motor Command, Cleaner Command, Enable "Move Forward", Trigger "Clean Normal", Section 4.4 and Section 4.6. |
| Every DFD Level 0–3 flow and store still defined | Yes. C27 adds no flow, signal or store. The 9 Level 3 signals, 2 commands, 2 stores and 7 external flows are unchanged. |
| Every control flow from 2.1.1 still defined | Yes. 9 entries, 9 dashed arrows in `dfd_level_3.md`. C27 adds a use (S0) to two existing signals, not a new signal. |
| Motor/Cleaner Command value sets and mapping | Unchanged. Each value maps 1:1 to Direction and Clean. Start-up now goes through this mapping rather than a separate send (Section 4.4). |
| Consistent with the Event List | Yes. Names, types and value sets are unchanged. The start-up values (Forward, clean_on) are the same; only the internal path changed. |
| Consistent with `dfd_level_4_fsm.md` | Yes. T1 (Init / Enable "Move Forward", Trigger "Clean Normal") matches S0 in Sections 2.1 and 4.5. Its Issue 1 Data Dictionary follow-up is done by this revision. |
| Duplicate or conflicting definitions | None inside this dictionary. The C22 wording "sends … once" in the Rev 6 notes was refined, not contradicted: the single start-up send still happens, now through the S0 commands (C27). `dfd_level_3.md` S0 and `process_specification.md` 2.1/2.2/2.3 still describe the older start-up path (Section 7). |

### Rev 8 Re-check (SA consistency pass, C1–C28)

| Check | Result |
|---|---|
| All clarifications cited | Yes. C1–C28. C28 appears on Clean, Dust Existence, Cleaner Command, Trigger "Clean Normal", Trigger "Power-Up Clean" and the power-up tick count. |
| C27 applied | Yes, since Rev 7. Re-checked against `dfd_level_3.md` S0 (now updated) and `dfd_level_4_fsm.md` T1. |
| Every DFD Level 0–3 flow and store still defined | Yes. No flow, signal or store was added or removed. |
| Consistent with `dfd_level_4_fsm.md` | Yes. T11 (power-up ends, Trigger "Clean Normal") and T6 (new power-up on later dust) match the C28 text here. |
| Duplicate or conflicting definitions | None. Section 7 lists no stale artifact. |

## 6. Open Issues and Inconsistencies

Each issue was re-checked against `clarifications.md` (C1–C28). No issue is open.

1. **Process name (cosmetic). Resolved.** The Context Diagram (Rev 2) now labels the system "RVC Control", matching DFD Level 0 and this dictionary.
2. **Event List and Context Diagram citations were out of date. Resolved.** Both (Rev 2) now cite C1–C20, and the Event List marks its Open Issues 1–7 as resolved. No definitions conflict.
3. **Tick period and sensor sampling rate. Resolved (C17).** A tick is an abstract time unit. Left, right and dust are sampled once per tick. The real period is HW detail and out of scope (R6). The definitions of Tick, the sensor inputs, Left/Right Obstacle, Dust and Dust Existence are updated.
4. **Unresolved behaviour. Resolved (C13, C14, C15).**
   - Side-only obstacle with the front clear causes no reaction (C13). Left and Right Sensor Input are updated.
   - Dust during an active power-up does not restart the 3-tick period (C14). Clean and Dust Existence are updated.
   - A front interrupt during a turn or reverse is not acted on. If the front is still blocked when a turn ends, a new turn decision starts (C15). Front Sensor Input and Motor Command are updated.
5. **Interrupt carried through a data flow. Resolved (C20).** The front interrupt updates Obstacle Location straight away. Control decisions are made on Tick, after that tick's sensing. At Level 2 this is shown by the Obstacle Location data store, which 2.1 reads on Tick. No separate control flow from Front Obstacle to the Controller is needed.
6. **Conflict between C20 and earlier artifacts. Resolved.** `dfd_level_1.md` Note 1 and the Process 2 specification used to say that Process 2 reacts to a front-obstacle change straight away. Both have been revised to match C20 (`process_specification.md` Rev 3): the front value is stored straight away, and the decision is made on the next Tick.
7. **Start-up values. Resolved (C16, C21, C22, C23).** Direction starts as Forward and Clean starts as clean_on (C16). Both are sent once at start-up, and C12 applies after that (C22). All stored sensor values start as false (C21). Left, Right and Dust are overwritten on the first tick, and C21 is kept so that behaviour is deterministic (C23). Direction, Clean and Section 4.3 are updated.
8. **Front Obstacle = false. Resolved (C19).** It only updates Obstacle Location and causes no action.
9. **Turn decision data. Resolved (C18).** The decision uses the latest stored left and right values from the Obstacle Location store.
10. **Issuing rate of Motor Command and Cleaner Command. Resolved (C24, C25, C26; DFD Level 3).** The Controller sends Enable, Disable or Trigger only on a state transition (C26). Each one-shot process (2.1.3, 2.1.4, 2.1.6–2.1.8) produces one command per Trigger (C25). The continuous processes 2.1.2 Move Forward and 2.1.5 Move Backward (C24) produce their Motor Command once on Enable and nothing until Disable. C24 states this directly ("On Enable, the process produces its Motor Command once; while enabled it produces nothing further"). This also fits Level 3, where they have no Tick input. Motor Command, Section 2.1 and Section 4.4 are updated. *(Rev 7: the earlier remark that this was "not a clarification yet" is removed, because C24 covers it. The "please confirm" in `dfd_level_3.md` Note 1 and `process_specification.md` Issue 15 has since been marked resolved (C24) in both artifacts.)*
11. **Level 3 process numbers. Resolved (DFD Level 3).** The Controller is 2.1.1. Move Forward is 2.1.2, Turn Left 2.1.3, Turn Right 2.1.4, Move Backward 2.1.5, Clean Normal 2.1.6, Clean Off 2.1.7 and Power-Up Clean 2.1.8. Section 2.1 uses these numbers.
12. **Start-up Enable of Move Forward. Resolved (C27).** At start-up the Controller enables Move Forward and triggers Clean Normal. These produce the start-up commands of C22 (Forward, clean_on), and no separate start-up send exists. This rules out option (b). It keeps option (a)'s start-up Enable, but the Forward produced is not filtered out: it *is* the start-up Direction. C27 also adds Trigger "Clean Normal" to S0, which neither option had. Updated: Section 2.1 (Enable "Move Forward", Trigger "Clean Normal", issuing rule), Section 3, Section 4.1 (Direction, Clean), Section 4.2 (Motor Command, Cleaner Command), Section 4.4, Section 4.5 and Section 4.6. No data type or value changed. The artifacts that still show the older start-up path are listed in Section 7.
13. **Dust on the tick a power-up ends (`dfd_level_4_fsm.md` Issue 2, PS Open Issue 16). Resolved (C28).** When a power-up ends, the mode returns to clean_on on that tick, and Dust Existence is not tested on that tick. If dust is still detected on a later tick, a new power-up starts; this is not a restart under C14. Updated: Clean, Dust Existence, Cleaner Command, Trigger "Clean Normal", Trigger "Power-Up Clean" and Section 4.6. No data type or value changed.

## 7. Stale Artifacts

**Rev 8: none.** The SA consistency pass updated every artifact listed below. Each one now cites C1–C28 and describes the C27 start-up path. The table is kept as the record of the Rev 7 check.

Rev 7 record (C1–C27): at that time these artifacts cited an older set of clarifications, listed as open an issue that was resolved, or described the start-up path from before C27.

| Artifact | Problem | Resolved by |
|---|---|---|
| `event_list.md` (Rev 2) | Cites C1–C20 (Sources, Open Issues header). The clarification table does not include C21–C27. The start-up rows (Forward, clean_on) cite C16 only, not C22/C27. | C21–C27 |
| `context_diagram.md` (Rev 2) | Cites C1–C20. | C21–C27 |
| `DFD/dfd_level_0.md` | Cites C1–C20 (Sources and the Process 0 row). | C21–C27 |
| `DFD/dfd_level_1.md` | Cites C1–C20. Note 3 still says "initial outputs at start-up: Partly resolved (C16) … whether they are sent at start-up is still open". | C22, C27 |
| `DFD/dfd_level_2.md` | Cites C1–C20, Data Dictionary Rev 3 and Process Specifications Rev 2. Note 5 still says it is not stated whether start-up values are sent. The Data Stores table gives no initial values. The 2.1 row does not state the on-transition rule. | C21, C22, C23, C26, C27 |
| `DFD/dfd_level_3.md` | Cites C1–C26 and Data Dictionary Rev 5. S0 says "Enable Move Forward … No cleaner Trigger: the mode is already clean_on"; C27 requires Trigger Clean Normal as well. Note 1 asks for confirmation that is now given by C24. Note 2 is still marked "Needs a decision" (resolved by C27). "Processes Not Decomposed" says 2.2 and 2.3 do "the start-up send (C22)"; under C27 they have none. | C24, C27; DD Open Issues 10, 12 |
| `DFD/dfd_level_4_fsm.md` | Up to date (C1–C27). T1 matches this dictionary. Its Issue 1 Data Dictionary item is done by this Rev 7; its other items concern `dfd_level_3.md` and `process_specification.md`. Its Issue 2 (dust on the tick a power-up ends) is awaiting confirmation and does not affect any data definition. | This revision |
| `process_specification.md` (Rev 5) | Cites C1–C26 and Data Dictionary Rev 6. Processes 0 and 2 (INIT: "send Direction(Forward); send Clean(clean_on)") and 2.2/2.3 ("Sends … once at start-up") describe a separate start-up send, which C27 rules out. 2.1 step 4 and INIT say "no Cleaner Command; 2.3 sends clean_on at start-up" and that the start-up Forward is "filtered by 2.2"; under C27 2.1 issues both, and both are sent. 2.1.1 S0 lacks Trigger Clean Normal. Open Issue 14 is still open (resolved by C27). Issue 15 asks for confirmation now given by C24. Validation rows still say "C1–C20" and "`dfd_level_3.md` does not exist yet (Open Issue 12)". | C24, C27; `/sa-process-specification` |
