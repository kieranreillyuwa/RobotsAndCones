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

#define IMG_HEIGHT 720 // for depth
#define IMG_WIDTH 1280 // for depth

#define ZERO_OFFSET M_PI/180

#define GPS_RES 1
#define LOCALISE_DIST 1
#define NUM_POINTS 2

#define BASE_SPEED 1

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
#define IMG_HB 5

#define IMG_SIZE_THRESH_W 320
#define IMG_SIZE_THRESH_H 60000

#define BOUND_X_LOW 900
#define BOUND_X_HIGH 1100

#define CONE 0
#define BUCKET 1

#define DEPTH_THRESH 1.1 // Amount of depth image that can be less than 200 // WAS 0.9
#define DEPTH_MIN_ALLOW -1 // Was 55

#define d2r (M_PI / 180.0)

// static const uint16_t lowerThresh = WIDTH_IMAGE/3 + OFFSET_IMAGE;
// static const uint16_t upperThresh = WIDTH_IMAGE - WIDTH_IMAGE/3 - OFFSET_IMAGE;

static const int imgSize = IMG_HEIGHT*IMG_WIDTH;


enum MainState_t
{
    MANUAL,
    LOCALISE,
    DRIVE_TOW,
    TURN_CONE,
    DRIVE_CONE,
    IMAGE_CONE,
    SEARCH_BUCKET,
    IMAGE_BUCKET,
    OBSTACLE,
    END_ROUTINE
};
enum LocaliseState_t
{
    GET_POS1,
    GET_POS2,
    GET_HEADING,
    INACTIVE
};

