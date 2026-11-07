# Process Specification

Sources: `docs/preliminary_requirements.md` (lines cited as R1–R7), `docs/clarifications.md` (C1–C29), `artifacts/SA/event_list.md`, `artifacts/SA/context_diagram.md`, `artifacts/SA/data_dictionary.md` (Rev 8), `artifacts/SA/DFD/dfd_level_0.md`, `artifacts/SA/DFD/dfd_level_1.md`, `artifacts/SA/DFD/dfd_level_2.md`, `artifacts/SA/DFD/dfd_level_3.md`, `artifacts/SA/DFD/dfd_level_4_fsm.md` (read for consistency only; no specification is written for it).

## Revision History

| Rev | Trigger | Changes |
|---|---|---|
| 1 | DFD Level 0 generated | Initialized the Process Specifications. Added Process 0 RVC Control. |
| 2 | DFD Level 1 generated | Added Process 1 Obstacle & Dust Detection and Process 2 Cleaner & Motor Control, which refine Process 0. Added Level 1 rows to the summary table and a Level 1 validation table. Process 0 is unchanged. |
| 3 | DFD Level 2 generated | Added Processes 1.1–1.6 (children of 1) and 2.1–2.3 (children of 2), with Level 2 rows and a Level 2 validation table. Re-checked the open issues against `clarifications.md`. C13–C20 resolve Open Issues 1–3 and 5–8, and C16 partly resolves Open Issue 4. As a result, Processes 0, 1 and 2 are refined: control decisions are now made on Tick after sensing (C20); a turn that ends with the front still blocked starts a new turn decision (C15); start-up values (C16), once-per-tick sampling (C17) and Front = false (C19) are stated. The previous logic is otherwise kept. |
| 4 | Re-check against `clarifications.md` C21–C26 (no new DFD level; `dfd_level_3.md` does not exist yet) | Open Issue 4 resolved (C22): Direction = Forward and Clean = clean_on are sent once at start-up by 2.2 and 2.3, and C12 applies after that. Open Issue 9 resolved (C21, C23): all stored sensor values start as false. The `TBD` initial values in Processes 0, 1 and 1.5 were replaced, and initial values were added to 1.6. Open Issue 10 resolved (C24–C26): 2.1 now issues Motor Command and Cleaner Command only when it decides a new movement or cleaning mode (a state transition), not on every tick. This matches the Level 3 Controller, which issues Enable / Disable / Trigger only on a transition. Direction and Clean are unchanged. Processes 0, 1, 2, 1.5, 1.6, 2.1, 2.2 and 2.3 were refined; all other logic is kept. No Level 3 specifications were added, because DFD Level 3 has not been generated (Open Issue 12). |
| 5 | DFD Level 3 generated (decomposition of 2.1 Main Control) | Added Processes 2.1.1–2.1.8 (children of 2.1): the control process 2.1.1 Controller, the continuous processes 2.1.2 Move Forward and 2.1.5 Move Backward (Enable / Disable, C24), and the one-shot processes 2.1.3, 2.1.4, 2.1.6, 2.1.7 and 2.1.8 (Trigger, C25). Added Level 3 rows to the summary table and a Level 3 validation table. Revised 2.1 step 4 and its INIT pseudocode to match the start-up Enable of Move Forward (`dfd_level_3.md` S0, Note 2 option (a)); this is provisional and pending a human decision (Open Issue 14). Open Issue 12 resolved; Open Issue 13 updated (the Data Dictionary item is fixed by DD Rev 6). No Level 4 FSM specification is written. All other logic is kept. |
| 6 | Re-check against `clarifications.md` C27 (no new DFD level) | Open Issue 14 resolved (C27): at start-up 2.1.1 enables Move Forward **and** triggers Clean Normal (S0, FSM T1). These produce the start-up Forward and clean_on of C22, and 2.2/2.3 have no separate start-up send. Refined 2.1 (step 4, trigger, INIT: it now issues both Motor Command = Forward and Cleaner Command = clean_on at start-up, and neither is filtered), 2.1.1 (S0 row, trigger, FSM transition column), 2.1.2 and 2.1.6 (start-up use), and 2.2 and 2.3 (no start-up send; nothing has been sent at start-up, so the first command always passes the C12 compare). Open Issue 15 resolved (C24 states "once on Enable" itself). Open Issues 4 and 13 refreshed; Open Issue 16 added (FSM Issue 2, awaiting confirmation). Processes 0, 1 and 2 are unchanged: their external behaviour (Forward and clean_on sent once at start-up) is the same under C27. No process, flow or store was added or removed. All Rev 5 specifications are kept. |
| 7 | SA consistency pass (C1–C28) | C27 (FSM Issue 1) was already applied in Rev 6; re-checked, no change. Added C28 to the power-up logic of Processes 0, 2, 2.1 and 2.1.1 (S4, S5, timing rules), and to the summary rows for 0, 2 and 2.1: when a power-up ends, the RVC returns to clean_on on that tick, and dust on a later tick starts a new power-up, which is not a restart under C14. The logic and pseudocode already behaved this way; only text, comments and citations changed. Open Issue 16 resolved (C28). Open Issue 13 resolved (the other artifacts were updated in this pass). Added the Rev 7 validation table. No process, flow or store was added or removed. All Rev 6 specifications are kept. |
| 8 | Re-check against `clarifications.md` C29 (no new DFD level) | Added C29 to the dust logic of Processes 0, 2, 2.1 and 2.1.1 (priority rules, S6, S8, S9), to the summary rows for 2 and 2.1, and to the trigger text of 2.1.6 and 2.1.8: dust on the tick a turn or reverse ends is ignored, and it is acted on from the next tick if still present. The pseudocode already behaved this way (the turn and reverse branches never test dust; FSM T16, T21, T22, T23). Only text, comments and citations changed. Added Open Issue 17 and the Rev 8 validation table. No process, flow or store was added or removed. All Rev 7 specifications are kept. |

## Process Summary (Lecture Table Format)

| Ref. No. | Name | Input | Output | Process Description |
|---|---|---|---|---|
| 0 | RVC Control | Front Sensor Input, Left Sensor Input, Right Sensor Input, Dust Sensor Input, Tick | Direction, Clean | Starts with all stored sensor values false, sends Forward and clean_on once and moves forward with normal cleaning. On each tick, after sensing, it decides. If the front is blocked while moving forward, it stops cleaning and turns left if the left side is free, otherwise right. It reverses if both sides are also blocked. A turn lasts 3 ticks. When it ends, the RVC resumes forward cleaning, or makes a new turn decision if the front is still blocked. Reversing lasts at least 3 ticks and ends with a turn when a side opens. Side-only obstacles cause no reaction. When dust is detected while moving forward, it powers up cleaning for 3 ticks. More dust does not restart the period, and an obstacle cancels it. When the power-up ends it returns to normal cleaning; dust on a later tick starts a new power-up. It sends Direction and Clean only when their values change. |
| 1 | Obstacle & Dust Detection | Front Sensor Input, Left Sensor Input, Right Sensor Input, Dust Sensor Input, Tick | Obstacle Location, Dust Existence | Converts the sensor inputs into the obstacle situation and dust status. All values start as false. It updates Front Obstacle as soon as the front sensor interrupt arrives. On each tick, it samples the left, right and dust sensors once and updates Left Obstacle, Right Obstacle and Dust Existence. It provides Obstacle Location (Front + Left + Right Obstacle) and Dust Existence to Process 2. |
| 2 | Cleaner & Motor Control | Obstacle Location, Dust Existence, Tick | Direction, Clean | Decides the movement and cleaning mode on each tick, after sensing. It starts in Forward with clean_on, and both are sent once at start-up. When Front Obstacle is true while moving forward, it cancels any power-up, sets clean_off, and then turns left (3 ticks), turns right (3 ticks) or reverses (at least 3 ticks, until a side opens). After a turn it resumes Forward with clean_on, or decides again if the front is still blocked. When Dust Existence is true while moving forward with clean_on, it powers up to clean_up for 3 ticks, then returns to clean_on on that tick; dust on a later tick starts a new power-up. It ignores dust while turning or reversing, on the tick a turn or reverse ends (C29), and during an active power-up. It sends Direction and Clean only on change. |
| 1.1 | Front Sensor Interface | Front Sensor Input | Front Obstacle | When the front sensor interrupt arrives, passes its Boolean value on straight away as Front Obstacle. A false value is passed on too. |
| 1.2 | Left Sensor Interface | Left Sensor Input, Tick | Left Obstacle | On each tick, samples the left sensor once and passes the value on as Left Obstacle. |
| 1.3 | Right Sensor Interface | Right Sensor Input, Tick | Right Obstacle | On each tick, samples the right sensor once and passes the value on as Right Obstacle. |
| 1.4 | Dust Sensor Interface | Dust Sensor Input, Tick | Dust | On each tick, samples the dust sensor once and passes the value on as Dust. |
| 1.5 | Determine Obstacle Location | Front Obstacle, Left Obstacle, Right Obstacle | Obstacle Location (to data store) | Writes each obstacle value that arrives into its part of the Obstacle Location store and leaves the other parts unchanged. All parts start as false. The front part is written at once; the left and right parts once per tick. |
| 1.6 | Determine Dust Existence | Dust | Dust Existence (to data store) | Writes the latest Dust value to the Dust Existence store once per tick. The store starts as false. |
| 2.1 | Main Control | Obstacle Location (from data store), Dust Existence (from data store), Tick | Motor Command, Cleaner Command | On each tick, after sensing, reads the two stores and decides the movement and cleaning mode. It covers forward cleaning, the turn and reverse decision, the 3-tick turn, the reverse of at least 3 ticks, the 3-tick power-up (ending with clean_on on that tick; later dust starts a new one), power-up cancellation and the dust-ignore rules (including the tick a turn or reverse ends, C29). At start-up it issues Motor Command = Forward and Cleaner Command = clean_on once (C27). After that it issues Motor Command and Cleaner Command only when it decides a new movement or cleaning mode, not on every tick. |
| 2.2 | Motor Interface | Motor Command | Direction | Sends Direction to the Motor only when Motor Command differs from the last value sent. Nothing has been sent at start-up, so the first Motor Command (Forward, C27) is always sent; this is the start-up Direction of C22. |
| 2.3 | Cleaner Interface | Cleaner Command | Clean | Sends Clean to the Cleaner only when Cleaner Command differs from the last value sent. Nothing has been sent at start-up, so the first Cleaner Command (clean_on, C27) is always sent; this is the start-up Clean of C22. |
| 2.1.1 | Controller (control process) | Obstacle Location (from data store), Dust Existence (from data store), Tick | Enable / Disable (to 2.1.2, 2.1.5); Trigger (to 2.1.3, 2.1.4, 2.1.6, 2.1.7, 2.1.8) | At start-up, enables Move Forward and triggers Clean Normal (C27). On each tick, after sensing, reads F, L, R and D, times the 3-tick turn, the reverse of at least 3 ticks and the 3-tick power-up, and decides each transition. Front is checked before dust, and left before right. Issues Enable, Disable and Trigger only on a state transition. Sends no data. State logic is in the Level 4 FSM. |
| 2.1.2 | Move Forward | Enable, Disable | Motor Command (Forward) | Active from Enable until Disable. On Enable, produces Motor Command = Forward once, then produces nothing further until Disable. |
| 2.1.3 | Turn Left | Trigger | Motor Command (Turn Left) | Runs once per Trigger: produces Motor Command = Turn Left, then ends. |
| 2.1.4 | Turn Right | Trigger | Motor Command (Turn Right) | Runs once per Trigger: produces Motor Command = Turn Right, then ends. |
| 2.1.5 | Move Backward | Enable, Disable | Motor Command (Backward) | Active from Enable until Disable. On Enable, produces Motor Command = Backward once, then produces nothing further until 2.1.1 disables it (after at least 3 ticks, when a side opens). |
| 2.1.6 | Clean Normal | Trigger | Cleaner Command (clean_on) | Runs once per Trigger: produces Cleaner Command = clean_on, then ends. |
| 2.1.7 | Clean Off | Trigger | Cleaner Command (clean_off) | Runs once per Trigger: produces Cleaner Command = clean_off, then ends. |
| 2.1.8 | Power-Up Clean | Trigger | Cleaner Command (clean_up) | Runs once per Trigger: produces Cleaner Command = clean_up, then ends. |

