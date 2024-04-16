/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	tmdview5: GsDOBJ5 object viewing rotine
 *
 *	"tuto4.c" ******** Multi Screen Coordinate
 *                Use libgs VER 3.2 LATER
 *
 *		Version 1.00	Jun,  15, 1995
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment
 *		All rights Reserved
 */

#include <sys/types.h>

#include <libetc.h>		/* PADを使うためにインクルードする必要あり */
#include <libgte.h>		/* LIGSを使うためにインクルードする必要あり*/
#include <libgpu.h>		/* LIGSを使うためにインクルードする必要あり */
#include <libgs.h>		/* グラフィックライブラリ を使うための
				   構造体などが定義されている */

#define OBJECTMAX 100		/* ３Dのモデルは論理的なオブジェクトに
                                   分けられるこの最大数 を定義する */

#define TEX_ADDR   0x80010000	/* テクスチャデータ（TIMフォーマット）
				   がおかれるアドレス */

#define TEX_ADDR1   0x80020000  /* Top Address of texture data1 (TIM FORMAT) */
#define TEX_ADDR2   0x80030000  /* Top Address of texture data2 (TIM FORMAT) */


#define MODEL_ADDR 0x80040000	/* モデリングデータ（TMDフォーマット）
				   がおかれるアドレス */

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

GsRVIEW2  view;			/* 視点を設定するための構造体 */
GsF_LIGHT pslt[3];		/* 平行光源を設定するための構造体 */
u_long padd;			/* コントローラのデータを保持する */

/* パケットデータを作成するためのワーク ダブルバッファのため2倍必要 */
u_long PacketArea[0x10000];

/************* MAIN START ******************************************/
main()
{
  int     i;
  GsDOBJ5 *op;			/* オブジェクトハンドラへのポインタ */
  int     outbuf_idx;
  MATRIX  tmpls,tmplw;

  ResetCallback();
  init_all();
    
  FntLoad(960, 256);
  SetDumpFnt(FntOpen(0, -64, 256, 200, 0, 512));
  
  while(1)
    {
      GsSetRefView2(&view);	/* ワールドスクリーンマトリックス計算 */      
      if(obj_interactive()==0)
         return 0;	/* パッドデータから動きのパラメータを入れる */
      outbuf_idx=GsGetActiveBuff();/* ダブルバッファのどちらかを得る */
      
      GsClearOt(0,0,&Wot[outbuf_idx]); /* オーダリングテーブルをクリアする */
      
      for(i=0,op=object;i<Objnum;i++)
	{
	  /* LW／LSマトリックスを計算する */
	  GsGetLws(op->coord2,&tmplw,&tmpls);
	  /* ライトマトリックスをGTEにセットする */
	  GsSetLightMatrix(&tmplw);
	  /* スクリーン／ローカルマトリックスをGTEにセットする */
	  GsSetLsMatrix(&tmpls);
	  /* オブジェクトを透視変換しオーダリングテーブルに登録する */
	  GsSortObject5(op,&Wot[outbuf_idx],14-OT_LENGTH,getScratchAddr(0));
	  op++;
	}
      padd=PadRead(1);		/* パッドのデータを読み込む */
      VSync(0);			/* Vブランクを待つ */
      ResetGraph(1);		/* GPUをリセットする */
      GsSwapDispBuff();		/* ダブルバッファを切替える */
      SetDispMask(1);
      
      /* 画面のクリアをオーダリングテーブルの最初に登録する */
      GsSortClear(0x0,0x0,0x0,&Wot[outbuf_idx]);
      
      /* オーダリングテーブルに登録されているパケットの描画を開始する */
      GsDrawOt(&Wot[outbuf_idx]);
      FntFlush(-1);
    }
}

