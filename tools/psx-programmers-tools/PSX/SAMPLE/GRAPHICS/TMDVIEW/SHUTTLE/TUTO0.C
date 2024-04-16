/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	spaceshuttle: sample program
 *
 *	"tuto0.c" main routine
 *
 *		Version 1.00	Mar,  31, 1994
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

#define TEX_ADDR   0x80010000	/* テクスチャデータ（TIMフォーマット）
				   がおかれるアドレス */

#define MODEL_ADDR 0x80040000	/* モデリングデータ（TMDフォーマット）
				   がおかれるアドレス */


#define OT_LENGTH  10		/* オーダリングテーブルの解像度 */


GsOT            Wot[2];		/* オーダリングテーブルハンドラ
				   ダブルバッファのため２つ必要 */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* オーダリングテーブル実体 */

GsDOBJ5		object[OBJECTMAX]; /* オブジェクトハンドラ
				      オブジェクトの数だけ必要 */

unsigned long   Objnum;		/* モデリングデータのオブジェクトの数を
				   保持する */


GsCOORDINATE2   DSpaceShattle,DSpaceHatchL,DSpaceHatchR,DSatt;
/* オブジェクトごとの座標系 */

SVECTOR         PWorld,PSpaceShattle,PSpaceHatchL,PSpaceHatchR,PSatt;
/* オブジェクトごとの座標系を作るための元データ */

GsRVIEW2  view;			/* 視点を設定するための構造体 */
GsF_LIGHT pslt[3];		/* 平行光源を設定するための構造体 */
unsigned long padd;		/* コントローラのデータを保持する */

u_long          preset_p[0x10000];

