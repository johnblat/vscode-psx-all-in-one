/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/* 
 * 	todview: sample program to view TOD data
 * 
 *	"main.c" simple TOD viewing routine
 *
 * 		Version 2.10	Apr, 17, 1996
 * 		Version 2.20	Mar, 08, 1997
 * 
 * 		Copyright (C) 1994, 1995 by Sony Computer Entertainment
 * 		All rights Reserved
 */

/*****************/
/* Include files */
/*****************/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>	

#include "tod.h"		/* definition for TOD functions */
#include "te.h"			/* TMD ID list */

/**********/
/* For OT */
/**********/
#define OT_LENGTH	12		 /* OT解像度を12bit（大きさ） */
GsOT		WorldOT[2];		 /* OT情報（ダブルバッファ） */
GsOT_TAG	OTTags[2][1<<OT_LENGTH]; /* OTのタグ領域（ダブルバッファ） */

/***************/
/* For Objects */
/***************/
#define OBJMAX	50			/* 最大オブジェクト数 */
GsDOBJ2		obj_area[OBJMAX];	/* オブジェクト変数領域 */
GsCOORDINATE2	obj_coord[OBJMAX];	/* ローカル座標変数領域 */
GsCOORD2PARAM	obj_cparam[OBJMAX];	/* ローカル座標変数領域 */
TodOBJTABLE	objs;			/* オブジェクトテーブル */

/***************/
/* For Packets */
/***************/
#define PACKETMAX	6000		/* 1フレームの最大パケット数 */
#define PACKETMAX2	(PACKETMAX*24)	/* 1パケット平均サイズを24Byteとする */
PACKET	GpuPacketArea[2][PACKETMAX2];	/* パケット領域（ダブルバッファ） */

/***************/
/* For Viewing */
/***************/
#define VPX	-8000
#define VPY	-2000
#define VPZ	-5000
#define VRX	-900
#define VRY	-1500
#define VRZ	0
GsRVIEW2	view;

/************************/
/* For Lights ( three ) */
/************************/
GsF_LIGHT	pslt[3];

/*********************/
/* For Modeling data */
/*********************/
#define MODEL_ADDR	0x800c0000
u_long		*TmdP;

/**********************/
/* For Animation data */
/**********************/
#define TOD_ADDR	0x800e0000 /* addr for TOD data */
u_long	*TodP;		/* アニメーションデータポインタ配列 */
int	NumFrame;	/* アニメーションデータフレーム数配列 */
int 	StartFrameNo;	/* アニメーションスタートフレーム番号配列 */

/******************/
/* For Controller */
/******************/
u_long		padd;

/* メイン・ルーチン
 *
 */
main()
{
    /* イニシャライズ */
    ResetCallback();
    initSystem();
    initView();
    initLight();
    initModelingData( MODEL_ADDR );
    initTod();

    /* Execute first TOD frame */
    drawTod();

    while( 1 ) {

	/* Read the pad data */
	padd = PadRead( 1 );

	/* play TOD data according to the pad data */
	if(obj_interactive()) break;
    }

    PadStop();
    ResetGraph(3);
    StopCallback();
    return 0;
}

/* 3Dオブジェクト描画処理
 *
 */
drawObject()
{
    int		i;
    int		activeBuff;
    GsDOBJ2	*objp;
    MATRIX	LsMtx;	

    activeBuff = GsGetActiveBuff();

    /* 描画コマンドを格納するアドレスを設定する */
    GsSetWorkBase( (PACKET*)GpuPacketArea[activeBuff] );

    /* OT の初期化 */
    GsClearOt( 0, 0, &WorldOT[activeBuff] );

    /* オブジェクト配列へのポインタをセットする */
    objp = objs.top;

    for( i = 0; i < objs.nobj; i++ ) { 

	/* coord が書き換えられたかどうかのフラグ */
	objp->coord2->flg = 0;

	if ( ( objp->id  != TOD_OBJ_UNDEF ) && ( objp->tmd != 0 ) ) {

	    /* ローカルスクリーンマトリックスを計算 */
	    GsGetLs( objp->coord2, &LsMtx );

	    /* ローカルスクリーンマトリックスを GTE にセットする */
	    GsSetLsMatrix( &LsMtx );

	    /* ライトマトリックスを GTE にセットする */
	    GsSetLightMatrix( &LsMtx );

	    /* オブジェクトを透視変換しオーダリングテーブルに割り付ける */
	    GsSortObject4( objp, 		/* Pointer to the object */
			   &WorldOT[activeBuff],/* Pointer to the OT */
			   14-OT_LENGTH,	/* number of bits to be shifted*/
			   getScratchAddr(0));
	}

	objp++;
    }

    VSync( 0 );				/* Wait for V-BLNK */
    ResetGraph( 1 );			/* Cancel the current display */
    GsSwapDispBuff();			/* Switch the double buffer */

    /* 画面クリアコマンドを OT に登録 */
    GsSortClear( 0x0,				/* R of the background */
		 0x0,				/* G of the background */
		 0x0,				/* B of the background */
		 &WorldOT[activeBuff] );	/* Pointer to the OT */

    /* OT に割り付けられた描画コマンドの描画 */
    GsDrawOt( &WorldOT[activeBuff] );
}