---

# DFD Level 0

## Process 0: RVC Control

### Process Identification

| Field | Description |
|---|---|
| Process ID | 0 |
| Process Name | RVC Control |
| DFD Level | Level 0 |
| Parent Process | None (root of the decomposition) |

### Purpose
Provide the automatic cleaning function of the RVC (R1, R7):
- drive straight forward while cleaning (R2);
- avoid obstacles by stopping cleaning and turning aside, or by reversing first when front, left and right are all blocked (R3, R4);
- power up cleaning for a while when dust is detected (R5).

Hardware control details are out of scope (R6). Sensors, the clock and the actuators are treated as abstract signals.

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Front Sensor Input | Control (interrupt) | true = obstacle in front / false = no obstacle | R3, R4, C1, C3 |
| Left Sensor Input | Data (periodic) | true = obstacle on the left / false = no obstacle | R3, R4, C2, C3, C4 |
| Right Sensor Input | Data (periodic) | true = obstacle on the right / false = no obstacle | R3, R4, C2, C3, C4 |
| Dust Sensor Input | Data (periodic) | true = dust detected / false = no dust | R5, C2, C3 |
| Tick | Control (temporal) | occurrence only | C2, C5, C6, C9, C17, C20 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Direction | Data (output command) to Motor | Forward / Backward / Turn Left / Turn Right | R2, R3, R4, C4, C5, C9, C12, C16 |
| Clean | Data (output command) to Cleaner | clean_off / clean_on / clean_up | R1, R3, R5, C6, C7, C8, C11, C12, C16 |

### Data Stores
None at Level 0 (see `dfd_level_0.md`). Values held internally are refined at lower levels:
- the latest front, left, right and dust values become the Level 2 data stores Obstacle Location and Dust Existence;
- the current direction, the cleaning mode and the tick counts stay inside 2.1–2.3.

### Trigger / Execution Condition
- **Front Sensor Input (interrupt):** the latest front value is recorded straight away (C1, C20). It is acted on at the next Tick.
- **Tick:** on every tick, the periodic sensors are sampled once (C2, C17). Then the control decision is made and the 3-tick durations advance (C5, C6, C9, C20).

### Processing Logic
1. **Start-up.** Direction = Forward and Clean = clean_on (C16). Both are sent once (C22). All recorded sensor values (front, left, right, dust) start as false (C21). Left, right and dust are overwritten on the first tick before the first decision (C20, C23).
2. **Forward cleaning (normal operation).** Direction = Forward and Clean = clean_on (R1, R2, C7). A left or right obstacle with the front clear causes no reaction (C13).
3. **Front obstacle.** On a Tick where the latest front value is true while moving forward (C20):
   1. If a power-up is active, cancel it (C11).
   2. Stop cleaning: Clean = clean_off (R3, C8).
   3. Choose the avoidance manoeuvre from the latest left and right values (C18):
      - left free → Turn Left (C4: left has priority);
      - left blocked, right free → Turn Right;
      - both blocked → Backward (R4).
4. **Turning.** Turn Left or Turn Right lasts 3 ticks (C5). When it ends:
   - if the front is clear, Direction = Forward and Clean = clean_on (R3);
   - if the front is still blocked, a new turn decision is made as in step 3.3 (C15).

   A front interrupt during the turn is not acted on (C15).
5. **Reversing.** Backward lasts at least 3 ticks. It continues while both left and right are blocked. It ends with a turn when one side opens, left first (C9). The turn then follows step 4. A front interrupt during reversing is not acted on (C15).
6. **Dust.** When the dust value is true while moving forward with clean_on, set Clean = clean_up for 3 ticks (R5, C6). Then set Clean = clean_on on the tick the power-up ends; the dust value is not tested on that tick (C28). Dust detected during an active power-up does not restart the period (C14). If dust is still detected on a later tick, a new power-up starts; this is not a restart (C28). Dust detected while turning or reversing is ignored (C10). Dust on the tick a turn or reverse ends is also ignored; it is acted on from the next tick if still present (C29).
7. **Front = false.** Only updates the recorded front value. It causes no action (C19).
8. **Output on change.** Send Direction and Clean only when their value differs from the last value sent (C12).

### Pseudocode

```
INIT:                                             // C16, C22
    direction = Forward; clean = clean_on
    send Direction(Forward); send Clean(clean_on) // once at start-up (C22); at Level 3 via S0 (C27)
    front, left, right, dust = FALSE              // C21; left/right/dust overwritten on first tick (C23)

ON Front Sensor Input:                            // C1, C20
    front = Front Sensor Input                    // false: update only (C19)

ON Tick:
    left  = Left Sensor Input                     // C2, C17: sampled once per tick
    right = Right Sensor Input
    dust  = Dust Sensor Input
    // decision after sensing (C20)

    IF direction == Turn Left OR direction == Turn Right THEN
        turnTicks = turnTicks + 1                 // front not acted on during turn (C15)
        IF turnTicks >= 3 THEN                    // C5
            IF front == TRUE THEN
                DecideAvoidance()                 // C15: new turn decision
            ELSE
                SetDirection(Forward); SetClean(clean_on)   // R3
            END IF
        END IF
    ELSE IF direction == Backward THEN
        reverseTicks = reverseTicks + 1           // front not acted on while reversing (C15)
        IF reverseTicks >= 3 AND NOT (left == TRUE AND right == TRUE) THEN   // C9
            IF left == FALSE THEN StartTurn(Turn Left)
            ELSE StartTurn(Turn Right)
            END IF
        END IF
    ELSE IF direction == Forward THEN
        IF front == TRUE THEN
            powerUpTicks = 0                      // C11: cancel power-up
            SetClean(clean_off)                   // R3, C8
            DecideAvoidance()
        ELSE IF clean == clean_up THEN            // C14: dust does not restart
            powerUpTicks = powerUpTicks + 1
            IF powerUpTicks >= 3 THEN SetClean(clean_on) END IF   // C6; dust not tested on this tick (C28)
        ELSE IF dust == TRUE THEN
            SetClean(clean_up); powerUpTicks = 0  // R5, C6; also a new power-up after one ended (C28)
        END IF
        // left/right alone with front clear: no reaction (C13)
    END IF
    // Dust is not evaluated while turning or reversing (C10),
    // nor on the tick a turn or reverse ends (C29).

DecideAvoidance():                                // R3, R4, C4, C18
    IF left == FALSE THEN StartTurn(Turn Left)
    ELSE IF right == FALSE THEN StartTurn(Turn Right)
    ELSE SetDirection(Backward); reverseTicks = 0 // R4, C9
    END IF

StartTurn(t):  SetDirection(t); turnTicks = 0     // clean stays clean_off (C8)

SetDirection(d): IF d != direction THEN direction = d; send Direction(d) END IF   // C12
SetClean(c):     IF c != clean     THEN clean = c;     send Clean(c)     END IF   // C12
```

### Relevant Requirements
R1, R2, R3, R4, R5, R6, R7, C1–C23, C27, C28, C29. (C24–C26 apply at Level 3 only.)

---

# DFD Level 1

## Process 1: Obstacle & Dust Detection

### Process Identification

| Field | Description |
|---|---|
| Process ID | 1 |
| Process Name | Obstacle & Dust Detection |
| DFD Level | Level 1 |
| Parent Process | 0 (RVC Control) |

### Purpose
Turn the four raw sensor inputs into the sensing results that the control logic uses:
- where obstacles are (Obstacle Location);
- whether dust is present (Dust Existence).

It takes over the sensing part of Process 0: receiving the front interrupt and sampling the periodic sensors (C1, C2).

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Front Sensor Input | Control (interrupt) | true / false | R3, R4, C1, C3 |
| Left Sensor Input | Data (periodic) | true / false | R3, R4, C2, C3 |
| Right Sensor Input | Data (periodic) | true / false | R3, R4, C2, C3 |
| Dust Sensor Input | Data (periodic) | true / false | R5, C2, C3 |
| Tick | Control (temporal) | occurrence only | C2, C17 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Obstacle Location | Data to Process 2 | Front Obstacle + Left Obstacle + Right Obstacle (each Boolean) | R3, R4, C1–C4 |
| Dust Existence | Data to Process 2 | true / false | R5, C2, C3 |

