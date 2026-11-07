# Test Coverage Report

Date: 2026-11-08. Scope: C production code in `src/`, except `main.c`. Test results: `artifacts/TDD/test_execution_results.md`.

## 1. Tooling

| Item | Value |
|---|---|
| Instrumentation | gcov (`--coverage -O0`, C production code only) |
| Reporting | gcovr 8.8 |
| Reports | `artifacts/TDD/logs/coverage_line.txt`, `artifacts/TDD/logs/coverage_branch.txt`, HTML `artifacts/TDD/coverage/index.html` |

How to reproduce (after building and running the tests, see `test_execution_results.md` Section 1):

```
python -m gcovr -r . --filter src/ --exclude src/main.c --txt-metric branch build
```

## 2. Results

| Metric | Result |
|---|---|
| Lines | **100.0 %** (197 / 197) |
| Functions | **100.0 %** (40 / 40) |
| Branches | **94.9 %** (37 / 39) |

Per-file branch coverage: 100% everywhere except `controller.c` (25 / 27, 92%).

## 3. Uncovered Code

There are two branches, both in `controller.c`. Each is the compiler's implicit "no case matched" exit of a `switch` on an enum:
- line 55: `switch (choice)` on `AvoidChoice`
- line 156: `switch (s_state)` on `ControllerState`

Every enumerator has a case, and both variables only ever hold enum values. `choice` comes from `decide_avoidance()`, and `s_state` is set only by the Controller. So these branches cannot be reached through any valid input. No test was added for them, because a test would have to put an invalid value into hidden state, and the design has no way to do that. No behaviour is untested.

## 4. Exclusions

`main.c` is excluded from coverage. It is compiled but not executed, because its Tick loop has no end (SC Section 9 item 5). Its 3 calls are checked statically by CH-01, and the same entry points are exercised by `test_system`.
