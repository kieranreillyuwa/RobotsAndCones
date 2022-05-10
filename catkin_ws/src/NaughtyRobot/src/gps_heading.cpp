#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/MagneticField.h>


#define GPS_RES 5

sensor_msgs::NavSatFix livePos;
sensor_msgs::NavSatFix currentPos;
sensor_msgs::NavSatFix startPos;

bool getGpsCoords = false;
uint8_t glbGpsCounter = GPS_RES;

sensor_msgs::NavSatFix collectionArr[GPS_RES];





/**
 * @brief For an array of GPS_RES size, this will return the median gps coordinate. Good for filtering noise. 
 * 
 * @param arr The array of GPS messages.
 * @return sensor_msgs::NavSatFix The NavSatFix message with the relevant GPS coordinate. 
 */
sensor_msgs::NavSatFix GetMedianValue(sensor_msgs::NavSatFix *arr)
{
    sensor_msgs::NavSatFix _tempArray[GPS_RES];
    double lowestVal = DBL_MAX;
    double lastValue = 0.0;

    // First reorder latitude.

    for(int i = 0; i < GPS_RES; i++)
    {
        for(int j = 0; j < GPS_RES; j++)
        {
            if(abs(arr[j].latitude) < lowestVal && abs(arr[j].latitude) > lastValue)
            {
                _tempArray[i]. latitude = arr[j].latitude;
                lowestVal = abs(arr[j].latitude);
            }
        }
    }

    lowestVal = DBL_MAX;
    lastValue = 0.0;

    for(int i = 0; i < GPS_RES; i++)
    {
        for(int j = 0; j < GPS_RES; j++)
        {
            if(abs(arr[j].longitude) < lowestVal && abs(arr[j].longitude) > lastValue)
            {
                _tempArray[i]. latitude = arr[j].longitude;
                lowestVal = abs(arr[j].longitude);
            }
        }
    }

    sensor_msgs::NavSatFix returnVal;
    returnVal.latitude = _tempArray[GPS_RES/2].latitude;
    returnVal.longitude = _tempArray[GPS_RES/2].longitude;

    return returnVal;

}

/**
 * @brief Get the current heading after a driving of a certain distance. Used filtered GPS only. 
 * 
 * @param startPos The starting position.
 * @param currentPos The current position. 
 * @return The current heading in radians. 
 */
double GetHeading(sensor_msgs::NavSatFix startPos, sensor_msgs::NavSatFix currentPos)
{
    return atan2(currentPos.latitude - startPos.latitude,currentPos.longitude-startPos.longitude);
}


void GpsCallBack(const sensor_msgs::NavSatFixConstPtr &msg)
{
    livePos = *msg;

    if(getGpsCoords)
    {
        if(--glbGpsCounter > 0)
        {
            collectionArr[glbGpsCounter] = *msg;
        }
        else
        {
            collectionArr[0] = *msg;
            glbGpsCounter = GPS_RES;
            getGpsCoords = false;
        }
        
    } 
}

void Rotate(double angle, ros::Publisher *pcmdVelPub, ros::Rate *prate, double speed = 0.5)
{
    double timeToWait = double(angle / speed);
    geometry_msgs::Twist vel;
    vel.linear.x = 0;
    vel.angular.z = speed;
    ros::Time startTime = ros::Time::now();
    pcmdVelPub->publish(vel);
    while (ros::ok() && ros::Time::now() - startTime < ros::Duration(timeToWait))
    {
        pcmdVelPub->publish(vel);
        ros::spinOnce();
        prate->sleep();
    }
    vel.angular.z = 0;
    pcmdVelPub->publish(vel);
}

void Drive(double dist, ros::Publisher *pcmdVelPub, ros::Rate *prate, double speed = 1)
{
    double timeToWait = double(dist / speed);
    geometry_msgs::Twist vel;
    vel.linear.x = speed;
    vel.angular.z = 0;
    ros::Time startTime = ros::Time::now();
    pcmdVelPub->publish(vel);
    while (ros::ok() && ros::Time::now() - startTime < ros::Duration(timeToWait))
    {
        pcmdVelPub->publish(vel);
        ros::spinOnce();
        prate->sleep();
    }
    vel.angular.z = 0;
    pcmdVelPub->publish(vel);
}

enum State_t{GET_POS1,GET_POS2,GET_HEADING, DONE};

int main(int argc, char **argv)
{

    ros::init(argc, argv, "Gps_Test_Node");
    ros::NodeHandle nh;

    ros::Publisher cmdVelPub = nh.advertise<geometry_msgs::Twist>("RosAria/cmd_vel", 10);
    ros::Subscriber gpsSub = nh.subscribe<sensor_msgs::NavSatFix>("fix", 10, GpsCallBack);

    geometry_msgs::Twist vel;
    vel.linear.x = 0;
    vel.angular.z = 0;

    ros::Rate rate(10);

    State_t state = GET_POS1;

    while(ros::ok())
    {
        switch(state)
        {
            case GET_POS1:
                getGpsCoords = true;

                while(getGpsCoords && ros::ok())
                {
                    ros::spinOnce();
                    rate.sleep();
                }
                startPos = GetMedianValue(collectionArr);
                state = GET_POS2;
                Drive(5.0,&cmdVelPub,&rate);

                
                break;
            
            case GET_POS2:
                getGpsCoords = true;

                while(getGpsCoords && ros::ok())
                {
                    cmdVelPub.publish(vel);
                    ros::spinOnce();
                    rate.sleep();
                }
                currentPos = GetMedianValue(collectionArr);
                state = GET_HEADING;
                // Drive(5.0,&cmdVelPub,&rate);

                
                break;
            case GET_HEADING:
                    cmdVelPub.publish(vel);

                double heading = GetHeading(startPos,currentPos);
                ROS_INFO("The heading is: %.9f", heading);
                state = DONE;
                break;
        }





        ros::spinOnce();
        rate.sleep();
    }

    


}