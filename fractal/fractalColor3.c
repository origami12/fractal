/**
 * マンデルブロ集合のフラクタル図形カラー描画プログラム(3色グラデーション)
 * 経営情報学部システム情報学科3年 今田 翔
 * 作成日: 2013/06/07
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUTPUT_WIDTH           256 //出力する画像の横(pixel)
#define OUTPUT_HEIGHT          256 //出力する画像の高さ(pixel)
#define COMPLEX_NUMBER_PLANE_X 1.5 //複素数平面のx軸の最大値
#define COMPLEX_NUMBER_PLANE_Y 1.5 //複素数平面のy軸の最大値
#define E                      4.0 //発散判定


//RGBの構造体
struct rgb{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

//画像データ(PPM)の構造体
struct imageCanvas{
    char *magicNumber; //マジックナンバー
    int  width;        //画像の横の長さ(pixel)
    int  height;       //画像の高さ(pixel)
    struct rgb color[256][256]; //rgbデータ
};

struct rgb *createCanvas(int,int);
struct rgb charToRGB(char*);
void       imageCanvasRGBSet(struct rgb*,int,int,int);
void       drawPixelColor(struct imageCanvas*,int,int,int,int,struct rgb,struct rgb,struct rgb);
int        convergenceInspection(int,double,double,double);

int main(int argc, char* argv[])
{

    int    i,j;                //イテレータ変数
    double x,y;                //座標変数
    struct imageCanvas *image; //画像のキャンバス
    int    diverge;            //発散の強さ
    struct rgb startRGB;       //グラデーションの開始値
    struct rgb middleRGB;      //グラデーションの中央値
    struct rgb endRGB;         //グラデーションの終了値
    int    inspectionRepeat;   //試行回数

    //引数の処理
    if(argc != 5)
    {
        fputs("コマンドライン引数が間違っています\n",stderr);
        exit( EXIT_FAILURE );
    }

    //コマンドライン引数 RGBの処理
    startRGB = charToRGB(argv[1]);
    printf("%d\n",startRGB.red);
    if(startRGB.red == -1 || strlen(argv[1]) != 6)
    {
        fputs("RGBの指定が間違っています\n", stderr);
        exit( EXIT_FAILURE );
    }
    middleRGB = charToRGB(argv[2]);

    printf("%d\n",middleRGB.green);
    if(middleRGB.red == -1 || strlen(argv[2]) != 6)
    {
        fputs("RGBの指定が間違っています\n", stderr);
        exit( EXIT_FAILURE );
    }
    endRGB = charToRGB(argv[3]);
    printf("%d\n",endRGB.blue);
    if(endRGB.red == -1 || strlen(argv[3]) != 6)
    {
        fputs("RGBの指定が間違っています\n", stderr);
        exit( EXIT_FAILURE );
    }
    //試行回数の代入
    if((inspectionRepeat = atoi(argv[4])) <= 0)
    {
        fputs("数値に変換できないあるいは、負の値です\n", stderr);
        exit( EXIT_FAILURE );
    }

    image->magicNumber = "P3";
    image->width       = OUTPUT_WIDTH;
    image->height      = OUTPUT_HEIGHT;

    //キャンバスに値を代入
    for(i = 0; i < OUTPUT_WIDTH; i++)
    {
        for(j = 0; j < OUTPUT_HEIGHT; j++)
        {
            x = COMPLEX_NUMBER_PLANE_X * 2.0 * j / OUTPUT_WIDTH - COMPLEX_NUMBER_PLANE_X;
            y = COMPLEX_NUMBER_PLANE_Y * 2.0 * i / OUTPUT_HEIGHT - COMPLEX_NUMBER_PLANE_Y;
            diverge = convergenceInspection(inspectionRepeat, E, x, y);
            drawPixelColor(image, i, j, diverge, inspectionRepeat, startRGB, middleRGB, endRGB);
        }
    }

    //ヘッダの書き出し
    printf("%s\n",image->magicNumber);
    printf("%d %d\n",image->width,image->height);
    printf("%d\n",255);

    //キャンバスを描画
    for(i = 0; i < OUTPUT_WIDTH; i++)
    {
        for(j = 0; j < OUTPUT_HEIGHT; j++)
        {
            printf("%d %d %d ",image->color[i][j].red,
                               image->color[i][j].green,
                               image->color[i][j].blue);
        }
    }

    return 0;

}

/**
 * 横と高さを指定しキャンバスを作成する
 * @param int width            : 横
 * @param int height           : 高さ
 * @return struct rgb          : キャンバスの先頭アドレス
 */
