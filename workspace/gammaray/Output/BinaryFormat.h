#ifndef BINARYFORMAT_H
#define BINARYFORMAT_H

#include <stdint.h>

#pragma pack(push, 1)

struct GrayBinaryStandard {
    double time;
    float energy;
    float x,y,z;
    int log;
    int i;
    int det_id;
};
#pragma pack(pop)

struct GrayBinaryNoPosition {
    double time;
    float energy;
    int log;
    int i;
    int det_id;
};

#endif /*BINARYFORMAT_*/
