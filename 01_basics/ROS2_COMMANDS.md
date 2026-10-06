# ROS 2 Command Reference

A quick reference for the ROS 2 CLI commands used while learning the fundamentals.

---

## 1. Check ROS 2 Environment

Check the active ROS 2 distribution:

```bash
echo $ROS_DISTRO
```

Expected:

```text
jazzy
```

Check where the ROS 2 CLI is installed:

```bash
which ros2
```

---

## 2. Package Commands

List installed ROS 2 packages:

```bash
ros2 pkg list
```

Find the installation prefix of a package:

```bash
ros2 pkg prefix my_first_cpp_pkg
```

---

## 3. Node Commands

List currently running nodes:

```bash
ros2 node list
```

Get information about a node:

```bash
ros2 node info /my_first_node
```

Run a node:

```bash
ros2 run my_first_cpp_pkg my_first_node
```

---

## 4. Build Commands

Build the complete workspace:

```bash
cd ~/ros2_ws
colcon build
```

Build a specific package:

```bash
colcon build --packages-select my_first_cpp_pkg
```

Source the workspace after building:

```bash
source ~/ros2_ws/install/setup.zsh
```

---

## 5. Useful ROS 2 Development Workflow

A typical ROS 2 development workflow is:

```text
Edit Source Code
       ↓
Build Package
       ↓
Source Workspace
       ↓
Run Node
       ↓
Inspect ROS 2 Graph
       ↓
Test
       ↓
Commit Changes
```

---

## 6. Important ROS 2 Concepts

### Package

A ROS 2 package contains related source code, configuration, dependencies, and other resources required for a particular ROS 2 application or component.

### Node

A node is an executable ROS 2 component responsible for performing a specific task.

### Workspace

A ROS 2 workspace is a directory used to organize and build ROS 2 packages.

### `colcon`

`colcon` is the build tool commonly used to build ROS 2 workspaces.

### `rclcpp`

`rclcpp` is the C++ client library for ROS 2.

---

## 7. Useful Linux Commands

Show the current directory:

```bash
pwd
```

List files:

```bash
ls
```

List files with detailed information:

```bash
ls -la
```

Change directory:

```bash
cd <directory>
```

Clear the terminal:

```bash
clear
```

---

## 8. Stopping a ROS 2 Node

To stop a running ROS 2 node:

```text
CTRL + C
```

---

## 9. Basic ROS 2 Session Example

Terminal 1:

```bash
source /opt/ros/jazzy/setup.zsh
source ~/ros2_ws/install/setup.zsh

ros2 run my_first_cpp_pkg my_first_node
```

Terminal 2:

```bash
source /opt/ros/jazzy/setup.zsh
source ~/ros2_ws/install/setup.zsh

ros2 node list
```

Expected:

```text
/my_first_node
```

Inspect the node:

```bash
ros2 node info /my_first_node
```

---

## Key Takeaway

The ROS 2 CLI provides the tools needed to:

* Discover packages
* Run nodes
* Inspect nodes
* Build packages
* Debug the ROS 2 system
* Understand the running ROS 2 graph

This command reference will be expanded as new ROS 2 concepts are introduced.
