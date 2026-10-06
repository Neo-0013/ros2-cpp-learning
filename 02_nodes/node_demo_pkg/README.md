# ROS 2 Nodes — C++ Learning Chapter

This chapter covers the fundamentals of creating and controlling a ROS 2 node in C++ with `rclcpp`.

The implementation grows from a minimal node into a **class-based node with a timer and runtime-configurable parameters**.

> **Environment:** ROS 2 (Humble or newer), C++17, `colcon`. Commands assume a `zsh` shell; use `setup.bash` if you use bash.

---

## Learning Objectives

By the end of this chapter you should be able to:

- [ ] Create a ROS 2 node in C++
- [ ] Explain how `rclcpp::Node` is used
- [ ] Explain why class-based nodes are useful
- [ ] Explain how `rclcpp::spin()` keeps a node alive
- [ ] Create and own a ROS 2 timer
- [ ] Declare and read parameters
- [ ] Change parameters at runtime
- [ ] Validate parameter values
- [ ] Recreate a timer when its period changes
- [ ] Inspect a node and its parameters from the ROS 2 CLI

---

## Package Structure

```text
02_nodes/
└── node_demo_pkg/
    ├── CMakeLists.txt
    ├── package.xml
    └── src/
        └── node_demo.cpp
```

The package is linked into the ROS 2 workspace with a symbolic link:

```text
ros2-cpp-learning/02_nodes/node_demo_pkg      (Git repository)
        │
        └── symlink ──▶  ~/ros2_ws/src/node_demo_pkg
```

The GitHub repository is the **source of truth**; the workspace only provides the build environment.

### `package.xml` (essentials)

```xml
<buildtool_depend>ament_cmake</buildtool_depend>
<depend>rclcpp</depend>

<test_depend>ament_lint_auto</test_depend>
<test_depend>ament_lint_common</test_depend>
```

### `CMakeLists.txt` (essentials)

```cmake
cmake_minimum_required(VERSION 3.8)
project(node_demo_pkg)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

add_executable(node_demo src/node_demo.cpp)
ament_target_dependencies(node_demo rclcpp)

install(TARGETS node_demo DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
```

---

## 1. Creating a ROS 2 Node

A class-based node inherits from `rclcpp::Node`:

```cpp
class NodeDemo : public rclcpp::Node
{
public:
  NodeDemo()
  : Node("node_demo")
  {
  }
};
```

Inheriting gives the class direct access to core ROS 2 functionality:

| Capability  | Example                   |
|-------------|---------------------------|
| Publishers  | `create_publisher<T>()`   |
| Subscribers | `create_subscription<T>()`|
| Timers      | `create_wall_timer()`     |
| Parameters  | `declare_parameter<T>()`  |
| Services    | `create_service<T>()`     |
| Actions     | via `rclcpp_action`       |
| Logging     | `get_logger()`            |

The constructor argument `Node("node_demo")` sets the node name, which appears in ROS 2 as `/node_demo`.

**Why class-based nodes?** State (the timer, parameter values, publishers) lives together with the callbacks that use it, so there are no globals and the node is easy to extend and test.

---

## 2. Initializing and Running ROS 2

Every ROS 2 C++ executable follows the same pattern:

```cpp
int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);                    // 1. initialize ROS 2

  auto node = std::make_shared<NodeDemo>();    // 2. create the node
  rclcpp::spin(node);                          // 3. process callbacks until shutdown

  rclcpp::shutdown();                          // 4. clean up
  return 0;
}
```

```text
rclcpp::init()
      ↓
Create node (std::make_shared)
      ↓
rclcpp::spin()   ← blocks; executor runs timers, parameter events, etc.
      ↓
Ctrl+C / shutdown signal
      ↓
rclcpp::shutdown()
```

`rclcpp::spin()` creates a single-threaded executor that waits for events and runs the matching callbacks. Without it, `main()` would return immediately and the timer would never fire.

