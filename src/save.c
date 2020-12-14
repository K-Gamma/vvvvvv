#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include "obj.h"
#include "draw.h"
#include "mytime.h"

#define START_STAGE 1 // 開始ステージ
#define START_X 7     // プレーヤー開始 X 座標
#define START_Y 20    //                Y 座標
#define GLAVITY 0.4   // 重力
#define BUF_SIZE 8

void encdec(unsigned char *buf)
{
    int i;
    for (i = 0; i < BUF_SIZE; i++)
        buf[i] = ~buf[i];
}

void EDcrypt()
{
    FILE *fp;
    unsigned char buf[BUF_SIZE];
    int size;

    if ((fp = fopen("data/.svf.dat", "rb")) == NULL)
        return;

    size = fread(buf, sizeof(unsigned char), BUF_SIZE, fp);
    encdec(buf);
    fwrite(buf, sizeof(unsigned char), size, fp);
    fclose(fp);
}

void Save(int stage_num, Obj player, double accel, int facenum,
          double t0, double diff)
{

    FILE *fp;
    double t = Time();
    Obj s;

    InitObj(&s, 1, stage_num, player.px, player.py, accel, facenum, t - t0 + diff);

    fp = fopen("data/.svf.dat", "wb");
    fwrite(&s, sizeof(Obj), 1, fp);
    fclose(fp);
    EDcrypt();
    return;
}

int Read(int *stage_num, Obj *player, double *accel, int *facenum,
         double *diff, int timeinit)
{

    FILE *fp;
    Obj s;

    EDcrypt();
    if ((fp = fopen("data/.svf.dat", "rb")) == NULL)
    {
        *stage_num = START_STAGE;
        player->px = START_X;
        player->py = START_Y;
        *accel = GLAVITY;
        *facenum = 0;
        return 0;
    };
    if (fread(&s, sizeof(Obj), 1, fp) < 1)
        return 0;

    fclose(fp);
    *stage_num = s.px;
    player->px = s.py;
    player->py = s.vx;
    *accel = s.vy;
    *facenum = s.sx;
    if (!timeinit)
        return 1;

    *diff = s.sy;
    return 1;
}
