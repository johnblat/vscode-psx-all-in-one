/* $PSLibId: Run-time Library Release 4.4$ */
/*	tuto0
 *	プリシークの効果の検証（ DA トラック）
 *
 *	Copyright (C) 1997 Sony Computer Entertainment Inc.,
 *	All rights reserved.
 *
 *	PADLleft:		１曲戻す
 *	PADLright:		１曲進める
 *	PADLup:			５曲戻す
 *	PADLdown:		５曲進める
 *	PADRup:			プリシークする／しない
 *	PADRdown:		演奏開始／終了
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>

#define MAXTRACKS	100

static u_long padd_read( void );

int main( void )
{
	u_long	padd;

	int track_id = 1, pre_seek = 0;
	DslLOC track[ MAXTRACKS ];
	int maxtrack;
	int is_play = 0;
	int i, cntflag = 0;
	u_char status;
	int start, response = 0;
	int id = 0;

	ResetGraph( 0 );
	PadInit( 0 );

	FntLoad( 960, 256 );
	SetDumpFnt( FntOpen( 16, 16, 320, 200, 0, 512 ) );

	DsInit();
	DsSetDebug( 0 );

	/* TOC を読み込む*/
	if( ( maxtrack = DsGetToc( track ) ) == 0 ) {
		goto abort;
	}

	while( ( ( padd = padd_read() ) & PADselect ) == 0 ) {
		balls();

		/* １曲進める*/
		if( padd & PADLright ) {
			if( track_id + 1 <= maxtrack )
				track_id++;
			if( pre_seek ) {
				DsFlush();
				id = 0;
				id = DsPacket( DslModeDA, &track[ track_id ],
				  DslSeekP, 0, -1 );
			}
			is_play = 0;
		}

		/* １曲戻す*/
		if( padd & PADLleft ) {
			if( track_id - 1 >= 1 )
				track_id--;
			if( pre_seek ) {
				DsFlush();
				id = 0;
				id = DsPacket( DslModeDA, &track[ track_id ],
				  DslSeekP, 0, -1 );
			}
			is_play = 0;
		}

		/* ５曲進める*/
		if( padd & PADLdown ) {
			if( track_id + 5 <= maxtrack )
				track_id += 5;
			if( pre_seek ) {
				DsFlush();
				id = 0;
				id = DsPacket( DslModeDA, &track[ track_id ],
				  DslSeekP, 0, -1 );
			}
			is_play = 0;
		}

		/* ５曲戻す*/
		if( padd & PADLup ) {
			if( track_id - 5 >= 1 )
				track_id -= 5;
			if( pre_seek ) {
				DsFlush();
				id = 0;
				id = DsPacket( DslModeDA, &track[ track_id ],
				  DslSeekP, 0, -1 );
			}
			is_play = 0;
		}

		/* 演奏開始／終了*/
		if( padd & PADRdown ) {
			if( is_play == 0 ) {
				if( pre_seek ) {
					DsCommand( DslPlay, 0, 0, -1 );
				} else {
					DsPacket( DslModeDA,
					  &track[ track_id ], DslPlay, 0, -1 );
				}
				is_play = 1;
			} else {
				DsCommand( DslPause, 0, 0, -1 );
				is_play = 0;
				if( pre_seek ) {
					id = DsPacket( DslModeDA,
					  &track[ track_id ], DslSeekP, 0, -1 );
				}
			}
			start = VSync( -1 );
		}

		/* toggle で変える*/
		if( padd & PADRup ) {
			pre_seek = pre_seek ? 0 : 1;
		}

		status = DsStatus();
		if( status & DslStatSeek )
			cntflag = 1;
		if( cntflag && ( status & DslStatPlay ) ) {
			response = VSync( -1 ) - start;
			cntflag = 0;
		}

		FntPrint( "\n\n" );
		FntPrint( "\tPRE SEEK       %s\n", pre_seek ? " ON" : "OFF" );
		FntPrint( "\t\t" );
		for( i = 1; i <= 5 && i <= maxtrack; i++ ) {
			FntPrint( "~c%s%2d ",
			  i == track_id ? "888" : "444", i );
		}
		FntPrint( "\n" );
		FntPrint( "\t\t" );
		for( i = 6; i <= 10 && i <= maxtrack; i++ ) {
			FntPrint( "~c%s%2d ",
			  i == track_id ? "888" : "444", i );
		}
		FntPrint( "\n" );
		FntPrint( "\t\t" );
		for( i = 11; i <= 15 && i <= maxtrack; i++ ) {
			FntPrint( "~c%s%2d ",
			  i == track_id ? "888" : "444", i );
		}
		FntPrint( "\n" );
		FntPrint( "\t\t" );
		for( i = 16; i <= 20 && i <= maxtrack; i++ ) {
			FntPrint( "~c%s%2d ",
			  i == track_id ? "888" : "444", i );
		}
		FntPrint( "\n" );
		FntPrint( "~c888\n" );
		FntPrint( "\tQUEUE LENGTH   %3d\n", DsQueueLen() );
		FntPrint( "\tSTATUS          %02x\n", status );
		FntPrint( "\tTIME TO START  %3d\n", response );
		if( pre_seek ) {
			if( DsSync( id, 0 ) == DslComplete )
				FntPrint( "\tSEEK COMPLETE\n" );
		}
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
