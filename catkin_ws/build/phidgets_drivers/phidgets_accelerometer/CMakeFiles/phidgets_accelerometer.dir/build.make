# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/netipc/RobotsAndCones/catkin_ws/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/netipc/RobotsAndCones/catkin_ws/build

# Include any dependencies generated for this target.
include phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/depend.make

# Include the progress variables for this target.
include phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/progress.make

# Include the compile flags for this target's objects.
include phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/flags.make

phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/src/accelerometer_ros_i.cpp.o: phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/flags.make
phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/src/accelerometer_ros_i.cpp.o: /home/netipc/RobotsAndCones/catkin_ws/src/phidgets_drivers/phidgets_accelerometer/src/accelerometer_ros_i.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/netipc/RobotsAndCones/catkin_ws/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/src/accelerometer_ros_i.cpp.o"
	cd /home/netipc/RobotsAndCones/catkin_ws/build/phidgets_drivers/phidgets_accelerometer && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/phidgets_accelerometer.dir/src/accelerometer_ros_i.cpp.o -c /home/netipc/RobotsAndCones/catkin_ws/src/phidgets_drivers/phidgets_accelerometer/src/accelerometer_ros_i.cpp

phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/src/accelerometer_ros_i.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/phidgets_accelerometer.dir/src/accelerometer_ros_i.cpp.i"
	cd /home/netipc/RobotsAndCones/catkin_ws/build/phidgets_drivers/phidgets_accelerometer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/netipc/RobotsAndCones/catkin_ws/src/phidgets_drivers/phidgets_accelerometer/src/accelerometer_ros_i.cpp > CMakeFiles/phidgets_accelerometer.dir/src/accelerometer_ros_i.cpp.i

phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/src/accelerometer_ros_i.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/phidgets_accelerometer.dir/src/accelerometer_ros_i.cpp.s"
	cd /home/netipc/RobotsAndCones/catkin_ws/build/phidgets_drivers/phidgets_accelerometer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/netipc/RobotsAndCones/catkin_ws/src/phidgets_drivers/phidgets_accelerometer/src/accelerometer_ros_i.cpp -o CMakeFiles/phidgets_accelerometer.dir/src/accelerometer_ros_i.cpp.s

# Object files for target phidgets_accelerometer
phidgets_accelerometer_OBJECTS = \
"CMakeFiles/phidgets_accelerometer.dir/src/accelerometer_ros_i.cpp.o"

# External object files for target phidgets_accelerometer
phidgets_accelerometer_EXTERNAL_OBJECTS =

/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/src/accelerometer_ros_i.cpp.o
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/build.make
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /opt/ros/noetic/lib/libnodeletlib.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /opt/ros/noetic/lib/libbondcpp.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libuuid.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /opt/ros/noetic/lib/libclass_loader.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libPocoFoundation.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libdl.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /opt/ros/noetic/lib/libroslib.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /opt/ros/noetic/lib/librospack.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libpython3.8.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libboost_program_options.so.1.71.0
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libtinyxml2.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_api.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidget22.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /opt/ros/noetic/lib/libroscpp.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libpthread.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libboost_chrono.so.1.71.0
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so.1.71.0
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /opt/ros/noetic/lib/librosconsole.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /opt/ros/noetic/lib/librosconsole_log4cxx.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /opt/ros/noetic/lib/librosconsole_backend_interface.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/liblog4cxx.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libboost_regex.so.1.71.0
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /opt/ros/noetic/lib/libxmlrpcpp.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /opt/ros/noetic/lib/libroscpp_serialization.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /opt/ros/noetic/lib/librostime.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libboost_date_time.so.1.71.0
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /opt/ros/noetic/lib/libcpp_common.so
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libboost_system.so.1.71.0
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libboost_thread.so.1.71.0
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: /usr/lib/x86_64-linux-gnu/libconsole_bridge.so.0.4
/home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so: phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/netipc/RobotsAndCones/catkin_ws/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library /home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so"
	cd /home/netipc/RobotsAndCones/catkin_ws/build/phidgets_drivers/phidgets_accelerometer && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/phidgets_accelerometer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/build: /home/netipc/RobotsAndCones/catkin_ws/devel/lib/libphidgets_accelerometer.so

.PHONY : phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/build

phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/clean:
	cd /home/netipc/RobotsAndCones/catkin_ws/build/phidgets_drivers/phidgets_accelerometer && $(CMAKE_COMMAND) -P CMakeFiles/phidgets_accelerometer.dir/cmake_clean.cmake
.PHONY : phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/clean

phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/depend:
	cd /home/netipc/RobotsAndCones/catkin_ws/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/netipc/RobotsAndCones/catkin_ws/src /home/netipc/RobotsAndCones/catkin_ws/src/phidgets_drivers/phidgets_accelerometer /home/netipc/RobotsAndCones/catkin_ws/build /home/netipc/RobotsAndCones/catkin_ws/build/phidgets_drivers/phidgets_accelerometer /home/netipc/RobotsAndCones/catkin_ws/build/phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : phidgets_drivers/phidgets_accelerometer/CMakeFiles/phidgets_accelerometer.dir/depend
