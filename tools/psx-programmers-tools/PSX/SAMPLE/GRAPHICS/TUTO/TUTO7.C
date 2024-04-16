/* $PSLibId: Run-time Library Release 4.4$ */
/*		  tuto7: many cubes with local coordinates
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/* 複数の 3D オブジェクトの描画
 *
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* コントローラレコーダ用マクロ */
#define PadRead(x)	myPadRead(x)

#define SCR_Z	1024		
#define	OTSIZE	4096
#define NCUBE	256		/* 表示する立方体の数の最大値 */

typedef struct {
	POLY_F4		s[6];		/* 立方体の側面 */
} CUBE;

typedef struct {
	DRAWENV		draw;		/* 描画環境 */
	DISPENV		disp;		/* 表示環境 */
	u_long		ot[OTSIZE];	/* オーダリングテーブル */
	CUBE		c[NCUBE];	/* 立方体のデータ */
} DB;

typedef struct {
	CVECTOR	col[6];			/* 側面の色 */
	SVECTOR	trans;			/* 平行移動ベクトル（ローカル座標）*/
} CUBEATTR;

/* 光源色（ローカルカラーマトリックス） */
static MATRIX	cmat = {
/* 光源    #0, #1, #2, */
		ONE*3/4,  0,  0, /* R */
		ONE*3/4,  0,  0, /* G */
		ONE*3/4,  0,  0, /* B */
};

/* 光源ベクトル（ローカルライトマトリックス） */
static MATRIX lgtmat = {
	/*          X     Y     Z */
	          ONE,  ONE, ONE,	/* 光源 #0 */
		    0,    0,    0,	/* #1 */
		    0,    0,    0	/* #2 */
};

static int pad_read(int *ncube, 
		MATRIX *world, MATRIX *local, MATRIX *rotlgt);
static void init_attr(CUBEATTR *attr, int nattr);
static init_prim(DB *db);

void tuto7(void)
{
	
	DB	db[2];		/* double buffer */
	DB	*cdb;		/* current buffer */
	
	/* 立方体の属性 */
	CUBEATTR	attr[NCUBE];	
	
	/* ワールドスクリーンマトリクス */
	MATRIX		ws;

	/* ローカルスクリーンマトリクス */
	MATRIX		ls;

	/* 光源のローカルスクリーンマトリックス */
	MATRIX		lls;		
	
	/* 最終的な光源マトリックス */
	MATRIX		light;

	/* 表示する立方体の数 */
	int		ncube = NCUBE/2;
	
	int		i;		/* work */
	long		dmy, flg;	/* dummy */

	/* ダブルバッファ用の環境設定（インターレースモード）*/
	init_system(320, 240, SCR_Z, 0);
	SetDefDrawEnv(&db[0].draw, 0, 0, 640, 480);
	SetDefDrawEnv(&db[1].draw, 0, 0, 640, 480);
	SetDefDispEnv(&db[0].disp, 0, 0, 640, 480);
	SetDefDispEnv(&db[1].disp, 0, 0, 640, 480);

	/* 背景色の設定 */
	SetBackColor(64, 64, 64);	
	
	/* ローカルカラーマトリックスの設定 */
	SetColorMatrix(&cmat);		

	/* プリミティブバッファの初期設定 */
	init_prim(&db[0]);
	init_prim(&db[1]);
	init_attr(attr, NCUBE);

	/* 表示開始 */
	SetDispMask(1);		

	PutDrawEnv(&db[0].draw); /* 描画環境の設定 */
	PutDispEnv(&db[0].disp); /* 表示環境の設定 */

	while (pad_read(&ncube, &ws, &ls, &lls) == 0) {

		cdb = (cdb==db)? db+1: db;	
		ClearOTagR(cdb->ot, OTSIZE);	

		/* 光源マトリックスの設定
		 * Mulmatrix() はカレントマトリクスを破壊することに注意
		 */
		PushMatrix();
		MulMatrix0(&lgtmat, &lls, &light);
		PopMatrix();
		SetLightMatrix(&light);
		
		/* 立方体のＯＴへの登録 */
		limitRange(ncube, 1, NCUBE);
		
		/* ncube 個の立方体を登録
		 * OT に登録
		 * ローカルスクリーンマトリクスの移動ベクトル成分だけ
		 * を更新していることに注意
		 */ 
		for (i = 0; i < ncube; i++) {
			RotTrans(&attr[i].trans, (VECTOR *)ls.t, &flg);
			add_cubeF4L(cdb->ot, cdb->c[i].s, &ls, attr[i].col);
		}

		VSync(0);
		ResetGraph(1);

		/* 背景クリア */
		ClearImage(&cdb->draw.clip, 60, 120, 120);

		DrawOTag(cdb->ot+OTSIZE-1);	/* 描画 */
		FntFlush(-1);			/* for debug */
	}
	DrawSync(0);
	return;
}


