/* $PSLibId: Run-time Library Release 4.4$ */
/*				db.c
 *			
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Mar,15,1994	suzu
 *		1.10		Jun,19,1995	suzu
*/
/* ダブルバッファハンドラ */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>


/*
 * Setup frame double buffer
 *	(0,0)-(xxx, 240), (0, 240)-(xxx, 480)	(xxx = 256/320/512/640)	
 *	(0,0)-(xxx, 480), (0,   0)-(xxx, 480)	(xxx = 256/320/512/640)	
 */	
static int	id = 0;		
static DRAWENV	draw[2];	
static DISPENV	disp[2];	

/* ダブルバッファのための描画・表示環境を初期化する */
/* 描画・表示エリア */
/* プロジェクション */
/* 背景色 */

void db_init(int w, int h, int z, u_char r0, u_char g0, u_char b0)
{
	/* インターレースでない場合はダブルバッファする。 */	 
	int	oy = (h==480)? 0: 240;
	
	PadInit(0);			
	ResetGraph(0);			
	InitGeom();			

	/* 原点を画面中央にする */
	SetGeomOffset(w/2, h/2);	

	/* 投影面までの距離 */
	SetGeomScreen(z);	
	
	/* フレームバッファ上でダブルバッファを構成するための描画環境・
	 * 表示環境構造体のメンバを設定する。
	 */	 
	SetDefDrawEnv(&draw[0], 0,  0, w, h);
	SetDefDispEnv(&disp[0], 0, oy, w, h);
	SetDefDrawEnv(&draw[1], 0, oy, w, h);
	SetDefDispEnv(&disp[1], 0,  0, w, h);

	/* isbg = 1 にすると PutDrawEnv() の時点で描画エリアがクリアされる */
	draw[0].isbg = draw[1].isbg = 1;
	setRGB0(&draw[0], r0, g0, b0);
	setRGB0(&draw[1], r0, g0, b0);

	/* フォント表示環境を設定 (VRAM アドレス=(960,256)) */
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(32, 32, 0, 0, 2, 1024));
}

/* ダブルバッファをスワップする */
void db_swap(u_long *ot)
{
	id = id? 0: 1;			/* swap id */
	if (draw[id].dfe) {
		DrawSync(0);		/* sync GPU */
		VSync(0);		/* wait for V-BLNK */
	}
	else {
		VSync(0);		/* wait for V-BLNK */
		ResetGraph(1);		/* reset GPU */
	}
	PutDrawEnv(&draw[id]);		/* update DRAWENV */
	PutDispEnv(&disp[id]);		/* update DISPENV */
	DrawOTag(ot);			/* draw OT */
	FntFlush(-1);			/* flush debug message */
}

/* ワールドスクリーンマトリクスを表示のアスペクト比に応じて補正する */
void db_set_matrix(MATRIX *m)
{
	/* adjust x */
	m->m[0][0] = m->m[0][0]*disp[0].disp.w/640;
	m->m[0][1] = m->m[0][1]*disp[0].disp.w/640;
	m->m[0][2] = m->m[0][2]*disp[0].disp.w/640;
		
	/* adjust y */
	m->m[1][0] = m->m[1][0]*disp[0].disp.h/480;
	m->m[1][1] = m->m[1][1]*disp[0].disp.h/480;
	m->m[1][2] = m->m[1][2]*disp[0].disp.h/480;

	/* set matrix to GTE */
	SetRotMatrix(m);
	SetTransMatrix(m);
}
