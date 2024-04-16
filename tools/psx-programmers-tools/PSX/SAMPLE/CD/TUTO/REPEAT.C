/* $PSLibId: Run-time Library Release 4.4$ */
/*			repeat: CD-DA/XA repeat
 *
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------	
 *		1.00		Sep.12,1994	suzu
 *		1.10		Oct,24,1994	suzu
 *		2.00		Feb,02,1995	suzu
*/
/*		     リピート再生ライブラリサンプル
 *	    CD-DA/XA トラックの任意の２点間をオートリピートする。
 *--------------------------------------------------------------------------
 * cdRepeat	CD-DA の自動リピート再生をする。
 *
 * 形式		int cdRepeat(int startp, int endp)
 *
 * 引数		startp	演奏開始位置
 *		endp	演奏終了位置
 *
 * 解説		startp と endp で指定された間の CD-DA データを繰り返し
 *		てバックグラウンドで再生する。 
 *
 * 返り値	つねに 0
 *	
 * 備考		内部の位置検出は、レポートモードを使用しているため高速
 *	
 *--------------------------------------------------------------------------
 * cdRepeatXA	CD-XA の自動リピート再生をする。
 *
 * 形式		int cdRepeatXA(int startp, int endp)
 *
 * 引数		startp	演奏開始位置
 *		endp	演奏終了位置
 *
 * 解説		startp と endp で指定された間の CD-XA データを繰り返し
 *		てバックグラウンドで再生する。 
 *
 * 返り値	つねに 0
 *	
 * 備考		内部の位置検出は、VSyncCallback() を使用して行なうので、
 *		するので、このソースコードをそのまま使用する場合は注意
 *		すること。
 *		再生は倍速度 XA のみ。
 *		マルチチャネルを使用する場合は、前もって CdlSetfilter 
 *		を使用してチャネルを指定しておくこと。
 *--------------------------------------------------------------------------
 * cdGetPos	現在再生中の位置を知る
 *
 * 形式		int cdGetPos(void)
 *
 * 引数		なし
 *
 * 解説		現在再生中の位置（セクタ番号）を調べる。
 *
 * 返り値	現在再生中のセクタ番号
 *	
 *--------------------------------------------------------------------------
 * cdGetRepPos	現在までの繰り返し回数を調べる
 *
 * 形式		int cdGetRepTime()
 *
 * 引数		なし
 *
 * 解説		現在までの繰り返し回数を調べる。タイムアウトによるエラー
 *		の検出に使用する。
 *
 * 返り値	現在までの繰り返し回数
 *--------------------------------------------------------------------------
 */
	
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>

/* アンダーフローした時の保険 */
#define SP_MARGIN	(4*75)	/* 4sec */

/* ポーリング間隔 */
#define	XA_FREQ	32/*15*/			

static int	StartPos, EndPos;	/* 開始・終了位置 */
static int	CurPos;			/* 現在位置 */
static int	RepTime;		/* 繰り返し回数 */

/* CdlDataReady 発生時のコールバック */
static void cbready(u_char intr, u_char *result);	

/* VSync コールバック */
static void cbvsync(void);			

static cdplay(u_char com);

int cdRepeat(int startp, int endp)
{
	u_char	param[4];

	StartPos = startp;
	EndPos   = endp;
	CurPos   = StartPos;
	RepTime  = 0;

	param[0] = CdlModeRept|CdlModeDA;
	CdControlB(CdlSetmode, param, 0);
	VSync( 3 );

	CdReadyCallback(cbready);
	cdplay(CdlPlay);

	return(0);
}

int cdRepeatXA(int startp, int endp)
{
	u_char	param[4];

	StartPos = startp;
	EndPos   = endp;
	CurPos   = StartPos;
	RepTime  = 0;

	param[0] = CdlModeSpeed|CdlModeRT|CdlModeSF;
	CdControlB(CdlSetmode, param, 0);
	VSync( 3 );

	VSyncCallback(cbvsync);
	cdplay(CdlReadS);

	return(0);
}

int cdGetPos()
{
	return(CurPos);
}

int cdGetRepTime()
{
	return(RepTime);
}

/* cdRepeat() で使用するコールバック */
static void cbready(u_char intr, u_char *result)
{
	CdlLOC	pos;
	if (intr == CdlDataReady) {
		if ((result[4]&0x80) == 0) {
			pos.minute = result[3];
			pos.second = result[4];
			pos.sector = 0;
			CurPos = CdPosToInt(&pos);
		}
		if (CurPos > EndPos || CurPos < StartPos - SP_MARGIN) 
			cdplay(CdlPlay);
	}
	else {
		/*printf("cdRepeat: error:%s\n", CdIntstr(intr));*/
		while (cdplay(CdlPlay) != 0);
	}
}	


/* cdRepeatXA() で使用するコールバック */
static void cbvsync(void)
{
	u_char		result[8];
	int		cnt, ret;
	
	if (VSync(-1)%XA_FREQ)	return;
	
	if ((ret = CdSync(1, result)) == CdlDiskError) {
		/*printf("cdRepeatXA: DiskError\n");*/
		cdplay(CdlReadS);
	}
	else if (ret == CdlComplete) {
		if (CurPos > EndPos || CurPos < StartPos - SP_MARGIN) 
			cdplay(CdlReadS);
		else {
			if (CdLastCom() == CdlGetlocP &&
			    (cnt = CdPosToInt((CdlLOC *)&result[5])) > 0)
				CurPos = cnt;
			CdControlF(CdlGetlocP, 0);
		}
	}
}


static cdplay(u_char com)
{
	CdlLOC	loc;
	
	CdIntToPos(StartPos, &loc);
	if (CdControl(com, (u_char *)&loc, 0) != 1)
		return(-1);
	
	CurPos = StartPos;
	RepTime++;
	return(0);
}

