#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>

uint8_t i = 0;

void GpsCallBack(const sensor_msgs::NavSatFixConstPtr& msg)
{
    ROS_INFO("Latitude %d: %.9f",i,msg->latitude);
    ROS_INFO("Longitude %d: %.9f",i++,msg->longitude);
}

int main(int argc, char** argv)
{
     ros::init(argc, argv, "Main_Node");
    ros::NodeHandle nh;
//    ros::Rate rate(20);

   ros::Publisher cmdVelPub = nh.advertise<geometry_msgs::Twist>("RosAria/cmd_vel",10);
    ros::Subscriber gpsSub = nh.subscribe<sensor_msgs::NavSatFix>("fix",10,GpsCallBack);

    geometry_msgs::Twist vel;
    vel.linear.x = 1.0;

    ros::Time startTime = ros::Time::now();

    uint8_t state = 0;

    ros::Rate rate(10);

    while(ros::ok())
    {

        switch(state)
        {
            case 0:
                if(ros::Time::now()-startTime > ros::Duration(5.0))
                {
                    state = 1;
                    vel.linear.x = 0.0;
                    vel.angular.z = 0.0;
                    cmdVelPub.publish(vel);
                    startTime = ros::Time::now();
                    break;
                }
                else
                {
                    vel.linear.x = 0.5;
                vel.angular.z = 0.0;
                cmdVelPub.publish(vel);
                }break;
            case 1:
                if(ros::Time::now()-startTime > ros::Duration(5.0))
                {
                    state = 2;
                    vel.linear.x = 0.0;
                    vel.angular.z = 0.0;
                    cmdVelPub.publish(vel);
                    startTime = ros::Time::now();
                    break;
                }
                else
                {
                    vel.linear.x = 0.5;
                    vel.angular.z = -0.1;
                    cmdVelPub.publish(vel);
                }
                break;
            case 2:
                if(ros::Time::now()-startTime > ros::Duration(5.0))
                {
                    state = 3;
                    vel.linear.x = 0.0;
                    vel.angular.z = 0.0;
                    cmdVelPub.publish(vel);
                    return 0;
                }
                else
                {
                    vel.linear.x = 0.5;
                    vel.angular.z = 0;
                    cmdVelPub.publish(vel);
                }
                break;
                
        }

        ros::spinOnce();
        rate.sleep();

    }


    return 0;

}
