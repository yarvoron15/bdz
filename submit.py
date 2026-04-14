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
import json
import re
from urllib.parse import urlencode
from urllib.request import Request, urlopen
from urllib.error import HTTPError

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


def git_repo_name(root: Path) -> str:
    ci_name = os.getenv("CI_PROJECT_NAME")
    if ci_name:
        return ci_name
    url = git_out("remote", "get-url", "origin", cwd=root).strip()
    name = url.rstrip("/").split("/")[-1]
    if name.endswith(".git"):
        name = name[:-4]
    return name


def extract_group_tag(repo_name: str) -> str:
    match = re.match(r"(se\d+|staff)", repo_name)
    if match:
        return match.group(1)
    return repo_name.split("-", 1)[0].split("_", 1)[0]


# API helpers
def api_json(method: str, url: str, token: str, payload: dict | None = None):
    headers = {
        "PRIVATE-TOKEN": token,
        "Content-Type": "application/json",
    }
    data = None if payload is None else json.dumps(payload).encode("utf-8")
    req = Request(url, data=data, headers=headers, method=method)

    try:
        with urlopen(req, timeout=30) as resp:
            body = resp.read()
            return json.loads(body.decode("utf-8")) if body else None
    except HTTPError as e:
        body = e.read().decode("utf-8", errors="replace")
        raise RuntimeError(f"HTTP {e.code}: {body}") from e


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


def step_submit(task: str, root: Path):
    step("Step 6 - Submit")
    target_branch = f"submits/{task}"
    push_base = ["push", "-u", "origin", f"HEAD:refs/heads/{target_branch}"]
    ok = git_logged(*push_base, cwd=root)

    if not ok:
        fail("git push failed.")
        sys.exit(6)

    info("Done. Changes were pushed.")


def step_ensure_mr(task: str, root: Path):
    step("Creating merge request")

    if os.getenv("CI_PIPELINE_SOURCE") != "push":
        info("Not a push pipeline, skipping MR creation.")
        return

    branch = f"submits/{task}"
    if os.getenv("CI_OPEN_MERGE_REQUESTS"):
        info("Open MR already exists, skipping.")
        return

    token = os.getenv("GITLAB_TOKEN")
    if not token:
        fail("GITLAB_TOKEN is not set.")
        sys.exit(7)

    project_id = os.getenv("CI_PROJECT_ID")
    api_v4 = os.getenv("CI_API_V4_URL")
    default_branch = os.getenv("CI_DEFAULT_BRANCH", "main")
    project_name = os.getenv("CI_PROJECT_NAME", root.name)

    if not project_id or not api_v4:
        fail("CI_PROJECT_ID or CI_API_V4_URL is not set.")
        sys.exit(7)

    group_tag = extract_group_tag(project_name)
    params = urlencode({
        "state": "opened",
        "source_branch": branch,
        "target_branch": default_branch,
    })
    list_url = f"{api_v4}/projects/{project_id}/merge_requests?{params}"

    existing = api_json("GET", list_url, token)
    if existing:
        info(f"MR already exists: {existing[0]['web_url']}")
        return

    payload = {
        "source_branch": branch,
        "target_branch": default_branch,
        "title": f"[{group_tag}] Submit: {task}",
        "description": f"Auto-submit for task '{task}'.",
        "remove_source_branch": False,
    }
    created = api_json("POST", f"{api_v4}/projects/{project_id}/merge_requests", token, payload)
    info(f"MR created: {created['web_url']}")


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
    parser.add_argument(
        "--ensure-mr",
        action="store_true",
        help="CI only",
    )
    args = parser.parse_args()
    task = args.task
    root = git_repo_root()

    if args.ensure_mr:
        step_ensure_mr(task, root)
        sys.exit(0)

    print(f"HSE C++ Submitter | task: {task}")
    if args.no_send:
        warn("--no-send: will run checks but skip push")

    step_check_tools()
    step_branch(task, root, no_uncommitted=args.no_uncommitted)
    step_lint(root)
    step_build(task, root) # Build before tidy to ensure compile_commands.json is up to date
    step_tidy(root)
    step_tests(task, root)

    info(f"All local checks passed.")
    if args.no_send:
        sys.exit(0)

    step_submit(task, root)
    print(f"Submission complete.")


if __name__ == "__main__":
    main()
