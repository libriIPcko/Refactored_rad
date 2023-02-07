#ifndef RADAR_V2_H
#define RADAR_V2_H
#include <time_measure.h>
#include <tlv_dat.h>

#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QObject>

#include <vector>
#include <deque>

using namespace std;

class radar_v2 : public QObject
{
    Q_OBJECT
public:
    radar_v2();
    bool init(QString cfgFile_path);
    bool init_AWR1843(QString cfgFile_path);
    bool init_AWR2243(QString cfgFile_path);

    void parse_offline_data(QString path, bool asciidata);
    void parse_offline_data();
    void readFromFile(QString path);

    QString cfgFile_path = "C:/Users/RPlsicik/Documents/GitHub/RVC/tst/untitled4/xwr18xx_profile_2022_05_30T13_05_06_607.txt";

    //Debug atributes:
    time_measure a1;
    QString data;

    int baudrate_CMD_port;
    int baudrate_DATA_port;
    QString COMport_name_CMD;
    QString COMport_name_DATA;

private:
    QSerialPort *CMD_port = new QSerialPort;
    QSerialPort *DATA_port = new QSerialPort;

    //Port Connect
    //@type = CMD or type = DATA
    bool portConnect(QString type,QString name_COM, int baudrate);

    QString COMport_name_CMD_AWR1843 = "COM3";
    QString COMport_name_DATA_AWR1843 = "COM4";
    int baudrate_CMD_port_AWR1843 = 115200;
    int baudrate_DATA_port_AWR1843 = 921600;
        //QString COMport_name_CMD_AWR2243 = "COM11";
        //QString COMport_name_DATA_AWR2243 = "COM17";
    QString COMport_name_CMD_AWR2243 = "COM3";
    QString COMport_name_DATA_AWR2243 = "COM4";
    int baudrate_CMD_port_AWR2243 = 115200;
    int baudrate_DATA_port_AWR2243 = 892857;

    //Initialization
    bool readConfigFile(QString path);
    bool config_dev();
    bool stop();
    QString buffer_CMD_port = "";
    QTimer *timer_cfgFile = new QTimer();
    deque<QString> cfgCMD;
    int sendCMD(QString data);

    //Parse data
    //void readFromFile(QString path);
    void readFromFile(QString path,bool asciidata);
    void savePackedData(QString path);
    void parseData(QString data);
    void saveData_byType(QString path);
    TLV_dat outData;

    std::vector<QString> TLV_RAW_packets;
    std::vector<TLV_dat> TLV_packets;     //Wrong architecture approach

    std::deque<QString> packedData;

    //Data pre-process
    QByteArray buffer_DATA_port = "";
    std::deque<QByteArray> packets;

private: signals:
    void timeout();
    void readyRead();
private slots:
    void on_CMD_receive();
    void on_DATA_receive();
    void on_config_dev();
};

#endif // RADAR_V2_H
