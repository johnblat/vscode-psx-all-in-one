/* $PSLibId: Run-time Library Release 4.4$ */
/*			    tuto0
*/			
/* 動画をオブジェクトの表面にマップする */
/*		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	----------------------------------------------------	
 *	1.00		Jul,14,1994	yutaka
 *	1.10		Sep,01,1994	suzu
 *	2.00		Feb,8,1996	suzu	(rewrite)
 */

#include <sys/types.h>	
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define FILE_NAME   "\\XDATA\\STR\\MOV.STR;1"

/* ３角形ポリゴンパケット
 * 頂点・パケットバッファは、malloc() で動的に割り付けるべきです。	
 */
#define MAX_POLY	512			/* max polygon (per object) */

/* オブジェクトの定義 */
typedef struct {
	MATRIX		m;	/* ローカルマトリクス */
	MATRIX		lw;	/* ローカルマトリクス */
	SVECTOR		v;	/* ローカルベクトル */
	int		n;	/* ポリゴン数 */
	struct {		/* ポリゴンの情報 */
		POLY_FT3	prim[2];
		SVECTOR		v0, v1, v2;
	} p[MAX_POLY];
} OBJ_FT3;

#define SCR_Z		1024		/* 投影面までの距離 */
#define OTLENGTH	12		/* ＯＴの段数 */
#define OTSIZE		(1<<OTLENGTH)	/* ＯＴの段数 */
#define MAX_OBJ		16		/* オブジェクトの個数 */

#define MODELADDR	((u_long *)0x80120000)	/* TMD address */
#define TEXADDR		((u_long *)0x80140000)	/* TIM address */

static int pad_read(OBJ_FT3 *obj, int nobj, MATRIX *m);
static void set_position(OBJ_FT3 *obj, int n);
static void add_OBJ_FT3(MATRIX *ws, u_long *ot, OBJ_FT3 *obj, int id);
static void loadTIM(u_long *addr);
static void setSTP(u_long *col, int n);
static int loadTMD_FT3(u_long *tmd, OBJ_FT3 *obj);

int main( void )
{
	static SVECTOR	ang = {0,0,0};	/* 自転角 */
	MATRIX	rot;			/* 自転角 */
	MATRIX	ws;			/* ワールドマトリクス */
	OBJ_FT3	obj[MAX_OBJ];		/* オブジェクト */
	u_long	ot[2][OTSIZE];		/* ＯＴ バッファ*/
	int	nobj = 1;		/* オブジェクトの個数 */
	int	id   = 0;		/* パケット ID */
	int	i, n; 			/* work */
	RECT	rect;			/* work */
	
	/* init CD */
	CdInit();
	
	/* ダブルバッファの初期化 */
	db_init(640, /*480*/240, SCR_Z, 60, 120, 120);	

	/* TIM をフレームバッファに転送する */
	loadTIM(TEXADDR);	

	/* TMD をばらばらにして読み込む */
	for (i = 0; i < MAX_OBJ; i++) 
		loadTMD_FT3(MODELADDR, &obj[i]);	
	
	/* オブジェクト位置をレイアウト */
	set_position(obj, 0);		
	
	/* clear texture buffer */
	setRECT(&rect, 640, 0, 256, 256);
	ClearImage(&rect, 64, 64, 64);

	/* start animation */
	animInit(FILE_NAME, 640, 32);
	
	/* 表示開始 */
	SetDispMask(1);			
	
	/* メインループ */
	while ((nobj = pad_read(obj, nobj, &ws)) != -1) {
		
		/* pollling animation */
		animPoll();
		
		/* パケットバッファ IDをスワップ */
		id = id? 0: 1;
		
		/* ＯＴをクリア */
		ClearOTagR(ot[id], OTSIZE);			

		/* 地球を自転させる */
		ang.vy += 32;
		RotMatrix(&ang, &rot);
		
		/* プリミティブの設定 */
		for (i = 0; i < nobj; i++) {
			MulMatrix0(&obj[i].m, &rot, &obj[i].lw);
			add_OBJ_FT3(&ws, ot[id], &obj[i], id);
		}
		
		/* デバッグストリングの設定 */
		FntPrint("polygon=%d\n", obj[0].n);
		FntPrint("objects=%d\n", nobj);
		FntPrint("total  =%d\n", obj[0].n*nobj);
		
		/* ダブルバッファのスワップとＯＴ描画 */
		db_swap(&ot[id][OTSIZE-1]);
	}
	PadStop();
	StopCallback();
	return 0;
}

