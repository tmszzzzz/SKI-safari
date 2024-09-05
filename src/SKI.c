#include "SKIh.h"
#undef main

int main(int argc, char *argv[])
{
    InitSDL();
    while(1) {
        Init();
        InitNoise();
        CalculateNoise();
        CopyNoise();
        WaitBegin();
        Begin();

        while (HaveNotDefeat) {
            uint32_t begin = SDL_GetTicks();

            SDL_WaitEventTimeout(&MainEvent, wait_event);//事件
            JumpOrRotate();//跳远/旋转
            PlayerGo();//角色移动
            if (Player->x >= 10000) {
                Move();//如果到达了最后一个屏幕长度，生成新一轮地形，并将所有元素移动回第一个屏幕长度
            }
            RendMount();//渲染山
            DetectObstacle();//撞石判定（颠倒判定掺杂在JumpOrRotate()和PlayerGo()中）
            FallDownMain();//摔倒与起身时间、行为控制
            DecorationMain();//装饰物（树）的随机生成
            ObstacleMain();//障碍的随机生成
            EquipmentMain();//装备的随机生成
            RendTrace();//渲染轨迹
            SaveTrace();//更新轨迹形状
            RendPlayer();//渲染角色
            SnowCrashMain();//雪崩
            RendWarning();
            WarningMove();
            RendScore();
            SDL_RenderPresent(Renderer);
            Pause();

            uint32_t end = SDL_GetTicks();
            int cost = (int) end - (int) begin;
            if (ms_per_frame - cost > 0) SDL_Delay(ms_per_frame - cost);//帧频控制
        }
        while (SnowCrash->x < Player->x + 400) {
            AfterDefeat();
        }
        FreeAll();
        Scoreboard();
    }
}

void InitSDL()
{
    TTF_Init();
    KeyValue = SDL_GetKeyboardState(NULL);
    // 初始化随机数生成器
    srand((unsigned int)time(NULL));
    SDL_Init(SDL_INIT_VIDEO); //初始化
    Window = SDL_CreateWindow("SKI", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, bgw, bgh, SDL_WINDOW_SHOWN);
    //开始背景
    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);//初始化渲染器
}

void Init()
{
    SDL_RenderClear(Renderer);
    Player = PLR_Create();
    Player->x = 800;
    Player->y = 0;
    Player->speed_x = 20;
    Player->speed_y = 0;
    Player->in_ground = 1;
    Player->delta_rotation = 0;
    Player->whether_fall_down = 0;
    Player->equipment[0]=0;
    Player->equipment[1]=0;
    Player->equipment[2]=0;
    Player->equipment[3]=0;
    lowest_speed = 10;
    PlayerSurface = IMG_Load("src/resource/player/0000.png");
    DecorationPtr = 0;
    JumpBeginTick = SDL_GetTicks();
    InitTrace();
    TraceSurface = IMG_Load("src/resource/trace0.png");
    HaveObstacle = 0;
    HaveEquipment = 0;
    ObstacleSurface = IMG_Load("src/resource/obstacle.png");
    DecorationSurface1 = IMG_Load("src/resource/DEC_TREE_0.png");
    DecorationSurface2 = IMG_Load("src/resource/DEC_TREE_1.png");
    InitSnowCrash();
    SnowCrashSurface = IMG_Load("src/resource/snowcrash.png");
    SnowCrashBeginTick = SDL_GetTicks();
    HaveNotDefeat = 1;
    WarningSurface = IMG_Load("src/resource/warning.png");
    Warning_x = 10;
    Warning_y = 10;
    score = 0;
    ScoreFont = TTF_OpenFont("src/resource/HFShinySunday-2.ttf",75);
    ScoreSurface = TTF_RenderUTF8_Blended(ScoreFont,"000000000",ScoreFontColor);
    PauseSurface = IMG_Load("src/resource/pause.png");
    BeginPlayerSurface = IMG_Load("src/resource/begin.png");
    ScoreboardSurface = IMG_Load("src/resource/scoreboard.png");
    WaitBeginSurface = TTF_RenderUTF8_Blended(ScoreFont,"Press SPACE to start",WaitBeginColor);
}

void Draw(SDL_Surface* Surface,int x,int y,int w,int h,int angle)
{
    SDL_Texture *temp_texture = SDL_CreateTextureFromSurface(Renderer,Surface);
    SDL_Rect temp_rect = {x,y,w,h};
    SDL_RenderCopyEx(Renderer,temp_texture,NULL,&temp_rect,angle,NULL,SDL_FLIP_NONE);
    SDL_DestroyTexture(temp_texture);
}

