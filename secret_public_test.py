"""Public self-checks for the optional secret phase, using the local personal bomb."""

import json
from pathlib import Path

from runner import Result, answer_line, run_limited


def phase_transcript(result):
    if result.timed_out:
        return None, "timeout"
    if result.returncode:
        return None, "solver_error"
    if result.truncated:
        return None, "output_limit"
    lines = result.stdout.splitlines(keepends=True)
    if len(lines) != 6:
        return None, "format_error"
    for line in lines:
        _, error = answer_line(Result(0, line, b""))
        if error:
            return None, error
    return result.stdout, None


def check_seed(root, seed):
    flags = ["--seed", str(seed)]
    solver = str(root / "defuser")
    bomb = str(root / "bomb")
    result = run_limited([solver, *flags], cwd=root)
    prefix, error = phase_transcript(result)
    if error:
        return "prerequisite_" + error
    checked = run_limited([bomb, *flags], cwd=root, data=prefix)
    if checked.timed_out:
        return "prerequisite_timeout"
    if checked.truncated:
        return "prerequisite_output_limit"
    if checked.returncode:
        return "prerequisite_wrong_answer"
    result = run_limited([solver, "--secret", *flags], cwd=root)
    answer, error = answer_line(result)
    if error:
        return error
    checked = run_limited([bomb, *flags], cwd=root, data=prefix + answer)
    if checked.timed_out:
        return "timeout"
    if checked.truncated:
        return "output_limit"
    if checked.returncode:
        return "wrong_answer"
    if b"Secret phase defused." not in checked.stdout.splitlines():
        return "secret_not_reached"
    return None


def main():
    root = Path(__file__).resolve().parent
    for name in ("bomb", "defuser"):
        if not (root / name).is_file():
            print(f"Missing {name}: download your personal bomb and run make first.")
            return 1
    seeds = json.loads((root / "tests/public.json").read_text())["seeds"]
    passed = 0
    for seed in seeds:
        error = check_seed(root, seed)
        if error:
            print(f"Secret seed {seed}: FAIL ({error})")
        else:
            passed += 1
            print(f"Secret seed {seed}: PASS")
    print(f"Secret public tests: {passed}/{len(seeds)} (optional)")
    return 0 if passed == len(seeds) else 1


if __name__ == "__main__":
    raise SystemExit(main())