struct rgb *createCanvas(int width, int height)
{

    //メモリの動的確保
    return (struct rgb*)malloc(sizeof(struct rgb) * width * height);

}

/**
 * 文字データの色コード(16進数)をrgb構造体に変換する関数
 * @param  char *color : 色データ
 * @return struct rgb  : データのrgb構造体
 */
struct rgb charToRGB(char *color)
{

    struct rgb returnRGB;
    char       *errorWord;
    int        convertRGB;

    convertRGB = strtol(color, &errorWord, 16);

    if(strlen(errorWord) == 0)
    {
        returnRGB.red   = (convertRGB & 0xff0000) >> 16;
        returnRGB.green = (convertRGB & 0x00ff00) >> 8;
        returnRGB.blue  = convertRGB & 0x0000ff;
    }else
    {
        returnRGB.red   = -1;
        returnRGB.green = -1;
        returnRGB.blue  = -1;
    }

    return returnRGB;

}

/**
 * imageCanvas構造体にRGBをセットする関数
 * @param struct rgb *image       : キャンバスのカラーのポインタ
 * @param int red                   : r値
 * @param int green                 : g値
 * @param int blue                  : b値
 */
void imageCanvasRGBSet(struct rgb *image, int red, int green, int blue)
{

    image->red   = red;
    image->green = green;
    image->blue  = blue;

}

/**
 * 座標x,yのピクセルをカラーで描画する関数
 * @param struct imageCanvas image : キャンバスのポインタ
 * @param int x                    : x座標
 * @param int y                    : y座標
 * @param int diverge              : 発散の強さ
 * @param int inspectionRepeat     : 試行回数
 * @param struct rgb startRGB      : 3色グラデーションの初期値
 * @param struct rgb middleRGB     : 3色グラデーションの中間値
 * @param struct rgb endRGB        : 3色グラデーションの終了値
 */
void drawPixelColor(struct imageCanvas *image, int x, int y, int diverge,
        int inspectionRepeat, struct rgb startRGB, struct rgb middleRGB,
        struct rgb endRGB)
{
    printf("%d %d\n" , x,y);
    //発散の度合いによって色情報をキャンバスに挿入
    if(diverge == -1)
    {
        image->color[x][y].red = endRGB.red;
        image->color[x][y].green = endRGB.green;
        image->color[x][y].blue = endRGB.blue;
    }
    else
    {
        double convergenceRatio = (double)diverge / inspectionRepeat;
        if(convergenceRatio > 0.5)
        {
            //中間値と終了値の値を元にして代入する
            convergenceRatio = (convergenceRatio - 0.5) * 2; 
            image->color[x][y].red = (endRGB.red - middleRGB.red) * convergenceRatio + middleRGB.red;
            image->color[x][y].green = (endRGB.green - middleRGB.green) * convergenceRatio + middleRGB.green;
            image->color[x][y].blue = (endRGB.blue - middleRGB.blue) * convergenceRatio + middleRGB.blue;
        }else
        {
            convergenceRatio = convergenceRatio * 2;
            image->color[x][y].red = (middleRGB.red - startRGB.red) * convergenceRatio + startRGB.red;
            image->color[x][y].green = (middleRGB.green - startRGB.green) * convergenceRatio + startRGB.green;
            image->color[x][y].blue = (middleRGB.blue - startRGB.blue) * convergenceRatio + startRGB.blue;
        }
    }

}

/**
 * 与えられた検査回数で実部と虚部が収束するか調べる関数
 * @param  int inspectionRepeat : 検査回数
 * @param  double e             : 発散と判定する値
 * @param  double x             : 実部
 * @param  double y             : 虚部
 * @return int                  : 発散か収束 発散の時は発散の強さ 収束は-1
 */
int convergenceInspection(int inspectionRepeat, double e, double x, double y)
{

    int i;
    double zx, zy, zxNum, zyNum; //計算用変数

    zx = 0.0;
    zy = 0.0;

    //発散するかのチェック
    for(i = 0; i < inspectionRepeat; i++){
        zxNum = zx * zx - zy * zy + x;
        zyNum = 2 * zx * zy + y;
        if(zxNum * zxNum + zyNum * zyNum > e){
            return i;
        }
        zx = zxNum;
        zy = zyNum;
    }

    return -1;

}