void DrawByTransparency(SDL_Surface* Surface,int x,int y,int w,int h,int angle,int transparency)
{
    SDL_Texture *temp_texture = SDL_CreateTextureFromSurface(Renderer,Surface);
    SDL_SetTextureBlendMode(temp_texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(temp_texture, transparency);
    SDL_Rect temp_rect = {x,y,w,h};
    SDL_RenderCopyEx(Renderer,temp_texture,NULL,&temp_rect,angle,NULL,SDL_FLIP_NONE);
    SDL_DestroyTexture(temp_texture);
}

void Quit()
{
    SDL_DestroyRenderer(Renderer);//摧毁渲染器
    SDL_DestroyWindow(Window);//摧毁窗口
    PLR_Destroy(Player);
    SDL_FreeSurface(PlayerSurface);
    SDL_FreeSurface(DecorationSurface1);
    SDL_FreeSurface(DecorationSurface2);
    SDL_FreeSurface(TraceSurface);
    SDL_FreeSurface(ObstacleSurface);
    SDL_FreeSurface(EquipmentSurface);
    SNC_DestroySnowCrash(SnowCrash);
    SDL_FreeSurface(SnowCrashSurface);
    SDL_FreeSurface(WarningSurface);
    SDL_FreeSurface(ScoreSurface);
    SDL_FreeSurface(PauseSurface);
    SDL_FreeSurface(BeginPlayerSurface);
    SDL_FreeSurface(WaitBeginSurface);
    SDL_FreeSurface(ScoreboardSurface);
    SDL_Quit;//退出
    exit(0);
}

double Distance(int x1,int y1,int x2,int y2)
{
    return sqrt(pow(x2-x1,2)+pow(y2-y1,2));
}

void InitNoise()
{
    for(int i=0;i<10400;i++)
    {
        Noise[i]=0;
    }
}

void CopyNoise()
{
    for(int i=0;i<800;i++)
    {
        Noise[i]=Noise[i+9600]+3000;
    }
}

void CalculateNoise()
{
    int TempNoise[9601];
    TempNoise[0]=0;
    TempNoise[9600]=0;
    for(int i=800;i<10400;i++)
    {
        Noise[i]=0;
    }

    for(int i=1;i<3;i++)//三分
    {
        TempNoise[3200*i]=rand()%(2*height1+1)-height1;
    }
    for(int i=0;i<9600;i++)
    {
        int l = (i/3200)*3200;
        double percent = (double)(i-l)/3200;
        TempNoise[i] = (int)(TempNoise[l+3200]*SmoothFunction(percent)+TempNoise[l]* SmoothFunction(1-percent));
        Noise[i+800]+=TempNoise[i];
    }

    for(int i=1;i<4;i++)//四分
    {
        TempNoise[2400*i]=rand()%(2*height2+1)-height2;
    }
    for(int i=0;i<9600;i++)
    {
        int l = (i/2400)*2400;
        double percent = (double)(i-l)/2400;
        TempNoise[i] = (int)(TempNoise[l+2400]*SmoothFunction(percent)+TempNoise[l]* SmoothFunction(1-percent));
        Noise[i+800]+=TempNoise[i];
    }

    for(int i=1;i<6;i++)//六分
    {
        TempNoise[1600*i]=rand()%(2*height3+1)-height3;
    }
    for(int i=0;i<9600;i++)
    {
        int l = (i/1600)*1600;
        double percent = (double)(i-l)/1600;
        TempNoise[i] = (int)(TempNoise[l+1600]*SmoothFunction(percent)+TempNoise[l]* SmoothFunction(1-percent));
        Noise[i+800]+=TempNoise[i];
    }

    for(int i=1;i<12;i++)//十二分
    {
        TempNoise[800*i]=rand()%(2*height4+1)-height4;
    }
    for(int i=0;i<9600;i++)
    {
        int l = (i/800)*800;
        double percent = (double)(i-l)/800;
        TempNoise[i] = (int)(TempNoise[l+800]* SmoothFunction(percent)+TempNoise[l]* SmoothFunction(1-percent));
        Noise[i+800]+=TempNoise[i];
    }

    for(int i=0;i<9600;i++)
    {
        Noise[i+800]-=(int)(((double)i/9600)*250*12);
    }//为生成的地形添加一个下降的趋势，使得爬坡的情况出现的少一些
}

void Move()
{
    MoveNoise();
    MoveTrace();
    MoveDecoration();
    MoveObstacle();
    MoveEquipment();
    MoveSnowCrash();
}

void MoveNoise()
{
    CopyNoise();
    CalculateNoise();
    Player->x -= 9600;
    Player->y += 3000;
}

double SmoothFunction(double per)
{
    return (0.5-0.5*cos(pi*per));
}

void RendMount()
{
    SDL_Texture *tempMountTexture = SDL_CreateTexture(Renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,800,500);
    void* pix;
    int pitch;
    SDL_LockTexture(tempMountTexture,NULL,&pix,&pitch);
    for(int i=0;i<bgh;i++)
    {
        for(int j=0;j<bgw;j++)
        {
            if(i>=250+Player->y-Noise[j+Player->x-400])//成立为山体
            {
                ((uint32_t*)pix)[i*bgw+j] = 0xffffffff;
            } else //if(i>250)
            {
                ((uint32_t*)pix)[i*bgw+j] = 0xff00bfff;
            }
        }
    }
    SDL_UnlockTexture(tempMountTexture);
    SDL_RenderCopy(Renderer,tempMountTexture,NULL,NULL);
    SDL_DestroyTexture(tempMountTexture);
}

void PlayerGo()
{
    if(Player->in_ground)
    {//地上
        double Slope = GetSlope();
        CalculateAcceleration();
        Player->speed_x += Player->acceleration_x;
        Player->speed_y += Player->acceleration_y;
        NotEqualToZero();
        if(Player->speed_y/Player->speed_x<Slope)
        {//需要分解速度,inGround保持1
            double ang_v = atan(Player->speed_y/Player->speed_x);
            double ang_slope = atan(Slope);
            double ang_0 = ang_slope-ang_v;
            double v = cos(ang_0)*sqrt(pow(Player->speed_x,2)+pow(Player->speed_y,2));
            Player->speed_x = v * cos(ang_slope);
            Player->speed_y = v * sin(ang_slope);
            //NotEqualToZero();
            KeepTheLowestSpeed();
            Player->x+=(int)Player->speed_x;
            Player->y = Noise[Player->x];
        }else
        {//无需分解速度,inGround变为0
            Player->in_ground = 0;
            Player->x+=(int)Player->speed_x;
            Player->y+=(int)Player->speed_y;
        }
    }else
    {//空中
        CalculateAcceleration();
        Player->speed_x += Player->acceleration_x;
        Player->speed_y += Player->acceleration_y;
        NotEqualToZero();
        if(Noise[Player->x+(int)Player->speed_x] >= Player->y+(int)Player->speed_y)
        {//撞上了,inGround变为1
            DetectUpsideDown();
            Player->delta_rotation=0;
            Player->in_ground = 1;
            Player->x += (int)Player->speed_x;
            Player->y = Noise[Player->x];
            double ang_v;
            if(Player->speed_x>0)
            {
                ang_v = atan(Player->speed_y / Player->speed_x);
            }else
            {
                ang_v = atan(Player->speed_y / Player->speed_x)+pi;
            }
            double ang_slope = atan(GetSlope());
            double ang_0 = ang_slope-ang_v;
            double v = cos(ang_0)*sqrt(pow(Player->speed_x,2)+pow(Player->speed_y,2));
            if(Player->whether_fall_down==0) {
                if (Player->speed_x > 0) {
                    Player->speed_x = v * cos(ang_slope);
                    Player->speed_y = v * sin(ang_slope);
                } else {
                    Player->speed_x = lowest_speed * cos(ang_slope);
                    Player->speed_y = lowest_speed * sin(ang_slope);
                }
            }
            KeepTheLowestSpeed();
        }else
        {//没撞上,inGround保持0
            Player->x+=(int)Player->speed_x;
            Player->y+=(int)Player->speed_y;
        }
    }
}

void RendPlayer()
{
    if(Player->whether_fall_down) {
        if (Player->y-Noise[Player->x]<=delta_distance) {
            int ang = (int) (atan(GetSlope()) * 180 / pi);
            Draw(PlayerSurface, 355, 235, 90, 30, -ang);
        } else {
            int ang = (int) ((atan((double) (Player->speed_y) / (double) (Player->speed_x))) * 180 / pi);
            Draw(PlayerSurface, 355, 235, 90, 30, -ang);
        }
    }else {
        int ang;
        if (Player->speed_x == 0) {
            if (Player->speed_y > 0) {
                ang = 90;
            } else if (Player->speed_y == 0) {
                ang = 0;
            } else {
                ang = -90;
            }
        } else if (Player->speed_x < 0) {
            ang = 180 + (int) ((atan((double) (Player->speed_y) / (double) (Player->speed_x))) * 180 / pi);
        } else {
            ang = (int) ((atan((double) (Player->speed_y) / (double) (Player->speed_x))) * 180 / pi);
        }
        Draw(PlayerSurface, 355, 235, 90, 30, -(ang + Player->delta_rotation));
    }
}

double GetSlope()
{
    return ((double)(Noise[Player->x]-Noise[Player->x-15])/15);
}

double GetSlopeByX(int x)
{
    return ((double)(Noise[x]-Noise[x-15])/15);
}

void CalculateAcceleration()
{
    Player->acceleration_x = 0;
    Player->acceleration_y = 0;
    Player->acceleration_y -= gravity;
    if(Player->in_ground)
    {
        double ang_slp = atan(GetSlope());
        Player->acceleration_x-=resistance_snow*cos(ang_slp);
        Player->acceleration_y-=resistance_snow*sin(ang_slp);
        if(Player->whether_fall_down)
        {
            if(Player->speed_x>0)
            {
                Player->acceleration_x-=fall_down_resistance_snow*cos(ang_slp);
                Player->acceleration_y-=fall_down_resistance_snow*sin(ang_slp);
            }else
            {
                Player->acceleration_x+=fall_down_resistance_snow*cos(ang_slp);
                Player->acceleration_y+=fall_down_resistance_snow*sin(ang_slp);
            }
        }
    }

}

void JumpOrRotate()
{
    if(KeyValue[SDL_SCANCODE_SPACE] &&
    Player->y-Noise[Player->x]<=delta_distance &&
    Player->whether_fall_down == 0)
    {
        uint32_t JumpEndTick = SDL_GetTicks();
        if(JumpEndTick-JumpBeginTick>=jumpCD) {
            Player->speed_y += jump_height;
            JumpBeginTick = SDL_GetTicks();
            if (Player->in_ground == 0) {
                DetectUpsideDown();
                Player->delta_rotation = 0;
            }
            return;
        }
    }
    if(KeyValue[SDL_SCANCODE_SPACE] &&
    Player->y-Noise[Player->x]>delta_distance &&
    Player->whether_fall_down == 0)
    {
        Player->delta_rotation+=rotate_speed;
    }
    if(MainEvent.type == SDL_QUIT)
    {
        Quit();
    }
}

void KeepTheLowestSpeed()
{
    double v = sqrt(pow(Player->speed_x,2)+pow(Player->speed_y,2));
    if(Player->speed_x<=0&&Player->whether_fall_down==0)
    {
        Player->speed_x = lowest_speed*cos(atan(GetSlope()));
        Player->speed_y = lowest_speed*sin(atan(GetSlope()));
    }
    if(v<lowest_speed&&Player->whether_fall_down==0)
    {
        Player->speed_x *= lowest_speed/v;
        Player->speed_y *= lowest_speed/v;
    }
}

void InitDecoration() {
    DecorationArray[DecorationPtr] = DEC_CreateDecoration();
    DecorationArray[DecorationPtr]->x = Player->x + 440;

    DecorationArray[DecorationPtr]->y = Noise[Player->x + 440];

    DecorationArray[DecorationPtr]->Type = rand() % 2;
    DecorationPtr++;
}

void RendDecoration()
{
    for(int i=0;i<DecorationPtr;i++)
    {
        switch (DecorationArray[i]->Type) {
            case DEC_TREE_1:
                Draw(DecorationSurface1, 360 + DecorationArray[i]->x - Player->x,
                     170 + Player->y - DecorationArray[i]->y, 80, 80, 0);
                break;
            case DEC_TREE_2:
                Draw(DecorationSurface2, 360 + DecorationArray[i]->x - Player->x,
                     170 + Player->y - DecorationArray[i]->y, 80, 80, 0);
                break;
            default:
                break;
        }

    }
}

void MoveDecoration()
{
    for(int i=0;i<DecorationPtr;i++)
    {
        DecorationArray[i]->x -= 9600;
        DecorationArray[i]->y += 3000;
    }
}

void DecorationMain() {
    while (DecorationPtr) {
        if (DecorationArray[0]->x <= Player->x - 440) {
            DEC_DestroyDecoration(DecorationArray[0]);
            for (int i = 0; i < DecorationPtr - 1; i++) {
                DecorationArray[i] = DecorationArray[i + 1];
            }
            DecorationArray[DecorationPtr] = NULL;
            DecorationPtr--;
        } else {
            break;
        }
    }
    RendDecoration();
    if (rand() % average_decoration_interval == 0 && Player->whether_fall_down == 0 && Player->x + 440 < 10400) {
        if (DecorationPtr == 0) {
            InitDecoration();
        } else if (Player->x + 430 - DecorationArray[DecorationPtr - 1]->x > 40) {
            InitDecoration();
        }
    }
}

void InitTrace()
{
    for(int i=0;i<10;i++)
    {
        trace[i].x = 800;
        trace[i].y = 0;
        trace[i].facing = 0;
    }
}

void SaveTrace()
{
    for(int i=8;i>=0;i--)
    {
        trace[i+1].x=trace[i].x;
        trace[i+1].y=trace[i].y;
        trace[i+1].facing=trace[i].facing;
    }
    trace[0].x = Player->x;
    trace[0].y = Player->y;
    trace[0].facing = (int)(atan((double)Player->speed_y/(double)Player->speed_x)/pi*180);
}

void RendTrace()
{
    for(int i=9;i>=0;i--)
    {
        DrawByTransparency(TraceSurface,390+trace[i].x-Player->x,240+Player->y-trace[i].y,20,20,-trace[i].facing,255-(int)25.5*i);
    }
}

void MoveTrace()
{
    for(int i=0;i<10;i++)
    {
        trace[i].x-=9600;
        trace[i].y+=3000;
    }
}

void DetectUpsideDown()
{
    if(Player->whether_fall_down == 0) {
        double speed_ang = atan((double)Player->speed_y / (double)Player->speed_x);
        if(Player->speed_x<0)
        {
            speed_ang+=pi;
        }
        double rotate_ang = (double) Player->delta_rotation / 180 * pi;
        double slope_ang = atan(GetSlope());
        if (cos(speed_ang + rotate_ang - slope_ang) <= 0) {
            Player->whether_fall_down = 1;
            Player->equipment[0] = 0;
            Player->equipment[1] = 0;
            Player->equipment[2] = 0;
            Player->equipment[3] = 0;
            UploadWhenEquipment();
            PlayerSurface = IMG_Load("src/resource/falldown.png");
            FallDownBeginTick = SDL_GetTicks();
        }
    }
}

void DetectObstacle() {
    if (HaveObstacle) {
        int if_hit = 0;
        for (int i = 0; i < Player->speed_x; i++) {
            if(Distance(Player->x+i,Player->y+(int)(i*Player->speed_y/Player->speed_x),Obstacle->x,Obstacle->y+30)<=40)
            {
                if_hit = 1;
                break;
            }
        }

        if (if_hit) {
            int i = 0;
            while (Distance(Player->x + i, Player->y + (int) (i * Player->speed_y / Player->speed_x), Obstacle->x,
                            Obstacle->y + 30) > 40) {
                i++;
            }
            Player->x += (i-1);
            Player->y += (int) ((i-1) * Player->speed_y / Player->speed_x);
            double player_speed_facing = atan((double) Player->speed_y / (double) Player->speed_x);
            double player_facing = player_speed_facing + (double) Player->delta_rotation * pi / 180;
            double slope_stone;
            if (Player->x < Obstacle->x) {
                slope_stone = atan((double) (Player->y - (Obstacle->y + 30)) / (double) (Obstacle->x - Player->x));
            } else if (Player->x == Obstacle->x) {
                slope_stone = pi / 2;
            } else {
                slope_stone = pi + atan((double) (Player->y - (Obstacle->y + 30)) / (double) (Obstacle->x - Player->x));
            }
            if (sin(slope_stone + player_facing) > 0) {
                double v = sqrt(pow(Player->speed_x, 2) + pow(Player->speed_y, 2));
                double vt = v * cos(player_speed_facing + slope_stone-pi/2); //切向
                double vn = -v * sin(player_speed_facing + slope_stone-pi/2); //法向
                vn/=2;//法向速度亏损
                double tempvx = Player->speed_x;
                double tempvy = Player->speed_y;
                Player->speed_x = 0;
                Player->speed_y = 0;
                Player->delta_rotation = 0;
                Player->speed_x += vt * cos(player_speed_facing);
                Player->speed_y += vt * sin(player_speed_facing);
                Player->speed_x += vn * cos(player_speed_facing + pi/2);
                Player->speed_y += vn * sin(player_speed_facing + pi/2);
                if(Player->speed_x<5)
                {
                    Player->speed_x = tempvx;
                    Player->speed_y = tempvy;
                    goto falldown;
                }
            } else {
                falldown:
                OBS_DestroyObstacle(Obstacle);
                HaveObstacle = 0;
                int if_have_equipment = 0;
                for(int j=0;j<4;j++)
                {
                    if(Player->equipment[j]==1)
                    {
                        if_have_equipment = 1;
                        Player->equipment[j] = 0;
                        break;
                    }
                }
                if(if_have_equipment)
                {
                    Player->speed_x /= 2;
                    Player->speed_y /= 2;
                    UploadWhenEquipment();
                }else {
                    Player->speed_x /= 2;
                    Player->speed_y /= 2;
                    Player->whether_fall_down = 1;
                    UploadWhenEquipment();
                    PlayerSurface = IMG_Load("src/resource/falldown.png");
                    FallDownBeginTick = SDL_GetTicks();
                    return;
                }
            }
        }
    }
}

void FallDownMain()
{
    if(Player->whether_fall_down)
    {
        uint32_t FallDownEndTick = SDL_GetTicks();
        if(FallDownEndTick - FallDownBeginTick >= fall_down_delay_ms)
        {
            PlayerSurface = IMG_Load("src/resource/player/0000.png");
            Player->whether_fall_down = 0;
            if(-Player->y+Noise[Player->x]<=delta_distance)
            {
                double ang = atan(GetSlope());
                Player->speed_x = lowest_speed*cos(ang);
                Player->speed_y = lowest_speed*sin(ang);
            }
        }
    }
}

void InitObstacle() {
    Obstacle = OBS_CreateObstacle();
    Obstacle->x = Player->x + 440;
    Obstacle->y = Noise[Player->x + 440];
}

void RendObstacle()
{
    int ang;
    if(Obstacle->x<10400){
        ang = (int)(atan(GetSlopeByX(Obstacle->x))*180/pi);
    }else
    {
        ang = 0;
    }
    Draw(ObstacleSurface,370+Obstacle->x-Player->x,190+Player->y-Obstacle->y,60,60,-ang);
}

void MoveObstacle()
{
    if(HaveObstacle)
    {
        Obstacle->x -= 9600;
        Obstacle->y += 3000;
    }
}

void ObstacleMain()
{
    if(HaveObstacle)
    {
        if(Player->x-Obstacle->x>440)
        {
            OBS_DestroyObstacle(Obstacle);
            HaveObstacle = 0;
        }
        else
        {
            RendObstacle();
        }
    }else
    {
        if(rand()%average_obstacle_interval == 0 && Player->whether_fall_down == 0 && GetSlopeByX(Player->x+399)<highest_slope_when_obstacle && Player->x+440<10400)
        {
            InitObstacle();
            HaveObstacle = 1;
        }
    }
}

void InitEquipment()
{
    Equipment = EQU_CreateEquipment();
    Equipment->x = Player->x+440;
    Equipment->y = Noise[Player->x+440];
    Equipment->Type = rand()%4;
    switch (Equipment->Type) {
        case DROPLET:
            EquipmentSurface = IMG_Load("src/resource/droplet.png");
            break;
        case FLAME:
            EquipmentSurface = IMG_Load("src/resource/flame.png");
            break;
        case FIST1:
            EquipmentSurface = IMG_Load("src/resource/fist1.png");
            break;
        case FIST2:
            EquipmentSurface = IMG_Load("src/resource/fist2.png");
            break;
        default:
            break;
    }
    double ESlope = atan(GetSlopeByX(Equipment->x));
    Equipment->speed_x = equ_lowest_speed * cos(ESlope);
    Equipment->speed_y = equ_lowest_speed * sin(ESlope);
}

void RendEquipment()
{
    int ang = (int)(atan(Equipment->speed_y/Equipment->speed_x)*180/pi);
    Draw(EquipmentSurface,385+Equipment->x-Player->x,235+Player->y-Equipment->y,30,30,-ang);
}

void MoveEquipment()
{
    Equipment->x-=9600;
    Equipment->y+=3000;
}

void EquipmentMain()
{
    if(HaveEquipment)
    {
        if(abs(Player->x-Equipment->x)>500 || abs(Player->y-Equipment->y)>1500 || Equipment->x<0 || Equipment->x>=10400)
        {
            EQU_DestroyEquipment(Equipment);
            HaveEquipment = 0;
        }
        else if(Equipment->x<10400)
        {
            EQU_EquipmentGo(Equipment,Noise);
            RendEquipment();
        }
    }else
    {
        if(rand()%average_equipment_interval == 0 && Player->whether_fall_down == 0 && Player->x+440<10400)
        {
            InitEquipment();
            HaveEquipment = 1;
        }
    }
    if(HaveEquipment)
    {
        int if_capture_equipment = 0;
        for(int i=0;i<Player->speed_x;i++)
        {
            if(Distance(Player->x+i,Player->y+(int)(i*(double)Player->y/(double)Player->x),Equipment->x,Equipment->y)<30)
            {
                if_capture_equipment = 1;
                break;
            }
        }
        if(if_capture_equipment)
        {
            if(Player->equipment[Equipment->Type]==0 && Player->whether_fall_down==0)
            {
                Player->equipment[Equipment->Type]=1;
                EQU_DestroyEquipment(Equipment);
                HaveEquipment = 0;
                UploadWhenEquipment();
            }
        }
    }
}

void UploadWhenEquipment()
{
    UploadPlayerSurfaceWhenEquipment();
    UploadLowestSpeed();
    UploadTrace();
}

void UploadPlayerSurfaceWhenEquipment()
{
    char file[29] = "src/resource/player/0000.png\0";
    file[23] = (char)(48+Player->equipment[0]);
    file[22] = (char)(48+Player->equipment[1]);
    file[21] = (char)(48+Player->equipment[2]);
    file[20] = (char)(48+Player->equipment[3]);
    PlayerSurface = IMG_Load(file);
}

void UploadLowestSpeed()
{
    lowest_speed = Player->equipment[0]*add_speed_of_per_equipment +
            Player->equipment[1]*add_speed_of_per_equipment +
            Player->equipment[2]*add_speed_of_per_equipment +
            Player->equipment[3]*add_speed_of_per_equipment +
            10;
}

void UploadTrace()
{
    if(Player->equipment[1]==1&&Player->equipment[2]==1&&Player->equipment[3]==1)
    {
        TraceSurface = IMG_Load("src/resource/trace3.png");
    }else if(Player->equipment[1]==1)
    {
        TraceSurface = IMG_Load("src/resource/trace1.png");
    }else if(Player->equipment[0]==1)
    {
        TraceSurface = IMG_Load("src/resource/trace2.png");
    }else
    {
        TraceSurface = IMG_Load("src/resource/trace0.png");
    }
}

void NotEqualToZero()
{
    if(Player->speed_x==0)
    {
        Player->speed_x=0.1;
    }
}

void InitSnowCrash()
{
    SnowCrash = SNC_CreateSnowCrash();
    SnowCrash->x = 800;
    SnowCrash->speed = snow_crash_initial_speed;
}

void RendSnowCrash() {
    if (Player->x - SnowCrash->x < 450) {
        if (Player->x - Player->x + SnowCrash->x >= 0) {
            Draw(SnowCrashSurface, 400 - Player->x + SnowCrash->x - 50,
                 150 - Noise[Player->x - 400 + 400 - Player->x + SnowCrash->x] + Player->y, 100, 100, 0);
        } else {
            Draw(SnowCrashSurface, 400 - Player->x + SnowCrash->x - 50, 150 - Noise[Player->x - 400] + Player->y, 100,
                 100, 0);
        }
    }
    SDL_Texture *tempSnowCrashTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 800, 500);
    void* pix;
    int pitch;
    SDL_SetTextureBlendMode(tempSnowCrashTexture, SDL_BLENDMODE_BLEND);
    SDL_LockTexture(tempSnowCrashTexture, NULL, &pix, &pitch);
    for(int i=0;i<bgh;i++)
    {
        for(int j=0;j<bgw;j++)
        {
            if(i<250+Player->y-Noise[Player->x-400+j]&&i>150+Player->y-Noise[Player->x-400+j]&&j<SnowCrash->x-Player->x+400)
            {
                ((uint32_t*)pix)[i*bgw+j] = 0xffe5e5e5;
            } else
            {
                ((uint32_t*)pix)[i*bgw+j] = 0x00000000;
            }
        }
    }
    SDL_UnlockTexture(tempSnowCrashTexture);
    SDL_RenderCopy(Renderer, tempSnowCrashTexture, NULL, NULL);
    SDL_DestroyTexture(tempSnowCrashTexture);
}

