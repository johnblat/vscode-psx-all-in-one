/* $PSLibId: Run-time Library Release 4.4$ */
/*	   dif_rmat: difference between RotMatrix & RotMatrix_gte
 *
 *         Copyright (C) 1993-1997 by Sony Computer Entertainment
 *			All rights Reserved
 */
/* ２つの立方体をRotMatrixとRotMatrix_gteする  */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

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
	POLY_F4		s[2][6];	/* 立方体の側面 */
	POLY_F4		t[2][6];	/* 立方体の側面 */
	POLY_F4		u[2][6];	/* 立方体の側面 */
} DB;

static int pad_read(MATRIX *rottrans,MATRIX *rottrant,MATRIX *rottranu);
static void init_prim(DB *db, CVECTOR *c);

main()
{
	/* ダブルバッファ */
	DB		db[2];		
	
	/* 現在のバッファアドレス */
	DB		*cdb;		
	
	/* 回転・平行移動マトリックス */
	MATRIX		rottrans[2];	
	MATRIX		rottrant[2];	
	MATRIX		rottranu[2];	
	
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
	while (pad_read(rottrans,rottrant,rottranu) == 0) {	

		/* ダブルバッファポインタの切り替え */
		cdb = (cdb==db)? db+1: db;	
		
		/* OT をクリアする。
		 * ClearOTagR() は逆順の OT を生成する。これは 3D のアプリ
		 * ケーションの場合には自然な順番になる。また ClearOTagR()
		 * はハードウェアで OT をクリアするので高速
		 */
		ClearOTagR(cdb->ot, OTSIZE);	

		/* 立方体をＯＴに登録する */
		SetRotMatrix(&rottrans[0]);
		SetTransMatrix(&rottrans[0]);
		SetGeomOffset(150,180);
		add_cubeF4(cdb->ot, cdb->s[0], &rottrans[0]);

		SetRotMatrix(&rottrans[1]);
		SetTransMatrix(&rottrans[1]);
		SetGeomOffset(150,350);
		add_cubeF4(cdb->ot, cdb->s[1], &rottrans[1]);

		SetRotMatrix(&rottrant[0]);
		SetTransMatrix(&rottrant[0]);
		SetGeomOffset(320,180);
		add_cubeF4(cdb->ot, cdb->t[0], &rottrant[0]);

		SetRotMatrix(&rottrant[1]);
		SetTransMatrix(&rottrant[1]);
		SetGeomOffset(320,350);
		add_cubeF4(cdb->ot, cdb->t[1], &rottrant[1]);

		SetRotMatrix(&rottranu[0]);
		SetTransMatrix(&rottranu[0]);
		SetGeomOffset(490,180);
		add_cubeF4(cdb->ot, cdb->u[0], &rottranu[0]);

		SetRotMatrix(&rottranu[1]);
		SetTransMatrix(&rottranu[1]);
		SetGeomOffset(490,350);
		add_cubeF4(cdb->ot, cdb->u[1], &rottranu[1]);

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
	PadStop();	
	ResetGraph(1);
	StopCallback();
	return;
}

/* コントローラの解析と、変換マトリックスの設定を行う。 */
/* 回転・平行移動マトリックス */
static int pad_read(MATRIX *rottrans,MATRIX *rottrant,MATRIX *rottranu)
{
	
	/* 回転角度( 360°= 4096 ) */
	static SVECTOR	ang  = { 0, 0, 0};	
	
	/* 平行移動ベクトル */
	static VECTOR	vec  = {0, 0, 2*SCR_Z};	

	/* コントローラからデータを読み込む */
/*	u_long	padd = PadRead(1);*/
	u_long	padd;
	
	int	ret = 0;

	padd = PadRead(1);

	/* 終了 */
	if (padd & PADselect) 	ret = -1;	

	/* 回転角度の変更 */
	if (padd & PADRup)	ang.vz += 32;
	if (padd & PADRdown)	ang.vz -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;

	if (padd & PADL2)	ang.vx += 32;
	if (padd & PADR2) 	ang.vx -= 32;

	/* 原点からの距離 */
	if (padd & PADL1)	vec.vz += 8;
	if (padd & PADR1) 	vec.vz -= 8;

	/* マトリックスの計算 */
	RotMatrix(&ang, &rottrans[0]);	/* 回転 */
	RotMatrix_gte(&ang, &rottrans[1]);	/* 回転 */
	TransMatrix(&rottrans[0], &vec);	/* 平行移動 */
	TransMatrix(&rottrans[1], &vec);	/* 平行移動 */

	RotMatrixYXZ(&ang, &rottrant[0]);	/* 回転 */
	RotMatrixYXZ_gte(&ang, &rottrant[1]);	/* 回転 */
	TransMatrix(&rottrant[0], &vec);	/* 平行移動 */
	TransMatrix(&rottrant[1], &vec);	/* 平行移動 */

	RotMatrixZYX(&ang, &rottranu[0]);	/* 回転 */
	RotMatrixZYX_gte(&ang, &rottranu[1]);	/* 回転 */
	TransMatrix(&rottranu[0], &vec);	/* 平行移動 */
	TransMatrix(&rottranu[1], &vec);	/* 平行移動 */

	/* 現在のジオメトリ状況をプリント */
	FntPrint("dif_rmat: angle=(%d,%d,%d)\n",
		 ang.vx, ang.vy, ang.vz);
	FntPrint("\n\n\n\n\n\n\n\n\n");
	FntPrint("        RotMatrix            RotMatrixYXZ          RotMatrixZYX");
	FntPrint("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	FntPrint("        RotMatrix_gte        RotMatrixYXZ_gte      RotMatrixZYX_gte");
		
	return(ret);
}

/* プリミティブの初期化 */
/* DB	*db;	プリミティブバッファ */
/* CVECTOR *c;	側面の色 */
static void init_prim(DB *db, CVECTOR *c)
{
	int	i;

	/* 側面の初期設定 */
	for (i = 0; i < 6; i++) {
		/* フラット４角形プリミティブの初期化 */
		SetPolyF4(&db->s[0][i]);	
		setRGB0(&db->s[0][i], c[i].r, c[i].g, c[i].b);
		SetPolyF4(&db->s[1][i]);	
		setRGB0(&db->s[1][i], c[i].r, c[i].g, c[i].b);

		SetPolyF4(&db->t[0][i]);	
		setRGB0(&db->t[0][i], c[i].r, c[i].g, c[i].b);
		SetPolyF4(&db->t[1][i]);	
		setRGB0(&db->t[1][i], c[i].r, c[i].g, c[i].b);

		SetPolyF4(&db->u[0][i]);	
		setRGB0(&db->u[0][i], c[i].r, c[i].g, c[i].b);
		SetPolyF4(&db->u[1][i]);	
		setRGB0(&db->u[1][i], c[i].r, c[i].g, c[i].b);
	}
}

