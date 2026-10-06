# 01 - ROS 2 Basics

This chapter is the starting point of my ROS 2 C++ learning journey.

The goal is to understand the basic structure of a ROS 2 C++ program, create a simple ROS 2 node, build it using `colcon`, and run it using the ROS 2 CLI.

---

## 🎯 Learning Objectives

By completing this chapter, I learned:

* What ROS 2 is
* The basic architecture of a ROS 2 system
* What a ROS 2 node is
* How to create a ROS 2 C++ package
* How `rclcpp` is used in C++ ROS 2 programs
* How to initialize and shut down ROS 2
* How to create a node
* How to use ROS 2 logging
* How `rclcpp::spin()` keeps a node alive
* How to build a ROS 2 package using `colcon`
* How to run a ROS 2 executable using the ROS 2 CLI

---

## 📁 Package Information

**Package:** `my_first_cpp_pkg`

**Executable:** `my_first_node`

**Build type:** `ament_cmake`

**Language:** C++

**ROS 2 Distribution:** Jazzy Jalisco

---

## 📂 Project Structure

```text
01_basics/
└── my_first_cpp_pkg/
    ├── CMakeLists.txt
    ├── package.xml
    ├── include/
    │   └── my_first_cpp_pkg/
    └── src/
        └── my_first_node.cpp
```

---

## 🧠 What is ROS 2?

ROS 2 (Robot Operating System 2) is a middleware framework used for building robotic applications.

It provides tools and communication mechanisms that allow different parts of a robot software system to work together.

A robot can be divided into multiple independent software components called **nodes**.

For example:

```text
                 Robot System
                      │
       ┌──────────────┼──────────────┐
       ↓              ↓              ↓
   Camera Node    Control Node    Motor Node
```

These nodes can communicate using ROS 2 communication mechanisms such as:

* Topics
* Services
* Actions
* Parameters

These concepts will be studied in later chapters.

---

## 🔵 What is a ROS 2 Node?

A **node** is an executable unit of a ROS 2 application.

A node usually performs one specific responsibility.

Examples of nodes in a robot could include:

```text
Camera Node
    ↓
Image Processing Node
    ↓
Navigation Node
    ↓
Motor Control Node
```

In this chapter, we create our first node:

```text
/my_first_node
```

---

# 💻 Creating the Package

The package was created using:

```bash
cd ~/ros2-cpp-learning/01_basics

ros2 pkg create \
    --build-type ament_cmake \
    my_first_cpp_pkg \
    --dependencies rclcpp
```

This creates the basic structure required for a ROS 2 C++ package.

---

# 📝 First ROS 2 C++ Program

The source file is:

```text
my_first_cpp_pkg/src/my_first_node.cpp
```

The program follows this basic structure:

```text
C++ Program
     │
     ├── Include ROS 2
     │
     ├── Initialize ROS 2
     │
     ├── Create Node
     │
     ├── Log Message
     │
     ├── Spin Node
     │
     ├── Shutdown ROS 2
     │
     └── Exit
```

---

## 🔍 Important ROS 2 Concepts in the Program

### 1. Include `rclcpp`

```cpp
#include "rclcpp/rclcpp.hpp"
```

`rclcpp` is the ROS 2 C++ client library.

It provides the C++ API used to create and interact with ROS 2 nodes.

---

### 2. Initialize ROS 2

```cpp
rclcpp::init(argc, argv);
```

This initializes the ROS 2 communication system.

It should be called before creating ROS 2 nodes.

---

### 3. Create the Node

```cpp
auto node = rclcpp::Node::make_shared("my_first_node");
```

This creates a ROS 2 node named:

```text
my_first_node
```

The node will appear in the ROS 2 graph using the name:

```text
/my_first_node
```

---

### 4. Log a Message

```cpp
RCLCPP_INFO(
    node->get_logger(),
    "Hello ROS 2!"
);
```

`RCLCPP_INFO()` is used to print an informational log message.

The node's logger is obtained using:

```cpp
node->get_logger()
```

---

### 5. Keep the Node Running

```cpp
rclcpp::spin(node);
```

`spin()` keeps the node alive and allows ROS 2 to process callbacks and communication events.

In this simple example, there are no callbacks yet, but `spin()` establishes the normal ROS 2 node execution pattern.

---

### 6. Shut Down ROS 2

```cpp
rclcpp::shutdown();
```

This shuts down the ROS 2 system cleanly.

---

# 🏗️ Building the Package

From the ROS 2 workspace:

```bash
cd ~/ros2_ws
```

Build the package:

```bash
colcon build --packages-select my_first_cpp_pkg
```

After a successful build, source the workspace:

```bash
source ~/ros2_ws/install/setup.zsh
```

---

# ▶️ Running the Node

Run:

```bash
ros2 run my_first_cpp_pkg my_first_node
```

Expected output:

```text
[INFO] [my_first_node]: Hello ROS 2!
```

The node will continue running because of:

```cpp
rclcpp::spin(node);
```

Press:

```text
CTRL + C
```

to stop it.

---

# 🔎 Inspecting the Node

While the node is running, open another terminal and source ROS 2:

```bash
source /opt/ros/jazzy/setup.zsh
source ~/ros2_ws/install/setup.zsh
```

Check running nodes:

```bash
ros2 node list
```

Expected:

```text
/my_first_node
```

Get information about the node:

```bash
ros2 node info /my_first_node
```

At this stage, the node does not have publishers, subscribers, services, or actions.

---

# 🔄 ROS 2 Node Lifecycle in This Example

The basic execution flow is:

```text
┌──────────────────────┐
│     Start Program    │
└──────────┬───────────┘
           ↓
┌──────────────────────┐
│   rclcpp::init()     │
└──────────┬───────────┘
           ↓
┌──────────────────────┐
│     Create Node      │
└──────────┬───────────┘
           ↓
┌──────────────────────┐
│     Log Message      │
└──────────┬───────────┘
           ↓
┌──────────────────────┐
│    rclcpp::spin()    │
└──────────┬───────────┘
           │
           │ CTRL + C
           ↓
┌──────────────────────┐
│ rclcpp::shutdown()   │
└──────────┬───────────┘
           ↓
┌──────────────────────┐
│     Exit Program     │
└──────────────────────┘
```

---

# 🧪 What I Practiced

* Creating a ROS 2 C++ package
* Understanding `ament_cmake`
* Writing a basic ROS 2 C++ executable
* Using `rclcpp`
* Creating a ROS 2 node
* Using ROS 2 logging
* Building with `colcon`
* Running nodes with `ros2 run`
* Inspecting nodes with `ros2 node`

---

# 📚 Key Takeaways

### ROS 2

A framework/middleware used to build distributed robotic applications.

### Node

An executable ROS 2 component responsible for a specific task.

### `rclcpp`

The ROS 2 C++ client library.

### `rclcpp::init()`

Initializes ROS 2.

### `rclcpp::spin()`

Keeps a node running and processes ROS 2 events/callbacks.

### `rclcpp::shutdown()`

Shuts down ROS 2 cleanly.

### `colcon`

The build tool commonly used for ROS 2 workspaces.

---

# 🚀 Next Chapter

The next chapter goes deeper into **ROS 2 Nodes**, including:

* Node structure
* Node names
* Node discovery
* Node introspection
* Multiple nodes
* Node-to-node architecture
* Better C++ node design

➡️ **[Go to Chapter 02 - Nodes](../02_nodes/)**
