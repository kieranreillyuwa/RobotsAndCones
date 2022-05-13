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
  CATKIN_DEPENDS roscpp rospy sensor_msgs geometry_msgs tf
)


include_directories(include ${catkin_INCLUDE_DIRS})


add_executable(main_node src/main_node.cpp)
target_link_libraries(main_node ${catkin_LIBRARIES})
add_dependencies(main_node ${catkin_EXPORTED_TARGETS})

install(TARGETS
  main_node
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
 )

 add_executable(gps_heading src/gps_heading.cpp)
target_link_libraries(gps_heading ${catkin_LIBRARIES})
add_dependencies(gps_heading ${catkin_EXPORTED_TARGETS})

install(TARGETS
  gps_heading
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
 )

 add_executable(gps_testing src/gps_testing.cpp)
target_link_libraries(gps_testing ${catkin_LIBRARIES})
add_dependencies(gps_testing ${catkin_EXPORTED_TARGETS})

install(TARGETS
  gps_testing
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
 )

  add_executable(obstacle_avoidance src/obstacle_avoidance.cpp)
target_link_libraries(obstacle_avoidance ${catkin_LIBRARIES})
add_dependencies(obstacle_avoidance ${catkin_EXPORTED_TARGETS})

install(TARGETS
  obstacle_avoidance
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
 )