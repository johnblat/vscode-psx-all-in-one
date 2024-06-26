/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/* GsOT に プリミティブを AddPrim する例 (GS ベース） */
/*	 Version	Date		Design
 *	-----------------------------------------
 *	2.00		Aug,31,1993	masa	(original)
 *	2.10		Mar,25,1994	suzu	(added addPrimitive())
 *      2.20            Dec,25,1994     yuta	(chaned GsDOBJ4)
 *      2.30            Mar, 5,1997     sachiko	(added autopad)
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

/* コントローラレコーダ用マクロ */
#define PadRead(x)	myPadRead(x)


#define MODEL_ADDR	(u_long *)0x80100000	/* modeling data info. */
#define TEX_ADDR	(u_long *)0x80180000	/* texture info. */
	
#define SCR_Z		1000		/* projection */
#define OT_LENGTH	12		/* OT resolution */
#define OTSIZE		(1<<OT_LENGTH)	/* OT tag size */
#define PACKETMAX	4000		/* max number of packets per frame */
#define PACKETMAX2	(PACKETMAX*24)	/* average packet size is 24 */

PACKET	GpuPacketArea[2][PACKETMAX2];	/* packet area (double buffer) */
GsOT	WorldOT[2];			/* OT info */
SVECTOR	PWorld;			 	/* vector for making Coordinates */

GsOT_TAG	OTTags[2][OTSIZE];	/* OT tag */
GsDOBJ2		object;			/* object substance */
GsRVIEW2	view;			/* view point */
GsF_LIGHT	pslt[3];		/* lighting point */
u_long		PadData;		/* controller info. */
GsCOORDINATE2   DWorld;			/* Coordinate for GsDOBJ2 */

extern MATRIX GsIDMATRIX;

static initSystem(void);
static void initView(void);
static void initLight(void);
static void initModelingData(u_long *addr);
static void initTexture(u_long *addr);
static void set_coordinate(SVECTOR *pos, GsCOORDINATE2 *coor);
static void initPrimitives(void);
static void addPrimitives(u_long *ot);
static int  moveObject(void);

/* メインルーチン */
void tuto0(void)
{
	
	/* イニシャライズ */
	initSystem();			/* grobal variables */
	initView();			/* position matrix */
	initLight();			/* light matrix */
	initModelingData(MODEL_ADDR);	/* load model data */
	initTexture(TEX_ADDR);		/* load texture pattern */
	initPrimitives();		/* GPU primitives */
	
	while(1) {
		if ( moveObject() ) break;
		drawObject();
	}
	DrawSync(0);
	return;
}

/* 3Dオブジェクト描画処理 */
drawObject()
{
	int activeBuff;
	MATRIX tmpls;
	
	/* ダブルバッファのうちどちらがアクティブか？ */
	activeBuff = GsGetActiveBuff();
	
	/* GPUパケット生成アドレスをエリアの先頭に設定 */
	GsSetWorkBase((PACKET*)GpuPacketArea[activeBuff]);
	
	/* OTの内容をクリア */
	ClearOTagR((u_long *)WorldOT[activeBuff].org, OTSIZE);
	
	/* 3Dオブジェクト（キューブ）のOTへの登録 */
	GsGetLw(object.coord2,&tmpls);		
	GsSetLightMatrix(&tmpls);
	GsGetLs(object.coord2,&tmpls);
	GsSetLsMatrix(&tmpls);
	GsSortObject4(&object,
		      &WorldOT[activeBuff],14-OT_LENGTH, getScratchAddr(0));
	
	/* プリミティブを追加 */
	addPrimitives((u_long *)WorldOT[activeBuff].org);
	
	/* パッドの内容をバッファに取り込む */
	PadData = PadRead(0);

	/* V-BLNKを待つ */
	VSync(0);

	/* 前のフレームの描画作業を強制終了 */
	ResetGraph(1);

	/* ダブルバッファを入れ換える */
	GsSwapDispBuff();

	/* OTの先頭に画面クリア命令を挿入 */
	GsSortClear(0x0, 0x0, 0x0, &WorldOT[activeBuff]);

	/* OTの内容をバックグラウンドで描画開始 */
	/*DumpOTag(WorldOT[activeBuff].org+OTSIZE-1);*/
	DrawOTag((u_long *) (WorldOT[activeBuff].org+OTSIZE-1));
}

