#include "rclcpp/rclcpp.hpp"

/**
 * @brief Entry point for the first ROS 2 C++ node.
 *
 * This program demonstrates the basic lifecycle of a ROS 2 node:
 * 1. Initialize ROS 2
 * 2. Create a node
 * 3. Log a message
 * 4. Keep the node alive
 * 5. Shut down ROS 2
 */
int main(int argc, char * argv[])
{
  // Initialize the ROS 2 communication system.
  rclcpp::init(argc, argv);

  // Create a ROS 2 node named "my_first_node".
  auto node = rclcpp::Node::make_shared("my_first_node");

  // Print an informational message using the node's logger.
  RCLCPP_INFO(
    node->get_logger(),
    "Hello ROS 2!"
  );

  // Keep the node alive and allow ROS 2 to process events.
  rclcpp::spin(node);

  // Shut down ROS 2 cleanly.
  rclcpp::shutdown();

  return 0;
}

