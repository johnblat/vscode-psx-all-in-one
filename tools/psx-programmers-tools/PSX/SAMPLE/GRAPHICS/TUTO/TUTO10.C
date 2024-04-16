/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*		   tuto10: 3 dimentional cell type BG
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul,29,1994	suzu 
 *		2.00		May,22,1995	sachiko
 *		2.01		Mar, 9,1997	sachiko	(added autopad)
 */
/* ３Ｄセルタイプの BG の実現
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

/* BG の最大幅 */
#define BG_WIDTH	1024			
#define BG_HEIGHT	512			
	
/* セルの個数 */
#define BG_NX		(BG_WIDTH/BG_CELLX)	
#define BG_NY		(BG_HEIGHT/BG_CELLY)	

/* ＯＴの分解能は 4 */
#define OTSIZE		4			

/* スクリーンサイズ(640x240) */
#define SCR_W		640	
#define SCR_H		240

/* スクリーンの深さ */
#define SCR_Z		512			

/* BG 構造体を新しく定義する */
typedef struct {
	SVECTOR		*ang;		/* 回転角 */
	VECTOR		*vec;		/* 移動量 */
	SVECTOR		*ofs;		/* マップ上オフセット */
	POLY_GT4	cell[BG_NY*BG_NX];	/* BG セル配列 */
} BG;

/* パケットダブルバッファ */
typedef struct {
	DRAWENV		draw;		/* 描画環境 */
	DISPENV		disp;		/* 表示環境 */
	u_long		ot[OTSIZE];	/* ＯＴ */
	BG		bg0;		/* BG 0 */
} DB;

static void init_prim(DB *db,
		      int dr_x, int dr_y, int ds_x, int ds_y, int w, int h);
static void bg_init(BG *bg, SVECTOR *ang, VECTOR *vec, SVECTOR *ofs);
static void bg_update(u_long *ot, BG *bg);
static int pad_read(BG *bg);

void tuto10(void)
{
	/* ダブルバッファ */
	DB		db[2];		
	
	/* 現在のバッファアドレス */
	DB		*cdb;		


	/* GTE の初期化 */
	init_system(SCR_W/2, SCR_H/2, SCR_Z);
	
	/* パケットバッファの内容の初期値を設定 			
	 * ここで、BG 用のプリミティブリストのリンクまで張ってしまう
	 */
	init_prim(&db[0], 0,     0, 0, SCR_H, SCR_W, SCR_H);
	init_prim(&db[1], 0, SCR_H, 0,     0, SCR_W, SCR_H);

	/* 表示開始 */
	SetDispMask(1);

	/* メインループ */
	cdb = db;
	while (pad_read(&cdb->bg0) == 0) {

		/* パケットバッファの交換 */
		cdb = (cdb==db)? db+1: db;	
		
		/* OT のクリア */
		ClearOTag(cdb->ot, OTSIZE);	

		bg_update(cdb->ot, &cdb->bg0);

		DrawSync(0);	/* 描画の終了を待つ */
		VSync(0);	/* 垂直同期の発生を待つ */
		
		/* ダブルバッファ交換 */
		PutDrawEnv(&cdb->draw);
		PutDispEnv(&cdb->disp);
		DrawOTag(cdb->ot);	
	}
	DrawSync(0);
	return;
}

/* パケットダブルバッファの各メンバの初期化
 * メインループ内で変更されないものはすべてここであらかじめ設定しておく。
 */
/* DB	*db,		/* パケットバッファ  */
/* int	dr_x, dr_y	/* 描画環境の左上 ＸY */
/* int	ds_x, ds_y	/* 表示環境の左上 ＸY */
/* int	w,h		/* 描画・表示領域の幅と高さ */
static void init_prim(DB *db,
		      int dr_x, int dr_y, int ds_x, int ds_y, int w, int h)
{
	/* BG の位置バッファ 
	 * GTE では、角度を 360°= 4096 (ONE) とします。*/
	static SVECTOR	ang = {-ONE/5, 0,       0};
	static VECTOR	vec = {0,      SCR_H/2, SCR_Z/2};
	static SVECTOR	ofs = {0,      0,       0};

	/* ダブルバッファの設定 */
	SetDefDrawEnv(&db->draw, dr_x, dr_y, w, h);
	SetDefDispEnv(&db->disp, ds_x, ds_y, w, h);

	/* バックグラウンド自動クリアの設定 */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 0, 0, 0);

	/* BG の初期化 */
	bg_init(&db->bg0, &ang, &vec, &ofs);
}

/* BG ハンドリング
 *
 ***************************************************************************/
/* BG セル構造体 */
typedef struct {
	u_char	u, v;	/* セルテクスチャパターン座標 */
	u_short	clut;	/* セルテクスチャパターン CLUT */
	u_long	attr;	/* アトリビュート */
} CTYPE;

/* BG Mesh 構造体（内部のワーク用） */
/* セルタイプを記述した２次元配列 */
#include "bgmap.h"	
/* BG テクスチャ画像パターン・テクスチャ CLUT */
extern	u_long	bgtex[];

/* initialize BG */
/* BG		*bg,	/* BG データ */
/* int		x,y	/* スクリーン上の表示位置（Ｘ） */
/* VECTOR	*vec	/* 平行移動ベクトル */
/* SVECTOR	*ofs	/* オフセット */

