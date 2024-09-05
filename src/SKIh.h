//
// Created by tmsz on 2023/1/7.
//

#ifndef SKI_SKIH_H
#define SKI_SKIH_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "src/objects/PLR.h"
#include "src/objects/EQU.h"
#include "src/objects/DEC.h"
#include "src/objects/OBS.h"
#include "src/objects/SNC.h"

#define bgw 800
#define bgh 500
#define height1 500
#define height2 500
#define height3 400
#define height4 300
#define pi 3.1415926
#define gravity 0.4
#define resistance_snow 0.2
#define fall_down_resistance_snow 0.8
#define jump_height 9
#define jumpCD 500
#define wait_event 1
#define ms_per_frame 25
#define delta_distance 8
#define rotate_speed 5
#define fall_down_delay_ms 3000
#define average_obstacle_interval 50
#define average_decoration_interval 17
#define average_equipment_interval 10
#define add_speed_of_per_equipment 2
#define highest_slope_when_obstacle 0.1
#define snow_crash_initial_speed 10
#define snow_crash_acceleration 0.05
#define warning_distance 2000
#define critical_warning_distance 1000

struct XYf{
    int x;
    int y;
    int facing;
};

int Noise[10400];//13*bgw->12+1
int HaveObstacle;
int DecorationPtr;
int HaveEquipment;
int lowest_speed;
int HaveNotDefeat;
int score;
int Warning_x;
int Warning_y;
struct DEC_Decoration* DecorationArray[100];
uint32_t FallDownBeginTick;
uint32_t JumpBeginTick;
struct XYf trace[10];
uint32_t SnowCrashBeginTick;

SDL_Window *Window = NULL;
SDL_Renderer *Renderer = NULL; //渲染器
SDL_Event MainEvent; //主事件
const Uint8 *KeyValue = NULL;
struct PLR_Player* Player = NULL;
SDL_Surface *PlayerSurface = NULL;
SDL_Surface *DecorationSurface1 = NULL;
SDL_Surface *DecorationSurface2 = NULL;
SDL_Surface *TraceSurface = NULL;
struct OBS_Obstacle* Obstacle = NULL;
SDL_Surface *ObstacleSurface = NULL;
struct EQU_Equipment* Equipment = NULL;
SDL_Surface *EquipmentSurface = NULL;
struct SNC_SnowCrash* SnowCrash = NULL;
SDL_Surface *SnowCrashSurface = NULL;
SDL_Surface *WarningSurface = NULL;
TTF_Font *ScoreFont = NULL;
SDL_Surface *ScoreSurface = NULL;
SDL_Color ScoreFontColor = {255,255,255,255};
SDL_Surface *WaitBeginSurface = NULL;
SDL_Color WaitBeginColor = {0,0,0,255};
SDL_Surface *PauseSurface = NULL;
SDL_Surface *BeginPlayerSurface = NULL;
SDL_Surface *ScoreboardSurface = NULL;

void InitSDL();

void Init();

void Draw(SDL_Surface* Surface,int x,int y,int w,int h,int angle);

void DrawByTransparency(SDL_Surface* Surface,int x,int y,int w,int h,int angle,int transparency);

void Quit();

double Distance(int x1,int y1,int x2,int y2);

void CalculateNoise();//计算噪声

void Move();

void MoveNoise();

void CopyNoise();

double SmoothFunction(double per);//光滑函数

void RendMount();

void InitNoise();

void PlayerGo();

void RendPlayer();

double GetSlope();//获取角色所在的坡度,返回值为正切值,只在inGround=1使用

double GetSlopeByX(int x);//获取给定x值所在坡度，返回正切

void CalculateAcceleration();

void KeepTheLowestSpeed();//在不摔倒的前提下，雪地上保持最低速度

void JumpOrRotate();


void InitDecoration();

void RendDecoration();

void MoveDecoration();

void DecorationMain();


void InitTrace();

void SaveTrace();

void RendTrace();

void MoveTrace();


void DetectUpsideDown();//检查是否由于朝向而摔倒

void DetectObstacle();//检查是否由于撞上障碍而摔倒

void FallDownMain();


void InitObstacle();

void RendObstacle();

void MoveObstacle();

void ObstacleMain();


void InitEquipment();

void RendEquipment();

void MoveEquipment();

void EquipmentMain();


void UploadWhenEquipment();//在拾取/掉落装备时更新的信息

void UploadPlayerSurfaceWhenEquipment();//更新装备信息

void UploadLowestSpeed();//更新最低速度

void UploadTrace();//更新轨迹纹理


void NotEqualToZero();


void InitSnowCrash();

void RendSnowCrash();

void SnowCrashMain();

void SnowCrashGo();

void MoveSnowCrash();

void DetectDefeat();

void AfterDefeat();

void RendWarning();

void WarningMove();

void RendScore();

void Pause();

void Begin();

void WaitBegin();

void Scoreboard();

void FreeAll();

#endif //SKI_SKIH_H