---

## 3. Logging

```cpp
RCLCPP_INFO(this->get_logger(), "Node has started!");
```

Output:

```text
[INFO] [1700000000.123456789] [node_demo]: Node has started!
```

Prefer ROS 2 logging over `std::cout`: it carries the node name and a timestamp, supports severity levels (`DEBUG`, `INFO`, `WARN`, `ERROR`, `FATAL`), can be filtered at runtime, and is published on `/rosout`.

---

## 4. Timers

A wall timer runs a callback periodically.

```cpp
timer_ = this->create_wall_timer(
  std::chrono::duration<double>(timer_period_),
  std::bind(&NodeDemo::timer_callback, this));
```

The callback:

```cpp
void timer_callback()
{
  RCLCPP_INFO(
    this->get_logger(),
    "Timer callback executed! Period: %.2f s",
    timer_period_);
}
```

The timer is stored as a class member:

```cpp
rclcpp::TimerBase::SharedPtr timer_;
```

**Why store the timer?** A timer only exists while something owns it. If the `SharedPtr` were a local variable, the timer would be destroyed when the constructor returned and the callback would never run. Making it a member ties its lifetime to the node.

---

## 5. Parameters

The timer period is configurable through a parameter:

```cpp
this->declare_parameter<double>("timer_period", 1.0);
```

| Property      | Value          |
|---------------|----------------|
| Name          | `timer_period` |
| Type          | `double`       |
| Default value | `1.0`          |

Read the initial value:

```cpp
timer_period_ = this->get_parameter("timer_period").as_double();
```

Inspect it from another terminal:

```bash
ros2 param get /node_demo timer_period
```

---

## 6. Dynamic Parameter Updates

The key feature of this chapter: **the timer period can change while the node is running.**

Register a parameter callback:

```cpp
parameter_callback_handle_ =
  this->add_on_set_parameters_callback(
    std::bind(
      &NodeDemo::parameter_callback,
      this,
      std::placeholders::_1));
```

Store the handle as a member. If it is destroyed, the callback is unregistered:

```cpp
rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr
  parameter_callback_handle_;
```

The callback receives the parameters that are about to be set:

```cpp
rcl_interfaces::msg::SetParametersResult parameter_callback(
  const std::vector<rclcpp::Parameter> & parameters)
```

When `timer_period` changes, the node:

1. Reads the new value
2. Validates it
3. Stores it
4. Cancels the old timer
5. Creates a new timer with the new period

```text
ros2 param set
       │
       ▼
Parameter callback
       │
       ▼
Validate value ──── invalid ──▶ result.successful = false (change rejected)
       │ valid
       ▼
Update timer_period_
       │
       ▼
Cancel old timer
       │
       ▼
Create new timer
       │
       ▼
New callback frequency
```

---

## 7. Parameter Validation

Reject invalid values before they affect runtime behavior:

```cpp
if (new_period <= 0.0) {
  result.successful = false;
  result.reason = "timer_period must be greater than 0";
  return result;
}
```

So both of these are rejected:

```bash
ros2 param set /node_demo timer_period 0.0
ros2 param set /node_demo timer_period -1.0
```

> **Principle:** Validate configuration *before* allowing it to change runtime behavior.

---

## 8. Parameter Types

ROS 2 parameters are **strongly typed**. `timer_period` was declared as `double`, so it must be set with a floating-point value:

```bash
ros2 param set /node_demo timer_period 2.0   # OK
ros2 param set /node_demo timer_period 1     # FAILS: 1 is parsed as an integer
```

The second command is rejected with a type-mismatch error. Always include a decimal point (`1.0`, `0.0`) for `double` parameters.

> **Note:** This also applies to the invalid-value test. Use `0.0`, not `0`. With `0` you would see a *type* error and never reach your validation code.

---

## 9. Complete Source

