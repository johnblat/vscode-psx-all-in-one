/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *          Movie Sample Program
 *
 *      Copyright (C) 1994,5,6 by Sony Corporation
 *          All rights Reserved
*/
/*  CD-ROM からムービーをストリーミングするサンプル
 *  再生側の処理が間に合わないことでリングバッファが溢れコマ落ちが
 *  起こる場合に CDROMを巻戻しコマ落ちを抑制するサンプル
 *  ただし XAインタリーブした音つきのムービーには使えない
*/
/*   Version    Date
 *  ------------------------------------------
 *  1.00        Mar,29,1996     yutaka
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libcd.h>
#include <r3000.h>
#include <asm.h>
#include <libapi.h>
#include <libpress.h>


#define FILE_NAME   "\\XDATA\\STR\\MOV.STR;1"
#define START_FRAME 1
#define END_FRAME   577
#define POS_X       36
#define POS_Y       24
#define SCR_WIDTH   320
#define SCR_HEIGHT  240

/*  デコードする色数の指定(16bit/24bit) */
#define IS_RGB24    1   /* 0:RGB16, 1:RGB24 */
#if IS_RGB24==1
#define PPW 3/2     /* １ショートワードに何ピクセルあるか  */
#define DCT_MODE    3   /* 24bit モードでデコード */
#else
#define PPW 1       /* １ショートワードに何ピクセルあるか */
#define DCT_MODE    2   /* 16bit モードでデコード */
#endif

/* strNext(),strNextVlc()
   がタイムアウトさせる繰り返しの数 */
#define MOVIE_WAIT 2000

/* デコード環境変数 */
typedef struct {
    u_long  *vlcbuf[2]; /* VLC buffer (double) */
    int vlcid;      /* 現在 VLC デコード中バッファの ID */
    u_short *imgbuf[2]; /* decode image buffer (double) */
    int imgid;      /* 現在使用中の画像バッファのID */
    RECT    rect[2];    /* VRAM上座標値（ダブルバッファ） */
    int rectid;     /* 現在転送中のバッファ ID */
    RECT    slice;      /* １回の DecDCTout で取り出す領域 */
    int isdone;     /* １フレーム分のデータができたか */
} DECENV;
static DECENV   dec;        /* デコード環境の実体 */

/*  ストリーミング用リングバッファ
 *  CD-ROMからのデータをストック
 *  最低２フレーム分の容量を確保する。
 */
#define RING_SIZE   64      /* 単位はセクタ */
static u_long   Ring_Buff[RING_SIZE*SECTOR_SIZE];
/* 1 frame sector size < MINIMUM_FREE_SECTORS << RING_SIZE */
#define MINIMUM_FREE_SECTORS 16

/*  VLCの係数テーブルを格納する領域
 *  DecDCTvlcBuild()で展開されて使用する。
 *  展開前は 約3Kbyte 展開後は 64Kbyte
 */
DECDCTTAB  vlc_table;

/*  VLCバッファ（ダブルバッファ）
 *  VLCデコード後の中間データをストック
 */
#define VLC_BUFF_SIZE 320/2*256     /* とりあえず充分な大きさ */
static u_long   vlcbuf0[VLC_BUFF_SIZE];
static u_long   vlcbuf1[VLC_BUFF_SIZE];

/*  イメージバッファ（ダブルバッファ）
 *  DCTデコード後のイメージデータをストック
 *  横幅16ピクセルの矩形毎にデコード＆転送
 */
#define SLICE_IMG_SIZE 16*PPW*SCR_HEIGHT
static u_short  imgbuf0[SLICE_IMG_SIZE];
static u_short  imgbuf1[SLICE_IMG_SIZE];
    
/* その他の変数 */
static int  StrWidth  = 0;  /* ムービー画像の大きさ（横と縦） */
static int  StrHeight = 0;  
static int  Rewind_Switch;  /* 終了フラグ 所定のフレームまで再生すると１になる */

/*  関数のプロトタイプ宣言 */
static int anim();
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1);
static void strInit(CdlLOC *loc, void (*callback)());
static void strCallback();
static int strNextVlc(DECENV *dec);
static void strSync(DECENV *dec, int mode);
static u_long *strNext(DECENV *dec);
static void strKickCD(CdlLOC *loc);

int main( void )
{
    ResetCallback();
    CdInit();
    PadInit(0);
    ResetGraph(0);
    SetGraphDebug(0);
    
    while(1) {
        if(anim()==0)
	   return 0;     /* アニメーションサブルーチン */
    }
}


