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
    return init(cfgFile_path);
}
bool radar_v2::init_AWR2243(QString cfgFile_path){
    baudrate_CMD_port = baudrate_CMD_port_AWR2243;
    baudrate_DATA_port = baudrate_DATA_port_AWR2243;
    COMport_name_CMD = COMport_name_CMD_AWR2243;
    COMport_name_DATA = COMport_name_DATA_AWR2243;
    return init(cfgFile_path);
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
    int returnStatement = sendCMD("sensorStop\r");
    if(returnStatement == 0){
        //ok
        return true;
    }
    else{
        //false of stop
        return false;
    }

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
            for(int i=0;i <(int) packets.size();i++){
                txt << packets.at(i);
            }
        }
        a1.wallTime_start();
    }
}



//File read and parse
//QByteArray buffer_DATA_port = "";
//std::deque<QByteArray> packets;
//Output from function is packed
void radar_v2::parse_offline_data(QString path, bool asciidata){
    qDebug() << "OK";
    readFromFile(path,asciidata);
    //savePackedData("C:/Users/RPlsicik/Documents/QTCreatorWorkspace/Refactored_rad/ReceivedData/ParsedData/saved_PackedData.txt");
    savePackedData("C:/ti/XXX_Rad Tools Examples/AWR1843/Industrial/traffic_monitoring/18xx_68xx_traffic_monitoring/Recorded data/packetizedData.txt");
    int i = 0;
    int length = packedData.size();
    while(i < length){
        parseData(packedData.front());
        packedData.pop_front();
        if(i == length-40){
            qDebug() << "Here";
        }
    }
}

void radar_v2::parse_offline_data(){
    qDebug() << "OK";
    readFromFile("C:/ti/XXX_Rad Tools Examples/AWR1843/Industrial/traffic_monitoring/18xx_68xx_traffic_monitoring/Recorded data/tm_demo_uart_stream - Copy.txt");
    //savePackedData("C:/Users/RPlsicik/Documents/QTCreatorWorkspace/Refactored_rad/ReceivedData/ParsedData/saved_PackedData.txt");
    savePackedData("C:/ti/XXX_Rad Tools Examples/AWR1843/Industrial/traffic_monitoring/18xx_68xx_traffic_monitoring/Recorded data/packetizedData.txt");
    int i = 0;
    int length = packedData.size();
    while(i < length){
        parseData(packedData.front());
        packedData.pop_front();
        if(i == length-40){
            qDebug() << "Here";
        }
    }
}

void radar_v2::readFromFile(QString path,bool asciidata){
    /*
    buffer_DATA_port.append(DATA_port->readAll());
    QString ssync = "0201040306050807";
    QByteArray sync = QByteArray::fromHex("0201040306050807");
    //QByteArrayView sync = ssync.toUtf8();
    int count = buffer_DATA_port.indexOf(sync);
     */

    QFile file(path);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text) == true){

        QByteArray sync = QByteArray::fromHex("0201040306050807");
        QByteArray file_output = file.readAll();
        QString temp;
        if(asciidata == true){
            //QString temp(file_output);
            temp = file_output;
        }
        else{
            //QString temp(file_output.toHex());
            temp = file_output.toHex();
        }
        QFile out("C:/Users/RPlsicik/Documents/QTCreatorWorkspace/Refactored_rad/ReceivedData/tst.txt");
        QTextStream out_stream(&out);
        out.open(QIODevice::WriteOnly);
        out_stream << file_output.toHex();

    //Finding sync
        std::deque<int> posOfPacket;
        int it = 0;
        int length = temp.size();
        while(it < length) {
            posOfPacket.push_back(temp.indexOf("0201040306050807",it));
            it = posOfPacket.back();// + 1 ;
            //if(it == -1 || it == 0){
            if(it == -1){
                break;
            }
            it++;
        }
    //Filling packet;
        QString hexVal;
        int i = posOfPacket.front();
        posOfPacket.pop_front();
        while(i<temp.length()){
            if(i < posOfPacket.front()-1){
                hexVal.append(temp.at(i));
            }
            else{
                //hexVal.append(temp.at(i));
                packedData.push_back(hexVal);
                hexVal.clear();
                posOfPacket.pop_front();
            }
            i++;
            if(posOfPacket.front() == NULL || posOfPacket.front() == -1){
                break;
            }
        }


    }
    else{
        //error
    }
}


