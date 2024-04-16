/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/* 
 * 	janken: sample program using TOD data
 * 
 *	"main.c" main routine
 *
 * 		Version 2.00	Feb, 3, 1995
 * 		Version 2.01	Mar, 8, 1997
 * 
 * 		Copyright (C) 1994, 1995 by Sony Computer Entertainment
 * 		All rights Reserved
 */

/*****************/
/* Include files */
/*****************/
#include <sys/types.h>
#include <libetc.h>		/* PADを使うためにインクルードする必要あり */
#include <libgte.h>		/* LIGSを使うためにインクルードする必要あり */
#include <libgpu.h>		/* LIGSを使うためにインクルードする必要あり */
#include <libgs.h>		/* グラフィックライブラリ を使うための構造体などが定義されている */
#include "tod.h"		/* TOD処理関数の宣言部 */
#include "te.h"			/* TMD-ID リスト (rsdlinkで作成) */

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
GsRVIEW2	view;			/* 視点情報 */

/************************/
/* For Lights ( three ) */
/************************/
GsF_LIGHT	pslt[3];		/* 光源×３個 */

/*********************/
/* For Modeling data */
/*********************/
#define MODEL_ADDR	0x800c0000
u_long		*TmdP;			/* モデリングデータポインタ */

/**********************/
/* For Animation data */
/**********************/
#define TOD_ADDR0	0x800e0000
#define TOD_ADDR1	0x80100000
#define TOD_ADDR2	0x80120000
#define TOD_ADDR3	0x80140000
#define TOD_ADDR4	0x80160000
#define TOD_ADDR5	0x80180000
#define TOD_ADDR6	0x801a0000
#define TOD_ADDR7	0x801c0000
#define TOD_ADDR8	0x801e0000
#define NUMTOD		9
u_long		*TodP[NUMTOD];		/* アニメーションデータポインタ配列 */
int		NumFrame[NUMTOD];	/* アニメーションデータフレーム数配列 */
int		StartFrameNo[NUMTOD];	/* アニメーションスタートフレーム番号配列 */

/******************/
/* For Controller */
/******************/
u_long		padd;

/*******************/
/* For Janken type */
/*******************/
int		pose;

/*************************/
/* Prototype Definitions */
/*************************/
int	exitProgram();
void	drawObject();
void	initSystem();
void	initView();
void	initLight();
void	initModelingData();
void	initTod();
void	initPose();
int	obj_interactive();

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
    initModelingData();
    initTod();
    initPose();
    
    FntLoad(960, 256);
    SetDumpFnt(FntOpen(32-320, 32-120, 256, 200, 0, 512));

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
void
drawObject()
{
    int		i;
    int		activeBuff;
    GsDOBJ2	*objp;
    MATRIX	LsMtx;
    int         load_count1,load_count2,load_count3,load_org;

    activeBuff = GsGetActiveBuff();

    /* 描画コマンドを格納するアドレスを設定する */
    GsSetWorkBase( (PACKET*)GpuPacketArea[activeBuff] );

    /* OT の初期化 */
    GsClearOt( 0, 0, &WorldOT[activeBuff] );

    /* オブジェクト配列へのポインタをセットする */
    objp = objs.top;

    load_count1=load_count2=load_count3=0;
    for( i = 0; i < objs.nobj; i++ ) { 

	/* coord が書き換えられたかどうかのフラグ */
	objp->coord2->flg = 0;

	if ( ( objp->id != TOD_OBJ_UNDEF ) && ( objp->tmd != 0 ) ) {

	    /* ローカルスクリーンマトリックスを計算 */
	    load_org = VSync(1);
	    GsGetLs( objp->coord2, &LsMtx );
	    load_count1+= VSync(1)-load_org;
	    /* ローカルスクリーンマトリックスを GTE にセットする */
	    GsSetLsMatrix( &LsMtx );
	    
	    /* ライトマトリックスを GTE にセットする */
	    GsSetLightMatrix( &LsMtx );
	    
	    /* オブジェクトを透視変換しオーダリングテーブルに割り付ける */
	    load_org = VSync(1);
	    GsSortObject4( objp, 		/* Pointer to the object */
			   &WorldOT[activeBuff],/* Pointer to the OT */
			   14-OT_LENGTH,getScratchAddr(0) );	/* number of bits to be shifted*/
	    load_count2+= VSync(1)-load_org;
	}

	objp++;
    }
    FntPrint("%d %d\n",load_count1,load_count2);

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
    FntFlush(-1);
}

