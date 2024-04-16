/* $PSLibId: Run-time Library Release 4.4$ */
/*                  balls:
 *
 *        画面内をバウンドする複数のボールを描画する
 *
 *        Copyright  (C)  1993 by Sony Corporation
 *             All rights Reserved
 *
 *    Version  Date      Design
 *   -----------------------------------------
 *   1.00      Aug,31,1993    suzu
 *   2.00      Nov,17,1993    suzu  (using 'libgpu) 
 *   3.00      Dec.27.1993    suzu  (rewrite) 
 *   3.01      Dec.27.1993    suzu  (for newpad) 
 *   3.02      Aug.31.1994    noda     (for KANJI) 
 *   4.00      May.22.1995    sachiko    (added comments in Japanese) 
 *   4.01      Mar. 5.1997    sachiko    (added autopad) 
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* コントローラレコーダ用マクロ */
#define PadRead(x)	myPadRead(x)

/* 漢字をプリントするための定義  */
#define KANJI

/*#define DEBUG 	/* for debug */

/* プリミティブ関連のバッファ */
#define OTSIZE	1		/* OT の段数 */
#define MAXOBJ	4000		/* ボール数の上限 */

typedef struct {
	DRAWENV	draw;		/* 描画環境 */
	DISPENV	disp;		/* 表示環境 */
	u_long	ot[OTSIZE];	/* オーダリングテーブル */
	SPRT_16	sprt[MAXOBJ];	/* 16x16 スプライト */
} DB;

/* スプライトの動きに関するバッファ  */
typedef struct {
	u_short x, y;		/* 現在の位置 */
	u_short dx, dy;		/* 速度 */
} POS;

/* 表示領域 */
#define	FRAME_X	320		/* 表示領域サイズ(320x240)*/
#define	FRAME_Y	240
#define WALL_X	(FRAME_X-16)	/* 反射壁の位置) */
#define WALL_Y	(FRAME_Y-16)

/* プロトタイプ */
static void init_prim(DB *db);	
static int  pad_read(int n);	
static int  init_point(POS *pos);
static void cbvsync(void);

/* メイン関数 */
void Balls(void)
{
	
	/* ボールの座標値と移動距離を格納するバッファ*/
	POS	pos[MAXOBJ];	
	
	/* ダブルバッファのため２つ用意する */
	DB	db[2];		
	
	/* 現在のダブルバッファバッファのアドレス */
	DB	*cdb;		

	/* 表示するスプライトの数（最初は１つから）*/
	int	nobj = 1;	
	
	/* 現在のＯＴのアドレス */
	u_long	*ot;		
	
	SPRT_16	*sp;		/* work */
	POS	*pp;		/* work */
	int	i, cnt, x, y;	/* work */
	
	/* デバッグモードの設定 */
	SetGraphDebug(0);	
	
	/* V-sync時のコールバック関数の設定*/
	VSyncCallback(cbvsync);	

	/* 描画・表示環境をダブルバッファ用に設定     
  	(0,  0)-(320,240)に描画しているときは(0,240)-(320,480)を表示(db[0])
	(0,240)-(320,480)に描画しているときは(0,  0)-(320,240)を表示(db[1])
*/
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* フォントの設定 */
#ifdef KANJI	/* KANJI */
	KanjiFntOpen(160, 16, 256, 200, 704, 0, 768, 256, 0, 512);
#endif	
	/* フォントパターンをフレームバッファにロード */
	FntLoad(960, 256);	
	
	/* 文字列表示位置の設定 */
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));	

	/* プリミティブバッファの初期設定(db[0])*/
	init_prim(&db[0]);	
	
	/* プリミティブバッファの初期設定(db[1])*/
	init_prim(&db[1]);	
	
	/* ボールの動きに関する初期設定 */
	init_point(pos);	

	/* メインループ */
	while ((nobj = pad_read(nobj)) > 0) {
		/* ダブルバッファポインタの切り替え */
		cdb  = (cdb==db)? db+1: db;	
#ifdef DEBUG
		/* dump DB environment */
		DumpDrawEnv(&cdb->draw);
		DumpDispEnv(&cdb->disp);
		DumpTPage(cdb->draw.tpage);
#endif

 		/* オーダリングテーブルのクリア */
		ClearOTag(cdb->ot, OTSIZE);

		/* ボールの位置を計算してＯＴに登録 */
		ot = cdb->ot;
		sp = cdb->sprt;
		pp = pos;
		for (i = 0; i < nobj; i++, sp++, pp++) {
			/* 反射計算 */
			if ((x = (pp->x += pp->dx) % WALL_X*2) >= WALL_X)
				x = WALL_X*2 - x;
			if ((y = (pp->y += pp->dy) % WALL_Y*2) >= WALL_Y)
				y = WALL_Y*2 - y;

			/* 計算した座標値をセット */
			setXY0(sp, x, y);	
			
			/* ＯＴへ登録 */
			AddPrim(ot, sp);	
		}
		/* 描画の終了待ち */
		DrawSync(0);		
		
		/* VSync(1) で CPU タイムがわかる */
		/* cnt = VSync(1); */

		/* 30 fps の時はこちらを使用 */
		/* cnt = VSync(2); */	

		/* VSync を待つ */
		cnt = VSync(0);		/* wait for V-BLNK (1/60) */

		/* ダブルバッファの切替え */
		/* 表示環境の更新 */
		PutDispEnv(&cdb->disp); 
		
		/* 描画環境の更新 */
		PutDrawEnv(&cdb->draw); 
		
		/* ＯＴに登録されたプリミティブの描画 */
		DrawOTag(cdb->ot);	
#ifdef DEBUG
		DumpOTag(cdb->ot);
#endif
		/* ボールの数と経過時間のプリント */
#ifdef KANJI
		KanjiFntPrint("玉の数＝%d\n", nobj);
		KanjiFntPrint("時間=%d\n", cnt);
		KanjiFntFlush(-1);
#endif
		FntPrint("sprite = %d\n", nobj);
		FntPrint("total time = %d\n", cnt);
		FntFlush(-1);
	}
	/* Callback をクリア */
	VSyncCallback(0);
	
	DrawSync(0);
	return;
}