void radar_v2::savePackedData(QString path){
    QFile outFile(path);
    QTextStream outFile_stream(&outFile);
    outFile.open(QIODevice::WriteOnly);
    for(int it=0;it <(int) packedData.size(); it++){
        outFile_stream << it << " - " << packedData.at(it) << "\n";
    }
}


void radar_v2::parseData(QString data){
    int n = 0;
    int bO = 2; //because one hex data is represented by two char
    int length = data.length();

        //Wrong Approach
    //outData.vect_iterator = 0;
    //outData.packets_iterator = 0;


    QFile outFile("C:/Users/RPlsicik/Documents/QTCreatorWorkspace/Refactored_rad/ReceivedData/ParsedData/outFile_sortedData.txt");
    outFile.open(QIODevice::WriteOnly);
    QTextStream out(&outFile);

    outData.packets.resize(outData.packets_iterator+1);
    while(n<data.length()){
    //FrameHeaderStructType     defaultPosition
        //outData.frameHeaderStructType_vect.resize(vect_iterator+1);
        if(n== length-500){
            qDebug() << "Here";
        }
        //sync
        if(n < 8*bO){
            //pos = n;
            if(n==0){
                //out << "\n"<< QString::number(outData.packets[outData.packets_iterator].size()) << "/" << QString::number(n)  << " "<< "sync: \t\t\t\t";
                out << "\n"<<"sync: \t\t\t\t";
            }
            out << data.toUtf8().at(n);
            //qDebug() << data.toUtf8().at(n);
            //outData.fHST.sync.append(data.toUtf8().at(n));
            //outData.frameHeaderStructType_vect[vect_iterator].sync.append(data.toUtf8().at(n));
            outData.packets[outData.packets_iterator].packetHeader.sync.append(data.toUtf8().at(n));
        }
        //version
        else if(n < (12*bO)){
            //if(n==9){ofset=9;pos=n-ofset;}
            //QTextStream out(&outFile);
            if(n==8*bO){
                //out << "\n"<< QString::number(outData.packets[outData.packets_iterator].size()) << "/" << QString::number(n)  << " "<< "version: \t\t\t";
                out << "\n"<<"version: \t\t\t";
            }
            out << data.toUtf8().at(n);
            //qDebug() << data.toUtf8().at(n);
            //outData.frameHeaderStructType_vect[vect_iterator].version.append(data.toUtf8().at(n));
            outData.packets[outData.packets_iterator].packetHeader.version.append(data.toUtf8().at(n));
        }
        //platform
        else if(n < (16*bO)){
            //QTextStream out(&outFile);
            if(n==12*bO){
                //out << "\n"<< QString::number(outData.packets[outData.packets_iterator].size()) << "/" << QString::number(n)  << " "<< "platform: \t\t\t";
                out << "\n"<<"platform: \t\t\t";
            }
            out << data.toUtf8().at(n);
            //qDebug() << data.toUtf8().at(n);
            //outData.frameHeaderStructType_vect[vect_iterator].platform.append(data.toUtf8().at(n));
            outData.packets[outData.packets_iterator].packetHeader.platform.append(data.toUtf8().at(n));
        }
        //timestamp
        else if(n < 20*bO){
            //QTextStream out(&outFile);
            if(n==16*bO){
                //out << "\n"<< QString::number(outData.packets[outData.packets_iterator].size()) << "/" << QString::number(n)  << " "<< "timestamp: \t\t";
                out << "\n"<<"timestamp: \t\t";
            }
            out << data.toUtf8().at(n);
            //qDebug() << data.toUtf8().at(n);
            //outData.frameHeaderStructType_vect[vect_iterator].timestamp.append(data.toUtf8().at(n));
            outData.packets[outData.packets_iterator].packetHeader.timestamp.append(data.toUtf8().at(n));
        }
        //packetLength
        else if(n < 24*bO){
            //QTextStream out(&outFile);
            if(n==20*bO){
                //out << "\n"<< QString::number(TLV_packets.size()) << "/" << QString::number(n)  << " "<< "packetLength: \t\t";
                out << "\n"<<"packetLength: \t\t";
            }
            out << data.toUtf8().at(n);
            //qDebug() << data.toUtf8().at(n);
            //outData.frameHeaderStructType_vect[vect_iterator].packetLength.append(data.toUtf8().at(n));
            outData.packets[outData.packets_iterator].packetHeader.packetLength.append(data.toUtf8().at(n));
        }
        //frameNumber
        else if(n < 28*bO){
            //QTextStream out(&outFile);
            if(n==24*bO){
                //out << "\n"<< QString::number(TLV_packets.size()) << "/" << QString::number(n)  << " "<< "frameNumber: \t\t";
                out << "\n"<<"frameNumber: \t\t";
            }
            out << data.toUtf8().at(n);
            //qDebug() << data.toUtf8().at(n);
            //outData.frameHeaderStructType_vect[vect_iterator].frameNumber.append(data.toUtf8().at(n));
            outData.packets[outData.packets_iterator].packetHeader.frameNumber.append(data.toUtf8().at(n));
        }
        //subframeNumber
        else if(n < 32*bO){
            //QTextStream out(&outFile);
            if(n==28*bO){
                //out << "\n"<< QString::number(TLV_packets.size()) << "/" << QString::number(n)  << " "<< "subframeNumber: \t";
                out << "subframeNumber: \t";
            }
            out << data.toUtf8().at(n);
            //qDebug() << data.toUtf8().at(n);
            //outData.frameHeaderStructType_vect[vect_iterator].subframeNumber.append(data.toUtf8().at(n));
            outData.packets[outData.packets_iterator].packetHeader.subframeNumber.append(data.toUtf8().at(n));

        }
        //chirpMargin
        else if(n < 36*bO){
            //QTextStream out(&outFile);
            if(n==32*bO){
                //out << "\n"<< QString::number(TLV_packets.size()) << "/" << QString::number(n)  << " "<< "chirpMargin: \t\t";
                out << "\n"<<"chirpMargin: \t\t";
            }
            out << data.toUtf8().at(n);
            //qDebug() << data.toUtf8().at(n);
            //outData.frameHeaderStructType_vect[vect_iterator].chirpMargin.append(data.toUtf8().at(n));
            outData.packets[outData.packets_iterator].packetHeader.chirpMargin.append(data.toUtf8().at(n));
        }
        //frameMargin
        else if(n < 40*bO){
            //QTextStream out(&outFile);
            if(n==36*bO){
                //out << "\n"<< QString::number(TLV_packets.size()) << "/" <<  QString::number(n) << " " << "frameMargin: \t\t";
                out << "\n"<<"frameMargin: \t\t";
            }
            out << data.toUtf8().at(n);
            //qDebug() << data.toUtf8().at(n);
            //outData.frameHeaderStructType_vect[vect_iterator].frameMargin.append(data.toUtf8().at(n));
            outData.packets[outData.packets_iterator].packetHeader.frameMargin.append(data.toUtf8().at(n));
        }
        //uartSendTime
        else if(n < 44*bO){
            //QTextStream out(&outFile);
            if(n==40*bO){
                //out << "\n"<< QString::number(TLV_packets.size()) << "/" << QString::number(n)  << " " << "uartSendTime: \t\t";
                out << "\n"<<"uartSendTime: \t\t";
            }
            out << data.toUtf8().at(n);
            //qDebug() << data.toUtf8().at(n);
            //outData.frameHeaderStructType_vect[vect_iterator].uartSendTime.append(data.toUtf8().at(n));
            outData.packets[outData.packets_iterator].packetHeader.uartSendTime.append(data.toUtf8().at(n));
        }
        //trackProcessTime
        else if(n < 48*bO){ //orig val n < 48*bO
            //QTextStream out(&outFile);
            if(n==44*bO){
                //out << "\n"<< QString::number(TLV_packets.size()) << "/" << QString::number(n)  << " " << "trackProcessTime: \t";
                out << "\n"<<"trackProcessTime: \t";
            }
            out << data.toUtf8().at(n);
            //qDebug() << data.toUtf8().at(n);
            //outData.frameHeaderStructType_vect[vect_iterator].trackProcessTime.append(data.toUtf8().at(n));
            outData.packets[outData.packets_iterator].packetHeader.trackProcessTime.append(data.toUtf8().at(n));
        }

        // !!! //numTLVs
        else if(n < 52*bO){
            //QTextStream out(&outFile);
            if(n==48*bO){
                //out << "\n"<< QString::number(TLV_packets.size()) << "/" << QString::number(n)  << " " << "numTLVs: \t\t\t";
                out << "\n"<<"numTLVs: \t\t\t";
            }
            out << data.toUtf8().at(n);
            //qDebug() << data.toUtf8().at(n);
            //outData.frameHeaderStructType_vect[vect_iterator].numTLVs.append(data.toUtf8().at(n));
            outData.packets[outData.packets_iterator].packetHeader.numTLVs.append(data.toUtf8().at(n));
        }
        //checksum
        //else if((n < 52*bO) && (n < 56*bO)){
        else if((n > 51*bO) && (n < 54*bO)){
            //QTextStream out(&outFile);
            if(n==52*bO){
                //out << "\n"<< QString::number(TLV_packets.size()) << "/" << QString::number(n)  << " " << "checksum: \t\t";
                out << "\n"<<"checksum: \t\t";
            }
            out << data.toUtf8().at(n);
            //qDebug() << data.toUtf8().at(n);
            //outData.fHST.checksum.append(data.toUtf8().at(n));
            //outData.frameHeaderStructType_vect[vect_iterator].checksum.append(data.toUtf8().at(n));
            outData.packets[outData.packets_iterator].packetHeader.checksum.append(data.toUtf8().at(n));
        }
        //TLV_Header OR Point Cloud OR TargetObject dependent on type
        //else if((n > 56*bO)&&(n<=length)){

        else if((n<=length)){
            //if(n==52*bO){
            if(n<=length-25){
                //out << "\n"<< QString::number(TLV_packets.size()) << "/" << QString::number(n)  << " " << "Other: \t\t\n";
                //TLV Header 4+4 = 8
                    int i = 0;
                    //outData.tlvHeader_vect.resize(vect_iterator+1);
                    //outData.pointCloud_vect.resize(vect_iterator+1);
                    outData.packets[outData.packets_iterator].tlvHeader_vect.resize(outData.vect_iterator+1);
                    outData.packets[outData.packets_iterator].pointCloud_vect.resize(outData.vect_iterator+1);
                    while(i<4){
                        outData.packets[outData.packets_iterator].tlvHeader_vect[outData.vect_iterator].type.append(data.toUtf8().at(n));
                        i++;
                        n++;
                    }
                        out << "\n type:" << outData.packets[outData.packets_iterator].tlvHeader_vect[outData.vect_iterator].type;
                    while(i>=4 && i<8){
                        outData.packets[outData.packets_iterator].tlvHeader_vect[outData.vect_iterator].length.append(data.toUtf8().at(n));
                        i++;
                        n++;
                    }
                        out << "\n length:" << outData.packets[outData.packets_iterator].tlvHeader_vect[outData.vect_iterator].length;
                    //PointCloud
                    //if(outData.packets[packets_iterator].tlvHeader_vect[vect_iterator].type.compare("0600") == 0){
                    if(outData.packets[outData.packets_iterator].tlvHeader_vect[outData.vect_iterator].type.contains("6") == 1){
                        //Point Cloud 4+4+4+4 = 16
                            outData.packets[outData.packets_iterator].pointCloud_vect.resize(outData.vect_iterator+1);
                            while(i>=8 && i<24){
                                if(i>=8 && i<12){    //range, in m
                                    //outData.point_cloud.range.append(data.toUtf8().at(n));
                                    outData.packets[outData.packets_iterator].pointCloud_vect[outData.vect_iterator].range.append(data.toUtf8().at(n));
                                }
                                else if(i>=12 && i<16){    //azimuth, in rad
                                    outData.packets[outData.packets_iterator].pointCloud_vect[outData.vect_iterator].azimuth.append(data.toUtf8().at(n));
                                }
                                else if(i>=16 && i<20){    //Doppler, in m/s
                                    outData.packets[outData.packets_iterator].pointCloud_vect[outData.vect_iterator].doppler.append(data.toUtf8().at(n));
                                }
                                else if(i>=20 && i<24){    //SNR, ratio
                                    outData.packets[outData.packets_iterator].pointCloud_vect[outData.vect_iterator].snr.append(data.toUtf8().at(n));
                                }
                                i++;
                                n++;
                            }
                            out << "\n range: " << outData.packets[outData.packets_iterator].pointCloud_vect[outData.vect_iterator].range;
                            out << "\n azimuth: " << outData.packets[outData.packets_iterator].pointCloud_vect[outData.vect_iterator].azimuth;
                            out << "\n doppler: " << outData.packets[outData.packets_iterator].pointCloud_vect[outData.vect_iterator].doppler;
                            out << "\n snr: " << outData.packets[outData.packets_iterator].pointCloud_vect[outData.vect_iterator].snr;


                            //outData.pointCloud_vect[vect_iterator].range = outData.point_cloud.range;
                            //outData.pointCloud_vect[vect_iterator].azimuth = outData.point_cloud.azimuth;
                            //outData.pointCloud_vect[vect_iterator].doppler = outData.point_cloud.doppler;
                            //outData.pointCloud_vect[vect_iterator].snr = outData.point_cloud.snr;


                    }
                    //Target Object List
                    //else if(outData.packets[packets_iterator].tlvHeader_vect[vect_iterator].type.compare("0700") == 0){
                    else if(outData.packets[outData.packets_iterator].tlvHeader_vect[outData.vect_iterator].type.contains("7") == 1){
                        //Track ID
                        while(i<24 && i<24){

                        }
                        //posX
                        //posY
                        //velX
                        //velY
                        //accX
                        //accY
                        //EC - Error covariance matrix 3x3
                        while(i > 16  && i<=36){            //Target Object 4+4+4+4+4+4+4+9*4+4 = 36
                            out << " \nTargetObjectList: \t" << data.toUtf8().at(n);
                            outData.packets[outData.packets_iterator].targetObject_vect[outData.vect_iterator].G.append(data.toUtf8().at(n));
                            i++;
                            n++;
                        }
                        //G - gating function


                    }
                    //Target index
                    //else if(outData.packets[packets_iterator].tlvHeader_vect[vect_iterator].type.compare("0800") == 0){
                    else if(outData.packets[outData.packets_iterator].tlvHeader_vect[outData.vect_iterator].type.contains("8") == 1){
                        //Target Object list 1 byte
                            while(i > 36){
                                out << " \nTargetObjectList: \t" << data.toUtf8().at(n);
                                //outData.targetObject_vect[vect_iterator].EC.append(data.toUtf8().at(n));
                                outData.packets[outData.packets_iterator].targetIndex_vect[outData.vect_iterator].id.append(data.toUtf8().at(n));
                                i++;
                                n++;
                            }
                    }
                    //outData.vect_iterator++;


                    //The TLV data
                    /*
                    else{

                    }
                    */
                //out << "\n"<< "Other: \t\t\n";
            }
            out <<"\n other:" <<  data.toUtf8().at(n);
            //qDebug() << n <<"/"<<length << ":" << data.toUtf8().at(n);
        }

        else{
            break;
        }
        n++;

    }

    outData.vect_iterator++;    //added for incrementation of vector
    outData.packets_iterator++;

    //TLV_packets.push_back(outData); //old one
    outData.clear();
}
void radar_v2::saveData_byType(QString path){
    QString warningClear = path;
}




