/* $PSLibId: Run-time Library Release 4.4$ */
/*			    tuto1
 */			
/* TMD ビューアプロトタイプ（光源計算なし  FT3 型） */
/*		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------	
 *	1.00		Mar,15,1994	suzu
 *	1.10		Jan,22,1996	suzu	(English comment)
 *	1.20		Mar,07,1997	sachiko	(added autopad)
 */

#include <sys/types.h>	
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include "rtp.h"

/* コントローラレコーダ用マクロ */
#define PadRead(x)	myPadRead(x)

/* ３角形ポリゴンの頂点情報 */
typedef struct {
	SVECTOR	n0;		/* 法線 */
	SVECTOR	v0, v1, v2;	/* 頂点 */
} VERT_F3;

/* ３角形ポリゴンパケット
 * 頂点・パケットバッファは、malloc() で動的に割り付けるべきです。	
 */
#define MAX_POLY	400			/* max polygon (per object) */

/* オブジェクトの定義 */
typedef struct {
	MATRIX		m;	/* ローカルマトリクス */
	MATRIX		lw;	/* ローカルマトリクス */
	int		n;	/* ポリゴン数 */
	VERT_F3		vert[MAX_POLY];		/* vertex */
	POLY_FT3	prim[2][MAX_POLY];	/* primitives (2 set) */
} OBJ_FT3;

#define SCR_Z		1024	/* 投影面までの距離 */
#define OTSIZE		4096	/* ＯＴの段数（２のべき乗） */
#define MAX_OBJ		9	/* オブジェクトの個数 */

#define MODELADDR	((u_long *)0x80120000)	/* TMD address */
#define TEXADDR		((u_long *)0x80140000)	/* TIM address */

static int pad_read(OBJ_FT3 *obj, int nobj, MATRIX *m);
static void set_position(OBJ_FT3 *obj, int n);
static void add_OBJ_FT3(MATRIX *ws, u_long *ot, OBJ_FT3 *obj, int id);
static void loadTIM(u_long *addr);
static void setSTP(u_long *col, int n);
static int loadTMD_FT3(u_long *tmd, OBJ_FT3 *obj);

void tuto1(void)
{
	static SVECTOR	ang = {0,0,0};	/* 自転角 */
	MATRIX	rot;			/* 自転 */
	MATRIX	ws;			/* word-screen matrix */
	OBJ_FT3	obj[MAX_OBJ];		/* オブジェクト */
	u_long	ot[2][OTSIZE];		/* ＯＴ バッファ*/
	int	nobj = 1;		/* オブジェクトの個数 */
	int	id   = 0;		/* パケット ID */
	int	i, n; 			/* work */
	
	/* ダブルバッファの初期化 */
	db_init(640, 480/*240*/, SCR_Z, 60, 120, 120);	
	
	/* TIM をフレームバッファに転送する */
	loadTIM(TEXADDR);	

	/* TMD をばらばらにして読み込む */
	for (i = 0; i < MAX_OBJ; i++) 
		loadTMD_FT3(MODELADDR, &obj[i]);	
	
	/* オブジェクト位置をレイアウト */
	set_position(obj, 0);		

	/* 表示開始 */
	SetDispMask(1);			
	
	/* メインループ */
	while ((nobj = pad_read(obj, nobj, &ws)) != -1) {
		
		/* パケットバッファ IDをスワップ */
		id = id? 0: 1;
		
		/* ＯＴをクリア */
		ClearOTagR(ot[id], OTSIZE);			

		/* 地球を自転させる */
		ang.vy += 32;
		RotMatrix(&ang, &rot);
		
		/* プリミティブの設定 */
		for (n = i = 0; i < nobj; i++) {
			MulMatrix0(&obj[i].m, &rot, &obj[i].lw);
			add_OBJ_FT3(&ws, ot[id], &obj[i], id);
		}
		
		/* デバッグストリングの設定 */
		FntPrint("objects=%d\n", nobj);
		FntPrint("total  =%d\n", obj[0].n*nobj);
		
		/* ダブルバッファのスワップとＯＴ描画 */
		db_swap(&ot[id][OTSIZE-1]);
	}
	DrawSync(0);
	return;
}


/* パッドの読み込み
 * 回転の中心位置と回転角度を読み込みワールドスクリーンマトリクスを設定
 * 	
 */	
static int pad_read(OBJ_FT3 *obj, int nobj, MATRIX *m)
{
	static SVECTOR	ang   = {0, 512, 512};
	static VECTOR	vec   = {0, 0, SCR_Z*3/2};
	static int	scale = ONE;
	static int	opadd = 0;
	
	VECTOR	svec;
	int 	padd = PadRead(1); 

	if (padd & PADselect)	return(-1);
	if (padd & PADRup)	ang.vx -= 8;
	if (padd & PADRdown)	ang.vx += 8;
	if (padd & PADRright)	ang.vy -= 8;
	if (padd & PADRleft) 	ang.vy += 8;
	if (padd & PADR1) 	ang.vz += 8;
	if (padd & PADR2)	ang.vz -= 8;
	
	if (padd & PADL1)	vec.vz += 8;
	if (padd & PADL2) 	vec.vz -= 8;
	
	if ((opadd==0) && (padd & PADLup))	set_position(obj, nobj++);
	if ((opadd==0) && (padd & PADLdown))	nobj--;
	
	limitRange(nobj, 1, MAX_OBJ-1);
	opadd = padd;
	
	setVector(&svec, scale, scale, scale);
	RotMatrix(&ang, m);	
	
#ifdef ROTATE_YORSELF	/* 主観視（自分が動く) */
	{
		VECTOR	vec2;
		ApplyMatrixLV(m, &vec, &vec2);
		TransMatrix(m, &vec2);
		dumpVector("vec2=", &vec2);
	}
#else			/* 客観視（世界が動く) */
	TransMatrix(m, &vec);	
#endif
	SetRotMatrix(m);
	SetTransMatrix(m);

	return(nobj);
}

