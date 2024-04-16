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
void	tuto0(void),tuto1(void),tuto2(void),tuto3(void);
void	tuto4(void),tuto5(void),tuto6(void);

/* メニューのタイトル */
static char *_title = "inside TMD and PMD ";
	
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
	" tuto0   demo help : draw TMD                ",
	" tuto1   demo help : draw many TMDs          ",
	" tuto2   demo help : draw PMD		      ",	
	" tuto3   demo help : from TMD to PMD         ",	
	" tuto4   demo help : draw many TMDs as PMD   ",
	" tuto5   demo help : lighting on PMD         ",
	" tuto6   demo help : dmpsx PMD               ",
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
	" draw POLY_F3 TMD data ",
	" draw many TMDs located on local coordinates ",
	" draw PMD data ",
	" convert TMD to PMD",
	" draw many PMD",
	" add lighting effect on PMD ",
	" inside of PMD handler (DMPSX version) ",
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
	tuto6,
};

/* ヘルプメッセージ。HELP が押された時に表示される */
static char *_help[] = {
	"tuto0: Usage",
	"   Up       increment number of polygons  ",
	"   Down     decrement number of polygons  ",
	"   -------",	
	"   Triangle rotate -x  ",
	"   Cross    rotate +x  ",
	"   Circle   rotate -y  ",
	"   Square   rotate +y  ",
	"   -------",	
	"   L1       move far   ",
	"   L2       move near ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto1: Usage",
	"   Up       increment number of objects  ",
	"   Down     decrement number of objects  ",
	"   -------",	
	"   Triangle rotate -x  ",
	"   Cross    rotate +x  ",
	"   Circle   rotate -y  ",
	"   Square   rotate +y  ",
	"   -------",	
	"   L1       move far   ",
	"   L2       move near ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto2: Usage",
	"   Triangle rotate -x  ",
	"   Cross    rotate +x  ",
	"   Circle   rotate -y  ",
	"   Square   rotate +y  ",
	"   -------",	
	"   L1       move far   ",
	"   L2       move near ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto3: Usage",
	"   Up       increment number of polygons  ",
	"   Down     decrement number of polygons  ",
	"   -------",	
	"   Triangle rotate -x  ",
	"   Cross    rotate +x  ",
	"   Circle   rotate -y  ",
	"   Square   rotate +y  ",
	"   -------",	
	"   L1       move far   ",
	"   L2       move near ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto4: Usage",
	"   Up       increment number of objects  ",
	"   Down     decrement number of objects  ",
	"   -------",	
	"   Triangle rotate -x  ",
	"   Cross    rotate +x  ",
	"   Circle   rotate -y  ",
	"   Square   rotate +y  ",
	"   -------",	
	"   L1       move far   ",
	"   L2       move near ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto5: Usage",
	"   Up       light rotate -x ",
	"   Down     light rotate +x ",
	"   Right    light rotate -y ",
	"   Left     light rotate +y ",
	"   -------",	
	"   Triangle rotate -x  ",
	"   Cross    rotate +x  ",
	"   Circle   rotate -y  ",
	"   Square   rotate +y  ",
	"   -------",	
	"   L1       move far   ",
	"   L2       move near ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto5: Usage",
	"   Up       Change mode upword   ",
	"   Down     Change mode downword ",
	"   -------",	
	"   Triangle rotate -x  ",
	"   Cross    rotate +x  ",
	"   Circle   rotate -y  ",
	"   Square   rotate +y  ",
	"   -------",	
	"   L1       move far   ",
	"   L2       move near ",
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
void tuto6(void) {printf("tuto6...\n");}
*/