obj_interactive()
{
  SVECTOR v1;
  MATRIX  tmp1;
  static  int level = 0;
  static  SVECTOR  scale = {ONE,ONE,ONE};

  /* オブジェクトをY軸回転させる */
  if((padd & PADRleft)>0) PWorld.vy -=5*ONE/360;
  /* オブジェクトをY軸回転させる */
  if((padd & PADRright)>0) PWorld.vy +=5*ONE/360;
  /* オブジェクトをX軸回転させる */
  if((padd & PADRup)>0) PWorld.vx+=5*ONE/360;
  /* オブジェクトをX軸回転させる */
  if((padd & PADRdown)>0) PWorld.vx-=5*ONE/360;

  /* オブジェクトをZ軸にそって動かす */
  if((padd & PADm)>0) DWorld.coord.t[2]-=1000;
  /* オブジェクトをZ軸にそって動かす */
  if((padd & PADl)>0) DWorld.coord.t[2]+=1000;
  /* オブジェクトをX軸にそって動かす */
  if((padd & PADLleft)>0) DWorld.coord.t[0] +=10;
  /* オブジェクトをX軸にそって動かす */
  if((padd & PADLright)>0) DWorld.coord.t[0] -=10;
  /* オブジェクトをY軸にそって動かす */
  if((padd & PADLdown)>0) DWorld.coord.t[1]+=10;
  /* オブジェクトをY軸にそって動かす */
  if((padd & PADLup)>0) DWorld.coord.t[1]-=10;
  /* プログラムを終了してモニタに戻る */
/*  if((padd & PADk)>0) {PadStop();ResetGraph(3);StopCallback();return 0;}*/
  if((padd & PADk)>0) {
	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
  }
  
  /* オブジェクトのパラメータからマトリックスを計算し座標系にセット */
  set_coordinate(&PWorld,&DWorld);
  
  FntPrint("Z = %d\n",DWorld.coord.t[2]);
  
  if((padd & PADn)>0 && level == 0)
    {
      level = 1;
      scale.vx = ONE/8;
      scale.vy = ONE/8;
      scale.vz = ONE/8;
    }
  if((padd & PADo)>0 && level == 1)
    {
      level = 0;
      scale.vx = ONE;
      scale.vy = ONE;
      scale.vz = ONE;
    }
  
  if(level == 0)
    FntPrint("SCREEN SCALE is NORMAL");
  else
    FntPrint("SCREEN SCALE is 1/8");
  
  GsScaleScreen(&scale);	/* reset normal screen coordinate scale */
  return 1;
}


init_all()			/* 初期化ルーチン群 */
{
  ResetGraph(0);		/* GPUリセット */
  PadInit(0);			/* コントローラ初期化 */
  padd=0;			/* コントローラ値初期化 */


  GsInitGraph(640,480,GsINTER|GsOFSGPU,1,0);
  /* 解像度設定（インターレースモード） */
  
  GsDefDispBuff(0,0,0,0);	/* ダブルバッファ指定 */

#if 0  
  GsInitGraph(640,240,GsNONINTER|GsOFSGPU,1,0);
  /* 解像度設定（ノンインターレースモード） */
  GsDefDispBuff(0,0,0,240);   /* ダブルバッファ指定 */
#endif
  
  GsInit3D();                 /* ３Dシステム初期化 */
  
  Wot[0].length=OT_LENGTH;    /* OT１に解像度設定 */
  Wot[0].org=zsorttable[0];   /* OT１にオーダリングテーブルの実体設定 */
  /* ダブルバッファのためもう一方にも同じ設定 */
  Wot[1].length=OT_LENGTH;
  Wot[1].org=zsorttable[1];
  
  coord_init();			/* 座標定義 */
  model_init();			/* モデリングデータ読み込み */
  view_init();			/* 視点設定 */
  light_init();			/* 平行光源設定 */
  
  texture_init(TEX_ADDR);	/* 16bit texture load */
  texture_init(TEX_ADDR1);	/* 8bit  texture load */
  texture_init(TEX_ADDR2);	/* 4bit  texture load */
}


view_init()			/* 視点設定 */
{
  /*---- Set projection,view ----*/
  GsSetProjection(1000);	/* プロジェクション設定 */
  
  /* 視点パラメータ設定 */
  view.vpx = 0; view.vpy = 0; view.vpz = 2000;
  /* 注視点パラメータ設定 */  
  view.vrx = 0; view.vry = 0; view.vrz = -4000;
  /* 視点の捻りパラメータ設定 */
  view.rz=0;
  view.super = WORLD;		/* 視点座標パラメータ設定 */
  
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
  
  /* ローテーションベクタから回転マトリックスを作成する */
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


model_init()
{				/* モデリングデータの読み込み */
  u_long *dop;
  GsDOBJ5 *objp;		/* モデリングデータハンドラ */
  int i;
  u_long *oppp;
  
  dop=(u_long *)MODEL_ADDR;	/* モデリングデータが格納されているアドレス */
  dop++;			/* hedder skip */
  
  GsMapModelingData(dop);	/* モデリングデータ（TMDフォーマット）を
				   実アドレスにマップする */
  dop++;
  Objnum = *dop;		/* オブジェクト数をTMDのヘッダから得る */
  dop++;			/* GsLinkObject5でリンクするためにTMDの
				   オブジェクトの先頭にもってくる */

  for(i=0;i<Objnum;i++)		/* TMDデータとオブジェクトハンドラを接続する */
    GsLinkObject5((u_long)dop,&object[i],i);
  
  oppp = PacketArea;		/* packetの枠組を作るアドレス */
  for(i=0,objp=object;i<Objnum;i++)
    { /* デフォルトのオブジェクトの座標系の設定 */
      objp->coord2 =  &DWorld;
      
      objp->attribute = 0;	/* attribute init */
      /* objp->attribute |= GsLOFF;	/* attributeの光源計算を制御 */
      
      oppp = GsPresetObject(objp,oppp);
      objp++;
    }
}
