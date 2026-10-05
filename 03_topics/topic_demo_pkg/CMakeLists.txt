cmake_minimum_required(VERSION 3.8)
project(topic_demo_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(status_publisher src/publisher.cpp)
ament_target_dependencies(status_publisher
  rclcpp
  std_msgs
)

add_executable(status_subscriber src/subscriber.cpp)
ament_target_dependencies(status_subscriber
  rclcpp
  std_msgs
)

install(TARGETS
  status_publisher
  status_subscriber
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