/* 初期化ルーチン群
 *
 */
initSystem()
{
  /* コントロールパッドの初期化 */
  PadInit( 0 );
  padd = 0;
	
  /* GPUの初期化 */
  GsInitGraph( 640, 240, 0, 1, 0 );
  GsDefDispBuff( 0, 0, 0, 240 );
	
  /* OTの初期化 */
  WorldOT[0].length = OT_LENGTH;
  WorldOT[0].org = OTTags[0];
  WorldOT[0].offset = 0;
  WorldOT[1].length = OT_LENGTH;
  WorldOT[1].org = OTTags[1];
  WorldOT[1].offset = 0;
	
  /* 3Dライブラリの初期化 */
  GsInit3D();
}

/* 視点位置の設定
 *
 */
initView()
{
    /* プロジェクション（視野角）の設定 */
    GsSetProjection( 1000 );

    /* 視点位置の設定 */
    view.vpx = VPX; view.vpy = VPY; view.vpz = VPZ;
    view.vrx = VRX; view.vry = VRY; view.vrz = VRZ;
    view.rz = 0;
    view.super = WORLD;
    GsSetRefView2( &view );

    /* Zクリップ値を設定 */
    GsSetNearClip( 100 );
}

/* 光源の設定（照射方向＆色）
 *
 */
initLight()
{
    /* 光源#0 (100,100,100)の方向へ照射 */
    pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
    pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
    GsSetFlatLight( 0,&pslt[0] );
	
    /* 光源#1（使用せず） */
    pslt[1].vx = 100; pslt[1].vy= 100; pslt[1].vz= 100;
    pslt[1].r=0; pslt[1].g=0; pslt[1].b=0;
    GsSetFlatLight( 1,&pslt[1] );
	
    /* 光源#2（使用せず） */
    pslt[2].vx = 100; pslt[2].vy= 100; pslt[2].vz= 100;
    pslt[2].r=0; pslt[2].g=0; pslt[2].b=0;
    GsSetFlatLight( 2,&pslt[2] );
	
    /* アンビエント（周辺光）の設定 */
    GsSetAmbient( ONE/2,ONE/2,ONE/2 );

    /* 光源モードの設定（通常光源/FOGなし） */
    GsSetLightMode( 0 );
}

/* メモリ上のTMDデータの読み込み (先頭の１個のみ使用）
 *
 */
initModelingData( addr )
u_long	*addr;
{
    /* TMDデータの先頭アドレス */
    TmdP = addr;

    /* ファイルヘッダをスキップ */
    TmdP++;

    /* 実アドレスへマップ */
    GsMapModelingData( TmdP );

    /* オブジェクトテーブルの初期化 */
    TodInitObjTable( &objs, obj_area, obj_coord, obj_cparam, OBJMAX );
}


/* メモリ上のTODデータの読み込み */
initTod()
{
    TodP = ( u_long * )TOD_ADDR;
    TodP++;
    NumFrame = *TodP++;
    StartFrameNo = *( TodP + 1 );
}

/* パッドにより、TODデータを表示する */
obj_interactive()
{
    int		i;

    if ( ( ( padd & PADRright ) > 0 ) ||
	 ( ( padd & PADRleft )  > 0 ) || 
	 ( ( padd & PADRup )    > 0 ) ||
	 ( ( padd & PADRdown )  > 0 ) ) {

	drawTod();
    }

    /* 視点の変更 */
    if ( ( padd & PADn ) > 0 ) {
	view.vpz += 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADo ) > 0 ) {
	view.vpz -= 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADLleft ) > 0 ) {
	view.vpx -= 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADLright ) > 0 ) {
	view.vpx += 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADLdown ) > 0 ) {
	view.vpy += 100;
	GsSetRefView2( &view );
	drawObject();
    }
    if ( ( padd & PADLup ) > 0 ) {
	view.vpy -= 100;
	GsSetRefView2( &view );
	drawObject();
    }
    
    /* 視点を元に戻す */
    if ( ( padd & PADh ) > 0 ) {
	initView();
	drawObject();
    }

    /* プログラムを終了する */
    if ( ( padd & PADk ) > 0 ) {
	return -1;
    }

    return 0;
}


drawTod()
{
    int		i;
    int		j;
    u_long	*TodPtmp;
    int		frameNo;
    int		oldFrameNo;

    TodPtmp = TodP;
    TodPtmp = TodSetFrame( StartFrameNo, TodPtmp, &objs, te_list,
			   TmdP, TOD_CREATE );
    drawObject();

    oldFrameNo = StartFrameNo;

    for ( i = StartFrameNo + 1 ; i < NumFrame + StartFrameNo ; i++ ) {
	    frameNo = *( TodPtmp + 1 );

        for ( j = 0 ; j < frameNo - oldFrameNo - 1 ; j++ ) {
            drawObject();
        }

        TodPtmp = TodSetFrame( frameNo, TodPtmp, &objs, te_list,
			       TmdP, TOD_CREATE );
        drawObject();

	oldFrameNo = frameNo;
    }
    drawObject();
}

