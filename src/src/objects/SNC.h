//
// Created by tmsz on 2023/1/20.
//

#ifndef SKI_SNC_H
#define SKI_SNC_H

#include <stdlib.h>

struct SNC_SnowCrash
{
    int x;
    int y;
    double speed;
};

struct SNC_SnowCrash* SNC_CreateSnowCrash();

void SNC_DestroySnowCrash(struct SNC_SnowCrash* snc);

#endif //SKI_SNC_H