/* オブジェクトをワールド座標系にレイアウト
 * ここでは乱数を使用して適当に配置している	
 * そのため、二つのオブジェクトが同じ位置にぶつかることもある。	
 */	
#define UNIT	512		/* 最小解像度 */

static void set_position(OBJ_FT3 *obj, int n)
{
	SVECTOR	ang;

	static loc_tab[][3] = {
		 0, 0, 0,
		 1, 0, 0,	0, 1, 0,	 0, 0, 1,
		-1, 0, 0,	0,-1, 0,	 0, 0,-1,
		 1, 1, 0,	0, 1, 1,	 1, 0, 1,
		-1,-1, 0,	0,-1,-1,	-1, 0,-1,
		 1,-1, 0,	0,-1, 1,	-1, 0, 1,
		-1, 1, 0,	0, 1,-1,	 1, 0,-1,
	};
	
	/* 各地球の地軸を設定 */
	ang.vx = rand()%4096;
	ang.vy = rand()%4096;
	ang.vz = rand()%4096;
	RotMatrix(&ang, &obj[n].m);	
	
	/* 各地球の場所を設定 */
	obj[n].lw.t[0] = loc_tab[n][0]*UNIT;
	obj[n].lw.t[1] = loc_tab[n][1]*UNIT;
	obj[n].lw.t[2] = loc_tab[n][2]*UNIT;
}

/* オブジェクトの登録 */
static void add_OBJ_FT3(MATRIX *ws, u_long *ot, OBJ_FT3 *obj, int id)
{
	MATRIX		ls;		/* local-screen matrix */
	VERT_F3		*vp;		/* work */
	POLY_FT3	*pp;		/* work */
	int		i;
	long		flg;
	
	vp = obj->vert;
	pp = obj->prim[id];
		
	/* カレントマトリクスを退避 */
	PushMatrix();				

	/* ローカルスクリーンマトリクスを作る */
	CompMatrix(ws, &obj->lw, &ls);

	/* ローカルスクリーンマトリクスを設定 */
	SetRotMatrix(&ls);		/* set matrix */
	SetTransMatrix(&ls);		/* set vector */
	
	/* rotTransPers3 はマクロ。rtp.h を参照のこと */
	for (i = 0; i < obj->n; i++, vp++, pp++) {
		rotTransPers3(ot, OTSIZE, pp,
			      &vp->v0, &vp->v1, &vp->v2);
	}
	
	/* マトリクスを元にもどしてリターン */
	PopMatrix();
}


/* TIM をロードする */	
static void loadTIM(u_long *addr)
{
	TIM_IMAGE	image;		/* TIM header */
	
	OpenTIM(addr);			/* open TIM */
	while (ReadTIM(&image)) {
		if (image.caddr) {	/* load CLUT (if needed) */
			setSTP(image.caddr, image.crect->w);
			LoadImage(image.crect, image.caddr);
		}
		if (image.paddr) 	/* load texture pattern */
			LoadImage(image.prect, image.paddr);
	}
}
	
/* 透明色処理を禁止するために STP bit を 1 にする */	
static void setSTP(u_long *col, int n)
{
	n /= 2;  
	while (n--) 
		*col++ |= 0x80008000;
}

/* TMD の解析 */
static int loadTMD_FT3(u_long *tmd, OBJ_FT3 *obj)
{
	VERT_F3		*vert;
	POLY_FT3	*prim0, *prim1;
	TMD_PRIM	tmdprim;
	int		col, i, n_prim = 0;

	vert  = obj->vert;
	prim0 = obj->prim[0];
	prim1 = obj->prim[1];
	
	/* TMD のオープン */
	if ((n_prim = OpenTMD(tmd, 0)) > MAX_POLY) 
		n_prim = MAX_POLY;
	
	/* メモリライトアクセスを減らすためにプリミティブバッファのうち、
	 * 書き換えないものをあらかじめ設定しておく
	 */	 
	for (i = 0; i < n_prim && ReadTMD(&tmdprim) != 0; i++) {

		/* プリミティブの初期化 */
		SetPolyFT3(prim0);

		/* 法線と頂点ベクトルをコピーする */
		copyVector(&vert->n0, &tmdprim.n0);
		copyVector(&vert->v0, &tmdprim.x0);
		copyVector(&vert->v1, &tmdprim.x1);
		copyVector(&vert->v2, &tmdprim.x2);
		
		/* 光源計算（最初の一回のみ）*/
		col = (tmdprim.n0.vx+tmdprim.n0.vy)*128/ONE/2+128;
		setRGB0(prim0, col, col, col);
		
		/* テクスチャ座標は変わらないのでここでコピーしておく */
		setUV3(prim0,
		       tmdprim.u0, tmdprim.v0,
		       tmdprim.u1, tmdprim.v1,
		       tmdprim.u2, tmdprim.v2);
		
		/* テクスチャページ／テクスチャ CLUT ID をコピー */
		prim0->tpage = tmdprim.tpage;
		prim0->clut  = tmdprim.clut;

		/* ダブルバッファを使用するのでプリミティブの複製をもう
		 * 一組つくっておく
		 */  
		memcpy(prim1, prim0, sizeof(POLY_FT3));
		
		vert++, prim0++, prim1++;
	}
	return(obj->n = i);
}
