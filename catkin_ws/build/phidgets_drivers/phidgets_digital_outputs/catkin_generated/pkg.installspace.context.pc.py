# generated from catkin/cmake/template/pkg.context.pc.in
CATKIN_PACKAGE_PREFIX = ""
PROJECT_PKG_CONFIG_INCLUDE_DIRS = "${prefix}/include".split(';') if "${prefix}/include" != "" else []
PROJECT_CATKIN_DEPENDS = "nodelet;phidgets_api;phidgets_msgs;roscpp;std_msgs".replace(';', ' ')
PKG_CONFIG_LIBRARIES_WITH_PREFIX = "-lphidgets_digital_outputs".split(';') if "-lphidgets_digital_outputs" != "" else []
PROJECT_NAME = "phidgets_digital_outputs"
PROJECT_SPACE_DIR = "/home/netipc/RobotsAndCones/catkin_ws/install"
PROJECT_VERSION = "1.0.5"