### Data Stores
None at Level 1. The process keeps the latest Front, Left and Right Obstacle values, so that Obstacle Location is always complete. At Level 2 these values are kept in the data stores Obstacle Location and Dust Existence.

### Trigger / Execution Condition
- **Front Sensor Input (interrupt):** handled as soon as it is raised (C1, C20).
- **Tick:** on every tick, the left, right and dust sensors are sampled once (C2, C17). This happens before Process 2 decides on that tick (C20).

### Processing Logic
0. At start-up, Front, Left and Right Obstacle and Dust Existence are all false (C21). Left, Right and Dust are overwritten on the first tick before Process 2 decides (C20, C23). Front stays false until the first front interrupt.
1. When Front Sensor Input arrives, set Front Obstacle to its value and update Obstacle Location straight away, without waiting for a tick (C1, C20). A false value is passed on too (C19).
2. On each Tick, sample Left Sensor Input, Right Sensor Input and Dust Sensor Input once (C2, C17).
3. Set Left Obstacle and Right Obstacle to the sampled left and right values. Update Obstacle Location.
4. Set Dust Existence to the sampled dust value.
5. Pass the values through unchanged. The inputs are already Boolean (C3), and no filtering or debouncing is specified.

### Pseudocode

```
INIT:                                                 // C21, C23
    frontObstacle = FALSE; leftObstacle = FALSE; rightObstacle = FALSE
    dustExistence = FALSE

ON Front Sensor Input:
    frontObstacle = Front Sensor Input                // C1, C3, C19, C20
    OUTPUT Obstacle Location = (frontObstacle, leftObstacle, rightObstacle)

ON Tick:                                              // C2, C17; before Process 2 (C20)
    leftObstacle  = Left Sensor Input
    rightObstacle = Right Sensor Input
    OUTPUT Obstacle Location = (frontObstacle, leftObstacle, rightObstacle)
    OUTPUT Dust Existence    = Dust Sensor Input
```

### Relevant Requirements
R3, R4, R5, R6, C1, C2, C3, C17, C19, C20, C21, C23.

---

## Process 2: Cleaner & Motor Control

### Process Identification

| Field | Description |
|---|---|
| Process ID | 2 |
| Process Name | Cleaner & Motor Control |
| DFD Level | Level 1 |
| Parent Process | 0 (RVC Control) |

### Purpose
Decide and send the movement direction and the cleaning mode from the sensing results:
- forward with cleaning (R2);
- obstacle avoidance by turning or reversing (R3, R4);
- powered-up cleaning on dust (R5).

It takes over the deciding and acting part of Process 0. This covers the turn and reverse choice, the 3-tick timing, the cleaning mode rules and sending outputs only on change (C4–C16, C18).

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Obstacle Location | Data from Process 1 | Front Obstacle + Left Obstacle + Right Obstacle (each Boolean) | R3, R4, C1–C4, C9, C18 |
| Dust Existence | Data from Process 1 | true / false | R5, C10, C14 |
| Tick | Control (temporal) | occurrence only | C5, C6, C9, C20 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Direction | Data (output command) to Motor | Forward / Backward / Turn Left / Turn Right | R2–R4, C4, C5, C9, C12, C16 |
| Clean | Data (output command) to Cleaner | clean_off / clean_on / clean_up | R1, R3, R5, C6–C8, C11, C12, C16 |

### Data Stores
None at Level 1. The process keeps internally:
- the current direction and cleaning mode (needed for C12);
- the turn, reverse and power-up tick counters.

At Level 2 these stay inside 2.1–2.3 and are not shown as data stores.

### Trigger / Execution Condition
- **Tick:** on every tick, after Process 1 has done that tick's sensing, the process reads Obstacle Location and Dust Existence, makes the control decision and advances the 3-tick durations (C5, C6, C9, C20).
- A change in Front Obstacle between ticks is acted on at the next Tick (C20). *(Refined in Rev 3. Rev 2 said it was handled straight away.)*

### Processing Logic
1. **Start-up.** Direction = Forward and Clean = clean_on (C16). Both are sent once (C22).
2. **Forward cleaning (normal operation).** Direction = Forward and Clean = clean_on (R1, R2, C7). Left Obstacle or Right Obstacle alone, with Front Obstacle false, causes no reaction (C13).
3. **Front obstacle.** On a Tick where Front Obstacle is true while moving forward:
   1. If a power-up is active, cancel it (C11).
   2. Stop cleaning: Clean = clean_off (R3, C8).
   3. Choose the avoidance manoeuvre from the latest Left Obstacle and Right Obstacle (C18):
      - left free → Turn Left (C4: left has priority);
      - left blocked, right free → Turn Right;
      - both blocked → Backward (R4).
4. **Turning.** Turn Left or Turn Right lasts 3 ticks (C5). When it ends:
   - if Front Obstacle is false, Direction = Forward and Clean = clean_on (R3);
   - if Front Obstacle is still true, a new turn decision is made as in step 3.3 (C15).

   Front Obstacle is not acted on during the turn (C15).
5. **Reversing.** Backward lasts at least 3 ticks. It continues while Left Obstacle and Right Obstacle are both true. It ends with a turn when one side opens, left first (C9). The turn then follows step 4. Front Obstacle is not acted on while reversing (C15).
6. **Dust.** When Dust Existence = true while moving forward with clean_on, set Clean = clean_up for 3 ticks (R5, C6). Then set Clean = clean_on on the tick the power-up ends; Dust Existence is not tested on that tick (C28). Dust Existence is ignored during an active power-up (C14) and while turning or reversing (C10). If it is still true on a later tick, a new power-up starts; this is not a restart (C28). Dust Existence on the tick a turn or reverse ends is also ignored; it is acted on from the next tick if still true (C29).
7. **Output on change.** Send Direction and Clean only when their value differs from the last value sent (C12).

### Pseudocode

```
INIT: direction = Forward; clean = clean_on        // C16
      send Direction(Forward); send Clean(clean_on)   // once at start-up (C22); at Level 3 via S0 (C27)

ON Tick:                                           // after Process 1 sensing (C20)
    (F, L, R) = Obstacle Location
    D         = Dust Existence

    IF direction == Turn Left OR direction == Turn Right THEN
        turnTicks = turnTicks + 1                  // F not acted on during turn (C15)
        IF turnTicks >= 3 THEN                     // C5
            IF F == TRUE THEN DecideAvoidance()    // C15
            ELSE SetDirection(Forward); SetClean(clean_on)   // R3
            END IF
        END IF
    ELSE IF direction == Backward THEN
        reverseTicks = reverseTicks + 1            // F not acted on while reversing (C15)
        IF reverseTicks >= 3 AND NOT (L == TRUE AND R == TRUE) THEN   // C9
            IF L == FALSE THEN StartTurn(Turn Left)
            ELSE StartTurn(Turn Right)
            END IF
        END IF
    ELSE IF direction == Forward THEN
        IF F == TRUE THEN
            powerUpTicks = 0                       // C11: cancel power-up
            SetClean(clean_off)                    // R3, C8
            DecideAvoidance()
        ELSE IF clean == clean_up THEN             // C14
            powerUpTicks = powerUpTicks + 1
            IF powerUpTicks >= 3 THEN SetClean(clean_on) END IF   // C6; D not tested on this tick (C28)
        ELSE IF D == TRUE THEN
            SetClean(clean_up); powerUpTicks = 0   // R5, C6; also a new power-up after one ended (C28)
        END IF
        // L or R alone with F == FALSE: no reaction (C13)
    END IF
    // D is not evaluated while turning or reversing (C10),
    // nor on the tick a turn or reverse ends (C29).

DecideAvoidance():                                 // R3, R4, C4, C18
    IF L == FALSE THEN StartTurn(Turn Left)
    ELSE IF R == FALSE THEN StartTurn(Turn Right)
    ELSE SetDirection(Backward); reverseTicks = 0  // R4, C9
    END IF

StartTurn(t):    SetDirection(t); turnTicks = 0          // clean stays clean_off (C8)
SetDirection(d): IF d != direction THEN direction = d; send Direction(d) END IF   // C12
SetClean(c):     IF c != clean     THEN clean = c;     send Clean(c)     END IF   // C12
```

### Relevant Requirements
R1, R2, R3, R4, R5, R7, C4–C16, C18, C20, C22, C27, C28, C29.

---

# DFD Level 2

## Process 1.1: Front Sensor Interface

### Process Identification

| Field | Description |
|---|---|
| Process ID | 1.1 |
| Process Name | Front Sensor Interface |
| DFD Level | Level 2 |
| Parent Process | 1 (Obstacle & Dust Detection) |

### Purpose
Receive the interrupt-driven front sensor input and pass on whether an obstacle is in front.

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Front Sensor Input | Control (interrupt) | true / false | R3, R4, C1, C3 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Front Obstacle | Data to 1.5 | true / false | R3, R4, C1, C3, C19 |

### Data Stores
None.

### Trigger / Execution Condition
When Front Sensor Input is raised (interrupt, C1). It does not use Tick.

### Processing Logic
1. Receive Front Sensor Input.
2. Pass its value on straight away as Front Obstacle (C1, C20). The value is already Boolean (C3), so it is not changed.
3. A false value is passed on too. It only updates the stored location (C19).

### Pseudocode

```
ON Front Sensor Input:                 // C1
    Front Obstacle = Front Sensor Input   // C3; false is passed on (C19)
    OUTPUT Front Obstacle TO 1.5
```

### Relevant Requirements
R3, R4, R6, C1, C3, C19, C20.

---

## Process 1.2: Left Sensor Interface

### Process Identification

| Field | Description |
|---|---|
| Process ID | 1.2 |
| Process Name | Left Sensor Interface |
| DFD Level | Level 2 |
| Parent Process | 1 (Obstacle & Dust Detection) |

### Purpose
Sample the periodic left sensor once per tick and pass on whether an obstacle is on the left.

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Left Sensor Input | Data (periodic) | true / false | R3, R4, C2, C3 |
| Tick | Control (temporal) | occurrence only | C2, C17 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Left Obstacle | Data to 1.5 | true / false | R3, R4, C2, C3 |

