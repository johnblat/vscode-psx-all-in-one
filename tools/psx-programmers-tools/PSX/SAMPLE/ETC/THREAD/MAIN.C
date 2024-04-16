/* $PSLibId: Run-time Library Release 4.4$ */
/*        balls & thread:
 *
 *	  画面内をバウンドする複数のボールを描画したあと、
 *	  空き時間に、カウンターをアップする
 *
 *        Copyright  (C)  1996 by Sony Corporation
 *             All rights Reserved
 *
 *    Version  Date      Design
 *   -----------------------------------------
 *   1.00      Apr.25.1996    hakama   (based balls 4.00) 
 *   -----------------------------------------
 */

#include <r3000.h>
#include <asm.h>
#include <libapi.h>

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* 漢字をプリントするための定義 */
#define KANJI

/*#define DEBUG */
/* プリミティブ関連のバッファ */
#define OTSIZE		1		/* オーダリングテーブルの数 */
#define MAXOBJ		4000		/* スプライト（ボール）数の上限 */
typedef struct {
	DRAWENV		draw;		/* 描画環境 */
	DISPENV		disp;		/* 表示環境 */
	u_long		ot[OTSIZE];	/* オーダリングテーブル */
	SPRT_16		sprt[MAXOBJ];	/* 16x16固定サイズのスプライト */
} DB;

/* スプライトの動きに関するバッファ */
typedef struct {
	u_short x, y;			/* 現在の位置 */
	u_short dx, dy;			/* 速度 */
} POS;

/* 表示領域 */
#define	FRAME_X		320		/* 表示領域サイズ(320x240)*/
#define	FRAME_Y		240
#define WALL_X		(FRAME_X-16)	/* スプライトの可動領域サイズ */
#define WALL_Y		(FRAME_Y-16)

/* プリミティブバッファの初期設定 */
static void init_prim(DB *db);	

/* コントローラの解析 */
static int  pad_read(int n);	

/* V-Sync時のコールバックルーチン */
static void  cbvsync(void);	

/* ボールのスタート位置と移動距離の設定 */
static int  init_point(POS *pos);

/* thread 用のデータ定義 */
#define THREAD_SAMPLE

#ifdef THREAD_SAMPLE
/* thread 用のデータ定義 */
#define SUB_STACK 0x80180000 /* sub-thread用のスタック。適当に変更 */
static volatile unsigned long count1,count2; /* カウンタ*/
struct ToT *sysToT = (struct ToT *) 0x100 ; /* Table of Tabbles  */
struct TCBH *tcbh ; /* タスク状態キューアドレス*/
struct TCB *master_thp,*sub_thp; /* thread コンテクストの先頭アドレス*/
static long sub_func() ; /* sub thread 関数*/
#endif

main()
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
	
#ifdef THREAD_SAMPLE
	/* thread data (local)*/
	unsigned long sub_th,gp;

	/* thread 用の初期化*/

	/* コンフィギュレーションのやり直し*/
	/* この作業は、本来必要ないのですが、２０００を使用していると
	   必要になります。詳細は 2190 の \bata\setconf を参照してくだ
	   さい。ここの SetConf2 は、version3.4 では、SetConf におき変
	   わっています */
	SetConf(16,4,0x80200000);

	/* タスク状態キューアドレスと 現 thread コンテクストアドレスの取得*/
	tcbh = (struct TCBH *) sysToT[1].head;
	master_thp = tcbh->entry;

	/* sub thread の作成*/
	/* ここらは、ライブラリリファレンスDTL-S2150A vol-1 p25-p29*/
	/* vol-2 p22-p24 を参照してください*/
	gp = GetGp();
	EnterCriticalSection();
	sub_th = OpenTh(sub_func, SUB_STACK, gp);
	ExitCriticalSection();
	/* sub thread コンテクストアドレスの取得と割り込み状態の設定*/
	sub_thp = (struct TCB *) sysToT[2].head + (sub_th & 0xffff);
	sub_thp->reg[R_SR] = 0x404;
