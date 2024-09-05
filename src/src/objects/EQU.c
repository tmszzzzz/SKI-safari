//
// Created by tmsz on 2023/1/7.
//

#include "EQU.h"

struct EQU_Equipment* EQU_CreateEquipment()
{
    struct EQU_Equipment* equ = malloc(sizeof(struct EQU_Equipment));
    return equ;
}

void EQU_EquipmentGo(struct EQU_Equipment* EQU_equipment, int Noise[10400])
{
    if(EQU_equipment->in_ground)
    {//地上
        double Slope = EQU_GetSlope(EQU_equipment, Noise);
        EQU_CalculateAcceleration(EQU_equipment, Noise);
        EQU_equipment->speed_x += EQU_equipment->acceleration_x;
        EQU_equipment->speed_y += EQU_equipment->acceleration_y;
        if(EQU_equipment->speed_y / EQU_equipment->speed_x < Slope)
        {//需要分解速度,inGround保持1
            double ang_v = atan(EQU_equipment->speed_y / EQU_equipment->speed_x);
            double ang_slope = atan(Slope);
            double ang_0 = ang_slope-ang_v;
            double v = cos(ang_0)*sqrt(pow(EQU_equipment->speed_x, 2) + pow(EQU_equipment->speed_y, 2));
            EQU_equipment->speed_x = v * cos(ang_slope);
            EQU_equipment->speed_y = v * sin(ang_slope);
            EQU_KeepTheLowestSpeed(EQU_equipment, Noise);
            EQU_equipment->x+=(int)EQU_equipment->speed_x;
            EQU_equipment->y = (Noise)[EQU_equipment->x];
        }else
        {//无需分解速度,inGround变为0
            EQU_equipment->in_ground = 0;
            EQU_equipment->x+=(int)EQU_equipment->speed_x;
            EQU_equipment->y+=(int)EQU_equipment->speed_y;
        }
    }else//空中
    {
        EQU_CalculateAcceleration(EQU_equipment, Noise);
        EQU_equipment->speed_x += EQU_equipment->acceleration_x;
        EQU_equipment->speed_y += EQU_equipment->acceleration_y;
        if((Noise)[EQU_equipment->x + (int)EQU_equipment->speed_x] >= EQU_equipment->y + (int)EQU_equipment->speed_y)
        {//撞上了,inGround变为1
            EQU_equipment->in_ground = 1;
            EQU_equipment->x += (int)EQU_equipment->speed_x;
            EQU_equipment->y = (Noise)[EQU_equipment->x];
            double ang_v = atan(EQU_equipment->speed_y / EQU_equipment->speed_x);
            double ang_slope = atan(EQU_GetSlope(EQU_equipment, Noise));
            double ang_0 = ang_slope-ang_v;
            double v = cos(ang_0)*sqrt(pow(EQU_equipment->speed_x, 2) + pow(EQU_equipment->speed_y, 2));
            EQU_equipment->speed_x = v * cos(ang_slope);
            EQU_equipment->speed_y = v * sin(ang_slope);
            EQU_KeepTheLowestSpeed(EQU_equipment, Noise);
        }else
        {//没撞上,inGround保持0
            EQU_equipment->x+=(int)EQU_equipment->speed_x;
            EQU_equipment->y+=(int)EQU_equipment->speed_y;
        }
    }
}

double EQU_GetSlope(struct EQU_Equipment *EQU_equipment, int Noise[10400])
{
    return ((double)((Noise)[EQU_equipment->x]-(Noise)[EQU_equipment->x-15])/15);
}

void EQU_CalculateAcceleration(struct EQU_Equipment *EQU_equipment, int Noise[10400])
{
    EQU_equipment->acceleration_x = 0;
    EQU_equipment->acceleration_y = 0;
    EQU_equipment->acceleration_y -= gravity;
    if(EQU_equipment->in_ground)
    {
        double ang_slp = atan(EQU_GetSlope(EQU_equipment, Noise));
        EQU_equipment->acceleration_x-= equ_resistance_snow * cos(ang_slp);
        EQU_equipment->acceleration_y-= equ_resistance_snow * sin(ang_slp);
    }

}

void EQU_KeepTheLowestSpeed(struct EQU_Equipment *EQU_equipment, int Noise[10400])
{
    double v = sqrt(pow(EQU_equipment->speed_x, 2) + pow(EQU_equipment->speed_y, 2));
    if(EQU_equipment->speed_x <= 0)
    {
        EQU_equipment->speed_x = equ_lowest_speed*cos(atan(EQU_GetSlope(EQU_equipment,Noise)));
        EQU_equipment->speed_y = equ_lowest_speed*sin(atan(EQU_GetSlope(EQU_equipment,Noise)));
    }
    if(v<equ_lowest_speed)
    {
        EQU_equipment->speed_x *= equ_lowest_speed / v;
        EQU_equipment->speed_y *= equ_lowest_speed / v;
    }
}

void EQU_DestroyEquipment(struct EQU_Equipment* equ)
{
    free(equ);
}