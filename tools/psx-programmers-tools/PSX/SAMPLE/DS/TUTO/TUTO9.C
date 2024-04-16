/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			tuto9: Load and Exec
 *
 *	Copyright(C) 1994 1997  Sony Computer Entertainment Inc.,
 *	All rights Reserved.
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		May.16,1995	suzu
 *		2.00		Apr.25,1997	makoto
*/
/* CD-ROM 上の実行形式プログラムを読みだし、子プロセスとして起動
 *	する。子プロセス終了後は親（このプログラム）へリターンする
 *	子プロセスは大抵 0x80010000 からロードされるので親プログラムは、
 *	そのアドレスを避けてロード・実行する必要がある。
 */

#include <sys/types.h>
#include <libds.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libapi.h>

/* menu title */
char title[] = "\t\t Exec test \t\t";

/* 実行するファイルのリスト */
static char* file[] = {
	"\\XDATA\\EXE\\CDRCUBE.EXE;1",
	"\\XDATA\\EXE\\CDANIM.EXE;1",
	"\\XDATA\\EXE\\CDBALLS.EXE;1",
	0,
};

/* callback parameters */
static int is_exec = 0;				/* indicate executing */
static int menu_id = -1;			/* menu ID */

static void cbvsync( void );
int dsExec( char* file, int mode );

static void cbvsync( void )
{
	/* dsExec() を実行中かどうかで画面を切替える */
	if( is_exec == 0 ) {
		bgUpdate( 96 );
		menu_id = menuUpdate( title, file, PadRead( 1 ) );
	} else {
		bgUpdate( 48 );
		FntPrint( "loading...\n" );
		FntFlush( -1 );
	}
}

/* Read 中に画面が停止するのは好ましくないので、読み込み中にコールバッ
 * ク内で簡単な描画やサウンドの再生を行なう
 */
int main( void )
{
	int id;
	DRAWENV draw;
	DISPENV disp;
	u_long padd;

	ResetGraph( 0 );
	PadInit( 0 );

	DsInit();
	DsSetDebug( 0 );

	menuInit( 0, 32, 88, 512 );	/* initialize menu */

	/* VSync のコールバック内で描画を起動する */
	VSyncCallback( cbvsync );

	/* バックグラウンドで BGM を再生する */
	sndInit();
	SetDispMask( 1 );		/* start display */

	/* initialize graphics */
	while( ( ( padd = PadRead( 1 ) ) & PADselect ) == 0 ) {

		VSync( 0 );
		if( ( id = menu_id ) != -1 ) {
			/* プログラムを実行する */
			is_exec = 1;	/* notify to callback */
			dsExec( file[ id ], 0 );
			is_exec = 0;	/* notify to callback */

			/* clear PAD */
			do {
				VSync( 0 );
			} while( PadRead( 1 ) );
		}
	}
	printf( "ending...\n" );
	sndEnd();
	DsClose();
	ResetGraph( 1 );
	PadStop();
	StopCallback();
	return 0;
}

/* dsExec	実行可能データをロードして実行
 *
 * 形式	EXEC *dsExec(char *file, int mode)
 *
 * 引数		file	実行ファイル名
 *		mode	実行モード（予約）
 *
 * 解説		位置 file より実行可能データをリードして、これを子プロセス
 *		として実行する。
 *
 * 備考
 *		mode は現在は見ていない
 *
 * 返り値	なし
 *
 ***************************************************************************/

int dsExec( char* file, int mode )
{
	static u_long headbuf[ 2048 / 4 ];
	static struct XF_HDR* head = ( struct XF_HDR* )headbuf;

	DRAWENV draw;
	DISPENV disp;
	DslFILE fp;
	DslLOC p0, p1;

	/* read header of EXE file */
	printf( "loading %s...\n", file );
	if( DsReadFile( file, ( u_long* )head, 2048 ) == 0 ) {
		printf( "%s: cannot open\n", file );
		return 0;
	}
	while( DsReadSync( 0 ) );

	head->exec.s_addr = 0;
	head->exec.s_size = 0;

	/* load contents of EXE file to t_addr */
	DsReadFile( 0, ( u_long* )head->exec.t_addr, 0 );
	while( DsReadSync( 0 ) );
	printf( "executing %s at %08x...\n", file, head->exec.t_addr );

	/* execute */
	GetDispEnv( &disp );		/* push DISPENV */
	GetDrawEnv( &draw );		/* push DRAWENV */
	SetDispMask( 0 );		/* black out */
	DsClose();			/* flush CD-ROM */
	PadStop();			/* stop PAD */
	ResetGraph( 1 );		/* flush GPU */
	StopCallback();			/* stop callback */
	Exec( &head->exec, 1, 0 );	/* execute */
	RestartCallback();		/* recover callback */
	PadInit( 0 );			/* recover PAD */
	DsInit();			/* recover CD */
	PutDispEnv( &disp );		/* pop DISPENV */
	PutDrawEnv( &draw );		/* pop DRAWENV */
	SetDispMask( 1 );		/* recover dispmask */

	return head->exec.t_addr;
}
