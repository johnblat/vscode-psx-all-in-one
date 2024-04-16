/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *		Copyright (C) 1994-1997 by Sony Corporation
 *			All rights Reserved
 *		
 *		"control.h"
 */

#define CTLMAX 	16	/* 制御波形の最大チャンネル数 */

#define CTLTIME 32	/* 伝達関数波形の最大長さ */

/* 制御波形生成プログラム用構造体 */
typedef struct {
	int  *cnv;	/* 伝達関数波形データのポインタ */

	int num;	/* 伝達関数波形データの長さ */

	int in;		/* 入力 */
	int out;	/* 出力 */

} CTLFUNC;

extern void	init_cntrlarry(int cnvwave[], int number,
			CTLFUNC ctlfc[CTLMAX], int cntrlarry[CTLMAX][CTLTIME]);
extern void	set_cntrl(u_long frm,
			CTLFUNC ctlfc[CTLMAX], int cntrlarry[CTLMAX][CTLTIME]);
