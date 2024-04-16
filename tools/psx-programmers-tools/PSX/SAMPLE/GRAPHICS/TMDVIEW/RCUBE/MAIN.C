/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *	rcube: PS-X Demonstration program
 *
 *	"main.c" Main routine
 *
 *		Version 3.02	Jan, 9, 1995
 *
 *		Copyright (C) 1993,1994,1995 by Sony Computer Entertainment
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "table.h"
#include "pos.h"

/* テクスチャ情報 */
#define TIM_ADDR 0x80020000		/* 使用するTIMファイルの格納アドレス */

#define TIM_HEADER 0x00000010

/* モデリングデータ情報 */
#define TMD_ADDR 0x80010000		/* 使用するTMDファイルの格納アドレス */

u_long *TmdBase;			/* TMDのうち、オブジェクト部のアドレス */

int CurrentTmd; 			/* 使用中のTMD番号 */

/* オーダリングテーブル (OT) */
#define OT_LENGTH  7			/* OT解像度（大きさ） */
GsOT WorldOT[2];			/* OT情報（ダブルバッファ） */
GsOT_TAG OTTags[2][1<<OT_LENGTH];	/* OTのタグ領域（ダブルバッファ） */

/* GPUパケット生成領域 */
#define PACKETMAX 1000			/* 1フレームの最大パケット数 */

PACKET GpuPacketArea[2][PACKETMAX*64];	/* パケット領域（ダブルバッファ） */

/* オブジェクト（キューブ）変数 */
#define NCUBE 44
#define OBJMAX NCUBE
int nobj;				/* キューブ個数 */
GsDOBJ2 object[OBJMAX];			/* 3Dオブジェクト変数 */
GsCOORDINATE2 objcoord[OBJMAX];		/* ローカル座標変数 */

SVECTOR Rot[OBJMAX];			/* 回転角 */
SVECTOR RotV[OBJMAX];			/* 回転スピード（角速度） */

VECTOR Trns[OBJMAX];			/* キューブ位置（平行移動量） */

VECTOR TrnsV[OBJMAX];			/* 移動スピード */

/* 視点（VIEW） */
GsRVIEW2  View;			/* 視点変数 */
int ViewAngleXZ;		/* 視点の高さ */
int ViewRadial;			/* 視点からの距離 */
#define DISTANCE 600		/* Radialの初期値 */

/* 光源 */
GsF_LIGHT pslt[3];			/* 光源情報変数×3 */

/* その他... */
int Bakuhatu;				/* 爆発処理フラグ */
u_long PadData;				/* コントロールパッドの情報 */
u_long oldpad;				/* １フレーム前のパッド情報 */
GsFOGPARAM dq;				/* デプスキュー(フォグ)用パラメータ */
int dqf;				/* フォグがONかどうか */
int back_r, back_g, back_b;		/* バックグラウンド色 */

#define FOG_R 160
#define FOG_G 170
#define FOG_B 180

/* 関数のプロトタイプ宣言 */
void drawCubes();
int moveCubes();
void initModelingData();
void allocCube();
void initSystem();
void initAll();
void initTexture();
void initView();
void initLight();
void changeFog();
void changeTmd();

/* メインルーチン */
main()
{
	/* システムの初期化 */
	ResetCallback();
	initSystem();

	/* その他の初期化 */
	Bakuhatu = 0;
	PadData = 0;
	CurrentTmd = 0;
	dqf = 0;
	back_r = back_g = back_b = 0;
	initView();
	initLight(0, 0xc0);
	initModelingData(TMD_ADDR);
	initTexture(TIM_ADDR);
	allocCube(NCUBE);
	
	/* メインループ */
	while(1) {
		if(moveCubes()==0)
		  return 0;
		GsSetRefView2(&View);
		drawCubes();
	}
}


