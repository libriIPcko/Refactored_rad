#include "tlv_dat.h"

TLV_dat::TLV_dat(){

}
TLV_dat::~TLV_dat(){

}

void TLV_dat::clear(){
/*
    fHST.sync =  0;                //1B     //syncPattern in hex is: '02 01 04 03 06 05 08 07'
    fHST.version =  0;             //8B     //0xA1642 or 0xA1443
    fHST.platform =  0;            //12B    //See description below
    fHST.timestamp =  0;           //16B    //600MHz free running clocks
    fHST.packetLength =  0;        //20B    //In bytes, including header
    fHST.frameNumber =  0;         //24B    //Starting from 1
    fHST.subframeNumber =  0;      //28B
    fHST.chirpMargin =  0;         //32B    //Chirp Processing margin, in ms
    fHST.frameMargin =  0;         //36B    //Frame Processing margin, in ms
    fHST.uartSendTime =  0;        //40B    //Time spent to send data, in ms
    fHST.trackProcessTime =  0;    //44B    //Tracking Processing time, in ms
    fHST.numTLVs =  0;             //48B    //Number of TLVs in thins frame
    fHST.checksum =  0;            //52B    //Header checksum

    tlv_header.type =  0;                //56B    //TLV object
    tlv_header.length =  0;              //60B    //TLV object Length, in bytes, including TLV header

    point_cloud.range= 0;               //64B    //Range, in m
    point_cloud.azimuth= 0;             //68B    //Angle, in rad
    point_cloud.doppler= 0;             //72B    //Doppler, in m/s
    point_cloud.snr= 0;                 //76B    //SNR, ratio

    target_object.tid= 0;                 //80B    //Track ID
    target_object.posX= 0;                //84B    //Target position in X dimension, m
    target_object.posY= 0;                //88B    //Target position in Y dimension, m
    target_object.velX= 0;                //92B    //Target velocity in X dimension, m
    target_object.velY= 0;                //96B    //Target velocity in Y dimension, m
    target_object.accX= 0;                //100B   //Target acceleration in X dimension, m/s2
    target_object.accY= 0;                //104B   //Target acceleration in Y dimension, m/s2
    target_object.EC= 0;                  //108B   //Error covariance matrix[3x3], in range/angle/doppler coordinates
    target_object.G= 0;                   //144B   //Gating function gain

    target_index.id= 0;                    //148B   //track ID
*/
}
