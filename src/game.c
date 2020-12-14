#include <math.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "draw.h"
#include "judge.h"
#include "move.h"
#include "obj.h"
#include "save.h"
#include "mytime.h"

#define PLAYER_SIZE 5 // プレーヤー文字配列の長さ
#define GAME_COL 24   // Game 画面の縦幅
#define GAME_LINE 80  //            横幅
#define GLAVITY 0.4   // 重力
#define BLOCK_NUM 64  // block の個数
#define START_STAGE 1 // 開始ステージ
#define BUF_LEN 256   // ファイル読み込みに使用
#define VAL_NUM 7     // ファイルから読み込む引数の数
#define DELAY 0.02    // 動作速度調整のための遅延時間[秒]
#define TBLO_HP 15    // タイムブロックのHP

/* Player の操作 */
int Control(Obj *player, int *landing, double *air_t0, double *accel, double *kvector, double *vec_t0, int *facenum)
{
    int key = 0;
    double vec_t, air_t;
    key = getch(); // キー入力

    if (!*landing)
    {
        air_t = Time();
        if (air_t - *air_t0 < 0.8)
            player->vx = ((player->vx > 0) - (player->vx < 0)) * 0.4;
        else
            player->vx = 0;
    }
    else
    {
        player->vx = 0;
    }
    player->vy = 0.0;

    switch (key)
    {
    case KEY_LEFT:
        player->vx = *kvector = -1.0;
        *vec_t0 = *air_t0 = Time();
        break;
    case KEY_RIGHT:
        player->vx = *kvector = 1.0;
        *vec_t0 = *air_t0 = Time();
        break;
    case ' ':
        if (!*landing)
            break;

        vec_t = Time();
        if (vec_t - *vec_t0 < 0.08)
            player->vx = *kvector;

        *accel *= -1.0;                // 重力反転
        *facenum = (*facenum + 1) % 2; // 顔変更
        break;
    case 'm':
    case 'M':
        switch (DrawMenu())
        {
        case 1:
            return 's';
        case 2:
            return 't';
        case 3:
            return 'q';
        }
        break;
    default:
        break;
    }
    return 0;
}

/* ステージ読み込み関数 */
int StageRead(int stage_num, Obj *block, int sfilexist, int *tblo_hp, int *tblo_flag)
{
    FILE *stage;           // ステージファイルポインタ
    char stagename[32];    // ファイル名
    char buf[BUF_LEN], *p; // バッファ, strtok用変数
    double value[VAL_NUM]; // 構造体の引数
    int i = 0, j;

    //ステージ読み込み
    sprintf(stagename, "data/stage/stage_%d.script", stage_num);
    if ((stage = fopen(stagename, "r")) == NULL)
        return 2; //開けなければ終了

    // ファイルを全行読む無限ループ
    while (1)
    {
        // 数値をバッファへ入力
        if (fgets(buf, BUF_LEN, stage) == NULL)
            break;

        // 行を分解, 数値を取得
        p = strtok(buf, " \t\n");
        //空回し
        if ((NULL == p) || ('#' == *p))
            continue;

        value[0] = atof(p);

        if ((3 == (int)value[0]) && sfilexist) // セーブブロックは, セーブデータがある時
            continue;                          // 非表示

        if (4 == (int)value[0])
        { // タイムブロックがある時
            for (j = 0; j < BLOCK_NUM; j++)
            { // 初期化
                tblo_hp[j] = TBLO_HP;
                tblo_flag[j] = 0;
            }
        }
        else
        {
            for (j = 0; j < BLOCK_NUM; j++)
            { // 初期化
                tblo_hp[j] = TBLO_HP;
                tblo_flag[j] = 0;
            }
        }

        for (j = 1; j < VAL_NUM; j++)
        {
            p = strtok(NULL, " \t\n");
            // 引数が足りなければ終了
            if (p == NULL)
                return 3;

            value[j] = atof(p);
        }

        // ブロック初期化
        InitObj(&block[i], value[0], value[1], value[2],
                value[3], value[4], value[5], value[6]);
        i++;
    }
    fclose(stage); //ファイルを閉じる

    // 定義外の使われてないblockの初期化
    while (BLOCK_NUM != i)
    {
        InitObj(&block[i], 0, 0, 0, 0, 0, 0, 0);
        i++;
    }
    return 0;
}