/* 初期化ルーチン群
 *
 */
void
initSystem()
{
  /* コントロールパッドの初期化 */
  PadInit( 0 );
  padd = 0;
	
  /* GPUの初期化 */
  GsInitGraph( 640, 240, GsOFSGPU|GsNONINTER, 1, 0 );
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
void
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
void
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
void
initModelingData()
{
    /* TMDデータの先頭アドレス */
    TmdP = ( u_long * )MODEL_ADDR;

    /* ファイルヘッダをスキップ */
    TmdP++;

    /* 実アドレスへマップ */
    GsMapModelingData( TmdP );

    /* オブジェクトテーブルの初期化 */
    TodInitObjTable( &objs, obj_area, obj_coord, obj_cparam, OBJMAX );
}


/* メモリ上のTODデータの読み込み
 *
 */
void
initTod()
{
	TodP[0] = ( u_long * )TOD_ADDR0;
	TodP[0]++;
	NumFrame[0] = *TodP[0]++;
	StartFrameNo[0] = *( TodP[0] + 1 );

	TodP[1] = ( u_long * )TOD_ADDR1;
	TodP[1]++;
	NumFrame[1] = *TodP[1]++;
	StartFrameNo[1] = *( TodP[1] + 1 );

	TodP[2] = ( u_long * )TOD_ADDR2;
	TodP[2]++;
	NumFrame[2] = *TodP[2]++;
	StartFrameNo[2] = *( TodP[2] + 1 );

	TodP[3] = ( u_long * )TOD_ADDR3;
	TodP[3]++;
	NumFrame[3] = *TodP[3]++;
	StartFrameNo[3] = *( TodP[3] + 1 );

	TodP[4] = ( u_long * )TOD_ADDR4;
	TodP[4]++;
	NumFrame[4] = *TodP[4]++;
	StartFrameNo[4] = *( TodP[4] + 1 );

	TodP[5] = ( u_long * )TOD_ADDR5;
	TodP[5]++;
	NumFrame[5] = *TodP[5]++;
	StartFrameNo[5] = *( TodP[5] + 1 );

	TodP[6] = ( u_long * )TOD_ADDR6;
	TodP[6]++;
	NumFrame[6] = *TodP[6]++;
	StartFrameNo[6] = *( TodP[6] + 1 );

	TodP[7] = ( u_long * )TOD_ADDR7;
	TodP[7]++;
	NumFrame[7] = *TodP[7]++;
	StartFrameNo[7] = *( TodP[7] + 1 );

	TodP[8] = ( u_long * )TOD_ADDR8;
	TodP[8]++;
	NumFrame[8] = *TodP[8]++;
	StartFrameNo[8] = *( TodP[8] + 1 );
}

/* 登場から初期ポーズをとるまで
 *
 */
void
initPose()
{
    int		i;
    int		j;
    int		frameNo;
    int		oldFrameNo;

    /* 最初のフレームの TODデータの処理 */
    TodP[0] = TodSetFrame( StartFrameNo[0], TodP[0], &objs, te_list, 
			   TmdP, TOD_CREATE );
    drawObject();
    oldFrameNo = StartFrameNo[0];

    /* それ以降のフレームの TODデータの処理 */
    for ( i = 1 ; i < NumFrame[0] ; i++ ) {

	frameNo = *( TodP[0] + 1 );

	for ( j = 0 ; j < frameNo - oldFrameNo - 1 ; j++ ) {
	    drawObject();
	}

	/* 1フレーム分の TODデータの処理 */
	TodP[0] = TodSetFrame( frameNo, TodP[0], &objs, te_list, 
			       TmdP, TOD_CREATE );
	drawObject();
	oldFrameNo = frameNo;
    }
    drawObject();

    pose = -1;
}

/* パッドにより、TODデータを表示する
 *
 */
int obj_interactive()
{
    int		i;
    u_long	*TodPtmp;		/* pointer to TOD data  */
    int		step;
    
    /* 「ぐー」( ) */
    if ( ( padd & PADRright ) > 0 ) {

	/* 現在のポーズから振り始めの定位置に戻る */
	if ( pose > 0 ) {
	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* 「じゃ～んけ～ん」と握り拳を振る */
	TodPtmp = TodP[1];
	for ( i = StartFrameNo[1] ; i < NumFrame[1] + StartFrameNo[1] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* 定位置から「ぐー」を出す */
	TodPtmp = TodP[2];
	for ( i = StartFrameNo[2] ; i < NumFrame[2] + StartFrameNo[2] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	pose = 2;
    }

    /* 「ぱー」 */
    if ( ( padd & PADRleft ) > 0 ) {

	/* 現在のポーズから振り始めの定位置に戻る */
	if ( pose > 0 ) {
	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* 「じゃ～んけ～ん」と握り拳を振る */
	TodPtmp = TodP[1];
	for ( i = StartFrameNo[1] ; i < NumFrame[1] + StartFrameNo[1] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* 定位置から「ぱー」を出す */
	TodPtmp = TodP[4];
	for ( i = StartFrameNo[4] ; i < NumFrame[4] + StartFrameNo[4] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	pose = 4;
    }

    /* 「ちょき」 */
    if ( ( padd & PADRup ) > 0 ) {

	/* 現在のポーズから振り始めの定位置に戻る */
	if ( pose > 0 ) {
	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* 「じゃ～んけ～ん」と握り拳を振る */
	TodPtmp = TodP[1];
	for ( i = StartFrameNo[1] ; i < NumFrame[1] + StartFrameNo[1] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* 定位置から「ちょき」を出す */
	TodPtmp = TodP[3];
	for ( i = StartFrameNo[3] ; i < NumFrame[3] + StartFrameNo[3] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	pose = 3;
    }

    /* 「ぐー」「ちょき」「ぱー」のいずれかをランダムに出す */
    if ( ( padd & PADRdown ) > 0 ) {

	/* 現在のポーズから振り始めの定位置に戻る */
	if ( pose > 0 ) {
	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* 「じゃ～んけ～ん」と握り拳を振る */
	TodPtmp = TodP[1];
	for ( i = StartFrameNo[1] ; i < NumFrame[1] + StartFrameNo[1] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* 定位置から「ぐー」「ちょき」「ぱー」のいずれかをランダムに出す */
	pose = rand()%3 + 2;
	TodPtmp = TodP[pose];
	for ( i = StartFrameNo[pose] 
	      ; i < NumFrame[pose] + StartFrameNo[pose] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();
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

	/* 初期ポーズに戻す */
	if ( pose > 0 ) {

	    TodPtmp = TodP[pose+3];
	    for ( i = StartFrameNo[pose+3] 
		  ; i < NumFrame[pose+3] + StartFrameNo[pose+3] 
		  ; i++ ) {

		TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				       TmdP, TOD_CREATE );
		drawObject();
	    }
	    drawObject();
	}

	/* 視点を元の位置に戻す */
	step = ( ( VPX - view.vpx ) * ( VPX - view.vpx )
		 + ( VPY - view.vpy ) * ( VPY - view.vpy )
		 + ( VPZ - view.vpz ) * ( VPZ - view.vpz ) ) / 500000;
	if ( step > 50 ) {
	    step = 50;
	}
	for ( i = 1 ; i <= step ; i++ ) {
	    view.vpx = view.vpx + ( i * ( VPX - view.vpx ) ) / step;
	    view.vpy = view.vpy + ( i * ( VPY - view.vpy ) ) / step;
	    view.vpz = view.vpz + ( i * ( VPZ - view.vpz ) ) / step;
	    GsSetRefView2( &view );
	    drawObject();
	}

	/* 退場 */
	TodPtmp = TodP[8];
	for ( i = StartFrameNo[8] ; i < NumFrame[8] + StartFrameNo[8] 
	      ; i++ ) {

	    TodPtmp = TodSetFrame( i, TodPtmp, &objs, te_list, 
				   TmdP, TOD_CREATE );
	    drawObject();
	}
	drawObject();

	/* プログラムの終了*/
	return -1;
    }

    return 0;
}
