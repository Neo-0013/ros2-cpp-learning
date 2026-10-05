cmake_minimum_required(VERSION 3.8)
project(node_demo_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(node_demo src/node_demo.cpp)

ament_target_dependencies(node_demo rclcpp)

install(TARGETS
  node_demo
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
