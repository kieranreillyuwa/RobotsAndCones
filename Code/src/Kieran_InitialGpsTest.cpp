#include "Aria.h"
#include "ArGPS.h"
#include "ArGPSConnector.h"
#include "ArTrimbleGPS.h"
#include "ArTCMCompassDirect.h"
#include <iostream>


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
  ArArgumentParser parser(&argc, argv);
  parser.loadDefaultArguments();
  ArRobot robot;
  ArRobotConnector robotConnector(&parser, &robot);
  ArGPSConnector gpsConnector(&parser);

  // Connect to the robot, get some initial data from it such as type and name,
  // and then load parameter files for this robot.
  if(!robotConnector.connectRobot())
  {
    ArLog::log(ArLog::Terse, "gpsExample: Warning: Could not connect to robot.  Will not be able to switch GPS power on, or load GPS options from this robot's parameter file.");
  }


    ArArgumentParser parser(&argc, argv);
    parser.loadDefaultArguments();
  if (!Aria::parseArgs() || !parser.checkHelpAndWarnUnparsed())
  {
    Aria::logOptions();
    ArLog::log(ArLog::Terse, "gpsExample options:\n  -printTable   Print data to standard output in regular columns rather than a refreshing terminal display, and print more digits of precision");
    Aria::exit(1);
  }

  ArLog::log(ArLog::Normal, "gpsExample: Connected to robot.");

  robot.runAsync(true);

  // check command line arguments for -printTable
  bool printTable = parser.checkArgument("printTable");

  // On the Seekur, power to the GPS receiver is switched on by this command.
  // (A third argument of 0 would turn it off). On other robots this command is
  // ignored.
  robot.com2Bytes(116, 6, 1);

  // Try connecting to a GPS. We pass the robot pointetr to the connector so it
  // can check the robot parameters for this robot type for default values for
  // GPS device connection information (receiver type, serial port, etc.)
  ArLog::log(ArLog::Normal, "gpsExample: Connecting to GPS, it may take a few seconds...");
  ArGPS *pgps = gpsConnector.createGPS(&robot);
  if(!pgps || !pgps->connect())
  {
    ArLog::log(ArLog::Terse, "gpsExample: Error connecting to GPS device.  Try -gpsType, -gpsPort, and/or -gpsBaud command-line arguments. Use -help for help.");
    return -1;
  }

  if(gpsConnector.getGPSType() == ArGPSConnector::Simulator)
  {
    ArLog::log(ArLog::Normal, "gpsExample: GPS data is from simulator.");
    /*
      If connected to MobileSim, and aa map is loaded into MobileSim that contains an OriginLatLonAlt line,
      then MobileSim will provides simulated GPS data based on the robot's 
      true position in the simulator.  But you can also manually set "dummy"
      positions like this instead, or to simulate GPS without connecting
      to MobileSim:
    */
    //ArLog::log(ArLog::Normal, "gpsExample: GPS is a simulator. Setting dummy position.");
    //(dynamic_cast<ArSimulatedGPS*>(gps))->setDummyPosition(42.80709, -71.579047, 100);
  }
  


  ArLog::log(ArLog::Normal, "gpsExample: Reading data...");
    ArTime lastReadTime;
    if(printTable)
    pgps->printDataLabelsHeader();
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