```cpp
#include <chrono>
#include <functional>
#include <memory>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "rcl_interfaces/msg/set_parameters_result.hpp"

class NodeDemo : public rclcpp::Node
{
public:
  NodeDemo()
  : Node("node_demo")
  {
    RCLCPP_INFO(this->get_logger(), "Node has started!");

    this->declare_parameter<double>("timer_period", 1.0);
    timer_period_ = this->get_parameter("timer_period").as_double();

    parameter_callback_handle_ =
      this->add_on_set_parameters_callback(
        std::bind(
          &NodeDemo::parameter_callback,
          this,
          std::placeholders::_1));

    create_timer();
  }

private:
  void create_timer()
  {
    if (timer_) {
      timer_->cancel();
    }
    timer_ = this->create_wall_timer(
      std::chrono::duration<double>(timer_period_),
      std::bind(&NodeDemo::timer_callback, this));
  }

  void timer_callback()
  {
    RCLCPP_INFO(
      this->get_logger(),
      "Timer callback executed! Period: %.2f s",
      timer_period_);
  }

  rcl_interfaces::msg::SetParametersResult parameter_callback(
    const std::vector<rclcpp::Parameter> & parameters)
  {
    rcl_interfaces::msg::SetParametersResult result;
    result.successful = true;

    for (const auto & parameter : parameters) {
      if (parameter.get_name() != "timer_period") {
        continue;
      }

      const double new_period = parameter.as_double();

      if (new_period <= 0.0) {
        result.successful = false;
        result.reason = "timer_period must be greater than 0";
        return result;
      }

      timer_period_ = new_period;
      create_timer();

      RCLCPP_INFO(
        this->get_logger(),
        "timer_period changed to %.2f s", timer_period_);
    }

    return result;
  }

  double timer_period_{1.0};
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr
    parameter_callback_handle_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<NodeDemo>());
  rclcpp::shutdown();
  return 0;
}
```

---

## 10. Build and Run

Build:

```bash
cd ~/ros2_ws

CC=gcc-14 CXX=g++-14 colcon build \
  --packages-select node_demo_pkg
```

Source the workspace:

```bash
source install/setup.zsh
```

Run the node:

```bash
ros2 run node_demo_pkg node_demo
```

---

## 11. Testing Dynamic Behavior

With the node running, use a second terminal (sourced as above).

| Goal                  | Command                                          | Expected result                                 |
|-----------------------|--------------------------------------------------|-------------------------------------------------|
| Default behavior      | *(none)*                                         | Callback fires every 1 s                        |
| Slow down             | `ros2 param set /node_demo timer_period 2.0`     | `Set parameter successful`, callback every 2 s  |
| Speed up              | `ros2 param set /node_demo timer_period 0.5`     | Callback every 0.5 s                            |
| Invalid value         | `ros2 param set /node_demo timer_period 0.0`     | Rejected: `timer_period must be greater than 0`  |
| Wrong type            | `ros2 param set /node_demo timer_period 1`       | Rejected: type mismatch                         |

The exact wording of CLI messages can differ slightly between ROS 2 distributions.

---

## 12. Inspecting the Node from the CLI

```bash
ros2 node list                                   # running nodes
ros2 node info /node_demo                        # topics, services, actions, parameters
ros2 param list /node_demo                       # all parameters
ros2 param get /node_demo timer_period           # current value
ros2 param describe /node_demo timer_period      # type, constraints, description
ros2 param dump /node_demo                       # export parameters as YAML
```

You can also set the parameter at startup instead of at runtime:

```bash
ros2 run node_demo_pkg node_demo --ros-args -p timer_period:=0.25
```

---

## 13. Automated Tests

```bash
cd ~/ros2_ws
colcon test --packages-select node_demo_pkg
colcon test-result --verbose
```

Expected result:

```text
Summary: 8 tests, 0 errors, 0 failures, 1 skipped
```