static void bg_init(BG *bg, SVECTOR *ang, VECTOR *vec, SVECTOR *ofs)
{
	POLY_GT4	*cell;
	u_short		tpage, clut;
	int		i, x, y, ix, iy;
	u_char		col;

	/* 位置バッファを設定 */
	bg->ang = ang;
	bg->vec = vec;
	bg->ofs = ofs;

	/* テクスチャ・テクスチャ CLUT をロード */
	tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex, 0, 481);

	/* getnerate primitive list */
	for (cell = bg->cell, iy = 0; iy < BG_NY; iy++) {
		for (ix = 0; ix < BG_NX; ix++, cell++) {

			/* POLY_GT4 プリミティブ */
			SetPolyGT4(cell);	

			/* 奥を暗く、手前を明るくするために色を設定する */
			/* 奥側 */
			col = 224*iy/BG_NY+16;		
			setRGB0(cell, col, col, col);
			setRGB1(cell, col, col, col);

			/* 手前側 */
			col = 224*(iy+1)/BG_NY+16;	
			setRGB2(cell, col, col, col);
			setRGB3(cell, col, col, col);

			/* テクスチャ tpage を設定 */
			cell->tpage = tpage;	
			
			/* テクスチャ CLUT を設定 */
			cell->clut  = clut;	
		}
	}
	
}

/* BG のメンバを更新する */	
/* u_long	*ot,	/* オーダリングテーブル */
/* BG		*bg	/* BG バッファ */
static void bg_update(u_long *ot, BG *bg)
{
	static SVECTOR	Mesh[BG_NY+1][BG_NX+1];

	MATRIX		m;
	POLY_GT4	*cell;
	CTYPE		*ctype;		/* cell type */
	SVECTOR		mp;
	
	/* 現在位置（ワールド座標） */
	int		tx, ty;		
	
	/* 現在位置（マップの区画） */
	int		mx, my;		
	
	/* 現在位置（マップの区画内） */
	int		dx, dy;		
	
	int		ix, iy;		/* work */
	int		xx, yy;		/* work */
	long		dmy, flg;	/* work */

	/* 現在位置（ BG の左上） */
	
	/* ( BG_CELLX*BG_MAPX , BG_CELLY*BG_MAPY ) でラップラウンド */
	tx = (bg->ofs->vx)&(BG_CELLX*BG_MAPX-1);
	ty = (bg->ofs->vy)&(BG_CELLY*BG_MAPY-1);

	/* tx を BG_CELLX で割った値がマップの位置 (mx)*/
	/* tx を BG_CELLX で割った余りが表示移動量 (dx)*/
	mx =  tx/BG_CELLX;	my =  ty/BG_CELLY;
	dx = -(tx%BG_CELLX);	dy = -(ty%BG_CELLY);

	PushMatrix();

	/* マトリクスの計算 */
	RotMatrix(bg->ang, &m);		/* 回転角度 */
	TransMatrix(&m, bg->vec);	/* 平行移動ベクトル */
	
	/* マトリクスの設定 */
	SetRotMatrix(&m);		/* 回転角度 */
	SetTransMatrix(&m);		/* 平行移動ベクトル */

	mp.vy = -BG_HEIGHT + dy;
	mp.vz = 0;

	/* メッシュを生成 */
	for (iy = 0; iy < BG_NY+1; iy++, mp.vy += BG_CELLY) {
		mp.vx = -BG_WIDTH/2 + dx; 
		for (ix = 0; ix < BG_NX+1; ix++, mp.vx += BG_CELLX) 
			RotTransPers(&mp, (long *)&Mesh[iy][ix], &dmy, &flg);
	}

	/* プリミティブリストの (u0,v0), (x0,y0) メンバを変更 */
	for (cell = bg->cell, iy = 0; iy < BG_NY; iy++) {
		for (ix = 0; ix < BG_NX; ix++, cell++) {

			/* 表示領域内かどうかのチェック */
			if (Mesh[iy  ][ix+1].vx <     0) continue;
			if (Mesh[iy  ][ix  ].vx > SCR_W) continue;
			if (Mesh[iy+1][ix  ].vy <     0) continue;
			if (Mesh[iy  ][ix  ].vy > SCR_H) continue;

			/* (BG_MAPX, BG_MAPY) でラップラウンド */
			xx = (mx+ix)&(BG_MAPX-1);
			yy = (my+iy)&(BG_MAPY-1);

			/* マップからセルタイプを獲得 */

			/* Map[][] はキャラクタコードで ID が入っている */
			ctype = &CType[(Map[yy])[xx]-'0'];

			/* (u,v), (x, y) を更新 */
			setUVWH(cell, ctype->u, ctype->v,
				BG_CELLX-1, BG_CELLY-1);

			setXY4(cell,
			       Mesh[iy  ][ix  ].vx, Mesh[iy  ][ix  ].vy,
			       Mesh[iy  ][ix+1].vx, Mesh[iy  ][ix+1].vy,
			       Mesh[iy+1][ix  ].vx, Mesh[iy+1][ix  ].vy,
			       Mesh[iy+1][ix+1].vx, Mesh[iy+1][ix+1].vy);

			/* ＯＴに登録 */
			AddPrim(ot, cell);
		}
	}
	/* マトリクスの復帰 */
	PopMatrix();
}

/* コントロールパッドのデータを読む。 */
#define DT	8	/* speed */
static int pad_read(BG *bg)
{
	u_long	padd = PadRead(1);
	
	/* プログラムの終了 */
	if(padd & PADselect) 	return(-1);

	bg->ofs->vy -= 4;

	/* 平行移動 */
	if(padd & PADLup)	bg->ofs->vy -= 2;
	if(padd & PADLdown)	bg->ofs->vy += 2;
	if(padd & PADLleft)	bg->ofs->vx -= 2;
	if(padd & PADLright)	bg->ofs->vx += 2;

	/* 回転 */
	if (padd & PADRup)	bg->ang->vx += DT;
	if (padd & PADRdown)	bg->ang->vx -= DT;
	if (padd & PADRleft) 	bg->ang->vy += DT;
	if (padd & PADRright)	bg->ang->vy -= DT;

	return(0);
}