/* コントローラの解析と、変換マトリックスの計算 */
/* int	  *ncube	/* 表示する立方体の数 */
/* MATRIX *ws	 	/* 全体の回転マトリックス */
/* MATRIX *ls	 	/* 各立方体の回転マトリックス */
/* MATRIX *lls		/* 光源の回転マトリックス */
static int pad_read(int *ncube, 
		MATRIX *ws, MATRIX *ls, MATRIX *lls)
{
	/* Play Station では、角度を 360°= 4096 で扱います。*/
	
	/* 自分が回転するということは自分以外の世界が逆方向に回転する
	   ことと等価なことに注意 */
	 
	/* 全体の回転角度 */
	static SVECTOR	wang    = {0,  0,  0};	
	
	/* 個々の立方体の回転角度 */
	static SVECTOR	lang   = {0,  0,  0};	
	
	/* 光源の回転角度 */
	static SVECTOR	lgtang = {1024, -512, 1024};	
	
	/* 全体の平行移動ベクトル */
	static VECTOR	vec    = {0,  0,  SCR_Z};

	/* 各立方体の拡大・縮小率 */
	static VECTOR	scale  = {1024, 1024, 1024, 0};
	
	SVECTOR	dwang, dlang;
	int	ret = 0;
	u_long	padd = PadRead(0);
	
	/* 終了 */
	if (padd & PADselect) 	ret = -1;	

	/* 立方体（全体）を回転する。*/
	if (padd & PADRup)	wang.vz += 32;
	if (padd & PADRdown)	wang.vz -= 32;
	if (padd & PADRleft) 	wang.vy += 32;
	if (padd & PADRright)	wang.vy -= 32;

	/* 立方体みを回転する */
	if (padd & PADLup)	lang.vx += 32;
	if (padd & PADLdown)	lang.vx -= 32;
	if (padd & PADLleft) 	lang.vy += 32;
	if (padd & PADLright)	lang.vy -= 32;
	
	/* 原点からの距離 */
	if (padd & PADR2)	vec.vz += 8;
	if (padd & PADR1)	vec.vz -= 8;

	/* 表示する立方体の数の変更 */
	if (padd & PADL2)       (*ncube)++;
	if (padd & PADL1)	(*ncube)--;
	limitRange(*ncube, 1, NCUBE-1);

	FntPrint("objects = %d\n", *ncube);
	
	/* ワールドスクリーンマトリックスの計算 */
	RotMatrix(&wang, ws);	
	TransMatrix(ws, &vec);

	/* 個々の立方体のローカルマトリックスの計算 
	 * この場合は、立方体は同じ方向に回転するのでローカルマトリクスは同じ
	 */
	RotMatrix(&lang, ls);
	
	/* ローカルスクリーンマトリックスをつくる 
	 * MulMatrix() と MulMatrix2() の違いに注意
	 */
	MulMatrix2(ws, ls); 
	
	/* 光源の回転マトリックスの計算 */
	RotMatrix(&lgtang, lls);
	MulMatrix(lls, ls);

	/* オブジェクトのスケールはローカルマトリクスのスケールで表現できる */
	ScaleMatrix(ls, &scale);
	
	/* マトリックスの設定（全体） */
	SetRotMatrix(ws);	/* 回転マトリックス */
	SetTransMatrix(ws);	/* 平行移動ベクトル */

	return(ret);
}

#define MIN_D 	64		/* minumus distance between each cube */
#define MAX_D	(SCR_Z/2)	/* maximum distance */
/* CUBEATTR	*attr,	/* 立方体の属性 */
/* int		nattr	/* 立方体の数 */
static void init_attr(CUBEATTR *attr, int nattr)
{
	int	i;
	POLY_F4	templ;

	SetPolyF4(&templ);

	for (; nattr; nattr--, attr++) {
		for (i = 0; i < 6; i++) {
			attr->col[i].cd = templ.code;	/* sys code */
			attr->col[i].r  = rand();	/* R */
			attr->col[i].g  = rand();	/* G */
			attr->col[i].b  = rand();	/* B */
		}
		/* スタート位置の設定 */
		attr->trans.vx = (rand()/MIN_D*MIN_D)%MAX_D-(MAX_D/2);
		attr->trans.vy = (rand()/MIN_D*MIN_D)%MAX_D-(MAX_D/2);
		attr->trans.vz = (rand()/MIN_D*MIN_D)%MAX_D-(MAX_D/2);
	}
}
	
/* DB	*db;	/* プリミティブバッファ */
static init_prim(DB *db)
{
	int	i, j;

	for (i = 0; i < NCUBE; i++) 
		for (j = 0; j < 6; j++) 
			SetPolyF4(&db->c[i].s[j]);
}
