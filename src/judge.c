/* 判定関数
 *  当たってる  : 1
 * 当たってない : 0
 */
#include "obj.h"

#define GAME_COL 24 // Game 画面の縦幅

/* Object 同士の当たり判定 */
int JudgeHit(int x0, int y0, int x1, int y1, Obj *obj)
{
    int x2, y2, x3, y3;
    x2 = obj->px;
    y2 = obj->py;
    x3 = obj->px + obj->sx;
    y3 = obj->py + obj->sy;

    if ((x0 <= x3 - 1) && (x2 <= x1 - 1)     // x 軸方向
        && (y0 <= y3 - 1) && (y2 <= y1 - 1)) // y 軸方向
        return 1;

    return 0;
}