/* プリミティブバッファの初期設定 */
#include "balltex.h"	/* ボールのテクスチャパターン */
static void init_prim(DB *db)
{
	u_short	clut[32];	/* テクスチャ CLUT */
	SPRT_16	*sp;		/* work */
	int	i;		/* work */

	/* 背景色のセット */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);

	/* テクスチャのロード */
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);
#ifdef DEBUG
	DumpTPage(db->draw.tpage);
#endif
	/* テクスチャ CLUTのロード */
	for (i = 0; i < 32; i++) {
		clut[i] = LoadClut(ballcolor[i], 0, 480+i);
#ifdef DEBUG
		DumpClut(clut[i]);
#endif
	}

	/* スプライトの初期化 */
	for (sp = db->sprt, i = 0; i < MAXOBJ; i++, sp++) {
		/* 16x16スプライトプリミティブの初期化 */
		SetSprt16(sp);		
		
		/* 半透明属性オフ */
		SetSemiTrans(sp, 0);	
		
		/* シェーディングを行わない */
		SetShadeTex(sp, 1);	
		
		/* u,vを(0,0)に設定 */
		setUV0(sp, 0, 0);	
		
		/* CLUT のセット */
		sp->clut = clut[i%32];	
	}
}

/* プリミティブバッファの初期設定 */
static init_point(POS *pos)	
{
	int	i;
	for (i = 0; i < MAXOBJ; i++) {
		pos->x  = rand();		/* スタート座標 Ｘ*/
		pos->y  = rand();		/* スタート座標 Ｙ*/
		pos->dx = (rand() % 4) + 1;	/* 移動距離 Ｘ (1<=x<=4)*/
		pos->dy = (rand() % 4) + 1;	/* 移動距離 Ｙ (1<=y<=4)*/
		pos++;
	}
}

/* コントローラの解析 */
static int pad_read(int	n)		
{
	u_long	padd;

	/* コントローラの読み込み */
	padd = PadRead(1);

	if(padd & PADLup)	n += 4;		/* 増加 */
	if(padd & PADLdown)	n -= 4;		/* 減少 */
	if(padd & PADRup)	n += 4;		/* 増加 */
	if(padd & PADRdown)	n -= 4;		/* 減少 */
	if(padd & PADselect) 	return(-1);	/* 終了 */

	/* [0,MAXOBJ-1] クランプする。libgpu.h の定義を参照 */
	limitRange(n, 1, MAXOBJ-1);	
					
	return(n);
}

/* コールバック */
static void cbvsync(void)
{
	/* Vsync のカウント値を表示 */
	FntPrint("V-BLNK(%d)\n", VSync(-1));	
}