#endif	
	/* コントローラのリセット*/
	PadInit(0);		
	
	/* 描画・表示環境のリセット(0:cold,1:warm)  */
	ResetGraph(0);		
	
	/* デバッグモードの設定(0:off,1:monitor,2:dump)*/
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
	/* 基本フォントパターンをフレームバッファにロード*/
	FntLoad(960, 256);	
	
	/* フォントの表示位置の設定 */
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));	

	/* プリミティブバッファの初期設定(db[0])*/
	init_prim(&db[0]);	
	
	/* プリミティブバッファの初期設定(db[1])*/
	init_prim(&db[1]);	
	
	/* ボールの動きに関する初期設定 */
	init_point(pos);	

	/* ディスプレイへの表示開始 */
	SetDispMask(1);		/* ０:不可  １:可 */

	/* メインループ*/
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

		/* ボールの位置を１つずつ計算してＯＴに登録していく */
		ot = cdb->ot;
		sp = cdb->sprt;
		pp = pos;
		for (i = 0; i < nobj; i++, sp++, pp++) {
			/* 座標値の更新および画面上での位置の計算 */
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
		
		/* cnt = VSync(1);	/* check for count */
		/* cnt = VSync(2);	/* wait for V-BLNK (1/30) */
		/* cnt = VSync(0);	/* wait for V-BLNK (1/60) */
#ifdef THREAD_SAMPLE
		/* sub thread へ移る。sub thread では、前回割り込みで
		   制御を取られた場所から始まります（コルーチン）。 */
		ChangeTh(sub_th);
#else
		cnt = VSync(0);		/* wait for V-BLNK (1/60) */
#endif		
		/* ダブルバッファの切替え*/
		/* 表示環境の更新 */
		PutDispEnv(&cdb->disp); 
		
		/* 描画環境の更新 */
		PutDrawEnv(&cdb->draw); 
		
		/* ＯＴに登録されたプリミティブの描画*/
		DrawOTag(cdb->ot);	
#ifdef DEBUG
		DumpOTag(cdb->ot);
#endif
		/* ボールの数と経過時間のプリント*/
#ifdef KANJI
		KanjiFntPrint("玉の数＝%d\n", nobj);
#ifdef THREAD_SAMPLE
		/* 割り込みまでの時間に何回ループできたかを表示する*/
		KanjiFntPrint("空き時間=%d\n", count2-count1);
#else
		KanjiFntPrint("時間=%d\n", cnt);
#endif
		KanjiFntFlush(-1);
#endif
		
		FntPrint("sprite = %d\n", nobj);
#ifdef THREAD_SAMPLE
		/* 割り込みまでの間に何回ループできたかを表示する・その２*/
		FntPrint("free time = %d\n", count2 - count1);
#else
		FntPrint("total time = %d\n", cnt);
#endif
		FntFlush(-1);
		
#ifdef THREAD_SAMPLE
		/* カウンタを待避*/
		count1 = count2;
#endif
	}
#ifdef THREAD_SAMPLE
	/* thread の消去*/
	EnterCriticalSection();
	CloseTh(sub_th);
	ExitCriticalSection();
#endif
	PadStop();	/* コントローラのクローズ*/
	StopCallback();
	return(0);
}

/* プリミティブバッファの初期設定 */
#include "balltex.h"	/* ボールのテクスチャパターンが入っているファイル */

/* プリミティブバッファ*/
static void init_prim(DB *db)
{
	u_short	clut[32];		/* テクスチャ CLUT */
	SPRT_16	*sp;			/* work */
	int	i;			/* work */

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

/* ボールのスタート位置と移動量を設定する */

/* ボールの動きに関する構造体*/
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
/* スプライトの数*/
static int pad_read(int	n)		
{
	u_long	padd = PadRead(1);		/* コントローラの読み込み*/

	if(padd & PADLup)	n += 4;		/* 左の十字キーアップ*/
	if(padd & PADLdown)	n -= 4;		/* 左の十字キーダウン*/

	if (padd & PADL1) 			/* pause */
		while (PadRead(1)&PADL1);

	if(padd & PADselect) 	return(-1);	/* プログラムの終了*/

	limitRange(n, 1, MAXOBJ-1);	/* nを1<=n<=(MAXOBJ-1)の値にする*/
					/* libgpu.hに記載*/
	return(n);
}

/* コールバック */

static void cbvsync(void)
{
	/* print absolute VSync count */
	FntPrint("V-BLNK(%d)\n", VSync(-1));
#ifdef THREAD_SAMPLE
	/* 割り込みの戻りを main thread に設定する。これをやらないと、
	   sub thread(sub_func() 内) に戻ってきます。 */
	tcbh->entry = master_thp;
#endif
}

#ifdef THREAD_SAMPLE
/* 空き時間の間、ループしてカウントアップするプログラム。
   ChangeTh() でコールされた関数は return しても戻る場所がないので注意 */
static long sub_func()
{
  count1 = 0;
  count2 = 0;
  while(1){
    /* この while loop のどこかで Vsync割り込みがかかり、制御が取られる。
       次に ChangeTh() されたときは、そこから始まる。 */
    count2 ++;
  }
}
#endif
