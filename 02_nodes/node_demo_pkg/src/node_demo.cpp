#include "rclcpp/rclcpp.hpp"

int main(int argc, char * argv[])
{
    // Initialize ROS 2
    rclcpp::init(argc, argv);

    // Create our node
    auto node = rclcpp::Node::make_shared("node_demo");

    // Print a message
    RCLCPP_INFO(node->get_logger(), "Node has started!");

    // Keep the node alive
    rclcpp::spin(node);

    // Shutdown ROS 2
    rclcpp::shutdown();

    return 0;
}
