/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			tuto6: multi channel CD-XA
 *
 *	Copyright(C) 1994 1997  Sony Computer Entertainment Inc.,
 *	All rights Reserved.
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul.30,1994	suzu
 *		1.10		Dec,27,1994	suzu
 *		1.20		Jun,02,1995	suzu
 *		2.00		Apr.24,1997	makoto
*/
/* マルチチャンネル CD-XA テストプログラム
 *
 *	CD 倍速再生時には、最大 8ch まで音声・データをインターリーブして
 *	記録することができる。ここでは、以下のデータが記録されているディ
 *	スクを想定して音声・データの再生を行なう。
 *
 *		0ch-6ch		オーディオ(CD-XA)チャンネル
 *		7ch		データチャンネル
 *
 *	7ch 目のデータを読み込みながら 0-6ch のいずれか一つのチャンネルの
 *	オーディオを再生する。
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>

#define SCTSIZE		2048
#define RNGSIZE		8
#define RNGMASK		7

static u_long sector[ RNGSIZE ][ SCTSIZE / 4 ];
static int rid = 0, errflag = 0;

int dsRepeatXA( int startp, int endp );
int dsGetPos( void );

static void notfound( char* file );
static void cbdataready( u_char intr, u_char* result );

int main( void )
{
	u_char param[ 4 ], result[ 8 ];
	char file[] = "\\XDATA\\XA\\MULTI8.XA;1";
	DslFILE fp;
	DslFILTER filter;
	DslLOC loc;
	int padd, opadd, ret, i, j;

	/* initialize graphics and controller */
	ResetGraph( 0 );
	PadInit( 0 );
	FntLoad( 960, 256 );
	SetDumpFnt( FntOpen( 16, 16, 320, 200, 0, 512 ) );

	DsInit();
	DsSetDebug( 0 );

	/* ファイルの位置を確定 */
	if( DsSearchFile( &fp, file ) == 0 ) {
		notfound( file );
		goto abort;
	}

	/* set mode */
	param[ 0 ] = DslModeSpeed | DslModeRT | DslModeSF;
	DsCommand( DslSetmode, param, 0, -1 );

	/* ADPCM 再生チャネル設定 */
	filter.file = 1;
	filter.chan = 0;
	DsCommand( DslSetfilter, ( u_char* )&filter, 0, -1 );

	/* コールバックをフックする */
	DsReadyCallback( cbdataready );

	/* 自動ループスタート */
	dsRepeatXA( DsPosToInt( &fp.pos ),
	  DsPosToInt( &fp.pos ) + fp.size / 2048 );

	while( ( ( padd = PadRead( 1 ) ) & PADselect ) == 0 ) {
		balls();

		if( DsSystemStatus() == DslReady ) {
			switch( DsSync( 0, 0 ) ) {
			case DslComplete:
				/* 再生チャネル選択 */
				if( opadd == 0
				  && padd & ( PADLright | PADLleft ) ) {
					filter.chan = ( padd & PADLright )
					  ? ( filter.chan + 1 ) % 8
					  : ( filter.chan + 7 ) % 8;

					DsControlB( DslSetfilter,
					  ( u_char* )&filter, result );
				}
				break;

			case DslDiskError:
				DsCommand( DslReadS, 0, 0, -1 );
				break;
			}
		}

		/* 位置の獲得 */
		DsIntToPos( dsGetPos(), &loc );

		/* ステータスの表示 */
		FntPrint( "\tMulti-Channl XA-AUDIO \n\n" );
		FntPrint( "position=(%02x:%02x:%02X)\n\n", loc.minute,
		  loc.second, loc.sector );

		/* 読み込んだデータを表示 */
		for( i = 0; i < RNGSIZE; i++ ) {
			for( j = 0; j < 3; j++ )
				FntPrint( "%08x", sector[ i ][ j ] );
			FntPrint( "\n" );
		}

		/* display current channel */
		FntPrint( "\nchannel=" );
		for( i = 0; i < 8; i++ )
			FntPrint( "~c%s%d", i == filter.chan ? "888" : "444",
			  i );

		FntFlush( -1 );
		opadd = padd;
	}

abort:
	DsControlB( DslPause, 0, 0 );
	DsClose();
	ResetGraph( 1 );
	PadStop();
	StopCallback();
	return 0;
}

/* CD-XA  再生しながらインターリーブデータを読み出す
 * インターリーブデータはここではリングバッファに格納される
 */
static void cbdataready( u_char intr, u_char* result )
{
	if( intr == DslDataReady ) {
		DsGetSector( sector[ rid ], 2048 / 4 );
		rid = ( ( rid + 1 ) & RNGMASK );
	} else
		errflag++;
}

static void notfound( char* file )
{
	int n = 60 * 4;

	while( n-- ) {
		balls();
		FntPrint( "\n\n%s: not found (waiting)\n", file );
		FntFlush( -1 );
	}
	printf( "%s: not found\n", file );
}
