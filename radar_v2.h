#ifndef RADAR_V2_H
#define RADAR_V2_H
#include <time_measure.h>

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
    QString cfgFile_path = "C:/Users/RPlsicik/Documents/GitHub/RVC/tst/untitled4/xwr18xx_profile_2022_05_30T13_05_06_607.txt";

    //Debug atributes:
    time_measure a1;
    QString data;

private:
    QSerialPort *CMD_port = new QSerialPort;
    QSerialPort *DATA_port = new QSerialPort;

    //Port Connect
    //@type = CMD or type = DATA
    bool portConnect(QString type,QString name_COM, int baudrate);
    int baudrate_CMD_port = 115200;
    int baudrate_DATA_port = 921600;
    QString COMport_name_CMD = "COM3";
    QString COMport_name_DATA = "COM4";

    //Initialization
    bool readConfigFile(QString path);
    bool config_dev();
    bool stop();
    QString buffer_CMD_port = "";
    QTimer *timer_cfgFile = new QTimer();
    deque<QString> cfgCMD;
    int sendCMD(QString data);

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
