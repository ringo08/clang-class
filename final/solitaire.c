#include <stdio.h>
#include <handy.h>
#include <stdlib.h>
#include <time.h>

//  トランプ       ：   https://chicodeza.com/freeitems/torannpu-illust.html
//  トランプの裏面 ：   https://www.ac-illust.com/main/detail.php?id=65472&word=1%E6%9E%9A%E3%81%AE%E3%83%88%E3%83%A9%E3%83%B3%E3%83%97

#define WINDOW_X 1200
#define WINDOW_Y 800

typedef struct {
    int number;  // 1~13
    int mark;  // 0~4
    int side;  // 0 or 1
    int image;
}Card;

void cardInput(int mode, Card *tramp);                                        // トランプにカードの数字、マーク、画像を代入
void cardShuffle(Card *tramp);                                                // トランプをシャッフル
void cardDeal(Card *tramp, Card *deck, Card field[][52]);                     // トランプをデッキ、フィールドに配る
void imageFirst(Card field[][52]);                                            // 開始時の画像の表示
void mouseDownField(double fx, double fy, hgevent *event, Card field[][52]);  // トランプをクリックした時の処理
void rowNum(Card field[][52]);                                                // 各行が何枚あるかの判定
void judge(int i, Card field[][52]);                                          // トランプのマーク、数字が並んでいるかの判定
void cardMove(double fx, double fy, hgevent *event, Card field[][52]);        // トランプを動かす処理
void cardPut(hgevent *event, Card field[][52]);                               // トランプを移動させる処理
void deckDeal(Card *deck, Card field[][52]);                                  // デッキからトランプを配る
void complete(int i, Card field[][52]);                                       // 1~13まで同じマークで並んでいたら、上の列にストック

int spadeImage[13], cloverImage[13], diamondImage[13], heartImage[13], trampImage, background;  // 画像を入れるための変数
int layer[10], layerMove, deckLayer, layerPic, finishLayer, fieldLayer[10];                     // レイヤーを入れるための変数
double xField[10], yFieldBase;                                                                  // トランプの各行各列の基準となる座標
int lineNum[10] = {}, number[10], deckNumber = 0, backNum[10] = {};                             // トランプの数に関する変数
int w = -1, h = -1;                                                                             // クリック時のトランプの行と列
double x, y;                                                                                    // 移動時のx、y座標
int count = 0;                                                                                  // 上にストックされたペアの数
int flagPut = -1;
doubleLayer layers;

int main() {
    Card tramp[104];
    Card deck[50];
    Card field[10][52] = {};
    int i, j, flag;
    int temp, random;
    double fx, fy;
    int moveVar = 0;
    srand(time(NULL));
    int mode, modeflag =　0;

    printf("Please Choose Mode \n easy : 1 \t normal : 2 \t hard : 3\n");
    scanf("%d", &mode);

    for(; modeflag==0; ){
        if(mode==1||mode==2||mode==3){
            modeflag = 1;
        }
        else {
            printf("easy : 1 \t normal : 2 \t hard : 3 \n上の一覧から選んでください\n");

            scanf("%d", &mode);
        }
    }

    for(i = 0; i < 10; i++) {
        xField[i] = (WINDOW_X-100) * (i + 1) / 10;
        number[i] = 1;
    }
    yFieldBase = + (WINDOW_Y * 5.5 / 8);

    cardInput(mode, tramp);
    cardShuffle(tramp);
    cardDeal(tramp, deck, field);

    HgOpen(WINDOW_X, WINDOW_Y);
    HgWSetEventMask(0, HG_MOUSE_EVENT_MASK);
    HgWSetColor(0, HG_WHITE);
    HgWSetFont(0, HG_MI, 25);

    for(i = 0; i < 10; i++) {
        layer[i] = HgWAddLayer(0);
    }
    deckLayer = HgWAddLayer(0);
    finishLayer = HgWAddLayer(0);
    layerPic = HgWAddLayer(0);
    layers = HgWAddDoubleLayer(0);

    imageFirst(field);

    for(; count != 8; ) {
        hgevent *event = HgEventNonBlocking();
        if(event != NULL) {
            if(event->type == HG_MOUSE_DOWN) {
                rowNum(field);
                fx = event -> x;
                fy = event -> y;

                if(fx >= xField[0] - 50
                && fx <= xField[0] + 50
                && fy <= WINDOW_Y * 7 / 8 + 10 + 60
                && fy >= WINDOW_Y * 7 / 8 + 10 - 60) {
                    if(deckNumber != 0) {
                        deckDeal(deck, field);
                        flag = 0;
                    }
                }
                else {
                    mouseDownField(fx, fy, event, field);
                }
            }//end if:79
            if(event -> type == HG_MOUSE_DRAG && field[w][h].side == 1) {
                moveVar = 0;
                cardMove(fx, fy, event, field);
            }
            else if(event -> type == HG_MOUSE_UP) {
                HgLClear(layerPic);
            }
            if(event -> type != HG_MOUSE_DRAG && moveVar == 0 && field[w][h].side == 1) {
                moveVar = 1;
                cardPut(event, field);
                flag = 0;
            }
            if(deckNumber == 0) {
                HgLClear(deckLayer);
            }
            if(flag == 0){
                for(i = 0; i < 10; i++) {
                    judge(i, field);
                }
                for(i = 0; i < 10; i++) {
                    if(number[i] == 13) {
                        complete(i, field);
                        flag = 1;
                    }
                }
            }
        }//end if:80
    }// end for:78

    HgWSetColor(0, HG_BLACK);
    HgWSetFont(0, HG_GBI, 50);
    HgWText(0, WINDOW_X/4 + 80, WINDOW_Y/2 - 20, "Congratulations!!");

    HgGetChar();
    HgClose();

    return 0;
}


