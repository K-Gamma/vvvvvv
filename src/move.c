#include <time.h>
#include "judge.h"
#include "obj.h"
#include "save.h"

#define GAME_COL 24   // Game 画面の縦幅
#define GAME_LINE 80  // Game 画面の横幅
#define GLAVITY 0.4   // 重力
#define BLOCK_NUM 64  // block の個数
#define START_STAGE 1 // 開始ステージ

/* Player の移動関数 */
/* X 軸方向 */
void MovePX(Obj *player, Obj *block, int *stage_num, int *life,
            double *accel, int *facenum, double *t0, double *diff, int *sfilexist)
{
    double x, y;
    int i;

    // 次移動予定座標
    x = player->px + player->vx;
    y = player->py;

    // 全block に対して当たり判定
    for (i = 0; i < BLOCK_NUM; i++)
    {
        if (JudgeHit(x, y, x + player->sx, y + player->sy, &block[i]))
        {
            switch ((int)block[i].type)
            {
            case 2:
                *life = 0;
                break;
            case 3:
                Save(*stage_num, *player, *accel, *facenum, *t0, *diff);
                *sfilexist = 1;
                DestroyObj(&block[i]);
                break;
            case 9:
            case 10:
                continue;
            }
            return;
        }
    }

    player->px = x;

    // 左端
    if ((int)player->px < 0)
    {
        if (*stage_num == START_STAGE)
        { // 初期ステージなら移動しない
            player->px = 0;
            return;
        }
        *stage_num -= 1; // 前ステージ
        player->px = GAME_LINE - (player->sx);
    }
    // 右端
    if ((int)(player->px + player->sx) > GAME_LINE)
    {
        *stage_num += 1; // 次ステージ
        player->px = 0;
    }
    return;
}

/* Y 軸方向(重力も含む) */
void MovePY(Obj *player, Obj *block, int *stage_num, int *life, double *accel, int *facenum, double *t0,
            double *diff, int *sfilexist, int *tblo_hp, int *tblo_flag, int *landing)
{
    double x, y;
    int i;

    // 次移動予定座標
    x = player->px;
    y = player->py + player->vy + *accel;

    // 上端
    if (y < 0)
        y = GAME_COL - 1;

    // 下端
    if (y >= GAME_COL)
        y = 0;

    // 全block に対して当たり判定
    for (i = 0; i < BLOCK_NUM; i++)
    {
        if (tblo_flag[i])
            tblo_hp[i]--;

        if (0 > tblo_hp[i])
            DestroyObj(&block[i]);

        if (JudgeHit(x, y, x + player->sx, y + player->sy, &block[i]))
        { // y軸方向の当たり判定
            switch ((int)block[i].type)
            {
            case 2:
                *life = 0;
                break;
            case 3:
                Save(*stage_num, *player, *accel, *facenum, *t0, *diff);
                *sfilexist = 1;
                DestroyObj(&block[i]);
                break;
            case 4:
                tblo_flag[i] = 1;
                break;
            case 6:
                *accel *= -1;
                *facenum = (*facenum + 1) % 2;
                break;
            case 9:
            case 10:
                continue;
            }
            if (6 == (int)block[i].type)
                return;

            *landing = 1;
            return;
        }
    }
    *landing = 0;
    player->py = y;
    return;
}

