/* $PSLibId: Run-time Library Release 4.4$ */
/*
 * Ansyncronous Communication Sample
 *
 *	Copyright (C) 1998 by Sony Computer Entertainment
 *			All rights Reserved
 */
#include <sys/types.h>
#include <sys/file.h>
#include <libapi.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsn.h>
#include "balls.h"
#include "remote.h"

/**************************************************************************/
#define	PIH	(320)
#define	PIV	(240)
#define	OTLEN	(16)
/**************************************************************************/
static	int	side;
static	long	ot[2][OTLEN];
static	DISPENV	disp[2];
static	DRAWENV	draw[2];
/*************************************************************************/

int main(void)
{
	long	hcnt;
	long	sencnt, reccnt;

	/* システム環境初期化 */
	SetDispMask(0);
	ResetGraph(0);
	SetGraphDebug(0);
	ResetCallback();
	PadInit(0);
	ExitCriticalSection();

	/* 描画環境設定 */
	SetDefDrawEnv(&draw[0], 0,   0, PIH, PIV);
	SetDefDrawEnv(&draw[1], 0, PIV, PIH, PIV);
	SetDefDispEnv(&disp[0], 0, PIV, PIH, PIV);
	SetDefDispEnv(&disp[1], 0,   0, PIH, PIV);
	draw[0].isbg = draw[1].isbg = 1;
	setRGB0( &draw[0], 0, 0, 64 );
	setRGB0( &draw[1], 0, 0, 64 );
	PutDispEnv(&disp[0]);
	PutDrawEnv(&draw[0]);
	side = 0;
	SetDispMask(1);

	/* フォントシステム初期化 */
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));

	/* init balls module */
	_make_balls_data();

	/* 対戦ケーブルドライバ初期化 */
	_init_remote();
	_start_remote( 2073600/4 );

	sencnt = reccnt = 0;

	while(1) {
		ClearOTag( ot[side], OTLEN );

		/* データの送受信 */
		if (_rec_remote()==1 ) reccnt++;
		if (_send_remote()==1 ) sencnt++;

		_draw_balls_data( side, ot[side] );

		hcnt = VSync(0);
		side ^= 1;
		PutDispEnv(&disp[side]);
		PutDrawEnv(&draw[side]);
		DrawOTag( ot[side^1] );
		FntPrint("\nREMOTE CONTROLLER\n\n");
		FntPrint("ANSYNCRONOUS READ\n");
		FntPrint("SYNCRONOUS WRITE\n\n");
		FntPrint("SEND     %d\nRECEIVE  %d\n", sencnt, reccnt );
		FntPrint("ERROR1   %d\nERROR2   %d\n", _get_error_count1(), _get_error_count2());
		FntPrint("HSYNC    %d", hcnt );
		FntFlush(-1);
	}
	return 0;
}
