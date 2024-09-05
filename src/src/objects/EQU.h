//
// Created by tmsz on 2023/1/7.
//

#ifndef SKI_EQU_H
#define SKI_EQU_H

#define gravity 0.4
#define equ_lowest_speed 8
#define equ_resistance_snow 0.2

#include "SDL2/SDL.h"
#include <stdlib.h>
#include "SDL2/SDL_image.h"
#include <math.h>

struct EQU_Equipment
{
    int x;
    int y;
    double speed_x;
    double speed_y;
    double acceleration_x;
    double acceleration_y;
    int in_ground;
    int Type;
};

enum Type
{
    DROPLET,//0
    FLAME,//1
    FIST1,//2
    FIST2//3
};

struct EQU_Equipment* EQU_CreateEquipment();

void EQU_EquipmentGo(struct EQU_Equipment* EQU_equipment, int Noise[10400]);

void EQU_DestroyEquipment(struct EQU_Equipment* equ);

double EQU_GetSlope(struct EQU_Equipment *Equipment, int Noise[10400]);

void EQU_CalculateAcceleration(struct EQU_Equipment *EQU_equipment, int Noise[10400]);

void EQU_KeepTheLowestSpeed(struct EQU_Equipment *EQU_equipment, int Noise[10400]);

#endif //SKI_EQU_H
