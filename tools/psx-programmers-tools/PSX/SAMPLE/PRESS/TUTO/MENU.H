/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*				menu
 *
 *			Simple Menu Database
 *
 *		Copyright (C) 1997 by Sony Corporation
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	0.10		Jul,15,1997	suzu
 */

/* メニューから起動するプログラム（関数）リスト */
void  tuto1(void), tuto2(void), tuto3(void),tuto4(void);
void  tuto5(void), tuto6(void), tuto7(void);

/* メニューのタイトル */
static char *_title = "Movie Tutorial";
	
/* 起動するプログラムのリスト
 *=============================================================
 *	colum	contents		 :; 内容
 *------------------------------------------------------------
 *	 0-8	program name		 :; プログラム名		
 *	 9-13	execututtion button name :; 実行ボタン名	
 *	14-17	help button name	 :; ヘルプボタン名
 *	18-	description		 :; 詳細
 *------------------------------------------------------------
 */
static char *_list[] = {
	" tuto1   demo help : VLC+MDEC                 ",	
	" tuto2   demo help : Piplined LoadImage       ",	
	" tuto3   demo help : On-memory movie          ",
	" tuto4   demo help : callback                 ",
	" tuto5   demo help : callback chain           ",
	" tuto6   demo help : piplined on-memory movie ",
	" tuto7   demo help : tune-up parametes        ",
	0,
};

/* カーソルが停止する位置。数字はそこの位置のカーソルの幅を表す */
static char *_attr[] = {
	"         4    4 ",
	"         4    4 ",
	"         4    4 ",
	"         4    4 ",
	"         4    4 ",
	"         4    4 ",
	"         4    4 ",
	0,
};

/* ユーザメッセージ。カーソルの位置に応じて、メニューの最下段に表示される */
static char *_msg[] = {
	"simple VLC decode and MDEC on memory decompression",
	"paralell execution of LoadImage() and DecDCTout()",
	"simple on-memory movie operation",
	"handshake using callback",
	"parallel execution using callback",
	"complete background on-memory movie decompression",
	"fine tune-up for parameters",
	0,
};

/* 実行プログラム（関数）名 */
static void (*_func[])() = {
	tuto1,
	tuto2,
	tuto3,
	tuto4,
	tuto5,
	tuto6,
	tuto7,
};

/* ヘルプメッセージ。HELP が押された時に表示される */
static char *_help[] = {
	
	"tuto1: Usage",
	"   SELECT   exit    ",
	0,
	
	"tuto2: Usage",
	"   SELECT   exit    ",
	0,
	
	"tuto3: Usage",
	"   SELECT   exit    ",
	0,
	
	"tuto4: Usage",
	"   SELECT   exit    ",
	0,

	"tuto5: Usage",
	"   SELECT   exit    ",
	0,
	
	"tuto6: Usage",
	"   SELECT   exit    ",
	0,
	
	"tuto7: Usage",
	"   Up        Incriment Y[0,0]      ",
	"   Down      Decriment Y[0,0]      ",
	"   Right     Incriment Cb/Cr[0,0]  ",
	"   Left      Decriment Cb/Cr[0,0]  ",
	"   -------",	
	"   SELECT   exit",
	0,
};

/* (for GUI debug) 
void tuto1(void) {printf("tuto1...\n");}
void tuto2(void) {printf("tuto2...\n");}
void tuto3(void) {printf("tuto3...\n");}
void tuto4(void) {printf("tuto4...\n");}
void tuto5(void) {printf("tuto5...\n");}
void tuto6(void) {printf("tuto6...\n");}
void tuto7(void) {printf("tuto7...\n");}
*/
