#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/MagneticField.h>
#include <sensor_msgs/Joy.h>
#include <std_msgs/Bool.h>
#include <std_msgs/UInt16MultiArray.h>
#include <sensor_msgs/Image.h>


#define WIDTH_IMAGE 1200
#define HEIGHT_IMAGE 720
#define OFFSET_IMAGE 200

#define GPS_RES 20
#define NUM_POINTS 10

#define BUT_X 0
#define BUT_SQ 3
#define PD_L 2
#define PD_R 5
#define PD_LAXES_F 1
#define PD_LAXES_S 0

#define IMG_ID 0
#define IMG_X 1
#define IMG_Y 2
#define IMG_W 3
#define IMG_H 4

#define IMG_SIZE_THRESH_W 60000
#define IMG_SIZE_THRESH_H 60000

#define CONE 0
#define BUCKET 1

static const uint16_t lowerThresh = WIDTH_IMAGE/3 + OFFSET_IMAGE;
static const uint16_t upperThresh = WIDTH_IMAGE - WIDTH_IMAGE/3 - OFFSET_IMAGE;

enum MainState_t
{
    MANUAL,
    LOCALISE,
    DRIVE_TOW,
    DRIVE_CONE,
    IMAGE_CONE,
    SEARCH_BUCKET,
    IMAGE_BUCKET,
    END_ROUTINE
};
enum LocaliseState_t
{
    GET_POS1,
    GET_POS2,
    GET_HEADING,
    INACTIVE
};

float joyForward = 0.0;
float joySide = 0.0; 

MainState_t mainState;
MainState_t prevState;
LocaliseState_t localiseState;

sensor_msgs::NavSatFix goalCoords[NUM_POINTS];
uint8_t numGpsPoints;
bool firstPoint = true;

bool getGpsCoords = false;
sensor_msgs::NavSatFix tempPos;    // Used for localisation.
sensor_msgs::NavSatFix currentPos; // current position.
sensor_msgs::NavSatFix livePos;
sensor_msgs::NavSatFix goalPos;
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


std_msgs::UInt16MultiArray toConeDriving;
std_msgs::UInt16MultiArray toBucket;
bool coneFound = false;
bool bucketFound = false;



void ImageStatusCallback(const std_msgs::UInt16MultiArrayConstPtr &msg)
{
    if(msg->data[IMG_ID] == CONE)
    {
        if(mainState == DRIVE_TOW || mainState== DRIVE_CONE || mainState == IMAGE_CONE)
        {
            toConeDriving= *msg;
            coneFound = true;
        }
    }
    else if(msg->data[IMG_ID]==BUCKET)
    {
        if(mainState == SEARCH_BUCKET || mainState == IMAGE_BUCKET)
        {
            toBucket = *msg;
            bucketFound = true;
        }
    }

}

static bool grabAnImage = false;
uint8_t imageCounter = 0;
sensor_msgs::Image keyImages[40];
void GetImageCallback(const sensor_msgs::ImageConstPtr &msg)
{
    if(grabAnImage)
    {
        keyImages[imageCounter++] = *msg;
        grabAnImage = false;
    }
}

static const float driveForwardTime = 1.0;
enum BucketSearch_t{SIDE_1,SIDE_2,SIDE_3,SIDE_4};
BucketSearch_t bucketSearch = SIDE_1;


bool getDistToPoint = false;
uint8_t distArr[40];
uint8_t distArrCount = 0;

