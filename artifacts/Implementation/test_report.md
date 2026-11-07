# Unit Test Report

Date: 2026-11-08. Test plan: `tests/TEST_PLAN.md`. Raw logs: `artifacts/Implementation/logs/`.

## 1. Environment

| Item | Value |
|---|---|
| Compiler (C11 production code) | GCC 11.2.0, x86_64-w64-mingw32, posix threads |
| Compiler (tests) | G++ 11.2.0, C++14 |
| Build | CMake 4.4.4, generator "MinGW Makefiles" |
| Test framework | GoogleTest v1.14.0 (FetchContent) |
| Test runner | CTest |
| Coverage | gcov (`--coverage -O0`, C production code only) + gcovr 8.8 |
| Warnings | `-Wall -Wextra -Wpedantic -Werror` on all C code (clean) |

How to reproduce:

```
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
cmake --build build -j 8
ctest --test-dir build --output-on-failure
python -m gcovr -r . --filter src/ --exclude src/main.c --txt-metric branch build
```

## 2. Test Execution Result (final run, `logs/ctest_final.log`)

**84 / 84 passed, 0 failed.**

| Executable / suite | Tests | Passed | Covers |
|---|---|---|---|
| `test_afferent` (Afferent) | 11 | 11 | INIT-01, INIT-02, AFF-01 – AFF-09 |
| `test_interfaces` (Interfaces) | 9 | 9 | INIT-03, INIT-04, C12-01 – C12-07 |
| `test_actions` (Actions) | 10 | 10 | INIT-05, INIT-06, ACT-01 – ACT-09 |
| `test_avoidance` (Avoidance) | 4 | 4 | AV-01 – AV-04 |
| `test_controller` (Controller) | 38 | 38 | INIT-07, T1 – T25, CL-C10/C11/C13/C14/C28/C29, CNT-01 – CNT-05 |
| `test_system` (System) | 11 | 11 | INIT-08, SYS-01 – SYS-10 |
| `check_call_hierarchy.py` (CallHierarchy) | 1 | 1 | CH-01 |
| **Total** | **84** | **84** | |

### FSM transition coverage (test_controller)

All 25 transitions have a dedicated test, `Controller.T<n>_*`. Each one asserts the exact ordered list of Enable / Disable / Trigger calls, and then checks the next state through what follows. For example, after entering a turn, the test expects 2 silent ticks, and then the 3rd tick exits.

| T1 | T2 | T3 | T4 | T5 | T6 | T7 | T8 | T9 | T10 | T11 | T12 | T13 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ |

| T14 | T15 | T16 | T17 | T18 | T19 | T20 | T21 | T22 | T23 | T24 | T25 |
|---|---|---|---|---|---|---|---|---|---|---|---|
| ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ | ✔ |

## 3. TDD Cycle Log (Red → Green → Refactor)

The test files for all groups were written first, from `TEST_PLAN.md`, against stub modules that compile but do nothing. So the RED step for every group comes from the baseline run, and each group was then implemented and turned GREEN one at a time.

| Cycle | Group | RED (tests failing for the expected reason) | GREEN | REFACTOR |
|---|---|---|---|---|
| 0 | Baseline (all stubs) | 84 tests: 81 failed, 3 passed (`logs/00_red_baseline.log`). The 3 passes are "nothing happens" or default-value cases that the stubs satisfy: INIT05_06, AV04 (stub returns BACKWARD), SYS09 | — | — |
| 1 | G1 Afferent (sensors, stores, Detect) | 11 / 11 failed: wrong store values, 0 port reads | 11 / 11 pass | none needed |
| 2 | G2 Motor / Cleaner Interface | 9 / 9 failed: no port writes | 9 / 9 pass | none needed |
| 3 | G3 Action modules | 9 / 10 failed: no interface calls | 10 / 10 pass | none needed |
| 4 | G4 Decide Avoidance | 3 / 4 failed | 4 / 4 pass | none needed |
| 5 | G5 Controller FSM | 38 / 38 failed: no signals | 38 / 38 pass | Pointer parameters of the private helpers replaced by value parameters (C rule "check pointers"); `count_tick()` returns the new value. Re-run: 38 / 38 pass |
| 6 | G6 RVC System, Main, CH-01 | 10 / 11 System failed (no writes); CH-01 failed (calls missing) | 11 / 11 + CH-01 pass. Full suite 84 / 84 | none needed |

