/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	PMD VIEWING ROUTINE
 *
 *
 *		Version 1.00	Jul,  26, 1994
 *
 *		Copyright (C) 1994  Sony Computer Entertainment
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

#define TEX_ADDR   0xa0010000	/* テクスチャデータ（TIMフォーマット）
				   がおかれるアドレス */

#define TIM_HEADER 0x00000010

#define MODEL_ADDR 0xa0040000	/* モデリングデータ（TMDフォーマット）
				   がおかれるアドレス */

#define OT_LENGTH  10		/* オーダリングテーブルの解像度 */


GsOT            Wot[2];		/* オーダリングテーブルハンドラ
				   ダブルバッファのため２つ必要 */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* オーダリングテーブル実体 */

GsDOBJ3		object[OBJECTMAX]; /* オブジェクトハンドラ
				      オブジェクトの数だけ必要 */

u_long          Objnum;		/* モデリングデータのオブジェクトの数を
				   保持する */

/* オブジェクト座標系 */
GsCOORDINATE2   DWorld;


SVECTOR         PWorld; /* 座標系を作るためのローテーションベクター */

extern MATRIX GsIDMATRIX;	/* 単位行列 */

GsRVIEW2  view;			/* 視点を設定するための構造体 */
GsF_LIGHT pslt[3];		/* 平行光源を設定するための構造体 */
u_long padd;			/* コントローラのデータを保持する */

/************* MAIN START ******************************************/
main()
{
  int     i;
  GsDOBJ3 *op;			/* オブジェクトハンドラへのポインタ */
  int     outbuf_idx;		/* double buffer index */
  MATRIX  tmpls;
  
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
	  /* ワールド／ローカルマトリックスを計算する */
	  GsGetLw(op->coord2,&tmpls);
	  
	  /* ライトマトリックスをGTEにセットする */
	  GsSetLightMatrix(&tmpls);
	  
	  /* スクリーン／ローカルマトリックスを計算する */
	  GsGetLs(op->coord2,&tmpls);

	  /* スクリーン／ローカルマトリックスをGTEにセットする */
	  GsSetLsMatrix(&tmpls);
	  
	  /* オブジェクトを透視変換しオーダリングテーブルに登録する */
	  GsSortObject3(op,&Wot[outbuf_idx],14-OT_LENGTH);
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


obj_interactive()
{
  SVECTOR v1;
  MATRIX  tmp1;
  
  /* オブジェクトをY軸回転させる */
  if((padd & PADRleft)>0) PWorld.vy -=5*ONE/360;

  /* オブジェクトをY軸回転させる */
  if((padd & PADRright)>0) PWorld.vy +=5*ONE/360;

  /* オブジェクトをX軸回転させる */
  if((padd & PADRup)>0) PWorld.vx+=5*ONE/360;

  /* オブジェクトをX軸回転させる */
  if((padd & PADRdown)>0) PWorld.vx-=5*ONE/360;
  
  /* オブジェクトをZ軸にそって動かす */
  if((padd & PADm)>0) DWorld.coord.t[2]-=100;
  
  /* オブジェクトをZ軸にそって動かす */
  if((padd & PADl)>0) DWorld.coord.t[2]+=100;

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
  
  /* 再計算のためのフラグをクリアする */
  DWorld.flg = 0;
  return 1;
}

/* 初期化ルーチン群 */
init_all()
{
  ResetGraph(0);		/* GPUリセット */
  PadInit(0);			/* コントローラ初期化 */
  padd=0;			/* コントローラ値初期化 */
  
  GsInitGraph(640,480,GsINTER|GsOFSGPU,1,0);
  /* 解像度設定（インターレースモード） */
    
  GsDefDispBuff(0,0,0,0);	/* ダブルバッファ指定 */
#if 0    
  GsInitGraph(320,240,GsNONINTER|GsOFSGPU,0,0);
  /* 解像度設定（インターレースモード） */
  GsDefDispBuff(0,0,0,240);	/* ダブルバッファ指定 */
#endif
  
  GsInit3D();			/* ３Dシステム初期化 */
  
  Wot[0].length=OT_LENGTH;	/* オーダリングテーブルハンドラに解像度設定 */

  Wot[0].org=zsorttable[0];	/* オーダリングテーブルハンドラに
				   オーダリングテーブルの実体設定 */
    
  /* ダブルバッファのためもう一方にも同じ設定 */
  Wot[1].length=OT_LENGTH;
  Wot[1].org=zsorttable[1];
  
  coord_init();			/* 座標定義 */
  model_init();			/* モデリングデータ読み込み */  
  view_init();			/* 視点設定 */
  light_init();			/* 平行光源設定 */
  
  initTexture1();	        /* texture load of TEX_ADDR */
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


initTexture1()
{
        RECT rect1;
        GsIMAGE tim1;
        u_long *tex_addr;
        int i;

        tex_addr = (u_long *)TEX_ADDR;          /* Top of TIM data address */
        while(1) {
                if(*tex_addr != TIM_HEADER) {
                        break;
                }
                tex_addr++;     /* Skip TIM file header (1word) */
                GsGetTimInfo(tex_addr, &tim1);
                tex_addr += tim1.pw*tim1.ph/2+3+1;    /* Next Texture address*/
                rect1.x=tim1.px;
                rect1.y=tim1.py;
                rect1.w=tim1.pw;
                rect1.h=tim1.ph;
                printf("XY:(%d,%d) WH:(%d,%d)\n",tim1.px,tim1.py,
                                                tim1.pw,tim1.ph);
                LoadImage(&rect1,tim1.pixel);
                DrawSync(0);
                if((tim1.pmode>>3)&0x01) {      /* if clut exist */
                        rect1.x=tim1.cx;
                        rect1.y=tim1.cy;
                        rect1.w=tim1.cw;
                        rect1.h=tim1.ch;
                        LoadImage(&rect1,tim1.clut);
                        DrawSync(0);
                        tex_addr += tim1.cw*tim1.ch/2+3;
                printf("CXY:(%d,%d) CWH:(%d,%d)\n",tim1.cx,tim1.cy,
                                                tim1.cw,tim1.ch);
                }
        }
}


/* モデリングデータの読み込み */
model_init()
{
  u_long *dop;
  GsDOBJ3 *objp;		/* モデリングデータハンドラ */
  int i;
  
  dop=(u_long *)MODEL_ADDR;	/* モデリングデータが格納されているアドレス */

  /* TMDデータとオブジェクトハンドラを接続する */
  Objnum = GsLinkObject3((unsigned long)dop,object);

  for(i=0,objp=object;i<Objnum;i++)
    {	
      /* デフォルトのオブジェクトの座標系の設定 */
      objp->coord2 =  &DWorld;
      
      objp->attribute = 0;	/* init attribute */
      objp++;
    }
}
