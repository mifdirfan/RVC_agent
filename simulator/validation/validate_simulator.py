"""Simulator validation (skill Step 10). Checks the SIMULATOR, not the RVC.

It verifies that the environment model, the port wiring and the recorded
history are faithful (sensors match the map, grid updates match the motor
output, ticks advance correctly, the tick limit and map validation hold, the
pre-generated traces equal fresh runs of the current C code, traceability is
complete). It never judges whether the RVC behaviour is correct: that is the
human evaluator's job.

Usage: python simulator/validation/validate_simulator.py
Writes artifacts/SystemTest/simulator_validation.md. Exit code 1 if a check fails.
"""
import datetime
import json
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
SIM_DIR = os.path.dirname(HERE)
ROOT = os.path.dirname(SIM_DIR)
sys.path.insert(0, SIM_DIR)

import build_runner  # noqa: E402
from scenario_defs import FUNCTIONS, SCENARIOS  # noqa: E402

REPORT = os.path.join(ROOT, "artifacts", "SystemTest", "simulator_validation.md")
DELTA = {"N": (0, -1), "E": (1, 0), "S": (0, 1), "W": (-1, 0)}
ORDER = ["N", "E", "S", "W"]
LEFT = {d: ORDER[(i + 3) % 4] for i, d in enumerate(ORDER)}
RIGHT = {d: ORDER[(i + 1) % 4] for i, d in enumerate(ORDER)}
BACK = {d: ORDER[(i + 2) % 4] for i, d in enumerate(ORDER)}
VALID_REQ = {f"R{i}" for i in range(1, 8)} | {f"C{i}" for i in range(1, 30)}
TICK_KEYS = {"tick", "phase", "pose", "sensors", "front_interrupt", "controller", "stores", "port_calls",
             "outputs", "transition", "motion", "cleaned", "dust"}
CTRL_KEYS = {"state", "turn_ticks", "reverse_ticks", "power_up_ticks", "move_forward_active", "move_backward_active"}
STATES = {"FwdNormal", "FwdPowerUp", "TurnLeft", "TurnRight", "MoveBackward"}

EXE = build_runner.build()


def run(scenario):
    data = {k: scenario[k] for k in ("start", "obstacles", "dust", "max_ticks")}
    fd, path = tempfile.mkstemp(suffix=".json")
    with os.fdopen(fd, "w") as f:
        json.dump(data, f)
    res = subprocess.run([EXE, path], capture_output=True, text=True)
    os.remove(path)
    return res.returncode, json.loads(res.stdout)


def blocked(obs, x, y):
    return not (0 <= x < 10 and 0 <= y < 10) or (x, y) in obs


