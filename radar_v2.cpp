#include "radar_v2.h"

radar_v2::radar_v2()
{

}

bool radar_v2::portConnect(QString type,QString name_COM, int baudrate){
    if(type.compare("CMD", Qt::CaseInsensitive) == 0){
        CMD_port->setPortName(name_COM);
        CMD_port->setBaudRate(baudrate, QSerialPort::AllDirections);
        return CMD_port->open(QIODeviceBase::ReadWrite);
    }
    else if(type.compare("DATA", Qt::CaseInsensitive) == 0){
        DATA_port->setPortName(name_COM);
        DATA_port->setBaudRate(baudrate, QSerialPort::AllDirections);
        return DATA_port->open(QIODeviceBase::ReadOnly);
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

bool radar_v2::init(QString cfgFile_path){
    if(portConnect("CMD",COMport_name_CMD,baudrate_CMD_port) == false && portConnect("DATA",COMport_name_DATA,baudrate_DATA_port) == false){
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
    timer_cfgFile->setInterval(400);
    connect(timer_cfgFile,SIGNAL(timeout()),this,SLOT(on_config_dev()));
    connect(CMD_port,SIGNAL(readyRead()),this,SLOT(on_config_dev()));
    sendCMD(cfgCMD.front());

    return true;
}

void radar_v2::on_config_dev(){
    if (QObject::sender() == timer_cfgFile) {
        // the signal was emitted by the timer_cfgFile object
        qDebug() << "Too long time for waiting";
        sendCMD(cfgCMD.front());
        //if reach limit shut down with error

    } else if (QObject::sender() == CMD_port) {
        // the signal was emitted by the CMD_port object
            //read CMD port
            QString input;
            //SURE?
            /*
            if(CMD_port->waitForReadyRead(500)){
               input  = QString(CMD_port->readAll());
            }
            */
            //for(CMD_port->bytesAvailable())
            input = CMD_port->readLine();
            /*
            if(CMD_port->waitForReadyRead(500)){
               //input  = QString(CMD_port->readAll());
                qDebug() << CMD_port->readAll() <<CMD_port->bytesAvailable();
            }
            */


            char *c;
            CMD_port->getChar(c);
            buffer_CMD_port.append(c);
            if(buffer_CMD_port.back().toLatin1() == '>'){
                qDebug() << buffer_CMD_port;
                sendCMD(cfgCMD.back());
                cfgCMD.pop_front();
            }
            //

            }
            /*
            //2statements if contain if do not contain
            if(input.contains("Done",Qt::CaseInsensitive)){
                qDebug() << "WITH confirmation" <<cfgCMD.front() << input;
                cfgCMD.pop_front();
                //if(cfgCMD.size() == 0 || cfgCMD.back() == NULL)
                sendCMD(cfgCMD.front());
            }
            else{
                qDebug() << "Without confirmation" <<cfgCMD.front() << input;

                cfgCMD.pop_front();
                sendCMD(cfgCMD.front());
            }
            //send next CMD
            //test of clear vector cfg_CMD
    }
    */
}

void radar_v2::on_CMD_receive(){
    //QString CMD_portInput = CMD_port->readAll();

}

void radar_v2::on_DATA_receive(){

}
