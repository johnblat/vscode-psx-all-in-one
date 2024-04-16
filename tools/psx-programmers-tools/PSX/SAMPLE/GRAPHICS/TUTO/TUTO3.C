/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			tuto3: simple cube
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/* 回転する立方体を描画する  */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* コントローラレコーダ用マクロ */
#define PadRead(x)	myPadRead(x)

/* OTにプリミティブを登録するときに参照する otz の値を返す
 * libgte 関数の多くは、実際の奥行き(z)の値の1/4の値を返すため、
 * OTのサイズも実際の分解能の1/4 (4096) でよい。
 */
#define SCR_Z	(512)		/* スクリーンの深さ */
#define	OTSIZE	(4096)		/* ＯＴのサイズ */

typedef struct {
	DRAWENV		draw;		/* 描画環境 */
	DISPENV		disp;		/* 表示環境 */
	u_long		ot[OTSIZE];	/* オーダリングテーブル */
	POLY_F4		s[6];		/* 立方体の側面 */
} DB;

static int pad_read(MATRIX *rottrans);
static void init_prim(DB *db, CVECTOR *c);

void tuto3(void)
{
	/* ダブルバッファ */
	DB		db[2];		
	
	/* 現在のバッファアドレス */
	DB		*cdb;		
	
	/* 回転・平行移動マトリックス */
	MATRIX		rottrans;	
	
	/* 立方体の色 */
	CVECTOR		col[6];		
	
	int		i;		/* work */
	int		dmy, flg;	/* work */

	/* ダブルバッファ用の環境設定（インターレースモード）*/
	/*	buffer #0	(0,  0)-(640,480)
	 *	buffer #1	(0,  0)-(640,480)
	 */
	init_system(320, 240, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);

	/* 立方体の側面の色を設定する */
	for (i = 0; i < 6; i++) {
		col[i].r = rand();	/* R */
		col[i].g = rand();	/* G */
		col[i].b = rand();	/* B */
	}

	/* プリミティブバッファの初期設定 #0/#1 */
	init_prim(&db[0], col);	
	init_prim(&db[1], col);	

	/* 表示開始 */
	SetDispMask(1);			

	/* インターレースモードの場合、描画／表示ともフレームバッファ中の
	 * 同じ領域を使用しているため、１フレームごとに描画環境／表示環境の
	 * 切替えを行う必要がありません。
	 * したがって環境設定は最初に一度だけ行います。
	 * ただし、描画はプログラムの処理と並列に行われるため、
         *  プリミティブはダブルで持つ必要があります。
	 */
	PutDrawEnv(&db[0].draw);	/* 描画環境の設定 */
	PutDispEnv(&db[0].disp);	/* 表示環境の設定 */
	
	/* select キーが押されるまでループする */
	while (pad_read(&rottrans) == 0) {	

		/* ダブルバッファポインタの切り替え */
		cdb = (cdb==db)? db+1: db;	
		
		/* OT をクリアする。
		 * ClearOTagR() は逆順の OT を生成する。これは 3D のアプリ
		 * ケーションの場合には自然な順番になる。また ClearOTagR()
		 * はハードウェアで OT をクリアするので高速
		 */
		ClearOTagR(cdb->ot, OTSIZE);	

		/* 立方体をＯＴに登録する */
		add_cubeF4(cdb->ot, cdb->s, &rottrans);

		/* インターレースモードの場合は、すべての描画処理は 1/60sec で
		 * 終了しなくてはならない。そのため、DrawSync(0) の代わりに
		 * ResetGraph(1) を使用してVSync のタイミングで描画を打ち切る
		 */
		VSync(0);	
		ResetGraph(1);	

		/* 背景のクリア */
		ClearImage(&cdb->draw.clip, 60, 120, 120);
		
		/* ClearOTagR() は OT を逆順にクリアするので OT の先頭ポイ
		 * ンタは ot[0] ではなくて ot[OTSIZE-1] になる。そのため、
		 * DrawOTag() は ot[OTSIZE-1] から開始しなくてはならない
		 * ことに注意
		 */
		/*DumpOTag(cdb->ot+OTSIZE-1);	/* for debug */
		DrawOTag(cdb->ot+OTSIZE-1);	
		FntFlush(-1);
	}
        /* コントローラをクローズ */
	DrawSync(0);
	return;
}

/* コントローラの解析と、変換マトリックスの設定を行う。 */
/* 回転・平行移動マトリックス */
static int pad_read(MATRIX *rottrans)
{
	
	/* 回転角度( 360°= 4096 ) */
	static SVECTOR	ang  = { 0, 0, 0};	
	
	/* 平行移動ベクトル */
	static VECTOR	vec  = {0, 0, SCR_Z};	

	/* コントローラからデータを読み込む */
	u_long	padd = PadRead(1);
	
	int	ret = 0;

	/* 終了 */
	if (padd & PADselect) 	ret = -1;	

	/* 回転角度の変更（z, y, x の順に回転） */
	if (padd & PADRup)	ang.vz += 32;
	if (padd & PADRdown)	ang.vz -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;

	/* 原点からの距離 */
	if (padd & (PADL2|PADR2))	vec.vz += 8;
	if (padd & (PADL1|PADR1))	vec.vz -= 8;

	/* マトリックスの計算 */
	RotMatrix(&ang, rottrans);	/* 回転 */
	TransMatrix(rottrans, &vec);	/* 平行移動 */

	/* マトリックスの設定 */
	SetRotMatrix(rottrans);		/* 回転 */
	SetTransMatrix(rottrans);	/* 平行移動 */

	/* 現在のジオメトリ状況をプリント */
	FntPrint("tuto3: simple cube angle=(%d,%d,%d)\n",
		 ang.vx, ang.vy, ang.vz);
		
	return(ret);
}

/* プリミティブの初期化 */
/*DB	*db;	/* プリミティブバッファ */
/*CVECTOR *c;	/* 側面の色 */
static void init_prim(DB *db, CVECTOR *c)
{
	int	i;

	/* 側面の初期設定 */
	for (i = 0; i < 6; i++) {
		/* フラット４角形プリミティブの初期化 */
		SetPolyF4(&db->s[i]);	
		setRGB0(&db->s[i], c[i].r, c[i].g, c[i].b);
	}
}