## 4. Coverage (C production code, `src/` except `main.c`)

Reports: `logs/coverage_line.txt`, `logs/coverage_branch.txt`, HTML `coverage/index.html`.

| Metric | Result |
|---|---|
| Lines | **100.0 %** (197 / 197) |
| Functions | **100.0 %** (40 / 40) |
| Branches | **94.9 %** (37 / 39) |

Per-file branch coverage: 100% everywhere except `controller.c` (25 / 27, 92%).

**Uncovered code.** There are two branches, both in `controller.c`. Each is the compiler's implicit "no case matched" exit of a `switch` on an enum:
- line 55: `switch (choice)` on `AvoidChoice`
- line 156: `switch (s_state)` on `ControllerState`

Every enumerator has a case, and both variables only ever hold enum values. `choice` comes from `decide_avoidance()`, and `s_state` is set only by the Controller. So these branches cannot be reached through any valid input. No test was added for them, because a test would have to put an invalid value into hidden state, and the design has no way to do that. No behaviour is untested.

`main.c` is excluded from coverage. It is compiled but not executed, because its Tick loop has no end (SC Section 9 item 5). Its 3 calls are checked statically by CH-01, and the same entry points are exercised by `test_system`.

## 5. Requirement Verification Evidence

| Req. | Verified by (all passing) |
|---|---|
| R1, R2 | SYS01, INIT08, T5, ACT07 |
| R3 | T2, T3, T7, T8, T16, T21, SYS02 |
| R4 | AV04, T4, T9, T15, T20, T22, T23, SYS05 |
| R5 | T6, T11, SYS07 |
| R6, R7 | No HW detail implemented; all I/O goes through `rvc_port.h` |
| C1, C20 | AFF04, SYS03 |
| C2, C17, C18 | AFF09, SYS04, T5 (stores read on every Tick) |
| C3 | AFF01 – AFF03 |
| C4 | AV01 – AV03, T2, T19, T22, SYS10 |
| C5 | T12, T13, CNT01, CNT05 |
| C6 | T10, T11, CNT02, SYS07 |
| C7, C8 | ACT07 – ACT09, T2 – T4, SYS02 |
| C9 | T22 – T25, CNT03, CNT04 (300-tick reverse, counter saturation), SYS05 |
| C10 | T12, T17, T24, CL_C10 |
| C11 | T7 – T9, CL_C11, SYS08 |
| C12 | C12-01 – C12-07, SYS01, SYS06 (T13 re-entry writes no duplicate Turn Left) |
| C13 | T5, T10, CL_C13 |
| C14 | T10, CL_C14 |
| C15 | T12 – T15, T17 – T20, SYS06 |
| C16, C22, C27 | T1, INIT07, INIT08, SYS01 |
| C19 | AFF05, SYS09 |
| C21, C23 | INIT01, INIT02, INIT08 |
| C24, C25 | ACT01 – ACT09 |
| C26 | T5, T10, T12, T17, T24, T25 |
| C28 | T11, CL_C28 |
| C29 | T16, T21, CL_C29 |

## 6. Differences from the Structure Chart

None. See `module_traceability.md` Section 4. CH-01 verifies the caller–callee relationships automatically.

## 7. Status

- Unit tests: **all passing (84/84)**.
- Coverage: reported above.
- Controller behaviour: matches the FSM. Each of T1–T25 is tested.
- Next phase (system testing with the web simulator) waits for human approval of this implementation.
