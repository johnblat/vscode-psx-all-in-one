/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*			  tuto0: draw sprites
 *
 *	   Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */	
/* とにかく画面上にスプライトを表示してみる。
 *		せっかくだからスプライトをアニメーションする。
 *
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* コントローラレコーダ用マクロ */
#define PadRead(x)	myPadRead(x)

#define DT	4	/* スプライトの移動単位 */

static void load_texture4(u_short tpage[4], u_short clut[4]);

void tuto0(void)
{
	/* ダブルバッファ使用のため、それぞれ２組ずつ用意 */
	DRAWENV		draw[2];	/* 描画環境 */
	DISPENV		disp[2];	/* 表示環境 */

	POLY_FT4	mat0, mat1;	/* テクスチャマップポリゴン */
	SVECTOR		x0, x1;		/* 表示位置（vx,vy メンバのみ使用）*/

	u_short		tpage[4];	/* テクスチャページ ID （４枚）*/
	u_short		clut[4];	/* テクスチャ CLUT ID ４エントリ）*/
	int		frame = 0;	/* カウンタ */
	int		w = 64, h = 64;	/* 拡大・縮小率 */
	int		u, v;		/* work */
	u_long		padd;		/* work */

	/* デバッグプリント関数の初期化
	 * FntLoad(x,y) はフレームバッファの (x,y) にフォントをロードする
	 * SetDumpFnt() は標準ストリームを定義する
	 */
	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(16, 16, 256, 64, 0, 512));
	   
	/* フレームバッファ上でダブルバッファを構成するための描画環境・
	 * 表示環境構造体のメンバを設定する。
	 * バッファ #1 に描画している間は バッファ #2 を表示
	 * バッファ #2 に描画している間は バッファ #1 を表示する。
	 */
	/*	buffer #1	(0,  0)-(320,240)
	 *	buffer #2	(0,240)-(320,480)
	 */
	SetDefDrawEnv(&draw[0], 0,   0, 320, 240);
	SetDefDispEnv(&disp[0], 0, 240, 320, 240);
	SetDefDrawEnv(&draw[1], 0, 240, 320, 240);
	SetDefDispEnv(&disp[1], 0,   0, 320, 240);

	/* 背景を黒((R,G,B)=(0,0,0))でクリアする */
	draw[0].isbg = draw[1].isbg = 1;
	setRGB0(&draw[0], 0, 0, 0);	/* (R,G,B)=(0,0,0) */
	setRGB0(&draw[1], 0, 0, 0);	/* (R,G,B)=(0,0,0) */

	/* ポリゴンプリミティブ（拡大・縮小スプライト）を初期化
	 * SetPolyFT4() は POLY_FT4 プリミティブを初期化する
	 * SetShadeTex() はプリミティブの ShadeTex 機能（テクスチャにシェ
	 * ーディングをかける機能）を禁止する
	 */
	SetPolyFT4(&mat0);		
	SetShadeTex(&mat0, 1);		/* ShadeTex disable */
	SetPolyFT4(&mat1);
	SetShadeTex(&mat1, 1);		/* ShadeTex disable */

	/* スプライトの表示位置の初期値を決定 */
	x0.vx = 80-w/2;  x1.vx = 240-w/2;
	x0.vy = 120-h/2; x1.vy = 120-h/2;

	/* テクスチャパターンをフレームバッファに転送
	 * 結果のテクスチャページ・テクスチャ CLUT を tpage, clut に返す。
	 */
	load_texture4(tpage, clut);

	/* 表示開始 */
	SetDispMask(1);

	/* メインループ */
	while (1) {
		/* ダブルバッファをスワップ */
		PutDrawEnv(frame%2? &draw[0]:&draw[1]);
		PutDispEnv(frame%2? &disp[0]:&disp[1]);

		/* コントロールパッドを解析 */
		padd = PadRead(1);

		/* スプライトの移動量を決定 */
		if (padd & PADLup)	x0.vy -= DT;
		if (padd & PADLdown)	x0.vy += DT;
		if (padd & PADLleft) 	x0.vx -= DT;
		if (padd & PADLright)	x0.vx += DT;

		if (padd & PADRup)	x1.vy -= DT;
		if (padd & PADRdown)	x1.vy += DT;
		if (padd & PADRleft) 	x1.vx -= DT;
		if (padd & PADRright)	x1.vx += DT;

		/* 拡大・縮小率を決定 */
		if (padd & PADL1)	w--, h--;
		if (padd & PADR1) 	w++, h++;

		/* アニメーションの終了 */
		if (padd & PADselect) 	break;

		/* スプライト (Matchang) を描画する。
		 * スプライト (Matchang) を描画する。
		 * スプライトアニメーションをするために、４枚のテクスチャ
		 * ページにおかれたアニメーションパターンを順番に切替えていく。
		 */
 
		/* テクスチャページを設定する。*/
		mat0.tpage = mat1.tpage = tpage[(frame/16)%4];
		mat0.clut  = mat1.clut  = clut[(frame/16)%4];

		/* テクスチャ座標 (u,v) を設定 */
		u = (frame%4)*64;
		v = ((frame/4)%4)*64;
		setUVWH(&mat0, u, v, 63, 63);
		setUVWH(&mat1, u, v, 63, 63);

		/* スプライトの座標を設定 */
		setXYWH(&mat0, x0.vx, x0.vy, w, h);
		setXYWH(&mat1, x1.vx, x1.vy, w, h);

		/* スプライトプリミティブの描画 */
		DrawPrim(&mat0);
		DrawPrim(&mat1);

		/* デバッグメッセージを出力 */
		FntPrint("tuto0: simplest sample\n");
		FntPrint("mat0=");dumpXY0(&mat0);
		FntPrint("mat1=");dumpXY0(&mat1);

		/* FntFlush(-1) で標準ストリームをフラッシュする */
		FntFlush(-1);
		
		/* V-BLNK が来るのを待つ */
		VSync(0);

		/* フレーム番号をアップデート */
		frame++;
	}
	DrawSync(0);
	return;
}

