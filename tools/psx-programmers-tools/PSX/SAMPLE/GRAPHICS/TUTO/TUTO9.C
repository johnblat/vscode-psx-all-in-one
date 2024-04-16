/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			tuto9: cell type BG
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul,29,1994	suzu 
 *		2.00		May,22,1995	sachiko
 *		2.01		Mar, 9,1997	sachiko	(added autopad)
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 * 			All rights Reserved
 */
/* セルタイプの BG の実現 
 *
 *  	このプログラムは、4bit/8bit テクスチャを使用した BG の速度評価
 *	プログラムです。
 *	8bit テクスチャの場合は 4bit テクスチャに比べて速度が低下します。
 *	この例では POLY_FT4 のアレイを使用して、4 枚のフルサイズ BG を描
 *	画しています。
 *
 *	BG_SPRT を define するとスプライトを使用した BG を描画します。
 *
 *	BG を回転・拡大したい場合は SPRT を使用する代わりに POLY_FT4
 *	を使用する必要があります。POLY_FT4 は SPRT よりも描画速度が低下し
 *	ます。従ってこの例の様に等倍のマッピングのみを使用する場合は SPRT
 *	の方が高速です。
 *
 *
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* コントローラレコーダ用マクロ */
#define PadRead(x)	myPadRead(x)

/* BG セルの単位は 32x32 */
#define BG_CELLX	32
#define BG_CELLY	32

#define BG_WIDTH	288	/* BG の最大幅 */
#define BG_HEIGHT	192	/* BG の最大高さ */

/* セルの個数（横方向）*/
#define BG_NX		(BG_WIDTH/BG_CELLX+2)
#define BG_NY		(BG_HEIGHT/BG_CELLY+2)

/* OT の分解能は 4 */
#define OTSIZE		4	

/* BG 構造体を新しく定義する */
typedef struct {
	/* 各 BG ごとの描画環境 */
	DRAWENV		env;	
	
	/* 各 BG ごとの OT */
	u_long		ot[OTSIZE];	
	
	SPRT		cell[BG_NY*BG_NX];	/* BG cells (SPRT) */
	POLY_FT4	cell2[BG_NY*BG_NX];	/* BG cells (POLY_FT4) */
} BG;

/* プリミティブダブルバッファ */
#define MAXBG	4			/* 最大 BG 面 */
typedef struct {
	DISPENV	disp;			/* 表示環境 */
	BG	bg[MAXBG];		/* BG */
} DB;

/* スプライト BGを使用する場合（ポリゴンのときは、コメントアウト） */
#define BG_SPRT

/* 4bit テクスチャを使用する場合（8bit のときはコメントアウト） */
#define BG_4bit

static void bg_init(BG *bg, int x, int y);
static void bg_update(BG *bg, SVECTOR *mapxy);
static void bg_draw(BG *bg);
static int pad_read(SVECTOR *mapxy);