/* パッドの読み込み
 * 回転の中心位置と回転角度を読み込みワールドスクリーンマトリクスを設定
 * 	
 */	
static int pad_read(OBJ_FT3 *obj, int nobj, MATRIX *m)
{
	static SVECTOR	ang   = {0, 0, 0};
	static VECTOR	vec   = {0, 0, SCR_Z*3};
	static int	scale = ONE;
	static int	opadd = 0;
	
	VECTOR	svec;
/*	int 	padd = PadRead(1);*/
	int 	padd;

	padd = PadRead(1);
	
	if (padd & PADk)	return(-1);
	if (padd & PADRup)	ang.vx += 8;
	if (padd & PADRdown)	ang.vx -= 8;
	if (padd & PADRleft) 	ang.vy += 8;
	if (padd & PADRright)	ang.vy -= 8;
	if (padd & PADL1)	vec.vz += 8;
	if (padd & PADL2) 	vec.vz -= 8;
	
	if ((opadd==0) && (padd & PADLup))	set_position(obj, nobj++);
	if ((opadd==0) && (padd & PADLdown))	nobj--;
	
	limitRange(nobj, 1, MAX_OBJ);
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
	/* ワールドスクリーンマトリクスをアスペクト比補正をした後に設定 */
	db_set_matrix(m);
	/*
	SetRotMatrix(m);
	SetTransMatrix(m);
	*/
	return(nobj);
}


/* オブジェクトをワールド座標系にレイアウト
 * ここでは乱数を使用して適当に配置している	
 * そのため、二つのオブジェクトが同じ位置にぶつかることもある。	
 */	
#define UNIT	400		/* 最小解像度 */

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
	MATRIX	ls;		/* local-screen matrix */
	
	/* カレントマトリクスを退避 */
	PushMatrix();				

	/* ローカルスクリーンマトリクスを作る */
	CompMatrix(ws, &obj->lw, &ls);

	/* ローカルスクリーンマトリクスを設定 */
	SetRotMatrix(&ls);		/* set matrix */
	SetTransMatrix(&ls);		/* set vector */
	
	/* 回転・移動・透視変換 */
	RotPMD_FT3((long *)&obj->n, ot, OTLENGTH, id, 0);

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
	TMD_PRIM	tmdprim;
	int		col, i, n_prim = 0;

	/* TMD のオープン */
	if ((n_prim = OpenTMD(tmd, 0)) > MAX_POLY) 
		n_prim = MAX_POLY;
	
	/* メモリライトアクセスを減らすためにプリミティブバッファのうち、
	 * 書き換えないものをあらかじめ設定しておく
	 */	 
	for (i = 0; i < n_prim && ReadTMD(&tmdprim) != 0; i++) {

		/* プリミティブの初期化 */
		SetPolyFT3(&obj->p[i].prim[0]);

		/* 法線と頂点ベクトルをコピーする */
		copyVector(&obj->p[i].v0, &tmdprim.x0);
		copyVector(&obj->p[i].v1, &tmdprim.x1);
		copyVector(&obj->p[i].v2, &tmdprim.x2);
		
		/* 光源計算（最初の一回のみ）*/
		col = (tmdprim.n0.vx+tmdprim.n0.vy)*128/ONE/2+128;
		setRGB0(&obj->p[i].prim[0], col, col, col);
		
		/* テクスチャ座標は変わらないのでここでコピーしておく */
		setUV3(&obj->p[i].prim[0], 
		       tmdprim.u0, tmdprim.v0,
		       tmdprim.u1, tmdprim.v1,
		       tmdprim.u2, tmdprim.v2);
		
		/* テクスチャページ／テクスチャ CLUT ID をコピー */
		obj->p[i].prim[0].tpage = GetTPage(2, 0, 640, 0);
		/*
		obj->p[i].prim[0].tpage = tmdprim.tpage;
		obj->p[i].prim[0].clut  = tmdprim.clut;
		*/
		/* ダブルバッファを使用するのでプリミティブの複製をもう
		 * 一組つくっておく
		 */  
		memcpy(&obj->p[i].prim[1],
		       &obj->p[i].prim[0], sizeof(POLY_FT3));
		
	}
	return(obj->n = i);
}