double haversine_m(float lat1, float long1, float lat2, float long2)
{
    double dlong = (long2 - long1) * d2r;
    double dlat = (lat2 - lat1) * d2r;
    double a = pow(sin(dlat/2.0), 2) + cos(lat1*d2r) * cos(lat2*d2r) * pow(sin(dlong/2.0), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double d = 6367 * c;

    return abs(d * 1000);
}

float joyForward = 0.0;
float joySide = 0.0;

MainState_t mainState;
MainState_t prevState;
LocaliseState_t localiseState;

sensor_msgs::NavSatFix goalCoords[NUM_POINTS + 1];
uint8_t numGpsPoints = NUM_POINTS;
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

bool obstacle = false;


void Rotate(double angle, ros::Publisher *pcmdVelPub, ros::Rate *prate, double speed = 0.5)
{
    printf("Rotating...\n");
    double timeToWait = double(abs(angle / speed));
    geometry_msgs::Twist vel;
    vel.linear.x = 0;
    vel.angular.z = sgn(angle) * speed;
    ros::Time startTime = ros::Time::now();
    pcmdVelPub->publish(vel);
    while (ros::ok() && ros::Time::now() - startTime < ros::Duration(timeToWait))
    {
    
        pcmdVelPub->publish(vel);
        if (mainState == MANUAL)
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

void Drive(double dist, ros::Publisher *pcmdVelPub, ros::Rate *prate, double speed = BASE_SPEED)
{
    printf("Driving...");
    double timeToWait = double(dist / speed);
    geometry_msgs::Twist vel;
    vel.linear.x = speed;
    // vel.angular.z = 0;
    vel.angular.z = ZERO_OFFSET;

    ros::Time startTime = ros::Time::now();
    pcmdVelPub->publish(vel);
    while (ros::ok() && ros::Time::now() - startTime < ros::Duration(timeToWait))
    {
        pcmdVelPub->publish(vel);
        pcmdVelPub->publish(vel);
        if (mainState == MANUAL || obstacle)
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
int hbCone = 0;
int hbBucket = 0;

void ImageStatusCallback(const std_msgs::UInt16MultiArrayConstPtr &msg)
{
    if (msg->data[IMG_ID] == CONE)
    {
        if (mainState == DRIVE_TOW || mainState == DRIVE_CONE || mainState == IMAGE_CONE || mainState == TURN_CONE)
        {
            toConeDriving = *msg;
            coneFound = true;
            return;
        }
    }
    else if (msg->data[IMG_ID] == BUCKET)
    {
        if (mainState == SEARCH_BUCKET || mainState == IMAGE_BUCKET)
        {
            toBucket = *msg;
            bucketFound = true;
            return;
        }
    }
    coneFound = false;
    bucketFound = false;
}



// static const float driveForwardTime = 1.0;
// enum BucketSearch_t
// {
//     SIDE_1,
//     SIDE_2,
//     SIDE_3,
//     SIDE_4
// };
// BucketSearch_t bucketSearch = SIDE_1;


struct DepthData_t
{
    float percentage;
    float avgValue;
};

DepthData_t left;
DepthData_t right;


bool getLeft = false;
bool getRight = false;
bool goingToCone = false;
bool processingObstacle = false;
void DepthImageCallback(const sensor_msgs::ImageConstPtr &msg)
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
    

    if(percentage>DEPTH_THRESH && !processingObstacle && mainState!=MANUAL)
    {
        printf("Percentage of image too close %.9f\n",percentage);
        printf("Average depth: %.9f\n",average);
        if(!goingToCone) 
        {
            obstacle = true;
            mainState = OBSTACLE;
        }
    }

    if(getLeft)
    {
        left.percentage = percentage;
        left.avgValue = average;
        getLeft = false;
        printf("Percentage of image too close %.9f\n",percentage);
        printf("Average depth: %.9f\n",average);
    }

    if(getRight)
    {
        right.percentage = percentage;
        right.avgValue = average;
        getRight = false;
        printf("Percentage of image too close %.9f\n",percentage);
        printf("Average depth: %.9f\n",average);
    }

}

bool savedImage = true;

void AckCB(const std_msgs::BoolConstPtr &msg)
{
    if(msg->data == 1)
    {
        savedImage = true;
    }
}

int main(int argc, char **argv)
{
    printf("Program Starting...");

    ros::init(argc, argv, "Main_Node");
    ros::NodeHandle nh;
    ros::Publisher cmdVelPub = nh.advertise<geometry_msgs::Twist>("RosAria/cmd_vel", 10);
    ros::Subscriber gpsSub = nh.subscribe<sensor_msgs::NavSatFix>("fix", 10, GpsCallBack);
    ros::Subscriber imuMagSub = nh.subscribe<sensor_msgs::MagneticField>("imu/mag", 10, ImuCallBack);
    ros::Subscriber joySub = nh.subscribe<sensor_msgs::Joy>("joy", 5, JoyCallBack);
    ros::Publisher mainStatusPub = nh.advertise<std_msgs::Bool>("main_status", 5);
    ros::Subscriber imageStatusSub = nh.subscribe<std_msgs::UInt16MultiArray>("image_status", 10, ImageStatusCallback);
    // ros::Subscriber imageSub = nh.subscribe<sensor_msgs::Image>("vision", 5, GetImageCallback);
    ros::Subscriber depthImageSub = nh.subscribe<sensor_msgs::Image>("rgb_stereo_publisher/stereo/depth", 10, DepthImageCallback);
    ros::Publisher saveImagePub = nh.advertise<std_msgs::Bool>("get_highlights",1);
    ros::Subscriber ackSub = nh.subscribe<std_msgs::Bool>("ack_highlights",1,AckCB);

    goalCoords[0].latitude = -31.98038731577529;
    goalCoords[0].longitude = 115.8171782781675;
    
    goalCoords[1].latitude = -31.98017884452402;
    goalCoords[1].longitude = 115.8171702857572;



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

    ros::Time startTime;

    uint8_t sideCount = 0;
    bool cantFind = false;
    double relHeading;
    double heading;
    double roughHeading;
    double distToGoal;
    bool gotTheCone = false;
    std_msgs::Bool msgOut;
    while (ros::ok())
    {
        switch (mainState)
        {
        case MANUAL:
            printf("Manual mode..\n");
            // ROS_INFO("here 3");
            vel.linear.x = joyForward;
            vel.angular.z = 0.5 * joySide;
            // ROS_INFO("here 4");

            cmdVelPub.publish(vel);

            break;
        case LOCALISE:
            printf("Localising ");  
            switch (localiseState)
            {
            case GET_POS1:
                getGpsCoords = true;
                printf("starting position\n");

                while (getGpsCoords && ros::ok())
                {
                    cmdVelPub.publish(stopVel);
                    ros::spinOnce();
                    rate.sleep();
                }
                tempPos = GetMedianValue(collectionArr);
                if (firstPoint)
                {
                    goalCoords[numGpsPoints++] = tempPos;
                    firstPoint = false;
                }
                ROS_INFO("GPS Latitude: %.9f, Longitude: %.9f", tempPos.latitude, tempPos.longitude);
                localiseState = GET_POS2;
                Drive(LOCALISE_DIST, &cmdVelPub, &rate);

                break;

            case GET_POS2:
                printf("finishing position\n");
                getGpsCoords = true;

                while (getGpsCoords && ros::ok())
                {
                    cmdVelPub.publish(stopVel);
                    ros::spinOnce();
                    rate.sleep();
                }
                currentPos = GetMedianValue(collectionArr);
                ROS_INFO("GPS Latitude: %.9f, Longitude: %.9f", currentPos.latitude, currentPos.longitude);

                localiseState = GET_HEADING;
                // Drive(5.0,&cmdVelPub,&rate);

                break;
            case GET_HEADING:
                printf("getting heading\n");

                cmdVelPub.publish(stopVel);

                heading = GetHeading(tempPos, currentPos);
                ROS_INFO("The heading is: %.9f", heading * 180 / M_PI);
                localiseState = INACTIVE;
                mainState = DRIVE_TOW;
                std_msgs::Bool startImgProcessing;
                startImgProcessing.data = 1;
                mainStatusPub.publish(startImgProcessing);
                break;
            }
	    break;
        case DRIVE_TOW:
            printf("Turning towards goal.....\n");
            // Drive(10, &cmdVelPub, &rate);                           // just incase other cones are in the vision
            relHeading = GetHeading(currentPos, goalPos) - heading; // get the angle between
            if (relHeading > M_PI)
            {
                relHeading -= M_PI * 2;
            }
            else if (relHeading < -M_PI)
            {
                relHeading += M_PI * 2;
            }
            heading+=relHeading;
            Rotate(relHeading, &cmdVelPub, &rate); // turn to goal
            // distToGoal = haversine_m(currentPos.latitude, currentPos.longitude,goalPos.latitude,goalPos.longitude);
            // printf("Driving to goal..\n");
            // // Drive(distToGoal-5,&cmdVelPub,&rate);  /** UNCOMMENT THIS **/
            // // should be facing goal now
            // printf("Looking for cone...\n");
            // vel.linear.x = 0.3;
            // vel.angular.z = M_PI/180;
            // startTime = ros::Time::now();
            // while (!coneFound && ros::ok()) // keep driving forward until cone is found
            // {
            //     if(ros::Time::now()-startTime>ros::Duration(30)) std::runtime_error("couldn't find goal");
            //     if(obstacle) break;
            //     cmdVelPub.publish(vel);
            //     ros::spinOnce();
            //     rate.sleep();
            // }
            // cmdVelPub.publish(stopVel);
            // mainState = DRIVE_CONE;
            // goingToCone = true;
            for(int i = 0; i < 3; i++)
            {
                if(coneFound && toConeDriving.data[IMG_HB] > hbCone) {hbCone =toConeDriving.data[IMG_HB]; gotTheCone = true; break;}
                Rotate(15*M_PI/180,&cmdVelPub,&rate);
                startTime = ros::Time::now();
                while(ros::Time::now()-startTime<ros::Duration(2.0))
            {
                 cmdVelPub.publish(stopVel);

                ros::spinOnce();
                rate.sleep();
            }
                if(coneFound && toConeDriving.data[IMG_HB] > hbCone) {hbCone =toConeDriving.data[IMG_HB]; gotTheCone = true; break;}
                Rotate(30*M_PI/180,&cmdVelPub,&rate);
                startTime = ros::Time::now();

                while(ros::Time::now()-startTime<ros::Duration(2.0))
            {
                 cmdVelPub.publish(stopVel);

                ros::spinOnce();
                rate.sleep();
            }
                if(coneFound && toConeDriving.data[IMG_HB] > hbCone) {hbCone =toConeDriving.data[IMG_HB]; gotTheCone = true; break;}
                Rotate(-15*M_PI/180,&cmdVelPub,&rate);
                startTime = ros::Time::now();

                while(ros::Time::now()-startTime<ros::Duration(2.0))
            {
                 cmdVelPub.publish(stopVel);

                ros::spinOnce();
                rate.sleep();
            }
                Drive(5,&cmdVelPub,&rate);
                startTime = ros::Time::now();

                while(ros::Time::now()-startTime<ros::Duration(2.0))
            {
                 cmdVelPub.publish(stopVel);

                ros::spinOnce();
                rate.sleep();
            }

            }
            if(!gotTheCone)
            {
                printf("Could not find cone after localisation...\n");
                mainState = LOCALISE;
                localiseState = GET_POS1;
            }
            else
            {
                mainState = TURN_CONE;
            }

            break;

        case TURN_CONE:
            printf("Cone found, driving turning towards...\n");
            cmdVelPub.publish(stopVel);


            // vel.linear.x = 0.5;
            if (toConeDriving.data[IMG_W] > IMG_SIZE_THRESH_W)
            { // at cone, or at least close enough.
                cmdVelPub.publish(stopVel);
                mainState = IMAGE_CONE;
                continue;
            }

            printf("CONE HB: %d\n",hbCone);
            printf("LIVE HB: %d\n",toConeDriving.data[IMG_HB]);

            if(toConeDriving.data[IMG_HB] > hbCone)
            {
                hbCone = toConeDriving.data[IMG_HB];
                if(toConeDriving.data[IMG_X] < BOUND_X_LOW)
                {
                    printf("Turning left towards cone...\n");
                    Rotate(3*M_PI/180,&cmdVelPub,&rate);
                }
                else if(toConeDriving.data[IMG_X] > BOUND_X_HIGH)
                {
                    printf("Turning right towards cone...\n");
                    Rotate(-3*M_PI/180,&cmdVelPub,&rate);
                }
                else // Cone is in range
                {
                    mainState = DRIVE_CONE;
                }

            }
            else
            {
                printf("Lost cone during turn, or camera is off....\n");
            }

            // if (toConeDriving.data[IMG_X] < BOUND_X_LOW) // cone is to the left of the screen
            // {
            //     printf("Going left...\n");
            //     vel.angular.z = 0.2;
            // }
            // else if (toConeDriving.data[IMG_X] > BOUND_X_HIGH) // cone is to the right
            // {
            //     printf("Going right...\n");
            //     vel.angular.z = -0.2;
            // }
            // else // cone is in desired place in the screen
            // {
            //     printf("Going straight...\n");
            //     vel.angular.z = 180/M_PI;
            //     // vel.angular.z = 0;

            // }

            // cmdVelPub.publish(stopVel);
            break;
        case DRIVE_CONE:
            printf("Driving to cone...\n");
            vel.linear.x = 0.2;
            vel.angular.z = ZERO_OFFSET;

            if (toConeDriving.data[IMG_W] > IMG_SIZE_THRESH_W)
            { // at cone, or at least close enough.
                cmdVelPub.publish(stopVel);
                mainState = IMAGE_CONE;
                continue;
            }

            printf("CONE HB: %d\n",hbCone);
            
            printf("LIVE HB: %d\n",toConeDriving.data[IMG_HB]);

            if(toConeDriving.data[IMG_HB] > hbCone)
            {
                hbCone = toConeDriving.data[IMG_HB];
                if(toConeDriving.data[IMG_X] > BOUND_X_LOW && toConeDriving.data[IMG_X] < BOUND_X_HIGH)
                {
                    
                    Drive(1,&cmdVelPub,&rate);
                }
                else
                {
                    

                    mainState = TURN_CONE;
                    cmdVelPub.publish(stopVel);

                }

            }
            else
            {
                // cmdVelPub.publish(stopVel);
                printf("Lost cone, or camera....\n");
            }


            // cmdVelPub.publish(vel);

            break;

        case IMAGE_CONE:
            printf("Close to cone, saving image....\n");
            savedImage = false; 
            msgOut.data = true;
            saveImagePub.publish(msgOut);
            cmdVelPub.publish(stopVel);

            while(!savedImage)
            {
                printf("waiting for image to save...\n");
                ros::spinOnce();
                rate.sleep();
            }
            
            mainState = SEARCH_BUCKET;
            
            break;

        case SEARCH_BUCKET:
            printf("Searching for bucket...\n");
            Rotate(-M_PI_4, &cmdVelPub, &rate);
            heading-=M_PI_4;
            sideCount = 0;
            cantFind = false;
            while (!bucketFound && ros::ok() && !cantFind)
            {
                Drive(2, &cmdVelPub, &rate,0.2);
                ros::spinOnce();
                if (!bucketFound)
                {
                    Rotate(M_PI_2, &cmdVelPub, &rate);
                    heading+=M_PI_2;
                }
                ros::spinOnce();
                if (++sideCount > 4)
                { // cant find a bucket, so proceed to next weighpoint instead
                    cantFind = true;
                }
            }

            if (cantFind)
            {
                printf("Couldn't find bucket, moving to next goal...\n");
                mainState = LOCALISE;
                localiseState = GET_POS1;
                goalPos = goalCoords[c++];
                roughHeading = GetHeading(livePos,goalPos) - heading;
                Rotate(roughHeading,&cmdVelPub,&rate);
                continue;
            }
            mainState = IMAGE_BUCKET;
        
            // getDistToPoint = true;
            break;

        case IMAGE_BUCKET:
            printf("Getting image of bucket...\n");
            cmdVelPub.publish(stopVel);
            savedImage = false; 
            msgOut.data = true;
            saveImagePub.publish(msgOut);
            cmdVelPub.publish(stopVel);

            while(!savedImage)
            {
                printf("waiting for image to save...\n");
                ros::spinOnce();
                rate.sleep();
            }
                mainState = LOCALISE;
                localiseState = GET_POS1;
                roughHeading = GetHeading(livePos,goalPos) - heading;
                while(roughHeading >180 || relHeading<-180)
                {
                    if(roughHeading<-180)
                    {
                        roughHeading+=360;
                    }
                    else if(roughHeading>180)
                    {
                        roughHeading-=360;
                    }
                }
                Rotate(roughHeading,&cmdVelPub,&rate);
            
            break;
        case OBSTACLE:
            printf("Obstacle detected...\n");
            cmdVelPub.publish(stopVel);
            processingObstacle = true;
            startTime = ros::Time::now();
            while(ros::Time::now()-startTime<ros::Duration(5.0))
            {
                 cmdVelPub.publish(stopVel);

                ros::spinOnce();
                rate.sleep();
            }
            obstacle = false;
            Rotate(55*M_PI/180,&cmdVelPub,&rate);
            heading += 55*M_PI/180;
            getLeft = true;
            while(getLeft)
            {
                ros::spinOnce();
                rate.sleep();
            }
            Rotate(-110 * M_PI/180,&cmdVelPub,&rate);
            heading-=110 * M_PI/180;
            getRight = true;
            while(getRight)
            {
                ros::spinOnce();
                rate.sleep();
            }
            if(left.avgValue > right.avgValue) //&& left.avgValue>DEPTH_MIN_ALLOW)
            {
                Rotate(110 * M_PI/180,&cmdVelPub,&rate);
                heading+=110 * M_PI/180;
            }
            else if(right.avgValue<DEPTH_MIN_ALLOW)
            {
                throw std::runtime_error("Could not find free path");
            }
            processingObstacle = false;
            mainState = LOCALISE;
            localiseState = GET_POS1;

            break;

        case END_ROUTINE:
            /** PRINT SOMETHING OR SOMETHING IDK **/
            printf("Routine complete, exiting...\n");
            return 1;
        }

        ros::spinOnce();
        rate.sleep();
    }

    // ROS_INFO("HERE");

    return 0;
}
