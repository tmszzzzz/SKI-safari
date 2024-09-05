//
// Created by tmsz on 2023/1/7.
//

#ifndef SKI_PLR_H
#define SKI_PLR_H

#include <stdlib.h>

struct PLR_Player
{
    int x;
    int y;
    double speed_x;
    double speed_y;
    double acceleration_x;
    double acceleration_y;
    int in_ground;
    int equipment[4];
    int delta_rotation;
    int whether_fall_down;
};

struct PLR_Player* PLR_Create();//创建玩家

void PLR_Destroy(struct PLR_Player* P);//销毁玩家

#endif //SKI_PLR_H
