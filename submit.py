#!/usr/bin/env python3
"""
Usage:
    python submit.py <task_name> [options]

Examples:
    python submit.py multiplication
    python submit.py multiplication --no-send
"""

import argparse
import difflib
import os
import shutil
import subprocess
import sys
from datetime import datetime
from pathlib import Path

# Logging helpers
def info(msg: str):
    print(f"[+] {msg}")

def warn(msg: str):
    print(f"[!] {msg}")

def step(msg: str):
    print(f"{'─' * 4} {msg} {'─' * 4}")

def fail(msg: str):
    print(f"[X] {msg}", file=sys.stderr)


# Subprocess helpers
def run_silent(cmd: list, *, cwd: Path = None, check: bool = True) -> subprocess.CompletedProcess:
    return subprocess.run(
        cmd, capture_output=True, text=True, check=check,
        cwd=str(cwd) if cwd else None,
    )


def run_logged(cmd: list, *, cwd: Path = None) -> bool:
    print(" ".join(str(c) for c in cmd))
    proc = subprocess.Popen(
        cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        text=True, cwd=str(cwd) if cwd else None,
    )
    for line in proc.stdout:
        sys.stdout.write(line)
    proc.wait()
    return proc.returncode == 0


# Git helpers
def git_out(*args, cwd: Path = None) -> str:
    return run_silent(["git"] + list(args), cwd=cwd).stdout.strip()


def git_silent(*args, cwd: Path = None) -> bool:
    return run_silent(["git"] + list(args), cwd=cwd).returncode == 0


def git_logged(*args, cwd: Path = None) -> bool:
    return run_logged(["git"] + list(args), cwd=cwd)


def git_repo_root() -> Path:
    try:
        return Path(git_out("rev-parse", "--show-toplevel"))
    except subprocess.CalledProcessError:
        fail("Not inside a git repository.")
        sys.exit(1)


def git_remote_branch_exists(branch: str, root: Path) -> bool:
    out = git_out("ls-remote", "--heads", "origin", branch, cwd=root)
    return bool(out.strip())


def git_full_diff(root: Path) -> list:
    committed = git_out("diff", "--name-only", "main...HEAD", cwd=root)
    staged = git_out("diff", "--name-only", "--cached", cwd=root)
    combined = (committed + "\n" + staged).splitlines()
    return list(dict.fromkeys(f.strip() for f in combined if f.strip()))


def git_ext_diff(root: Path, extensions: tuple) -> list:
    full_diff = git_full_diff(root)
    return [file for file in full_diff if file.endswith(extensions)]


# Logical steps
def step_check_tools():
    utils = ["git", "cmake", "clang-format", "clang-tidy"]
    failed = False
    for t in utils:
        if not shutil.which(t):
            fail(f"Tool '{t}' not found in PATH.")
            failed = True
    if failed:
        sys.exit(-1)


def step_branch(task: str, root: Path, no_uncommitted: bool = False):
    step("Step 1 - Branch check")
    target_branch = f"submits/{task}"
    if not git_out("branch", "--list", target_branch, cwd=root):
        fail(f"Target branch '{target_branch}' does not exist.")
        sys.exit(1)
    
    git_silent("checkout", target_branch, cwd=root)

    changed = git_full_diff(root)
    allowed_prefix = f"tasks/{task}/"
    bad = [file for file in changed if not file.startswith(allowed_prefix)]
    if bad:
        fail("Extra changes detected outside tasks/" + task + "/:")
        for f in bad:
            fail(f"  {f}")
        sys.exit(1)

    task_prefix = f"tasks/{task}/"
    staged = git_out("diff", "--name-only", "--cached", cwd=root)
    unstaged = git_out("diff", "--name-only", cwd=root)

    staged_task = [file for file in staged.splitlines() if file.strip().startswith(task_prefix)]
    unstaged_task = [file for file in unstaged.splitlines() if file.strip().startswith(task_prefix)]

    if (staged_task or unstaged_task) and not no_uncommitted:
        fail("Uncommitted changes detected in task directory. Commit them or use flag --no-uncommitted to ignore.")
        for f in staged_task:
            warn(f"  [staged]   {f}")
        for f in unstaged_task:
            warn(f"  [modified] {f}")
        sys.exit(1)

    committed = git_out("diff", "--name-only", "main...HEAD", cwd=root)
    if not committed:
        fail(f"No changes found on branch submits/{task} compared to main.")
        sys.exit(1)

    info(f"All changes are properly committed. {len(changed)} changed files.")


