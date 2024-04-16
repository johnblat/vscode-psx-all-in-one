/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *
 *
 *	"tuto0.c" loop 3DBG (FOG MAP DATA)
 *
 *		Version 1.00	Jul,  14, 1994
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment
 *		All rights Reserved
 */
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>	

/* コントローラレコーダ用マクロ */
#define PadRead(x)	myPadRead(x)

#include "struct.h"

/* BG ハンドリング
 *
 ***************************************************************************/
/* BG セル構造体 
 * この例では CLUT は１本なので clut メンバは使用しない。
 * この例では 属性判定をしないので attr メンバは使用しない。
 * bgmap.h を参照 
 */
extern	u_long	bgtex[];	/* BG texture, CLUT (4bit) */
extern	u_long	bgtex8[];	/* BG texture, CLUT (8bit) */

#define PACKETMAX 4000		/* Max GPU packets */

#define OBJECTMAX 100		/* ３Dのモデルは論理的なオブジェクトに
                                   分けられるこの最大数 を定義する */

#define PACKETMAX2 (PACKETMAX*24) /* size of PACKETMAX (byte)
                                     paket size may be 24 byte(6 word) */

#define OT_LENGTH  4		/* オーダリングテーブルの解像度 */


GsOT            Wot[2];		/* オーダリングテーブルハンドラ
				   ダブルバッファのため２つ必要 */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* オーダリングテーブル実体 */

#include "bgmap.h"		/* セルタイプを記述した２次元配列 */
Gs3DBG0		gobj = {32, 32, 10, _map, _ctype};
GsDIVCOND0      cond;
GsDPCLUT0       ClutHandle;

u_long divz[6] = {3,2,2,2,1,1}; /* shift length */

#define CBIT 5
#define CNUM (1<<CBIT)
#define BGR 200
#define BGG 200
#define BGB 200

GsCOORDINATE2   DJimen;  /* オブジェクトごとの座標系 */

SVECTOR         PJimen; /* 座標系を作るためのローテーションベクター */
SVECTOR         PView; /* 座標系を作るためのローテーションベクター */

GsCOORDINATE2   DLocal;		/* 視点をぶらさげるローカル座標 */
SVECTOR         PLocal;		/* 回転ベクトルwork */


extern MATRIX GsIDMATRIX,GsIDMATRIX2;	/* 単位行列 */
extern PACKET *GsOUT_PACKET_P;

GsVIEW2  view;			/* 視点を設定するための構造体 */
GsF_LIGHT pslt[3];		/* 平行光源を設定するための構造体 */
u_long padd;			/* コントローラのデータを保持する */

PACKET		out_packet[2][PACKETMAX2];  /* GPU PACKETS AREA */

u_long Projection = 800;


#define DEBUG

/************* MAIN START ******************************************/
tuto5()
{
  int     i;

  int     outbuf_idx;		/* double buffer index */
  MATRIX  tmpls,tmplw;
#ifdef DEBUG
  int ratch_vcount;
#endif
  ResetCallback();
  init_all();
  SetGraphDebug(0);
  
/*  KanjiFntOpen(-320+32, -240+32, 640-64, 200, 704, 0, 768, 256, 0, 512); */
  FntLoad(960, 256);
/*  SetDumpFnt(FntOpen(-320+32, -240+32, 640-64, 200, 0, 512)); */
  FntOpen(-320+32, -240+32, 640-64, 200, 0, 512);
  
  while((PadRead(1)&PADselect) == 0) 
    {
      obj_interactive2();	/* パッドデータから動きのパラメータを入れる */

      GsSetView2(&view);	/* ワールドスクリーンマトリックス計算 */

      outbuf_idx=GsGetActiveBuff();/* ダブルバッファのどちらかを得る */

      /* Set top address of Packet Area for output of GPU PACKETS */
      GsSetWorkBase((PACKET*)out_packet[outbuf_idx]);

      GsClearOt(0,0,&Wot[outbuf_idx]); /* オーダリングテーブルをクリアする */

      /* ワールド／ローカルマトリックスを計算する */
	  GsGetLw(gobj.coord2,&tmplw);
      
      /* ライトマトリックスをGTEにセットする */
	GsSetLightMatrix(&tmplw);
      
      /* スクリーン／ローカルマトリックスを計算する */
	GsGetLs(gobj.coord2,&tmpls);
      
#ifdef DEBUG
      ratch_vcount = VSync(1);
#endif
      GsSort3DBG0_DPQ(&gobj,&Wot[outbuf_idx],14-OT_LENGTH,&tmpls,&ClutHandle);
#ifdef DEBUG
      FntPrint("load = %d\n%d (%d %d %d)\n",VSync(1)-ratch_vcount,
		    PLocal.vy,
		    DLocal.coord.t[0],DLocal.coord.t[1],DLocal.coord.t[2]);
#endif
      VSync(0);			/* Vブランクを待つ */

/*      DrawSync(0);*/
      ResetGraph(1);
      padd=PadRead(1);		/* パッドのデータを読み込む */
      GsSwapDispBuff();		/* ダブルバッファを切替える */

      /* 画面のクリアをオーダリングテーブルの最初に登録する */
      GsSortClear(BGR, BGG, BGB, &Wot[outbuf_idx]);

      
      /* オーダリングテーブルに登録されているパケットの描画を開始する */
      GsDrawOt(&Wot[outbuf_idx]);
      FntFlush(-1);
    }
    DrawSync(0);
    return;
}


