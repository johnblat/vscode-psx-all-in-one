/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/* 			tuto8: 1D scroll
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/* 1D スクロール BG の実験 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* コントローラレコーダ用マクロ */
#define PadRead(x)	myPadRead(x)

/* オーダリングテーブルの深さは、８ */
#define OTSIZE		8

/* 通常のスプライトは、テクスチャページを持たないので、ここで、
 * 新しくテクスチャページ付きスプライトを定義する。
 */
typedef struct {
	DR_MODE	mode;	/* モード設定プリミティブ */
	SPRT	sprt;	/* スプライト プリミティブ */
} TSPRT;

/* テクスチャ関係の情報を集めた構造体 */

typedef struct {
	/* テクスチャパターンがあるアドレス */
	u_long	*addr;	
	
	/* テクスチャパターンをロードする場所*/
	short	tx, ty;	
	
	/* テクスチャ CLUT をロードする場所 */
	short	cx, cy;	
	
	/* テクスチャページ ID （後から計算する）*/
	u_short	tpage;	
	
	/* テクスチャ CLUT ID （後から計算する）*/
	u_short	clut;	
} TEX;

/* プリミティブダブルバッファ */
typedef struct {
	DRAWENV		draw;		/* 描画環境 */
	DISPENV		disp;		/* 表示環境 */
	u_long		ot[OTSIZE];	/* オーダリングテーブル */
	TSPRT		far0, far1;	/* 遠くの山々 */
	TSPRT		near0, near1;	/* 近くの山々 */
	TSPRT		window;		/* 電車の窓 */
} DB;

static int update(DB *db,int padd);
static void add_sprite(u_long *ot,TSPRT *tsprt,
			int x0,int y0,int u0,int v0,int w,int h,TEX *tex);
static int SetTSprt(TSPRT *tsprt,int dfe,int dtd,int tpage,RECT *tw);

void tuto8(void)
{
	/* パケットダブルバッファ */
	DB		db[2];		
							  
	/* 現在のバッファアドレス */
	DB		*cdb;		
	
	/* コントロールパッドのデータ */
	int	padd;		

	/* テクスチャデータをフレームバッファに転送 */
	load_tex();		

	/* ダブルバッファ用の環境設定 */
	/*	buffer #0:	(0,  0)-(255,239) (256x240)
	 *	buffer #1:	(0,240)-(255,479) (256x240)
	*/
	SetDefDrawEnv(&db[0].draw, 0,   0, 256, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 256, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 256, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 256, 240);
	db[0].draw.isbg = db[1].draw.isbg = 1;

	SetDispMask(1);		/* 表示開始 */

	while (((padd = PadRead(1)) & PADselect) == 0) {

		/* db[0], db[1] を交換 */
		cdb = (cdb==db)? db+1: db;	

		/* 座標をアップデート */
		update(cdb, padd);		

		DrawSync(0);	/* 描画の終了を待つ */
		VSync(0);	/* 垂直同期を待つ */

		/* ダブルバッファの切替え */
		PutDrawEnv(&cdb->draw);	/* 描画環境の更新 */
		PutDispEnv(&cdb->disp);	/* 表示環境の更新 */
		DrawOTag(cdb->ot);	/* 描画 */
	}

	DrawSync(0);
	return;
}

/* テクスチャページは５枚使用（8bit モード)
 * 遠景、近景とも、256x256 を２枚横にならべてその一部を表示する。
 * 景色はラップラウンドする。
 * 各ページの内容は以下の通り
 */
extern u_long far0[];		/* 遠くの山々(左側)*/
extern u_long far1[];		/* 遠くの山々(右側)*/
extern u_long near0[];		/* 近くの山々(左側)*/
extern u_long near1[];		/* 近くの山々(右側)*/
extern u_long window[];		/* 汽車の窓 */

/* 各テクスチャに通し番号を定義する。*/
#define TEX_FAR0	0
#define TEX_FAR1	1
#define TEX_NEAR0	2
#define TEX_NEAR1	3
#define TEX_WINDOW	4

/* テクスチャ構造体の初期値 */
static TEX tex[] = {
	/*addr   tx   ty cx   cy tpage clut			*/
	/*--------------------------------------------------------------*/ 
	far0,   512,   0, 0, 481,    0,   0,	/* far0    */ 
	far1,   512, 256, 0, 482,    0,   0,	/* far1    */ 
	near0,  640,   0, 0, 483,    0,   0,	/* near0   */ 
	near1,  640, 256, 0, 484,    0,   0,	/* near1   */ 
	window, 768,   0, 0, 485,    0,   0,	/* window  */ 
	0,					/* 終端 */
};

/* テクスチャページをまとめてフレームバッファへ転送する。*/
load_tex(void)
{
	int	i;

	/* 'addr' が 0 でない限りループ */
	for (i = 0; tex[i].addr; i++) {	

		/* テクスチャパターンをロード */
		tex[i].tpage = LoadTPage(tex[i].addr+0x80, 1, 0,
					 tex[i].tx, tex[i].ty,  256, 256);
		
		/* テクスチャ CLUT をロードする */
		tex[i].clut = LoadClut(tex[i].addr, tex[i].cx, tex[i].cy);
	}
}

/* コントロールパッドを読んでスクロールのパラメータを決定する。*/

