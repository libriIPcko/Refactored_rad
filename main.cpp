#include <QCoreApplication>
#include <cfgFiles.h>
#include <radar_v2.h>
#include <time_measure.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    radar_v2 rad;
    //rad.init(rad.cfgFile_path);
    rad.init_AWR2243(cfg_default_AWR2243);

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
