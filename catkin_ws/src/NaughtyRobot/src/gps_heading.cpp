#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/MagneticField.h>


#define GPS_RES 5


sensor_msgs::NavSatFix currentPos;
sensor_msgs::NavSatFix prevPos;

bool done;

sensor_msgs::NavSatFix _tempArray[GPS_RES]


void Reorder()

