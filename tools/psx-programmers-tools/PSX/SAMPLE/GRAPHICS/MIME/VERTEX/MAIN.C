/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *	mime (from tmdview5: GsDOBJ5 object viewing rotine)
 *
 *	"tuto0.c" ******** simple GsDOBJ5 Viewing routine
 *
 *		Version 1.00	Jul,  14, 1994
 *
 *		Copyright (C) 1993,1994,1995 by Sony Computer Entertainment
 *		All rights Reserved
 */

#include <sys/types.h>

#include <libetc.h>		/* PADを使うためにインクルードする必要あり */
#include <libgte.h>		/* LIBGS uses libgte (Must be included when using LIBGS
				/* LIGSを使うためにインクルードする必要あり*/
#include <libgpu.h>		/* LIBGS uses libgpu (Must be included when using LIBGS
				/* IGSを使うためにインクルードする必要あり */
#include <libgs.h>		/* グラフィックライブラリ を使うための
				   構造体などが定義されている */

#include "addr.h"
#include "nmime.h"  
#include "control.h"  

int obj_interactive();

#define OBJECTMAX 100		/* ３Dのモデルは論理的なオブジェクトに
                                   分けられるこの最大数 を定義する */
   
#define OT_LENGTH  12		/* オーダリングテーブルの解像度 */


GsOT            Wot[2];		/* オーダリングテーブルハンドラ
				   ダブルバッファのため２つ必要 */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* オーダリングテーブル実体 */

GsDOBJ5		object[OBJECTMAX]; /* オブジェクトハンドラ
				      オブジェクトの数だけ必要 */

u_long          Objnum;		/* モデリングデータのオブジェクトの数を
				   保持する */

GsCOORDINATE2   DWorld;  /* オブジェクトごとの座標系 */

SVECTOR         PWorld; /* 座標系を作るためのローテーションベクター */

extern MATRIX GsIDMATRIX;	/* 単位行列 */

GsRVIEW2  view;			/* 視点を設定するための構造体 */
GsF_LIGHT pslt[3];		/* 平行光源を設定するための構造体 */
u_long padd;			/* コントローラのデータを保持する */

/* パケットデータを作成するためのワークダブルバッファのため2倍必要 */
/* u_long PacketArea[0x10000]; */


u_long *PacketArea;

/* Transfer Function for smooth control */

extern int cntrlarry[CTLMAX][CTLTIME]; 
extern CTLFUNC ctlfc[CTLMAX];

/* Data table for Transfer Function */

static int cnv0[16] = {
	  39,  79, 118, 197, 315, 394, 433, 473,
	 473, 433, 394, 315, 197, 118,  79,  39
};

static int cnv1[24] = {
	 200, 325, 450, 475, 500, 475, 450, 375,
	 300, 250, 200, 150, 100,  80,  55,   0,
	 -50, -75,-100, -70, -40, -25, -10,  -5
};

static int cnv2[32] = {
	  20,  30,  40,  50,  59,  79,  98, 128,
	 157, 177, 197, 206, 216, 226, 236, 238,
	 236, 226, 216, 206, 197, 177, 157, 128,
	  98,  79,  59,  50,  40,  30,  20,  10
};

u_long *SCRATCH  = (u_long *)0x1f800000;

