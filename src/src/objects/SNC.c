//
// Created by tmsz on 2023/1/20.
//

#include "SNC.h"

struct SNC_SnowCrash* SNC_CreateSnowCrash()
{
    struct SNC_SnowCrash* snc = malloc(sizeof (struct SNC_SnowCrash));
    return snc;
}

void SNC_DestroySnowCrash(struct SNC_SnowCrash* snc)
{
    free(snc);
}