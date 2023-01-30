#ifndef TLV_DAT_H
#define TLV_DAT_H

#include "QByteArray"
#include "QString"
#include "vector"

class TLV_dat
{
public:
    TLV_dat();
    ~TLV_dat();
    int variable;

    //    datType       name            startBytN.O.                description
        struct frameHeaderStructType{
            //std::array<QByteArray,8>  sync;   //1B     //syncPattern in hex is: '02 01 04 03 06 05 08 07'
//          QByteArray  sync[8];             //1B     //syncPattern in hex is: '02 01 04 03 06 05 08 07'
            QByteArray  sync;                //1B     //syncPattern in hex is: '02 01 04 03 06 05 08 07'
            QByteArray  version;             //8B     //0xA1642 or 0xA1443
            QByteArray  platform;            //12B    //See description below
            QByteArray  timestamp;           //16B    //600MHz free running clocks
            QByteArray  packetLength;        //20B    //In bytes, including header
            QByteArray  frameNumber;         //24B    //Starting from 1
            QByteArray  subframeNumber;      //28B
            QByteArray  chirpMargin;         //32B    //Chirp Processing margin, in ms
            QByteArray  frameMargin;         //36B    //Frame Processing margin, in ms
            QByteArray  uartSendTime;        //40B    //Time spent to send data, in ms
            QByteArray  trackProcessTime;    //44B    //Tracking Processing time, in ms
            QByteArray  numTLVs;             //48B    //Number of TLVs in thins frame
            QByteArray  checksum;            //52B    //Header checksum
        }fHST;

        struct  TLV_Header{
            //TLV Type: 06 = Point cloud, 07 = Target object list, 08 = Target index
            QByteArray  type;                //56B    //TLV object
            QByteArray  length;              //60B    //TLV object Length, in bytes, including TLV header
        }tlv_header;

        struct PointCloud{      //more PC in one packet                                                             x06
            QByteArray  range;               //64B    //Range, in m
            QByteArray  azimuth;             //68B    //Angle, in rad
            QByteArray  doppler;             //72B    //Doppler, in m/s
            QByteArray  snr;                 //76B    //SNR, ratio
        }point_cloud;

        struct TargetObject{    //more TO in one packet                                                             x07
            QByteArray  tid;                 //80B    //Track ID
            QByteArray  posX;                //84B    //Target position in X dimension, m
            QByteArray  posY;                //88B    //Target position in Y dimension, m
            QByteArray  velX;                //92B    //Target velocity in X dimension, m
            QByteArray  velY;                //96B    //Target velocity in Y dimension, m
            QByteArray  accX;                //100B   //Target acceleration in X dimension, m/s2
            QByteArray  accY;                //104B   //Target acceleration in Y dimension, m/s2
            QByteArray  EC;                 //108B   //Error covariance matrix[3x3], in range/angle/doppler coordinates
            QByteArray  G;                   //144B   //Gating function gain
        }target_object;
        struct TargetIndex{     //more TI in one packet                                                             x08
            uint8_t id;                         //148B   //track ID
            /*
             * 253  -   Point not associated, SNR too weak
             * 254  -   Point not associated, located outside boundary of interest
             * 255  -   Point not associated, considered as noise
             */
        }target_index;

        std::vector<TLV_Header> tlvHeader_vect;
        std::vector<TargetObject> targetObject_vect;
        std::vector<TargetIndex> targetIndex_vect;
        std::vector<PointCloud> pointCloud_vect;
private:
};
#endif // TLV_DAT_H


/*

    //    datType       name            startBytN.O.                description
        struct frameHeaderStructType{
            //std::array<QByteArray,8>  sync;   //1B     //syncPattern in hex is: '02 01 04 03 06 05 08 07'
//          QByteArray  sync[8];                   //1B     //syncPattern in hex is: '02 01 04 03 06 05 08 07'
            QByteArray  sync[8];                   //1B     //syncPattern in hex is: '02 01 04 03 06 05 08 07'
            QByteArray  version[4];             //8B     //0xA1642 or 0xA1443
            QByteArray  platform[4];            //12B    //See description below
            QByteArray  timestamp[4];           //16B    //600MHz free running clocks
            QByteArray  packetLength[4];        //20B    //In bytes, including header
            QByteArray  frameNumber[4];         //24B    //Starting from 1
            QByteArray  subframeNumber[4];      //28B
            QByteArray  chirpMargin[4];         //32B    //Chirp Processing margin, in ms
            QByteArray  frameMargin[4];         //36B    //Frame Processing margin, in ms
            QByteArray  uartSendTime[4];        //40B    //Time spent to send data, in ms
            QByteArray  trackProcessTime[4];    //44B    //Tracking Processing time, in ms
            QByteArray  numTLVs[4];             //48B    //Number of TLVs in thins frame
            QByteArray  checksum[4];            //52B    //Header checksum
        }fHST;
        struct  TLV_Header{
            //TLV Type: 06 = Point cloud, 07 = Target object list, 08 = Target index
            QByteArray  type[4];                //56B    //TLV object
            QByteArray  length[4];              //60B    //TLV object Length, in bytes, including TLV header
        }tlv_header;
        struct PointCloud{      //more PC in one packet                                                             x06
            QByteArray  range[4];               //64B    //Range, in m
            QByteArray  azimuth[4];             //68B    //Angle, in rad
            QByteArray  doppler[4];             //72B    //Doppler, in m/s
            QByteArray  snr[4];                 //76B    //SNR, ratio
        }point_cloud;
        struct TargetObject{    //more TO in one packet                                                             x07
            QByteArray  tid[4];                 //80B    //Track ID
            QByteArray  posX[4];                //84B    //Target position in X dimension, m
            QByteArray  posY[4];                //88B    //Target position in Y dimension, m
            QByteArray  velX[4];                //92B    //Target velocity in X dimension, m
            QByteArray  velY[4];                //96B    //Target velocity in Y dimension, m
            QByteArray  accX[4];                //100B   //Target acceleration in X dimension, m/s2
            QByteArray  accY[4];                //104B   //Target acceleration in Y dimension, m/s2
            QByteArray  EC[36];                 //108B   //Error covariance matrix[3x3], in range/angle/doppler coordinates
            QByteArray  G[4];                   //144B   //Gating function gain
        }target_object;
        struct TargetIndex{     //more TI in one packet                                                             x08
            uint8_t id;                         //148B   //track ID

             // 253  -   Point not associated, SNR too weak
             // 254  -   Point not associated, located outside boundary of interest
             // 255  -   Point not associated, considered as noise

       }target_index;*/
/**/
