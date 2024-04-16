/* $PSLibId: Run-time Library Release 4.4$ */
/* --------------------------------------------------------------------------
 * menuInit	メニューシステムの初期化
 *
 * 形式		void menuInit(int mode, int x, int y, int nstr)
 *
 * 引数		mode	0:	フォントパターンをロードして初期化
 *			1:	フォントパターンをロードせずに初期化
 *			2:	フォントパターンのみ再ロード
 *		x, y	メニューの左上端点
 *		nstr	使用する最大文字数
 *
 * 解説		メニューシステムを初期化する。
 *
 * NOTES
 *	mode が 2 の場合は、x, y, nstr は意味をもたない。（直前に設定
 *	された値が使用される。
 *	フォントは、(960, 256) にロードされる。
 *--------------------------------------------------------------------------
 * menuUpdate	メニューを表示
 *
 * 形式		int menuUpdate(char *title, char **path, padd)
 *
 * 引数		title	タイトルバーの文字列
 *		path	メニューの文字列配列
 *		padd	コントローラステータス
 *
 * 解説
 *	menuUpdate() は与えられた文字列のメニューを表示し、選ばれた項
 *	目の番号を返す。
 *
 * RETURNS
 *	-1	ボタンが押されていない。
 *	else	選ばれた項目の番号
 *--------------------------------------------------------------------------
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

static int	sx = 80, sy = 64, nstr = 64, id = 0;

void menuInit(int mode, int x, int y, int n)
{
	if (mode == 0 || mode == 2)
		FntLoad(960, 256);	
	if (mode == 0 || mode == 1)
		id = FntOpen((sx = x), (sy = y), 0, 0, 2, (nstr = n));	
}
	
int menuUpdate(char *title, char **path, u_long padd)
{
	static int	cp = 0, opadd = 0;
	int i, ispress = 0;
	
	FntPrint(id, "%s\n\n", title);
	for (i = 0; path[i]; i++) {
		if (i == cp)
			FntPrint(id, "~c888 %s\n", path[i]);
		else
			FntPrint(id, "~c444 %s \n", path[i]);
	}
	if (opadd == 0 && (padd&PADLup))    cp--;
	if (opadd == 0 && (padd&PADLdown))  cp++;
	if (opadd == 0 && (padd&PADRdown))  ispress = 1;
	if (opadd == 0 && (padd&PADRright)) ispress = 1;
	

	limitRange(cp, 0, i-1);
	opadd = padd;
	FntFlush(id);
	return(ispress? cp: -1);
}
