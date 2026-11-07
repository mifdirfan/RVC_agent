"""Generates, from scenario_defs.py:
  simulator/scenarios/ST-xx.json            scenario files (map + metadata)
  simulator/traces/ST-xx.json               pre-generated traces (real C controller)
  artifacts/SystemTest/test_scenarios.md
  artifacts/SystemTest/requirement_traceability.md
  artifacts/SystemTest/test_results.md      (via results_report.py; keeps recorded verdicts)

Usage: python simulator/generate.py
Standard library only. Assigns no PASS/FAIL.
"""
import json
import os
import subprocess
import sys

SIM_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, SIM_DIR)

import build_runner  # noqa: E402
import results_report  # noqa: E402
from scenario_defs import FUNCTIONS, PS, SCENARIOS, UNREACHABLE_ON_STATIC_MAP  # noqa: E402

ROOT = os.path.dirname(SIM_DIR)
SCEN_DIR = os.path.join(SIM_DIR, "scenarios")
TRACE_DIR = os.path.join(SIM_DIR, "traces")
OUT_DIR = os.path.join(ROOT, "artifacts", "SystemTest")

MAP_KEYS = ("start", "obstacles", "dust", "max_ticks")


def map_only(s):
    return {k: s[k] for k in MAP_KEYS}


def run_trace(exe, scenario, out_path):
    tmp = out_path + ".in.json"
    with open(tmp, "w", encoding="utf-8") as f:
        json.dump(map_only(scenario), f)
    res = subprocess.run([exe, tmp, out_path], capture_output=True, text=True)
    os.remove(tmp)
    if res.returncode != 0:
        raise SystemExit(f"{scenario['id']}: runner failed: {res.stdout}{res.stderr}")
    with open(out_path, encoding="utf-8") as f:
        return json.load(f)


def ascii_map(s):
    obs = {tuple(c) for c in s["obstacles"]}
    dust = {tuple(c) for c in s["dust"]}
    arrow = {"N": "^", "E": ">", "S": "v", "W": "<"}[s["start"]["dir"]]
    lines = ["    x 0 1 2 3 4 5 6 7 8 9"]
    for y in range(10):
        row = []
        for x in range(10):
            if (x, y) == (s["start"]["x"], s["start"]["y"]):
                row.append(arrow if (x, y) not in dust else arrow)
            elif (x, y) in obs:
                row.append("#")
            elif (x, y) in dust:
                row.append("*")
            else:
                row.append(".")
        lines.append(f"  y{y} " + " ".join(row))
    return "\n".join(lines)


def cells(lst):
    return ", ".join(f"({x},{y})" for x, y in lst) if lst else "none"


def observed_rows(trace):
    return sorted({t["transition"]["fsm"] for t in trace["ticks"]}, key=lambda r: int(r[1:]))