/* コントロールパッドによるオブジェクト移動 */
static int moveObject(void)
{
	/* オブジェクト変数内のローカル座標系の値を更新 */
	if(PadData & PADRleft)	PWorld.vy += 5*ONE/360;
	if(PadData & PADRright) PWorld.vy -= 5*ONE/360;
	if(PadData & PADRup)	PWorld.vx -= 5*ONE/360;
	if(PadData & PADRdown)	PWorld.vx += 5*ONE/360;
	
	if(PadData & PADR1) DWorld.coord.t[2] += 200;
	if(PadData & PADR2) DWorld.coord.t[2] -= 200;
	
	/* オブジェクトのパラメータからマトリックスを計算し座標系にセット */
	set_coordinate(&PWorld,&DWorld);
	
	/* 再計算のためのフラグをクリアする */
	DWorld.flg = 0;
	
	/* quit */
	if(PadData & PADselect) 
		return(-1);		
	return(0);
}

/* 初期化ルーチン群 */
static initSystem(void)
{
	int	i;

	PadData = 0;
	
	/* 環境の初期化 */
	GsInitGraph(320, 240, 0, 0, 0);
	GsDefDispBuff(0, 0, 0, 240);
	
	/* OTの初期化 */
	for (i = 0; i < 2; i++) {
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].point  = 0;
		WorldOT[i].offset = 0;
		WorldOT[i].org    = OTTags[i];
		WorldOT[i].tag    = OTTags[i] + OTSIZE - 1;
	}
	
	/* 3Dライブラリの初期化 */
	GsInit3D();
}

/* 視点位置の設定 */
static void initView(void)
{
	/* プロジェクション（視野角）の設定 */
	GsSetProjection(SCR_Z);

	/* 視点位置の設定 */
	view.vpx = 0; view.vpy = 0; view.vpz = -1000;
	view.vrx = 0; view.vry = 0; view.vrz = 0;
	view.rz = 0;
	view.super = WORLD;
	GsSetRefView2(&view);

	/* Zクリップ値を設定 */
	GsSetNearClip(100);
}

/* 光源の設定（照射方向＆色） */
static void initLight(void)
{
	/* 光源#0 (100,100,100)の方向へ照射 */
	pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
	pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
	GsSetFlatLight(0,&pslt[0]);
	
	/* 光源#1（使用せず） */
	pslt[1].vx = 100; pslt[1].vy= 100; pslt[1].vz= 100;
	pslt[1].r=0; pslt[1].g=0; pslt[1].b=0;
	GsSetFlatLight(1,&pslt[1]);
	
	/* 光源#1（使用せず） */
	pslt[2].vx = 100; pslt[2].vy= 100; pslt[2].vz= 100;
	pslt[2].r=0; pslt[2].g=0; pslt[2].b=0;
	GsSetFlatLight(2,&pslt[2]);
	
	/* アンビエント（周辺光）の設定 */
	GsSetAmbient(ONE/2,ONE/2,ONE/2);

	/* 光源モードの設定（通常光源/FOGなし） */
	GsSetLightMode(0);
}

/* メモリ上のTMDデータの読み込み&オブジェクト初期化
 *		(先頭の１個のみ使用）
 */
static void initModelingData(u_long *addr)
{
	u_long *tmdp;
	
	/* TMDデータの先頭アドレス */
	tmdp = addr;			
	
	/* ファイルヘッダをスキップ */
	tmdp++;				
	
	/* 実アドレスへマップ */
	GsMapModelingData(tmdp);	
	
	tmdp++;		/* フラグ読み飛ばし */
	tmdp++;		/* オブジェクト個数読み飛ばし */
	
	GsLinkObject4((u_long)tmdp,&object,0);
	
	/* マトリックス計算ワークのローテーションベクター初期化 */
        PWorld.vx=PWorld.vy=PWorld.vz=0;
	GsInitCoordinate2(WORLD, &DWorld);
	
	/* 3Dオブジェクト初期化 */
	object.coord2 =  &DWorld;
	object.coord2->coord.t[2] = 4000;
	object.tmd = tmdp;		
	object.attribute = 0;
}

/* （メモリ上の）テクスチャデータの読み込み */
static void initTexture(u_long *addr)
{
	RECT rect1;
	GsIMAGE tim1;

	/* TIMデータの情報を得る */	
	/* ファイルヘッダを飛ばして渡す */
	GsGetTimInfo(addr+1, &tim1);	

	/* ピクセルデータをVRAMへ送る */	
	rect1.x=tim1.px;
	rect1.y=tim1.py;
	rect1.w=tim1.pw;
	rect1.h=tim1.ph;
	LoadImage(&rect1,tim1.pixel);

	/* CLUTがある場合はVRAMへ送る */
	if((tim1.pmode>>3)&0x01) {
		rect1.x=tim1.cx;
		rect1.y=tim1.cy;
		rect1.w=tim1.cw;
		rect1.h=tim1.ch;
		LoadImage(&rect1,tim1.clut);
	}
}

