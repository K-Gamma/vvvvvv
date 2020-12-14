#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "draw.h"
#include "obj.h"
#include "mytime.h"

#define PLAYER_SIZE 5 // プレーヤー文字配列の長さ
#define GAME_COL 24   // Game 画面の縦幅
#define GAME_LINE 80  //            横幅
#define BLOCK_TYPE 11 // ブロックのタイプの数
#define BLOCK_NUM 64  // blockの個数

/* ストーリー */
void DrawStory(int count)
{

    char *story[3] = {
        "You were traveling the universe.",
        "Anything problem occurred when you had landed at a star.",
        "You have lost your friends and lost in the forest...",
    };
    // 宇宙旅行をしていたあなたたち。
    // ある星に着陸するときにトラブルが発生！
    // 仲間とはぐれてしまい、不思議な洞窟に迷い込んでしまった...

    attrset(COLOR_PAIR(1) | A_BOLD);

    if (10 < count)
        mvaddstr(5, GAME_LINE / 2 - strlen(story[0]) / 2, story[0]);

    if (110 < count)
        mvaddstr(8, GAME_LINE / 2 - strlen(story[1]) / 2, story[1]);

    if (210 < count)
        mvaddstr(11, GAME_LINE / 2 - strlen(story[2]) / 2, story[2]);
}
/* 大文字の描画 */
void DrawLetter(char *letter_name, int x, int y)
{
    FILE *fp;
    char filename[32];
    char buf[GAME_LINE], *p;
    int i;

    sprintf(filename, "data/%s.txt", letter_name);

    if ((fp = fopen(filename, "r")) == NULL)
    {
        printw("File isn't exist.\n");
        return;
    }
    for (i = 0; i < GAME_COL; i++)
    {
        if (fgets(buf, GAME_LINE, fp) == NULL)
            break;

        p = strtok(buf, "\n");
        mvaddstr(y + i, x, p);
    }
    fclose(fp);
    return;
}

/* Start画面の表示 */
void DrawStart()
{
    FILE *fp;
    char buf[256], *p;
    char *letter;
    int x, y;
    char *message[] = {
        "   New Game   [ N ]",
        "Continue Game [ C ]",
        "     Quit     [ Q ]"};
    int mx, my;
    int i;

    attrset(COLOR_PAIR(1) | A_BOLD);
    if ((fp = fopen("data/start.txt", "r")) == NULL)
    {
        printw("File isn't exist.");
        goto MESSAGE;
    }
    while (1)
    {
        if (fgets(buf, 256, fp) == NULL)
            break;

        p = strtok(buf, " \t\n");

        //空回し
        if ((NULL == p) || ('#' == *p))
            continue;

        letter = p;
        p = strtok(NULL, " \t\n");
        x = atoi(p);
        p = strtok(NULL, " \t\n");
        y = atoi(p);

        DrawLetter(letter, x, y);
    }
    fclose(fp);

MESSAGE:
    for (i = 0; i < (sizeof(message) / sizeof(message[1])); i++)
    {
        mx = GAME_LINE / 2 - strlen(message[i]) / 2;
        my = 17 + i;
        mvaddstr(my, mx, message[i]);
    }
    return;
}

/* Ending の表示 */
void DrawEnding(int count)
{
    char *comment[4] = {
        "Hi everyone!! I wanted to meet you!!",
        "I finally able to meet you.",
        "Ah! Come back!!",
        "I finally go home!! Yay!!"};

    attrset(COLOR_PAIR(12) | A_BOLD);

    if (50 < count && 200 >= count)
        mvaddstr(17, 10, comment[0]);
    else if (200 < count && 350 >= count)
        mvaddstr(17, 10, comment[1]);
    else if (350 < count && 500 >= count)
        mvaddstr(17, 40, comment[2]);
    else if (500 < count && 650 >= count)
        mvaddstr(17, 40, comment[3]);
}

/* 他のみんな */
void DrarOthers()
{
    char face[5][6] = {"(-v-)", "(>v<)", "(^v^)", "(ovo)", "(@v@)"};
    int i;

    attrset(COLOR_PAIR(12) | A_BOLD);
    for (i = 0; i < 5; i++)
    {
        mvaddstr(19, +36 + 6 * i, face[i]); // 顔を描画
    }
}

/* 宇宙船 */
void DrawShip()
{
    char *ship[8] = {
        "     /\\     ",
        "    /  \\    ",
        "   /    \\   ",
        "  |      |  ",
        "  | OOO  |  ",
        "  /      \\  ",
        " /        \\ ",
        "/----------\\"};
    int i;

    attrset(COLOR_PAIR(12) | A_BOLD);
    for (i = 0; i < 8; i++)
    {
        mvaddstr(12 + i, 68, ship[i]);
    }
}

