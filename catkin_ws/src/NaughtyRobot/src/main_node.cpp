#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/MagneticField.h>
#include <sensor_msgs/Joy.h>

#define GPS_RES 20

#define BUT_X 0
#define BUT_SQ 3
#define PD_L 2
#define PD_R 5
#define PD_LAXES_F 1
#define PD_LAXES_S 0

enum MainState_t
{
    MANUAL,
    LOCALISE,
    DRIVE_TOW,
    DRIVE_CONE,
    IMAGE_CONE,
    SEARCH_BUCKET,
    IMAGE_BUCKET,
};
enum LocaliseState_t
{
    GET_POS1,
    GET_POS2,
    GET_HEADING,
    INACTIVE
};

sensor_msgs::Joy joyState;
bool deadmanOff = false;
float joyForward = 0.0;
float joySide = 0.0; 

MainState_t mainState;
MainState_t prevState;
LocaliseState_t localiseState;

bool getGpsCoords = false;
sensor_msgs::NavSatFix tempPos;    // Used for localisation.
sensor_msgs::NavSatFix currentPos; // current position.
sensor_msgs::NavSatFix livePos;
double heading;                                // Current heading.
sensor_msgs::NavSatFix collectionArr[GPS_RES]; // Used for localisation.
uint8_t glbGpsCounter = GPS_RES;

geometry_msgs::Twist vel;     // This is dynamic, change this.
geometry_msgs::Twist stopVel; // Use for stopping.

sensor_msgs::NavSatFix GetMedianValue(sensor_msgs::NavSatFix *arr)
{
    sensor_msgs::NavSatFix _tempArray[GPS_RES];
    double lowestVal = DBL_MAX;
    double lastValue = 0.0;

    // First reorder latitude.

    for (int i = 0; i < GPS_RES; i++)
    {
        for (int j = 0; j < GPS_RES; j++)
        {
            // ROS_INFO("DBG Latitude: %.9f, Longitude: %.9f", arr[j].latitude,arr[j].longitude);

            if ((abs(arr[j].latitude) < lowestVal) && (abs(arr[j].latitude) > lastValue))
            {
                _tempArray[i].latitude = arr[j].latitude;
                lowestVal = abs(arr[j].latitude);
                // ROS_INFO("DBG Latitude: %.9f, Longitude: %.9f", arr[j].latitude,arr[j].longitude);
            }
        }
        lowestVal = DBL_MAX;
        lastValue = abs(_tempArray[i].latitude);
    }

    lowestVal = DBL_MAX;
    lastValue = 0.0;

    for (int i = 0; i < GPS_RES; i++)
    {
        for (int j = 0; j < GPS_RES; j++)
        {
            // ROS_INFO("DBG Latitude: %.9f, Longitude: %.9f", arr[j].latitude,arr[j].longitude);

            if (abs(arr[j].longitude) < lowestVal && abs(arr[j].longitude) > lastValue)
            {
                _tempArray[i].longitude = arr[j].longitude;
                lowestVal = abs(arr[j].longitude);
            }
        }
        lowestVal = DBL_MAX;
        lastValue = abs(_tempArray[i].longitude);
    }

    sensor_msgs::NavSatFix returnVal;
    returnVal.latitude = _tempArray[GPS_RES / 2].latitude;
    returnVal.longitude = _tempArray[GPS_RES / 2].longitude;

    ROS_INFO("MFFF Latitude: %.9f, Longitude: %.9f", returnVal.latitude, returnVal.longitude);

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
    return atan2(currentPos.latitude - startPos.latitude, currentPos.longitude - startPos.longitude);
}

void GpsCallBack(const sensor_msgs::NavSatFixConstPtr &msg)
{
    livePos = *msg;

    if (getGpsCoords)
    {
        // ROS_INFO("GPS Latitude: %.9f, Longitude: %.9f", msg->latitude,msg->longitude);

        if (--glbGpsCounter > 0)
        {
            collectionArr[glbGpsCounter] = *msg;

            ROS_INFO("CA Latitude: %.9f, Longitude: %.9f", collectionArr[glbGpsCounter].latitude, collectionArr[glbGpsCounter].longitude);
        }
        else
        {
            collectionArr[0] = *msg;
            glbGpsCounter = GPS_RES;
            getGpsCoords = false;
        }
    }
}

template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

void ImuCallBack(const sensor_msgs::MagneticFieldConstPtr &msg)
{
    float heading = atan2(msg->magnetic_field.y, msg->magnetic_field.x);
    ROS_INFO("Heading: %.9f", heading * 180 / M_PI);
}