obj_interactive2()
{
  MATRIX  tmpm;
  SVECTOR dd;
  VECTOR  tmpv;

  dd.vx = dd.vy = dd.vz = 0;
  
  if((padd & PADRright)>0)
    PLocal.vy+=ONE/360/2;
  
  if((padd & PADRleft)>0)
    PLocal.vy-=ONE/360/2;

  set_coordinate(&PLocal,&DLocal);
  
  /* 視点をZ軸にそって動かす */
  if((padd & PADR1)>0)
    dd.vz = -30;
  
  /* 視点をZ軸にそって動かす */
  if((padd & PADR2)>0)
    dd.vz = 30;
  
  /* 視点をX軸にそって動かす */
  if((padd & PADLleft)>0)
    dd.vx = -30;
  
  /* 視点をX軸にそって動かす */
  if((padd & PADLright)>0)
    dd.vx = 30;

  /* 視点をY軸にそって動かす */
  if((padd & PADLdown)>0)
    dd.vy = 10;
  
  /* 視点をY軸にそって動かす */
  if((padd & PADLup)>0)
    dd.vy = -10;

  /* 主観的移動のため（ローカル座標での移動）ワールドの移動に変換する */
  ApplyMatrix(&DLocal.coord,&dd,&tmpv);
  DLocal.coord.t[0]+=tmpv.vx;
  DLocal.coord.t[1]+=tmpv.vy;
  DLocal.coord.t[2]+=tmpv.vz;

  /* 見える範囲を変更する */ 
  if((padd & PADo)>0)
    {
      gobj.nw +=1;
      gobj.nh +=1;
    }
  if((padd & PADn)>0 && gobj.nw>1)
    {
      gobj.nw -=1;
      gobj.nh -=1;
    }
  
  /* 地面の出現する範囲をFOGで目立たなくする */
  SetFogFar(gobj.ch*gobj.nw/2,Projection);
}


/* 初期化ルーチン群 */
init_all()
{
  padd=0;			/* コントローラ値初期化 */
  GsInitGraph(640,480,GsINTER|GsOFSGPU,1,0);
  /* 解像度設定（インターレースモード） */

  GsDefDispBuff(0,0,0,0);	/* ダブルバッファ指定 */

#if 0
  GsInitGraph(320,240,GsINTER|GsOFSGPU,1,0);
  /* 解像度設定（ノンインターレースモード） */
  GsDefDispBuff(0,0,0,240);	/* ダブルバッファ指定 */
#endif

  GsInit3D();			/* ３Dシステム初期化 */
  
  Wot[0].length=OT_LENGTH;	/* オーダリングテーブルハンドラに解像度設定 */

  Wot[0].org=zsorttable[0];	/* オーダリングテーブルハンドラに
				   オーダリングテーブルの実体設定 */
  
  /* ブルバッファのためもう一方にも同じ設定 */
  Wot[1].length=OT_LENGTH;
  Wot[1].org=zsorttable[1];
  
  coord_init();			/* 座標定義 */
  model_init();			/* モデリングデータ読み込み */  
  view_init();			/* 視点設定 */
  light_init();			/* 平行光源設定 */
  texture_init();		/* load texture */
}


view_init()			/* 視点設定 */
{
  /*---- Set projection,view ----*/
  GsSetProjection(Projection);	/* プロジェクション設定 */
  
  view.view  = GsIDMATRIX2;
  view.super = &DLocal;
  view.view.t[2] = -100;
  view.view.t[1] = 0;
  view.view.t[0] = 0;
  PView.vx=PView.vy=PView.vz=0;
  /* 視点パラメータを群から視点を設定する
     ワールドスクリーンマトリックスを計算する */
  GsSetView2(&view);
}


