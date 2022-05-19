#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/MagneticField.h>
#include <sensor_msgs/Image.h>




#define IMG_HEIGHT 720
#define IMG_WIDTH 1280


static const int imgSize = IMG_HEIGHT*IMG_WIDTH;

void DepthCB(const sensor_msgs::ImageConstPtr &msg)
{
    int count = 0;
    long long int total = 0;
    
    for(int i = 0; i < msg->height; i++)
    {
        for(int j = 0; j < msg->width; j++)
        {
            // std::cout << (int)msg->data[i*msg->width + j]<< ","; 
            // std :: cout << msg->height << "  " << msg->width << "\n";

            if(msg->data[i*IMG_WIDTH + j] < 200)
            {
                count++;
            }
            total+=msg->data[i*IMG_WIDTH + j];
        }
    }
    // ROS_INFO("Min value is: %d", minVal);
    float percentage = (float)count/imgSize;
    float average  = (float)total/imgSize;
    printf("Percentage of image too close %.9f\n",percentage);
    printf("Average depth: %.9f\n",average);
    
    // std :: cout << "\n";
}


int main(int argc, char **argv)
{

    ros::init(argc, argv, "Main_Node");
    ros::NodeHandle nh;
    ros::Publisher cmdVelPub = nh.advertise<geometry_msgs::Twist>("RosAria/cmd_vel", 10);
    ros::Subscriber depthSub = nh.subscribe<sensor_msgs::Image>("rgb_stereo_publisher/stereo/depth",1,DepthCB);
    ros::Rate rate(10);


    geometry_msgs::Twist vel;
    vel.linear.x = 0.5;
    vel.angular.z = 0;

    while(ros::ok())
    {
        ros::spinOnce();
        rate.sleep();
    }



}