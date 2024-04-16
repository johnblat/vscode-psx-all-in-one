/* $PSLibId: Run-time Library Release 4.4$ */
/*   動画BGプログラム: ストリーミング部 */
/*   Ver 1.00  1995 Feb 21
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment
 *		All rights Reserved
*/   
/*  このサブルーチン群は 動画をグラフィックスと組み合わせる時に便利な
 *  サブルーチンです。
 *
 *  メインルーチン tuto0.cから 公開関数を呼んでいます。
 *
 *  このサブルーチンパッケージは 動画の処理を優先的に行ないます。MDECが
 *  休まないように メモリもダブルバッファで持っています。そのため
 *  メインメモリ上に ２画面分の 16bit/pixel イメージデータと それを作るための
 *  ２画面分の VLCバッファをとります。
 *  動画のスピードを優先にせず メインメモリを節約したい場合は イメージデータ
 *  領域 及び VLCバッファ領域のダブルバッファは省略できます。省略した
 *  サブルーチンパッケージは次にリリースする予定です。
 *
 *  メインメモリ上に確保する領域は 動画のサイズによって増減します。
 *  WIDTH HIGHTに動画のサイズをいれ確保する領域の大きさを決定します。
 *  ただし 動画のデータの中に埋め込まれている 動画のサイズが WIDTH,HIGHT
 *  を超えると確保した領域を超えてメモリを破壊し暴走するので かならず
 *  扱う動画の最大のサイズを WIDTH,HIGHTで指定しておく必要があります。
 *
 *
 *  内部関数の cdrom_play()は 特定の場所にシークし アニメーションの再生を
 *  始める関数ですが このプログラムではシーク動作に失敗した時のエラー処理
 *  が完全ではありません。注意して下さい。
 */

/* VLC のデコードワード数
 * VLC デコードが長時間他の処理をブロックすると不都合な場合は、一回の
 * デコードワードの最大値をここで指定する。
 * DecDCTvlc() は VLC_SIZEワードの VLC をデコードすると一旦制御をアプリ
 * ケーションに戻す。
 */
#define VLC_SIZE	2048

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <libsnd.h>

/* 公開関数 *******************/
void init_anim(void);
void start_anim(CdlFILE *,u_long);
void load_image_for_mdec_data(u_long,u_long);
void poll_anim(CdlFILE *);

/* 内部関数 *******************/
static void setup_frame();
static void out_callback();
static u_long *get_frame_movie(u_long);
static cdrom_play(CdlLOC *);


#define WIDTH 320		/* ストリーミングデータの最大の横 */
#define HIGHT 240		/* ストリーミングデータの最大の縦 */
#define SLICE  16		/* タンザクの幅 */



/* デコード環境 */
typedef struct {
        u_long	*vlcbuf[2];	/* VLC buffer (double) */
	int	vlcid;		/* 現在 VLC デコード中バッファの ID */
	u_short	*imgbuf[2];	/* デコード画像バッファ（ダブル）*/
	RECT	rect[2];	/* 転送エリア（ダブルバッファ） */
	int	rectid;		/* 現在転送中のバッファ ID */
	RECT	slice;		/* １回の DecDCTout で取り出す領域 */
} DECENV;

static DECENV	dec;		/* デコード環境の実体 */
static int	MdecFree;	/* MDEC BUSY STATUS */
static int      Frame_ny;   /* STREAMING FLAME DATA NOT READY */
static int      slicew = 0;	/* 画面の横と縦 */
static int      sliceh = 0;
static int      Vlc_size = 0;	/* 一度にVLCするサイズ */

static int      Rewind_Switch;	/* CDが終りまでいくと１になる */
static u_long   EndFrame = 0;	/* ストリーミングを終わらせるフレーム */

#define RING_SIZE 24		/* ストリーミングライブラリで使用される
				   リングバッファのサイズ */

u_long SECT_BUFF[RING_SIZE*SECTOR_SIZE]; /* ストリーミングライブラリで使用
					    されるリングバッファの実体 */


/* バックグラウンドプロセス 
 * (DecDCTout() が終った時に呼ばれるコールバック関数)
 */
static void out_callback()
{
  MdecFree = 1;			/* この関数がコールバックされる時には１枚の
				   絵のデコードが終わっている */
}


/* フォアグラウンドプロセス */	
#define setRECT(r, _x, _y, _w, _h) \
	(r)->x = (_x),(r)->y = (_y),(r)->w = (_w),(r)->h = (_h)


