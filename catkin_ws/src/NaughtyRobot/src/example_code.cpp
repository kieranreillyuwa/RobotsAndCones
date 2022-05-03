/**
 * @file kieran_sim.cpp
 * @author Kieran Reilly (22484032@student.uwa.edu.au)
 * @brief THIS IS CODE I USE IN MY THESIS FOR TESTING 
 * @version 0.1
 * @date 2022-04-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/PositionTarget.h>
#include <mavros_msgs/Thrust.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/CommandBool.h>

// --- GLOBAL CONSTANTS --- //






// --- GLOBAL NON-CONSTANT VARIABLES --- //

mavros_msgs::State current_state;
geometry_msgs::PoseStamped localPos;
geometry_msgs::PoseStamped targetPos;

// ----------------------------------- //

void PrintPosition(geometry_msgs::PoseStamped *pposition, const char* positionID)
{
    ROS_INFO("%s position: x = %.2f  y = %.2f  z = %.2f",positionID, pposition->pose.position.x, pposition->pose.position.y, pposition->pose.position.z);
}

/**
 * @brief Testing Callback function which will print current location.
 *
 * @param msg The incoming local position message.
 */
void CallbackFunction(const geometry_msgs::PoseStamped::ConstPtr &msg)
{
    // ROS_INFO("Local Position: x = %.2f  y = %.2f  z = %.2f             ", msg->pose.position.x, msg->pose.position.y, msg->pose.position.z);
    localPos = *msg;
}

void StateCallback(const mavros_msgs::State::ConstPtr &msg)
{
    current_state = *msg;
    // ROS_INFO("STATE");
}

void TargetCallback(const geometry_msgs::PoseStamped::ConstPtr &_targetPosition)
{
    targetPos = *_targetPosition;
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "Kieran_Node");
    ros::NodeHandle nh;
    ros::Rate rate(20);

    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>("mavros/state", 10, StateCallback);
    ros::Publisher targetPositionPub = nh.advertise<geometry_msgs::PoseStamped>("mavros/setpoint_position/local", 5);
    ros::Subscriber localPositionSub = nh.subscribe<geometry_msgs::PoseStamped>("mavros/local_position/pose", 10, CallbackFunction);
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");
    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>("mavros/cmd/arming");

    ros::Subscriber targetSub = nh.subscribe<geometry_msgs::PoseStamped>("target",10,TargetCallback);

    geometry_msgs::PoseStamped initialPose;
    initialPose.pose.position.x = 0;
    initialPose.pose.position.y = 0;
    initialPose.pose.position.z = 2;

    initialPose.header.stamp = ros::Time::now();

    uint8_t counter = 0;

    geometry_msgs::PoseStamped target;
    target.pose.position.x = 0;
    target.pose.position.y = 0;
    target.pose.position.z = 2;

    while (ros::ok() && !current_state.connected)
    {
        ros::spinOnce();
        rate.sleep();

        ROS_INFO("No Connection");
    }

    for (int i = 100; ros::ok() && i > 0; --i)
    {
        targetPositionPub.publish(target);
        ros::spinOnce();
        rate.sleep();
    }

    mavros_msgs::SetMode offb_set_mode;
    offb_set_mode.request.custom_mode = "OFFBOARD";

    mavros_msgs::CommandBool arm_cmd;
    arm_cmd.request.value = true;

    ros::Time lastRequest = ros::Time::now();

    bool offboardArming = false;

    while (ros::ok() && !offboardArming)
    {
        // ROS_INFO("Got here 1");
        if (current_state.mode != "OFFBOARD" &&
            (ros::Time::now() - lastRequest > ros::Duration(5.0)))
        {
            if (set_mode_client.call(offb_set_mode) &&
                offb_set_mode.response.mode_sent)
            {
                ROS_INFO("Offboard enabled");
                // ROS_INFO("%s", current_state.mode.c_str());
            }
            else
            {
                ROS_INFO("Failed to go into offboard mode.");
            }
            lastRequest = ros::Time::now();
        }
        else
        {
            if (!current_state.armed &&
                (ros::Time::now() - lastRequest > ros::Duration(5.0)))
            {
                if (arming_client.call(arm_cmd) &&
                    arm_cmd.response.success)
                {
                    ROS_INFO("Vehicle armed");
                    offboardArming = true;
                }
                else
                {
                    ROS_INFO("Vehicle not arming.");

                    ROS_INFO("%d", arming_client.call(arm_cmd));
                    ROS_INFO("%d", arm_cmd.response.success);
                }
                lastRequest = ros::Time::now();
            }
        }
        targetPositionPub.publish(initialPose);
        ros::spinOnce();
        rate.sleep();
    }

    ros::Rate rate2(10);
    while (ros::ok())
    {
        ROS_INFO("Local Position: x = %.2f  y = %.2f  z = %.2f             ", localPos.pose.position.x, localPos.pose.position.y, localPos.pose.position.z);
        ROS_INFO("Target Position: x = %.2f  y = %.2f  z = %.2f             ", targetPos.pose.position.x, targetPos.pose.position.y, targetPos.pose.position.z);

        // ROS_INFO("Got here 2");
        // target.pose.position.x = 3 * counter;
        target.pose.position.x = 0;
        targetPositionPub.publish(target);
        counter++;

        ros::spinOnce();
        rate2.sleep();
    }

    return 0;
}
