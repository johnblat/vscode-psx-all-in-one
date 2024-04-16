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
void  tuto0(void), tuto1(void), tuto2(void), tuto3(void),tuto4(void);
void  tuto5(void), tuto6(void), tuto7(void), tuto8(void),tuto9(void);
void tuto10(void),tuto11(void),tuto12(void),tuto13(void);

/* メニューのタイトル */
static char *_title = "Basic Graphics Tutorial";
	
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
	" tuto0   demo help : 2D sprites            ",
	" tuto1   demo help : priority control      ",
	" tuto2   demo help : basic libgte          ",
	" tuto3   demo help : basic 3D object       ",
	" tuto4   demo help : lighting              ",
	" tuto5   demo help : texture mapping       ",
	" tuto6   demo help : depth queue (fog)     ",
	" tuto7   demo help : many objects          ",
	" tuto8   demo help : 1D BG                 ",
	" tuto9   demo help : 2D cell type BG       ", 
	" tuto10  demo help : 3D cell type BG       ", 
	" tuto11  demo help : pseudo mosaic         ", 
	" tuto12  demo help : pseudo line scroll    ", 
	" tuto13  demo help : multi-windows         ", 
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
	" simple moving sprite animation ",
	" OT (Ordering Table)  for display priority ",
	" Rotation-Translation-Perspective translation ",
	" draw plane cube ",
	" add lighting effect to [tuto3] ",
	" add texture mapping effect to [tuto4] ",
	" add depth queue effect to [tuto4] ",
	" hierachy coordiate system ",
	" scroll background plane (one dimensional) ",
	" cell type scrolling BG (two dimensional) ",
	" 3D cell type BG (bird view) ",
	" UV manipulation for mosaic ",
	" recursive mapping for line scroll ",
	" dynamic environment change ",
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
	tuto7,
	tuto8,
	tuto9,
	tuto10,
	tuto11,
	tuto12,
	tuto13,
};

/* ヘルプメッセージ。HELP が押された時に表示される */
static char *_help[] = {
	"tuto0: Usage",
	"   Up       move obj0 upper ",
	"   Down     move obj0 lower ",
	"   Left     move obj0 left  ",
	"   Right    move obj0 right ",
	"   -------",	
	"   Triangle move obj1 upper ",
	"   Cross    move obj1 lower ",
	"   Circle   move obj1 left  ",
	"   Square   move obj1 right ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto1: Usage",
	"   Up       move upper ",
	"   Down     move lower ",
	"   Left     move left  ",
	"   Right    move right ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto2: Usage",
	"   Triangle  Rotate +x angle  ",
	"   Cross     Rotate -x angle  ",
	"   Circle    Rotate -y angle  ",
	"   Squeare   Rotate +y angle  ",
	"   -------",	
	"   R1/L1     move near        ",
	"   R2/L2     move far         ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto3: Usage",
	"   Triangle  Rotate +x angle  ",
	"   Cross     Rotate -x angle  ",
	"   Circle    Rotate -y angle  ",
	"   Squeare   Rotate +y angle  ",
	"   -------",	
	"   R1/L1     move near        ",
	"   R2/L2     move far         ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto4: Usage",
	"   Up        Ligth rotate +x angle  ",
	"   Down      Light rotate -x angle  ",
	"   Right     Light rotate -y angle  ",
	"   Left      Light rotate +y angle  ",
	"   -------",	
	"   Triangle  Rotate +x angle  ",
	"   Cross     Rotate -x angle  ",
	"   Circle    Rotate -y angle  ",
	"   Squeare   Rotate +y angle  ",
	"   -------",	
	"   R1/L1     move near        ",
	"   R2/L2     move far         ",
	"   -------",	
	"   SELECT   exit",
	0,

	"tuto5: Usage",
	"   Up        Ligth rotate +x angle  ",
	"   Down      Light rotate -x angle  ",
	"   Right     Light rotate -y angle  ",
	"   Left      Light rotate +y angle  ",
	"   -------",	
	"   Triangle  Rotate +x angle  ",
	"   Cross     Rotate -x angle  ",
	"   Circle    Rotate -y angle  ",
	"   Squeare   Rotate +y angle  ",
	"   -------",	
	"   R1/L1     move near        ",
	"   R2/L2     move far         ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto6: Usage",
	"   Up        Ligth rotate +x angle  ",
	"   Down      Light rotate -x angle  ",
	"   Right     Light rotate -y angle  ",
	"   Left      Light rotate +y angle  ",
	"   -------",	
	"   Triangle  Rotate +x angle  ",
	"   Cross     Rotate -x angle  ",
	"   Circle    Rotate -y angle  ",
	"   Squeare   Rotate +y angle  ",
	"   -------",	
	"   R1/L1     move near        ",
	"   R2/L2     move far         ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto7: Usage",
	"   Up        Local rotate +x angle  ",
	"   Down      Local rotate -x angle  ",
	"   Right     Local rotate -y angle  ",
	"   Left      Local rotate +y angle  ",
	"   -------",	
	"   Triangle  Rotate +x angle  ",
	"   Cross     Rotate -x angle  ",
	"   Circle    Rotate -y angle  ",
	"   Squeare   Rotate +y angle  ",
	"   -------",	
	"   R1        move near        ",
	"   R2        move far         ",
	"   -------",	
	"   L1        incriment numbe of objects ",
	"   L2        decriment numbe of objects ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto8: Usage",
	"   Right    speed up to right direction ",
	"   Left     speed up to left direction  ",
	"   -------",	
	"   R1/R2    clear near landscape        ",
	"   L1/L2    clear train window          ",
	"   -------",	
	"   SELECT   exit",
	0,
		
	"tuto9: Usage",
	"   R1       select BG0  ",
	"   R2       select BG1  ",
	"   L1       select BG2  ",
	"   L2       select BG3  ",
	"   -------",	
	"   Up       move upper  ",
	"   Down     move lower  ",
	"   Right    move right  ",
	"   Left     move left   ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto10: Usage",
	"   Up        Move forward  ",
	"   Down      Move backword ",
	"   Right     Move right    ",
	"   Left      Move left     ",
	"   -------",	
	"   Triangle  Rotate +x angle  ",
	"   Cross     Rotate -x angle  ",
	"   Circle    Rotate -y angle  ",
	"   Squeare   Rotate +y angle  ",
	"   -------",	
	"   SELECT   exit",
	0,
	
	"tuto11: Usage",
	"   Up        Move upper ",
	"   Down      Move lower ",
	"   Right     Move right ",
	"   Left      Move left  ",
	"   -------",	
	"   Triangle  mosic rate up   ",
	"   Cross     mosic rate down ",
	"   -------",	
	"   SELECT   exit",
	0,

	"tuto12: Usage",
	"   Up        Incriment number of balls ",
	"   Down      Decriment number of balls ",
	"   Right     line scroll rate up       ",
	"   Left      line scroll rate down     ",
	"   -------",	
	"   SELECT   exit",
	0,

	"tuto13: Usage",
	"   Up        Incriment number of balls ",
	"   Down      Decriment number of balls ",
	"   Triangle  Incriment number of windows ",
	"   Cross     Decriment number of windows ",
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
void tuto7(void) {printf("tuto7...\n");}
void tuto8(void) {printf("tuto8...\n");}
void tuto9(void) {printf("tuto9...\n");}
void tuto10(void){printf("tuto10..\n");}
void tuto11(void){printf("tuto11..\n");}
void tuto12(void){printf("tuto12..\n");}
void tuto13(void){printf("tuto13..\n");}
*/