DISPENV	disp;			/* 表示環境 */
u_long	vlcbuf0[WIDTH/2*HIGHT/2];	/* 大きさ適当 */
u_long	vlcbuf1[WIDTH/2*HIGHT/2];	/* 大きさ適当 */
u_short	imgbuf0[WIDTH*HIGHT];	/* 1画面分のフレームバッファ */
u_short	imgbuf1[WIDTH*HIGHT];


void init_anim()		/* 初期化ルーチン群 */
{
  RECT rect;
  int i;
  
  DecDCTReset(0);		/* MDEC をリセット */
  MdecFree = 0;			/* 旗を下げる */
  Frame_ny = 0;
  Rewind_Switch = 0;		/* 巻き戻し０ */
  Vlc_size = 0;
  
  /* デコード構造体に値を設定 */
  dec.vlcbuf[0] = vlcbuf0;
  dec.vlcbuf[1] = vlcbuf1;
  dec.vlcid     = 0;
  dec.imgbuf[0]  = imgbuf0;
  dec.imgbuf[1]  = imgbuf1;
  dec.rectid    = 0;
  setRECT(&dec.rect[0], 0,   0, WIDTH, HIGHT);
  setRECT(&dec.rect[1], 0, HIGHT, WIDTH, HIGHT);
  setRECT(&dec.slice,   0,   0,  SLICE, HIGHT);

  setRECT(&rect, 0, 0, WIDTH, HIGHT*2);	/* clear display buff */
  ClearImage(&rect, 0, 0, 0);

  for(i=0;i<WIDTH*HIGHT;i++)	/* clear imgbuf */
    {
      imgbuf0[i]=0;
      imgbuf1[i]=0;
    }
  
  SsInit();		/* SPU lib init */
  SsSetSerialAttr(SS_SERIAL_A,SS_MIX,SS_SON); /* Audio out enable */
  SsSetSerialVol(SS_SERIAL_A,0x7fff,0x7fff);  /* Audio volume set */
  
  /* ストリーミングライブラリで使われるリングバッファの設定 */
  StSetRing(SECT_BUFF,RING_SIZE);
  
  /* MDECが１デコードブロックを処理した時のコールバックを定義する */
  DecDCToutCallback(out_callback);
}


/* CDROMを倍速でスタートさせストリーミングを開始する
 * CDROMが最後までいったら end_cdrom() がコールバックされる
 *
 * fp          再生するストリーミングファイルのファイルポインタ
 * endFrame    ストリーミングの最後のフレーム番号（本当の終わりよりは
 *             2から3フレーム前に設定しないと フレームが読み飛ばされた時に
 *             終わりが検出できないので注意
 */
void start_anim(fp,endFrame)
CdlFILE *fp;
u_long  endFrame;
{
  int ret_val;
  u_long *frame_addr;
  
  StSetStream(0,1,0xffffffff,0,0);
  
  cdrom_play(&fp->pos);

  EndFrame = endFrame;
  
  /* 最初の１フレームをとってくる */
  while((frame_addr = get_frame_movie(EndFrame))==0);
  DecDCTvlcSize(0);  
  /* VLCデコードを行なう */
  DecDCTvlc(frame_addr, dec.vlcbuf[dec.vlcid]);
  
  /* VLCがデコードされたらリングバッファのそのフレームの領域は
     必要ないのでリングバッファのフレームの領域を解放する */
  StFreeRing(frame_addr);
  
  /* １フレーム全てのデータをMDECへ入力するように設定する */
  DecDCTin(dec.vlcbuf[dec.vlcid], 2);
  
  /* 1フレーム全てのデータをMDECからとり出すように設定する */
  DecDCTout(dec.imgbuf[dec.rectid], slicew*sliceh/2);
  
  /* ID をスワップ */
  dec.vlcid = dec.vlcid? 0: 1;
  
  /* 次のフレームのデータをとってくる もし 次のフレームの
     データがエラーだったら 1が返る */
  /* エラーだったらリングバッファをクリアして
     次のフレームのデータをとってくる */
  while((frame_addr=get_frame_movie(EndFrame))==0);
  DecDCTvlc(frame_addr, dec.vlcbuf[dec.vlcid]);
  StFreeRing(frame_addr);
}

/* 次の１フレームのMOVIEフォーマットのデータをリングバッファからとってくる
 * 1フレーム分のデータが揃っていれば １フレームのデータの先頭アドレスを
 * まだ揃ってなければ NULLを返す
 */