void tuto9(void)
{
	/* パケットダブルバッファの実体 */
	DB	db[2];		
	
	/* 設定パケットバッファへポインタ */
	DB	*cdb;		
	
	/* 設定パケットＯＴ */
	u_long	*ot;		
	
	/* bgの現在のマップ位置（ｚは使用しない） */
	SVECTOR	mapxy[4];	
	
	int	i;

	/* フレームダブルバッファをクリア */
	{
		RECT rect;
		setRECT(&rect, 0, 0, 320, 480);
		ClearImage(&rect, 0, 0, 0);
	}

	/* ダブルバッファの定義 */
	/*	Buffer0   #0:	(  0   0) -(320,240)
	 *	Buffer1   #1:	(  0,240) -(320,480)
	 */
	SetDefDispEnv(&db[0].disp,  0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp,  0,   0, 320, 240);

	/* プリミティブバッファの内容の初期値を設定
	 * ここで、BG 用のプリミティブリストのリンクまで張ってしまう
	 */

	bg_init(&db[0].bg[0], 0, 4);		/* BG #0 */
	bg_init(&db[1].bg[0], 0, 4+240);

	bg_init(&db[0].bg[1], 20, 12);		/* BG #1 */
	bg_init(&db[1].bg[1], 20, 12+240);

	bg_init(&db[0].bg[2], 40, 20);		/* BG #2 */
	bg_init(&db[1].bg[2], 40, 20+240);

	bg_init(&db[0].bg[3], 60, 28);		/* BG #3 */
	bg_init(&db[1].bg[3], 60, 28+240);

	/* マップの初期値を設定 */
	setVector(&mapxy[0],  0,  0,0);	/* default position (  0,  0) */
	setVector(&mapxy[1],256,256,0);	/* default position (256,256) */
	setVector(&mapxy[2],  0,256,0);	/* default position (  0,256) */
	setVector(&mapxy[3],256,  0,0);	/* default position (256,  0) */

	/* 表示開始 */
	SetDispMask(1);

	/* メインループ */
	while (pad_read(mapxy) == 0) {		/* mapxy を獲得 */

		/* 設定プリミティブバッファポインタを獲得する。*/
		cdb = (cdb==db)? db+1: db;

		/* BG を更新 */
		for (i = 0; i < 4; i++)
			bg_update(&cdb->bg[i], &mapxy[i]);

		/* ダブルバッファをスワップ */
		DrawSync(0);
		VSync(0);
		PutDispEnv(&cdb->disp); 

		/* BG を４面描画する */
		for (i = 0; i < 4; i++)
			bg_draw(&cdb->bg[i]);
	}
	DrawSync(0);
	return;
}

/* BG ハンドリング
 *
 ***************************************************************************/
/* BG セル構造体
 * この例では CLUT は１本なので clut メンバは使用しない。
 * この例では 属性判定をしないので attr メンバは使用しない。
 * bgmap.h を参照 
 */
typedef struct {
	u_char	u, v;	/* セルテクスチャパターン座標 */
	u_short	clut;	/* セルテクスチャパターン CLUT */
	u_long	attr;	/* アトリビュート */
} CTYPE;

/* セルタイプを記述した２次元配列 */
#include "bgmap.h"		

#ifdef BG_4bit
extern	u_long	bgtex[];	/* BG texture\CLUT (4bit) */
#else
extern	u_long	bgtex8[];	/* BG texture\CLUT (8bit) */
#endif

/* BG の初期化:あらかじめできることはみんなここで設定してしまう。 */
/* BG	*bg,	/* BG データ */
/* int	x,y	/* スクリーン上の表示位置（Ｘ） */
static void bg_init(BG *bg, int x, int y)
{
	SPRT		*cell;
	POLY_FT4	*cell2;
	u_short		clut, tpage;
	int		ix, iy;

	/* 描画環境を設定 */
	SetDefDrawEnv(&bg->env, x, y, BG_WIDTH, BG_HEIGHT);

	/* テクスチャ・テクスチャ CLUT をロード */

#ifdef BG_4bit	/* 4bit mode */
	tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex, 0, 480);
#else		/* 8bit mode */
	tpage = LoadTPage(bgtex8+0x80, 1, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex8, 0, 480);
#endif
	/* デフォルトテクスチャページを設定 */
	bg->env.tpage = tpage;

	/* ローカルＯＴをクリア */
	ClearOTag(bg->ot, OTSIZE);

	/* BG を構成するプリミティブリストを作成する。*/

	for (cell = bg->cell, cell2 = bg->cell2, iy = 0; iy < BG_NY; iy++)
	    for (ix = 0; ix < BG_NX; ix++, cell++, cell2++) {

#ifdef BG_SPRT
		SetSprt(cell);			/* SPRT Primitive */
		SetShadeTex(cell, 1);		/* ShadeTex forbidden */
		setWH(cell, BG_CELLX, BG_CELLY);/* Set the sizes of the cell */
		cell->clut = clut;		/* Set texture CLUT  */
		AddPrim(&bg->ot[0], cell);	/* Put SPRT primitive into OT */
#else
		SetPolyFT4(cell2);		/* POLY_FT4 Primitive */
		SetShadeTex(cell2, 1);		/* ShadeTex forbidden */
		cell2->tpage = tpage;		/* Set texture pages*/
		cell2->clut  = clut;		/* Set texture CLUT*/
		AddPrim(&bg->ot[0], cell2);	/* Put POLY_FT4 primitives 
						   into OT  */
#endif
	    }
}