### Data Stores
None.

### Trigger / Execution Condition
On each Tick, before 2.1 Main Control decides (C2, C17, C20).

### Processing Logic
1. On Tick, sample Left Sensor Input once (C17).
2. Pass the value on unchanged as Left Obstacle (C3).

### Pseudocode

```
ON Tick:                               // C2, C17
    Left Obstacle = Left Sensor Input
    OUTPUT Left Obstacle TO 1.5
```

### Relevant Requirements
R3, R4, R6, C2, C3, C17, C20.

---

## Process 1.3: Right Sensor Interface

### Process Identification

| Field | Description |
|---|---|
| Process ID | 1.3 |
| Process Name | Right Sensor Interface |
| DFD Level | Level 2 |
| Parent Process | 1 (Obstacle & Dust Detection) |

### Purpose
Sample the periodic right sensor once per tick and pass on whether an obstacle is on the right.

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Right Sensor Input | Data (periodic) | true / false | R3, R4, C2, C3 |
| Tick | Control (temporal) | occurrence only | C2, C17 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Right Obstacle | Data to 1.5 | true / false | R3, R4, C2, C3 |

### Data Stores
None.

### Trigger / Execution Condition
On each Tick, before 2.1 Main Control decides (C2, C17, C20).

### Processing Logic
1. On Tick, sample Right Sensor Input once (C17).
2. Pass the value on unchanged as Right Obstacle (C3).

### Pseudocode

```
ON Tick:                               // C2, C17
    Right Obstacle = Right Sensor Input
    OUTPUT Right Obstacle TO 1.5
```

### Relevant Requirements
R3, R4, R6, C2, C3, C17, C20.

---

## Process 1.4: Dust Sensor Interface

### Process Identification

| Field | Description |
|---|---|
| Process ID | 1.4 |
| Process Name | Dust Sensor Interface |
| DFD Level | Level 2 |
| Parent Process | 1 (Obstacle & Dust Detection) |

### Purpose
Sample the periodic dust sensor once per tick and pass on whether dust is detected.

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Dust Sensor Input | Data (periodic) | true / false | R5, C2, C3 |
| Tick | Control (temporal) | occurrence only | C2, C17 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Dust | Data to 1.6 | true / false | R5, C2, C3 |

### Data Stores
None.

### Trigger / Execution Condition
On each Tick, before 2.1 Main Control decides (C2, C17, C20).

### Processing Logic
1. On Tick, sample Dust Sensor Input once (C17).
2. Pass the value on unchanged as Dust (C3).

### Pseudocode

```
ON Tick:                               // C2, C17
    Dust = Dust Sensor Input
    OUTPUT Dust TO 1.6
```

### Relevant Requirements
R5, R6, C2, C3, C17, C20.

---

## Process 1.5: Determine Obstacle Location

### Process Identification

| Field | Description |
|---|---|
| Process ID | 1.5 |
| Process Name | Determine Obstacle Location |
| DFD Level | Level 2 |
| Parent Process | 1 (Obstacle & Dust Detection) |

### Purpose
Combine the front, left and right obstacle values into the Obstacle Location data store, so that 2.1 Main Control always reads a complete and current obstacle situation.

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Front Obstacle | Data from 1.1 | true / false | R3, R4, C1, C19, C20 |
| Left Obstacle | Data from 1.2 | true / false | R3, R4, C2, C17 |
| Right Obstacle | Data from 1.3 | true / false | R3, R4, C2, C17 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Obstacle Location | Data, written to the Obstacle Location store | Front Obstacle + Left Obstacle + Right Obstacle | R3, R4, C1–C4, C18 |

### Data Stores

| Data Store | Access | Reference |
|---|---|---|
| Obstacle Location | Write (each part is updated separately) | C1, C2, C18, C20 |

### Trigger / Execution Condition
- When Front Obstacle arrives from 1.1: straight away, between ticks (C1, C20).
- When Left Obstacle and Right Obstacle arrive from 1.2 and 1.3: once per tick, before 2.1 reads the store (C17, C20).

### Processing Logic
1. When Front Obstacle arrives, write it to the front part of the store. Leave the left and right parts unchanged.
2. When Left Obstacle and Right Obstacle arrive, write them to the left and right parts of the store. Leave the front part unchanged.
3. Do not otherwise interpret the values. The decision rules (C4, C9, C13, C15) belong to 2.1.

### Pseudocode

```
ON Front Obstacle:                                        // C1, C20
    Obstacle Location.Front = Front Obstacle              // false: update only (C19)

ON Left Obstacle, Right Obstacle:                         // once per tick (C17)
    Obstacle Location.Left  = Left Obstacle
    Obstacle Location.Right = Right Obstacle

INIT:                                                     // C21
    Obstacle Location = (Front = FALSE, Left = FALSE, Right = FALSE)
    // Left/Right overwritten on the first tick before 2.1 reads (C20, C23);
    // Front stays FALSE until the first front interrupt.
```

### Relevant Requirements
R3, R4, C1, C2, C3, C4, C17, C18, C19, C20, C21, C23.

---

## Process 1.6: Determine Dust Existence

### Process Identification

| Field | Description |
|---|---|
| Process ID | 1.6 |
| Process Name | Determine Dust Existence |
| DFD Level | Level 2 |
| Parent Process | 1 (Obstacle & Dust Detection) |

### Purpose
Record whether dust is present at the latest sample in the Dust Existence data store.

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Dust | Data from 1.4 | true / false | R5, C2, C3 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Dust Existence | Data, written to the Dust Existence store | true / false | R5, C2, C3 |

### Data Stores

| Data Store | Access | Reference |
|---|---|---|
| Dust Existence | Write | C2, C17 |

### Trigger / Execution Condition
When Dust arrives from 1.4, once per tick, before 2.1 reads the store (C17, C20).

### Processing Logic
1. Write the Dust value to the Dust Existence store.
2. Do not otherwise interpret the value. The dust rules (C6, C10, C14) belong to 2.1.

### Pseudocode

```
INIT: Dust Existence = FALSE              // C21; overwritten on the first tick (C23)

ON Dust:                                  // once per tick (C17)
    Dust Existence = Dust
```

### Relevant Requirements
R5, C2, C3, C17, C20, C21, C23.

---

## Process 2.1: Main Control

### Process Identification

| Field | Description |
|---|---|
| Process ID | 2.1 |
| Process Name | Main Control |
| DFD Level | Level 2 |
| Parent Process | 2 (Cleaner & Motor Control) |

### Purpose
Decide the movement and cleaning mode on each tick from the stored obstacle situation and dust status. It holds all of the decision and timing logic of Process 2, while 2.2 and 2.3 handle the outputs. At Level 3 it is decomposed into the Controller (2.1.1) and the action processes (2.1.2–2.1.8).

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Obstacle Location | Data, read from the Obstacle Location store | Front Obstacle + Left Obstacle + Right Obstacle | R3, R4, C4, C9, C13, C15, C18 |
| Dust Existence | Data, read from the Dust Existence store | true / false | R5, C10, C14 |
| Tick | Control (temporal) | occurrence only | C5, C6, C9, C20 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Motor Command | Data to 2.2 | Forward / Backward / Turn Left / Turn Right | R2–R4, C4, C5, C9, C15 |
| Cleaner Command | Data to 2.3 | clean_off / clean_on / clean_up | R1, R3, R5, C6–C8, C10, C11, C14 |

### Data Stores

| Data Store | Access | Reference |
|---|---|---|
| Obstacle Location | Read (on Tick) | C18, C20 |
| Dust Existence | Read (on Tick) | C20 |

Internal state, not shared, so not a data store: the current movement, the current cleaning mode, and the turn, reverse and power-up tick counters.

### Trigger / Execution Condition
- At start-up, once (C27).
- On each Tick, after that tick's sensing (1.2–1.6) is complete (C20).

### Processing Logic
Same decision rules as Process 2 (steps 1–6), with these Level 2 refinements:
1. On Tick, read Obstacle Location (F, L, R) and Dust Existence (D) from the stores.
2. Decide the movement and cleaning mode with the Process 2 rules:
   - forward cleaning (R2, C13);
   - front obstacle → cancel power-up, clean_off, turn left, turn right or reverse (R3, R4, C4, C8, C11, C18);
   - 3-tick turn, then forward or a new decision (C5, C15);
   - reverse of at least 3 ticks (C9);
   - dust power-up of 3 ticks, ending with clean_on on that tick; dust on a later tick starts a new power-up (R5, C6, C10, C14, C28);
   - dust on the tick a turn or reverse ends is ignored and acted on from the next tick if still present (C29).
3. Issue Motor Command only when a new movement is decided, and Cleaner Command only when a new cleaning mode is decided. Each issue corresponds to a state transition (C26). Nothing is issued on a tick that keeps the current movement and mode (C24, C25). A new turn decision at the end of a turn (C15) is a transition, so its Motor Command is issued even if the turn direction is the same; 2.2 then filters it (C12). *(Refined in Rev 4. Rev 3 issued both commands on every tick. Direction and Clean are the same either way, because 2.2 and 2.3 apply C12.)*
4. At start-up, the movement is Forward and the mode is clean_on (C16). 2.1 issues Motor Command = Forward and Cleaner Command = clean_on once (C27). At Level 3 these come from the S0 Enable "Move Forward" and Trigger "Clean Normal" of 2.1.1. 2.2 and 2.3 have sent nothing yet, so both commands pass the C12 compare and become the start-up Direction and Clean of C22. There is no separate start-up send in 2.2 or 2.3. *(Refined in Rev 6 per C27. Rev 5 issued only Motor Command at start-up and said 2.2 filtered it; Rev 4 said "2.1 issues no command at start-up". Direction and Clean at start-up are Forward and clean_on in every revision.)*

### Pseudocode