void cardInput(int mode, Card *tramp) {
    int i, kind, j;
    int count = 0;
    background = HgImageLoad("./png/background.png");
    trampImage = HgImageLoad("./png/cover.png");
    switch (mode) {
        case 1: kind=8;break;
        case 2: kind=4;break;
        case 3: kind=2;break;
    }

    for(i = 0; i < 13*kind; i++) {
        count++;
        (tramp + count) -> number = count % 13 + 1;
        (tramp + count) -> side = 0;
        (tramp + count) -> mark = 1;
        switch (tramp[count].number) {
            case 1 :
                (tramp + count) -> image = HgImageLoad("./png/spade1.png");
                break;
            case 2 :
                (tramp + count) -> image = HgImageLoad("./png/spade2.png");
                break;
            case 3 :
                (tramp + count) -> image = HgImageLoad("./png/spade3.png");
                break;
            case 4 :
                (tramp + count) -> image = HgImageLoad("./png/spade4.png");
                break;
            case 5 :
                (tramp + count) -> image = HgImageLoad("./png/spade5.png");
                break;
            case 6 :
                (tramp + count) -> image = HgImageLoad("./png/spade6.png");
                break;
            case 7 :
                (tramp + count) -> image = HgImageLoad("./png/spade7.png");
                break;
            case 8 :
                (tramp + count) -> image = HgImageLoad("./png/spade8.png");
                break;
            case 9 :
                (tramp + count) -> image = HgImageLoad("./png/spade9.png");
                break;
            case 10 :
                (tramp + count) -> image = HgImageLoad("./png/spade10.png");
                break;
            case 11 :
                (tramp + count) -> image = HgImageLoad("./png/spade11.png");
                break;
            case 12 :
                (tramp + count) -> image = HgImageLoad("./png/spade12.png");
                break;
            case 13 :
                (tramp + count) -> image = HgImageLoad("./png/spade13.png");
                break;
        }
    }
    if(count != 104){
        for(i = 0; i < 13*kind; i++) {
            count++;
            (tramp + count) -> number = count % 13 + 1;
            (tramp + count) -> side = 0;
            (tramp + count) -> mark = 2;
            switch (tramp[count].number) {
                case 1 :
                (tramp + count) -> image = HgImageLoad("./png/clover1.png");
                break;
                case 2 :
                (tramp + count) -> image = HgImageLoad("./png/clover2.png");
                break;
                case 3 :
                (tramp + count) -> image = HgImageLoad("./png/clover3.png");
                break;
                case 4 :
                (tramp + count) -> image = HgImageLoad("./png/clover4.png");
                break;
                case 5 :
                (tramp + count) -> image = HgImageLoad("./png/clover5.png");
                break;
                case 6 :
                (tramp + count) -> image = HgImageLoad("./png/clover6.png");
                break;
                case 7 :
                (tramp + count) -> image = HgImageLoad("./png/clover7.png");
                break;
                case 8 :
                (tramp + count) -> image = HgImageLoad("./png/clover8.png");
                break;
                case 9 :
                (tramp + count) -> image = HgImageLoad("./png/clover9.png");
                break;
                case 10 :
                (tramp + count) -> image = HgImageLoad("./png/clover10.png");
                break;
                case 11 :
                (tramp + count) -> image = HgImageLoad("./png/clover11.png");
                break;
                case 12 :
                (tramp + count) -> image = HgImageLoad("./png/clover12.png");
                break;
                case 13 :
                (tramp + count) -> image = HgImageLoad("./png/clover13.png");
                break;
            }
        }//end for:194
    }
    if(count != 104){
        for(i = 0; i < 13*kind; i++) {
            count++;
            (tramp + count) -> number = count % 13 + 1;
            (tramp + count) -> side = 0;
            (tramp + count) -> mark = 3;
            switch (tramp[count].number) {
                case 1 :
                (tramp + count) -> image = HgImageLoad("./png/diamond1.png");
                break;
                case 2 :
                (tramp + count) -> image = HgImageLoad("./png/diamond2.png");
                break;
                case 3 :
                (tramp + count) -> image = HgImageLoad("./png/diamond3.png");
                break;
                case 4 :
                (tramp + count) -> image = HgImageLoad("./png/diamond4.png");
                break;
                case 5 :
                (tramp + count) -> image = HgImageLoad("./png/diamond5.png");
                break;
                case 6 :
                (tramp + count) -> image = HgImageLoad("./png/diamond6.png");
                break;
                case 7 :
                (tramp + count) -> image = HgImageLoad("./png/diamond7.png");
                break;
                case 8 :
                (tramp + count) -> image = HgImageLoad("./png/diamond8.png");
                break;
                case 9 :
                (tramp + count) -> image = HgImageLoad("./png/diamond9.png");
                break;
                case 10 :
                (tramp + count) -> image = HgImageLoad("./png/diamond10.png");
                break;
                case 11 :
                (tramp + count) -> image = HgImageLoad("./png/diamond11.png");
                break;
                case 12 :
                (tramp + count) -> image = HgImageLoad("./png/diamond12.png");
                break;
                case 13 :
                (tramp + count) -> image = HgImageLoad("./png/diamond13.png");
                break;
            }
        }
        for(i = 0; i < 13*kind; i++) {
            count++;
            (tramp + count) -> number = count % 13 + 1;
            (tramp + count) -> side = 0;
            (tramp + count) -> mark = 4;
            switch (tramp[count].number) {
                case 1 :
                (tramp + count) -> image = HgImageLoad("./png/heart1.png");
                break;
                case 2 :
                (tramp + count) -> image = HgImageLoad("./png/heart2.png");
                break;
                case 3 :
                (tramp + count) -> image = HgImageLoad("./png/heart3.png");
                break;
                case 4 :
                (tramp + count) -> image = HgImageLoad("./png/heart4.png");
                break;
                case 5 :
                (tramp + count) -> image = HgImageLoad("./png/heart5.png");
                break;
                case 6 :
                (tramp + count) -> image = HgImageLoad("./png/heart6.png");
                break;
                case 7 :
                (tramp + count) -> image = HgImageLoad("./png/heart7.png");
                break;
                case 8 :
                (tramp + count) -> image = HgImageLoad("./png/heart8.png");
                break;
                case 9 :
                (tramp + count) -> image = HgImageLoad("./png/heart9.png");
                break;
                case 10 :
                (tramp + count) -> image = HgImageLoad("./png/heart10.png");
                break;
                case 11 :
                (tramp + count) -> image = HgImageLoad("./png/heart11.png");
                break;
                case 12 :
                (tramp + count) -> image = HgImageLoad("./png/heart12.png");
                break;
                case 13 :
                (tramp + count) -> image = HgImageLoad("./png/heart13.png");
                break;
            }
        }
    }
}// end  void cardInput:137

