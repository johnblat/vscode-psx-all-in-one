/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	tmdview5: GsDOBJ5 object viewing rotine
 *
 *	"tuto6.c" ******** GsDOBJ5 Viewing routine (cocpit view)
 *
 *		Version 1.00	Jul,  14, 1995
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

GsCOORDINATE2   DView;  /* 視点をぶら下げる座標系 */

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
    
  while(1)
    {
      if(obj_interactive()==0)
        return 0;	/* パッドデータから動きのパラメータを入れる */
      GsSetRefView2(&view);	/* ワールドスクリーンマトリックス計算 */
      
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
    }
}

obj_interactive()
{
  SVECTOR dd;
  SVECTOR ax; /* 座標系を作るためのローテーションベクター */
  MATRIX  tmp1;
  VECTOR  tmpv;
  static  int count = 0;

  count++;
  
  dd.vx = dd.vy = dd.vz = 0;
  ax.vx = ax.vy = ax.vz = 0;
  
  /* オブジェクトをY軸回転させる */
  if((padd & PADRleft)>0) ax.vy -=1*ONE/360;
  
  /* オブジェクトをY軸回転させる */
  if((padd & PADRright)>0) ax.vy +=1*ONE/360;
  
  /* オブジェクトをX軸回転させる */
  if((padd & PADRup)>0) ax.vx+=1*ONE/360;
  
  /* オブジェクトをX軸回転させる */
  if((padd & PADRdown)>0) ax.vx-=1*ONE/360;
  
  /* オブジェクトをZ軸で回す */
  if((padd & PADo)>0) ax.vz+=1*ONE/360/2;
  
  /* オブジェクトをZ軸で回す */
  if((padd & PADn)>0) ax.vz-=1*ONE/360/2;
  
  /* オブジェクトをZ軸にそって動かす */
  if((padd & PADm)>0) dd.vz=100;
  
  /* オブジェクトをZ軸にそって動かす */
  if((padd & PADl)>0) dd.vz= -100;
  
  /* オブジェクトをX軸にそって動かす */
  if((padd & PADLright)>0) dd.vx=20;
  
  /* オブジェクトをX軸にそって動かす */
  if((padd & PADLleft)>0) dd.vx= -20;
  
  /* オブジェクトをY軸にそって動かす */
  if((padd & PADLdown)>0) dd.vy=20;
  
  /* オブジェクトをY軸にそって動かす */
  if((padd & PADLup)>0) dd.vy= -20;

  /* 主観移動をWorldの移動に直す */
  ApplyMatrix(&DView.coord,&dd,&tmpv);
  DView.coord.t[0] += tmpv.vx;
  DView.coord.t[1] += tmpv.vy;
  DView.coord.t[2] += tmpv.vz;

  /* プログラムを終了してモニタに戻る */
/*  if((padd & PADk)>0) {PadStop();ResetGraph(3);StopCallback();return 0;}*/
  if((padd & PADk)>0) {
	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
  }
  
  /* オブジェクトのパラメータからマトリックスを計算し座標系にセット */
  set_shukan_coordinate(&ax,&DView);
  return 1;
}

/* ローテションベクタからマトリックスを作成し座標系にセットする */
set_shukan_coordinate(pos,coor)
SVECTOR *pos;			/* ローテションベクタ */
GsCOORDINATE2 *coor;		/* 座標系 */
{
  MATRIX tmp1;

  /* マトリックスキャッシュをフラッシュする */
  coor->flg = 0;
  
  /* 回転がない場合は何もせずにぬける */
  if(pos->vx==0 && pos->vy==0 && pos->vz==0)
    return;
  
  /* 回転マトリックスを求める */
  RotMatrix(pos,&tmp1);
  
  /* 回転マトリックスを掛ける */
  MulMatrix(&coor->coord,&tmp1);
  
  /* 誤差の蓄積のために 形が歪んでしまうのを整形する */
  MatrixNormal(&coor->coord,&coor->coord);
}

init_all()			/* 初期化ルーチン群 */
{
  ResetGraph(0);		/* GPUリセット */
  PadInit(0);			/* コントローラ初期化 */
  padd=0;			/* コントローラ値初期化 */

#if 0
  GsInitGraph(640,480,GsINTER|GsOFSGPU,1,0);
  /* 解像度設定（インターレースモード） */
  
  GsDefDispBuff(0,0,0,0);	/* ダブルバッファ指定 */
#endif
  GsInitGraph(640,240,GsNONINTER|GsOFSGPU,1,0);
  /* 解像度設定（ノンインターレースモード） */
  GsDefDispBuff(0,0,0,240);   /* ダブルバッファ指定 */
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
  GsInitCoordinate2(WORLD,&DView); /* 視点を設定する座標系の初期化 */
  view.super = &DView;		   /* 視点座標パラメータ設定 */
  
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
  GsSetAmbient(ONE/4,ONE/4,ONE/4);

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
