/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *		
 *		"control.h"
 */

#define CTLMAX 	10	/* 制御波形の最大チャンネル数 */

#define CTLTIME 32	/* 伝達関数波形の最大長さ */

/* 制御波形生成プログラム用構造体 */
typedef struct {
	int  *cnv;	/* 伝達関数波形データのポインタ */

	int num;	/* 伝達関数波形データの長さ */

	int in;		/* 入力 */
	int out;	/* 出力 */

} CTLFUNC;