void cardShuffle(Card *tramp) {
    int random;
    Card tmp;

    for(int i = 0; i < 104; i++) {
        random = rand()%104;
        tmp = tramp[i];
        tramp[i]  = tramp[random];
        tramp[random] = tmp;
    }
}//end  void cardShuffle:330

void cardDeal(Card *tramp, Card *deck, Card field[][52]) {
    int count, i, j, max;
    for(count = 0; count < 50; count++) {
        deck[count] = tramp[count];
        deck[count] = tramp[count];
        deck[count] = tramp[count];
        deck[count].side = 1;
    }
    deckNumber = count;

    for(i = 0; i < 10; i++) {
        if(i < 4) { max = 6; }
        else { max = 5; }

        for(j = 0; j < max; j++) {
            field[i][j] = tramp[count];

            if(j == max - 1) {
                field[i][j].side = 1;
            }
            else {
                field[i][j].side = 0;
            }
            count++;
        }
    }//end for:352
}//end  void cardDeal:342

void imageFirst(Card field[][52]) {
    int i, j, max;
    double trampSize = 0.28;

    HgWImagePut(0, WINDOW_X/2, WINDOW_Y/2, background, 100, 0);
    HgWImagePut(deckLayer, xField[0], WINDOW_Y * 7 / 8 + 10, trampImage, 1, 0);
    HgWText(deckLayer, xField[0] - 15, WINDOW_Y * 7 / 8 - 20, "%d" , deckNumber);

    for(i = 0; i < 10; i++) {
        if(i < 4) { max = 6; }
        else { max = 5; }

        for(j = 0; j < max; j++) {
            if(field[i][j].side == 1) {
                HgWImagePut(layer[i], xField[i], yFieldBase + (j + 1) * (-20), field[i][j].image, 1, 0);
            }
            else {
                HgWImagePut(layer[i], xField[i], yFieldBase + (j + 1) * (-19), trampImage, 1, 0);
                backNum[i]++;
            }
        }
    }
}//end  void imageFirst:370