/* 3Dオブジェクト（キューブ）の描画 */
void drawCubes()
{
	int i;
	GsDOBJ2 *objp;
	int activeBuff;
	MATRIX LsMtx;

	/* ダブルバッファのうちどちらがアクティブか？ */
	activeBuff = GsGetActiveBuff();

	/* GPUパケット生成アドレスをエリアの先頭に設定 */
	GsSetWorkBase((PACKET*)GpuPacketArea[activeBuff]);

	/* OTの内容をクリア */
	GsClearOt(0, 0, &WorldOT[activeBuff]);

	/* 3Dオブジェクト（キューブ）のOTへの登録 */
	objp = object;
	for(i = 0; i < nobj; i++) {

		/* 回転角->マトリクスにセット */
		RotMatrix(Rot+i, &(objp->coord2->coord));
		
		/* マトリクスを更新したのでフラグをリセット */
		objp->coord2->flg = 0;

		/* 平行移動量->マトリクスにセット */
		TransMatrix(&(objp->coord2->coord), &Trns[i]);
		
		/* 透視変換のためのマトリクスを計算してＧＴＥにセット */
		GsGetLs(objp->coord2, &LsMtx);
		GsSetLsMatrix(&LsMtx);
		GsSetLightMatrix(&LsMtx);

		/* 透視変換してOTに登録 */
		GsSortObject4(objp, &WorldOT[activeBuff], 14-OT_LENGTH,getScratchAddr(0));
		objp++;
	}

	/* パッドの内容をバッファに取り込む */
	oldpad = PadData;
	PadData = PadRead(1);
	
	/* V-BLANKを待つ */
 	VSync(0);
	
	/* 前のフレームの描画作業を強制終了 */
	ResetGraph(1);

	/* ダブルバッファを入れ換える */
	GsSwapDispBuff();

	/* OTの先頭に画面クリア命令を挿入 */
	GsSortClear(back_r, back_g, back_b, &WorldOT[activeBuff]);

	/* OTの内容をバックグラウンドで描画開始 */
	GsDrawOt(&WorldOT[activeBuff]);
}

/* キューブの移動 */
int moveCubes()
{
	int i;
	GsDOBJ2   *objp;
	
	/* プログラムを終了してモニタに戻る */
/*	if((PadData & PADk)>0) {PadStop();ResetGraph(3);StopCallback();return 0;}*/
	if((PadData & PADk)>0) {
		PadStop();
		ResetGraph(3);
		StopCallback();
		return 0;
	}
	
	/* パッドの値によって処理 */
	if((PadData & PADLleft)>0) {
		ViewAngleXZ++;
		if(ViewAngleXZ >= 72) {
			ViewAngleXZ = 0;
		}
	}
	if((PadData & PADLright)>0) {
		ViewAngleXZ--;
		if(ViewAngleXZ < 0) {
		  ViewAngleXZ = 71;
		}
	}
	if((PadData & PADLup)>0) View.vpy += 100;
	if((PadData & PADLdown)>0) View.vpy -= 100;
	if((PadData & PADRdown)>0) {
		ViewRadial-=3;
		if(ViewRadial < 8) {
			ViewRadial = 8;
		}
	}
	if((PadData & PADRright)>0) {
		ViewRadial+=3;
		if(ViewRadial > 450) {
			ViewRadial = 450;
		}
	}
	if((PadData & PADk)>0) return(-1);
	if(((PadData & PADRleft)>0)&&((oldpad&PADRleft) == 0)) changeFog();
	if(((PadData & PADRup)>0)&&((oldpad&PADRup) == 0)) changeTmd();
	if(((PadData & PADn)>0)&&((oldpad&PADn) == 0)) Bakuhatu = 1;
	if(((PadData & PADl)>0)&&((oldpad&PADl) == 0)) allocCube(NCUBE);

	View.vpx = rotx[ViewAngleXZ]*ViewRadial;
	View.vpz = roty[ViewAngleXZ]*ViewRadial;

	/* キューブの位置情報更新 */
	objp = object;
	for(i = 0; i < nobj; i++) {

		/* 爆発の開始 */
		if(Bakuhatu == 1) {

			/* 自転速度 up */
			RotV[i].vx *= 3;
			RotV[i].vy *= 3;
			RotV[i].vz *= 3;

			/* 移動方向&速度設定 */
			TrnsV[i].vx = objp->coord2->coord.t[0]/4+
						(rand()-16384)/200;	
			TrnsV[i].vy = objp->coord2->coord.t[1]/6+
						(rand()-16384)/200-200;
			TrnsV[i].vz = objp->coord2->coord.t[2]/4+
						(rand()-16384)/200;
		}
		/* 爆発中の処理 */
		else if(Bakuhatu > 1) {
			if(Trns[i].vy > 3000) {
				Trns[i].vy = 3000-(Trns[i].vy-3000)/2;
				TrnsV[i].vy = -TrnsV[i].vy*6/10;
			}
			else {
				TrnsV[i].vy += 20*2;	/* 自由落下 */
			}

			if((TrnsV[i].vy < 70)&&(TrnsV[i].vy > -70)&&
			   (Trns[i].vy > 2800)) {
				Trns[i].vy = 3000;
				TrnsV[i].vy = 0;

				RotV[i].vx *= 95/100;
				RotV[i].vy *= 95/100;
				RotV[i].vz *= 95/100;
			}


			TrnsV[i].vx = TrnsV[i].vx*97/100;
			TrnsV[i].vz = TrnsV[i].vz*97/100;
		}

		/* 回転角(Rotation)の更新 */
 		Rot[i].vx += RotV[i].vx;
		Rot[i].vy += RotV[i].vy;
		Rot[i].vz += RotV[i].vz;

		/* 平行移動量(Transfer)の更新 */
 		Trns[i].vx += TrnsV[i].vx;
		Trns[i].vy += TrnsV[i].vy;
		Trns[i].vz += TrnsV[i].vz;

		objp++;
	}

	if(Bakuhatu == 1)
		Bakuhatu++;

	return(1);
}