/*  アニメーションサブルーチン フォアグラウンドプロセス */
static int anim()
{
    DISPENV disp;       /* 表示バッファ */
    DRAWENV draw;       /* 描画バッファ */
    int id;     /* 表示バッファの ID */
    CdlFILE file;
    short free,over;
    int next_frame;
    CdlLOC backloc;
    CdlLOC  save_loc;		/* 現在のロケーション */
    int frame_no;		/* 現在のframe_no */
    
    /* ファイルをサーチ */
    if (CdSearchFile(&file, FILE_NAME) == 0) {
        printf("file not found\n");
	PadStop();
	ResetGraph(3);
        StopCallback();
        return 0;
    }
    
    /* VRAM上の座標値を設定 */
    strSetDefDecEnv(&dec, POS_X, POS_Y, POS_X, POS_Y+SCR_HEIGHT);

    /* ストリーミング初期化＆開始 */
    strInit(&file.pos, strCallback);
    
    /* VLCテーブルの展開 */
    DecDCTvlcBuild(vlc_table);
    
    /* 最初のフレームの VLCデコードを行なう */
    while(strNextVlc(&dec)== -1)
      {
	printf("time out in strNext()");
	save_loc = file.pos; /* start position */
	strKickCD(&save_loc);
      }
    
    Rewind_Switch = 0;
    
    while (1) {
      StRingStatus(&free,&over); /* added ring buffer status */
      if(free<MINIMUM_FREE_SECTORS)
	{
	  next_frame = StGetBackloc(&backloc); /* get the latest frame
						  loc and id */
	  StSetMask(1,next_frame,0xffffffff); /* masking from cdrom data */
	  strKickCD(&backloc); /* access backward to reduce traslation rate */
	}
      
        /* VLCの完了したデータをDCTデコード開始（MDECへ送信） */
        DecDCTin(dec.vlcbuf[dec.vlcid], DCT_MODE);
        
        /* DCTデコード結果の受信の準備をする            */
        /* この後の処理はコールバックルーチンで行なう */
        DecDCTout((u_long *)dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
        
        /* 次のフレームのデータの VLC デコード */
	while(strNextVlc(&dec)== -1)
	  {
	    frame_no = StGetBackloc(&save_loc);	/* get current location */
	    printf("time out in strNext() %d\n",frame_no);
	    if(frame_no>END_FRAME || frame_no<=0) /* invalid frame no */
	      save_loc = file.pos; /* start position */
	    strKickCD(&save_loc);
	  }
        
        /* １フレームのデコードが終了するのを待つ */
        strSync(&dec, 0);
        
        /* V-BLNK を待つ */
        VSync(0);
        
        /* 表示バッファをスワップ     */
        /* 表示バッファはデコード中バッファの逆であることに注意 */
        id = dec.rectid? 0: 1;
        SetDefDispEnv(&disp, 0, (id)*240, SCR_WIDTH*PPW, SCR_HEIGHT);
/*      SetDefDrawEnv(&draw, 0, (id)*240, SCR_WIDTH*PPW, SCR_HEIGHT);*/
        
#if IS_RGB24==1
        disp.isrgb24 = IS_RGB24;
        disp.disp.w = disp.disp.w*2/3;
#endif
        PutDispEnv(&disp);
/*      PutDrawEnv(&draw);*/
        SetDispMask(1);     /* 表示許可 */
        
        if(Rewind_Switch == 1)
            break;
        
        if(PadRead(1) & PADk)   /* ストップボタンが押されたらアニメーション
				   を抜ける */
            break;
    }
    
    /* アニメーション後処理 */
    DecDCToutCallback(0);
    StUnSetRing();
    CdControlB(CdlPause,0,0);
    if(Rewind_Switch==0) {
       PadStop();
       ResetGraph(3);
       StopCallback();
       return 0;
       }
    else
       return 1;
}


/* init DECENV    buffer0(x0,y0),buffer1(x1,y1) :
 * デコード環境を初期化
 *  x0,y0 デコードした画像の転送先座標（バッファ０）
 *  x1,y1 デコードした画像の転送先座標（バッファ１）
 *
 */
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1)
{

    dec->vlcbuf[0] = vlcbuf0;
    dec->vlcbuf[1] = vlcbuf1;
    dec->vlcid     = 0;

    dec->imgbuf[0] = imgbuf0;
    dec->imgbuf[1] = imgbuf1;
    dec->imgid     = 0;

    /* rect[]の幅／高さはSTRデータの値によってセットされる */
    dec->rect[0].x = x0;
    dec->rect[0].y = y0;
    dec->rect[1].x = x1;
    dec->rect[1].y = y1;
    dec->rectid    = 0;

    dec->slice.x = x0;
    dec->slice.y = y0;
    dec->slice.w = 16*PPW;

    dec->isdone    = 0;
}


/* ストリーミング環境を初期化して開始 */
static void strInit(CdlLOC *loc, void (*callback)())
{
    /* MDEC をリセット */
    DecDCTReset(0);
    
    /* MDECが１デコードブロックを処理した時のコールバックを定義する */
    DecDCToutCallback(callback);
    
    /* リングバッファの設定 */
    StSetRing(Ring_Buff, RING_SIZE);
    
    /* ストリーミングをセットアップ */
    /* 終了フレーム=∞に設定   */
    StSetStream(IS_RGB24, START_FRAME, 0xffffffff, 0, 0);
    
    /* ストリーミングリード開始 */
    strKickCD(loc);
}

/*  バックグラウンドプロセス
 *  (DecDCTout() が終った時に呼ばれるコールバック関数)
 */