static u_long *get_frame_movie(endFrame)
u_long endFrame;
{
  u_long   *addr;
  StHEADER *sector;
  
  if(StGetNext(&addr,(u_long **)&sector))
    return(0);			/* まだ１フレームのデータがリングバッファ
				   上に揃っていない */
  
  if(sector->frameCount > endFrame)
    Rewind_Switch = 1;
  
  /* 画面の解像度が 前のフレームと違うならば ClearImage を実行 */
  if(slicew != sector->width || sliceh != sector->height) {
    RECT    rect;
    setRECT(&rect, 0, 0, WIDTH, HIGHT);
    ClearImage(&rect, 0, 0, 0);
    slicew  = sector->width;
    sliceh  = sector->height;
  }
  
  /* ミニヘッダに合わせてデコード環境を変更する */
  dec.rect[0].w = dec.rect[1].w = slicew;
  dec.rect[0].h = dec.rect[1].h = sliceh;
  dec.slice.h   = sliceh;
  
  return(addr);
}


/* メインメモリにタンザク順でならんでいるMDECによってデコードされた
 * イメージデータを タンザク毎に VRAMへ転送する
 * 引数は VRAMのアドレスを示す
 */
void load_image_for_mdec_data(xo,yo)
u_long xo,yo;
{
  int i,x;
  RECT tmprect;
  
  for(i=0;i<slicew/16;i++)
    {
      x = i*16;
      tmprect.x = x+xo;
      tmprect.y = yo;
      tmprect.w = 16;
      tmprect.h = sliceh;
      
      LoadImage(&tmprect, (u_long*)(dec.imgbuf[dec.rectid? 0: 1]+x*sliceh));
      DrawSync(0);		/* 転送し終わるまで待つ */
    }
}


/*  次のフレームのデコードを開始する
 *  ステイタスフラグとして MdecFreeとFrame_nyがある
 *  MdecFreeは MDECが画像のでコードをしていない時に１になる
 *  Frame_nyは リングバッファに１フレーム分のデータが用意できていない時に
 *  １になる
 *  
 */
static void setup_frame()
{
  static u_long *frame_addr;
  
  if(Vlc_size == 0)
    {
      if(Frame_ny == 0)
	{
	  dec.rectid = dec.rectid? 0: 1;
	  
	  /* VLC の完了したデータを送信 */
	  DecDCTin(dec.vlcbuf[dec.vlcid], 2);
	  
	  /* 最初のデコードブロックの受信の準備をする */
	  DecDCTout(dec.imgbuf[dec.rectid], slicew*sliceh/2);
	  
	  /* ID をスワップ */
	  dec.vlcid = dec.vlcid? 0: 1;
	  MdecFree = 0;
	}
      
      if((frame_addr=get_frame_movie(EndFrame))==0)
	{
	  Frame_ny = 1;
	  return;
	}
      Frame_ny = 0;
      
      DecDCTvlcSize(VLC_SIZE);
      Vlc_size = DecDCTvlc(frame_addr, dec.vlcbuf[dec.vlcid]);
    }
  else
    Vlc_size = DecDCTvlc(0,0);
  
  if(Vlc_size==0)
    if(StFreeRing(frame_addr))
      printf("FREE ERROR\n");
}

/* ポーリングでアニメーションのルーチンを起動する
 * MDECがデータのデコード中で次のフレームのVLCも完了している場合 すなわち
 * MdecFree == 0 かつ Frame_ny == 0 の場合は 何もしないで抜ける
 */
void poll_anim(fp)
CdlFILE *fp;
{
  if(Rewind_Switch) /* 巻き戻しが設定されていたら 最初にジャンプ */
    {
      cdrom_play(&fp->pos);
      Rewind_Switch = 0;
    }
  
  /* ストリーミングルーチンを起動する必要がある場合は setup_frame()を呼ぶ */
  if(MdecFree || Frame_ny || Vlc_size)
    setup_frame();
}


/* locにシークしアニメーションをスタートさせる
 * ただし CdRead2()でシークに失敗した場合のエラー処理が入っていないので
 * 注意すること（製品版では 入れる必要がある）
 */
static cdrom_play(CdlLOC *loc)
{
  u_char param;

  param = CdlModeSpeed;
  
 loop:
  /* 目的地まで Seek する */
  while (CdControl(CdlSetloc, (u_char *)loc, 0) == 0);
  while (CdControl(CdlSetmode, &param, 0) == 0);
  VSync(3);  /* 倍速に切り替えてから ３V待つ必要がある */
    /* ストリーミングモードを追加してコマンド発行 */
  if(CdRead2(CdlModeStream|CdlModeSpeed|CdlModeRT) == 0)
    goto loop;
}
