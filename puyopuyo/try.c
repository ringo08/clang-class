/*  ぷよの大きさ:50
    プレイマット上     縦マス:13   横マス:6
    標準スピード:50   ＋落下:50

    window ------ wide:600   high:700



    ぷよ画像　　：　　フリー素材
    素材提供　　：　ぷにゅ
    　　　URL ：　http://ch.nicovideo.jp/punyu/blomaga/ar469732

*/
#include <stdio.h>
#include <handy.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_X 600
#define WINDOW_Y 700
#define FIELD_H 14
#define FIELD_W 8
#define PLAYMAT_H 13
#define PLAYMAT_W 6
#define PEACE_H 3
#define PEACE_W 3
#define SIZE 50
#define SPEED 50

void Count(int8_t x, int8_t y,int8_t *n);  // ぷよの数を数えるための関数
void Vanish(int8_t x, int8_t y);  // ぷよを消すための関数
int score_add(int x, int y);  // 加算点
void puyo_color(int layer, int x, int y , int color);



int8_t puyomath[FIELD_W][FIELD_H] = {0};  // プレイ画面のフィールド
int field_layer[FIELD_W][FIELD_H];  // ぷよ用レイヤ
int puyo_red, puyo_blue, puyo_yellow, puyo_green, puyo_purple;