```
INIT: move = Forward; mode = clean_on                    // C16
      OUTPUT Motor Command   = Forward  TO 2.2           // S0 Enable "Move Forward" (C27)
      OUTPUT Cleaner Command = clean_on TO 2.3           // S0 Trigger "Clean Normal" (C27)
      // both are the first commands, so 2.2 and 2.3 send them (C12, C22)

ON Tick:                                                 // after sensing (C20)
    (F, L, R) = READ Obstacle Location                   // C18
    D         = READ Dust Existence

    IF move == Turn Left OR move == Turn Right THEN
        turnTicks = turnTicks + 1                        // F not acted on (C15)
        IF turnTicks >= 3 THEN                           // C5
            IF F == TRUE THEN DecideAvoidance()          // C15
            ELSE SetMove(Forward); SetMode(clean_on)     // R3
            END IF
        END IF
    ELSE IF move == Backward THEN
        reverseTicks = reverseTicks + 1                  // F not acted on (C15)
        IF reverseTicks >= 3 AND NOT (L == TRUE AND R == TRUE) THEN   // C9
            IF L == FALSE THEN SetMove(Turn Left) ELSE SetMove(Turn Right) END IF
            turnTicks = 0
        END IF
    ELSE IF move == Forward THEN
        IF F == TRUE THEN
            powerUpTicks = 0                             // C11
            SetMode(clean_off)                           // R3, C8
            DecideAvoidance()
        ELSE IF mode == clean_up THEN                    // C14
            powerUpTicks = powerUpTicks + 1
            IF powerUpTicks >= 3 THEN SetMode(clean_on) END IF   // C6; D not tested on this tick (C28)
        ELSE IF D == TRUE THEN
            SetMode(clean_up); powerUpTicks = 0          // R5, C6; also a new power-up after one ended (C28)
        END IF
        // L or R alone with F == FALSE: no reaction (C13)
    END IF
    // D is not evaluated while turning or reversing (C10),
    // nor on the tick a turn or reverse ends (C29)
    // A tick with no transition issues no command (C26)

DecideAvoidance():                                       // R3, R4, C4, C18
    IF L == FALSE THEN SetMove(Turn Left);  turnTicks = 0
    ELSE IF R == FALSE THEN SetMove(Turn Right); turnTicks = 0
    ELSE SetMove(Backward); reverseTicks = 0             // R4, C9
    END IF
    // mode stays clean_off while turning or reversing (C8)

SetMove(m): move = m; OUTPUT Motor Command   = m TO 2.2  // only on a transition (C24–C26)
SetMode(c): mode = c; OUTPUT Cleaner Command = c TO 2.3  // only on a transition (C25, C26)
```

### Relevant Requirements
R1, R2, R3, R4, R5, R7, C4, C5, C6, C7, C8, C9, C10, C11, C13, C14, C15, C16, C18, C20, C22, C24, C25, C26, C27, C28, C29.

---

## Process 2.2: Motor Interface

### Process Identification

| Field | Description |
|---|---|
| Process ID | 2.2 |
| Process Name | Motor Interface |
| DFD Level | Level 2 |
| Parent Process | 2 (Cleaner & Motor Control) |

### Purpose
Send the movement direction to the Motor, only when it changes (C12).

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Motor Command | Data from 2.1 | Forward / Backward / Turn Left / Turn Right | R2–R4 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Direction | Data (output command) to Motor | Forward / Backward / Turn Left / Turn Right | R2–R4, C12 |

### Data Stores
None. The last Direction sent is internal to this process.

### Trigger / Execution Condition
When Motor Command arrives from 2.1. There is no separate start-up send (C27).

### Processing Logic
1. At start-up, no Direction has been sent yet (C27).
2. When Motor Command arrives, compare it with the last Direction sent.
3. If they differ, or nothing has been sent yet, send Direction = Motor Command to the Motor and remember it.
4. If they are the same, send nothing (C12).
5. The first Motor Command is Forward, issued by 2.1 at start-up (C27). It is always sent, so Direction = Forward is sent once at start-up (C16, C22). *(Refined in Rev 6. Rev 5 sent Forward itself at start-up and initialised the last value to Forward.)*

### Pseudocode

```
INIT: lastDirection = NONE                           // nothing sent yet (C27)

ON Motor Command:                                    // first one: Forward from S0 (C22, C27)
    IF Motor Command != lastDirection THEN           // C12
        lastDirection = Motor Command
        SEND Direction(lastDirection) TO Motor
    END IF
```

### Relevant Requirements
R2, R3, R4, R6, C12, C16, C22, C27.

---

## Process 2.3: Cleaner Interface

### Process Identification

| Field | Description |
|---|---|
| Process ID | 2.3 |
| Process Name | Cleaner Interface |
| DFD Level | Level 2 |
| Parent Process | 2 (Cleaner & Motor Control) |

### Purpose
Send the cleaning mode to the Cleaner, only when it changes (C12).

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Cleaner Command | Data from 2.1 | clean_off / clean_on / clean_up | R1, R3, R5, C7 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Clean | Data (output command) to Cleaner | clean_off / clean_on / clean_up | R1, R3, R5, C7, C12 |

### Data Stores
None. The last Clean value sent is internal to this process.

### Trigger / Execution Condition
When Cleaner Command arrives from 2.1. There is no separate start-up send (C27).

### Processing Logic
1. At start-up, no Clean value has been sent yet (C27).
2. When Cleaner Command arrives, compare it with the last Clean value sent.
3. If they differ, or nothing has been sent yet, send Clean = Cleaner Command to the Cleaner and remember it.
4. If they are the same, send nothing (C12).
5. The first Cleaner Command is clean_on, issued by 2.1 at start-up (C27). It is always sent, so Clean = clean_on is sent once at start-up (C16, C22). *(Refined in Rev 6. Rev 5 sent clean_on itself at start-up and initialised the last value to clean_on.)*

### Pseudocode

```
INIT: lastClean = NONE                               // nothing sent yet (C27)

ON Cleaner Command:                                  // first one: clean_on from S0 (C22, C27)
    IF Cleaner Command != lastClean THEN             // C12
        lastClean = Cleaner Command
        SEND Clean(lastClean) TO Cleaner
    END IF
```

### Relevant Requirements
R1, R3, R5, R6, C7, C12, C16, C22, C27.

---

# DFD Level 3

Level 3 decomposes **2.1 Main Control** only (`dfd_level_3.md`). The other Level 2 processes (1.1–1.6, 2.2, 2.3) are primitives and keep their Level 2 specifications. The signal types follow C24, C25 and C26. The Level 4 Controller FSM describes the state logic of 2.1.1 and has no separate specification here.

## Process 2.1.1: Controller

### Process Identification

| Field | Description |
|---|---|
| Process ID | 2.1.1 |
| Process Name | Controller |
| DFD Level | Level 3 (control process) |
| Parent Process | 2.1 (Main Control) |

### Purpose
Hold all decision and timing logic of 2.1. It reads the stored obstacle situation and dust status on each Tick and decides when each movement and cleaning action starts or stops. It sends no data, only the control signals Enable, Disable and Trigger.

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Obstacle Location | Data, read from the Obstacle Location store | Front Obstacle (F) + Left Obstacle (L) + Right Obstacle (R), each Boolean | R3, R4, C4, C9, C13, C15, C18, C20 |
| Dust Existence | Data, read from the Dust Existence store | D, Boolean | R5, C10, C14 |
| Tick | Control (temporal); the only event of 2.1.1 | occurrence only | C5, C6, C9, C17, C20 |

### Outputs (control signals)

| Output | Category | Destination | Reference |
|---|---|---|---|
| Enable "Move Forward" | Control (Enable/Disable) | 2.1.2 Move Forward | R2, R3, C16, C24, C26 |
| Disable "Move Forward" | Control (Enable/Disable) | 2.1.2 Move Forward | R3, R4, C24, C26 |
| Trigger "Turn Left" | Control (Trigger) | 2.1.3 Turn Left | R3, R4, C4, C5, C25, C26 |
| Trigger "Turn Right" | Control (Trigger) | 2.1.4 Turn Right | R3, R4, C5, C25, C26 |
| Enable "Move Backward" | Control (Enable/Disable) | 2.1.5 Move Backward | R4, C9, C24, C26 |
| Disable "Move Backward" | Control (Enable/Disable) | 2.1.5 Move Backward | R4, C9, C24, C26 |
| Trigger "Clean Normal" | Control (Trigger) | 2.1.6 Clean Normal | R1, R2, C6, C7, C25, C26 |
| Trigger "Clean Off" | Control (Trigger) | 2.1.7 Clean Off | R3, C7, C8, C11, C25, C26 |
| Trigger "Power-Up Clean" | Control (Trigger) | 2.1.8 Power-Up Clean | R5, C6, C7, C25, C26 |

### Data Stores

| Data Store | Access | Reference |
|---|---|---|
| Obstacle Location | Read (on Tick, after sensing) | C18, C20 |
| Dust Existence | Read (on Tick, after sensing) | C20 |

Internal data, hidden inside 2.1.1 and not a data store (DD Section 4.6): current movement, cleaning mode, and the turn, reverse and power-up tick counters.

### Trigger / Execution Condition
- On each Tick, after that tick's sensing (1.2–1.6) is complete (C20).
- At start-up, once (S0, C27; FSM initial transition T1).
- A front interrupt is not an event for 2.1.1. It updates the store straight away, and 2.1.1 reads F at the next Tick (C20).

### Processing Logic

**Timing rules** (all durations are timed here, because 2.1.2–2.1.8 have no Tick input):
- A turn (Turn Left or Turn Right) lasts 3 ticks (C5). F is not acted on during the turn (C15).
- Reversing lasts at least 3 ticks. It continues while L and R are both true, and ends with a turn when a side opens (C9). F is not acted on while reversing (C15).
- A power-up lasts 3 ticks (C6). Dust during an active power-up does not restart it (C14). A front obstacle cancels it (C11). When it ends, 2.1.1 triggers Clean Normal on that tick without testing D; if D is still true on a later tick, a new power-up starts (S4), which is not a restart (C28).
- Tick counting is the same as in the 2.1 pseudocode: a counter is set to 0 when its period starts and is incremented on each later Tick; the period ends on the Tick where it reaches 3.

**Priority rules:**
- Front before dust: while moving forward, F = true is handled before D, so an obstacle cancels or prevents a power-up (R3, C11).
- Left before right: when choosing a turn, L = false gives Turn Left before R = false gives Turn Right (C4). This applies to S1/S2, S7 and S8/S9.
- L or R alone with F = false causes no reaction (C13). D while turning or reversing is ignored (C10), and so is D on the tick a turn or reverse ends; it is acted on from the next tick if still true (C29). F = false causes no action (C19).

