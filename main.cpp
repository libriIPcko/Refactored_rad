#include <QCoreApplication>
#include <cfgFiles.h>
#include <radar_v2.h>
#include <time_measure.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    radar_v2 rad;
    //rad.init(rad.cfgFile_path);
    //rad.init_AWR2243(cfg_default_AWR2243);
    //rad.parse_offline_data("C:/Users/RPlsicik/Documents/QTCreatorWorkspace/Refactored_rad/ReceivedData/xwr18xx_processed_stream_2023_01_28T22_42_35_216.txt");
    //rad.parse_offline_data("C:/ti/XXX_Rad Tools Examples/AWR1843/Industrial/traffic_monitoring/18xx_68xx_traffic_monitoring/Recorded data/demoData.txt",true);
    //rad.parse_offline_data();

    for(int i = 0; i <= 10;i++ ){
        std::cout<< i << " -> "<< rad.test(1,9) << endl;
    }

    /*
    time_measure a1;
    a1.wallTime_start();
    int x1 = 1;
    float y1 =  0.132456789541;
    int c1 = x1*y1;
    qDebug() << c1 << qSetRealNumberPrecision(20) << "time spent by process: " << a1.wallTime_out(2) << a1.wallTime_unit;
    */

    return a.exec();
}
