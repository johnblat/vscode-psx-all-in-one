/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			tmdview4: GsDOBJ4 object viewing rotine 
 *
 * "tuto7.c" ******** simple GsDOBJ4 Viewing routine using jump table 
 * 							(material attenuation supported) 
 * 
 * Version 1.00	Jul,  14, 1994 
 * 
 * Copyright (C) 1993 by Sony Computer Entertainment All rights Reserved 
 */

#include <sys/types.h>

#include <libetc.h>		/* PADを使うためにインクルードする必要あり */
#include <libgte.h>		/* LIGSを使うためにインクルードする必要あり */
#include <libgpu.h>		/* LIGSを使うためにインクルードする必要あり */
#include <libgs.h>		/* グラフィックライブラリ を使うための
				   構造体などが定義されている */

#define PACKETMAX 10000		/* Max GPU packets */

#define OBJECTMAX 100		/* ３Dのモデルは論理的なオブジェクトに 分けられるこの最大数
				   を定義する */

#define PACKETMAX2 (PACKETMAX*24)	/* size of PACKETMAX (byte) paket
					   size may be 24 byte(6 word) */

#define MODEL_ADDR 0x80040000	/* モデリングデータ（TMDフォーマット）がおかれるアドレス */

#define OT_LENGTH  10		/* オーダリングテーブルの解像度 */


GsOT    Wot[2];			/* オーダリングテーブルハンドラダブルバッファのため２つ必要 */

GsOT_TAG zsorttable[2][1 << OT_LENGTH];	/* オーダリングテーブル実体 */

GsDOBJ2 object[OBJECTMAX];	/* オブジェクトハンドラオブジェクトの数だけ必要 */

u_long  Objnum;			/* モデリングデータのオブジェクトの数を保持する */


GsCOORDINATE2 DWorld;		/* オブジェクトごとの座標系 */

SVECTOR PWorld;			/* 座標系を作るためのローテーションベクター */

GsRVIEW2 view;			/* 視点を設定するための構造体 */
GsF_LIGHT pslt[3];		/* 平行光源を設定するための構造体 */
u_long  padd;			/* コントローラのデータを保持する */

PACKET  out_packet[2][PACKETMAX2];	/* GPU PACKETS AREA */

/************* MAIN START ******************************************/
main()
{
	int     i;
	GsDOBJ2 *op;		/* オブジェクトハンドラへのポインタ */
	int     outbuf_idx;	/* double buffer index */
	MATRIX  tmpls;

	ResetCallback();

	init_all();

	FntLoad(960, 256);
	SetDumpFnt(FntOpen(32, 32, 256, 200, 0, 512));

	while (1) {
		FntPrint("z = %d\n", DWorld.coord.t[2]);
		if (obj_interactive() == 0)
			return 0;	/* パッドデータから動きのパラメータを入れる */
		GsSetRefView2(&view);	/* ワールドスクリーンマトリックス計算 */
		outbuf_idx = GsGetActiveBuff();	/* ダブルバッファのどちらかを得る */

		/* Set top address of Packet Area for output of GPU PACKETS */
		GsSetWorkBase((PACKET *) out_packet[outbuf_idx]);

		GsClearOt(0, 0, &Wot[outbuf_idx]);	/* オーダリングテーブルをクリアする */

		for (i = 0, op = object; i < Objnum; i++) {
			/* ワールド／ローカルマトリックスを計算する */
			GsGetLw(op->coord2, &tmpls);

			/* ライトマトリックスをGTEにセットする */
			GsSetLightMatrix(&tmpls);

			/* スクリーン／ローカルマトリックスを計算する */
			GsGetLs(op->coord2, &tmpls);

			/* スクリーン／ローカルマトリックスをGTEにセットする */
			GsSetLsMatrix(&tmpls);

			/* オブジェクトを透視変換しオーダリングテーブルに登録する */
			GsSortObject4J(op, &Wot[outbuf_idx], 14 - OT_LENGTH, getScratchAddr(0));
			op++;
		}
		VSync(0);	/* Vブランクを待つ */
		DrawSync(0);
		padd = PadRead(1);	/* パッドのデータを読み込む */
		GsSwapDispBuff();	/* ダブルバッファを切替える */

		/* 画面のクリアをオーダリングテーブルの最初に登録する */
		GsSortClear(0x0, 0x0, 0x0, &Wot[outbuf_idx]);

		/* オーダリングテーブルに登録されているパケットの描画を開始する */
		GsDrawOt(&Wot[outbuf_idx]);
		FntFlush(-1);
	}
}


