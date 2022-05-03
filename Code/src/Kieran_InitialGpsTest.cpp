#include "Aria.h"
#include "ArGPS.h"
#include "ArGPSConnector.h"


struct GpsPosition
{
    double latitude;
    double longitude;

    void GetGPS(ArGPS *pgps)
    {
        latitude = pgps->getLatitude();
        longitude = pgps->getLongitude();
    }

    
};

void PrintPose(ArPose* ppose)
{
    printf("x: %.2f, y: %.2f, theta: %.2f\n",ppose->getX(),ppose->getY(),ppose->getTh());
}

int main(int argc, char **argv)
{
    Aria::init();

    ArRobot robot;


    ArArgumentParser parser(&argc, argv);
    parser.loadDefaultArguments();

    ArRobotConnector robotConnector(&parser, &robot);
    ArGPSConnector gpsConnector(&parser);

    if (!robotConnector.connectRobot())
    {
        ArLog::log(ArLog::Terse, "Can't connect to robot");
    }

    ArGPS *pgps = gpsConnector.createGPS(&robot);

    if (!pgps || !pgps->connect())
    {
        ArLog::log(ArLog::Terse, "gpsExample: Error connecting to GPS device.  Try -gpsType, -gpsPort, and/or -gpsBaud command-line arguments. Use -help for help.");
        return -1;
    }

    ArTime lastReadTime;
    for(;;)
    {
        int r = pgps->read();

        if(r & ArGPS :: ReadUpdated) // just a bitwise check to see if there is a difference in gps reading.
        {
            pgps->printData();
            printf("\r \n");
            ArPose xyt = pgps->getPose();

            ArPose robotPose = robot.getPose();
            PrintPose(&xyt);
            PrintPose(&robotPose);
            
            printf("\r \n");

            fflush(stdout);

            ArUtil::sleep(500);

            lastReadTime.setToNow();
        }else
        {
            if(lastReadTime.secSince() > 5 )
            {
                ArLog::log(ArLog::Terse, "been more than 5 seconds since GPS reading.");
            }
            ArUtil::sleep(1000);
        }
        

        

    }



    return 0;
}