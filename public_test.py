import argparse
import json
from pathlib import Path

from runner import answer_line, run_limited


def main():
    parser = argparse.ArgumentParser(description="Run reproducible public checks")
    parser.add_argument("--phase", type=int, choices=range(7))
    args = parser.parse_args()
    root = Path(__file__).resolve().parent
    cases = json.loads((root / "tests/public.json").read_text())
    phases = [args.phase] if args.phase is not None else cases["phases"]
    if args.phase == 0:
        root = root / "tutor"
    total = passed = 0
    for phase in phases:
        for seed in cases["seeds"]:
            flags = ["--seed", str(seed), "--phase", str(phase)]
            result = run_limited([str(root / "defuser"), *flags], cwd=root)
            answer, error = answer_line(result)
            if answer is not None:
                checked = run_limited([str(root / "bomb"), *flags], cwd=root, data=answer)
                if checked.returncode or checked.timed_out:
                    error = "wrong_answer"
            ok = error is None
            total += 1
            passed += ok
            print(f"Phase {phase} seed {seed}: {'PASS' if ok else 'FAIL (' + error + ')'}")
    print(f"Public tests: {passed}/{total}")
    return 0 if passed == total else 1


if __name__ == "__main__":
    raise SystemExit(main())
