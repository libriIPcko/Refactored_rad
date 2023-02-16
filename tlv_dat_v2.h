#ifndef TLV_DAT_V2_H
#define TLV_DAT_V2_H

#include <vector>
#include <deque>
#include <QString>

class tlv_dat_v2
{
public:
    tlv_dat_v2();


    //std::vector<QString> TLV_PointCloude;
    //std::vector<QString> TLV_TaretObject;
    //std::vector<QString> TLV_TargetIndex;

    struct packet{
        QString header;
        std::deque<QString> TLV_PointCloud;
        std::deque<QString> TLV_TaretObject;
        std::deque<QString> TLV_TargetIndex;
        std::deque<QString> TLV_nonIdentified;

    };

    std::deque<QString> header;
    std::deque<packet>  object_prop;

};



#endif // TLV_DAT_V2_H