void Rotate(double angle, ros::Publisher *pcmdVelPub, ros::Rate *prate, double speed = 0.5)
{
    double timeToWait = double(abs(angle / speed));
    geometry_msgs::Twist vel;
    vel.linear.x = 0;
    vel.angular.z = sgn(angle) * speed;
    ros::Time startTime = ros::Time::now();
    pcmdVelPub->publish(vel);
    while (ros::ok() && ros::Time::now() - startTime < ros::Duration(timeToWait))
    {
        pcmdVelPub->publish(vel);
        if(mainState==MANUAL)
        {
            pcmdVelPub->publish(stopVel);
            return;
        }
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
        pcmdVelPub->publish(vel);
        if(mainState==MANUAL)
        {
            pcmdVelPub->publish(stopVel);
            return;
        }
        ros::spinOnce();
        prate->sleep();
    }
    vel.angular.z = 0;
    pcmdVelPub->publish(vel);
}

/**
 * @brief Button xl
 *
 * @param msg
 */
void JoyCallBack(const sensor_msgs::JoyConstPtr &msg)
{
    if (mainState != MANUAL)
    {
        if (msg->axes[PD_L] > 0 && msg->axes[PD_R] > 0)
        {
            // deadmans switch
            vel.linear.x = 0;
            vel.angular.z = 0;
            deadmanOff = true;
            
            throw std::runtime_error("Deadman switch triggered... Ending program.");
            return;
        }
        if (msg->buttons[BUT_SQ] == 1)
        {
            mainState = MANUAL;
            ROS_INFO("Moving to manual mode");
            return;
        }
    }
    else
    {
        if (msg->buttons[BUT_X] == 1)
        {
            mainState = LOCALISE;
            localiseState = GET_POS1;
            ROS_INFO("Moving to autonomous program");
            return;
        }
        joyForward = msg->axes[PD_LAXES_F];
        joySide = msg->axes[PD_LAXES_S];

    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "Main_Node");
    ros::NodeHandle nh;
    ros::Publisher cmdVelPub = nh.advertise<geometry_msgs::Twist>("RosAria/cmd_vel", 10);
    ros::Subscriber gpsSub = nh.subscribe<sensor_msgs::NavSatFix>("fix", 10, GpsCallBack);
    ros::Subscriber imuMagSub = nh.subscribe<sensor_msgs::MagneticField>("imu/mag", 10, ImuCallBack);
    ros::Subscriber joySub = nh.subscribe<sensor_msgs::Joy>("joy", 5, JoyCallBack);

    ROS_INFO("here 1");

    stopVel.linear.x = 0;
    stopVel.angular.z = 0;

    cmdVelPub.publish(stopVel);

    ROS_INFO("here 2");



    ros::Rate rate(20);

    mainState = MANUAL;
    localiseState = INACTIVE;

    while (ros::ok())
    {
        if (deadmanOff)
        {
            cmdVelPub.publish(stopVel);
            return 0;
        }
        switch (mainState)
        {
        case MANUAL:
            ROS_INFO("here 3");

            vel.linear.x = joyForward;
            vel.angular.z = 0.5 * joySide;
            ROS_INFO("here 4");

            cmdVelPub.publish(vel);

            break;
        case LOCALISE:
            switch (localiseState)
            {
            case GET_POS1:
                getGpsCoords = true;

                while (getGpsCoords && ros::ok())
                {
                    ros::spinOnce();
                    rate.sleep();
                }
                tempPos = GetMedianValue(collectionArr);
                ROS_INFO("GPS Latitude: %.9f, Longitude: %.9f", tempPos.latitude, tempPos.longitude);
                localiseState = GET_POS2;
                Drive(20.0, &cmdVelPub, &rate);

                break;

            case GET_POS2:
                getGpsCoords = true;

                while (getGpsCoords && ros::ok())
                {
                    cmdVelPub.publish(vel);
                    ros::spinOnce();
                    rate.sleep();
                }
                currentPos = GetMedianValue(collectionArr);
                ROS_INFO("GPS Latitude: %.9f, Longitude: %.9f", currentPos.latitude, currentPos.longitude);

                localiseState = GET_HEADING;
                // Drive(5.0,&cmdVelPub,&rate);

                break;
            case GET_HEADING:
                cmdVelPub.publish(vel);

                double heading = GetHeading(tempPos, currentPos);
                ROS_INFO("The heading is: %.9f", heading * 180 / M_PI);
                localiseState = INACTIVE;
                mainState = DRIVE_TOW;
                break;
            }
        }

        ros::spinOnce();
        rate.sleep();
    }

    // ROS_INFO("HERE");

    return 0;
}
