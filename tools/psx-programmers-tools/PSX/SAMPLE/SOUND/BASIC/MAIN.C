/* ----------------------------------------------------------------
 *   sound sample...basic 
 * ----------------------------------------------------------------*/
/*
 * $PSLibId: Run-time Library Release 4.4$
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsnd.h> 
#ifdef __psx__
#include <libsn.h> 
#else
#define pollhost()
#endif

#define NOTICK

int   end_flag;

short seq1;  /* SEQ data id */
short sep1;  /* SEP data id */
short vab1;  /* VAB data id */

char seq_table[SS_SEQ_TABSIZ * 2 * 3];

/*    コントローラ設定
 * ----------------------------------------------------------------*/
void
padToCallSnd (unsigned long padd)
{
	static u_long opadd = 0;     /* a previous pad data */
	static int h_flag = 0;
	static int rl_flag = 0;
	static int rr_flag = 0;

	if (opadd == padd)
		return;
	
        if (padd & PADh) {
		if (h_flag == 0) {
               		SsSeqPlay (seq1, SSPLAY_PLAY, 3);
			h_flag = 1;
		}
	}

        if (padd & PADRleft) { 
		if (rl_flag == 0) {
			SsSepPlay (sep1, 2, SSPLAY_PLAY, 2);
			SsSeqSetDecrescendo (seq1, 50, 200);
			rl_flag = 1;
		}
        }

        if (padd & PADRright)  {
		if (rr_flag == 0) {
			SsSepPlay (sep1, 1, SSPLAY_PLAY, 1);
			rr_flag = 1;
		}
	}

        if (padd & PADRdown) {
		SsSepStop (sep1,2);
		SsSeqStop (seq1);
		h_flag = 0;
		rl_flag = 0;
        }

        if (padd & PADk)
                end_flag = 1;
	
	opadd = padd;
}

main (void)
{
        unsigned long padd;

	end_flag = 0;
	ResetCallback();

/*    コントロールパッド の初期化
 * ---------------------------------------------------------------- */

    	PadInit (0);

/*    グラフィックスの初期化 
 * ---------------------------------------------------------------- */

	ResetGraph(0);

/*    サウンドの初期化 
 * ---------------------------------------------------------------- */

	SsInit ();

/*    データ属性テーブル領域の設定
 * ---------------------------------------------------------------- */

	SsSetTableSize (seq_table, 2, 3);

/*    Tick の設定
 * ---------------------------------------------------------------- */

#ifdef NOTICK
	SsSetTickMode (SS_NOTICK);
#else 
	SsSetTickMode (SS_TICK60);
#endif

/*    VAB データのオープン，転送
 * ---------------------------------------------------------------- */

        vab1 = SsVabOpenHead ((unsigned char*)0x80030000L, -1);
	if( vab1 == -1 ) {
	  printf( "VAB headder open failed\n" );
	  exit(0);
	}
        vab1 = SsVabTransBody ((unsigned char*)0x80032a20L, vab1);
	if( vab1 == -1 ) {
	  printf( "VAB body open failed\n" );
	  exit(0);
	}
	SsVabTransCompleted (SS_WAIT_COMPLETED);

/*    サウンドシステムの開始
 * ---------------------------------------------------------------- */

#ifndef NOTICK
	SsStart ();

#endif

/*    SEQ/SEP データのオープン
 * ---------------------------------------------------------------- */

	seq1 = SsSeqOpen ((unsigned long *)0x80015000, vab1); 
	sep1 = SsSepOpen ((unsigned long *)0x80010000, vab1, 3); 

	SsSetMVol (127, 127);
	SsSeqSetVol (seq1, 127, 127);
	SsSepSetVol (sep1, 1, 80, 80);
	SsSepSetVol (sep1, 2, 127, 127);
	SsSepSetVol (sep1, 3, 127, 127);
	while (!end_flag) {
		VSync (0);
#if 0
		pollhost ();
#endif
#ifdef NOTICK
		SsSeqCalledTbyT ();
#endif
	    	padd = PadRead (0);
		padToCallSnd (padd);
	}

/*    SEQ/SEP および VAB データのクローズ
 * ---------------------------------------------------------------- */

        SsSeqClose (seq1);
        SsSepClose (sep1);
        SsVabClose (vab1);

/*    サウンドシステムの停止，終了
 * ---------------------------------------------------------------- */

        SsEnd ();
	SsQuit ();

        PadStop ();
	ResetGraph(3);
	StopCallback();
	return 0; 
}

/* ----------------------------------------------------------------
 *    End of File
 * ---------------------------------------------------------------- */
