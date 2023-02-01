#ifndef TLV_DAT_H
#define TLV_DAT_H

#include "QByteArray"
#include "QString"

#include "deque"
#include "vector"

class TLV_dat
{
public:
    TLV_dat();
    ~TLV_dat();
    int variable;
    int vect_iterator = 0;
    int packets_iterator = 0;

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
        };//fHST;

        struct  TLV_Header{
            //TLV Type: 06 = Point cloud, 07 = Target object list, 08 = Target index
            QByteArray  type;                //56B    //TLV object
            QByteArray  length;              //60B    //TLV object Length, in bytes, including TLV header
        };//tlv_header;

        struct PointCloud{      //more PC in one packet                                                             x06
            QByteArray  range;               //64B    //Range, in m
            QByteArray  azimuth;             //68B    //Angle, in rad
            QByteArray  doppler;             //72B    //Doppler, in m/s
            QByteArray  snr;                 //76B    //SNR, ratio
        };//point_cloud;

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
        };//target_object;
        struct TargetIndex{     //more TI in one packet                                                             x08
            QByteArray id;                         //148B   //track ID
            /*
             * 253  -   Point not associated, SNR too weak
             * 254  -   Point not associated, located outside boundary of interest
             * 255  -   Point not associated, considered as noise
             */
        };//target_index;

        struct Packet{
            frameHeaderStructType packetHeader;
            std::vector<TLV_Header> tlvHeader_vect;
            std::vector<TargetObject> targetObject_vect;
            std::vector<TargetIndex> targetIndex_vect;
            std::vector<PointCloud> pointCloud_vect;
        };

        std::vector<Packet> packets;




        void clear();
private:
};
#endif // TLV_DAT_H