int main(){
  int puyofield[FIELD_W][FIELD_H] = {0};  // 画面のピクセル
  int puyopeace[PEACE_W][PEACE_H] = {0};  // 予備ぷよのフィールド
  int radius = SIZE/2;  // ぷよの半径
  int random_wide, random_color1, random_color2, pre_random_color3, pre_random_color4;  // ランダム系変数
  int main_x, main_y, sub_x, sub_y, move_wide;  // 移動系変数
  int8_t i, j, p, q, t, a, b, n, r, k, l;  // カウント変数
  int8_t sx, sy, pre_main, pre_sub, wall;
  int window, score, plus;  // その他変数
  int combo = 0;
  double speed = 0.35;
  int m = 0;
  int puyo_picture[FIELD_W][FIELD_H];
  int puyo_main, puyo_sub;


  score = 0;
  HgOpen(WINDOW_X, WINDOW_Y);

  int layer1 = HgWAddLayer(0);
  int layer2 = HgWAddLayer(0);

  for(i=1; i<FIELD_W-1; i++){
    for(j=1; j<FIELD_H; j++){
      field_layer[i][j] = HgWAddLayer(0);  // ぷよ用レイヤの作成
    }
  }


//  field start
  HgWSetFillColor(0, HG_BLACK);
  HgWBoxFill(0, 0, 0, WINDOW_X, WINDOW_Y, 1);

  for(i=1; i<FIELD_W; i++){
    for(j=0; j<FIELD_H; j++){
      puyofield[i][j] = j*SIZE;
    }
  }

  HgWSetFillColor(0, HG_WHITE);
  HgWSetWidth(0, 0.3);
  for(i=1; i<=PLAYMAT_W; i++){
    for(j=1; j<PLAYMAT_H; j++){
      HgWBoxFill(0, i*SIZE, puyofield[i][j], SIZE, SIZE, 1);
    }
  }

  for(i=0; i<PEACE_W; i++){
    for(j=0; j<PEACE_H; j++){
      puyopeace[i][j] = (j+10)*SIZE;
      HgWBoxFill(0, (i+8)*SIZE, puyopeace[i][j], SIZE, SIZE, 1);
    }
  }

  HgWSetColor(0, HG_RED);
  HgWSetWidth(0, 10);
  HgWLine(0, SIZE*3.2, SIZE*12.8, SIZE*3.8, SIZE*12.2);
  HgWLine(0, SIZE*3.2, SIZE*12.2, SIZE*3.8, SIZE*12.8);
// field end


// contents start
  HgWSetEventMask(layer1, HG_KEY_DOWN);

  for(i=0; i<FIELD_W; i++){
    puyomath[i][0] = 1;
  }

  for(j=0; j<FIELD_H; j++){
    puyomath[0][j] = 1;
    puyomath[7][j] = 1;
  }

  srand(time(NULL));

  random_color1 = rand()%5+2;  // 最初の mainぷよ の色
  random_color2 = rand()%5+2;  // 最初の subぷよ の色

  pre_main = (random_wide*SIZE+move_wide)/SIZE;  // 初期のmainぷよの横軸
  pre_sub = (random_wide*SIZE+move_wide)/SIZE;  // 初期のsubぷよの横軸


  puyo_red = HgImageLoad("puyo_red.gif");
  puyo_blue = HgImageLoad("puyo_blue.gif");
  puyo_yellow = HgImageLoad("puyo_yellow.gif");
  puyo_green = HgImageLoad("puyo_green.gif");
  puyo_purple = HgImageLoad("puyo_purple.gif");



  for(;;){
    pre_random_color3 = rand()%5+2;  // 次の mainぷよ の色
    pre_random_color4 = rand()%5+2;  // 次の subぷよ の色

    random_wide = rand()%6+1;  // ぷよの最初の横座標

    move_wide = 0;
    sx = 0;
    sy = -1;
    wall = 0;
    main_x = random_wide*SIZE + radius;  // mainぷよのx座標
    sub_x = random_wide*SIZE + radius;  // subぷよのx座標


// 次のmainぷよ表示
    puyo_color(layer2, 9*SIZE+radius, 11*SIZE+radius, pre_random_color3);


// 次のsubぷよ表示
    puyo_color(layer2, 9*SIZE+radius, 10*SIZE+radius, pre_random_color4);


    pre_main = (random_wide*SIZE+move_wide)/SIZE;  // mainぷよの横軸
    pre_sub = (random_wide*SIZE+move_wide)/SIZE;  // subぷよの横軸

    p = 13;
    q = 13;
    t = 0;


// 移動開始
    for(;((puyomath[pre_main][p] == 0)||(puyomath[pre_sub+sx][q+sy] == 0));){

// キータイプ設定
      hgevent *event = HgEventNonBlocking();
      if(event != NULL){
        if(event->type == HG_KEY_DOWN){
          switch(event->ch){
            case HG_D_ARROW :  // ↓タイプ時　　落下速度アップ
              if((puyomath[pre_main][p-2] == 0)&&(puyomath[pre_sub+sx][q+sy-2] == 0)){
                p--;
                q--;
              }
            break;

            case HG_L_ARROW :  // ←タイプ時　　左にずれる
              if((puyomath[pre_main-1][p-1]==0)&&(puyomath[pre_sub+sx-1][q+sy-1] == 0)) {
                move_wide += -SIZE;
              }
            break;

            case HG_R_ARROW :  // →タイプ時　　右にずれる
              if((puyomath[pre_main+1][p-1]==0)&&(puyomath[pre_sub+sx+1][q+sy-1] == 0)) {
                move_wide += SIZE;
              }
            break;

            case HG_U_ARROW :  // ↑タイプ時　　回転する
              t += 1;
              if(t >= 4) {
                t = 0;
              }

              if(t==0&&puyomath[pre_main][p-2]==0) {
                sx = 0;
                sy = -1;
              }

              else if(t==1&&puyomath[pre_main-1][p-1]==0) {
                sx = -1;
                sy = 0;
              }
              else if(t==2) {
                sx = 0;
                sy = 1;
              }
              else if(t==3&&puyomath[pre_main+1][p-1]==0) {
                sx = 1;
                sy = 0;
              }


              else if(t==1&&puyomath[pre_main-1][p-1]!=0&&puyomath[pre_main+1][p-1]==0){
                sx = -1;
                sy = 0;
                wall = -1;
              }
              else if(t==3&&puyomath[pre_main+1][p-1]!=0&&puyomath[pre_main-1][p-1]==0){
                sx = 1;
                sy = 0;
                wall = 1;
              }
            break;
          }
        }
      }

// ぷよの座標設定
      main_x = random_wide*SIZE + radius;  // 移動中mainぷよのx座標
      if(wall==-1){
        main_x += SIZE;
      }
      else if(wall==1){
        main_x -= SIZE;
      }
      pre_main = (main_x-radius+move_wide)/SIZE;  // 移動中mainぷよの横軸
      sub_x = main_x + SIZE*sx;  // 移動中subぷよのx座標
      pre_sub = (main_x-radius+move_wide)/SIZE;  // 移動中subぷよの横軸

//同時に落下
      if(sub_y >= main_y){
        if(puyomath[pre_main][p-1] == 0) {
          p--;
        }
        if(puyomath[pre_sub+sx][q+sy-1] == 0) {
          q--;
        }
      }
      else if(main_y >= sub_y){
        if(puyomath[pre_sub+sx][q+sy-1] == 0) {
          q--;
        }
        if(puyomath[pre_main][p-1] == 0) {
          p--;
        }
      }

      main_y = puyofield[pre_main][p] + radius;  // 移動中mainぷよのy座標
      sub_y = puyofield[pre_sub+sx][q+sy] + radius;  // 移動中subぷよのy座標


// 落下ぷよの配色設定
      puyo_color(layer1, main_x+move_wide, main_y, random_color1);

      puyo_color(layer1, sub_x+move_wide, sub_y, random_color2);

      HgSleep(speed);


      if(sub_y >= main_y){
        if(puyomath[pre_main][p-1] != 0) {
          if(random_color1==2) {
            puyomath[pre_main][p] = 2;  // RED
          }
          else if(random_color1==3) {
            puyomath[pre_main][p] = 3;  // BLUE
          }
          else if(random_color1==4) {
            puyomath[pre_main][p] = 4;  // YELLOW
          }
          else if(random_color1==5) {
            puyomath[pre_main][p] = 5;  // GREEN
          }
          else if(random_color1==6) {
            puyomath[pre_main][p] = 6;  // PURPLE
          }
        }
        if(puyomath[pre_sub+sx][q+sy-1] != 0) {
          if(random_color2==2) {
            puyomath[pre_sub+sx][q+sy] = 2;  // RED
          }
          else if(random_color2==3) {
            puyomath[pre_sub+sx][q+sy] = 3;  // BLUE
          }
          else if(random_color2==4) {
            puyomath[pre_sub+sx][q+sy] = 4;  // YELLOW
          }
          else if(random_color2==5) {
            puyomath[pre_sub+sx][q+sy] = 5;  // GREEN
          }
          else if(random_color2==6) {
            puyomath[pre_sub+sx][q+sy] = 6;  // PURPLE
          }
        }
      }


      else if(main_y >= sub_y){
        if(puyomath[pre_sub+sx][q+sy-1] != 0) {
          if(random_color2==2) {
            puyomath[pre_sub+sx][q+sy] = 2;  // RED
          }
          else if(random_color2==3) {
            puyomath[pre_sub+sx][q+sy] = 3;  // BLUE
          }
          else if(random_color2==4) {
            puyomath[pre_sub+sx][q+sy] = 4;  // YELLOW
          }
          else if(random_color2==5) {
            puyomath[pre_sub+sx][q+sy] = 5;  // GREEN
          }
          else if(random_color2==6) {
            puyomath[pre_sub+sx][q+sy] = 6;  // PURPLE
          }
        }
        if(puyomath[pre_main][p-1] != 0) {
          if(random_color1==2) {
            puyomath[pre_main][p] = 2;  // RED
          }
          else if(random_color1==3) {
            puyomath[pre_main][p] = 3;  // BLUE
          }
          else if(random_color1==4) {
            puyomath[pre_main][p] = 4;  // YELLOW
          }
          else if(random_color1==5) {
            puyomath[pre_main][p] = 5;  // GREEN
          }
          else if(random_color1==6) {
            puyomath[pre_main][p] = 6;  // PURPLE
          }
        }
      }

      HgLClear(layer1);

      a = p;
      b = q;


    }
// 落下終了


// 落下後のぷよの位置確定
    puyo_color(field_layer[pre_main][a], main_x+move_wide, main_y, puyomath[pre_main][a]);

    puyo_color(field_layer[pre_sub+sx][b+sy], sub_x+move_wide, sub_y, puyomath[pre_sub+sx][b+sy]);


    loop:
// 浮いているぷよを落下させる
      for(i=1; i<FIELD_W; i++){
        for(j=1; j<FIELD_H; j++){
          for(r=0; r<FIELD_H-j; r++){
            if(puyomath[i][j]==0&&puyomath[i][j+r]!=0){

              puyomath[i][j] = puyomath[i][j+r];
              puyomath[i][j+r] = 0;
              HgLClear(field_layer[i][j+r]);
            }
          }
        }
      }

      for(i=1; i<FIELD_W; i++){
        for(j=1; j<FIELD_H; j++){
          if(puyomath[i][j]>=2){
            puyo_color(field_layer[i][j], i*SIZE+radius, j*SIZE+radius, puyomath[i][j]);
          }
        }
      }
      HgSleep(speed);


// 4連結上のぷよを消す
      for(i=1; i<FIELD_W; i++){
        for(j=1; j<FIELD_H; j++){
          if(puyomath[i][j]>=2){
            n=0;

            Count(i, j, &n);
            if(n >= 4){
              Vanish(i, j);
              combo++;
              m+=n;
              goto loop;
            }
          }
        }
      }


// スコア判定
    if(combo > 0){
      plus = score_add(combo, m);
      score = score+plus;
      combo = 0;
      m = 0;
      if(speed>=0.125){
        speed -= speed*0.005;
      }
    }

    HgLClear(layer2);

    HgWSetFont(layer2, HG_M, 35);
    HgWSetColor(layer2, HG_WHITE);
    HgWText(layer2, 415, 320, "SCORE\n \t\t%d", score);

    random_color1 = pre_random_color3;
    random_color2 = pre_random_color4;

    if(puyomath[3][12]!=0){
      break;
    }

  }

  HgWClear(0);


  HgWSetColor(0, HG_BLACK);
  HgWSetFont(0, HG_M, 50);
  HgWText(0, 125, 330, "GAME OVER");
  HgWText(0, 125, 250, "score : %d", score);


  HgGetChar();
  HgClose();

  return 0;
}