void rowNum(Card field[][52]) {
    int i, j;
    for(i = 0; i< 10; i++) {
        lineNum[i] = 0;
    }
    for(i = 0; i < 10; i++) {
        for(j = 0; field[i][j].number != 0; j++) {
            lineNum[i]++;
        }
    }
}//end void rowNum:394

void mouseDownField(double fx, double fy, hgevent *event, Card field[][52]) {
    int i, j;
    for(i = 0; i < 10; i++) {
        for(j = 0; j < lineNum[i]; j++) {
                if(j + 1 == lineNum[i] && field[i][j].side == 1) {
                    if(fx >= xField[i] - 50
                    && fx <= xField[i] + 50
                    && fy >= yFieldBase + (j + 1) * (-20) - 70
                    && fy <= yFieldBase + (j + 1) * (-20) + 70) {
                        HgWImagePut(layerPic, xField[i], yFieldBase + (j + 1) * (-20), field[i][j].image, 1.1, 0);
                        w = i;
                        h = j;
                    }
                }
                else if (field[i][j].side == 1){
                    if(fx >= xField[i] - 50
                    && fx <= xField[i] + 50
                    && fy >= yFieldBase + (j + 1) * (-20) + 50
                    && fy <= yFieldBase + (j + 1) * (-20) + 70) {
                        HgWImagePut(layerPic, xField[i], yFieldBase + (j + 1) * (-20), field[i][j].image, 1.1, 0);
                        w = i;
                        h = j;
                    }
                }
        }//end for:409
    }//end for:408
}//end void mouseDownField:406

void cardMove(double fx, double fy, hgevent *event, Card field[][52]) {
    int i, j;
    for(i = 0; i < 10; i++) {
        for(j = 0; j < lineNum[i]; j++) {
            if(fx >= xField[i] - 50
            && fx <= xField[i] + 50
            && fy >= yFieldBase + (j + 1) * (-20) - 70
            && fy <= yFieldBase + (j + 1) * (-20) + 70) {
                HgLClear(layer[w]);
            }
        }
    }
    for(i = 0; i < 10; i++) {
        number[i] = 1;
        judge(i, field);
    }
    if(event -> type == HG_MOUSE_DRAG) {
        HgLClear(layerPic);
        layerMove = HgLSwitch(&layers);
        HgLClear(layerMove);
        HgWTransWtoA(0, event->x, event->y, &x, &y);
        for(j = 0; j < h; j++) {
            if(field[w][j].side == 1) {
                HgWImagePut(layerMove, xField[w], yFieldBase + (j + 1) * (-20), field[w][j].image, 1, 0);
            }
            else if(field[w][j].side == 0) {
                HgWImagePut(layerMove, xField[w], yFieldBase + (j + 1) * (-19), trampImage, 1, 0);
            }
        }
        for(j = h; j < h + number[w]; j++) {
            HgWImagePut(layerMove, x, y + (j - h) * (-20), field[w][j].image, 1, 0);
        }
        HgSleep(0.10);
    }//end if:450
}//end  void cardMove:434

