/* $PSLibId: Run-time Library Release 4.4$ */
/*			tuto7: simple CdRead
 *
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Aug.05,1994	suzu
 *		1.10		Dec,27,1994	suzu
 *		1.20		Mar,12,1995	suzu
 *		1.30		Feg,12,1996	suzu
*/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>

static void read_test(char *file);
static void notfound(char *file);
static void cbvsync(void);

int main( void )
{

	/* initialize graphics and controller */
	ResetGraph(0);
	PadInit(0);
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 64, 0, 0, 1, 512));

	/* CD サブシステムを初期化 */
	CdInit();
	CdSetDebug(0);

	/* サウンドドライバを初期化 */
	sndInit();

	/* start display */
	SetDispMask(1);

	/* test loop */
	while ((PadRead(1)&PADselect) == 0)
		read_test("\\XDATA\\STR\\MOV.STR;1");

	/* ending */
	sndEnd();
	CdFlush();
	PadStop();
	StopCallback();
	return 0;
}


/* 読み出すセクタサイズ */
#define MAXSECTOR	256
static void read_test(char *file)
{
	static u_long	sectbuf[2][MAXSECTOR*2048/4];
	static int	n_trial = 0, n_err = 0, n_fatal = 0;
	int		i, cnt;
	CdlFILE		fp;
	int		nsector;
	unsigned char com;

	/* update trial counter */
	n_trial++;

	/* リードバッファを一旦クリア */
	for (i = 0; i < sizeof(sectbuf[0])/4; i++)
		sectbuf[0][i] = sectbuf[1][i] = 0;


	/* バックグラウンドリード */
	for (i = 0; i < 2; i++) {
		if  (CdSearchFile(&fp, file) == 0) {
			notfound(file);
			return;
		}

		/* ファイルの位置を確定 */
		if ((nsector = (fp.size+2047)/2048) > MAXSECTOR)
			nsector = MAXSECTOR;
		nsector = MAXSECTOR;	/* for debug */

		/* リード開始 */
		CdControl(CdlSetloc, (u_char *)&fp.pos, 0);
		com = CdlModeSpeed;
		CdControlB( CdlSetmode, &com, 0 );
		VSync( 3 );
		CdRead(nsector, sectbuf[i], CdlModeSpeed);

		/* リードの裏で通常の処理は実行できる。
		 * ここでは、Read が終了するまで残りのセクタ数を監視する
		 */
		while ((cnt = CdReadSync(1, 0)) > 0 ) {
			VSync(0);
			FntPrint("\t\t SIMPLE CDREAD\n\n");
			FntPrint("file name %s\n", file);
			FntPrint("trial count %d\n", n_trial);
			FntPrint("read  error %d\n", n_err);
			FntPrint("fatal error %d\n\n", n_fatal);
			FntPrint("reading(%d) ...%d Sectors\n", i, cnt);

			balls();
			FntFlush(-1);
		}

		/* 返り値が非零の場合はエラー */
		if (cnt != 0) {
			FntPrint("Read ERROR in %d\n\n", i);
			n_err++;
			return;
		}
	}

	/* 二つのデータを比較する */
	for (i = 0; i < sizeof(sectbuf[0])/4; i++)
		if (sectbuf[0][i] != sectbuf[1][i]) {
			printf("verify ERROR at (%08x:%08x)\n\n",
				 &sectbuf[0][i], &sectbuf[1][i]);
			n_fatal++;
			return;
		}
}

static void notfound(char *file)
{
	int n = 60*4;
	while (n--) {
		FntPrint("\n\n%s: not found\n", file);
		FntFlush(-1);
		VSync(0);
	}
	printf("%s: not found\n", file);
}

