# Requirement Clarifications

Decisions supplied by the project owner. They extend `preliminary_requirements.md`. SA artifacts cite them as C*n*, alongside R*n* for line *n* of the requirements file.

| ID | Clarification |
|---|---|
| C1 | The front sensor is interrupt-driven. |
| C2 | The left, right, and dust sensors are periodic. |
| C3 | Sensor values are Boolean. |
| C4 | If both left and right turns are allowed, the left turn has priority. |
| C5 | Rotation lasts 3 ticks. |
| C6 | Dust detection lasts 3 ticks. Interpreted as: the cleaning power-up in L5 lasts 3 ticks. |
| C7 | The cleaner modes are `clean_off`, `clean_on`, and `clean_up`. |
| C8 | `clean_off` is used only when reversing or rotating. |
| C9  | Reversing lasts at least 3 ticks; it continues while both left and right are blocked, and ends with a turn when a side opens (left first). |
| C10 | Dust detected while turning or reversing is ignored. |
| C11 | An obstacle during power-up cleaning cancels the power-up. |
| C12 | Direction and Clean are sent only when their value changes. |
| C13 | A left or right obstacle with the front clear causes no reaction; the RVC keeps moving forward. (Open Issue 1) |
| C14 | Dust detected during an active power-up does not restart the 3-tick period. (Open Issue 2) |
| C15 | A front interrupt during a turn or reverse is not acted on. When a turn ends, if the front is still blocked, avoidance starts again (new turn decision). (Open Issue 3) |
| C16 | At start-up: Direction = Forward, Clean = clean_on. (Open Issue 4) |
| C17 | A tick is an abstract time unit; left, right and dust are sampled once per tick. The real period is HW detail, out of scope (R6). (Open Issue 5) |
| C18 | The turn decision uses the latest stored left/right values. (Open Issue 6) |
| C19 | Front = false only updates Obstacle Location; it causes no action. (Open Issue 7) |
| C20 | The front interrupt updates Obstacle Location immediately. Control decisions are made on Tick, after sensing in the same tick. (Open Issue 8) |
| C21 | At start-up, all stored sensor values (Front, Left, Right Obstacle and Dust Existence) are false. |
| C22 | At start-up, the RVC sends Direction = Forward and Clean = clean_on once. After that, C12 applies. |
| C23 | Left, Right and Dust start-up values are not relevant because they are overwritten on the first tick (C20); C21 is kept for determinism. |
| C24 | In the Level 3 Controller decomposition, Move Forward and Move Backward are continuous processes activated by Enable / Disable. On Enable, the process produces its Motor Command once; while enabled it produces nothing further; on Disable it stops. Move Backward stays active until the Controller disables it (C9). |
| C25 | Turn Left, Turn Right, Clean Normal, Clean Off and Power-Up Clean are one-shot processes activated by Trigger; each produces one command per Trigger. |
| C26 | The Controller issues Enable / Disable / Trigger only on a state transition, never on every tick. |
| C27 | At start-up, the Controller enables Move Forward and triggers Clean Normal. These produce the start-up commands of C22 (Forward, clean_on); no separate start-up send exists. |
| C28 | When a power-up ends, the RVC returns to normal cleaning on that tick. If dust is still detected on a later tick, a new power-up starts; this is not a restart under C14. |
| C29 | Dust on the tick a turn or reverse ends is ignored; it is acted on from the next tick if still present. |