/* キューブを初期位置に配置 */
void allocCube(n)
int n;
{	
	int x, y, z;
	int i;
	int *posp;
	GsDOBJ2 *objp;
	GsCOORDINATE2 *coordp;

	posp = cube_def_pos;
	objp = object;
	coordp = objcoord;
	nobj = 0;
	for(i = 0; i < NCUBE; i++) {

		/* オブジェクト構造体の初期化 */
		GsLinkObject4((u_long)TmdBase, objp, CurrentTmd);
		GsInitCoordinate2(WORLD, coordp);
		objp->coord2 = coordp;
		objp->attribute = 0;
		coordp++;
		objp++;

		/* 初期位置の設定(pos.hから読む) */
		Trns[i].vx = *posp++;
		Trns[i].vy = *posp++;
		Trns[i].vz = *posp++;
		Rot[i].vx = 0;
		Rot[i].vy = 0;
		Rot[i].vz = 0;

		/* 速度の初期化 */
		TrnsV[i].vx = 0;
		TrnsV[i].vy = 0;
		TrnsV[i].vz = 0;
		RotV[i].vx = rand()/300;
		RotV[i].vy = rand()/300;
		RotV[i].vz = rand()/300;

		nobj++;
	}
	Bakuhatu = 0;
}

/* イニシャライズ関数群 */
void initSystem()
{
	int i;

	/* パッドの初期化 */
	PadInit(0);

	/* グラフィックの初期化 */
	GsInitGraph(640, 480, 2, 0, 0);
	GsDefDispBuff(0, 0, 0, 0);

	/* OTの初期化 */
	for(i = 0; i < 2; i++) {	
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].org = OTTags[i];
	}	

	/* 3Dシステムの初期化 */
	GsInit3D();
}

void initModelingData(tmdp)
u_long *tmdp;
{
	u_long size;
	int i;
	int tmdobjnum;
	
	/* ヘッダをスキップ */
	tmdp++;

	/* 実アドレスへマッピング */
	GsMapModelingData(tmdp);

	tmdp++;
	tmdobjnum = *tmdp;
	tmdp++; 		/* 先頭のオブジェクトをポイント */
	TmdBase = tmdp;
}

/* テクスチャの読み込み（VRAMへの転送） */
void initTexture(tex_addr)
u_long *tex_addr;
{
	RECT rect1;
	GsIMAGE tim1;
	int i;
	
	while(1) {
		if(*tex_addr != TIM_HEADER) {
			break;
		}
		tex_addr++;	/* ヘッダのスキップ(1word) */
		GsGetTimInfo(tex_addr, &tim1);
		tex_addr += tim1.pw*tim1.ph/2+3+1;	/* 次のブロックまで進める */

		rect1.x=tim1.px;
		rect1.y=tim1.py;
		rect1.w=tim1.pw;
		rect1.h=tim1.ph;
		LoadImage(&rect1,tim1.pixel);
		if((tim1.pmode>>3)&0x01) {	/* CLUTがあれば転送 */

			rect1.x=tim1.cx;
			rect1.y=tim1.cy;
			rect1.w=tim1.cw;
			rect1.h=tim1.ch;
			LoadImage(&rect1,tim1.clut);
			tex_addr += tim1.cw*tim1.ch/2+3;
		}
	}
}

