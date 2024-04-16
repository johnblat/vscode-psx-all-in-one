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
void  tuto0(void),tuto1(void),tuto2(void),tuto3(void),tuto4(void),tuto5(void);

/* メニューのタイトル */
static char *_title = "Zimen - The Ground - ";
	
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
	" tuto0   demo help : primitive divide        ",
	" tuto1   demo help : area clip               ",
	" tuto2   demo help : limitted CELL map       ",
	" tuto3   demo help : unlimitted CELL map     ",
	" tuto4   demo help : with automatic divide   ",
	" tuto5   demo help : fog                     ",
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
	0,
};

/* ユーザメッセージ。カーソルの位置に応じて、メニューの最下段に表示される */
static char *_msg[] = {
	" automatically divided POLY_FT4 ",
	" Corn clip ",
	" The ground expressed with the limited map ", 
	" The ground expressed with the lap-rounded map", 
	" Automatic division to reduce texture distortion",
	" CLUT fog ",
	0,
};

/* 実行プログラム（関数）名 */
static void (*_func[])() = {
	tuto0,
	tuto1,
	tuto2,
	tuto3,
	tuto4,
	tuto5,
};

/* ヘルプメッセージ。HELP が押された時に表示される */
static char *_help[] = {
	"tuto0: Usage",
	"   Up       move forward    ",
	"   Down     move backword   ",
	"   Right    move right      ",
	"   Left     move left       ",
	"   -------",	
	"   Triangle turn to upwrod   ",
	"   Cross    turn to downword ",
	"   Circle   turn to right    ",
	"   Square   turn to left     ",
	"   -------",	
	"   L1       move upword   ",
	"   L2       move downword ",
	"   -------",	
	"   R1       rotate right ",
	"   R2       rotate left  ",
	"   -------",	
	"   START    debug option",
	"   SELECT   exit",
	0,
	
	"tuto1: Usage",
	"   Up       move forward    ",
	"   Down     move backword   ",
	"   Right    move right      ",
	"   Left     move left       ",
	"   -------",	
	"   Triangle turn to upwrod   ",
	"   Cross    turn to downword ",
	"   Circle   turn to right    ",
	"   Square   turn to left     ",
	"   -------",	
	"   L1       move upword   ",
	"   L2       move downword ",
	"   -------",	
	"   R1       rotate right ",
	"   R2       rotate left  ",
	"   -------",	
	"   START    debug option",
	"   SELECT   exit",
	0,
	
	"tuto2: Usage",
	"   Up       move forward    ",
	"   Down     move backword   ",
	"   Right    move right      ",
	"   Left     move left       ",
	"   -------",	
	"   Triangle turn to upwrod   ",
	"   Cross    turn to downword ",
	"   Circle   turn to right    ",
	"   Square   turn to left     ",
	"   -------",	
	"   L1       move upword   ",
	"   L2       move downword ",
	"   -------",	
	"   R1       rotate right ",
	"   R2       rotate left  ",
	"   -------",	
	"   START    debug option",
	"   SELECT   exit",
	0,
	
	"tuto3: Usage",
	"   Up       move forward    ",
	"   Down     move backword   ",
	"   Right    move right      ",
	"   Left     move left       ",
	"   -------",	
	"   Triangle turn to upwrod   ",
	"   Cross    turn to downword ",
	"   Circle   turn to right    ",
	"   Square   turn to left     ",
	"   -------",	
	"   L1       move upword   ",
	"   L2       move downword ",
	"   -------",	
	"   R1       rotate right ",
	"   R2       rotate left  ",
	"   -------",	
	"   START    debug option",
	"   SELECT   exit",
	0,
	
	"tuto4: Usage",
	"   Up       move forward    ",
	"   Down     move backword   ",
	"   Right    move right      ",
	"   Left     move left       ",
	"   -------",	
	"   Triangle turn to upwrod   ",
	"   Cross    turn to downword ",
	"   Circle   turn to right    ",
	"   Square   turn to left     ",
	"   -------",	
	"   L1       move upword   ",
	"   L2       move downword ",
	"   -------",	
	"   R1       rotate right ",
	"   R2       rotate left  ",
	"   -------",	
	"   START    debug option",
	"   SELECT   exit",
	0,
	
	"tuto5: Usage",
	"   Up       turn up      ",
	"   Down     turn down    ",
	"   Right    turn  right  ",
	"   Left     turn left    ",
	"   -------",	
	"   Circle   rotate right  ",
	"   Square   rotate left   ",
	"   -------",	
	"   R1       go forword     ",
	"   R2       go backword    ",
	"   -------",	
	"   SELECT   exit",
	0,
};

/* (for GUI debug) 
void tuto0(void) {printf("tuto0...\n");}
void tuto1(void) {printf("tuto1...\n");}
void tuto2(void) {printf("tuto2...\n");}
void tuto3(void) {printf("tuto3...\n");}
void tuto4(void) {printf("tuto4...\n");}
void tuto5(void) {printf("tuto5...\n");}
*/