/* Congratulations!! & Thank you for Playing!! */
void DrawClear(double t, double t0, double diff)
{
    FILE *fp;
    char buf[256], *p;
    char *letter;
    int x, y;
    char *message[4] = {
        "Congratulations !!! & Thank you for playing !!!",
        "Please push key",
        " Title [ T ]",
        " Quit  [ Q ]"};

    attrset(COLOR_PAIR(12) | A_BOLD);

    if ((fp = fopen("data/clear.txt", "r")) == NULL)
    {
        printw("File isn't exist.");
        goto MESSAGE;
    }
    while (1)
    {
        if (fgets(buf, 256, fp) == NULL)
            break;

        p = strtok(buf, " \t\n");

        //空回し
        if ((NULL == p) || ('#' == *p))
            continue;

        letter = p;
        p = strtok(NULL, " \t\n");
        x = atoi(p);
        p = strtok(NULL, " \t\n");
        y = atoi(p);

        DrawLetter(letter, x, y);
    }
    fclose(fp);

MESSAGE:
    mvaddstr(14, GAME_LINE / 2 - strlen(message[0]) / 2, message[0]);
    mvprintw(16, GAME_LINE / 2 - strlen(message[0]) / 2, "Total Time : %4.2f", t - t0 + diff);
    attrset(COLOR_PAIR(13) | A_BOLD);
    mvaddstr(21, 37, message[1]);
    mvaddstr(21, 55, message[2]);
    mvaddstr(22, 55, message[3]);
    return;
}

/* GAME 画面との境界線 */
void DrawBorder()
{
    int h, w;

    attrset(COLOR_PAIR(2));
    getmaxyx(stdscr, h, w); // 画面サイズ取得
    // 横線
    if (GAME_COL < h)
    {
        move(GAME_COL, 0);
        hline(0, GAME_LINE);
    }
    // 縦線
    if (GAME_LINE < w)
    {
        move(0, GAME_LINE);
        vline(0, GAME_COL);
    }
}

/* Player の表示 */
void DrawPlayer(Obj player, int facenum, int stage_num)
{
    //プレーヤーの顔
    char face[4][PLAYER_SIZE + 1] = {"('^')", "(,v,)", "(>-<)", "('v')"};

    if (20 == stage_num)
    {
        attrset(COLOR_PAIR(12) | A_BOLD);
        facenum = 3;
    }
    else
    {
        attrset(COLOR_PAIR(2)); // 黒字, 白地
    }
    mvaddstr((int)(player.py), (int)(player.px), face[facenum]); // 顔を描画
}

/* Block の表示 */
void DrawBlock(Obj *block, int *tblo_hp)
{
    int i, j, l;
    int type;
    char parts[BLOCK_TYPE] = {'#', 'X', 'S', 'X', '>', '=', '!', 'G', ' ', ' ', ' '};

    for (i = 0; i < BLOCK_NUM; i++)
    {
        type = (int)block[i].type;
        attrset(COLOR_PAIR(type + 2));
        if ((2 <= type) && (6 >= type))
            attron(A_BOLD);

        move(block[i].py, block[i].px);
        for (j = 0; j < block[i].sy; j++)
        {
            move(block[i].py + j, block[i].px);
            for (l = 0; l < block[i].sx; l++)
            {
                if ((0 > block[i].py + j) || (GAME_COL < block[i].py + j))
                    continue;

                if ((0 > block[i].px + l) || (GAME_LINE < block[i].px + l))
                    continue;

                if ((5 == type) && (0 > block[i].vx))
                    addch('<');
                else
                    addch(parts[type - 1]); // ブロックを描画
            }
        }
    }
}

/* メニュー*/
int DrawMenu()
{
    int key;
    char *message[] = {
        "   Return The Game     [ M ]",
        "Back To The Save Point [ S ]",
        "    Back To Title      [ T ]",
        "    Quit The Game      [ Q ]"};
    int mx, my;
    int i, j;
    int Arlen = sizeof(message) / sizeof(message[1]);

    mx = GAME_LINE / 2 - strlen(message[1]) / 2;
    my = GAME_COL / 2 - 1;

    attron(A_REVERSE);
    for (i = 0; i < Arlen + 2; i++)
    {
        for (j = 0; j < strlen(message[1]) + 4; j++)
        {
            mvaddch(my - 2 + i, mx - 2 + j, ' ');
        }
    }

    for (i = 0; i < Arlen; i++)
    {
        mx = GAME_LINE / 2 - strlen(message[i]) / 2;
        my = GAME_COL / 2 - Arlen / 2 + i;
        mvaddstr(my, mx, message[i]);
    }

    while (1)
    {
        key = getch(); // キー入力
        switch (key)
        {
        case 'm':
        case 'M':
            return 0;
        case 's':
        case 'S':
            return 1;
        case 't':
        case 'T':
            return 2;
        case 'q':
        case 'Q':
            return 3;
            break;
        }
    }
}

/* ゲームメッセージ */
void DrawGameMessage(int stage_num, double t0, double diff)
{
    double t = Time();

    attrset(COLOR_PAIR(2));

    mvprintw(0, 0, "TIME : %4.2f", t - t0 + diff);
    //  mvprintw(1, 0, "Stage: %d", stage_num);
    return;
}

/* 警告メッセージ */
int DrawAnyMessage(char *message)
{
    int key;
    int mx, my;
    int i, j;

    mx = GAME_LINE / 2 - strlen(message) / 2;
    my = GAME_COL / 2;
    attrset(COLOR_PAIR(6));

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < strlen(message) + 4; j++)
        {
            mvaddch(my - 2 + i, mx - 2 + j, ' ');
        }
    }
    mvaddstr(my, mx, message);

    while (1)
    {
        key = getch(); // キー入力
        if ('n' == key)
            return 0;

        if ('y' == key)
            return 1;
    }
}
