#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/MagneticField.h>

uint8_t i = 0;

sensor_msgs::NavSatFix prevPos;

void GpsCallBack(const sensor_msgs::NavSatFixConstPtr& msg)
{
    ROS_INFO("Latitude %d: %.9f",i,msg->latitude);
    ROS_INFO("Longitude %d: %.9f",i++,msg->longitude);

	float heading = atan2(msg->latitude-prevPos.latitude,msg->longitude-prevPos.longitude)*180/M_PI;
	ROS_INFO("Heading from GPS: %.9f",heading*180/M_PI);
	prevPos = *msg;
}

void ImuCallBack(const sensor_msgs::MagneticFieldConstPtr& msg)
{
    float heading = atan2(msg->magnetic_field.y,msg->magnetic_field.x);
    ROS_INFO("Heading: %.9f", heading*180/M_PI);
}

/**
 * @brief Blocking rotation
 * 
 * @param angle the angle in radians
 * @param speed speed in rads^-1
 * @param pcmdVelPub pointer to the publisher to the cmd_vel topic
 * @param prate pointer to the rate object
 */
void Rotate(float angle, ros::Publisher *pcmdVelPub, ros::Rate *prate ,float speed = 0.5)
{
    double timeToWait = double(angle/speed);
    geometry_msgs::Twist vel;
    vel.linear.x = 0;
    vel.angular.z = speed;
    ros::Time startTime = ros::Time::now();
    pcmdVelPub->publish(vel);
    while(ros::Time::now()-startTime<ros::Duration(timeToWait))
    {
        ros::spinOnce();
        prate->sleep();
    }
    vel.angular.z = 0;
    pcmdVelPub->publish(vel);
}



int main(int argc, char** argv)
{
     ros::init(argc, argv, "Main_Node");
    ros::NodeHandle nh;
ros::Publisher cmdVelPub = nh.advertise<geometry_msgs::Twist>("RosAria/cmd_vel",10);
    ros::Subscriber gpsSub = nh.subscribe<sensor_msgs::NavSatFix>("fix",10,GpsCallBack);
    ros::Subscriber imuMagSub = nh.subscribe<sensor_msgs::MagneticField>("imu/mag",10,ImuCallBack);

    geometry_msgs::Twist vel;
    vel.linear.x = 1.0;

//    ros::Time startTime = ros::Time::now();

    uint8_t state = 0;

    ros::Rate rate(10);

	prevPos.latitude = 0;
	prevPos.longitude = 0;
    double timeToWait = M_PI/0.1;
        vel.linear.x = 0;
        vel.angular.z = 0.1;
        ros::Time startTime = ros::Time::now();

    while(ros::ok())
    {

        // switch(state)
        // {
        //     case 0:
        //         if(ros::Time::now()-startTime > ros::Duration(5.0))
        //         {
        //             state = 1;
        //             vel.linear.x = 0.0;
        //             vel.angular.z = 0.0;
        //             cmdVelPub.publish(vel);
        //             startTime = ros::Time::now();
        //             break;
        //         }
        //         else
        //         {
        //             vel.linear.x = 0.5;
        //         vel.angular.z = 0.0;
        //         cmdVelPub.publish(vel);
        //         }break;
        //     case 1:
        //         if(ros::Time::now()-startTime > ros::Duration(5.0))
        //         {
        //             state = 2;
        //             vel.linear.x = 0.0;
        //             vel.angular.z = 0.0;
        //             cmdVelPub.publish(vel);
        //             startTime = ros::Time::now();
        //             break;
        //         }
        //         else
        //         {
        //             vel.linear.x = 0.5;
        //             vel.angular.z = -0.1;
        //             cmdVelPub.publish(vel);
        //         }
        //         break;
        //     case 2:
        //         if(ros::Time::now()-startTime > ros::Duration(5.0))
        //         {
        //             state = 3;
        //             vel.linear.x = 0.0;
        //             vel.angular.z = 0.0;
        //             cmdVelPub.publish(vel);
        //             // return 0;
        //         }
        //         else
        //         {
        //             vel.linear.x = 0.5;
        //             vel.angular.z = 0;
        //             cmdVelPub.publish(vel);
        //         }
        //         break;
                
        // Rotate(M_PI,&cmdVelPub,&rate);

        
        cmdVelPub.publish(vel);
        if(ros::Time::now()-startTime<ros::Duration(timeToWait))
        {
            vel.angular.z = 0.1;
            cmdVelPub.publish(vel);
        }
        else
        {
            vel.angular.z = 0;
            cmdVelPub.publish(vel);
        ros::spinOnce();
        rate.sleep();
}    }
	
	// ROS_INFO("HERE");
	
        

    


    return 0;

}