def check_trace(sc, tr):
    """Returns a list of problems in the simulator's handling of one trace."""
    errs = []
    obs = {tuple(c) for c in sc["obstacles"]}
    ticks = tr["ticks"]
    if [t["tick"] for t in ticks] != list(range(sc["max_ticks"] + 1)):
        errs.append("V1 ticks are not 0..max_ticks without gaps")
    st = sc["start"]
    if ticks[0]["pose"] != {"x": st["x"], "y": st["y"], "dir": st["dir"]}:
        errs.append("V4 tick 0 pose differs from the configured start")
    if sorted(map(tuple, ticks[0]["dust"])) != sorted(map(tuple, sc["dust"])):
        errs.append("V6 tick 0 dust differs from the configured dust")
    last_front = False
    held_dir = held_clean = None
    turn_prog = 0
    for i, t in enumerate(ticks):
        k = t["tick"]
        missing = TICK_KEYS - t.keys()
        if missing or CTRL_KEYS - t["controller"].keys():
            errs.append(f"V8 tick {k}: missing fields {sorted(missing)}")
            continue
        if t["controller"]["state"] not in STATES:
            errs.append(f"V8 tick {k}: unknown state {t['controller']['state']}")
        pc = t["port_calls"]
        if pc["write_direction"]:
            held_dir = pc["write_direction"][-1]
            turn_prog = 0
        if pc["write_clean"]:
            held_clean = pc["write_clean"][-1]
        if t["outputs"]["motor"] != held_dir or t["outputs"]["cleaner"] != held_clean:
            errs.append(f"V7 tick {k}: displayed held outputs differ from the last port writes")
        if i == 0:
            continue
        prev = ticks[i - 1]
        p = t["sensed_at"]
        if p != prev["pose"]:
            errs.append(f"V4 tick {k}: sensed_at is not the pose after tick {k-1}")
        x, y, d = p["x"], p["y"], p["dir"]
        dust_before = {tuple(c) for c in prev["dust"]}
        exp = {
            "front": blocked(obs, x + DELTA[d][0], y + DELTA[d][1]),
            "left": blocked(obs, x + DELTA[LEFT[d]][0], y + DELTA[LEFT[d]][1]),
            "right": blocked(obs, x + DELTA[RIGHT[d]][0], y + DELTA[RIGHT[d]][1]),
            "dust": (x, y) in dust_before,
        }
        if t["sensors"] != exp:
            errs.append(f"V2 tick {k}: sensors {t['sensors']} do not match the map {exp}")
        # interrupt raised exactly when F changes; stores reflect the inputs
        want_irq = exp["front"] if exp["front"] != last_front else None
        if t["front_interrupt"] != want_irq:
            errs.append(f"V5 tick {k}: front interrupt {t['front_interrupt']} expected {want_irq}")
        last_front = exp["front"]
        if (t["stores"]["front"], t["stores"]["left"], t["stores"]["right"], t["stores"]["dust"]) != \
                (exp["front"], exp["left"], exp["right"], exp["dust"]):
            errs.append(f"V5 tick {k}: stores do not hold this tick's sensor values")
        if (pc["read_left"], pc["read_right"], pc["read_dust"]) != (1, 1, 1):
            errs.append(f"V5 tick {k}: periodic sensors not read exactly once")
        # cleaning effect
        cleaning = held_clean in ("clean_on", "clean_up")
        exp_clean = [x, y] if cleaning and (x, y) in dust_before else None
        if t["cleaned"] != exp_clean:
            errs.append(f"V6 tick {k}: cleaned {t['cleaned']} expected {exp_clean}")
        exp_dust = dust_before - ({(x, y)} if exp_clean else set())
        if {tuple(c) for c in t["dust"]} != exp_dust:
            errs.append(f"V6 tick {k}: dust map not updated correctly")
        # motion effect
        q = t["pose"]
        mo = t["motion"]
        if held_dir in ("Forward", "Backward"):
            md = d if held_dir == "Forward" else BACK[d]
            nx, ny = x + DELTA[md][0], y + DELTA[md][1]
            coll = blocked(obs, nx, ny)
            want = {"x": x, "y": y, "dir": d} if coll else {"x": nx, "y": ny, "dir": d}
            if q != want or mo["collision"] != coll:
                errs.append(f"V3 tick {k}: {held_dir} from {p} gave {q} (collision={mo['collision']})")
        elif held_dir in ("Turn Left", "Turn Right"):
            turn_prog += 1
            rot = turn_prog == 3
            if rot:
                turn_prog = 0
            nd = (LEFT if held_dir == "Turn Left" else RIGHT)[d] if rot else d
            if q != {"x": x, "y": y, "dir": nd} or mo["rotated"] != rot or mo["collision"]:
                errs.append(f"V3 tick {k}: {held_dir} from {p} gave {q} (rotated={mo['rotated']})")
    return errs


