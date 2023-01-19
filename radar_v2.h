#ifndef RADAR_V2_H
#define RADAR_V2_H

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
private:
    //Port Connect
    //@type = CMD or type = DATA
    bool portConnect(QString type,QString name_COM, int baudrate);
    int baudrate_CMD_port = 115200;
    int baudrate_DATA_port = 921600;
    QString COMport_name_CMD = "COM3";
    QString COMport_name_DATA = "COM4";
    QSerialPort *CMD_port = new QSerialPort;
    QSerialPort *DATA_port = new QSerialPort;

    //Initialization
    bool readConfigFile(QString path);
    bool config_dev();
    QString buffer_CMD_port;

    QTimer *timer_cfgFile = new QTimer();
    deque<QString> cfgCMD;

    int sendCMD(QString data);


private: signals:
    void timeout();
    void readyRead();
private slots:
  void on_CMD_receive();
  void on_DATA_receive();
  void on_config_dev();

};

#endif // RADAR_V2_H