def write_test_scenarios(traces):
    L = []
    a = L.append
    a("# RVC System Test Scenarios")
    a("")
    a("Generated from `simulator/scenario_defs.py` by `simulator/generate.py`. Sources: `preliminary_requirements.md` (R1-R7), "
      "`clarifications.md` (C1-C29), `artifacts/SA/DFD/dfd_level_4_fsm.md` (T1-T25), `artifacts/SA/event_list.md`, "
      "`artifacts/SA/process_specification.md`.")
    a("")
    a("## 1. Test Design Policy")
    a("")
    a(f"- {len(SCENARIOS)} scenarios, each on the 10x10 grid, at most 100 ticks, with an explicit start pose, obstacles and dust.")
    a("- Each scenario evaluates at least 3 essential system functions (Section 3).")
    a("- Expected behaviour is written from the requirements, clarifications and the FSM, not from simulator output.")
    a("- The test oracle is the human evaluator. No PASS/FAIL is assigned automatically; every result starts as *Pending*.")
    a("- The full state is recorded at every tick (`simulator/traces/ST-xx.json`, and live runs in the simulator).")
    a("")
    a("### Conventions")
    a("")
    a("- Coordinates: x = column 0..9 (left to right), y = row 0..9 (top to bottom). Default start (4,4).")
    a("- Heading N = up (towards y-1). The requirements give no compass direction; the default start heading N is a simulator convention. "
      "Left of N is W, right of N is E.")
    a("- Walls: cells outside the grid count as obstacles for the sensors and the motion model.")
    a("- Map legend: `^ > v <` RVC start and heading, `#` obstacle, `*` dust, `.` empty. A start cell with dust is listed under Dust.")
    a("- Tick model: tick k senses at the pose reached after tick k-1 (F/L/R are the neighbour cells relative to the heading, dust is the current cell). "
      "The front interrupt is raised when F changes. Then `rvc_tick()` runs, cleaning removes the dust of the current cell if the cleaner is clean_on/clean_up, "
      "and the motor applies the held Direction: Forward/Backward move 1 cell; a turn changes the heading by 90 degrees at the end of its 3rd tick. "
      "Tick 0 is the state after `rvc_startup()`.")
    a("")
    a("## 2. Scenarios")
    a("")
    for s in SCENARIOS:
        st = s["start"]
        a(f"### {s['id']} - {s['name']}")
        a("")
        a("#### Initial Configuration")
        a(f"- Grid: 10x10")
        default = " (default centre)" if (st["x"], st["y"]) == (4, 4) else " (custom start)"
        a(f"- RVC Start: ({st['x']},{st['y']}){default}")
        a(f"- Initial Direction: {st['dir']}")
        a(f"- Obstacles: {cells(s['obstacles'])}")
        a(f"- Dust: {cells(s['dust'])}")
        a(f"- Maximum Ticks: {s['max_ticks']}")
        a("")
        a("```text")
        a(ascii_map(s))
        a("```")
        a("")
        a("#### Test Objectives")
        for i, f in enumerate(s["functions"], 1):
            a(f"- Essential Function {i}: {f} - {FUNCTIONS[f][0]}")
        a("")
        a("#### Requirement Traceability")
        a(f"- Related Requirement IDs: {', '.join(s['requirements'])}")
        a(f"- Related SA Artifacts: FSM rows {', '.join(s['fsm'])} (`dfd_level_4_fsm.md`); Event List: {', '.join(s['events'])}; "
          f"Process Specifications: {', '.join(p + ' ' + PS[p] for p in s['ps'])}")
        a("")
        a("#### Expected Behavior")
        for e in s["expected"]:
            a(f"- {e}")
        a("")
        a("#### Human Evaluation")
        a("- Result: Pending (PASS / FAIL / INCONCLUSIVE)")
        a("- Evaluator:")
        a("- Observations:")
        kt = ", ".join(str(t) for t in s["key_ticks"]) if s["key_ticks"] else "all avoidance and dust events"
        a(f"- Related Tick(s): suggested {kt}")
        a("")
    a("## 3. Essential System Functions")
    a("")
    a("| ID | Function | Requirements | Scenarios |")
    a("|---|---|---|---|")
    for fid, (desc, reqs) in FUNCTIONS.items():
        used = [s["id"] for s in SCENARIOS if fid in s["functions"]]
        a(f"| {fid} | {desc} | {', '.join(reqs)} | {', '.join(used)} |")
    a("")
    a("## 4. Test Execution and Evaluation")
    a("")
    a("1. `python simulator/server.py`, open http://localhost:8000.")
    a("2. Select a scenario. Its pre-generated trace loads; **Run C controller** re-executes the real controller from reset.")
    a("3. Use Start / Pause / Step / Step back / Reset / Restart and the history table to inspect every tick.")
    a("4. Assign PASS / FAIL / INCONCLUSIVE, enter evaluator, observations and related ticks, and press **Save evaluation**. "
      "The verdict and the trace are stored in `simulator/results/` and `artifacts/SystemTest/test_results.md` is regenerated.")
    a("")
    a("## 5. Requirement Traceability")
    a("")
    a("| Scenario ID | Essential Functions | Requirement IDs | SA Artifacts | Human Result |")
    a("|---|---|---|---|---|")
    for s in SCENARIOS:
        a(f"| {s['id']} | {', '.join(s['functions'])} | {', '.join(s['requirements'])} | FSM {', '.join(s['fsm'])}; PS {', '.join(s['ps'])} | Pending |")
    a("")
    a("Full matrices: `requirement_traceability.md`.")
    a("")
    a("## 6. Custom Map Testing")
    a("")
    a("In the simulator, **Edit map** lets the user place or remove obstacles and dust, set the start cell and heading, clear the map or reset it to the selected scenario. "
      "The map is validated (start inside the grid and not on an obstacle, no cell both obstacle and dust, 1-100 ticks) before the C controller is run from reset. "
      "Every tick is recorded and inspectable, a human verdict can be recorded, and **Save as scenario** stores the map in `simulator/scenarios/custom/` "
      "so it appears in the scenario list.")
    a("")
    with open(os.path.join(OUT_DIR, "test_scenarios.md"), "w", encoding="utf-8") as f:
        f.write("\n".join(L))


