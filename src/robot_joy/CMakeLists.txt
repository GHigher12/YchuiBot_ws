cmake_minimum_required(VERSION 2.8.3)
project(robot_joy)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  roscpp
  rospy
  sensor_msgs
)

catkin_package(
 INCLUDE_DIRS include
 LIBRARIES robot_joy
  CATKIN_DEPENDS geometry_msgs roscpp rospy
#  DEPENDS system_lib
)


include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

# add_library(${PROJECT_NAME} src/robot_joy.cpp)
# target_link_libraries(${PROJECT_NAME}
#   ${catkin_LIBRARIES}
# )
# add_executable(${PROJECT_NAME}_node src/robot_joy_node.cpp)

# target_link_libraries(${PROJECT_NAME}_node
#   ${catkin_LIBRARIES}
#   ${PROJECT_NAME}
# )
add_executable(turtle_teleop_joy src/turtle_teleop_joy.cpp)
target_link_libraries(turtle_teleop_joy ${catkin_LIBRARIES})


add_executable(robot_teleop_joy src/robot_teleop_joy.cpp)
target_link_libraries(robot_teleop_joy ${catkin_LIBRARIES})

# install(TARGETS ${PROJECT_NAME}_node
#  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )
# install(TARGETS ${PROJECT_NAME}
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
# )
# install(DIRECTORY launch
#  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )
install(FILES
  readme.md
  # myfile2
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)