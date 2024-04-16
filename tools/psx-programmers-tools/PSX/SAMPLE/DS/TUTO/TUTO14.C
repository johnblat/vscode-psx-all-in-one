/* $PSLibId: Run-time Library Release 4.4$ */
/* ================================================
 *
 *	tuto2
 *	シークのコストを検証するサンプル
 *
 *	Copyright (C) 1997 Sony Computer Entertainment Inc.,
 *	All rights reserved.
 *
 *	PADRup:		リードモードの切り替え
 *				CONT: 連続リード
 *				EACH: それぞれシークする
 *	PADRdown:	リード開始
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>

static u_long padd_read( void );
static void cbsync( u_char intr, u_char* result );
static void cbready1( u_char intr, u_char* result, u_long* subhead );
static void cbready2( u_char intr, u_char* result, u_long* subhead );
static void file_setup( void );

static struct {
	int len[ 10 ];
	DslLOC loc[ 10 ];
	int cnt;
	int fnum;
	int mode;
	int finish;
} Attr;

#define MAXSECTOR	50
static u_long sectbuf[ 10 ][ MAXSECTOR * 2048 / 4 ];

int main( void )
{
	u_long	padd;

	u_char status;
	int start, response = 0;

	ResetGraph( 0 );
	PadInit( 0 );

	FntLoad( 960, 256 );
	SetDumpFnt( FntOpen( 16, 16, 320, 200, 0, 512 ) );

	DsInit();
	DsSetDebug( 1 );

	file_setup();
	Attr.mode = 1;
	Attr.finish = 0;

	while( ( ( padd = padd_read() ) & PADselect ) == 0 ) {
		balls();

		/* リード開始*/
		if( padd & PADRdown ) {
			/* パケットのコールバックに ReadySystem のキックを設定する*/
			DsPacket( DslModeSpeed | DslModeSize1,
			  &Attr.loc[ 0 ], DslReadN, cbsync, -1 );
			Attr.cnt = 0;
			if( Attr.mode )
				Attr.fnum = 0;
			else
				Attr.fnum = 9;
			Attr.finish = 0;
			start = VSync( -1 );
		}

		/* モードの切り替え*/
		if( padd & PADRup ) {
			Attr.mode = Attr.mode ? 0 : 1;
		}

		/* リードが終了したときに時間を計測する*/
		if( Attr.finish ) {
			response = VSync( -1 ) - start;
			Attr.finish = 0;
		}
		status = DsStatus();

		FntPrint( "\n\n" );
		FntPrint( "\tFILE          %4d\n", Attr.fnum );
		FntPrint( "\tPOS           %4d\n", Attr.cnt );
		FntPrint( "\tQUEUE LENGTH  %4d\n", DsQueueLen() );
		FntPrint( "\tREAD MODE     %s\n",
		  Attr.mode ? "CONT" : "EACH" );
		FntPrint( "\tSTATUS          %02x\n", status );
		FntPrint( "\tTIME TO READ  %4d\n", response );
		FntFlush( -1 );
		VSync( 0 );
	}

abort:
	DsControlB( DslPause, 0, 0 );
	DsClose();
	PadStop();
	ResetGraph( 1 );
	StopCallback();
	return 0;
}

/* 一度押したボタンは離されるまでマスクする*/
static u_long padd_read( void )
{
	u_long padd, ret;
	static u_long padd_mask = 0;

	padd = PadRead( 1 );
	padd_mask &= padd;
	ret = padd & ~padd_mask;
	padd_mask |= padd;
	return ret;
}

/* ReadySystem をスタートさせるためのコールバック */
static void cbsync( u_char intr, u_char* result )
{
	if( intr == DslComplete ) {
		if( Attr.mode == 1 )
			DsStartReadySystem( cbready1, -1 );
		else
			DsStartReadySystem( cbready2, -1 );
	}
}

/* ファイルを連続して読んだ場合 */
static void cbready1( u_char intr, u_char* result, u_long* subhead )
{
	if( intr == DslDataReady ) {
		DsGetSector( &sectbuf[ Attr.fnum ][ Attr.cnt * 2048 / 4 ],
		  2048 / 4 );

		/* セクタ数を数えて転送位置を切り替える*/
		if( ++Attr.cnt >= Attr.len[ Attr.fnum ] ) {
			Attr.cnt = 0;
			if( ++Attr.fnum >= 10 ) {
				DsEndReadySystem();
				Attr.finish = 1;
				Attr.fnum = 0;
				return;
			}
		}
	}
}

/* ファイル毎にシークして読んだ場合 */
static void cbready2( u_char intr, u_char* result, u_long* subhead )
{
	if( intr == DslDataReady ) {
		DsGetSector( &sectbuf[ Attr.fnum ][ Attr.cnt * 2048 / 4 ],
		  2048 / 4 );

		/* セクタ数を数えて転送位置を切り替える*/
		if( ++Attr.cnt >= Attr.len[ Attr.fnum ] ) {

			/* いちいち ReadySystem を終了させる*/
			DsEndReadySystem();
			Attr.cnt = 0;
			if( --Attr.fnum < 0 ) {
				Attr.finish = 1;
				Attr.fnum = 9;
				return;
			}

			/* 次のファイル位置をシーク */
			else {
				DsPacket( DslModeSpeed | DslModeSize1,
				  &Attr.loc[ Attr.fnum ], DslReadN, cbsync, -1 );
				return;
			}
		}
	}
}

static void file_setup( void )
{
	Attr.len[ 0 ] = 50;
	DsIntToPos( 10, &Attr.loc[ 0 ] );

	Attr.len[ 1 ] = 50;
	DsIntToPos( DsPosToInt( &Attr.loc[ 0 ] ) + Attr.len[ 0 ],
	  &Attr.loc[ 1 ] );

	Attr.len[ 2 ] = 50;
	DsIntToPos( DsPosToInt( &Attr.loc[ 1 ] ) + Attr.len[ 1 ],
	  &Attr.loc[ 2 ] );

	Attr.len[ 3 ] = 50;
	DsIntToPos( DsPosToInt( &Attr.loc[ 2 ] ) + Attr.len[ 2 ],
	  &Attr.loc[ 3 ] );

	Attr.len[ 4 ] = 50;
	DsIntToPos( DsPosToInt( &Attr.loc[ 3 ] ) + Attr.len[ 3 ],
	  &Attr.loc[ 4 ] );

	Attr.len[ 5 ] = 50;
	DsIntToPos( DsPosToInt( &Attr.loc[ 4 ] ) + Attr.len[ 4 ],
	  &Attr.loc[ 5 ] );

	Attr.len[ 6 ] = 50;
	DsIntToPos( DsPosToInt( &Attr.loc[ 5 ] ) + Attr.len[ 5 ],
	  &Attr.loc[ 6 ] );

	Attr.len[ 7 ] = 50;
	DsIntToPos( DsPosToInt( &Attr.loc[ 6 ] ) + Attr.len[ 6 ],
	  &Attr.loc[ 7 ] );

	Attr.len[ 8 ] = 50;
	DsIntToPos( DsPosToInt( &Attr.loc[ 7 ] ) + Attr.len[ 7 ],
	  &Attr.loc[ 8 ] );

	Attr.len[ 9 ] = 50;
	DsIntToPos( DsPosToInt( &Attr.loc[ 8 ] ) + Attr.len[ 8 ],
	  &Attr.loc[ 9 ] );
}