/* Block の移動関数 */
/* X 軸方向 */
void MoveBX(Obj *player, Obj *block, int *life)
{
    double x, y;
    int i, j;

    for (i = 0; i < BLOCK_NUM; i++)
    {
        if (0 == block[i].vx)
            continue; // 速さが設定されてなければ動かさない

        x = block[i].px;
        y = block[i].py;

        if (5 == block[i].type)
        {
            if (JudgeHit(x, y - 1, x + block[i].sx, y + block[i].sy + 1, player))
                player->vx += block[i].vx;

            continue;
        }

        if (JudgeHit(x, y - 1, x + block[i].sx, y + block[i].sy + 1, player))
        {
            if (2 == (int)block[i].type)
            {
                *life = 0;
                return;
            }
            block[i].vx *= -1;
        }

        x += block[i].vx;

        // 左端,右端
        if ((0 > (int)x) || (GAME_LINE < (int)x + block[i].sx))
            block[i].vx *= -1;

        for (j = 1; j < BLOCK_NUM; j++)
        {
            if (JudgeHit(x, y, x + block[i].sx, y + block[i].sy, &block[(i + j) % BLOCK_NUM]))
            { //他Blockに当たっているか
                block[i].vx *= -1;
            }
        }

        if (JudgeHit(block[i].px, block[i].py - 1, block[i].px + block[i].sx,
                     block[i].py + block[i].sy + 1, player))
        { // ブロックの上下にプレイヤーがいるかどうか
            player->vx += block[i].vx;
            player->px = (int)player->px + block[i].px - (int)block[i].px;
        }
        block[i].px += block[i].vx;
    }
    return;
}

/* Y 軸方向 */
void MoveBY(Obj *player, Obj *block, double *accel, int *Sflag, int *life)
{
    double x, y;
    int Gflag[BLOCK_NUM] = {};
    int i, j;

    // Y軸加速度
    for (i = 0; i < BLOCK_NUM; i++)
    {

        x = block[i].px;
        if (7 == block[i].type)
        { // タイプ 7 : センサーブロック
            if (-1 == Sflag[i])
                continue; //break;

            if (((int)block[i].px < (int)(player->px + player->sx)) && ((int)player->px < (int)(block[i].px + block[i].sx)))
                Sflag[i] = 1;

            y = block[i].py + GLAVITY;
            if ((0 > y) || (GAME_COL + 1 < y + block[i].sy)) // 画面上, 下端はみ出し禁止
                DestroyObj(&block[i]);

            if (JudgeHit(x, y, x + block[i].sx, y + block[i].sy, player))
            {
                *life = 0;
                return;
            }
            for (j = 1; j < BLOCK_NUM; j++)
            {
                if (JudgeHit(x, y, x + block[i].sx, y + block[i].sy, &block[(i + j) % BLOCK_NUM]))
                { //他Blockに当たっているか
                    Sflag[i] = -1;
                }
            }
            if (Sflag[i] == 1)
                block[i].py = y;
        }

        if (8 == block[i].type)
        { // タイプ 8 : 重力ブロック
            y = block[i].py + *accel * 0.8;
            if ((0 > y) || (GAME_COL + 1 < y + block[i].sy)) // 画面上, 下端はみ出し禁止
                DestroyObj(&block[i]);

            if (JudgeHit(x, y, x + block[i].sx, y + block[i].sy, player))
            {
                *life = 0;
                return;
            }
            for (j = 1; j < BLOCK_NUM; j++)
            {
                if (JudgeHit(x, y, x + block[i].sx, y + block[i].sy, &block[(i + j) % BLOCK_NUM]))
                { //他Blockに当たっているか
                    Gflag[i] = 1;
                }
            }
            if (!Gflag[i])
                block[i].py = y;
        }

        if (0 == block[i].vy)
            continue; // 速さが設定されてなければ動かさない

        y = block[i].py + block[i].vy;
        // 下端,上端
        if ((0 > y) || (GAME_COL < y + block[i].sy - 1))
            block[i].vy *= -1;

        for (j = 1; j < BLOCK_NUM; j++)
        {
            if (JudgeHit(x, y, x + block[i].sx, y + block[i].sy, &block[(i + j) % BLOCK_NUM]))
            { //他Blockに当たっているか
                block[i].vy *= -1;
            }
        }
        y = block[i].py + block[i].vy;

        if (JudgeHit(x, y, x + block[i].sx, y + block[i].sy, player))
        {
            player->vy += block[i].vy - *accel;
            player->py = (int)player->py + block[i].py - (int)block[i].py;
        }
        block[i].py += block[i].vy;
    }
    return;
}
