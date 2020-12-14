
#ifndef OBJ_H
#define OBJ_H
/* 物体の構造体 */
typedef struct
{
    double type;   // ブロックのタイプ
    double px, py; // x, y座標
    double vx, vy; // x, y方向速度ベクトル
    double sx, sy; // x, y方向サイズ
} Obj;
#endif

/* Obj 構造体初期化関数 */
void InitObj(Obj *obj, double type, double px, double py,
             double vx, double vy, double sx, double sy);

/* Obj 破壊関数 */
void DestroyObj(Obj *obj);