/* デバッグ */
void DrawDebug(Obj player, Obj *block, int landing)
{
    char debug[256];
    int i = 5, j;
    int X = 82;

    attrset(COLOR_PAIR(2));

    mvaddstr(i, X, debug);

    sprintf(debug, "P  ) %3d : %3d", (int)player.px, (int)player.py);
    mvaddstr(i, X, debug);
    i++;

    sprintf(debug, "   ) %3d : %3d", (int)(player.px + player.sx - 1), (int)(player.py + player.sy - 1));
    mvaddstr(i, X, debug);
    i++;

    for (j = 0; j < BLOCK_NUM; j++)
    {
        if ((0 == block[j].sx) || (0 == block[j].sy))
            continue;

        sprintf(debug, "B%2d) %3d : %3d : %3d ", j, (int)block[j].px, (int)block[j].py, (int)block[j].type);
        mvaddstr(i, X, debug);
        i++;
    }
    sprintf(debug, "landing %d", landing);
    mvaddstr(i, X, debug);
    i++;
}

int Start()
{
    FILE *fp;
    char *filename = "data/.svf.dat";
    int w, h;
    int key;
    char *message = "Do you want to delete the save file? (y / n)";

REDRAW:
    getmaxyx(stdscr, h, w);
    // game 画面サイズ未満なら終了
    if ((GAME_COL > h) || (GAME_LINE > w))
        return 1;

    timeout(0); //キー入力なしで更新
    erase();
    DrawStart();
    DrawBorder();

    move(0, 0);
    refresh();
    while (1)
    {
        key = getch(); // キー入力
        switch (key)
        {
        case 'n':
        case 'N':
            if ((fp = fopen(filename, "rb")) == NULL)
                ;
            else if (DrawAnyMessage(message))
            {
                fclose(fp);
                remove(filename);
            }
            else
            {
                fclose(fp);
                goto REDRAW;
            }
        case 'c':
        case 'C':
            return 0;
        case 'q':
        case 'Q':
            return -1;
        }
    }
    return 0;
}

