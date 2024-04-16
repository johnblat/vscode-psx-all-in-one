/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			chain: chained DsRead()
 *
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Jul.25,1995	suzu
 *		2.00		Apr.25,1997	makoto
*/
/* DsRead() が正常終了する際に発生するコールバック DsReadCallbacK()
 *	を使用して、配列に登録されたリード要求を次々に実行する
 *
 *--------------------------------------------------------------------------
 *
 * dsReadChain		複数のファイルをバックグラウンドで読み込む
 *
 * 形式	int dsReadChain( DslLOC* postbl, int* scttbl, u_long** buftbl,
 *	  int ntbl )
 *
 * 引数		postbl	データの CD-ROM 上の位置を格納した配列
 *		scttbl	データサイズを格納した配列
 *		buftbl	データのメインメモリのアドレスを格納した配列
 *		ntbl	各配列の要素数
 *
 * 解説		postbl にあらかじめ設定された位置から scttbl に設定さ
 *		れたセクタ数を読み出して、buftbl 以下のアドレスに順次
 *		格納する。 読み込み終了の検出は、DsReadCallback() が使
 *		用され、ntbl 個のファイルが読み込まれるまでバックグラ
 *		ウンドで処理を行なう。
 *
 * 備考		dsReadChain() は DsReadCallback() を排他的に使用する。
 *		リードに失敗すると、配列の先頭に戻って最初からリトライ
 *		する。
 *
 * 返り値	常に 0
 *
 *--------------------------------------------------------------------------
 *
 * dsReadChainSync	dsReadChain() の実行状態を調べる
 *
 * 形式	int dsReadChainSync(void)
 *
 * 引数		なし
 *
 * 解説		dsReadChain() で未処理のファイルの個数を調べる。
 *
 * 返り値	正整数	まだ処理が完了していないファイルの数。
 *		0	すべてのファイルが正常に読み込まれた
 *		-1	いずれかのファイルの読み込みに失敗した。
 *--------------------------------------------------------------------------
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>

static DslLOC *postbl;		/* position table */
static int* scttbl;		/* sector number table */
static u_long** buftbl;		/* destination buffer pointer table */
static int ctbl;		/* current DsRead */
static int ntbl;		/* total DsReads */

static void cbread( u_char intr, u_char* result );

int dsReadChainSync( void )
{
	return ntbl - ctbl;
}

int dsReadChain( DslLOC* _postbl, int* _scttbl, u_long** _buftbl, int _ntbl )
{
	unsigned char com;

	/* save pointers */
	postbl = _postbl;
	scttbl = _scttbl;
	buftbl = _buftbl;
	ntbl = _ntbl;
	ctbl = 0;

	DsReadCallback( cbread );
	DsRead( &postbl[ ctbl ], scttbl[ ctbl ], buftbl[ ctbl ],
	  DslModeSpeed );
	return 0;
}

static void cbread( u_char intr, u_char* result )
{
/*	printf( "cbread: (%s)...\n", DsIntstr( intr ) ); */
	if( intr == DslComplete ) {
		if( ++ctbl == ntbl )
			DsReadCallback( 0 );
		else {
			DsRead( &postbl[ ctbl ], scttbl[ ctbl ],
			  buftbl[ ctbl ], DslModeSpeed );
		}
	} else {
		printf( "dsReadChain: data error\n" );
		ctbl = 0;
		DsRead( &postbl[ ctbl ], scttbl[ ctbl ], buftbl[ ctbl ],
		  DslModeSpeed );
	}
}