void Count(int8_t x, int8_t y, int8_t *n){  // ぷよの数を判定するための再帰関数
  if((x-1>=0&&x+1<=8)&&(y-1>=0&&y+1<=14)){
    if(puyomath[x][y]!=0){
      int8_t c = puyomath[x][y];      // 自分の色
      puyomath[x][y]=0;
      (*n)++;


      if(x+1<FIELD_W && puyomath[x+1][y]==c) {
        Count(x+1, y, n);
      }
      if(y+1<FIELD_H && puyomath[x][y+1]==c) {
        Count(x, y+1, n);
      }
      if(x-1>0 && puyomath[x-1][y]==c) {
        Count(x-1, y, n);
      }
      if(y-1>0 && puyomath[x][y-1]==c) {
        Count(x, y-1, n);
      }

      puyomath[x][y]=c;

    }
  }
}


void Vanish(int8_t x, int8_t y){  // ぷよを消すための再帰関数
  if((x-1>=0&&x+1<=8)&&(y-1>=0&&y+1<=14)){
    if(puyomath[x][y]!=0){
      int8_t c = puyomath[x][y];      // 自分の色

      puyomath[x][y]=0;
      HgLClear(field_layer[x][y]);
      if(x+1<FIELD_W && puyomath[x+1][y]==c) {
        Vanish(x+1, y);
      }
      if(y+1<FIELD_H && puyomath[x][y+1]==c) {
        Vanish(x, y+1);
      }
      if(x-1>0 && puyomath[x-1][y]==c) {
        Vanish(x-1, y);
      }
      if(y-1>0 && puyomath[x][y-1]==c) {
        Vanish(x, y-1);
      }
    }
  }
}

int score_add(int x, int y){
  int add = 0;
  int combo_bonus;

// コンボボーナス
  if(x==1) combo_bonus = 0;
  else if(x<=3) combo_bonus = 8*(x-1);
  else if(x<=6) combo_bonus = 32*(x-3);
  else if(x==7) combo_bonus = 128;
  else if(x==8) combo_bonus = 160;
  else if(x==9) combo_bonus = 192;
  else if(x==10) combo_bonus = 224;
  else if(x<10) combo_bonus = 256;

  add = y*10+combo_bonus;

  return add;
}

void puyo_color(int layer, int x, int y , int color){
  if(color==2){
    HgWImagePut(layer, x, y, puyo_red, 2.1, 0);
  }
  else if(color==3){
    HgWImagePut(layer, x, y, puyo_blue, 2.1, 0);
  }
  else if(color==4){
    HgWImagePut(layer, x, y, puyo_yellow, 2.1, 0);
  }
  else if(color==5){
    HgWImagePut(layer, x, y, puyo_green, 2.1, 0);
  }
  else if(color==6){
    HgWImagePut(layer, x, y, puyo_purple, 2.1, 0);
  }
}
