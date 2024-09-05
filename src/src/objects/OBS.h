//
// Created by tmsz on 2023/1/8.
//

#ifndef SKI_OBS_H
#define SKI_OBS_H

#include "SDL2/SDL.h"
#include <stdlib.h>
#include "SDL2/SDL_image.h"

struct OBS_Obstacle
{
    int x;
    int y;
};

struct OBS_Obstacle* OBS_CreateObstacle();

void OBS_DestroyObstacle(struct OBS_Obstacle* obs);

#endif //SKI_OBS_H
