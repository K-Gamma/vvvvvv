#include "obj.h"

// セーブ
void Save(int stage_num, Obj player, double accel, int facenum, double t0, double diff);
// ロード
int Read(int *stage_num, Obj *player, double *accel, int *facenum, double *diff, int timeinit);
