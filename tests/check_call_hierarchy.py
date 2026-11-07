#!/usr/bin/env python3
"""CH-01: verify that the caller-callee relationships in src/*.c match the
approved Structure Chart (artifacts/SD/structure_chart.md, Sections 3.1-3.3, 5.1).

For each module file, every call to a system function (module API or
environment port) must be in the approved callee set, and every approved
call must be present. Calls to a function defined in the same file are
internal and ignored. Exit code 0 = match, 1 = mismatch.
"""
import re
import sys
from pathlib import Path

# Approved callee sets, from Structure Chart Sections 3.1, 3.2 and 5.1.
APPROVED = {
    "main.c": {"rvc_startup", "rvc_tick", "rvc_port_wait_tick"},
    "rvc.c": {
        "obstacle_location_init", "dust_existence_init",
        "motor_interface_init", "cleaner_interface_init",
        "move_forward_init", "move_backward_init", "controller_init",
        "detect_on_tick", "controller_tick",
    },
    "detection.c": {
        "left_sensor_sample", "right_sensor_sample", "dust_sensor_sample",
        "obstacle_location_set_sides", "dust_existence_set",
    },
    "front_sensor.c": {"obstacle_location_set_front"},
    "left_sensor.c": {"rvc_port_read_left_sensor"},
    "right_sensor.c": {"rvc_port_read_right_sensor"},
    "dust_sensor.c": {"rvc_port_read_dust_sensor"},
    "obstacle_location.c": set(),
    "dust_existence.c": set(),
    "controller.c": {
        "obstacle_location_get", "dust_existence_get", "decide_avoidance",
        "move_forward_enable", "move_forward_disable",
        "move_backward_enable", "move_backward_disable",
        "turn_left_trigger", "turn_right_trigger",
        "clean_normal_trigger", "clean_off_trigger", "power_up_clean_trigger",
    },
    "avoidance.c": set(),
    "move_forward.c": {"motor_interface_send"},
    "move_backward.c": {"motor_interface_send"},
    "turn_left.c": {"motor_interface_send"},
    "turn_right.c": {"motor_interface_send"},
    "clean_normal.c": {"cleaner_interface_send"},
    "clean_off.c": {"cleaner_interface_send"},
    "power_up_clean.c": {"cleaner_interface_send"},
    "motor_interface.c": {"rvc_port_write_direction"},
    "cleaner_interface.c": {"rvc_port_write_clean"},
}

PROTO = re.compile(r"^\s*(?:[A-Za-z_]\w*\s+)+\**\s*([a-z_]\w*)\s*\([^;{]*\)\s*;", re.M)
CALL = re.compile(r"\b([a-z_]\w*)\s*\(")
DEF = re.compile(r"^(?:static\s+)?(?:[A-Za-z_]\w*\s+)+\**\s*([a-z_]\w*)\s*\([^;]*\)\s*\{", re.M)


def strip_comments(text: str) -> str:
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    return re.sub(r"//[^\n]*", " ", text)


def main() -> int:
    src = Path(sys.argv[1] if len(sys.argv) > 1 else "src")
    inc = src.parent / "include"
    system_funcs = set()
    for h in inc.glob("*.h"):
        system_funcs |= set(PROTO.findall(strip_comments(h.read_text())))

    ok = True
    files = {p.name: p for p in src.glob("*.c")}
    for name in sorted(set(files) | set(APPROVED)):
        if name not in files:
            print(f"FAIL {name}: module file missing")
            ok = False
            continue
        if name not in APPROVED:
            print(f"FAIL {name}: module not in the Structure Chart")
            ok = False
            continue
        text = strip_comments(files[name].read_text())
        defined = set(DEF.findall(text))
        calls = {c for c in CALL.findall(text) if c in system_funcs} - defined
        extra = calls - APPROVED[name]
        missing = APPROVED[name] - calls
        if extra or missing:
            ok = False
            print(f"FAIL {name}: unapproved={sorted(extra)} missing={sorted(missing)}")
        else:
            print(f"ok   {name}: {sorted(calls) if calls else '(no calls)'}")
    print("CH-01", "PASS" if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
