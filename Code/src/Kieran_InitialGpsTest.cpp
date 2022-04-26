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
    // check command line arguments for -printTable
  bool printTable = parser.checkArgument("printTable");

  // On the Seekur, power to the GPS receiver is switched on by this command.
  // (A third argument of 0 would turn it off). On other robots this command is
  // ignored.
  robot.com2Bytes(116, 6, 1);

    ArGPS *pgps = gpsConnector.createGPS(&robot);

    if (!pgps || !pgps->connect())
    {
        ArLog::log(ArLog::Terse, "Can't connect to GPS");
        return -1;
    }

    ArTime lastReadTime;
    if(printTable)
    gps->printDataLabelsHeader();
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
