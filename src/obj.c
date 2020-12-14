#include "obj.h"

/* Obj 構造体初期化関数 */
void InitObj(Obj *obj, double type, double px, double py,
             double vx, double vy, double sx, double sy)
{
    obj->type = type;
    obj->px = px;
    obj->py = py;
    obj->vx = vx;
    obj->vy = vy;
    obj->sx = sx;
    obj->sy = sy;
}

/* Obj 破壊関数 */
void DestroyObj(Obj *obj)
{
    obj->type = 0;
    obj->px = obj->py = 0;
    obj->vx = obj->vy = 0;
    obj->sx = obj->sy = 0;
}