**Issuing rule:** 2.1.1 issues signals only on a state transition, never on every tick (C26). A Tick that matches no row below issues no signal.

**Signals per situation** (from `dfd_level_3.md`, with S0 per C27; FSM column from `dfd_level_4_fsm.md`):

| # | Situation | Signals issued | FSM | Reference |
|---|---|---|---|---|
| S0 | Start-up | Enable Move Forward; Trigger Clean Normal. These produce the start-up Forward and clean_on (C22); 2.2 and 2.3 have no separate start-up send. *(Refined in Rev 6 per C27. Rev 5 had no cleaner Trigger.)* | T1 | C16, C22, C24, C25, C27 |
| S1 | Moving forward, F = true, L = false | Disable Move Forward; Trigger Clean Off; Trigger Turn Left (any power-up cancelled) | T2, T7 | R3, C4, C8, C11, C18 |
| S2 | Moving forward, F = true, L = true, R = false | Disable Move Forward; Trigger Clean Off; Trigger Turn Right (any power-up cancelled) | T3, T8 | R3, C4, C8, C11, C18 |
| S3 | Moving forward, F = true, L = true, R = true | Disable Move Forward; Trigger Clean Off; Enable Move Backward (any power-up cancelled) | T4, T9 | R4, C8, C9, C11 |
| S4 | Moving forward with clean_on, F = false, D = true | Trigger Power-Up Clean (also a new power-up on a tick after one has ended, C28) | T6 | R5, C6, C28 |
| S5 | Moving forward with clean_up, F = false, 3 power-up ticks elapsed | Trigger Clean Normal (D not tested on this tick, C28) | T11 | C6, C14, C28 |
| S6 | Turning, 3 turn ticks elapsed, F = false | Enable Move Forward; Trigger Clean Normal (D not tested on this tick, C29) | T16, T21 | R3, C5, C29 |
| S7 | Turning, 3 turn ticks elapsed, F = true | Trigger Turn Left (L = false), or Trigger Turn Right (L = true, R = false), or Enable Move Backward (L = R = true). Mode stays clean_off. | T13–T15, T18–T20 | C4, C9, C15, C18 |
| S8 | Reversing, at least 3 reverse ticks, L = false | Disable Move Backward; Trigger Turn Left (D not tested on this tick, C29) | T22 | R4, C4, C9, C29 |
| S9 | Reversing, at least 3 reverse ticks, L = true, R = false | Disable Move Backward; Trigger Turn Right (D not tested on this tick, C29) | T23 | R4, C9, C29 |
| – | Any other tick (no transition) | none (C26) | T5, T10, T12, T17, T24, T25 | C10, C13, C14, C15, C19, C26 |

The order of several signals within one transition is not stated in the requirements and does not change any output (`dfd_level_3.md` Note 4). The states, transitions and counter handling of 2.1.1 are given by the Level 4 Controller FSM (`dfd_level_4_fsm.md`); no separate specification is written for the FSM.

### Relevant Requirements
R1, R2, R3, R4, R5, C4, C5, C6, C8, C9, C10, C11, C13, C14, C15, C16, C18, C19, C20, C22, C24, C25, C26, C27, C28, C29.

---

## Process 2.1.2: Move Forward

### Process Identification

| Field | Description |
|---|---|
| Process ID | 2.1.2 |
| Process Name | Move Forward |
| DFD Level | Level 3 (data process, continuous) |
| Parent Process | 2.1 (Main Control) |

### Purpose
Produce the forward movement command while forward movement is active.

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Enable "Move Forward" | Control (Enable/Disable) from 2.1.1 | event, no value | R2, R3, C16, C24 |
| Disable "Move Forward" | Control (Enable/Disable) from 2.1.1 | event, no value | R3, R4, C24 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Motor Command | Data to 2.2 Motor Interface | Forward | R2, C24 |

### Data Stores
None.

### Trigger / Execution Condition
Active from Enable until Disable. Enabled at start-up (S0, C27) and after a completed turn with the front clear (S6).

### Processing Logic
1. On Enable, produce Motor Command = Forward once.
2. Stay active, producing nothing further, until Disable.
3. On Disable, become inactive. Disable produces no command.
4. The Motor Command produced by the start-up Enable is the start-up Direction = Forward (C22, C27).

### Pseudocode

```
ON Enable "Move Forward":                 // C24; at start-up too (C27)
    active = TRUE
    OUTPUT Motor Command = Forward TO 2.2 // once
ON Disable "Move Forward":
    active = FALSE                        // no command
```

### Relevant Requirements
R2, R3, C16, C22, C24, C26, C27.

---

## Process 2.1.3: Turn Left

### Process Identification

| Field | Description |
|---|---|
| Process ID | 2.1.3 |
| Process Name | Turn Left |
| DFD Level | Level 3 (data process, one-shot) |
| Parent Process | 2.1 (Main Control) |

### Purpose
Produce the left-turn movement command.

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Trigger "Turn Left" | Control (Trigger) from 2.1.1 | event, no value | R3, R4, C4, C25 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Motor Command | Data to 2.2 Motor Interface | Turn Left | R3, R4, C4, C25 |

### Data Stores
None.

### Trigger / Execution Condition
Runs once per Trigger.

### Processing Logic
1. Produce one command, Motor Command = Turn Left, then end.
2. The 3-tick turn duration is timed by 2.1.1, not here (C5).

### Pseudocode

```
ON Trigger "Turn Left":                     // C25
    OUTPUT Motor Command = Turn Left TO 2.2
```

### Relevant Requirements
R3, R4, C4, C5, C25.

---

## Process 2.1.4: Turn Right

### Process Identification

| Field | Description |
|---|---|
| Process ID | 2.1.4 |
| Process Name | Turn Right |
| DFD Level | Level 3 (data process, one-shot) |
| Parent Process | 2.1 (Main Control) |

### Purpose
Produce the right-turn movement command.

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Trigger "Turn Right" | Control (Trigger) from 2.1.1 | event, no value | R3, R4, C25 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Motor Command | Data to 2.2 Motor Interface | Turn Right | R3, R4, C25 |

### Data Stores
None.

### Trigger / Execution Condition
Runs once per Trigger.

### Processing Logic
1. Produce one command, Motor Command = Turn Right, then end.
2. The 3-tick turn duration is timed by 2.1.1, not here (C5).

### Pseudocode

```
ON Trigger "Turn Right":                     // C25
    OUTPUT Motor Command = Turn Right TO 2.2
```

### Relevant Requirements
R3, R4, C5, C25.

---

## Process 2.1.5: Move Backward

### Process Identification

| Field | Description |
|---|---|
| Process ID | 2.1.5 |
| Process Name | Move Backward |
| DFD Level | Level 3 (data process, continuous) |
| Parent Process | 2.1 (Main Control) |

### Purpose
Produce the backward movement command while reversing is active, when front, left and right are all blocked (R4).

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Enable "Move Backward" | Control (Enable/Disable) from 2.1.1 | event, no value | R4, C9, C24 |
| Disable "Move Backward" | Control (Enable/Disable) from 2.1.1 | event, no value | R4, C9, C24 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Motor Command | Data to 2.2 Motor Interface | Backward | R4, C24 |

### Data Stores
None.

### Trigger / Execution Condition
Active from Enable until Disable.

### Processing Logic
1. On Enable, produce Motor Command = Backward once.
2. Stay active, producing nothing further, until Disable.
3. Reversing continues until 2.1.1 disables it, after at least 3 ticks, when a side opens (C9). The duration is timed by 2.1.1, not here.
4. On Disable, become inactive. Disable produces no command.

### Pseudocode

```
ON Enable "Move Backward":                 // C24
    active = TRUE
    OUTPUT Motor Command = Backward TO 2.2 // once
ON Disable "Move Backward":                // from 2.1.1 after >= 3 ticks, side open (C9)
    active = FALSE                         // no command
```

### Relevant Requirements
R4, C9, C24, C26.

---

## Process 2.1.6: Clean Normal

### Process Identification

| Field | Description |
|---|---|
| Process ID | 2.1.6 |
| Process Name | Clean Normal |
| DFD Level | Level 3 (data process, one-shot) |
| Parent Process | 2.1 (Main Control) |

### Purpose
Produce the normal cleaning command.

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Trigger "Clean Normal" | Control (Trigger) from 2.1.1 | event, no value | R1, R2, C25 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Cleaner Command | Data to 2.3 Cleaner Interface | clean_on | R1, R2, C7, C25 |

### Data Stores
None.

### Trigger / Execution Condition
Runs once per Trigger. Triggered at start-up (S0, C27), when the 3-tick power-up ends, on that tick (S5, C28), and after a completed turn with the front clear (S6; dust is not tested on that tick, C29).

### Processing Logic
1. Produce one command, Cleaner Command = clean_on, then end.
2. The command produced by the start-up Trigger is the start-up Clean = clean_on (C22, C27).

### Pseudocode

```
ON Trigger "Clean Normal":                     // C25; at start-up too (C27)
    OUTPUT Cleaner Command = clean_on TO 2.3
```

### Relevant Requirements
R1, R2, R3, C6, C7, C16, C22, C25, C27, C28, C29.

---

## Process 2.1.7: Clean Off

### Process Identification

| Field | Description |
|---|---|
| Process ID | 2.1.7 |
| Process Name | Clean Off |
| DFD Level | Level 3 (data process, one-shot) |
| Parent Process | 2.1 (Main Control) |

### Purpose
Produce the stop-cleaning command, used only when reversing or rotating (C8).

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Trigger "Clean Off" | Control (Trigger) from 2.1.1 | event, no value | R3, C8, C25 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Cleaner Command | Data to 2.3 Cleaner Interface | clean_off | R3, C7, C8, C25 |

### Data Stores
None.

### Trigger / Execution Condition
Runs once per Trigger.

### Processing Logic
1. Produce one command, Cleaner Command = clean_off, then end.

### Pseudocode

```
ON Trigger "Clean Off":                         // C25
    OUTPUT Cleaner Command = clean_off TO 2.3
```

