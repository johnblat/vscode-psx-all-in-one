/* $PSLibId: Run-time Library Release 4.4$ */
/*	mime
 *
 *	メモリ割り当て
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment
 *		All rights Reserved
 */

/* モデリングデータ（TMDフォーマット） がおかれるアドレス */
#define MODEL_ADDR (u_long *)0x800d0000

/* オリジナルの頂点データが退避されるアドレス */
#define ORGVTXBUF (u_long *)0x80050000	

/* オリジナルの法線データが退避されるアドレス */	
#define ORGNRMBUF (u_long *)0x80060000	

/* 頂点差分データ（VDFフォーマット） がおかれるアドレス */
#define MDFDATAVTX (u_long *)0x80090000	

/* 法線差分データ（NDFフォーマット） がおかれるアドレス */
#define MDFDATANRM (u_long *)0x800b0000	

/* パケットデータを置くアドレス */
#define PACKETBUF  (u_long *)0x80100000