/************* MAIN START ******************************************/
main()
{
  u_long  fn;			/* frame (field) No. */
  int     i;
  GsDOBJ5 *op;			/* オブジェクトハンドラへのポインタ */
  int     outbuf_idx;		/* double buffer index */
  MATRIX  tmpls;
  
  ResetCallback();
  PadInit(0);
  PacketArea = (u_long *)PACKETBUF;
  init_all();
  /*== initialize Transfer Function  ====*/

  init_cntrlarry(cnv1,24);
  /* initialize MIMe function */
  
  init_mime_data(0, MODEL_ADDR, MDFDATAVTX, MDFDATANRM, ORGVTXBUF, ORGNRMBUF);

  /* Initialize object position */
  DWorld.coord.t[2] = -30000; 
  PWorld.vy = ONE/2;
  PWorld.vx = -ONE/12;

  for(fn=0;;fn++)
    {
      	if (obj_interactive()<0) return;/* set motion data by pad data */

		set_cntrl(fn);	/* generate control wave by transfer function */

		PWorld.vy += ctlfc[4].out;  /* Rotate around the Y axis */
		PWorld.vx += ctlfc[5].out;  /* Rotate around the X axis */
		DWorld.coord.t[2] += ctlfc[8].out; /* Move along the Z axis */
		DWorld.coord.t[0] += ctlfc[6].out; /* Move along the X axis */
		DWorld.coord.t[1] += ctlfc[7].out; /* Move along the Y axis */

		/*==MIME Function=======*/
		mimepr[0][0] = ctlfc[0].out; /* MIMe Weight (Control) value 0 */
		mimepr[0][1] = ctlfc[1].out; /* MIMe Weight (Control) value 1 */
		mimepr[0][2] = ctlfc[2].out; /* MIMe Weight (Control) value 2 */
		mimepr[0][3] = ctlfc[3].out; /* MIMe Weight (Control) value 3 */
		vertex_mime(0);			/* MIMe operation (Vertex) */
		normal_mime(0);			/* MIMe operation (Normal) */

      GsSetRefView2(&view);	/* ワールドスクリーンマトリックス計算 */
      outbuf_idx=GsGetActiveBuff();/* ダブルバッファのどちらかを得る */

      GsClearOt(0,0,&Wot[outbuf_idx]); /* オーダリングテーブルをクリアする */

      for(i=0,op=object;i<Objnum;i++)
	{
	  /* ワールド／ローカルマトリックスを計算する */
	  GsGetLw(op->coord2,&tmpls);
	  
	  /* ライトマトリックスをGTEにセットする */
	  GsSetLightMatrix(&tmpls);
	  
	  /* スクリーン／ローカルマトリックスを計算する */
	  GsGetLs(op->coord2,&tmpls);

	  /* スクリーン／ローカルマトリックスをGTEにセットする */
	  GsSetLsMatrix(&tmpls);
	  
	  /* オブジェクトを透視変換しオーダリングテーブルに登録する */
	  GsSortObject5(op,&Wot[outbuf_idx],14-OT_LENGTH,SCRATCH);
	  op++;
	}
	
      VSync(0);			/* Vブランクを待つ */
      padd=PadRead(1);		/* パッドのデータを読み込む */
      GsSwapDispBuff();		/* ダブルバッファを切替える */
      
      /* 画面のクリアをオーダリングテーブルの最初に登録する */
      GsSortClear(0x0,0x0,0x0,&Wot[outbuf_idx]);
      
      /* オーダリングテーブルに登録されているパケットの描画を開始する */
      GsDrawOt(&Wot[outbuf_idx]);
    }
}


int obj_interactive()
{
  SVECTOR v1;
  MATRIX  tmp1;

/*===========================================================================*/ 
		/* set control data0 for MIMe value0 */
		if((padd & PADl)>0) ctlfc[0].in = ONE; else ctlfc[0].in = 0;
		/* set control data1 for MIMe value1 */
		if((padd & PADm)>0) ctlfc[1].in = ONE; else ctlfc[1].in = 0;
		/* set control data2 for MIMe value2 */
		if((padd & PADn)>0) ctlfc[2].in = ONE; else ctlfc[2].in = 0;
		/* set control data3 for MIMe value3 */
		if((padd & PADo)>0) ctlfc[3].in = ONE; else ctlfc[3].in = 0;
  		/* set control data4 for rotation of Y axis */
		if((padd & PADRleft)>0) 	 ctlfc[4].in =  ONE/64;
		else {  if((padd & PADRright)>0) ctlfc[4].in = -ONE/64; 
			else 			 ctlfc[4].in =      0; 
		}
  		/* set control data5 for rotation of X axis */
		if((padd & PADRup)>0) 		 ctlfc[5].in =  ONE/64;
		else { 	if((padd & PADRdown)>0)  ctlfc[5].in = -ONE/64;
			else			 ctlfc[5].in =      0; 
		}
  		/* set control data8 for motion along Z axis */
		if((padd & PADh)>0)		 ctlfc[8].in = -240;
		else {  if((padd & PADk)>0)	 ctlfc[8].in =  240; 
			else			 ctlfc[8].in =   0; 
		}
  		/* set control data6 for motion along X axis */
		if((padd & PADLleft)>0) 	 ctlfc[6].in =  180;
		else {  if((padd & PADLright)>0) ctlfc[6].in = -180; 
			else			 ctlfc[6].in =   0; 
		}
  		/* set control data7 for motion along Y axis */
		if((padd & PADLup)>0)		 ctlfc[7].in = -180;
		else {  if((padd & PADLdown)>0)	 ctlfc[7].in =  180;
			else			 ctlfc[7].in =   0; 
		}
/*===========================================================================*/ 
  if (((padd & PADk)>0)&&((padd & PADo)>0)&&((padd & PADm)>0)){
      PadStop();
      ResetGraph(3);
      StopCallback();
      return -1;
  }
 
  /* set matrix in the coordinate system  */
  set_coordinate(&PWorld,&DWorld);
  
  /* clear flag for recalcuration */
  DWorld.flg = 0;

  return 0;
}