obj_interactive()
{
	SVECTOR v1;
	MATRIX  tmp1;

	/* オブジェクトをY軸回転させる */
	if ((padd & PADRleft) > 0)
		PWorld.vy -= 5 * ONE / 360;

	/* オブジェクトをY軸回転させる */
	if ((padd & PADRright) > 0)
		PWorld.vy += 5 * ONE / 360;

	/* オブジェクトをX軸回転させる */
	if ((padd & PADRup) > 0)
		PWorld.vx += 5 * ONE / 360;

	/* オブジェクトをX軸回転させる */
	if ((padd & PADRdown) > 0)
		PWorld.vx -= 5 * ONE / 360;

	/* オブジェクトをZ軸にそって動かす */
	if ((padd & PADm) > 0)
		DWorld.coord.t[2] -= 100;

	/* オブジェクトをZ軸にそって動かす */
	if ((padd & PADl) > 0)
		DWorld.coord.t[2] += 100;

	/* オブジェクトをX軸にそって動かす */
	/* if((padd & PADLleft)>0) DWorld.coord.t[0] +=10; */
	if ((padd & PADLleft) > 0)
		view.vrx += 10;

	/* オブジェクトをX軸にそって動かす */
	/* if((padd & PADLright)>0) DWorld.coord.t[0] -=10; */
	if ((padd & PADLright) > 0)
		view.vrx -= 10;

	/* オブジェクトをY軸にそって動かす */
	/* if((padd & PADLdown)>0) DWorld.coord.t[1]+=10; */
	if ((padd & PADLdown) > 0)
		view.vry += 10;

	/* オブジェクトをY軸にそって動かす */
	/* if((padd & PADLup)>0) DWorld.coord.t[1]-=10; */
	if ((padd & PADLup) > 0)
		view.vry -= 10;

	/* プログラムを終了してモニタに戻る */
	if ((padd & PADk) > 0) {
		PadStop();
		ResetGraph(3);
		StopCallback();
		return 0;
	}
	/* オブジェクトのパラメータからマトリックスを計算し座標系にセット */
	set_coordinate(&PWorld, &DWorld);
	return 1;
}


/* 初期化ルーチン群 */
init_all()
{
	GsFOGPARAM fgp;
	ResetGraph(0);		/* GPUリセット */
	PadInit(0);		/* コントローラ初期化 */
	padd = 0;		/* コントローラ値初期化 */

#if 0
	GsInitGraph(640, 480, GsINTER | GsOFSGPU, 1, 0);
	/* 解像度設定（インターレースモード） */

	GsDefDispBuff(0, 0, 0, 0);/* ダブルバッファ指定 */
#endif

	GsInitGraph(640, 240, GsINTER | GsOFSGPU, 0, 0);
	/* 解像度設定（ノンインターレースモード） */
	GsDefDispBuff(0, 0, 0, 240);/* ダブルバッファ指定 */


	GsInit3D();		/* ３Dシステム初期化 */

	Wot[0].length = OT_LENGTH;	/* オーダリングテーブルハンドラに解像度設定 */

	Wot[0].org = zsorttable[0];	/* オーダリングテーブルハンドラに
					   オーダリングテーブルの実体設定 */

	/* ダブルバッファのためもう一方にも同じ設定 */
	Wot[1].length = OT_LENGTH;
	Wot[1].org = zsorttable[1];

	coord_init();		/* 座標定義 */
	model_init();		/* モデリングデータ読み込み */
	view_init();		/* 視点設定 */
	light_init();		/* 平行光源設定 */

	/* setting FOG parameters */
	fgp.dqa = -10000 * ONE / 64 / 1000;
	fgp.dqb = 5 / 4 * ONE * ONE;
	fgp.rfc = fgp.gfc = fgp.bfc = 0;
	GsSetFogParam(&fgp);

	/* setting jumptable for GsSortObject4J() */
	jt_init4();
}


