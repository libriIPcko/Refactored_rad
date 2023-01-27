#include "radar_v2.h"

radar_v2::radar_v2()
{

}

bool radar_v2::portConnect(QString type,QString name_COM, int baudrate){
    if(type.compare("CMD", Qt::CaseInsensitive) == 0){
        CMD_port->setPortName(name_COM);
        CMD_port->setBaudRate(baudrate, QSerialPort::AllDirections);                
        bool status = CMD_port->open(QIODeviceBase::ReadWrite);
        qDebug() <<  "CMD port was opened " << status;
        return status;
    }
    else if(type.compare("DATA", Qt::CaseInsensitive) == 0){
        DATA_port->setPortName(name_COM);
        DATA_port->setBaudRate(baudrate, QSerialPort::AllDirections);
        bool status = DATA_port->open(QIODeviceBase::ReadOnly);
        qDebug() << "DATA port was not opened " << status;
        return status;
    }
    else{
        return false;   //Error statement
    }
}

int radar_v2::sendCMD(QString data){
    auto lengthWrittenData = 0;
    /*                                                        //Waiting method of read
    while(!CMD_port->waitForBytesWritten(20)){
        lengthWrittenData = CMD_port->write(data.toUtf8(),qstrlen(data.toUtf8()));
    }
    */

    for(int i=0;i<data.length();i++){
        CMD_port->putChar(data.at(i).toLatin1());
        lengthWrittenData = CMD_port->bytesToWrite();
    }
    CMD_port->flush();


    if(lengthWrittenData == data.toUtf8().length()){
        return 0;   //all is OK
    }
    else if(lengthWrittenData > data.toUtf8().length()){
        return 1;   //It was write more than data contain
    }
    else if(lengthWrittenData < data.toUtf8().length()){
        return 2;   //I was write less than data contain
    }
    else{
        return 99;  //Non possible statement
    }
}

bool radar_v2::init_AWR1843(QString cfgFile_path){
    baudrate_CMD_port = baudrate_CMD_port_AWR1843;
    baudrate_DATA_port = baudrate_DATA_port_AWR1843;
    COMport_name_CMD = COMport_name_CMD_AWR1843;
    COMport_name_DATA = COMport_name_DATA_AWR1843;
    init(cfgFile_path);
}
bool radar_v2::init_AWR2243(QString cfgFile_path){
    baudrate_CMD_port = baudrate_CMD_port_AWR2243;
    baudrate_DATA_port = baudrate_DATA_port_AWR2243;
    COMport_name_CMD = COMport_name_CMD_AWR2243;
    COMport_name_DATA = COMport_name_DATA_AWR2243;
    init(cfgFile_path);
}

bool radar_v2::init(QString cfgFile_path){
    /*if(portConnect("CMD",COMport_name_CMD,baudrate_CMD_port) == false && portConnect("DATA",COMport_name_DATA,baudrate_DATA_port) == false){
        return false;
    }
    */
    bool status_CMD = portConnect("CMD",COMport_name_CMD,baudrate_CMD_port);
    bool status_DATA = portConnect("DATA",COMport_name_DATA,baudrate_DATA_port);
    if(status_CMD == false || status_DATA == false){
        return false;
    }
    if(readConfigFile(cfgFile_path) == false){
        return false;
    }
    config_dev();
    return true;
}

bool radar_v2::readConfigFile(QString path){
    QFile configFile(path);
    if(!configFile.open(QIODeviceBase::ReadOnly)){
        qCritical() << "ERROR during open the file at path:" << path;
        return false;
    }
    else{
        //qDebug() << "File has been open...";
    }
    //Save to txt block
    QTextStream stream(&configFile);
    QString allTxt;
    QString line;
    allTxt = stream.readAll();
    //Count lines & fill vector of commands
    int iterator = 0;
    while(iterator < allTxt.length()){
        if(allTxt.at(iterator) == '\n'  && line.size() != 0){
            if(line.at(0) == '%'){   //it remove header of file
                //cfgCMD.push_back(line);
                line.clear();
            }
            else{
                cfgCMD.push_back(line);
                qDebug() << cfgCMD.back();
                if(cfgCMD.back().isNull() == true){
                    cfgCMD.push_back(line);
                }

                line.clear();
            }
        }
        else{

            line.append(allTxt.at(iterator));
        }
    iterator++;
    }
    return true;
}

bool radar_v2::config_dev(){
    //Tu treba osefovat zacyklenie
    timer_cfgFile->setInterval(400);    //interval of configuration watchdog
    connect(timer_cfgFile,SIGNAL(timeout()),this,SLOT(on_config_dev()));
    connect(CMD_port,SIGNAL(readyRead()),this,SLOT(on_config_dev()));
    sendCMD(cfgCMD.front());

    return true;
}

bool radar_v2::stop(){
    connect(CMD_port,SIGNAL(readyRead()),this,SLOT(on_CMD_receive()));
    sendCMD("sensorStop\r");

}

//slot for CMD receive in th phase of configuration radar device
void radar_v2::on_config_dev(){
    /*******************************************************************/
    //There is possible bug, possible arive non-Done config command
    //The first if statement is prepared for configuration "watchdog"
    /*******************************************************************/
    if (QObject::sender() == timer_cfgFile) {
        qDebug() << "Too long time for waiting";
        sendCMD(cfgCMD.front());
        //if reach limit shut down with error

    }
    else if (QObject::sender() == CMD_port) {
        buffer_CMD_port.append(CMD_port->readAll());
        if(buffer_CMD_port.back().toLatin1() == '>'){
            buffer_CMD_port.clear();

            if(cfgCMD.size() != 0){
                sendCMD(cfgCMD.front());
                cfgCMD.pop_front();
            }
            else{
                //End configuration routine
                CMD_port->disconnect();
                timer_cfgFile->disconnect();
                //connect DATA receive slot
                connect(DATA_port,SIGNAL(readyRead()),this,SLOT(on_DATA_receive()));    //Start receiving the DATA from radar
            }
        }
    }
}

void radar_v2::on_CMD_receive(){
    //QString CMD_portInput = CMD_port->readAll();
    if(QObject::sender() == CMD_port){
        buffer_CMD_port.append(CMD_port->readAll());
        if(buffer_CMD_port.back().toLatin1() == '>'){
            qDebug() << "Done";
            disconnect(CMD_port);
        }
    }
}

void radar_v2::on_DATA_receive(){

    buffer_DATA_port.append(DATA_port->readAll());
    QString ssync = "0201040306050807";
    QByteArray sync = QByteArray::fromHex("0201040306050807");
    //QByteArrayView sync = ssync.toUtf8();
    int count = buffer_DATA_port.indexOf(sync);
    if(count == -1){
        qDebug() << "something";
    }



    if(buffer_DATA_port.contains(sync) == true){
        qDebug() << "time between packets" << a1.wallTime_out(2) << a1.wallTime_unit;
        packets.push_back(buffer_DATA_port);

        QFile testFile("C:/Users/RPlsicik/Documents/QTCreatorWorkspace/Refactored_rad/ReceivedData/debugDataFile.txt");
        testFile.open(QIODevice::WriteOnly |QIODevice::Text);
        QTextStream txt(&testFile);
        txt << buffer_DATA_port;

        buffer_DATA_port.clear();

        if(packets.size() % 10 == 0){

            for(int i=0;i < packets.size();i++){
                txt << packets.at(i);
            }
        }

        a1.wallTime_start();
    }

}