These tests come from `ament_lint_auto` (code style, copyright, static analysis, and so on). They check code quality, not node behavior. A natural next step is to add a `gtest` that verifies the parameter validation logic.

---

## 14. Mental Model

```text
                    ROS 2 Node
                        │
        ┌───────────────┼────────────────┐
        │               │                │
    Parameter         Timer           Logger
        │               │                │
        ▼               ▼                ▼
  timer_period      callback()       RCLCPP_INFO
        │
        ▼
 Runtime configuration
```

- The **node** is the main unit of execution and ownership.
- **Timers** and **parameter callbacks** are event sources.
- The **executor** (driven by `rclcpp::spin()`) waits for events and runs their callbacks.
- With the default single-threaded executor, callbacks never run at the same time, so `timer_period_` needs no mutex here. With a multi-threaded executor you would need to protect it.

---

## 15. Common Pitfalls

| Pitfall | Symptom | Fix |
|---------|---------|-----|
| Timer stored in a local variable | Callback never runs | Keep it as a class member |
| Forgot `rclcpp::spin()` | Node exits immediately | Spin the node in `main()` |
| Integer value for a `double` parameter | Type-mismatch error | Use `1.0`, not `1` |
| Parameter callback handle not stored | Callback silently stops working | Keep the handle as a member |
| Not checking `parameter.get_name()` | Wrong logic runs when other parameters are added | Filter by name in the callback |
| `ros2 run` can't find the executable | `No executable found` | Rebuild and re-source `install/setup.zsh` |

### Design note

`add_on_set_parameters_callback` runs **before** the value is committed. If a single `set` request contains several parameters and a later one is rejected, the timer may already have been recreated for an update that was ultimately rolled back. For this simple node that is acceptable. Newer distributions provide `add_post_set_parameters_callback`, which is a better place to apply side effects after validation has succeeded.

Also note that the callback is registered *after* `declare_parameter`, so an initial value supplied at launch (for example `-p timer_period:=0.0`) is not validated by it. Registering the callback before declaring the parameter closes that gap.

---

## Exercises

1. Add a second parameter, `message`, and print it from the timer callback.
2. Add a `ParameterDescriptor` with a `FloatingPointRange` (for example 0.01 to 10.0) and observe how ROS 2 enforces it.
3. Move the validation logic into a free function and unit-test it with `gtest`.
4. Replace the parameter callback with `add_post_set_parameters_callback` (if your distro supports it).
5. Load `timer_period` from a YAML file via `--params-file`.

---

## Quick Reference

| Task                      | API / command                                  |
|---------------------------|------------------------------------------------|
| Initialize ROS 2          | `rclcpp::init(argc, argv)`                     |
| Keep node alive           | `rclcpp::spin(node)`                           |
| Shut down                 | `rclcpp::shutdown()`                           |
| Log a message             | `RCLCPP_INFO(get_logger(), "...")`             |
| Periodic callback         | `create_wall_timer(period, callback)`          |
| Declare a parameter       | `declare_parameter<T>(name, default)`          |
| Read a parameter          | `get_parameter(name).as_double()`              |
| Validate changes          | `add_on_set_parameters_callback(cb)`           |
| Set a parameter (CLI)     | `ros2 param set /node name value`              |

---

## What I Learned

- [x] Create a ROS 2 C++ node with `rclcpp`
- [x] Build class-based nodes
- [x] Create and own periodic timers
- [x] Declare and read ROS 2 parameters
- [x] Inspect parameters from the CLI
- [x] Modify parameters at runtime
- [x] Validate parameter values and respect parameter types
- [x] Dynamically recreate a timer
- [x] Build and test a package with `colcon`

---

## Next Chapter

**ROS 2 Services** — the communication model changes from publish/subscribe:

```text
Publisher ──▶ Topic ──▶ Subscriber
```

to request/response:

```text
Client ──▶ Request  ──▶ Service Server
Client ◀── Response ◀── Service Server
```
