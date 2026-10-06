#include <chrono>
#include <vector>

#include "rcl_interfaces/msg/set_parameters_result.hpp"
#include "rclcpp/rclcpp.hpp"

class NodeDemo : public rclcpp::Node
{
public:
  NodeDemo()
  : Node("node_demo")
  {
    this->declare_parameter<double>("timer_period", 1.0);

    timer_period_ = this->get_parameter("timer_period").as_double();

    RCLCPP_INFO(
      this->get_logger(),
      "Node has started!"
    );

    RCLCPP_INFO(
      this->get_logger(),
      "Timer period: %.2f seconds",
      timer_period_
    );

    create_timer();

    parameter_callback_handle_ =
      this->add_on_set_parameters_callback(
      std::bind(
        &NodeDemo::parameter_callback,
        this,
        std::placeholders::_1
      ));
  }

private:
  void create_timer()
  {
    const auto timer_duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::duration<double>(timer_period_));

    timer_ = this->create_wall_timer(
      timer_duration,
      std::bind(&NodeDemo::timer_callback, this)
    );
  }

  void timer_callback()
  {
    RCLCPP_INFO(
      this->get_logger(),
      "Timer callback executed! Period: %.2f s",
      timer_period_
    );
  }

  rcl_interfaces::msg::SetParametersResult parameter_callback(
    const std::vector<rclcpp::Parameter> & parameters)
  {
    rcl_interfaces::msg::SetParametersResult result;
    result.successful = true;
    result.reason = "success";

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

      // Replace the existing timer with a new timer.
      timer_->cancel();
      create_timer();

      RCLCPP_INFO(
        this->get_logger(),
        "Timer period changed to %.2f seconds",
        timer_period_
      );
    }

    return result;
  }

  double timer_period_;

  rclcpp::TimerBase::SharedPtr timer_;

  rclcpp::node_interfaces::OnSetParametersCallbackHandle::SharedPtr
    parameter_callback_handle_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<NodeDemo>();

  rclcpp::spin(node);

  rclcpp::shutdown();

  return 0;
}
