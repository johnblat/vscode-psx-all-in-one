/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/* --------------------------------------------------------------------
		GetODE() Sample Program

        balls     suzu
        97/01/23  stanaka  vsynccallback, inter-race, getode


		PADLup  : ボールを１個増やす
		PADLdown: ボールを１個減らす

		ボールを増やしていくと CPU ネックで 30 fps になる。
		続けていくと GPU ネックが発生するが、考慮されてい
		ないので画面が乱れる。
   -------------------------------------------------------------------- */


#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define OTSIZE		1
#define MAXOBJ		7000
#define	FRAME_X		640
#define	FRAME_Y		480
#define WALL_X		(FRAME_X-16)
#define WALL_Y		(FRAME_Y-16)

typedef struct {
	DRAWENV		draw;
	DISPENV		disp;
	u_long		ot[OTSIZE];
	SPRT_16		sprt[MAXOBJ];
} DB;

volatile u_long VsyncWait, Frame;
DB	*Cdb, Db[2];

typedef struct {
	u_short x, y;
	u_short dx, dy;
} POS;

void init_prim(DB *);
int  pad_read(int);
void cbvsync(void);
void init_point(POS *);

main(){
	POS	pos[MAXOBJ];
	int	nobj = 3000;
	u_long	*ot;
	SPRT_16	*sp;
	POS	*pp;
	int	i, x, y;
	u_long sync = 0;

	ResetCallback();
	PadInit(0);
	ResetGraph(0);
	SetGraphDebug(0);

	SetDefDrawEnv(&Db[0].draw, 0, 0, FRAME_X, FRAME_Y);
	SetDefDrawEnv(&Db[1].draw, 0, 0, FRAME_X, FRAME_Y);
	SetDefDispEnv(&Db[0].disp, 0, 0, FRAME_X, FRAME_Y);
	SetDefDispEnv(&Db[1].disp, 0, 0, FRAME_X, FRAME_Y);

	KanjiFntOpen(160, 16, 256, 200, 704, 0, 768, 256, 0, 512);
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));

	init_prim(&Db[0]);
	init_prim(&Db[1]);

	/* CPU ネックに備えて両フレームの OT を初期化しておく */
	ClearOTag(Db[0].ot, OTSIZE);
	ClearOTag(Db[1].ot, OTSIZE);

	init_point(pos);

	SetDispMask(1);

	VSync(0);					/* V-SYNC タイミング用 */
	Frame = !GetODE();			/* GetODE() 判定初期値 */

	/* V-SYNC 時のコールバック関数の設定 */
	VSyncCallback(cbvsync);

	/* メインループ*/
	while ((nobj = pad_read(nobj)) > 0) {
		Cdb  = (Cdb==Db)? Db+1: Db;
		ClearOTag(Cdb->ot, OTSIZE);

		ot = Cdb->ot;
		sp = Cdb->sprt;
		pp = pos;
		for (i = 0; i < nobj; i++, sp++, pp++) {
			if ((x = (pp->x += pp->dx) % WALL_X*2) >= WALL_X)
				x = WALL_X*2 - x;
			if ((y = (pp->y += pp->dy) % WALL_Y*2) >= WALL_Y)
				y = WALL_Y*2 - y;
			setXY0(sp, x, y);
			AddPrim(ot, sp);
		}

		KanjiFntPrint("\n\n玉の数     = %d\n", nobj);
		KanjiFntPrint("CPUTIME = %d\n", VSync(1)-sync);
		KanjiFntFlush(-1);


		for(VsyncWait = 1; VsyncWait; );
		sync = VSync(1);

	}
	printf("nobj=%d\n",nobj);
	PadStop();
	StopCallback();
}

#include "balltex.h"
void init_prim(DB *db){
	u_short	clut[32];
	SPRT_16	*sp;
	int	i;

	db->disp.isinter = 1;
	db->draw.isbg = 1;
	db->draw.dfe = 0;
	setRGB0(&db->draw, 60, 120, 120);

	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);

	for (i = 0; i < 32; i++) {
		clut[i] = LoadClut(ballcolor[i], 0, 480+i);
	}

	for (sp = db->sprt, i = 0; i < MAXOBJ; i++, sp++) {
		SetSprt16(sp);
		SetSemiTrans(sp, 0);
		SetShadeTex(sp, 1);
		setUV0(sp, 0, 0);
		sp->clut = clut[i%32];
	}
}

void init_point(POS *pos){
	int	i;
	for (i = 0; i < MAXOBJ; i++) {
		pos->x  = rand();
		pos->y  = rand();
		pos->dx = (rand() % 4) + 1;
		pos->dy = (rand() % 4) + 1;
		pos++;
	}
}

int pad_read(int n){
	u_long	padd = PadRead(1);

	if(padd & PADLup)	n += 4;
	if(padd & PADLdown)	n -= 4;

	if (padd & PADL1)
		while (PadRead(1)&PADL1);

	if(padd & PADselect) 	return -1;

	limitRange(n, 1, MAXOBJ-1);
	return n;
}

/* VSync コールバック ルーチン */
void cbvsync(void){
	u_long i;

	/* GetODE() *****************************************************
	   コールバックルーチン内では偶数フレームしか返さないので、static
	   な変数 Frame を用いて比較している */
	for(; !(Frame^(i = GetODE())); );
	Frame = i;

	/* 特に奇数→偶数フレームの切り替わりは V-BLNK の直後になるので、
	   表示/描画環境の切り替えはなるべくすぐに行う */
	PutDispEnv(&Cdb->disp);
	PutDrawEnv(&Cdb->draw);

	if(VsyncWait){
		DrawOTag(Cdb->ot);
		VsyncWait = 0;
	}
	else
		/* CPU ネックが発生したので前フレームの OT を描画。前フレーム
		 の OT が確実に定義されていなくてはならない点に注意 */
		DrawOTag((Cdb==Db? Db+1:Db)->ot);
}