void SnowCrashGo()
{
    SnowCrash->x+=(int)SnowCrash->speed;
}

void MoveSnowCrash()
{
    SnowCrash->x-=9600;
    SnowCrash->y+=3000;
}

void SnowCrashMain()
{
    SnowCrashGo();
    RendSnowCrash();
    SnowCrash->speed = (int)(SDL_GetTicks()-SnowCrashBeginTick)*snow_crash_acceleration/1000 + snow_crash_initial_speed;
    DetectDefeat();
}

void DetectDefeat()
{
    if(Player->x<SnowCrash->x-15&&Player->y<=Noise[Player->x]+85)
    {
        HaveNotDefeat = 0;
    }
}

void AfterDefeat()
{
    uint32_t begin = SDL_GetTicks();

    RendMount();//渲染山

    RendDecoration();
    if(HaveObstacle) RendObstacle();
    EquipmentMain();
    SnowCrashMain();
    SDL_RenderPresent(Renderer);

    uint32_t end = SDL_GetTicks();
    int cost = (int)end - (int)begin;
    if(ms_per_frame-cost>0) SDL_Delay(ms_per_frame-cost);//帧频控制
}

void WarningMove()
{
    if(Player->x-SnowCrash->x<warning_distance)
    {
        if(Warning_x<10)
        {
            Warning_x+=10;
        }
    }else
    {
        if(Warning_x>-90)
        {
            Warning_x-=10;
        }
    }
}

