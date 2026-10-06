ROS 2 Nodes --- C++ Learning Chapter

This chapter covers the fundamentals of creating and controlling a ROS 2
node using C++ and rclcpp.

The implementation progresses from a basic node to a class-based node
with a timer and runtime-configurable parameters.

Learning Objectives

By completing this chapter, you should understand:

How a ROS 2 node is created in C++

How rclcpp::Node is used

Why class-based ROS 2 nodes are useful

How rclcpp::spin() keeps a node alive

How ROS 2 timers work

How to declare and read parameters

How to change parameters at runtime

How to validate parameter values

How to recreate a timer when its period changes

How to inspect a node and its parameters from the ROS 2 CLI

Package Structure

02_nodes/
└── node_demo_pkg/
    ├── CMakeLists.txt
    ├── package.xml
    └── src/
        └── node_demo.cpp

The package is linked into the ROS 2 workspace using a symbolic link:

ros2-cpp-learning/02_nodes/node_demo_pkg
                │
                └── symlink
                        ↓
                ros2_ws/src/node_demo_pkg

The GitHub repository is the source of truth. The ROS 2 workspace
provides the build environment.

1. Creating a ROS 2 Node

The node inherits from rclcpp::Node:

class NodeDemo : public rclcpp::Node
{
public:
  NodeDemo()
  : Node("node_demo")
  {
  }
};

This gives the class access to ROS 2 node functionality such as:

publishers

subscribers

timers

parameters

services

actions

logging

The constructor:

Node("node_demo")

sets the ROS 2 node name to:

/node_demo

2. Initializing ROS 2

Every normal ROS 2 C++ executable starts by initializing ROS 2:

rclcpp::init(argc, argv);

The node is then created:

auto node = std::make_shared<NodeDemo>();

and passed to the ROS 2 executor:

rclcpp::spin(node);

Finally, ROS 2 is shut down:

rclcpp::shutdown();

The basic lifecycle is:

rclcpp::init()
      ↓
Create node
      ↓
rclcpp::spin()
      ↓
Process callbacks/events
      ↓
rclcpp::shutdown()

3. ROS 2 Logging

The node uses the ROS 2 logging system:

RCLCPP_INFO(
  this->get_logger(),
  "Node has started!"
);

The logger belongs to the node and produces output similar to:

[INFO] [node_demo]: Node has started!

Logging is preferable to using std::cout for normal ROS 2 node
diagnostics because it integrates with the ROS 2 logging system.

4. Timers

A ROS 2 timer allows a callback to execute periodically.

The timer is created with:

timer_ = this->create_wall_timer(
  timer_duration,
  std::bind(&NodeDemo::timer_callback, this)
);

The callback is:

void timer_callback()
{
  RCLCPP_INFO(
    this->get_logger(),
    "Timer callback executed! Period: %.2f s",
    timer_period_
  );
}

The timer is stored as:

rclcpp::TimerBase::SharedPtr timer_;

Why store the timer?

The timer object must remain alive while the node is running. Making it
a class member keeps ownership of the timer.

5. ROS 2 Parameters

The timer period is configurable through a ROS 2 parameter:

this->declare_parameter<double>("timer_period", 1.0);

This declares:

timer_period

with a default value of:

1.0

and type:

double

The initial value is read with:

timer_period_ =
  this->get_parameter("timer_period").as_double();

The parameter can be inspected from another terminal:

ros2 param get /node_demo timer_period

6. Dynamic Parameter Updates

The important feature in this chapter is that the timer period can be
changed while the node is running.

A parameter callback is registered:

parameter_callback_handle_ =
  this->add_on_set_parameters_callback(
  std::bind(
    &NodeDemo::parameter_callback,
    this,
    std::placeholders::_1
  ));

The callback receives the parameters being changed.

rcl_interfaces::msg::SetParametersResult parameter_callback(
  const std::vector<rclcpp::Parameter> & parameters)

When timer_period changes, the node:

Reads the new value

Validates it

Stores it

Cancels the old timer

Creates a new timer using the new period

Conceptually:

ros2 param set
       │
       ▼
Parameter callback
       │
       ▼
Validate value
       │
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

7. Parameter Validation

The node prevents invalid timer periods:

if (new_period <= 0.0) {
  result.successful = false;
  result.reason = "timer_period must be greater than 0";
  return result;
}

Therefore:

ros2 param set /node_demo timer_period 0

is rejected.

Negative values are also rejected:

ros2 param set /node_demo timer_period -1.0

This demonstrates an important robotics software principle:

Validate configuration before allowing it to affect runtime behavior.

8. Parameter Types

ROS 2 parameters are strongly typed.

The parameter was declared as:

this->declare_parameter<double>("timer_period", 1.0);

Therefore, use a floating-point value when changing it:

ros2 param set /node_demo timer_period 2.0

This works.

But:

ros2 param set /node_demo timer_period 1

can fail because 1 is interpreted as an integer while the parameter
expects a double.

Use:

ros2 param set /node_demo timer_period 1.0

instead.

9. Testing Dynamic Behavior

Start the node:

ros2 run node_demo_pkg node_demo

The default period is one second.

Change it to two seconds:

ros2 param set /node_demo timer_period 2.0

Change it to half a second:

ros2 param set /node_demo timer_period 0.5

Try an invalid value:

ros2 param set /node_demo timer_period 0

Expected result:

Set parameter failed: timer_period must be greater than 0

10. Important ROS 2 Mental Model

This chapter demonstrates several core ROS 2 concepts:

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

The node itself is the main execution unit.

Timers and parameter callbacks are event sources processed by the ROS 2
executor.

11. Build and Test

Build the package:

cd ~/ros2_ws

CC=gcc-14 CXX=g++-14 colcon build \
  --packages-select node_demo_pkg

Source the workspace:

source install/setup.zsh

Run tests:

colcon test --packages-select node_demo_pkg

View detailed results:

colcon test-result --verbose

Expected result:

Summary: 8 tests, 0 errors, 0 failures, 1 skipped

What I Learned

After completing this chapter, I can:

Create a ROS 2 C++ node

Use rclcpp

Build class-based nodes

Create periodic timers

Use ROS 2 parameters

Inspect parameters from the CLI

Modify parameters at runtime

Validate parameter values

Dynamically recreate a timer

Build and test a ROS 2 package with colcon

Next Chapter

The next chapter introduces ROS 2 Services.

The communication model changes from:

Publisher → Topic → Subscriber

to:

Client → Request → Service Server
Client ← Response ← Service Server

This will introduce request/response communication in ROS 2.
