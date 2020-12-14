#include "obj.h"

// X軸方向
void MovePX(Obj *player, Obj *block, int *stage_num, int *life, double *accel, int *facenum, double *t0, double *diff, int *sfilexist);

// Y軸方向
void MovePY(Obj *player, Obj *block, int *stage_num, int *life, double *accel, int *facenum, double *t0, double *diff, int *sfilexist, int *tblo_hp, int *tblo_flag, int *landing);

/* X 軸方向 */
void MoveBX(Obj *player, Obj *block, int *life);

/* Y 軸方向 */
void MoveBY(Obj *player, Obj *block, double *accel, int *Sflag, int *life);