view_init()
{				/* 視点設定 */
	/*---- Set projection,view ----*/
	GsSetProjection(1000);	/* プロジェクション設定 */

	/* 視点パラメータ設定 */
	view.vpx = 0;
	view.vpy = 0;
	view.vpz = 2000;

	/* 注視点パラメータ設定 */
	view.vrx = 0;
	view.vry = 0;
	view.vrz = 0;

	/* 視点の捻りパラメータ設定 */
	view.rz = 0;

	/* 視点座標パラメータ設定 */
	view.super = WORLD;

	/* 視点パラメータを群から視点を設定する
	   ワールドスクリーンマトリックスを計算する */
	GsSetRefView2(&view);
}


light_init()
{				/* 平行光源設定 */
	/* ライトID０ 設定 */
	/* 平行光源方向パラメータ設定 */
	pslt[0].vx = 30;
	pslt[0].vy = 0;
	pslt[0].vz = -100;

	/* 平行光源色パラメータ設定 */
	pslt[0].r = 0xf0;
	pslt[0].g = 0;
	pslt[0].b = 0;

	/* 光源パラメータから光源設定 */
	GsSetFlatLight(0, &pslt[0]);


	/* ライトID１ 設定 */
	pslt[1].vx = 0;
	pslt[1].vy = 30;
	pslt[1].vz = -100;
	pslt[1].r = 0;
	pslt[1].g = 0xf0;
	pslt[1].b = 0;
	GsSetFlatLight(1, &pslt[1]);

	/* ライトID２ 設定 */
	pslt[2].vx = -30;
	pslt[2].vy = 0;
	pslt[2].vz = -100;
	pslt[2].r = 0;
	pslt[2].g = 0;
	pslt[2].b = 0xf0;
	GsSetFlatLight(2, &pslt[2]);

	/* アンビエント設定 */
	GsSetAmbient(0, 0, 0);

	/* 光源計算のデフォルトの方式設定 */
	GsSetLightMode(0);
}

coord_init()
{				/* 座標系設定 */
	/* 座標の定義 */
	GsInitCoordinate2(WORLD, &DWorld);

	/* マトリックス計算ワークのローテーションベクター初期化 */
	PWorld.vx = PWorld.vy = PWorld.vz = 0;

	/* オブジェクトの原点をワールドのZ = -4000に設定 */
	DWorld.coord.t[2] = -4000;
}

/* ローテションベクタからマトリックスを作成し座標系にセットする */
set_coordinate(pos, coor)
	SVECTOR *pos;		/* ローテションベクタ */
	GsCOORDINATE2 *coor;	/* 座標系 */
{
	MATRIX  tmp1;

	/* 平行移動をセットする */
	tmp1.t[0] = coor->coord.t[0];
	tmp1.t[1] = coor->coord.t[1];
	tmp1.t[2] = coor->coord.t[2];

	/* マトリックスにローテーションベクタを作用させる */
	RotMatrix(pos, &tmp1);

	/* 求めたマトリックスを座標系にセットする */
	coor->coord = tmp1;

	/* マトリックスキャッシュをフラッシュする */
	coor->flg = 0;
}


/* テクスチャデータをVRAMにロードする */
texture_init(addr)
	u_long  addr;
{
	RECT    rect1;
	GsIMAGE tim1;

	/* TIMデータのヘッダからテクスチャのデータタイプの情報を得る */
	GsGetTimInfo((u_long *) (addr + 4), &tim1);

	rect1.x = tim1.px;	/* テクスチャ左上のVRAMでのX座標 */
	rect1.y = tim1.py;	/* テクスチャ左上のVRAMでのY座標 */
	rect1.w = tim1.pw;	/* テクスチャ幅 */
	rect1.h = tim1.ph;	/* テクスチャ高さ */

	/* VRAMにテクスチャをロードする */
	LoadImage(&rect1, tim1.pixel);

	/* カラールックアップテーブルが存在する */
	if ((tim1.pmode >> 3) & 0x01) {
		rect1.x = tim1.cx;	/* クラット左上のVRAMでのX座標 */
		rect1.y = tim1.cy;	/* クラット左上のVRAMでのY座標 */
		rect1.w = tim1.cw;	/* クラットの幅 */
		rect1.h = tim1.ch;	/* クラットの高さ */

		/* VRAMにクラットをロードする */
		LoadImage(&rect1, tim1.clut);
	}
}