/* BG のメンバを更新する */
/* BG		*bg,	/* BG データ */
/* SVECTOR	*mapxy	/* テクスチャの BG へのマップ位置 */

static void bg_update(BG *bg, SVECTOR *mapxy)
{
	/* スプライト用セルデータ */
	SPRT		*cell;		
	
	/* ポリゴン用セルデータ */
	POLY_FT4	*cell2;		
	
	/* セルタイプ */
	CTYPE		*ctype;		
	
	/* マップのオフセット値 */
	int		mx, my;		
	
	/* マップの中でのオフセット値 */
	int		dx, dy;		
	
	int		tx, ty;		/* work */
	int		x, y, ix, iy;

	/* (tx, ty) 現在位置（BG_CELLX*BG_MAPX でラップラウンド）*/
	tx = (mapxy->vx)&(BG_CELLX*BG_MAPX-1);
	ty = (mapxy->vy)&(BG_CELLY*BG_MAPY-1);

	/* tx を BG_CELLX で割った値がマップの位置 (mx)  */
	/* tx を BG_CELLX で割った余りが表示移動量 (dx) */
	mx =  tx/BG_CELLX;	my =  ty/BG_CELLY;
	dx = -(tx%BG_CELLX);	dy = -(ty%BG_CELLY);

	/* プリミティブリストの (u0,v0), (x0,y0) メンバを変更 */

	cell  = bg->cell;
	cell2 = bg->cell2;
	for (iy = y = 0; iy < BG_NY; iy++, y += BG_CELLY) {
		for (ix = x = 0; ix < BG_NX; ix++, x += BG_CELLX) {

			/* (BG_MAPX, BG_MAPY) でラップラウンド */
			tx = (mx+ix)&(BG_MAPX-1);
			ty = (my+iy)&(BG_MAPY-1);

			/* マップからセルタイプを獲得 
			 * Map[][] はキャラクタコードで ID が入っている 
			 */ 
			ctype = &CType[(Map[ty])[tx]-'0'];
#ifdef BG_SPRT
			/* (u0,v0), (x0, y0) のみを更新 */
			setUV0(cell, ctype->u, ctype->v);
			setXY0(cell, x+dx, y+dy);
#else
			/* (u,v), (x, y) を更新 (POLY_FT4) */
			setUVWH(cell2, ctype->u, ctype->v,
				BG_CELLX-1, BG_CELLY-1);
			setXYWH(cell2, x+dx, y+dy, BG_CELLX, BG_CELLY);
#endif
			cell++;
			cell2++;
		}
	}
}

/* BG を描画 */
static void bg_draw(BG *bg)
{
	/* BG ごとに設定した描画環境に更新 */
	PutDrawEnv(&bg->env);	
	
	DrawOTag(bg->ot);	/* 描画 */
}

/* コントローラのデータを読んで、 BG の移動方向を決める。 */
/* SVECTOR *mapxy	/* テクスチャの BG へのマップ位置 */
static int pad_read(SVECTOR	*mapxy)
{
	static SVECTOR	v[4] = {
		 0, 0, 0, 0,	0,  0, 0, 0,
		 0, 0, 0, 0,	0,  0, 0, 0,
	};
	
	int	i,id = 3;
	u_long	padd = PadRead(1);

	/* 終了 */
	if(padd & PADselect) 	return(-1);

	/* window ID の選択 */
	if (padd&PADR1) id = 3;
	if (padd&PADR2) id = 2;
	if (padd&PADL1) id = 1;
	if (padd&PADL2) id = 0;

	/* BG の移動 */
	if(padd & PADLup)	setVector(&v[id],  0, -2, 0);
	if(padd & PADLdown)	setVector(&v[id],  0,  2, 0);
	if(padd & PADLleft)	setVector(&v[id], -2,  0, 0);
	if(padd & PADLright)	setVector(&v[id],  2,  0, 0);

	/* 今回の移動量を加算する */
	for (i = 0; i < 4; i++)
		addVector(&mapxy[i], &v[i]);

	return(0);
}