### Relevant Requirements
R3, C7, C8, C11, C25.

---

## Process 2.1.8: Power-Up Clean

### Process Identification

| Field | Description |
|---|---|
| Process ID | 2.1.8 |
| Process Name | Power-Up Clean |
| DFD Level | Level 3 (data process, one-shot) |
| Parent Process | 2.1 (Main Control) |

### Purpose
Produce the powered-up cleaning command when dust is detected (R5).

### Inputs

| Input | Category | Values | Reference |
|---|---|---|---|
| Trigger "Power-Up Clean" | Control (Trigger) from 2.1.1 | event, no value | R5, C25 |

### Outputs

| Output | Category | Values | Reference |
|---|---|---|---|
| Cleaner Command | Data to 2.3 Cleaner Interface | clean_up | R5, C7, C25 |

### Data Stores
None.

### Trigger / Execution Condition
Runs once per Trigger. Triggered when dust is found while moving forward with clean_on (S4), including on a tick after an earlier power-up has ended (C28) and from the tick after a turn has ended with the front clear (C29).

### Processing Logic
1. Produce one command, Cleaner Command = clean_up, then end.
2. The 3-tick power-up duration is timed by 2.1.1, not here (C6).

### Pseudocode

```
ON Trigger "Power-Up Clean":                   // C25
    OUTPUT Cleaner Command = clean_up TO 2.3
```

### Relevant Requirements
R5, C6, C7, C25, C28, C29.

---

# Validation (after DFD Level 0)

| Check | Result |
|---|---|
| Every DFD Level 0 process has a specification | Yes. Process 0 RVC Control. |
| Process ID and name match the DFD | Yes. "0 RVC Control". |
| Inputs and outputs match the DFD | Yes. 5 inputs (Front/Left/Right/Dust Sensor Input, Tick) and 2 outputs (Direction, Clean), with the same names and categories. |
| Consistent with the Data Dictionary | Yes. Flow names, types, values and references match `data_dictionary.md` Rev 2. |
| Consistent with requirements | Yes. Each logic step cites R1–R7 or C1–C12. |
| Child consistency | N/A. There are no child processes yet. |
| Existing specifications preserved | N/A. This is the first revision. |

# Validation (after DFD Level 1)

| Check | Result |
|---|---|
| Every DFD Level 1 process has a specification | Yes. Process 1 Obstacle & Dust Detection and Process 2 Cleaner & Motor Control. |
| Process IDs and names match the DFD | Yes. "1 Obstacle & Dust Detection" and "2 Cleaner & Motor Control". |
| Inputs and outputs match the DFD | Yes. Process 1 takes the 4 sensor inputs and Tick, and outputs Obstacle Location and Dust Existence. Process 2 takes Obstacle Location, Dust Existence and Tick, and outputs Direction and Clean. |
| Consistent with the Data Dictionary | Yes. Names, types, components (Front/Left/Right Obstacle) and update timing match `data_dictionary.md` Rev 3. |
| Children refine the parent | Yes. Together, Processes 1 and 2 cover every Process 0 logic step: sensing (C1, C2) is in Process 1, and decision, timing and output (R2–R5, C4–C12) are in Process 2. Process 2's logic is the same as Process 0's, written in terms of the internal flows. No behaviour was added or dropped. |
| Consistent with requirements | Yes. Each logic step cites R1–R7 or C1–C12. |
| Existing specifications preserved | Yes. Process 0 is unchanged. |

# Validation (after DFD Level 2)

| Check | Result |
|---|---|
| Every DFD Level 2 process has a specification | Yes. 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 2.1, 2.2 and 2.3. |
| Process IDs and names match the DFD | Yes. Same numbers and names as `dfd_level_2.md`. |
| Inputs and outputs match the DFD | Yes. 1.1: Front Sensor Input → Front Obstacle. 1.2/1.3/1.4: sensor input + Tick → Left Obstacle / Right Obstacle / Dust. 1.5: Front/Left/Right Obstacle → Obstacle Location store. 1.6: Dust → Dust Existence store. 2.1: both stores + Tick → Motor Command, Cleaner Command. 2.2: Motor Command → Direction. 2.3: Cleaner Command → Clean. |
| Data store access matches the DFD | Yes. 1.5 and 1.6 write; 2.1 reads. No other process uses the stores. |
| Consistent with the Data Dictionary | Yes. Names, types, values, store contents and update timing match `data_dictionary.md` Rev 4. Motor Command and Cleaner Command use the Direction and Clean values. |
| Children refine the parent | Yes. 1.1–1.6 together do exactly the Process 1 logic: interrupt handling in 1.1, tick sampling in 1.2–1.4, and combining in 1.5–1.6. 2.1–2.3 together do exactly the Process 2 logic: the decision and timing rules in 2.1, and C12 in 2.2 and 2.3. No behaviour was added or dropped. |
| Parents refined consistently (Rev 3) | Yes. The changes to Processes 0, 1 and 2 come only from C13–C20 and are applied the same way at every level. They do not change the external interface. |
| Consistent with requirements | Yes. Each logic step cites R1–R7 or C1–C20. |
| Existing specifications preserved | Yes. Every Rev 2 process is still here. The only changes are the refinements recorded in the Revision History (Rev 3). |

# Validation (Rev 4 re-check, C21–C26)

| Check | Result |
|---|---|
| Every DFD Level 0–2 process still has a specification | Yes. 0, 1, 2, 1.1–1.6 and 2.1–2.3. No process was added or removed. |
| Level 3 processes specified | No. `dfd_level_3.md` does not exist yet (Open Issue 12). |
| Inputs and outputs still match the DFD | Yes. No flow was added, removed or renamed. Only the issuing time of Motor Command and Cleaner Command changed. |
| Consistent with the Data Dictionary (Rev 5) | Yes. The store initial values (all false, C21), the one-time start-up send by 2.2 and 2.3 (C22, DD Section 4.4) and the one-to-one command mapping match. See Open Issue 13 for the remaining DD note on Issue 10. |
| Children refine the parent | Yes. Processes 0, 1 and 2 state the same start-up behaviour (C21, C22) that 1.5, 1.6, 2.2 and 2.3 carry out. 2.1 issuing commands only on transitions, plus 2.2 and 2.3 filtering (C12), gives the same Direction and Clean sequence as Process 2. |
| Consistent with requirements | Yes. Each new or changed step cites C21–C26. No behaviour was added beyond them. |
| Existing specifications preserved | Yes. Every Rev 3 process is still here. Rev 3 wording that changed is marked with *(Refined in Rev 4)* or recorded in the Revision History. |

# Validation (after DFD Level 3)

| Check | Result |
|---|---|
| Every DFD Level 3 process has a specification | Yes. 2.1.1, 2.1.2, 2.1.3, 2.1.4, 2.1.5, 2.1.6, 2.1.7 and 2.1.8. The Level 4 FSM has no specification, as required. |
| Process IDs and names match the DFD | Yes. Same numbers and names as `dfd_level_3.md`. |
| Inputs and outputs match the DFD | Yes. 2.1.1: both store reads + Tick → 9 control signals. 2.1.2 / 2.1.5: Enable, Disable → Motor Command (Forward / Backward). 2.1.3 / 2.1.4: Trigger → Motor Command (Turn Left / Turn Right). 2.1.6 / 2.1.7 / 2.1.8: Trigger → Cleaner Command (clean_on / clean_off / clean_up). |
| Signal types match C24, C25, C26 | Yes. Enable/Disable only for 2.1.2 and 2.1.5 ("Active from Enable until Disable"). Trigger only for 2.1.3, 2.1.4, 2.1.6–2.1.8 ("Runs once per Trigger"). 2.1.1 issues signals only on a state transition. |
| Data store access matches the DFD | Yes. 2.1.1 reads both stores. No action process uses a store. Level 3 adds no stores. |
| Consistent with the Data Dictionary (Rev 6) | Yes. Signal names, destinations and the per-situation table match DD Sections 2.1 and 4.5. Motor Command and Cleaner Command producers and values match DD Section 4.4. Internal data of 2.1.1 matches DD Section 4.6. |
| Children refine the parent | Yes. 2.1.1 carries out every 2.1 decision and timing rule (S1–S9 cover 2.1 pseudocode branches). The action processes carry out `SetMove(m)` and `SetMode(c)`: each value of Motor Command and Cleaner Command has exactly one producer. 2.1.1 issues signals only on a transition, so commands are issued exactly when 2.1 issues them. Inputs (both stores, Tick) and outputs (Motor Command, Cleaner Command) balance with 2.1. |
| Parent refined consistently (Rev 5) | Yes, with one provisional change: 2.1 step 4 now issues Motor Command = Forward at start-up to match S0 (Open Issue 14). Direction and Clean are unchanged, because 2.2 filters it (C12). |
| Consistent with requirements | Yes. Each step cites R1–R7 or C1–C26. No behaviour was added beyond them. |
| Existing specifications preserved | Yes. Every Rev 4 process is still here. The only change to existing text is 2.1 step 4 and its INIT pseudocode, marked *(Refined in Rev 5)*. |

# Validation (Rev 6 re-check, C27)

The validation tables above are kept as the record of each earlier revision. They cite the clarifications and revisions that were current then. This table is the current check.

| Check | Result |
|---|---|
| Every DFD Level 0–3 process has a specification | Yes. 0; 1, 2; 1.1–1.6, 2.1–2.3; 2.1.1–2.1.8 (20 processes). No process was added or removed. The Level 4 FSM has no specification, as required. |
| Process IDs, names, inputs and outputs match the DFD | Yes. C27 adds no flow, signal or store. Only the start-up use of Enable "Move Forward" and Trigger "Clean Normal" changed. |
| Signal types match C24, C25, C26 | Yes. Unchanged. S0 uses one Enable (2.1.2) and one Trigger (2.1.6), both of the correct type. |
| Consistent with the Data Dictionary (Rev 7) | Yes. S0 = Enable "Move Forward" + Trigger "Clean Normal" (DD Sections 2.1, 4.5). 2.2 and 2.3 have no separate start-up send, and nothing has been sent at start-up (DD Sections 3, 4.4). Start-up values of the current movement and cleaning mode match DD Section 4.6. |
| Consistent with the Level 4 FSM | Yes. S0–S9 map to T1–T23, and the no-signal ticks map to T5, T10, T12, T17, T24 and T25 (2.1.1 FSM column). The 2.1 pseudocode follows the same counter rule (reset on entry, increment per Tick, period ends at 3). |
| Children refine the parent | Yes. 2.1 issues Forward and clean_on at start-up, and 2.1.1's S0 issues exactly the signals that make 2.1.2 and 2.1.6 produce them. Because 2.2 and 2.3 start with nothing sent, Processes 0, 1 and 2 still see Direction = Forward and Clean = clean_on sent once at start-up (C22). Their text is therefore unchanged. |
| Consistent with requirements | Yes. Each changed step cites C27 (with C16, C22, C24, C25). No behaviour was added beyond C27. |
| Existing specifications preserved | Yes. Every Rev 5 process is still here. Changed text is marked *(Refined in Rev 6)* or recorded in the Revision History. |