light_init()			/* 平行光源設定 */
{
  /* ライトID０ 設定 */
  /* 平行光源方向パラメータ設定 */
  pslt[0].vx = 20; pslt[0].vy= -100; pslt[0].vz= -100;
  
  /* 平行光源色パラメータ設定 */
  pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
  
  /* 光源パラメータから光源設定 */
  GsSetFlatLight(0,&pslt[0]);

  
  /* ライトID１ 設定 */
  pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= 100;
  pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
  GsSetFlatLight(1,&pslt[1]);
  
  /* ライトID２ 設定 */
  pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= -100;
  pslt[2].r=0x60; pslt[2].g=0x60; pslt[2].b=0x60;
  GsSetFlatLight(2,&pslt[2]);
  
  /* アンビエント設定 */
  GsSetAmbient(0,0,0);

  /* 光源計算のデフォルトの方式設定 */
  GsSetLightMode(0);

  /* デプスキューイングの設定 */
  SetFogNear(8000,Projection);	/* default */
}

coord_init()			/* 座標系設定 */
{
  /* 座標の定義 */
  GsInitCoordinate2(WORLD,&DJimen);
  /* マトリックス計算ワークのローテーションベクター初期化 */
  PJimen.vx= -90*ONE/360;	/* x軸で地面を倒す */
  PJimen.vy= PJimen.vz=0;
  set_coordinate(&PJimen,&DJimen);

  GsInitCoordinate2(WORLD,&DLocal);
  DLocal.coord.t[1] = -200;
  PLocal.vx=PLocal.vy=PLocal.vz=0;
}

/* ローテションベクタからマトリックスを作成し座標系にセットする */
set_coordinate(pos,coor)
SVECTOR *pos;			/* ローテションベクタ */
GsCOORDINATE2 *coor;		/* 座標系 */
{
  MATRIX tmp1;
  
  /* 平行移動をセットする */
  tmp1.t[0] = coor->coord.t[0];
  tmp1.t[1] = coor->coord.t[1];
  tmp1.t[2] = coor->coord.t[2];
  
  /* マトリックスにローテーションベクタを作用させる */
  RotMatrix(pos,&tmp1);
  
  /* 求めたマトリックスを座標系にセットする */
  coor->coord = tmp1;

  /* マトリックスキャッシュをフラッシュする */
  coor->flg = 0;
}


/* テクスチャデータをVRAMにロードする */
texture_init()
{
	u_short		clut, tpage;
	int i;
  /* テクスチャ・テクスチャ CLUT をロード */

#ifdef BG_4bit	/* 4bit mode */
	tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex, 0, 480);
#else		/* 8bit mode */
	tpage = LoadTPage(bgtex8+0x80, 1, 0, 640, 0, 256, 256);
	clut  = LoadClut(bgtex8, 0, 480);
#endif

	ClutHandle.cbit   = CBIT;
	ClutHandle.clut   = (u_short *)bgtex8;
	ClutHandle.rectc.x = 640;
	ClutHandle.rectc.y = 480;
	ClutHandle.rectc.w = 256;
	ClutHandle.rectc.h = 1<<CBIT;
	ClutHandle.bgc.r=BGR;
	ClutHandle.bgc.g=BGG;
	ClutHandle.bgc.b=BGB;
	GsMakeDPClut0(&ClutHandle);
	
	for (i = 0; i < gobj.nctype; i++) {
		gobj.ctype[i].clut = &clut;
		gobj.ctype[i].tpage = tpage;
	}
}


/* モデリングデータの読み込み */
model_init()
{
  /* フォルトのオブジェクトの座標系の設定 */
    gobj.coord2 =  &DJimen;
    gobj.cw = 1024;		/* セルの横幅 */
    gobj.ch = 1024;		/* セルの縦幅 */
    gobj.nw = 16;		/* セルの横の数 */
    gobj.nh = 16;		/* セルの縦の数 */
    gobj.iw = 0x1f;		/* セルの繰り返しの単位(x) */
    gobj.ih = 0x1f;		/* セルの繰り返しの単位(y) */
    gobj.cond = &cond;
    
				/* アクティブサブディバイド条件 */
    cond.nearz = Projection/4;	/* divideを分割を開始するZ値 */
    cond.shift = 7;		/* shift値 */
    cond.nz    = 6;		/* divide段数の最大値 */
    cond.cond  = divz;		/* divide数テーブルへのポインタ */
}
