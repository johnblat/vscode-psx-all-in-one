/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*				tuto1: OT
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/* ＯＴを使用した描画のテスト */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* コントローラレコーダ用マクロ */
#define PadRead(x)	myPadRead(x)

/* プリミティブダブルバッファ
 * 描画と CPU を並列で実行させるためには２組のバッファが必要
 */	
typedef struct {		
	DRAWENV		draw;		/* 描画環境 */
	DISPENV		disp;		/* 表示環境 */
	u_long		ot[9];		/* ＯＴ */
	SPRT_16		ball[8][8][8];	/* スプライト */
					/* 奥行き方向   : ８個 */
					/* 横方向       : ８個 */
					/* 縦方向       : ８個 */
} DB;

static void init_prim(DB *db);
static int pad_read(int *dx, int *dy);

void tuto1(void)
{
	DB	db[2];		/* プリミティブダブルバッファ */
	DB	*cdb;		/* 設定パケットバッファへポインタ */
	SPRT_16	*bp;		/* work */
	int	ox, oy;		/* work */
	int	dx = 0, dy = 0;	/* work */
	int	depth;		/* work */
	int	x, y;		/* work */
	int	ix, iy;		/* work */

	/* 描画・表示環境のリセット */
	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(16, 16, 256, 64, 0, 512));
	
	/* ダブルバッファの定義 */
	/*	buffer #0:	(0,  0)-(320,240) (320x240)
	 *	buffer #1:	(0,240)-(320,480) (320x240)
	 */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* それぞれのプリミテブバッファの内容の初期値を設定 */
	init_prim(&db[0]);	/* パケットバッファ # 0 */
	init_prim(&db[1]);	/* パケットバッファ # 1 */

	/* 表示開始 */
	SetDispMask(1);

	/* メインループ */
	while (pad_read(&dx, &dy) == 0) {

		/* ダブルバッファポインタの切り替え */
		cdb = (cdb==db)? db+1: db;

		/* ＯＴを初期化 (エントリ数＝８)*/
		ClearOTag(cdb->ot,8);

		/* スプライトの位置を計算 */
		for (depth = 0; depth < 8; depth++) {
			/* 奥のスプライトほど遅く、
			 * 近くのスプライトほど速く動くように設定
			 */
			/* 左上座標値 (Y) */
			oy =  56 + dy*(depth+1);	
			
			/* 左上座標値 (X) */
			ox =  96 + dx*(depth+1);	
 			for (iy = 0, y = oy; iy < 8; iy++, y += 16) 
			for (ix = 0, x = ox; ix < 8; ix++, x += 16) {

				bp = &cdb->ball[depth][iy][ix];

				/* スプライトの右上点を指定 */
				setXY0(bp, x, y);

				/* ot[depth+1] に登録 */
				AddPrim(&cdb->ot[depth], bp);
			}
		}
		/* バックグラウンドで走っている描画の終了を待つ */
		DrawSync(0);

		/* V-BLNK が来るのを待つ */
		VSync(0);

		/* フレームダブルバッファを交換する
		 * カレントパケットバッファの描画環境・表示環境を設定する。
		 */
		PutDrawEnv(&cdb->draw);
		PutDispEnv(&cdb->disp);

		/* ＯＴ上のプリミティブをバックグラウンドで描画する。*/
		DrawOTag(cdb->ot);
		FntPrint("tuto1: OT\n");
		FntFlush(-1);
	}
	DrawSync(0);
	return;
}

/* プリミティブダブルバッファの各メンバの初期化
 * メインループ内で変更されないものはすべてここであらかじめ設定しておく。
 */	
/* プリミティブバッファ */
static void init_prim(DB *db)
{
	/* 16x16 テクスチャパターン（ボール）*/
	extern u_long	ball16x16[];	
	
	/* ボール CLUT (16色x32) */
	extern u_long	ballclut[][8];	

	SPRT_16	*bp;
	u_short	clut;
	int	depth, x, y;

	/* 4bit モードのボールのテクスチャパターンをフレームバッファに
	 * 転送する。その時のテクスチャページをデフォルトテクスチャページに
	 * 設定する。	 
	 */	 
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);

	/* 自動背景クリアモードにする */
	db->draw.isbg = 1;
	/* 背景色の設定 */
	setRGB0(&db->draw, 60, 120, 120);	

	/* 8x8x8=256 個のスプライトをまとめて初期化する。
	 * CLUT は、奥行きに応じて切替える。	 
	 */	 
	for (depth = 0; depth < 8; depth++) {
		/* CLUT をロード CLUT ロードアドレスに注意 */
		clut = LoadClut(ballclut[depth], 0, 480+depth);

		/* プリミティブのメンバで変化しないものをここで設定する。*/
		for (y = 0; y < 8; y++) 
			for (x = 0; x < 8; x++) {
				bp = &db->ball[depth][y][x];
			
				/* 16x16のスプライト */
				SetSprt16(bp);		
			
				/* シェーディング禁止 */
				SetShadeTex(bp, 1);	
			
				/* (u0,v0) = (0, 0) */
				setUV0(bp, 0, 0);	
			
				/* テクスチャCLUT ID  */
				bp->clut = clut;	
			}
	}
}

/* コントローラの解析 */
/* int	*dx;	スプライトの座標値のキーワード(X) */
/* int	*dy;	スプライトの座標値のキーワード(Y) */

static int pad_read(int *dx, int *dy)
{
	u_long	padd;	

	/* コントローラからデータを読み込む */
	padd = PadRead(1);

	/* プログラムの終了 */
	if (padd & PADselect)	return(-1);

	/* スプライトの移動 */
	if (padd & PADLup)	(*dy)--;
	if (padd & PADLdown)	(*dy)++;
	if (padd & PADLleft)	(*dx)--;
	if (padd & PADLright)	(*dx)++;

	return(0);
}