void RendWarning()
{
    int r;
    if(Player->x-SnowCrash->x<critical_warning_distance)
    {
        r = (critical_warning_distance+SnowCrash->x-Player->x)/50 + 1;
    }else
    {
        r = 1;
    }
    Draw(WarningSurface,Warning_x+rand()%r,Warning_y+rand()%r,70,70,0);
}

void RendScore()
{
    char scoretxt[10] = "000000000\0";
    score+=(int)Player->speed_x;
    int n=0;
    if(score<10)
    {
        n=1;
    }else
    {
        while(pow(10,n)<=score)
        {
            n++;
        }
    }
    itoa(score,&scoretxt[9-n],10);
    ScoreSurface = TTF_RenderUTF8_Blended(ScoreFont,scoretxt,ScoreFontColor);
    Draw(ScoreSurface,590,30,180,30,0);
    SDL_FreeSurface(ScoreSurface);
}

void Pause()
{
    if(KeyValue[SDL_SCANCODE_ESCAPE])
    {
        uint32_t delta_tick;
        uint32_t begin_tick = SDL_GetTicks();
        Draw(PauseSurface,370,220,60,60,0);
        SDL_RenderPresent(Renderer);
        while (SDL_WaitEvent(&MainEvent)) {
            switch (MainEvent.type) {
                case SDL_QUIT:
                    Quit();
                case SDL_KEYDOWN:
                    if(MainEvent.key.keysym.sym == SDLK_SPACE)
                    {
                        uint32_t end_tick = SDL_GetTicks();
                        delta_tick = end_tick - begin_tick;
                        FallDownBeginTick+=delta_tick;
                        JumpBeginTick+=delta_tick;
                        SnowCrashBeginTick+=delta_tick;
                        return;
                    }
                default:
                    break;
            }
        }
    }
}