def write_traceability(traces):
    L = []
    a = L.append
    a("# Requirement Traceability (System Test)")
    a("")
    a("Generated by `simulator/generate.py`. Links requirements (R1-R7 = lines of `preliminary_requirements.md`, C1-C29 = `clarifications.md`) "
      "to essential functions, SA artifacts and system test scenarios.")
    a("")
    a("## 1. Requirement -> Scenarios")
    a("")
    req_ids = [f"R{i}" for i in range(1, 8)] + [f"C{i}" for i in range(1, 30)]
    a("| Requirement | Scenarios | Note |")
    a("|---|---|---|")
    notes = {
        "R6": "Out of scope by definition (HW details). Sensors, clock and actuators are abstract in the simulator.",
        "R7": "Scope statement: only the automatic cleaning function is simulated.",
        "C24": "Observable as `move_forward_active` / `move_backward_active` (debug view).",
        "C25": "Observable as one Direction/Clean write per Trigger, filtered by C12.",
        "C26": "Observable: port writes occur only on ticks with a state transition.",
    }
    for r in req_ids:
        used = [s["id"] for s in SCENARIOS if r in s["requirements"]]
        note = notes.get(r, "")
        if not used and not note:
            note = "Not covered by a system scenario."
        a(f"| {r} | {', '.join(used) if used else '-'} | {note} |")
    a("")
    a("## 2. Essential Function -> Requirements -> Scenarios")
    a("")
    a("| Function | Description | Requirements | Scenarios |")
    a("|---|---|---|---|")
    for fid, (desc, reqs) in FUNCTIONS.items():
        used = [s["id"] for s in SCENARIOS if fid in s["functions"]]
        a(f"| {fid} | {desc} | {', '.join(reqs)} | {', '.join(used)} |")
    a("")
    a("## 3. FSM Transition Coverage (`dfd_level_4_fsm.md`)")
    a("")
    a("*Targeted* = rows the scenario was designed to exercise. *Reached in trace* = rows whose state change appears in the pre-generated trace "
      "(an annotation of observed state changes; it is coverage information, not a verdict).")
    a("")
    a("| Row | Targeted by | Reached in pre-generated trace |")
    a("|---|---|---|")
    for n in range(1, 26):
        r = f"T{n}"
        tgt = [s["id"] for s in SCENARIOS if r in s["fsm"]]
        seen = [s["id"] for s in SCENARIOS if r in observed_rows(traces[s["id"]])]
        if r in UNREACHABLE_ON_STATIC_MAP:
            seen_txt = "not reachable on a static map (see Section 5); unit tests `Controller.%s_*`" % r
        else:
            seen_txt = ", ".join(seen) if seen else "-"
        a(f"| {r} | {', '.join(tgt) if tgt else '-'} | {seen_txt} |")
    a("")
    a("## 4. Scenario -> SA Artifacts")
    a("")
    a("| Scenario | FSM rows | Event List events | Process Specifications |")
    a("|---|---|---|---|")
    for s in SCENARIOS:
        a(f"| {s['id']} | {', '.join(s['fsm'])} | {', '.join(s['events'])} | {', '.join(p + ' ' + PS[p] for p in s['ps'])} |")
    a("")
    a("## 5. Gaps")
    a("")
    a("- **T13, T14, T15, T18, T19, T20 (C15, turn ends with the front still blocked).** With a static map and adjacent-cell sensors, the front cell after a "
      "turn is the side cell that was sensed free when the turn was chosen (C18), so the front is always clear when a turn ends. These rows need a changing "
      "environment. They are verified by the unit tests `Controller.T13_*` ... `Controller.T20_*` (`artifacts/Implementation/test_report.md`).")
    a("- **R6, R7** are scope statements; no scenario tests them directly.")
    a("")
    with open(os.path.join(OUT_DIR, "requirement_traceability.md"), "w", encoding="utf-8") as f:
        f.write("\n".join(L))


def main():
    os.makedirs(SCEN_DIR, exist_ok=True)
    os.makedirs(TRACE_DIR, exist_ok=True)
    os.makedirs(OUT_DIR, exist_ok=True)
    exe = build_runner.build()
    traces = {}
    for s in SCENARIOS:
        assert len(s["functions"]) >= 3, s["id"]
        assert 1 <= s["max_ticks"] <= 100, s["id"]
        out = dict(s, function_text=[f"{f} – {FUNCTIONS[f][0]}" for f in s["functions"]])
        with open(os.path.join(SCEN_DIR, s["id"] + ".json"), "w", encoding="utf-8") as f:
            json.dump(out, f, indent=2, ensure_ascii=False)
        traces[s["id"]] = run_trace(exe, s, os.path.join(TRACE_DIR, s["id"] + ".json"))
        rows = observed_rows(traces[s["id"]])
        missing = [r for r in s["fsm"] if r not in rows]
        print(f"{s['id']}: {s['max_ticks']:3} ticks, rows reached {' '.join(rows)}"
              + (f"  [targeted but not reached: {' '.join(missing)}]" if missing else ""))
    write_test_scenarios(traces)
    write_traceability(traces)
    results_report.write_report()
    print("artifacts written to", os.path.relpath(OUT_DIR, ROOT))


if __name__ == "__main__":
    main()