def step_lint(root: Path):
    step("Step 2 - Lint (clang-format)")
    sources = git_ext_diff(root, (".cpp", ".c", ".h", ".hpp"))
    for src in sources:
        ok = run_logged(
            ["clang-format", "--dry-run", "--Werror", str(src)],
            cwd=root,
        )
        if ok:
            info(f"Passed: {src}.")
        else:
            fail(f"Format issues in: {src}. Fix with: clang-format -i {src}.")
            sys.exit(2)

    info("Lint passed.")


def step_build(task: str, root: Path):
    step("Step 3 - Build (CMake)")
    build_dir = root / "build"
    if not (build_dir / "CMakeCache.txt").exists():
        info(f"Configuring CMake in {build_dir}")
        ok = run_logged(
            ["cmake", "-S", str(root), "-B", str(build_dir),
             "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"],
            cwd=root,
        )
        if not ok:
            fail("CMake configuration failed.")
            sys.exit(3)

    cmake_target = f"test_{task}"
    info(f"Building target: {cmake_target}")
    ok = run_logged(
        ["cmake", "--build", str(build_dir), "--target", cmake_target, "--parallel"],
        cwd=root,
    )
    if ok:
        info("Successfully built.")
    else:
        fail("Build failed, fix compilation errors.")
        sys.exit(3)


def step_tidy(root: Path):
    step("Step 4 - clang-tidy")
    build_dir = root / "build"
    if not (build_dir / "compile_commands.json").exists():
        fail(f"compile_commands.json not found in {build_dir}.")
        sys.exit(4)

    sources = git_ext_diff(root, (".cpp", ".c"))
    for src in sources:
        ok = run_logged(
            ["clang-tidy", "-p", str(build_dir), "--warnings-as-errors=*", str(src)],
            cwd=root,
        )
        if ok:
            info(f"Passed: {src}.")
        else:
            fail(f"Format issues in: {src}. Fix with: clang-tidy -p {build_dir} {src}.")
            sys.exit(4)
    
    info("clang-tidy passed.")


def step_tests(task: str, root: Path):
    step("Step 5 - Tests")
    build_dir = root / "build"
    exe_name = f"test_{task}"
    candidates = [
        build_dir / "tasks" / task / exe_name,
        build_dir / "tasks" / task / (exe_name + ".exe"),
    ]
    exe = next((p for p in candidates if p.is_file()), None)
    if exe is None:
        fail(f"Test binary '{exe_name}' not found in {build_dir}.")
        sys.exit(5)

    info(f"Running: {exe.relative_to(root)}")
    ok = run_logged([str(exe)], cwd=root)
    if ok:
        info("Tests passed.")
    else:
        fail("Tests failed, fix your solution before submitting.")
        sys.exit(5)


def step_submit(task: str, root: Path, args: argparse.Namespace):
    step("Step 6 - Submit")
    target_branch = f"submits/{task}"
    now_str = datetime.now().strftime("%Y-%m-%d %H:%M")
    flags = " ".join(
        f"--{k.replace('_', '-')}"
        for k, v in vars(args).items()
        if k != "task" and v is True
    ) or "none"

    description = (
        f"Auto-submit: {task} | "
        f"Date: {now_str} | "
        f"Flags: {flags}"
    )

    push_base = ["push", "-u", "origin", f"HEAD:refs/heads/{target_branch}"]

    if git_remote_branch_exists(target_branch, root):
        info(f"Branch '{target_branch}' already exists on remote. Updating without creating new MR.")
        ok = git_logged(*push_base, cwd=root)
    else:
        ok = git_logged(
            *push_base,
            "-o", "merge_request.create",
            "-o", "merge_request.target=main",
            "-o", f"merge_request.title=Submit: {task}",
            "-o", f"merge_request.description={description}",
            cwd=root,
        )

    if not ok:
        fail("git push failed.")
        sys.exit(6)

    info("Done. Open GitLab to see the Merge Request.")


def main():
    parser = argparse.ArgumentParser(
        prog="submit.py",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument(
        "task",
        help="Task name, e.g. multiplication",
    )
    parser.add_argument(
        "--no-send",
        action="store_true",
        help="Run all checks, but do not push",
    )
    parser.add_argument(
        "--no-uncommitted",
        action="store_true",
        help="Ignore uncommitted changes in task directory (not recommended)",
    )
    args = parser.parse_args()
    task = args.task
    root = git_repo_root()

    print(f"HSE C++ Submitter | task: {task}")
    if args.no_send:
        warn("--no-send: will run checks but skip push and MR creation")

    step_check_tools()
    step_branch(task, root, no_uncommitted=args.no_uncommitted)
    step_lint(root)
    step_build(task, root) # Build before tidy to ensure compile_commands.json is up to date
    step_tidy(root)
    step_tests(task, root)

    info(f"All local checks passed. Ready to submit.")
    if args.no_send:
        sys.exit(0)

    step_submit(task, root, args)
    print(f"Submission complete.")


if __name__ == "__main__":
    main()