static void strCallback()
{
  RECT snap_rect;
  int  id;
  
#if IS_RGB24==1
    extern StCdIntrFlag;
    if(StCdIntrFlag) {
        StCdInterrupt();    /* RGB24の時はここで起動する */
        StCdIntrFlag = 0;
    }
#endif
    
  id = dec.imgid;
  snap_rect = dec.slice;
  
    /* 画像デコード領域の切替え */
    dec.imgid = dec.imgid? 0:1;

    /* スライス（短柵矩形）領域をひとつ右に更新 */
    dec.slice.x += dec.slice.w;
    
    /* 残りのスライスがあるか？ */
    if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w) {
        /* 次のスライスをデコード開始 */
        DecDCTout((u_long *)dec.imgbuf[dec.imgid], dec.slice.w*dec.slice.h/2);
    }
    /* 最終スライス＝１フレーム終了 */
    else {
        /* 終ったことを通知 */
        dec.isdone = 1;
        
        /* 転送先座標値を更新 */
        dec.rectid = dec.rectid? 0: 1;
        dec.slice.x = dec.rect[dec.rectid].x;
        dec.slice.y = dec.rect[dec.rectid].y;
    }
  
  /* デコード結果をフレームバッファに転送 */
  LoadImage(&snap_rect, (u_long *)dec.imgbuf[id]);
}



/*  VLCデコードの実行
 *  次の1フレームのデータのVLCデコードを行なう
 *  strNext()でデータが来ずタイムアウトしたら -1を返す
 */
static int strNextVlc(DECENV *dec)
{
    int cnt = MOVIE_WAIT;
    u_long  *next;
    static u_long *strNext();

    /* データを１フレーム分取り出す */
    while ((next = strNext(dec)) == 0) {
        if (--cnt == 0)
            return -1;
    }
    
    /* VLCデコード領域の切替え */
    dec->vlcid = dec->vlcid? 0: 1;

    /* VLC decode */
    DecDCTvlc2(next, dec->vlcbuf[dec->vlcid],vlc_table);

    /* リングバッファのフレームの領域を解放する */
    StFreeRing(next);

    return;
}

/*  リングバッファからのデータの取り出し
 *  （返り値）  正常終了時＝リングバッファの先頭アドレス
 *          エラー発生時＝NULL
 */
static u_long *strNext(DECENV *dec)
{
    u_long      *addr;
    StHEADER    *sector;
    int     cnt = MOVIE_WAIT;
    static int previous_count=0;

    /* データを取り出す（タイムアウト付き） */
    while(StGetNext((u_long **)&addr,(u_long **)&sector)) {
        if (--cnt == 0)
            return(0);
    }
    
    /* check routine for frame succession */
    if(sector->frameCount != previous_count+1)
      printf("frame skip %d %d \n",sector->frameCount,previous_count+1);
    previous_count = sector->frameCount;

    /* 現在のフレーム番号が指定値なら終了  */
    if(sector->frameCount >= END_FRAME) {
        Rewind_Switch = 1;
    }
    
    /* 画面の解像度が 前のフレームと違うならば ClearImage を実行 */
    if(StrWidth != sector->width || StrHeight != sector->height) {
        
        RECT    rect;
        setRECT(&rect, 0, 0, SCR_WIDTH * PPW, SCR_HEIGHT*2);
        ClearImage(&rect, 0, 0, 0);
        
        StrWidth  = sector->width;
        StrHeight = sector->height;
    }
    
    /* STRフォーマットのヘッダに合わせてデコード環境を変更する */
    dec->rect[0].w = dec->rect[1].w = StrWidth*PPW;
    dec->rect[0].h = dec->rect[1].h = StrHeight;
    dec->slice.h   = StrHeight;
    
    return(addr);
}


/*  １フレームのデコード終了を待つ
 *  時間を監視してタイムアウトをチェック
 */
static void strSync(DECENV *dec, int mode /* VOID */)
{
    volatile u_long cnt = WAIT_TIME;

    /* バックグラウンドプロセスがisdoneを立てるまで待つ */
    while (dec->isdone == 0) {
        if (--cnt == 0) {
            /* 強制的に切替える */
            printf("time out in decoding !\n");
            dec->isdone = 1;
            dec->rectid = dec->rectid? 0: 1;
            dec->slice.x = dec->rect[dec->rectid].x;
            dec->slice.y = dec->rect[dec->rectid].y;
        }
    }
    dec->isdone = 0;
}


/*  CDROMを指定位置からストリーミング開始する */
static void strKickCD(CdlLOC *loc)
{
  u_char param;

  param = CdlModeSpeed;
  
 loop:
  /* 目的地まで Seek する */
  while (CdControl(CdlSetloc, (u_char *)loc, 0) == 0);
  while (CdControl(CdlSetmode, &param, 0) == 0);
  VSync(3);  /* 倍速に切り替えてから ３V待つ必要がある */
    /* ストリーミングモードを追加してコマンド発行 */
  if(CdRead2(CdlModeStream2|CdlModeSpeed|CdlModeRT) == 0)
    goto loop;
}