int Game()
{
    Obj player;                  // プレイヤー構造体
    Obj block[BLOCK_NUM] = {};   // ブロック構造体
    int stage_num = START_STAGE; // ステージ番号
    int nowstage;                // 現在のステージ
    int error;                   // error 処理用
    int sfilexist = 0;           //セーブファイルが存在するかどうか
    char *comment = "where is this ... ?";
    int facenum = 0;        //プレイヤー配列の要素番号
    double accel = GLAVITY; // 画面全体の加速度
    double kvector = 0;     // ベクトルの保持
    double vec_t0;          // ベクトル保持時間
    int landing = 0;        // 地面着地のflag
    double air_t0 = 0;      // 空中での横移動時間
    int life = 1;           // プレイヤーの生存フラグ
    double time;
    double t0;
    double diff = 0;
    int tblo_hp[BLOCK_NUM];   // 時限ブロックのHP配列
    int tblo_flag[BLOCK_NUM]; // 時限ブロックのフラグ
    int Sflag[BLOCK_NUM];     // センサーブロックのフラグ
    int i;
    int count = 0; // エンディングのカウント
    int key = 0;

    // プレイヤー初期化 (座標はRead()関数で与えてくれる)
    InitObj(&player, 0, 0, 0, 0.0, 0.0, PLAYER_SIZE, 1.0);

    // (あるなら)savefile読み込み
    if (Read(&stage_num, &player, &accel, &facenum, &diff, 1))
        sfilexist = 1;

    error = StageRead(stage_num, block, sfilexist, tblo_hp, tblo_flag);
    if (error)
        return error; // ロードできなければエラー

    while (!sfilexist)
    {
        timeout(0); //キー入力なしで更新
        erase();

        // ストーリ表示
        // 宇宙旅行をしていたあなたたち。
        // ある星に着陸するときにトラブルが発生！
        // 仲間とはぐれてしまい、不思議な洞窟に迷い込んでしまった...
        DrawStory(count);

        // 境界線
        DrawBorder();
        refresh();

        if (400 < count)
            break;

        //動作速度調整
        usleep((int)(DELAY * 1000000)); // 20,000μ秒＝20 m秒＝0.02秒 停止
        count++;
    }
    count = 0;
    error = StageRead(stage_num, block, sfilexist, tblo_hp, tblo_flag);
    if (error)
        return error; // ロードできなければエラー

    while (!sfilexist)
    {
        erase();

        //全ブロック描画
        DrawBlock(block, tblo_hp);

        // プレイヤー描画
        DrawPlayer(player, facenum, stage_num);

        if (30 < count)
            mvaddstr(18, 6, comment);

        if (100 < count)
            break;

        // 境界線
        DrawBorder();

        refresh();

        //動作速度調整
        usleep((int)(DELAY * 1000000)); // 20,000μ秒＝20 m秒＝0.02秒 停止
        count++;
    }

    // 時間計測開始
    t0 = vec_t0 = air_t0 = Time();
    nowstage = stage_num;

    while (1)
    {
        if (20 == nowstage)
            break;

        // ステージ移動判定
        if (nowstage != stage_num)
        {
            sfilexist = 0;
            error = StageRead(stage_num, block, sfilexist, tblo_hp, tblo_flag);
            if (error)
                return error; // ロードできなければエラー

            for (i = 0; i < BLOCK_NUM; i++)
            {
                Sflag[i] = 0;
            }
        }
        timeout(0); //キー入力なしで更新

        erase();

        //全ブロック描画
        DrawBlock(block, tblo_hp);

        //死亡判定
        if (!life)
        {
            life = 1;
            facenum = 2;
            DrawPlayer(player, facenum, stage_num);
            refresh();
            beep();
            usleep(500000);
            flushinp(); // キューにある入力削除
            if (Read(&stage_num, &player, &accel, &facenum, &diff, 0))
                sfilexist = 1;

            StageRead(stage_num, block, sfilexist, tblo_hp, tblo_flag);
            player.vx = 0.0;
            for (i = 0; i < BLOCK_NUM; i++)
            {
                Sflag[i] = 0;
            }
        }

        // 現在ステージ記憶
        nowstage = stage_num;

        // デバック
        // DrawDebug(player, block, landing);

        // メッセージ表示
        if (20 != stage_num)
            DrawGameMessage(stage_num, t0, diff);

        // プレイヤー描画
        DrawPlayer(player, facenum, stage_num);
        // 境界線
        DrawBorder();

        move(0, 0);
        refresh();

        // キー入力／キャラクタの移動
        switch (Control(&player, &landing, &air_t0, &accel, &kvector, &vec_t0, &facenum))
        {
        case 's':
            if (Read(&stage_num, &player, &accel, &facenum, &diff, 0))
                sfilexist = 1;

            StageRead(stage_num, block, sfilexist, tblo_hp, tblo_flag);
            player.vx = 0.0;
            for (i = 0; i < BLOCK_NUM; i++)
            {
                Sflag[i] = 0;
            }
            nowstage = stage_num;
            break;
        case 't':
            return -1;
        case 'q':
            return 0;
        default:
            break;
        }

        // Block Y 軸方向移動
        MoveBY(&player, block, &accel, Sflag, &life);
        // Player Y 軸方向移動(重力込み)
        MovePY(&player, block, &stage_num, &life, &accel, &facenum, &t0, &diff, &sfilexist, tblo_hp, tblo_flag, &landing);

        // Block X 軸方向移動
        MoveBX(&player, block, &life);
        // Player X 軸方向移動
        MovePX(&player, block, &stage_num, &life, &accel, &facenum, &t0, &diff, &sfilexist);

        //動作速度調整
        usleep((int)(DELAY * 1000000)); // 20,000μ秒＝20 m秒＝0.02秒 停止
    }
    time = Time();

    player.py = 19;
    player.vx = 0;
    count = 0;
    key = 0;
    // クリア画面
    while (1)
    {
        timeout(0); //キー入力なしで更新
        erase();

        //全ブロック描画
        DrawBlock(block, tblo_hp);

        // デバック
        //DrawDebug(player, block, landing);

        // Ending の表示
        DrawEnding(count);

        // 他のみんな
        DrarOthers();

        // 宇宙船
        DrawShip();

        if (650 < count)
        {
            DrawClear(time, t0, diff);
            switch (key = getch())
            {
            case 't':
                return -1;
            case 'q':
                return 0;
            default:
                break;
            }
        }

        // プレイヤー描画
        DrawPlayer(player, facenum, stage_num);

        // 境界線
        DrawBorder();
        refresh();

        if (!count)
        {
            usleep(1000000); // 20,000μ秒＝20 m秒＝0.02秒 停止
            count++;
        }

        if (10 >= player.px)
            player.px += 0.2;

        // Player X 軸方向移動
        MovePX(&player, block, &stage_num, &life, &accel, &facenum, &t0, &diff, &sfilexist);

        //動作速度調整
        usleep((int)(DELAY * 1000000)); // 20,000μ秒＝20 m秒＝0.02秒 停止
        if (10 < player.px)
            count++;
    }
    return 0;
}