/* テクスチャロード関数 */
/* u_short tpage[4];	テクスチャページＩＤ */
/* u_short clut[4];	テクスチャ CLUT ＩＤ */
static void load_texture4(u_short tpage[4], u_short clut[4])
{
	/* スプライトアニメーションデータ
	 * 64x64 のスプライトアニメーションが 64 パターン、
	 * ４枚のテクスチャページに渡って格納されている。
	 * 各ページの先頭アドレスは (mat0, mat1, mat2, mat3)。
	 * 内容は、
	 *	0x000-0x07f	CLUT  (256x2byte entry)
	 *	0x200-		INDEX (4bit mode, size=256x256)
	 */
	extern	u_long	mat0[];		/* Matchang of texture page #0 */
	extern	u_long	mat1[];		/* Matchang of texture page #1 */
	extern	u_long	mat2[];		/* Matchang of texture page #2 */
	extern	u_long	mat3[];		/* Matchang of texture page #3 */

	/* テクスチャページをロードする。（４枚分）
	 * ４ビットモードのため、実際のフレームバッファ中の領域は、
	 * 64x256 しか使用しないことに注意。
	 */
	tpage[0] = LoadTPage(mat0+0x80, 0, 0, 640,  0, 256,256);
	tpage[1] = LoadTPage(mat1+0x80, 0, 0, 640,256, 256,256);
	tpage[2] = LoadTPage(mat2+0x80, 0, 0, 704,  0, 256,256);
	tpage[3] = LoadTPage(mat3+0x80, 0, 0, 704,256, 256,256);

	/* テクスチャ CLUT をロードする。（４本） */
	clut[0] = LoadClut(mat0, 0,500);
	clut[1] = LoadClut(mat1, 0,501);
	clut[2] = LoadClut(mat2, 0,502);
	clut[3] = LoadClut(mat3, 0,503);
}

