/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*		 tuto11: special effect (mosaic)
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/* モザイク */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* コントローラレコーダ用マクロ */
#define PadRead(x)	myPadRead(x)

#define NCELL_X	16		/* セル数 */
#define	NCELL_Y	16

typedef struct {
	DRAWENV		draw;		/* 描画環境 */
	DISPENV		disp;		/* 表示環境 */
	u_long		ot;		/* オーダリングテーブル */
	POLY_FT4	bg[NCELL_X*NCELL_Y];

} DB;

static void mozaic(u_long *ot, POLY_FT4 *ft4, int ox, int oy, int dx, 
		int dy, int ou, int ov, int du, int dv,
		int divx, int divy, int rate);
void tuto11(void)
{
	/* BG テクスチャパターン(bgtex.c) */
	extern	u_long	bgtex[];	
	
	DB		db[2];		/* primitive double buffer */
	DB		*cdb;		/* current buffer */
	SVECTOR		x0;		/* positoin */
	u_short		tpage;		/* texture page */
	u_short		clut;		/* texture CLUT */
	int		id, i, j;	/* work */
	u_long		padd;
	
	/* モザイクのレート（０～７） */
	int		mrate = 0;	

	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(16, 16, 256, 64, 0, 512));
	
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* テクスチャページをロードする。(bgtex.c) */
	tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256,256);
	clut  = LoadClut(bgtex, 0,500);

	for (i = 0; i < 2; i++) {
		/* 各バッファの背景色をきめる。*/
		db[i].draw.isbg = 1;
		setRGB0(&db[i].draw, 0, 0, 0);	

		/* ポリゴンプリミティブを初期化 */
		for (j = 0; j < NCELL_X*NCELL_Y; j++) {
			SetPolyFT4(&db[i].bg[j]);
			SetShadeTex(&db[i].bg[j], 1);
			db[i].bg[j].tpage = tpage;
			db[i].bg[j].clut  = clut;
		}
	}

	/* スプライトの表示位置の初期値を決定 */
	setVector(&x0, 0,   0, 0);

	/* メインループ */
	SetDispMask(1);
	while (1) {

		FntPrint("tuto11: mosaic\n");
		
		/* バッファ初期化 */
		cdb = (cdb==db)? db+1: db;
		ClearOTag(&cdb->ot, 1);

		/* コントロールパッドを解析 */
		padd = PadRead(1);

		if (padd & PADselect) 	break;
		if (padd & PADRup)	mrate++;
		if (padd & PADRdown)	mrate--;
		if (padd & PADLright)	x0.vx++;
		if (padd & PADLleft)	x0.vx--;
		if (padd & PADLup)	x0.vy--;
		if (padd & PADLdown)	x0.vy++;

		/* モザイク */
		limitRange(mrate, 0, 8);
		mozaic(&cdb->ot, cdb->bg,
				x0.vx, x0.vy, 256, 256, 0, 0, 256, 256,
				NCELL_X, NCELL_Y, mrate);

		/* 描画 */
		DrawSync(0);	
		VSync(0);	

		PutDrawEnv(&cdb->draw); 
		PutDispEnv(&cdb->disp); 
		DrawOTag(&cdb->ot);	
		FntFlush(-1);
	}
	DrawSync(0);
	return;
}

/* モザイクの例 */
/* u_long *ot,			
/* POLY_FT4 *ft4,	/* プリミティブ */
/* int ox, oy,		/* プリミティブの左上座標値 */
/* int dx, dy		/* プリミティブサイズ */
/* int ou, ov		/* テクスチャの左上座標値 */
/* int du, dv,	 	/* テクスチャサイズ */
/* int divx, divy,	/* 分割数 */
/* int rate		/* モザイクレート */
static void mozaic(u_long *ot, POLY_FT4 *ft4, int ox, int oy, int dx, 
		int dy, int ou, int ov, int du, int dv,
		int divx, int divy, int rate)
{
	
	/* モザイクをかける単位 */
	int	sx = dx/divx, sy = dy/divx;	
	int	su = du/divy, sv = du/divy; 	
	
	/* ループカウンター */
	int	ix, iy;				
	
	int	x, y, u, v;			/* work */
	int	u0, v0, u1, v1;			/* work */

	/* (u, v) の値を変更してモザイクをかける */
	for (v = ov, y = oy, iy = 0; iy < divy; iy++, v += sv, y += sy) {
		for(u = ou, x = ox, ix = 0; ix < divx; ix++, u += su, x += sx){

			/* (u, v) 値の計算 
			 * モザイクの基本はこの２行
			 */
			u0 = u + rate;		v0 = v + rate;
			u1 = u+su - rate;	v1 = v+sv - rate;

			/* (u, v) 値のオーバーフローチェック */
			if (u1 >= 256)	u1 = 255;
			if (v1 >= 256)	v1 = 255;

			/* プリミティブに値をセットする */
			setUV4(ft4, u0, v0, u1, v0, u0, v1, u1, v1);
			setXYWH(ft4, x, y, sx, sy);

			AddPrim(ot, ft4);	/* OT に登録 */
			ft4++;			
		}
	}
}

