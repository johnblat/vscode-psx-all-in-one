/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			repeat: CD-DA/XA repeat
 *
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Sep.12,1994	suzu
 *		1.10		Oct,24,1994	suzu
 *		2.00		Feb,02,1995	suzu
 *		3.00		Apr.22,1997	makoto
*/
/*		     リピート再生ライブラリサンプル
 *	    CD-DA/XA トラックの任意の２点間をオートリピートする。
 *--------------------------------------------------------------------------
 * dsRepeat	CD-DA の自動リピート再生をする。
 *
 * 形式		int dsRepeat(int startp, int endp)
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
 * dsRepeatXA	CD-XA の自動リピート再生をする。
 *
 * 形式		int dsRepeatXA(int startp, int endp)
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
 *		マルチチャネルを使用する場合は、前もって DslSetfilter
 *		を使用してチャネルを指定しておくこと。
 *--------------------------------------------------------------------------
 * dsGetPos	現在再生中の位置を知る
 *
 * 形式		int dsGetPos(void)
 *
 * 引数		なし
 *
 * 解説		現在再生中の位置（セクタ番号）を調べる。
 *
 * 返り値	現在再生中のセクタ番号
 *
 *--------------------------------------------------------------------------
 * dsGetRepPos	現在までの繰り返し回数を調べる
 *
 * 形式		int dsGetRepTime()
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
#include <libds.h>

/* アンダーフローした時の保険 */
#define SP_MARGIN	( 4 * 75 )	/* 4sec */

/* ポーリング間隔 */
#define	XA_FREQ		32

static int StartPos, EndPos;	/* 開始・終了位置 */
static int CurPos;		/* 現在位置 */
static int RepTime;		/* 繰り返し回数 */

/* DslDataReady 発生時のコールバック */
static void cbready( u_char intr, u_char* result );

/* VSync コールバック */
static void cbvsync( void );

/* DslGetlocP で使用するコールバック */
static void cbsync( u_char intr, u_char* result );

static int dsplay( u_char mode, u_char com );

int dsRepeat( int startp, int endp )
{
	StartPos = startp;
	EndPos = endp;
	CurPos = StartPos;
	RepTime = 0;

	DsReadyCallback( cbready );
	dsplay( DslModeRept | DslModeDA, DslPlay );

	return 0;
}

int dsRepeatXA( int startp, int endp )
{
	StartPos = startp;
	EndPos = endp;
	CurPos = StartPos;
	RepTime = 0;

	VSyncCallback( cbvsync );
	dsplay( DslModeSpeed | DslModeRT | DslModeSF, DslReadS );

	return 0;
}

int dsGetPos( void )
{
	return CurPos;
}

int dsGetRepTime( void )
{
	return RepTime;
}

/* dsRepeat() で使用するコールバック */
static void cbready( u_char intr, u_char* result )
{
	DslLOC loc;

	if( intr == DslDataReady ) {
		if( ( result[ 4 ] & 0x80 ) == 0 ) {
			loc.minute = result[ 3 ];
			loc.second = result[ 4 ];
			loc.sector = 0;
			CurPos = DsPosToInt( &loc );
		}
		if( CurPos > EndPos || CurPos < StartPos - SP_MARGIN )
			dsplay( DslModeRept | DslModeDA, DslPlay );
	} else
		dsplay( DslModeRept | DslModeDA, DslPlay );
}

/* dsRepeatXA() で使用するコールバック */
static void cbvsync( void )
{
	if( VSync( -1 ) % XA_FREQ )
		return;

	if( CurPos > EndPos || CurPos < StartPos - SP_MARGIN )
		dsplay( DslModeSpeed | DslModeRT | DslModeSF, DslReadS );
	else
		DsCommand( DslGetlocP, 0, cbsync, 0 );
}

/* DslGetlocP で使用するコールバック */
static void cbsync( u_char intr, u_char* result )
{
	int cnt;

	if( intr == DslComplete ) {
		cnt = DsPosToInt( ( DslLOC* )&result[ 5 ] );
		if( cnt > 0 )
			CurPos = cnt;
	}
}

static int dsplay( u_char mode, u_char com )
{
	DslLOC loc;
	int id;

	DsIntToPos( StartPos, &loc );
	id = DsPacket( mode, &loc, com, 0, -1 );
	CurPos = StartPos;
	RepTime++;
	return id;
}