/* 視点の初期化 */
void initView()
{
	/* 初期位置を視点変数にセット */
	ViewAngleXZ = 54;
	ViewRadial = 75;
	View.vpx = rotx[ViewAngleXZ]*ViewRadial;
	View.vpy = -100;
	View.vpz = roty[ViewAngleXZ]*ViewRadial;
	View.vrx = 0; View.vry = 0; View.vrz = 0;
	View.rz=0;

	/* 視点の親座標 */
	View.super = WORLD;

	/* 設定 */
	GsSetRefView2(&View);

	/* Projection */
	GsSetProjection(1000);

	/* Mode = 'normal lighting' */
	GsSetLightMode(0);
}

/* 光源の初期化 */
void initLight(c_mode, factor)
int c_mode;	/* ０のとき白色光、１のときカクテルライト */
int factor;	/* 明るさのファクター(0～255) */
{
	if(c_mode == 0) {
		/* 白色光のセット */
		pslt[0].vx = 200; pslt[0].vy= 200; pslt[0].vz= 300;
		pslt[0].r = factor; pslt[0].g = factor; pslt[0].b = factor;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -50; pslt[1].vy= -1000; pslt[1].vz= 0;
		pslt[1].r=0x20; pslt[1].g=0x20; pslt[1].b=0x20;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
		pslt[2].r=0x0; pslt[2].g=0x0; pslt[2].b=0x0;
		GsSetFlatLight(2,&pslt[2]);
	}
	else {
		/* カクテルライト（Gouraudで使用） */
		pslt[0].vx = 200; pslt[0].vy= 100; pslt[0].vz= 0;
		pslt[0].r = factor; pslt[0].g = 0; pslt[0].b = 0;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -200; pslt[1].vy= 100; pslt[1].vz= 0;
		pslt[1].r=0; pslt[1].g=0; pslt[1].b=factor;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = 0; pslt[2].vy= -200; pslt[2].vz= 0;
		pslt[2].r=0; pslt[2].g=factor; pslt[2].b=0;
		GsSetFlatLight(2,&pslt[2]);
	}	

	/* アンビエント（周辺光） */
	GsSetAmbient(ONE/2,ONE/2,ONE/2);
}

/* フォグのON/OFF */
void changeFog()
{
	if(dqf) {
		/* フォグのリセット */
		GsSetLightMode(0);
		dqf = 0;
		back_r = 0;
		back_g = 0;
		back_b = 0;
	}
	else {
		/* フォグの設定 */
		dq.dqa = -600;
		dq.dqb = 5120*4096;
		dq.rfc = FOG_R;
		dq.gfc = FOG_G;
		dq.bfc = FOG_B;
		GsSetFogParam(&dq);
		GsSetLightMode(1);
		dqf = 1;
		back_r = FOG_R;
		back_g = FOG_G;
		back_b = FOG_B;
	}
}

/* TMDデータの切り替え */
void changeTmd()
{
	u_long *tmdp;
	GsDOBJ2 *objp;
	int i;

	/* TMDを切り替え */
	CurrentTmd++;
	if(CurrentTmd == 4) {
		CurrentTmd = 0;
	}
	objp = object;
	for(i = 0; i < nobj; i++) {
		GsLinkObject4((u_long)TmdBase, objp, CurrentTmd);
		objp++;
	}

	/* TMDの種類にあわせて光源の色/明るさを切り替え */
	switch(CurrentTmd) {
	    case 0:
                /* ノーマル (flat) */
		initLight(0, 0xc0);
		break;
	    case 1:
	        /* 半透明 (flat) */
		initLight(0, 0xc0);
		break;
	    case 2:
	        /* Gouraud */
		initLight(1, 0xff);
		break;
	    case 3:
	        /* テクスチャ付き */
		initLight(0, 0xff);
		break;
	}
}

