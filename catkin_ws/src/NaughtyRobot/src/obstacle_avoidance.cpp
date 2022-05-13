#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/MagneticField.h>
#include <sensor_msgs/Image.h>




#define IMG_HEIGHT 720
#define IMG_WIDTH 1280

uint8_t obstacleImage[IMG_HEIGHT * IMG_WIDTH];

void DepthCB(const sensor_msgs::ImageConstPtr &msg)
{
    uint8_t minVal = 0xFF;
    for(int i = 0; i < msg->height; i++)
    {
        for(int j = 0; j < msg->width; j++)
        {
            // std::cout << (int)msg->data[i*msg->width + j]<< ","; 
            // std :: cout << msg->height << "  " << msg->width << "\n";

            if(msg->data[i*IMG_WIDTH + j] < minVal && msg->data[i*IMG_WIDTH + j]>2)
            {
                minVal = msg->data[i*msg->width + j];
            }
        }
    }
    // ROS_INFO("Min value is: %d", minVal);
    std :: cout << "\n";
}


int main(int argc, char **argv)
{

    ros::init(argc, argv, "Main_Node");
    ros::NodeHandle nh;
    ros::Publisher cmdVelPub = nh.advertise<geometry_msgs::Twist>("RosAria/cmd_vel", 10);
    ros::Subscriber depthSub = nh.subscribe<sensor_msgs::Image>("stereo_publisher/stereo/depth",10,DepthCB);
    ros::Rate rate(1);


    geometry_msgs::Twist vel;
    vel.linear.x = 0.5;
    vel.angular.z = 0;

    while(ros::ok())
    {
        ros::spinOnce();
        rate.sleep();
    }



}