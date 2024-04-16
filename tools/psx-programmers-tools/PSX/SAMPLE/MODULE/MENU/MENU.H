/*
 * $PSLibId: Run-time Library Release 4.4$
 */
#ifndef MENU_INCLUDED
#define MENU_INCLUDED

#define	MAX_DIRDEPTH	10

/* メニュー構造体 */
typedef struct {
	char 	str[81];			/* 名前 */
	char	root[81];			/* ルートパス */
	char	fname[MAX_DIRDEPTH][81];	/* file name */
	u_short	kind[MAX_DIRDEPTH];		/* & 0x7fff -> kind */
						/* & 0x8000 -> flag */
	int	depth;		/* ファイル名の階層の深さ */
	int	ndata;		/* データの数 */
	u_long  daddr[16];	/* データのアドレス */
	char	dname[16][81];	/* データのパス */
	char	sentj[161];	/* the explanation in Japanese */
	char	sente[161];	/* the explanation in English */
} MENU;

typedef struct {
	int	top;	/* 表示アイテムの先頭 */
	int	no;	/* 現在指しているアイテム No.*/
	int	cnt;	/* 表示するアイテムの数 */
	short	item[100];	/* メニューアイテム中の表示するアイテム */
} DISP_MENU;

#endif