/************* MAIN START ******************************************/
main()
{
  int     i;
  GsDOBJ5 *op;			/* オブジェクトハンドラへのポインタ */
  int     outbuf_idx;
  MATRIX  tmpls;
  
  ResetCallback();
  init_all();

  GsInitVcount();
  while(1)
    {
      if(obj_interactive()==0)
        return 0;	/* パッドデータから動きのパラメータを入れる */
      GsSetRefView2(&view);	/* ワールドスクリーンマトリックス計算 */
      outbuf_idx=GsGetActiveBuff();/* ダブルバッファのどちらかを得る */
      
      GsClearOt(0,0,&Wot[outbuf_idx]); /* オーダリングテーブルをクリアする */
      
      for(i=0,op=object;i<Objnum;i++)
	{
	  /* スクリーン／ローカルマトリックスを計算する */
	  GsGetLs(op->coord2,&tmpls);
	  /* ライトマトリックスをGTEにセットする */
	  GsSetLightMatrix(&tmpls);
	  /* スクリーン／ローカルマトリックスをGTEにセットする */
	  GsSetLsMatrix(&tmpls);
	  /* オブジェクトを透視変換しオーダリングテーブルに登録する */
	  GsSortObject5(op,&Wot[outbuf_idx],14-OT_LENGTH,getScratchAddr(0));
  	  op++;
	}
      padd=PadRead(0);		/* パッドのデータを読み込む */
      VSync(0);			/* Vブランクを待つ */
      ResetGraph(1);		/* Reset GPU */
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
  
  /* シャトルをY軸回転させる */
  if((padd & PADRleft)>0) PSpaceShattle.vy +=5*ONE/360;
  /* シャトルをY軸回転させる */
  if((padd & PADRright)>0) PSpaceShattle.vy -=5*ONE/360;
  /* シャトルをX軸回転させる */
  if((padd & PADRup)>0) PSpaceShattle.vx+=5*ONE/360;
  /* シャトルをX軸回転させる */
  if((padd & PADRdown)>0) PSpaceShattle.vx-=5*ONE/360;
  
  /* シャトルをZ軸にそって動かす */
  if((padd & PADh)>0)      DSpaceShattle.coord.t[2]+=100;
  
  /* シャトルをZ軸にそって動かす */
  /* if((padd & PADk)>0)      DSpaceShattle.coord.t[2]-=100; */
  
  /* プログラムを終了してモニタに戻る */
/*  if((padd & PADk)>0) {PadStop();ResetGraph(3);StopCallback();return 0;}*/
  if((padd & PADk)>0) {
	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
  }
  
  
  /* シャトルをX軸にそって動かす */
  if((padd & PADLleft)>0) DSpaceShattle.coord.t[0] -=10;
  /* シャトルをX軸にそって動かす */
  if((padd & PADLright)>0) DSpaceShattle.coord.t[0] +=10;

  /* シャトルをY軸にそって動かす */
  if((padd & PADLdown)>0) DSpaceShattle.coord.t[1]+=10;
  /* シャトルをY軸にそって動かす */
  if((padd & PADLup)>0) DSpaceShattle.coord.t[1]-=10;
  
  if((padd & PADl)>0)
    {				/* ハッチを開く */
      object[3].attribute &= 0x7fffffff;	/* 衛星を存在させる */
      /* 右ハッチをZ軸に回転させるためのパラメータをセットする */
      PSpaceHatchR.vz -= 2*ONE/360;
      /* 左ハッチをZ軸に回転させるためのパラメータをセットする */      
      PSpaceHatchL.vz += 2*ONE/360;
      /* 右ハッチをZ軸に回転させるためにパラメータからマトリックスを計算し
	 右ハッチの座標系にセットする */
      set_coordinate(&PSpaceHatchR,&DSpaceHatchR);
      /* 左ハッチをZ軸に回転させるためにパラメータからマトリックスを計算し
	 左ハッチの座標系にセットする */
      set_coordinate(&PSpaceHatchL,&DSpaceHatchL);
    }
  if((padd & PADm)>0)
    {				/* ハッチをとじる */
      PSpaceHatchR.vz += 2*ONE/360;
      PSpaceHatchL.vz -= 2*ONE/360;
      set_coordinate(&PSpaceHatchR,&DSpaceHatchR);
      set_coordinate(&PSpaceHatchL,&DSpaceHatchL);
    }

  if((padd & PADn)>0 && (object[3].attribute & 0x80000000) == 0)
    {				/* 衛星を送出する */
      DSatt.coord.t[1] -= 10;	/* 衛星をX軸に沿って動かすパラメータセット */
      /* 衛星をY軸に沿って回転させるためのパラメータセット */
      PSatt.vy += 2*ONE/360;
      /* パラメータからマトリックスを計算し座標系にセット */
      set_coordinate(&PSatt,&DSatt);
    }
  if((padd & PADo)>0 && (object[3].attribute & 0x80000000) == 0)
    {				/* 衛星を回収する */
      DSatt.coord.t[1] += 10;
      PSatt.vy -= 2*ONE/360;
      set_coordinate(&PSatt,&DSatt);
    }
  /* シャトルのパラメータからマトリックスを計算し座標系にセット */
  set_coordinate(&PSpaceShattle,&DSpaceShattle);
  return 1;
}


init_all()			/* 初期化ルーチン群 */
{
  ResetGraph(0);		/* reset GPU */
  PadInit(0);			/* コントローラ初期化 */
  padd=0;			/* コントローラ値初期化 */
  GsInitGraph(640,480,2,1,0);	/* 解像度設定（インターレースモード） */
  GsDefDispBuff(0,0,0,0);	/* ダブルバッファ指定 */

/*  GsInitGraph(640,240,1,1,0);	/* 解像度設定（ノンインターレースモード） */
/*  GsDefDispBuff(0,0,0,240);	/* ダブルバッファ指定 */

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
  /*
  texture_init(TEX_ADDR);
  */
}

view_init()			/* 視点設定 */
{
  /*---- Set projection,view ----*/
  GsSetProjection(1000);	/* プロジェクション設定 */
  
  /* 視点パラメータ設定 */
  view.vpx = 0; view.vpy = 0; view.vpz = -2000;
  /* 注視点パラメータ設定 */
  view.vrx = 0; view.vry = 0; view.vrz = 0;
  /* 視点の捻りパラメータ設定 */
  view.rz=0;
  /* 視点座標パラメータ設定 */  
  view.super = WORLD;
  /* view.super = &DSatt; */
  
  /* 視点パラメータを群から視点を設定する
     ワールドスクリーンマトリックスを計算する */
  GsSetRefView2(&view);
}


light_init()			/* 平行光源設定 */
{
  /* ライトID０ 設定 */  
  /* 平行光源方向パラメータ設定 */
  pslt[0].vx = 100; pslt[0].vy= 100; pslt[0].vz= 100;
  /* 平行光源色パラメータ設定 */
  pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
  /* 光源パラメータから光源設定 */
  GsSetFlatLight(0,&pslt[0]);
  
  /* ライトID１ 設定 */
  pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= -100;
  pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
  GsSetFlatLight(1,&pslt[1]);
  
  /* ライトID２ 設定 */
  pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
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
  /* SpaceShattle座標系はワールドに直にぶら下がる */
  GsInitCoordinate2(WORLD,&DSpaceShattle);
  /* ハッチ右の座標系はスペースシャトルにぶら下がる */
  GsInitCoordinate2(&DSpaceShattle,&DSpaceHatchL);
  /* ハッチ左の座標系はスペースシャトルにぶら下がる */
  GsInitCoordinate2(&DSpaceShattle,&DSpaceHatchR);
  /* 衛星の座標系はスペースシャトルにぶら下がる */
  GsInitCoordinate2(&DSpaceShattle,&DSatt);
  
  /* ハッチの原点をスペースシャトルのへりに持ってくる */
  DSpaceHatchL.coord.t[0] =  356;
  DSpaceHatchR.coord.t[0] = -356;
  DSpaceHatchL.coord.t[1] = 34;
  DSpaceHatchR.coord.t[1] = 34;
  
  /* 衛星の原点をスペースシャトルの原点からY軸に２０ずらす */
  DSatt.coord.t[1] = 20;
  
  /* マトリックス計算ワークのローテーションベクター初期化 */
  PWorld.vx=PWorld.vy=PWorld.vz=0;
  PSatt = PSpaceHatchR = PSpaceHatchL = PSpaceShattle = PWorld;
  /* スペースシャトルの原点をワールドのZ = 4000に設定 */
  DSpaceShattle.coord.t[2] = 4000;
}

/* マトリックス計算ワークからマトリックスを作成し座標系にセットする */
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
unsigned long addr;
{
  RECT rect1;
  GsIMAGE tim1;
  
  /* TIMデータのヘッダからテクスチャのデータタイプの情報を得る */  
  GsGetTimInfo((unsigned long *)(addr+4),&tim1);
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
  unsigned long *dop;
  GsDOBJ5 *objp;		/* モデリングデータハンドラ */
  int i;
  u_long *oppp;
  
  dop=(u_long *)MODEL_ADDR;	/* モデリングデータが格納されているアドレス */
  dop++;			/* hedder skip */
  
  GsMapModelingData(dop);	/* モデリングデータ（TMDフォーマット）を
				   実アドレスにマップする */
  dop++;
  Objnum = *dop;		/* オブジェクト数をTMDのヘッダから得る */
  dop++;			/* GsLinkObject2でリンクするためにTMDの
				   オブジェクトの先頭にもってくる */
  /* TMDデータとオブジェクトハンドラを接続する */
  for(i=0;i<Objnum;i++)		
    GsLinkObject5((unsigned long)dop,&object[i],i);
  
  oppp = preset_p;
  for(i=0,objp=object;i<Objnum;i++)
    {
      /* デフォルトのオブジェクトの座標系の設定 */
      objp->coord2 =  &DSpaceShattle;
				/* デフォルトのオブジェクトのアトリビュート
				   の設定（表示しない） */
      objp->attribute = GsDOFF;
      oppp = GsPresetObject(objp,oppp);
      objp++;
    }
  
  object[0].attribute &= ~GsDOFF;
  
  object[0].attribute &= ~GsDOFF;	/* 表示オン */
  object[0].coord2    = &DSpaceShattle;	/* スペースシャトルの座標に設定 */
  object[1].attribute &= ~GsDOFF;	/* 表示オン */
  object[1].attribute |= GsALON;	/* 半透明 */
  object[1].coord2    = &DSpaceHatchR;  /* ハッチ右の座標に設定 */
  object[2].attribute &= ~GsDOFF;	/* 表示オン */
  object[2].attribute |= GsALON;	/* 半透明 */
  object[2].coord2    = &DSpaceHatchL;  /* ハッチ左の座標に設定 */
  
  object[3].coord2    = &DSatt;	/* 衛星の座標に設定 */
}
