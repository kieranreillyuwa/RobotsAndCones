cmake_minimum_required(VERSION 2.8.3)
project(NaughtyRobot)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  sensor_msgs
  geometry_msgs
  tf
)

catkin_package(
  CATKIN_DEPENDS roscpp rospy sensor_msgs geometry_msgs tf mavros
)


include_directories(include ${catkin_INCLUDE_DIRS})

