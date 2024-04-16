/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			tuto4: cube with lighting
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/* 光源のある立方体の描画 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* コントローラレコーダ用マクロ */
#define PadRead(x)	myPadRead(x)

#define SCR_Z	(512)		
#define	OTSIZE	(4096)

/* プリミティブ関連のバッファ */
typedef struct {
	DRAWENV		draw;		/* 描画環境 */
	DISPENV		disp;		/* 表示環境 */
	u_long		ot[OTSIZE];	/* オーダリングテーブル */
	POLY_F4		s[6];		/* 立方体の側面 */
} DB;

/* 光源色（ローカルカラーマトリックス） */
static MATRIX	cmat = {
/* light source    #0, #1, #2, */
		ONE*3/4,  0,  0, /* R */
		ONE*3/4,  0,  0, /* G */
		ONE*3/4,  0,  0, /* B */
};

/* 光源ベクトル（ローカルライトマトリックス） */
static MATRIX lgtmat = {
	/*          X     Y     Z */
	          ONE,  ONE, ONE,	/* 光源 #0 */
		    0,    0,    0,	/*      #1 */
		    0,    0,    0	/*      #2 */
};

static int pad_read(MATRIX *rottrans, MATRIX *rotlgt);
static void init_prim(DB *db);

void tuto4()
{
	DB	db[2];		/* double buffer */
	DB	*cdb	;	/* current buffer */
	MATRIX	rottrans;	/* rot-trans matrix */
	
	/* 光源の回転マトリックス */
	MATRIX		rotlgt;		
	
	/* 最終的な光源マトリックス */
	MATRIX		light;

	/* 立方体の側面の色 */
	CVECTOR		col[6];	
	
	int		i;		/* work */
	int		dmy, flg;	/* dummy */

	/* ダブルバッファ用の環境設定（インターレースモード） */
	init_system(320, 240, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);

	/* バックカラー(アンビエント色)の設定 */
	SetBackColor(64, 64, 64);	
	
	/* ローカルカラーマトリックスの設定 */
	SetColorMatrix(&cmat);		

	/* プリミティブバッファの初期設定 */
	init_prim(&db[0]);	
	init_prim(&db[1]);	

	/* 立方体の側面の色の設定
	 * 後の NormalColorCol() で db[0].s[0].code の内容が壊されて
	 * しまうため、ここで待避しておく
	 */
	for (i = 0; i < 6; i++) {
		col[i].cd = db[0].s[0].code;	/* CODE */
		col[i].r  = rand();		/* R */
		col[i].g  = rand();		/* G */
		col[i].b  = rand();		/* B */
	}

	SetDispMask(1);			/* 表示開始 */
	PutDrawEnv(&db[0].draw);	/* 描画環境の設定 */
	PutDispEnv(&db[0].disp);	/* 表示環境の設定 */

	while (pad_read(&rottrans, &rotlgt) == 0) {

		cdb = (cdb==db)? db+1: db;	/* change current buffer */
		ClearOTagR(cdb->ot, OTSIZE);	/* clear OT */

		/* 光源マトリックスの設定 */
		MulMatrix0(&lgtmat, &rotlgt, &light);
		SetLightMatrix(&light);

		/* 立方体をＯＴに登録する */
		add_cubeF4L(cdb->ot, cdb->s, &rottrans, col);

		VSync(0);
		ResetGraph(1);
		
		/* 背景のクリア */
		ClearImage(&cdb->draw.clip, 60, 120, 120);

		/* 描画 */
		/*DumpOTag(cdb->ot+OTSIZE-1);	/* for debug */
		DrawOTag(cdb->ot+OTSIZE-1);	
		FntFlush(-1);
	}
	DrawSync(0);
	return;
}

/* コントローラの解析と、変換マトリックスの計算 */
/* MATRIX *rottrans; 	/* 立方体の回転・平行移動マトリックス */
/* MATRIX *rotlgt;	/* 光源マトリックス */
static int pad_read(MATRIX *rottrans, MATRIX *rotlgt)
{
	/* angle 立方体の回転角度 */
	static SVECTOR	ang  = { 0, 0, 0};	
	
	/* 光源の回転角度 */
	static SVECTOR	lgtang = {1024, -512, 1024};	
	
	/* 平行移動ベクトル */
	static VECTOR	vec  = {0, 0, SCR_Z};	

	/* コントローラからデータを読み込む */
	u_long	padd = PadRead(1);
	int	ret = 0;

	/* プログラムの終了 */
	if (padd & PADselect) 	ret = -1;	

	/* 光源と立方体の回転角度の変更 */
	if (padd & PADRup)	ang.vz += 32;
	if (padd & PADRdown)	ang.vz -= 32;
	if (padd & PADRleft) 	ang.vy += 32;
	if (padd & PADRright)	ang.vy -= 32;
	
	/* 光源のみの回転角度の変更 */
	if (padd & PADLup)	lgtang.vx += 32;
	if (padd & PADLdown)	lgtang.vx -= 32;
	if (padd & PADLleft) 	lgtang.vy += 32;
	if (padd & PADLright)	lgtang.vy -= 32;

	/* 原点からの距離 */
	if (padd & (PADL2|PADR2))	vec.vz += 8;
	if (padd & (PADL1|PADR1))	vec.vz -= 8;

	/* 光源の回転マトリクス計算 */
	RotMatrix(&lgtang, rotlgt);	
	MulMatrix(rotlgt, rottrans);

	/* 立方体の回転マトリクス計算 */
	RotMatrix(&ang, rottrans);	
	TransMatrix(rottrans, &vec);	

	FntPrint("tuto4: lighting angle=(%d,%d,%d)\n",
		 lgtang.vx, lgtang.vy, lgtang.vz);
	return(ret);
}

/* プリミティブ関連の初期設定 */
/* DB	*db;	/* プリミティブバッファ */
static void init_prim(DB *db)
{
	int i;

	/* フラットシェーディング４角形プリミティブの初期設定 */
	for(i = 0;i < 6;i++)
		SetPolyF4(&db->s[i]);	
}
