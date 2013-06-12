/**
 * マンデルブロ集合のフラクタル図形カラー描画プログラム
 * 経営情報学部システム情報学科3年 今田 翔
 * 作成日: 2013/06/06
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUTPUT_WIDTH           256 //出力する画像の横(pixel)
#define OUTPUT_HEIGHT          256 //出力する画像の高さ(pixel)
#define COMPLEX_NUMBER_PLANE_X 1.5 //複素数平面のx軸の最大値
#define COMPLEX_NUMBER_PLANE_Y 1.5 //複素数平面のy軸の最大値
#define INSPECTION_REPEAT      100  //収束検査する際の試行回数
#define E                      4.0 //発散判定

int  *createCanvas(int,int);
void drawPixelMonochrome(int*,int,int,int);
void drawPixelColor(int*,int,int,int,int,int);
int  convergenceInspection(int,double,double,double);

int main(int argc, char* argv[])
{

	int    i,j;          //イテレータ変数
	double x,y;          //座標変数
	int    *imageCanvas; //画像のキャンバス
	int    diverge;      //発散の強さ
    int    startRGB;     //グラデーションの開始値
    int    endRGB;       //グラデーションの終了値
    char   *errorWord;   //エラー文字退避用

    //引数の処理
    if(argc != 3)
    {
        fputs("コマンドライン引数が間違っています\n",stderr);
        exit( EXIT_FAILURE );
    }
    //コマンドライン引数 RGBの処理
    startRGB = strtol(argv[1], &errorWord, 16);
    if(strlen(errorWord) != 0 || strlen(argv[1]) != 6)
    {
        fputs("RGBの指定が間違っています\n", stderr);
        exit( EXIT_FAILURE );
    }
    endRGB = strtol(argv[2], &errorWord, 16);
    if(strlen(errorWord) != 0 || strlen(argv[2]) != 6)
    {
        fputs("RGBの指定が間違っています\n", stderr);
        exit( EXIT_FAILURE );
    }

	//PPM形式のヘッダ出力
	printf("P3\n");
	printf("%d %d\n", OUTPUT_WIDTH, OUTPUT_HEIGHT);
	printf("%d\n", 255);

	//キャンバスの作成
	if((imageCanvas = createCanvas(OUTPUT_WIDTH, OUTPUT_HEIGHT)) == NULL)
	{
		printf("メモリ確保ができなかったのでプログラムを終了します\n");
		return 1;
	}

	//キャンバスに値を代入
	for(i = 0; i < OUTPUT_WIDTH; i++)
	{
		for(j = 0; j < OUTPUT_HEIGHT; j++)
		{
			x = COMPLEX_NUMBER_PLANE_X * 2.0 * j / OUTPUT_WIDTH - COMPLEX_NUMBER_PLANE_X;
			y = COMPLEX_NUMBER_PLANE_Y * 2.0 * i / OUTPUT_HEIGHT - COMPLEX_NUMBER_PLANE_Y;
			diverge = convergenceInspection(INSPECTION_REPEAT, E, x, y);
			drawPixelColor(imageCanvas, i, j, diverge, startRGB, endRGB);
		}
	}

	//キャンバスを描画
	for(i = 0; i < OUTPUT_WIDTH; i++)
	{
		for(j = 0; j < OUTPUT_HEIGHT; j++)
		{
			printf("%d %d %d ",*(imageCanvas + i * OUTPUT_HEIGHT * 3 + j * 3),
					           *(imageCanvas + i * OUTPUT_HEIGHT * 3 + j * 3 + 1),
					           *(imageCanvas + i * OUTPUT_HEIGHT * 3 + j * 3 + 2));
		}
	}

	//メモリの解放
	free(imageCanvas);

	return 0;

}

/**
 * 横と高さを指定しキャンバスを作成する
 * @param int width  : 横
 * @param int height : 高さ
 * @return int*      : キャンバスの先頭アドレス
 */
int *createCanvas(int width, int height)
{

	int i,j;
	int *canvas; //キャンバスのポインタ

	//メモリの動的確保
	if((canvas = (int*)malloc(sizeof(int) * width * height * 3)) == NULL)
	{
		return NULL;
	}

	return canvas;

}

/**
 * 座標x,yのピクセルをカラーで描画する関数
 * @param int *canvas    : キャンバスのポインタ
 * @param int x          : x座標
 * @param int y          : y座標
 * @param int diverge    : 発散の強さ
 * @param int startRGB   : 2色グラデーションの初期値
 * @param int endRGB     : 2色グラデーションの終了値
 */
void drawPixelColor(int *canvas, int x, int y, int diverge,
                    int startRGB, int endRGB)
{

    int i;
    int sR,sG,sB,eR,eG,eB;              //各RGB値

    //各RGB値の取得
    sR = (startRGB & 0xff0000) >> 16;
    sG = (startRGB & 0x00ff00) >> 8;
    sB = startRGB  & 0x0000ff;
    eR = (endRGB   & 0xff0000) >> 16;
    eG = (endRGB   & 0x00ff00) >> 8;
    eB = endRGB    & 0x0000ff;

    //発散の度合いによって色情報をキャンバスに挿入
    if(diverge == -1)
    {
        *(canvas + x * OUTPUT_HEIGHT * 3 + y * 3)     = eR;
        *(canvas + x * OUTPUT_HEIGHT * 3 + y * 3 + 1) = eG;
        *(canvas + x * OUTPUT_HEIGHT * 3 + y * 3 + 2) = eB;
    }else
    {
        double convergenceRatio = (double)diverge / INSPECTION_REPEAT;
        *(canvas + x * OUTPUT_HEIGHT * 3 + y * 3)     = (eR - sR) * convergenceRatio + sR;
        *(canvas + x * OUTPUT_HEIGHT * 3 + y * 3 + 1) = (eG - sG) * convergenceRatio + sG;
        *(canvas + x * OUTPUT_HEIGHT * 3 + y * 3 + 2) = (eB - sB) * convergenceRatio + sB;
    }

}


/**
 * 座標x,yのピクセルを描画する関数
 * @param int x          : x座標
 * @param int y          : y座標
 * @param int monochrome : 白か黒 0=白 1=黒
 */
void drawPixelMonochrome(int *canvas, int x, int y, int monochrome)
{

	int i;
	int rgb; //rgbの値

	if(monochrome == 0)
	{
		rgb = 255;
	}else
	{
		rgb = 0;
	}

	for(i = 0; i < 3; i++)
	{
		*(canvas + x * OUTPUT_HEIGHT * 3 + y * 3 + i) = rgb;
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