void cardPut(hgevent *event, Card field[][52]) {
    int i, j;

    for(i = 0; i < 10; i++) {
        if(field[w][h].number == field[i][lineNum[i] - 1].number - 1 || lineNum[i] == 0) {
            if(x >= xField[i] - 50
            && x <= xField[i] + 50
            && y >= yFieldBase + (lineNum[i] + 1) * (-20) - 70
            && y <= yFieldBase + (lineNum[i] + 1) * (-20) + 70) {
                HgLClear(layerMove);
                layerMove = HgLSwitch(&layers);
                for(j = h; j < h + number[w]; j++) {
                    field[i][lineNum[i]] = field[w][j];
                    field[w][j].image = 0;
                    field[w][j].number = 0;
                    field[w][j].mark = 0;
                    lineNum[i]++;
                    lineNum[w]--;
                }
                if(field[w][lineNum[w] - 1].side == 0) {
                    field[w][lineNum[w] - 1].side = 1;
                }
                HgLClear(layer[i]);
                for(j = 0; j < lineNum[i]; j++) {
                    if(field[i][j].side == 1) {
                        HgWImagePut(layer[i], xField[i], yFieldBase + (j + 1) * (-20), field[i][j].image, 1, 0);
                    }
                    else if(field[i][j].side == 0) {
                        HgWImagePut(layer[i], xField[i], yFieldBase + (j + 1) * (-19), trampImage, 1, 0);
                    }
                }
                HgLClear(layer[w]);
                for(j = 0; j < lineNum[w]; j++) {
                    if(field[w][j].side == 1) {
                        HgWImagePut(layer[w], xField[w], yFieldBase + (j + 1) * (-20), field[w][j].image, 1, 0);
                    }
                    else if(field[w][j].side == 0){
                        HgWImagePut(layer[w], xField[w], yFieldBase + (j + 1) * (-19), trampImage, 1, 0);
                    }
                }
            }//end if:478
            break;
        }//end if:474
        else {
            HgLClear(layerMove);
            layerMove = HgLSwitch(&layers);
            for(j = 0; j < lineNum[w]; j++) {
                if(field[w][j].side == 1) {
                    HgWImagePut(layer[w], xField[w], yFieldBase + (j + 1) * (-20), field[w][j].image, 1, 0);
                }
                else if(field[w][j].side == 0){
                    HgWImagePut(layer[w], xField[w], yFieldBase + (j + 1) * (-19), trampImage, 1, 0);
                }
            }
        }
    }// end for:473
    w = -1;
    h = -1;
}// end  void cardPut:470

void deckDeal(Card *deck, Card field[][52]) {
    int i, j;
    for(i = 0; i < 10; i++) {
        field[i][lineNum[i]] = deck[deckNumber];
        deckNumber--;
    }
    for(i = 0; i < 10; i++) {
        HgWImagePut(layer[i], xField[i], yFieldBase + (lineNum[i] + 1) * (-20), field[i][lineNum[i]].image, 1, 0);
        lineNum[i]++;
    }
    HgLClear(deckLayer);
    HgWImagePut(deckLayer, xField[0], WINDOW_Y * 7 / 8 + 10, trampImage, 1, 0);
    HgWText(deckLayer, xField[0] - 15, WINDOW_Y * 7 / 8 - 20, "%d" , deckNumber);
}//end  void deckDeal:530

void judge(int i, Card field[][52]) {
    if(field[i][lineNum[i] - 1].side == 1
    && field[i][lineNum[i] - 1].side == field[i][lineNum[i] - 1 - number[i]].side
    && field[i][lineNum[i] - 1].mark == field[i][lineNum[i] - 1 - number[i]].mark
    && field[i][lineNum[i] - 1].number == field[i][lineNum[i] - 1 - number[i]].number - number[i]) {
        if(h - 1!= lineNum[i] - 1 - number[i]) {
            number[i]++;
            judge(i, field);
        }
    }
}//end  void judge:546

void complete(int i, Card field[][52]) {
    int j, k = 13;
    HgLClear(layer[i]);
    for(j = lineNum[i] - 1; j >= lineNum[i] - k; j--) {
        if(field[i][j].number == 13) {
            HgWImagePut(finishLayer, xField[count + 2], WINDOW_Y * 7 / 8 + 10, field[i][j].image, 1, 0);
            count++;
        }
        field[i][j].mark = 0;
        field[i][j].number = 0;
        field[i][j].image = 0;
    }
    lineNum[i] -= k;

    if(field[i][lineNum[i] - 1].side == 0) {
        field[i][lineNum[i] - 1].side = 1;
    }
    for(j = 0; j < lineNum[i]; j++) {
        if(field[i][j].side == 1){
            HgWImagePut(layer[i], xField[i], yFieldBase + (j + 1) * (-20), field[i][j].image, 1, 0);
        }
        else if(field[i][j].side == 0) {
            HgWImagePut(layer[i], xField[i], yFieldBase + (j + 1) * (-19), trampImage, 1, 0);
        }
    }
}//end  void complete:558
