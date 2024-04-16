/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			tuto5: CD-XA repeat
 *
 *	Copyright(C) 1994 1997  Sony Computer Entertainment Inc.,
 *	All rights Reserved.
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul.30,1994	suzu
 *		1.10		Dec,27,1994	suzu
 *		2.00		Feb,02,1995	suzu
 *		3.00		Apr.23,1997	makoto
*/
/* リピート再生
 *		CD-XA トラックの任意の２点間をリピートする。
 *
 *	CD-XA 再生時はレポートモードのコールバックが使用できない。
 *	そのため DslGetloc で場所を探す必要がある。CdControl() が重い
 *	場合は、CdControlF() を使用する。
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>

int dsRepeat( int startp, int endp );
int dsRepeatXA( int startp, int endp );
int dsGetPos( void );
int dsGetRepTime( void );

char title[] = "    SELECT MENU    ";
static char* menu[] = { "Play", "Pause", 0 };

static void print_gage( int startp, int endp, int curp );
static void notfound( char* file );

int main( void )
{
	char* file = "\\XDATA\\XA\\MULTI8.XA;1";

	DslFILE fp;
	DslLOC loc;
	DslFILTER filter;
	int padd, n, id;
	int startp, endp;

	/* initialize graphics and controller */
	ResetGraph( 0 );
	PadInit( 0 );
	menuInit( 0, 80, 88, 256 );
	SetDumpFnt( FntOpen( 16, 16, 320, 200, 0, 512 ) );

	/* CD サブシステムを初期化 */
	DsInit();
	DsSetDebug( 0 );

	/* ファイルの位置を確定 */
	if( DsSearchFile( &fp, file ) == 0 ) {
		notfound( file );
		goto abort;
	}

	/* XA チャンネルの設定*/
	filter.file = 1;
	filter.chan = 0;
	DsCommand( DslSetfilter, ( u_char* )&filter, 0, -1 );

	/* 自動ループスタート */
	startp = DsPosToInt( &fp.pos );
	endp = startp + fp.size / 2048;
	dsRepeatXA( startp, endp );

	while( ( ( padd = PadRead( 1 ) ) & PADselect ) == 0 ) {
		balls();

		id = menuUpdate( title, menu, padd );

		if( DsSystemStatus() == DslReady ) {
			switch( id ) {
			case 0:
				DsCommand( DslReadS, 0, 0, -1 );
				break;
			case 1:
				DsCommand( DslPause, 0, 0, -1 );
				break;
			}
		}

		/* ステータスの表示 */
		DsIntToPos( dsGetPos(), &loc );
		FntPrint( "\t\t XA-AUDIO Repeat\n\n" );
		FntPrint( "position=(%02x:%02x:%02X)\n", loc.minute,
		  loc.second, loc.sector );
		FntPrint( "%s...\n", DsComstr( DsLastCom() ) );
		print_gage( startp, endp, DsPosToInt( &loc ) );

		FntFlush( -1 );
	}
	DsCommand( DslStop, 0, 0, -1 );

abort:
	DsControlB( DslPause, 0, 0 );
	DsClose();
	ResetGraph( 1 );
	PadStop();
	StopCallback();
	return 0;
}

static void print_gage( int startp, int endp, int curp )
{
	int i = 0, rate;

	rate = 32 * ( curp - startp ) / ( endp - startp );

	FntPrint( "~c444" );
	while( i++ < rate )
		FntPrint( "*" );
	FntPrint( ( VSync( -1 ) >> 4 ) & 0x01 ? "~c888*~c444" : "*" );
	while( i++ < 32 )
		FntPrint( "*" );
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