void radar_v2::readFromFile(QString path){
    /*
    buffer_DATA_port.append(DATA_port->readAll());
    QString ssync = "0201040306050807";
    QByteArray sync = QByteArray::fromHex("0201040306050807");
    //QByteArrayView sync = ssync.toUtf8();
    int count = buffer_DATA_port.indexOf(sync);
     */

    QFile file(path);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text) == true){

        QByteArray sync = QByteArray::fromHex("0201040306050807");
        QByteArray file_output = file.readAll();
        QString temp;
        temp = file_output.toHex();

        QFile out("C:/Users/RPlsicik/Documents/QTCreatorWorkspace/Refactored_rad/ReceivedData/tst.txt");
        QTextStream out_stream(&out);
        out.open(QIODevice::WriteOnly);
        out_stream << file_output.toHex();

    //Finding sync
        std::deque<int> posOfPacket;
        int it = 0;
        int length = temp.size();
        while(it < length) {
            posOfPacket.push_back(temp.indexOf("0201040306050807",it));
            it = posOfPacket.back();// + 1 ;
            //if(it == -1 || it == 0){
            if(it == -1){
                break;
            }
            it++;
        }
    //Filling packet;
        QString hexVal;
        int i = posOfPacket.front();
        posOfPacket.pop_front();
        while(i<temp.length()){
            if(i < posOfPacket.front()-1){
                hexVal.append(temp.at(i));
            }
            else{
                //hexVal.append(temp.at(i));
                packedData.push_back(hexVal);
                hexVal.clear();
                posOfPacket.pop_front();
            }
            i++;
            if(posOfPacket.front() == NULL || posOfPacket.front() == -1){
                break;
            }
        }


    }
    else{
        //error
    }
}