void DepthImageCallback(const sensor_msgs::ImageConstPtr &msg)
{
    if(getDistToPoint)
    {
        distArr[distArrCount++] = msg->data[toBucket.data[IMG_X]*WIDTH_IMAGE+toBucket.data[IMG_Y]];
        getDistToPoint = false;
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
    ros::Publisher mainStatusPub = nh.advertise<std_msgs::Bool>("main_status",5);
    ros::Subscriber imageStatusSub = nh.subscribe<std_msgs::UInt16MultiArray>("image_status",10,ImageStatusCallback);
    ros::Subscriber imageSub = nh.subscribe<sensor_msgs::Image>("vision",5,GetImageCallback);
    ros::Subscriber depthImageSub = nh.subscribe<sensor_msgs::Image>("**INSERT**",10,DepthImageCallback);

    // ROS_INFO("here 1");

    stopVel.linear.x = 0;
    stopVel.angular.z = 0;

    cmdVelPub.publish(stopVel);

    // ROS_INFO("here 2");

    /** GET GPS COORDS HERE **/
    uint8_t c = 0;
    goalPos = goalCoords[c++];

    ros::Rate rate(20);

    mainState = MANUAL;
    localiseState = INACTIVE;

    uint8_t sideCount = 0;
            bool cantFind = false;
    double relHeading;
    double heading;
    while (ros::ok())
    {
        switch (mainState)
        {
        case MANUAL:
            // ROS_INFO("here 3");
            vel.linear.x = joyForward;
            vel.angular.z = 0.5 * joySide;
            // ROS_INFO("here 4");

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
                if(firstPoint)
                {
                    goalCoords[numGpsPoints++] = tempPos;
                    firstPoint = false;
                }
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

                heading = GetHeading(tempPos, currentPos);
                ROS_INFO("The heading is: %.9f", heading * 180 / M_PI);
                localiseState = INACTIVE;
                mainState = DRIVE_TOW;
                std_msgs::Bool startImgProcessing;
                startImgProcessing.data = 1;
                mainStatusPub.publish(startImgProcessing);
                break;
            }
        case DRIVE_TOW:
            Drive(10,&cmdVelPub,&rate); // just incase other cones are in the vision
            relHeading = GetHeading(currentPos,goalPos) - heading; // get the angle between
            if(relHeading>M_PI)
            {
                relHeading-=M_PI*2;
            }
            else if(relHeading<-M_PI)
            {
                relHeading+=M_PI*2;
            }
            Rotate(relHeading,&cmdVelPub,&rate); // turn to goal
            // should be facing goal now
            vel.linear.x = 1.0;
            vel.angular.z = 0;
            while(!coneFound && ros::ok()) // keep driving forward until cone is found
            {
                cmdVelPub.publish(vel);
                ros::spinOnce();
                rate.sleep();
            }
            cmdVelPub.publish(stopVel);
            mainState = DRIVE_CONE;
            break;

        case DRIVE_CONE:
            vel.linear.x = 0.5;
            if(toConeDriving.data[IMG_H] > IMG_SIZE_THRESH_H || toConeDriving.data[IMG_W] > IMG_SIZE_THRESH_W)
            { // at cone, or at least close enough.
                cmdVelPub.publish(stopVel);
                mainState = IMAGE_CONE;
                grabAnImage = true;
                continue;
            }
            if(toConeDriving.data[IMG_X] < lowerThresh) // cone is to the left of the screen
            {
                vel.angular.z = 0.2;
            }
            else if(toConeDriving.data[IMG_X]>upperThresh) // cone is to the right 
            {
                vel.angular.z = -0.2;
            }
            else // cone is in desired place in the screen
            {
                vel.angular.z = 0;
            }
            cmdVelPub.publish(vel);
            break;

        case IMAGE_CONE:
            cmdVelPub.publish(stopVel);
            if(!grabAnImage)
            {
                mainState = SEARCH_BUCKET;
            }
            break;

        case SEARCH_BUCKET:
            
            Rotate(-M_PI_4,&cmdVelPub,&rate);
            sideCount = 0;
            cantFind = false;
            while(!bucketFound && ros::ok() && !cantFind)
            {
                Drive(1,&cmdVelPub,&rate);
                ros::spinOnce();
                if(!bucketFound)
                {
                    Rotate(M_PI_2,&cmdVelPub,&rate);
                }
                ros::spinOnce();
                if(++sideCount>4)
                { // cant find a bucket, so proceed to next weighpoint instead
                    cantFind = true;

                }
            }

            if(cantFind)
            {
                mainState = LOCALISE;
                localiseState = GET_POS1;
                continue;
            }
            mainState = IMAGE_BUCKET;
            grabAnImage = true;
            getDistToPoint = true;
            break;

        case IMAGE_BUCKET:
                cmdVelPub.publish(stopVel);
                if(!getDistToPoint && !grabAnImage) // 
                {
                    goalPos = goalCoords[c++];
                    if(!(c < numGpsPoints)) //  last point
                    {
                        mainState = END_ROUTINE;
                        continue;
                    }
                    mainState = LOCALISE;
                    localiseState = GET_POS1;
                }
                break;

        case END_ROUTINE:
                /** PRINT SOMETHING OR SOMETHING IDK **/
                return 1;

        }
        


        ros::spinOnce();
        rate.sleep();
    }

    // ROS_INFO("HERE");

    return 0;
}