# Validation (Rev 7, SA consistency pass, C1–C28)

This table is the current check. The tables above are kept as the record of earlier revisions.

| Check | Result |
|---|---|
| Every DFD Level 0–3 process has a specification | Yes. 20 processes: 0; 1, 2; 1.1–1.6, 2.1–2.3; 2.1.1–2.1.8. The Level 4 FSM has no specification, as required. |
| Process IDs, names, inputs and outputs match the DFD | Yes. Unchanged since Rev 6. |
| C27 applied | Yes, since Rev 6. 2.1 step 4, 2.1.1 S0, 2.1.2, 2.1.6, 2.2 and 2.3 match `dfd_level_3.md` S0 (now updated) and FSM T1. |
| C28 applied | Yes. Processes 0, 2, 2.1 and 2.1.1 state that the power-up ends with clean_on on that tick, without testing dust, and that later dust starts a new power-up. The pseudocode already behaved this way (FSM T11, T6). |
| Consistent with the Data Dictionary (Rev 8) | Yes. Power-up text matches DD Clean, Dust Existence, Trigger "Clean Normal", Trigger "Power-Up Clean" and Section 4.6. |
| Consistent with requirements | Yes. Each changed step cites C28. No behaviour was added beyond it. |
| Existing specifications preserved | Yes. Every Rev 6 process is still here. |

# Validation (Rev 8 re-check, C29)

This table is the current check. The tables above are kept as the record of earlier revisions.

| Check | Result |
|---|---|
| Every DFD Level 0–3 process has a specification | Yes. Unchanged since Rev 7 (20 processes). |
| Process IDs, names, inputs and outputs match the DFD | Yes. Unchanged since Rev 6. |
| C29 applied | Yes. Processes 0, 2, 2.1 and 2.1.1 state that dust on the tick a turn or reverse ends is ignored and acted on from the next tick. 2.1.1 S6, S8 and S9 match FSM T16, T21, T22 and T23, none of which tests D. |
| Pseudocode unchanged | Yes. The turn and reverse branches never tested dust; only comments were added. |
| Consistent with the FSM | Yes. `dfd_level_4_fsm.md` cites C29 on T16, T21, T22, T23, in its clarification coverage table and in Issue 6. |
| Consistent with requirements | Yes. Each changed step cites C29. No behaviour was added beyond it. |
| Existing specifications preserved | Yes. Every Rev 7 process is still here. |

# Open Issues

Each issue was re-checked against `clarifications.md` (C1–C29). No issue is open.

1. **Side-only obstacle (Event List Open Issue 3). Resolved (C13).** A left or right obstacle with the front clear causes no reaction; the RVC keeps moving forward. Processes 0, 2 and 2.1 state this.
2. **Repeated dust during clean_up (Event List Open Issue 5). Resolved (C14).** Dust during an active power-up does not restart the 3-tick period. This matches the earlier logic. Processes 0, 2 and 2.1 now cite C14.
3. **Front obstacle while turning or reversing. Resolved (C15).** It is not acted on. When a turn ends with the front still blocked, a new turn decision starts. This was added to Processes 0, 2 and 2.1 (`DecideAvoidance()` at the end of a turn).
4. **Initial outputs. Resolved (C16, C22, C27).** At start-up, Direction = Forward and Clean = clean_on (C16). They are sent once at start-up, and C12 applies after that (C22). Processes 0 and 2 state it at their level. *(Rev 6: they are sent because 2.1 issues Motor Command = Forward and Cleaner Command = clean_on at start-up, through the S0 Enable "Move Forward" and Trigger "Clean Normal" of 2.1.1. 2.2 and 2.3 have no separate start-up send (C27).)*
5. **Tick period and sampling rate (Event List Open Issue 6). Resolved (C17).** A tick is an abstract time unit. Left, right and dust are sampled once per tick. The real period is out of scope (R6).
6. **Turn decision timing. Resolved (C18, C20).** The decision uses the latest stored left and right values. Because the decision is made after that tick's sensing (C20), these are the values sampled on the same tick.
7. **Front Obstacle returning to false. Resolved (C19).** It only updates Obstacle Location and causes no action.
8. **Same tick ordering. Resolved (C20).** The front interrupt updates Obstacle Location straight away. Control decisions are made on Tick, after that tick's sensing. As a result, the Rev 2 trigger of Process 2 ("handled straight away") was refined to "at the next Tick".
9. **Initial contents of the data stores. Resolved (C21, C23).** Front, Left and Right Obstacle and Dust Existence all start as false (C21). Left, Right and Dust are overwritten on the first tick before 2.1 reads them (C20, C23). Front stays false until the first front interrupt, so the first decision sees no front obstacle. Processes 0, 1, 1.5 and 1.6 now initialise to false.
10. **Command issuing rate. Resolved (C24, C25, C26).** The Level 3 Controller issues Enable / Disable / Trigger only on a state transition (C26). Each one-shot process produces one command per Trigger (C25). Move Forward and Move Backward produce their Motor Command once on Enable and nothing more until Disable (C24, following the Level 3 rules of the `sa-process-specification` skill). 2.1 is refined to match: it issues Motor Command and Cleaner Command only on a transition. Direction and Clean are unchanged.
11. **Earlier artifacts were out of date. Resolved.** These artifacts now cite C1–C20:
    - the Event List and Context Diagram (Rev 2);
    - `dfd_level_0.md` and `dfd_level_1.md`.

    `dfd_level_1.md` Note 1 now matches C20. The Context Diagram process name is "RVC Control". The requirements file name `preliminary_requirements.md` is now used consistently in the artifacts. The external interface is not affected.
12. **Level 3 Process Specifications. Resolved (Rev 5).** `dfd_level_3.md` has been generated. 2.1.1 Controller and 2.1.2–2.1.8 are specified in the DFD Level 3 section, with the numbers and flows from the diagram.
13. **Remaining inconsistencies in other artifacts. Resolved (Rev 7, SA consistency pass).** Every item below was updated, and all artifacts now cite C1–C28. Record of the Rev 6 check:
    - ~~`data_dictionary.md` Rev 5, Open Issue 10 and the Motor Command note, still say it is open whether Move Forward and Move Backward issue their command once on Enable or on every tick.~~ Fixed by DD Rev 6 (once on Enable). DD Rev 7 is up to date with C27.
    - `dfd_level_4_fsm.md` is up to date (C1–C27). Its Issue 1 items for this file are done by Rev 6.
    - `dfd_level_3.md` cites C1–C26. S0 says "No cleaner Trigger: the mode is already clean_on"; C27 also requires Trigger "Clean Normal". Note 1 asks for confirmation that C24 now gives. Note 2 is still marked "Needs a decision" (resolved by C27). "Processes Not Decomposed" says 2.2 and 2.3 do "the start-up send (C22)"; under C27 they have none.
    - `dfd_level_2.md` Note 5 still says it is not stated whether start-up values are sent (now C22, C27). Its Data Stores table has no initial values (now C21). The 2.1 row says it "issues Motor Command and Cleaner Command" without the on-transition rule (C26). The 2.2 and 2.3 rows cite C16 only.
    - `event_list.md`, `context_diagram.md`, `dfd_level_0.md` and `dfd_level_1.md` cite only C1–C20. `dfd_level_1.md` Note 3 still lists the start-up send as partly open (now C22).

    None of these changes the external interface.
14. **Start-up Enable of Move Forward. Resolved (C27).** At start-up the Controller enables Move Forward and triggers Clean Normal. These produce the start-up commands of C22, and no separate start-up send exists. This rules out option (b) and keeps option (a)'s start-up Enable, with two corrections: the start-up Forward is not filtered by 2.2 but *is* the start-up Direction, and S0 also triggers Clean Normal. Updated: 2.1 (step 4, trigger, INIT), 2.1.1 (trigger, S0), 2.1.2, 2.1.6, 2.2 and 2.3, and the summary rows for 2.1, 2.2, 2.3 and 2.1.1.
15. **Command production of continuous processes. Resolved (C24).** 2.1.2 and 2.1.5 produce Motor Command once on Enable and nothing more until Disable. C24 now states this directly ("On Enable, the process produces its Motor Command once; while enabled it produces nothing further"). No change to the specifications.
16. **Dust on the tick a power-up ends (`dfd_level_4_fsm.md` Issue 2). Resolved (C28).** When the third power-up tick elapses, 2.1 sets clean_on (S5) without testing D on that tick. If D is still true on a later tick, a new power-up starts (S4). C28 confirms this: it is a new power-up after the old one has ended, not a restart under C14. The 2.1 pseudocode and 2.1.1 (S4, S5) already followed this reading, matching FSM T11 and T6. Processes 0, 2, 2.1, 2.1.1, 2.1.6 and 2.1.8 now cite C28.
17. **Dust on the tick a turn or reverse ends (C29). Resolved.** C29 states that dust on that tick is ignored and acted on from the next tick if still present. 2.1 already behaved this way: the turn and reverse branches never test D, and a power-up can start only from the Forward branch on a later tick. After a reverse ends, the next tick is part of a turn, so D stays ignored under C10 until that turn ends. Processes 0, 2, 2.1, 2.1.1, 2.1.6 and 2.1.8 now cite C29, matching FSM T16, T21, T22 and T23 (`dfd_level_4_fsm.md` Issue 6).
