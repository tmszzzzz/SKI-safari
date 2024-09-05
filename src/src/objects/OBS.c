//
// Created by tmsz on 2023/1/8.
//

#include "OBS.h"

struct OBS_Obstacle* OBS_CreateObstacle()
{
    struct OBS_Obstacle* obs = malloc(sizeof(struct OBS_Obstacle));
    return obs;
}

void OBS_DestroyObstacle(struct OBS_Obstacle* obs)
{
    free(obs);
}