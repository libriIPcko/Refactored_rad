#ifndef CFGFILES_H
#define CFGFILES_H
#include <QString>

//CONFIGURATION COMMANDS
QString ownPath = "C:/Users/RPlsicik/Documents/QTCreatorWorkspace/Refactored_rad";
QString totalPath_AWR1843 = ownPath + "/ConfigurationFiles/AWR1843";
QString totalPath_AWR2243 = ownPath + "/ConfigurationFiles/AWR2243";
//AWR1843
QString cfg_default_AWR1843 = totalPath_AWR1843 + "/default.txt";

//AWR1843
QString cfg_default_AWR2243 = totalPath_AWR2243 +"/";


//RAW DATA
//QString raw_data = "C:/Users/RPlsicik/Downloads/xwr18xx_processed_stream_2023_02_06T14_33_51_007.dat";
    //from traffic monitoring
QString raw_data ="C:/ti/XXX_Rad Tools Examples/AWR1843/Industrial/traffic_monitoring/18xx_68xx_traffic_monitoring/Recorded data/tm_demo_uart_stream - Copy.txt";
#endif // CFGFILES_H
