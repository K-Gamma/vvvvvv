#include "obj.h"

/* ストーリー */
void DrawStory(int count);

/* Start画面の表示 */
void DrawStart();

/* Ending の表示 */
void DrawEnding(int count);

/* 他のみんな */
void DrarOthers();

/* 宇宙船 */
void DrawShip();

/* Congratulations!! & Thank you for Playing!! */
void DrawClear(double t, double t0, double diff);

/* GAME 画面との境界線 */
void DrawBorder();

/* Player の表示*/
void DrawPlayer(Obj player, int facenum, int stage_num);

/* Block の表示 */
void DrawBlock(Obj *block, int *tblo_hp);

/* メニュー*/
int DrawMenu();

/* ゲームメッセージ  */
void DrawGameMessage(int stage_num, double t0, double diff);

/* 警告メッセージ */
int DrawAnyMessage(char *message);