int main()
{
    int error;

    /* curses の設定 */
    initscr();
    curs_set(0);          // カーソルを表示しない
    noecho();             // 入力されたキーを表示しない
    cbreak();             // 入力バッファを使わない(Enter 不要の入力)
    keypad(stdscr, TRUE); // カーソルキーを使用可能にする

    /*
     * COLOR_BLACK
     * COLOR_RED
     * COLOR_GREEN
     * COLOR_YELLOW
     * COLOR_BLUE
     * COLOR_MAGENTA
     * COLOR_CYAN
     * COLOR_WHITE
     * init_color(COLOR_RED, 700, 0, 0);
     * param 1     : color name
     * param 2, 3, 4 : rgb content min = 0, max = 1000
     */

    /* カラーの設定 */
    start_color();

    assume_default_colors(COLOR_BLACK, COLOR_BLACK); // 背景色
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);        // START画面の文字色
    init_pair(2, COLOR_WHITE, COLOR_BLACK);          // プレイヤーの色
    init_pair(3, COLOR_MAGENTA, COLOR_MAGENTA);      // type 1 のブロック
    init_pair(4, COLOR_RED, COLOR_BLACK);            // type 2
    init_pair(5, COLOR_GREEN, COLOR_BLACK);          // type 3
    init_pair(6, COLOR_BLACK, COLOR_YELLOW);         // type 4 & error
    init_pair(7, COLOR_CYAN, COLOR_BLUE);            // type 5
    init_pair(8, COLOR_MAGENTA, COLOR_BLACK);        // type 6
    init_pair(9, COLOR_BLACK, COLOR_MAGENTA);        // type 7
    init_pair(10, COLOR_CYAN, COLOR_MAGENTA);        // type 8
    /* ここからは特別カラー */
    init_pair(11, COLOR_WHITE, COLOR_WHITE); // type 9
    init_pair(12, COLOR_BLACK, COLOR_CYAN);  // type 10（空）
    init_pair(13, COLOR_BLACK, COLOR_GREEN); // type 11（地面）
    bkgd(COLOR_PAIR(0));

START:
    /* スタート画面*/
    if ((error = Start()))
        goto END;

    /* ゲーム本体 */
    if ((error = Game()) == -1)
        goto START;
    else if (error)
        goto END;

    /* 終了 */
    endwin();
    return EXIT_SUCCESS;
END:
    /* 終了 */
    endwin();
    // エラーメッセージ
    if (error == 1)
        fprintf(stderr, "error : 端末サイズを確認してください. 80× 24推奨です.\n");
    else if (error == 2)
        fprintf(stderr, "error : ステージファイルが開けません.\n");
    else if (error == 3)
        fprintf(stderr, "error : ステージファイルの書き方がおかしいです.\n");
    return EXIT_FAILURE;
}