/* ローテションベクタからマトリックスを作成し座標系にセットする */
static void set_coordinate(SVECTOR *pos, GsCOORDINATE2 *coor)
{
  MATRIX tmp1;
  SVECTOR v1;
  
  tmp1   = GsIDMATRIX;		/* 単位行列から出発する */
    
  /* 平行移動をセットする */
  tmp1.t[0] = coor->coord.t[0];
  tmp1.t[1] = coor->coord.t[1];
  tmp1.t[2] = coor->coord.t[2];
  v1 = *pos;
  
  /* マトリックスにローテーションベクタを作用させる */
  RotMatrix(&v1,&tmp1);
  
  /* 求めたマトリックスを座標系にセットする */
  coor->coord = tmp1;
  
  /* マトリックスキャッシュをフラッシュする */
  coor->flg = 0;
}

/* プリミティブの初期化 */
#include "balltex.h"
/* ボールの個数 */
#define NBALL	256		

/* ボールが散らばっている範囲 */
#define DIST	SCR_Z/4		

/* プリミティブバッファ */
POLY_FT4	ballprm[2][NBALL];	

/* ボールの位置 */
SVECTOR		ballpos[NBALL];		
	
static void initPrimitives(void)
{

	int		i, j;
	u_short		tpage, clut[32];
	POLY_FT4	*bp;	
		
	/* ボールのテクスチャページをロードする */
	tpage = LoadTPage(ball16x16, 0, 0, 640, 256, 16, 16);

	/* ボール用の CLUT（３２個）をロードする */
	for (i = 0; i < 32; i++)
		clut[i] = LoadClut(ballcolor[i], 256, 480+i);
	
	/* プリミティブ初期化 */
	for (i = 0; i < 2; i++)
		for (j = 0; j < NBALL; j++) {
			bp = &ballprm[i][j];
			SetPolyFT4(bp);
			SetShadeTex(bp, 1);
			bp->tpage = tpage;
			bp->clut = clut[j%32];
			setUV4(bp, 0, 0, 16, 0, 0, 16, 16, 16);
		}
	
	/* 位置を初期化 */
	for (i = 0; i < NBALL; i++) {
		ballpos[i].vx = (rand()%DIST)-DIST/2;
		ballpos[i].vy = (rand()%DIST)-DIST/2;
		ballpos[i].vz = (rand()%DIST)-DIST/2;
	}
}

/* プリミティブのＯＴへの登録 */
static void addPrimitives(u_long *ot)
{
	static int	id    = 0;		/* buffer ID */
	static VECTOR	trans = {0, 0, SCR_Z};	/* world-screen vector */
	static SVECTOR	angle = {0, 0, 0};	/* world-screen angle */
	static MATRIX	rottrans;		/* world-screen matrix */
	int		i, padd;
	long		dmy, flg, otz;
	POLY_FT4	*bp;
	SVECTOR		*sp;
	SVECTOR		dp;
	
	
	id = (id+1)&0x01;	/* ID を スワップ */
	
	/* カレントマトリクスを退避させる */
	PushMatrix();		
	
	/* パッドの内容からマトリクス rottrans をアップデート */
	padd = PadRead(1);

	if(padd & PADLup)	angle.vx -= 10;
	if(padd & PADLdown)	angle.vx += 10;
	if(padd & PADLright)	angle.vy -= 10;
	if(padd & PADLleft)	angle.vy += 10;
	if(padd & PADL1)	trans.vz += 50;
	if(padd & PADL2)	trans.vz -= 50;
	
	RotMatrix(&angle, &rottrans);		/* 回転 */
	TransMatrix(&rottrans, &trans);		/* 並行移動 */
	
	/* マトリクス rottrans をカレントマトリクスに設定 */
	SetTransMatrix(&rottrans);	
	SetRotMatrix(&rottrans);
	
	/* プリミティブをアップデート */
	bp = ballprm[id];
	sp = ballpos;
	for (i = 0; i < NBALL; i++, bp++, sp++) {
		otz = RotTransPers(sp, (long *)&dp, &dmy, &flg);
		if (otz > 0 && otz < OTSIZE) {
			setXY4(bp, dp.vx, dp.vy,    dp.vx+16, dp.vy,
			           dp.vx, dp.vy+16, dp.vx+16, dp.vy+16);

			AddPrim(ot+otz, bp);
		}
	}

	/* 退避させていたマトリクスを引き戻す。*/
	PopMatrix();
}