/* 初期化ルーチン群 */
init_all()
{
  ResetGraph(0);		/* GPUリセット */
  padd=0;			/* コントローラ値初期化 */

  GsInitGraph(640,480,GsOFSGPU|GsINTER,1,0);	/* 解像度設定
						   （インターレースモード） */
    

  GsDefDispBuff(0,0,0,0);	/* ダブルバッファ指定 */
#if 0    
  GsInitGraph(320,240,1,0,0);	/* 解像度設定（インターレースモード） */
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
}


view_init()			/* 視点設定 */
{
  /*---- Set projection,view ----*/
  GsSetProjection(1000);	/* プロジェクション設定 */
  
  /* 視点パラメータ設定 */
  view.vpx = 0; view.vpy = 0; view.vpz = 2000;
  
  /* 注視点パラメータ設定 */
  view.vrx = 0; view.vry = 0; view.vrz = 0;
  
  /* 視点の捻りパラメータ設定 */
  view.rz=0;

  /* 視点座標パラメータ設定 */
  view.super = WORLD;
  
  /* 視点パラメータを群から視点を設定する
     ワールドスクリーンマトリックスを計算する */
  GsSetRefView2(&view);
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
}


coord_init()			/* 座標系設定 */
{
  /* 座標の定義 */
  GsInitCoordinate2(WORLD,&DWorld);
  
  /* マトリックス計算ワークのローテーションベクター初期化 */
  PWorld.vx=PWorld.vy=PWorld.vz=0;
  
  /* オブジェクトの原点をワールドのZ = -4000に設定 */
  DWorld.coord.t[2] = -4000;
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

texture_init(addr)
u_long addr;
{
  RECT rect1;
  GsIMAGE tim1;

  /* TIMデータのヘッダからテクスチャのデータタイプの情報を得る */  
  GsGetTimInfo((u_long *)(addr+4),&tim1);
  
  rect1.x=tim1.px;		/* テクスチャ左上のVRAMでのX座標 */
  rect1.y=tim1.py;		/* テクスチャ左上のVRAMでのY座標 */
  rect1.w=tim1.pw;		/* テクスチャ幅 */
  rect1.h=tim1.ph;		/* テクスチャ高さ */
  
  /* VRAMにテクスチャをロードする */
  LoadImage(&rect1,tim1.pixel);
  
  /* カラールックアップテーブルが存在する */  
  if((tim1.pmode>>3)&0x01)
    {
      rect1.x=tim1.cx;		/* クラット左上のVRAMでのX座標 */
      rect1.y=tim1.cy;		/* クラット左上のVRAMでのY座標 */
      rect1.w=tim1.cw;		/* クラットの幅 */
      rect1.h=tim1.ch;		/* クラットの高さ */

      /* VRAMにクラットをロードする */
      LoadImage(&rect1,tim1.clut);
    }
}


/* モデリングデータの読み込み */
model_init()
{
  u_long *dop;
  GsDOBJ5 *objp;		/* モデリングデータハンドラ */
  int i;
  u_long *oppp;			/* packet area pointer */
  
  dop=(u_long *)MODEL_ADDR;	/* モデリングデータが格納されているアドレス */
  dop++;			/* hedder skip */
  
  GsMapModelingData(dop);	/* モデリングデータ（TMDフォーマット）を
				   実アドレスにマップする*/

  dop++;
  Objnum = *dop;		/* オブジェクト数をTMDのヘッダから得る */

  dop++;			/* GsLinkObject2でリンクするためにTMDの
				   オブジェクトの先頭にもってくる */

  /* TMDデータとオブジェクトハンドラを接続する */
  for(i=0;i<Objnum;i++)
    GsLinkObject5((u_long)dop,&object[i],i);
  /* packetの枠組を作るアドレス */
  oppp = PacketArea;
  
  for(i=0,objp=object;i<Objnum;i++)
    {	
      /* フォルトのオブジェクトの座標系の設定 */
      objp->coord2 =  &DWorld;
      
      objp->attribute = 0;	/* init attribute */
      
      /* GsDOBJ5型のオブジェクトは予めパケットの枠組を作って
	 おかなかれば使えない */      
      oppp = GsPresetObject(objp,oppp);	
      
      objp++;
    }
}