void Begin()
{
    struct SNC_SnowCrash* BeginSNC = SNC_CreateSnowCrash();
    BeginSNC->x = 0;
    while(BeginSNC->x<800)
    {
        uint32_t begin = SDL_GetTicks();
        RendMount();
        Draw(BeginPlayerSurface,385,235,30,30,180);
        if (Player->x - BeginSNC->x < 450) {
            if (Player->x - Player->x + BeginSNC->x >= 0) {
                Draw(SnowCrashSurface, 400 - Player->x + BeginSNC->x - 50,
                     150 - Noise[Player->x - Player->x + BeginSNC->x] + Player->y, 100, 100, 0);
            } else {
                Draw(SnowCrashSurface, 400 - Player->x + BeginSNC->x - 50,
                     150 - Noise[Player->x - 400] + Player->y, 100,
                     100, 0);
            }
        }
        SDL_Texture *tempSnowCrashTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 800, 500);
        void* pix;
        int pitch;
        SDL_SetTextureBlendMode(tempSnowCrashTexture, SDL_BLENDMODE_BLEND);
        SDL_LockTexture(tempSnowCrashTexture, NULL, &pix, &pitch);
        for(int i=0;i<bgh;i++)
        {
            for(int j=0;j<bgw;j++)
            {
                if(i<250+Player->y-Noise[Player->x-400+j]&&i>150+Player->y-Noise[Player->x-400+j]&&j<BeginSNC->x-Player->x+400)
                {
                    ((uint32_t*)pix)[i*bgw+j] = 0xffe5e5e5;
                } else
                {
                    ((uint32_t*)pix)[i*bgw+j] = 0x00000000;
                }
            }
        }
        SDL_UnlockTexture(tempSnowCrashTexture);
        SDL_RenderCopy(Renderer, tempSnowCrashTexture, NULL, NULL);
        SDL_DestroyTexture(tempSnowCrashTexture);
        SDL_RenderPresent(Renderer);
        BeginSNC->x += 10;
        uint32_t end = SDL_GetTicks();
        int cost = (int) end - (int) begin;
        if (ms_per_frame - cost > 0) SDL_Delay(ms_per_frame - cost);//帧频控制
    }
    SNC_DestroySnowCrash(BeginSNC);
}