/* DB	*db,	/* プリミティブバッファ */
/* int	padd	/* コントローラのデータ */
static int update(DB *db, int padd)
{
	
	static int 	ofs_x = 0;	/* 風景の移動量 */
	static int	v     = 0;	/* 移動速度 */
	
	/* テクスチャデータ */
	TEX	*tp[2];			
	
	/* 移動方向 0:右→左, 1:左→右 */
	int	side;			
	
	/* 近景表示 1:する, else:しない */
	int	isnear   = 1;		
	
	/* 窓の表示 1:する, else:しない */
	int	iswindow = 1;		
	
	int	d;

	/* コントローラの解析 */
	if (padd & PADLright)	v++;	/* スピードアップ */
	if (padd & PADLleft)	v--;	/* スピードダウン */
	ofs_x += v;			/* 移動量の累積をとる */

	/* 場合によっては、近景、汽車の窓の表示を禁止する。*/
	if (padd & (PADR1|PADR2))	isnear   = 0;
	if (padd & (PADL1|PADL2))	iswindow = 0;

	/* ＯＴのクリア */
	ClearOTag(db->ot, OTSIZE);

	/* 遠くの山々を表示する。景色は横 512 でラップラウンド */
    display_far:

	side = (ofs_x/4)&0x100;	/* 表示向きを決定 */
	d    = (ofs_x/4)&0x0ff;	/* 移動量を 256 で正規化 */

	/* TSPRT を ＯＴに登録する関数をコールする 
	 * x0, y0 ではなく、u0, v0 を動かしてスクロールを行なう
	 * 右の一枚は、区間 (256-d,0)-(256,240) を担当する
	 * 左の一枚は、区間 (0, 0)-(256-d,240) を担当する。
	 */
	tp[0] = &tex[side==0? TEX_FAR0: TEX_FAR1];	/* right hand */
	tp[1] = &tex[side==0? TEX_FAR1: TEX_FAR0];	/* left hand */

	add_sprite(db->ot, &db->far0, 0,     16, d, 0, 256-d, 168, tp[0]);
	add_sprite(db->ot, &db->far1, 256-d, 16, 0, 0, d,     168, tp[1]);

	/* 近くの山々を表示する。景色は横 512 でラップラウンド */
    display_near:
	if (isnear == 0) goto display_window;

	side = (ofs_x/2)&0x100;	
	d    = (ofs_x/2)&0x0ff;

	tp[0] = &tex[side==0? TEX_NEAR0: TEX_NEAR1];
	tp[1] = &tex[side==0? TEX_NEAR1: TEX_NEAR0];

	add_sprite(db->ot+1, &db->near0, 0,     32, d, 0, 256-d, 168, tp[0]);
	add_sprite(db->ot+1, &db->near1, 256-d, 32, 0, 0, d,     168, tp[1]);

	/* 窓を描く */
    display_window:
	if (iswindow == 0) return;

	add_sprite(db->ot+2,
		   &db->window, 0, 0, 0, 0, 256, 200, &tex[TEX_WINDOW]);
}

/* TSPRT をＯＴに登録する */
/* u_long *ot,	  /* OT */
/* TSPRT  *tsprt, /* 登録するテクスチャスプライトプリミティブ */
/* int	  x0,y0	  /* スプライトの左上座標値 */
/* int	  u0,v0	  /* テクスチャの左上座標値（ｕ）*/
/* int	  w,h	  /* テクスチャのサイズ*/
/* TEX	  *tex	  /* テクスチャデータ */

static void add_sprite(u_long *ot,TSPRT *tsprt,
			int x0,int y0,int u0,int v0,int w,int h,TEX *tex)
{
	/* TSPRT プリミティブを初期化 */
	SetTSprt(tsprt, 1, 1, tex->tpage, 0);

	/* シェーディングオフ */
	SetShadeTex(&tsprt->sprt, 1);	
	
	/* スプライトの左上座標値 */
	setXY0(&tsprt->sprt, x0, y0);	
	
	/* テクスチャの左上座標値 */
	setUV0(&tsprt->sprt, u0, v0);	
	
	/* スプライトのサイズ */
	setWH(&tsprt->sprt, w, h);	
	
	/* テクスチャCLUT ID */
	tsprt->sprt.clut = tex->clut;	

	/* OT に登録 */
	AddPrim(ot, &tsprt->mode);	
}

/* 値をセットする TSPRT 構造体へのポインタ */
/* int	 dfe,	 /* 表示領域への描画  0:不可、1:可 */
/* int	 dtd,	 /* ディザ  0:なし、1:あり */
/* int	 tpage,	 /* テクスチャページ */
/* RECT	 *tw	 /* テクスチャウィンドウ */

static int SetTSprt(TSPRT *tsprt,int dfe,int dtd,int tpage,RECT *tw)
{
	/* MODE プリミティブを初期化 */
 	SetDrawMode(&tsprt->mode, dfe, dtd, tpage, tw);

	/* SPRT プリミティブを初期化 */
	SetSprt(&tsprt->sprt);

	/* ２つのプリミティブをマージする。
	 * マージするプリミティブは、メモリ上の連続した領域に置かれなくて
	 *  はいけない。マージするプリミティブのサイズは 16 ワード以下
	 */
	if (MargePrim(&tsprt->mode, &tsprt->sprt) != 0) {
		printf("Marge failed!\n");
		return(-1);
	}
	return(0);
}