def main():
    results = []  # (id, title, ok, detail)

    def rec(cid, title, problems, detail=""):
        results.append((cid, title, not problems, "; ".join(problems[:5]) or detail))

    # V1-V8 on every scenario; V12 determinism and pre-generated == fresh
    trace_problems, stale = [], []
    for s in SCENARIOS:
        rc, tr = run(s)
        if rc != 0:
            trace_problems.append(f"{s['id']}: runner rc={rc}")
            continue
        trace_problems += [f"{s['id']}: {e}" for e in check_trace(s, tr)]
        rc2, tr2 = run(s)
        if tr2 != tr:
            stale.append(f"{s['id']}: two runs differ")
        pre = os.path.join(SIM_DIR, "traces", s["id"] + ".json")
        with open(pre, encoding="utf-8") as f:
            if json.load(f) != tr:
                stale.append(f"{s['id']}: pre-generated trace differs from a fresh run (run generate.py)")
    n_ticks = sum(s["max_ticks"] for s in SCENARIOS)
    rec("V1-V8", f"Trace fidelity on all {len(SCENARIOS)} scenarios ({n_ticks} ticks): tick advancement, sensors vs map, "
        "grid update vs motor output, pose continuity, interrupt/store wiring, dust removal, held outputs, complete fields",
        trace_problems, "all ticks consistent")
    rec("V12", "Determinism; pre-generated traces equal fresh runs of the current src/", stale, "identical")

    # V9 tick limit
    base = {"start": {"x": 4, "y": 4, "dir": "N"}, "obstacles": [], "dust": []}
    p = []
    rc, tr = run(dict(base, max_ticks=100))
    if rc != 0 or len(tr["ticks"]) != 101:
        p.append("100 ticks not accepted / not 101 snapshots")
    for bad in (0, 101, 1000):
        rc, out = run(dict(base, max_ticks=bad))
        if rc != 2 or "error" not in out:
            p.append(f"max_ticks={bad} not rejected")
    rec("V9", "100-tick limit: 100 accepted (ticks 0..100 recorded); 0, 101, 1000 rejected", p, "enforced by sim_runner")

    # V10 map validation and custom start positions
    p = []
    invalid = [
        dict(base, start={"x": 4, "y": 4, "dir": "N"}, obstacles=[[4, 4]], max_ticks=10),
        dict(base, start={"x": -1, "y": 4, "dir": "N"}, max_ticks=10),
        dict(base, start={"x": 4, "y": 10, "dir": "N"}, max_ticks=10),
        dict(base, start={"x": 4, "y": 4, "dir": "X"}, max_ticks=10),
        dict(base, obstacles=[[2, 2]], dust=[[2, 2]], max_ticks=10),
        dict(base, obstacles=[[2, 2], [2, 2]], max_ticks=10),
        dict(base, dust=[[10, 0]], max_ticks=10),
    ]
    for sc in invalid:
        rc, out = run(sc)
        if rc != 2:
            p.append(f"not rejected: {sc}")
    for (x, y, d) in [(0, 0, "N"), (9, 9, "W"), (0, 9, "E"), (7, 2, "S")]:
        rc, tr = run(dict(base, start={"x": x, "y": y, "dir": d}, max_ticks=5))
        if rc != 0 or tr["ticks"][0]["pose"] != {"x": x, "y": y, "dir": d}:
            p.append(f"custom start ({x},{y},{d}) not honoured")
    rec("V10", "Map validation (7 invalid maps rejected, never altered) and custom start positions/headings honoured", p, "ok")

    # V11 traceability
    p = []
    ids = [s["id"] for s in SCENARIOS]
    if len(ids) < 20 or len(set(ids)) != len(ids):
        p.append("fewer than 20 distinct scenarios")
    for s in SCENARIOS:
        if len(s["functions"]) < 3:
            p.append(f"{s['id']}: fewer than 3 functions")
        if not s["requirements"] or set(s["requirements"]) - VALID_REQ:
            p.append(f"{s['id']}: invalid requirement ids {set(s['requirements']) - VALID_REQ}")
        if any(f not in FUNCTIONS for f in s["functions"]):
            p.append(f"{s['id']}: unknown function id")
        if not 1 <= s["max_ticks"] <= 100:
            p.append(f"{s['id']}: max_ticks out of range")
        if not s["expected"]:
            p.append(f"{s['id']}: no expected behaviour")
    rec("V11", f"Traceability: {len(ids)} distinct scenarios, each with >= 3 essential functions, valid R/C ids, expected behaviour, <= 100 ticks", p, "complete")

    ok = all(r[2] for r in results)
    L = ["# Simulator Validation", "",
         "Generated by `simulator/validation/validate_simulator.py` (skill Step 10). These checks validate the **simulator** "
         "(environment model, port wiring, history recording, limits, validation, traceability). They do not judge the RVC "
         "behaviour: verdicts on the scenarios are given only by human evaluators (`test_results.md`).", "",
         f"Run: {datetime.datetime.now().isoformat(timespec='seconds')} · runner: `{os.path.relpath(EXE, ROOT)}` built with GCC "
         "`-std=c11 -Wall -Wextra -Wpedantic -Werror` from the unmodified `src/` files.", "",
         "| Check | What is verified | Result | Detail |", "|---|---|---|---|"]
    for cid, title, good, detail in results:
        L.append(f"| {cid} | {title} | {'OK' if good else 'PROBLEM'} | {detail.replace('|', '/')} |")
    L += ["", "UI checks (controls, editing, history navigation, evaluation recording) are run by "
          "`simulator/validation/ui_check.mjs`; see Section \"UI check\" below when it has been run.", ""]
    ui = os.path.join(HERE, "ui_check_result.md")
    if os.path.exists(ui):
        with open(ui, encoding="utf-8") as f:
            L += ["## UI check", "", f.read()]
    with open(REPORT, "w", encoding="utf-8") as f:
        f.write("\n".join(L) + "\n")
    for cid, title, good, detail in results:
        print(f"{cid:6} {'OK     ' if good else 'PROBLEM'} {detail}")
    print("report:", os.path.relpath(REPORT, ROOT))
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
