"""Builds simulator/runner/sim_runner(.exe) with GCC (the MinGW gcc used for TDD).

Links the approved src/ modules except main.c. controller.c, move_forward.c and
move_backward.c are compiled through runner/debug/dbg_*.c (which #include the
unmodified files) so their hidden state can be read by rvc_debug_get().

Usage: python simulator/build_runner.py
Python standard library only.
"""
import os
import shutil
import subprocess
import sys

SIM_DIR = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(SIM_DIR)
SRC = os.path.join(ROOT, "src")
INC = os.path.join(ROOT, "include")
RUNNER = os.path.join(SIM_DIR, "runner")
DEBUG = os.path.join(RUNNER, "debug")
EXE = os.path.join(RUNNER, "sim_runner.exe" if os.name == "nt" else "sim_runner")

# src/ modules linked directly (main.c excluded: its tick loop never ends).
SRC_MODULES = [
    "rvc", "detection", "avoidance",
    "left_sensor", "right_sensor", "dust_sensor", "front_sensor",
    "obstacle_location", "dust_existence",
    "turn_left", "turn_right",
    "clean_normal", "clean_off", "power_up_clean",
    "motor_interface", "cleaner_interface",
]
# src/ modules compiled through a read-only debug wrapper instead.
WRAPPED = {"controller": "dbg_controller", "move_forward": "dbg_move_forward",
           "move_backward": "dbg_move_backward"}

CFLAGS = ["-std=c11", "-Wall", "-Wextra", "-Wpedantic", "-Werror", "-O2"]


def sources():
    files = [os.path.join(SRC, m + ".c") for m in SRC_MODULES]
    files += [os.path.join(DEBUG, w + ".c") for w in WRAPPED.values()]
    files += [os.path.join(DEBUG, "rvc_debug.c")]
    files += [os.path.join(RUNNER, f) for f in ("sim_world.c", "sim_ports.c", "sim_scenario.c", "sim_runner.c")]
    return files


def all_inputs():
    """Every file the binary depends on (for the up-to-date check)."""
    deps = sources() + [os.path.join(SRC, m + ".c") for m in WRAPPED]
    for d in (INC, RUNNER, DEBUG):
        deps += [os.path.join(d, f) for f in os.listdir(d) if f.endswith(".h")]
    return deps


def up_to_date():
    if not os.path.exists(EXE):
        return False
    t = os.path.getmtime(EXE)
    return all(os.path.getmtime(f) <= t for f in all_inputs())


def find_gcc():
    for cand in (r"C:\MinGW\bin\gcc.exe", shutil.which("gcc")):
        if cand and os.path.exists(cand):
            return cand
    raise SystemExit("gcc not found (expected C:\\MinGW\\bin\\gcc.exe or gcc on PATH)")


def build(force=False):
    if not force and up_to_date():
        return EXE
    gcc = find_gcc()
    cmd = [gcc] + CFLAGS + ["-I", INC, "-I", SRC, "-I", RUNNER, "-I", DEBUG] + sources() + ["-o", EXE]
    print("[build]", " ".join(os.path.relpath(c, ROOT) if os.path.isabs(c) and c.startswith(ROOT) else c for c in cmd))
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode != 0:
        sys.stderr.write(res.stdout + res.stderr)
        raise SystemExit("sim_runner build failed")
    return EXE


if __name__ == "__main__":
    print(build(force="--force" in sys.argv))
