ROS 2 Commands --- 02_nodes

Command reference for the node_demo_pkg chapter.

Navigate to the Workspace

cd ~/ros2_ws

Source ROS 2:

source /opt/ros/jazzy/setup.zsh

Source the workspace:

source ~/ros2_ws/install/setup.zsh

Build

Build only this package:

CC=gcc-14 CXX=g++-14 colcon build \
  --packages-select node_demo_pkg

Build from the workspace:

cd ~/ros2_ws
colcon build

Run the Node

ros2 run node_demo_pkg node_demo

Expected node name:

/node_demo

Node Inspection

List running nodes:

ros2 node list

Inspect the node:

ros2 node info /node_demo

The node information includes ROS graph interfaces such as:

publishers

subscribers

service servers

service clients

action servers

action clients

Parameters

List all parameters:

ros2 param list /node_demo

Get the timer period:

ros2 param get /node_demo timer_period

Expected initial value:

Double value is: 1.0

Set a new period:

ros2 param set /node_demo timer_period 2.0

Set a faster period:

ros2 param set /node_demo timer_period 0.5

Reset to one second:

ros2 param set /node_demo timer_period 1.0

Parameter Type Example

The parameter is declared as:

declare_parameter<double>("timer_period", 1.0);

Therefore, use a floating-point value:

ros2 param set /node_demo timer_period 1.0

Instead of:

ros2 param set /node_demo timer_period 1

The second command may fail because the CLI interprets 1 as an
integer.

Parameter Validation Test

Invalid zero value:

ros2 param set /node_demo timer_period 0

Invalid negative value:

ros2 param set /node_demo timer_period -1.0

Both should be rejected because the node requires:

timer_period > 0

Runtime Dynamic Update Test

Terminal 1

Start the node:

ros2 run node_demo_pkg node_demo

You should see approximately one callback per second.

Terminal 2

Change the period:

ros2 param set /node_demo timer_period 2.0

The node should report:

Timer period changed to 2.00 seconds

Then change it again:

ros2 param set /node_demo timer_period 0.5

The callback should now execute approximately twice per second.

Tests

Run package tests:

colcon test --packages-select node_demo_pkg

Show detailed results:

colcon test-result --verbose

Expected result:

Summary: 8 tests, 0 errors, 0 failures, 1 skipped

Useful Package Commands

Find the package:

ros2 pkg list | grep node_demo_pkg

Show the package prefix:

ros2 pkg prefix node_demo_pkg

Show the executable:

ros2 pkg executables node_demo_pkg

Useful Linux/Git Commands

Check repository status:

cd ~/ros2-cpp-learning
git status

View recent commits:

git log --oneline --decorate -5

View changes:

git diff

Core Commands to Remember

ros2 run
    Run a ROS 2 executable

ros2 node list
    List running nodes

ros2 node info
    Inspect a node

ros2 param list
    List parameters

ros2 param get
    Read a parameter

ros2 param set
    Change a parameter

colcon build
    Build ROS 2 packages

colcon test
    Run package tests

colcon test-result --verbose
    Display test results
