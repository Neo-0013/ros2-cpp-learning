# ROS 2 Commands — 02_nodes

Command reference for the `node_demo_pkg` chapter.

> **Environment:** ROS 2 Jazzy, `zsh`, workspace at `~/ros2_ws`. Replace `setup.zsh` with `setup.bash` if you use bash.

## Contents

1. [Setup](#1-setup)
2. [Build](#2-build)
3. [Run](#3-run-the-node)
4. [Node inspection](#4-node-inspection)
5. [Parameters](#5-parameters)
6. [Validation tests](#6-parameter-validation-tests)
7. [Dynamic update test](#7-runtime-dynamic-update-test)
8. [Package tests](#8-tests)
9. [Package commands](#9-package-commands)
10. [Git commands](#10-git-commands)
11. [Cheat sheet](#11-cheat-sheet)
12. [Troubleshooting](#12-troubleshooting)

---

## 1. Setup

```bash
cd ~/ros2_ws

source /opt/ros/jazzy/setup.zsh     # ROS 2 underlay
source ~/ros2_ws/install/setup.zsh  # your workspace overlay
```

Source the **underlay first, then the overlay**, in every new terminal. Re-source the workspace after each build that adds or renames executables.

---

## 2. Build

Build only this package:

```bash
cd ~/ros2_ws
CC=gcc-14 CXX=g++-14 colcon build \
  --packages-select node_demo_pkg
```

Build the whole workspace:

```bash
cd ~/ros2_ws
colcon build
```

Optional flags:

| Flag | Purpose |
|------|---------|
| `--symlink-install` | Link installed files to the source tree, so edits to scripts and config apply without rebuilding |
| `--event-handlers console_direct+` | Show compiler output live |
| `--cmake-args -DCMAKE_BUILD_TYPE=Debug` | Debug build |

---

## 3. Run the Node

```bash
ros2 run node_demo_pkg node_demo
```

Expected node name: `/node_demo`

Start with a different period without editing code:

```bash
ros2 run node_demo_pkg node_demo --ros-args -p timer_period:=0.25
```

---

## 4. Node Inspection

```bash
ros2 node list             # list running nodes
ros2 node info /node_demo  # inspect one node
```

`ros2 node info` lists the node's ROS graph interfaces:

- publishers
- subscribers
- service servers and clients
- action servers and clients

For this node, most sections will be empty or contain only the default parameter services and `/rosout`. You will see richer output in later chapters.

---

## 5. Parameters

| Task | Command |
|------|---------|
| List all parameters | `ros2 param list /node_demo` |
| Get the timer period | `ros2 param get /node_demo timer_period` |
| Describe type and constraints | `ros2 param describe /node_demo timer_period` |
| Dump all parameters as YAML | `ros2 param dump /node_demo` |
| Set to 2 seconds | `ros2 param set /node_demo timer_period 2.0` |
| Set to 0.5 seconds | `ros2 param set /node_demo timer_period 0.5` |
| Reset to 1 second | `ros2 param set /node_demo timer_period 1.0` |

Expected initial value:

```text
Double value is: 1.0
```

### Parameter types

The parameter is declared as:

```cpp
declare_parameter<double>("timer_period", 1.0);
```

Use a floating-point value:

```bash
ros2 param set /node_demo timer_period 1.0   # OK
ros2 param set /node_demo timer_period 1     # FAILS (parsed as integer)
```

The integer form is rejected with a type-mismatch error. Always include a decimal point for `double` parameters.

---

## 6. Parameter Validation Tests

The node requires `timer_period > 0`.

```bash
ros2 param set /node_demo timer_period 0.0    # invalid: zero
ros2 param set /node_demo timer_period -1.0   # invalid: negative
```

Both should be rejected with a message like:

```text
Setting parameter failed: timer_period must be greater than 0
```

> **Use `0.0`, not `0`.** The plain `0` is an integer, so you would get a *type* error before your validation code ever runs.

The exact CLI wording can differ slightly between ROS 2 distributions.

---

## 7. Runtime Dynamic Update Test

**Terminal 1** — start the node:

```bash
ros2 run node_demo_pkg node_demo
```

You should see about one callback per second.

**Terminal 2** — source ROS 2 and the workspace, then change the period:

```bash
ros2 param set /node_demo timer_period 2.0
```

Terminal 1 should report:

```text
Timer period changed to 2.00 seconds
```

Change it again:

```bash
ros2 param set /node_demo timer_period 0.5
```

The callback should now run about twice per second.

---

## 8. Tests

```bash
colcon test --packages-select node_demo_pkg   # run package tests
colcon test-result --verbose                  # show detailed results
```

Expected result:

```text
Summary: 8 tests, 0 errors, 0 failures, 1 skipped
```

These are mostly `ament_lint` style and static-analysis checks. They verify code quality, not runtime behavior.

---

## 9. Package Commands

```bash
ros2 pkg list | grep node_demo_pkg   # confirm the package is found
ros2 pkg prefix node_demo_pkg        # install location
ros2 pkg executables node_demo_pkg   # list executables
```

Expected executable output:

```text
node_demo_pkg node_demo
```

---

## 10. Git Commands

```bash
cd ~/ros2-cpp-learning

git status                       # working tree status
git log --oneline --decorate -5  # last 5 commits
git diff                         # unstaged changes
git diff --staged                # staged changes
```

---

## 11. Cheat Sheet

| Command | Purpose |
|---------|---------|
| `ros2 run <pkg> <exe>` | Run a ROS 2 executable |
| `ros2 node list` | List running nodes |
| `ros2 node info <node>` | Inspect a node |
| `ros2 param list <node>` | List parameters |
| `ros2 param get <node> <name>` | Read a parameter |
| `ros2 param set <node> <name> <value>` | Change a parameter |
| `ros2 param describe <node> <name>` | Show type and constraints |
| `ros2 pkg executables <pkg>` | List a package's executables |
| `colcon build` | Build packages |
| `colcon test` | Run package tests |
| `colcon test-result --verbose` | Show test results |

---

## 12. Troubleshooting

| Problem | Likely cause | Fix |
|---------|--------------|-----|
| `Package 'node_demo_pkg' not found` | Workspace not sourced | `source ~/ros2_ws/install/setup.zsh` |
| `No executable found` | Not rebuilt or not re-sourced | Rebuild, then source again |
| `ros2 node list` shows nothing | Different terminal or `ROS_DOMAIN_ID` | Source in this terminal and check `echo $ROS_DOMAIN_ID` matches |
| `Set parameter failed` with a type error | Integer used for a `double` | Use `1.0` instead of `1` |
| Parameter change rejected | Value is `<= 0` | Use a positive number |
| Build picks the wrong compiler | `CC`/`CXX` not set | Prefix with `CC=gcc-14 CXX=g++-14` |