void WaitBegin()
{
    while (1) {
        uint32_t begin = SDL_GetTicks();
        SDL_WaitEventTimeout(&MainEvent,1);
        switch (MainEvent.type) {
            case SDL_QUIT:
                Quit();
            case SDL_KEYDOWN:
                switch (MainEvent.key.keysym.sym) {
                    case SDLK_SPACE:
                        return;
                    default:
                        break;
                }
        }
        RendMount();
        Draw(BeginPlayerSurface, 385, 235, 30, 30, 180);
        int trans = (int) (127 + 127 * sin(pi * SDL_GetTicks() / 500));
        DrawByTransparency(WaitBeginSurface, 200, 400, 400, 40, 0, trans);
        SDL_RenderPresent(Renderer);
        uint32_t end = SDL_GetTicks();
        int cost = (int) end - (int) begin;
        if (ms_per_frame - cost > 0) SDL_Delay(ms_per_frame - cost);//帧频控制
    }
}

void Scoreboard()
{
    Draw(ScoreboardSurface,200,125,400,180,0);
    char scoretxt[10] = "000000000\0";
    score+=(int)Player->speed_x;
    int n=0;
    if(score<10)
    {
        n=1;
    }else
    {
        while(pow(10,n)<=score)
        {
            n++;
        }
    }
    itoa(score,&scoretxt[9-n],10);
    ScoreSurface = TTF_RenderUTF8_Blended(ScoreFont,scoretxt,WaitBeginColor);
    Draw(ScoreSurface,310,197,180,30,0);
    SDL_RenderPresent(Renderer);
    SDL_FreeSurface(ScoreSurface);
    while (SDL_WaitEvent(&MainEvent)) //主要的检测操作的部分，除非在这里面主动跳出，否则一般不会跳出
    {
        switch (MainEvent.type) //主事件的类型
        {
            case SDL_QUIT:
                Quit();//退出
                break;
            case SDL_MOUSEBUTTONDOWN:
                if(MainEvent.button.x>323&&MainEvent.button.x<474
                && MainEvent.button.y>247&&MainEvent.button.y<280)
                {
                    return;
                }
                break;
            default:
                break;
        }
    }
}

void FreeAll() {
    if(HaveObstacle)
    {
        OBS_DestroyObstacle(Obstacle);
    }
    if(HaveEquipment)
    {
        EQU_DestroyEquipment(Equipment);
    }
    for(int i=0;i<DecorationPtr;i++)
    {
        DEC_DestroyDecoration(DecorationArray[i]);
    }
}

