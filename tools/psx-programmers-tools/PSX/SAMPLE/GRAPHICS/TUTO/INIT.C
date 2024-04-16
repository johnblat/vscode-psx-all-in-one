/* $PSLibId: Run-time Library Release 4.4$ */
/*				init
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/* グラフィック環境を初期化する */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* int x, y;	/* GTE オフセット	*/
/* int z;	/* スクリーンまでの距離	*/
/* int level;	/* デバッグレベル	*/

extern char	*progname;

void init_system(int x, int y, int z, int level)
{
	/* 描画・表示環境のリセット */
	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(32, 32, 320, 64, 0, 512));
	
	/* グラフィックシステムのデバッグ (0:なし, 1:チェック, 2:プリント) */
	SetGraphDebug(level);	

	/* ＧＴＥの初期化 */
	InitGeom();			
	
	/* オフセットの設定 */
	SetGeomOffset(x, y);	
	
	/* 視点からスクリーンまでの距離の設定 */
	SetGeomScreen(z);		
}