/* モデリングデータの読み込み */
model_init()
{
	u_long *dop;
	GsDOBJ2 *objp;		/* モデリングデータハンドラ */
	int     i;

	dop = (u_long *) MODEL_ADDR;/* モデリングデータが格納されているアドレス */

	dop++;			/* hedder skip */

	GsMapModelingData(dop);	/* モデリングデータ（TMDフォーマット）を実アドレスにマップする */

	dop++;
	Objnum = *dop;		/* オブジェクト数をTMDのヘッダから得る */

	dop++;			/* GsLinkObject4JでリンクするためにTMDのオブジェクトの先頭にもってくる */

	/* TMDデータとオブジェクトハンドラを接続する */
	for (i = 0; i < Objnum; i++)
		GsLinkObject4((u_long) dop, &object[i], i);

	for (i = 0, objp = object; i < Objnum; i++) {
		/* デフォルトのオブジェクトの座標系の設定 */
		objp->coord2 = &DWorld;

		/* material attenuation setting */
		objp->attribute =  GsLLMOD | GsMATE | GsLDIM4;

		objp++;
	}
}

extern _GsFCALL GsFCALL4;	/* GsSortObject4J Func Table */
jt_init4()
{				/* Gs SortObject4J Fook Func (for material
				   attenuation) */
	PACKET *GsTMDfastF3NL(), *GsTMDfastF3MFG(), *GsTMDfastM3L(), *GsTMDfastNF3();
	PACKET *GsTMDdivF3NL(), *GsTMDdivF3LFG(), *GsTMDdivF3L(), *GsTMDdivNF3();
	PACKET *GsTMDfastG3NL(), *GsTMDfastG3MFG(), *GsTMDfastG3M(), *GsTMDfastNG3();
	PACKET *GsTMDdivG3NL(), *GsTMDdivG3LFG(), *GsTMDdivG3L(), *GsTMDdivNG3();
	PACKET *GsTMDfastTF3NL(), *GsTMDfastTF3MFG(), *GsTMDfastTF3M(), *GsTMDfastTNF3();
	PACKET *GsTMDdivTF3NL(), *GsTMDdivTF3LFG(), *GsTMDdivTF3L(), *GsTMDdivTNF3();
	PACKET *GsTMDfastTG3NL(), *GsTMDfastTG3MFG(), *GsTMDfastTG3M(), *GsTMDfastTNG3();
	PACKET *GsTMDdivTG3NL(), *GsTMDdivTG3LFG(), *GsTMDdivTG3L(), *GsTMDdivTNG3();
	PACKET *GsTMDfastF4NL(), *GsTMDfastF4MFG(), *GsTMDfastF4M(), *GsTMDfastNF4();
	PACKET *GsTMDdivF4NL(), *GsTMDdivF4LFG(), *GsTMDdivF4L(), *GsTMDdivNF4();
	PACKET *GsTMDfastG4NL(), *GsTMDfastG4MFG(), *GsTMDfastG4M(), *GsTMDfastNG4();
	PACKET *GsTMDdivG4NL(), *GsTMDdivG4LFG(), *GsTMDdivG4L(), *GsTMDdivNG4();
	PACKET *GsTMDfastTF4NL(), *GsTMDfastTF4MFG(), *GsTMDfastTF4M(), *GsTMDfastTNF4();
	PACKET *GsTMDdivTF4NL(), *GsTMDdivTF4LFG(), *GsTMDdivTF4L(), *GsTMDdivTNF4();
	PACKET *GsTMDfastTG4NL(), *GsTMDfastTG4MFG(), *GsTMDfastTG4M(), *GsTMDfastTNG4();
	PACKET *GsTMDdivTG4NL(), *GsTMDdivTG4LFG(), *GsTMDdivTG4L(), *GsTMDdivTNG4();
	PACKET *GsTMDfastF3GNL(), *GsTMDfastF3GLFG(), *GsTMDfastF3GL();
	PACKET *GsTMDfastG3GNL(), *GsTMDfastG3GLFG(), *GsTMDfastG3GL();

	/* flat triangle */
	GsFCALL4.f3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastF3M;
	GsFCALL4.f3[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastF3MFG;
	GsFCALL4.f3[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastF3NL;
	GsFCALL4.f3[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivF3L;
	GsFCALL4.f3[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivF3LFG;
	GsFCALL4.f3[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivF3NL;
	GsFCALL4.nf3[GsDivMODE_NDIV] = GsTMDfastNF3;
	GsFCALL4.nf3[GsDivMODE_DIV] = GsTMDdivNF3;
	/* gour triangle */
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastG3M;
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastG3MFG;
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastG3NL;
	GsFCALL4.g3[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivG3L;
	GsFCALL4.g3[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivG3LFG;
	GsFCALL4.g3[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivG3NL;
	GsFCALL4.ng3[GsDivMODE_NDIV] = GsTMDfastNG3;
	GsFCALL4.ng3[GsDivMODE_DIV] = GsTMDdivNG3;
	/* texture flat triangle */
	GsFCALL4.tf3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTF3M;
	GsFCALL4.tf3[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastTF3MFG;
	GsFCALL4.tf3[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastTF3NL;
	GsFCALL4.tf3[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivTF3L;
	GsFCALL4.tf3[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivTF3LFG;
	GsFCALL4.tf3[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivTF3NL;
	GsFCALL4.ntf3[GsDivMODE_NDIV] = GsTMDfastTNF3;
	GsFCALL4.ntf3[GsDivMODE_DIV] = GsTMDdivTNF3;
	/* texture gour triangle */
	GsFCALL4.tg3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTG3M;
	GsFCALL4.tg3[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastTG3MFG;
	GsFCALL4.tg3[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastTG3NL;
	GsFCALL4.tg3[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivTG3L;
	GsFCALL4.tg3[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivTG3LFG;
	GsFCALL4.tg3[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivTG3NL;
	GsFCALL4.ntg3[GsDivMODE_NDIV] = GsTMDfastTNG3;
	GsFCALL4.ntg3[GsDivMODE_DIV] = GsTMDdivTNG3;
	/* flat quad */
	GsFCALL4.f4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastF4M;
	GsFCALL4.f4[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastF4MFG;
	GsFCALL4.f4[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastF4NL;
	GsFCALL4.f4[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivF4L;
	GsFCALL4.f4[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivF4LFG;
	GsFCALL4.f4[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivF4NL;
	GsFCALL4.nf4[GsDivMODE_NDIV] = GsTMDfastNF4;
	GsFCALL4.nf4[GsDivMODE_DIV] = GsTMDdivNF4;
	/* gour quad */
	GsFCALL4.g4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastG4M;
	GsFCALL4.g4[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastG4MFG;
	GsFCALL4.g4[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastG4NL;
	GsFCALL4.g4[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivG4L;
	GsFCALL4.g4[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivG4LFG;
	GsFCALL4.g4[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivG4NL;
	GsFCALL4.ng4[GsDivMODE_NDIV] = GsTMDfastNG4;
	GsFCALL4.ng4[GsDivMODE_DIV] = GsTMDdivNG4;
	/* texture flat quad */
	GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTF4M;
	GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastTF4MFG;
	GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastTF4NL;
	GsFCALL4.tf4[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivTF4L;
	GsFCALL4.tf4[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivTF4LFG;
	GsFCALL4.tf4[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivTF4NL;
	GsFCALL4.ntf4[GsDivMODE_NDIV] = GsTMDfastTNF4;
	GsFCALL4.ntf4[GsDivMODE_DIV] = GsTMDdivTNF4;
	/* texture gour quad */
	GsFCALL4.tg4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTG4M;
	GsFCALL4.tg4[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastTG4MFG;
	GsFCALL4.tg4[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastTG4NL;
	GsFCALL4.tg4[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivTG4L;
	GsFCALL4.tg4[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivTG4LFG;
	GsFCALL4.tg4[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivTG4NL;
	GsFCALL4.ntg4[GsDivMODE_NDIV] = GsTMDfastTNG4;
	GsFCALL4.ntg4[GsDivMODE_DIV] = GsTMDdivTNG4;
	/* gradation  triangle */
	GsFCALL4.f3g[GsLMODE_NORMAL] = GsTMDfastF3GL;
	GsFCALL4.f3g[GsLMODE_FOG] = GsTMDfastF3GLFG;
	GsFCALL4.f3g[GsLMODE_LOFF] = GsTMDfastF3GNL;
	GsFCALL4.g3g[GsLMODE_NORMAL] = GsTMDfastG3GL;
	GsFCALL4.g3g[GsLMODE_FOG] = GsTMDfastG3GLFG;
	GsFCALL4.g3g[GsLMODE_LOFF] = GsTMDfastG3GNL;
}

