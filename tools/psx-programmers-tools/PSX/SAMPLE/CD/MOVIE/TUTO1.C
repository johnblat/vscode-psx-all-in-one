/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *          Movie Sample Program
 *
 *      Copyright (C) 1994,5 by Sony Corporation
 *          All rights Reserved
*/
/*  CD-ROM からムービーをストリーミングするサンプル
 *      1) 縦または横が１６の倍数でない大きさの動画でも再生できる。
 *         ただし、２４ビットモードで再生する際には、
 *         幅と再生位置のX座標は偶数である必要がある。
 *         １６ビットモードで再生する際にはこの制限はない。
 *      2) 複数の動画データをそれぞれのパラメータに従って再生する。     
*/
/* ------------------------------------------------------------------------
 *  1.10        Jul.20 1995 ume
 *                  (tuto0.c ver1.41 からの変更)
 *                  縦または横が１６の倍数でない大きさの動画に対応。
 *                  複数の動画データをそれぞれのパラメータに従って再生
 *                  するように改良。
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

#define TRUE    1
#define FALSE   0

/* MDEC 動画の再生パラメータ */
typedef struct {
    char *fileName;
    int is24bit;
    int startFrame;
    int endFrame;
    int posX;
    int posY;
    int scrWidth;
    int scrHeight;
} MovieInfo;

/*
 * 再生する MDEC 動画
 * 
 *     注意：
 *         ２４ビットモードで再生するときは、posX および 動画データの幅は偶数でなくてはならない。
 *         １６ビットで再生するときはこの制限はない。
 */
static MovieInfo movieInfo[] = {

    /* file name               24bit, start, end,   x,  y, scrW, scrH */

    /* 24 bit */
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,   0, 18,  256,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,   0, 34,  256,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,   0, 50,  256,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,  12, 18,  320,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,  32, 34,  320,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,  52, 50,  320,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,  12, 18,  512,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100, 128, 34,  512,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100, 244, 50,  512,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100,  12, 18,  640,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100, 192, 34,  640,  240},
    {"\\XDATA\\STR\\MOV.STR;1", TRUE,     1, 100, 372, 50,  640,  240},

    /* 16 bit */
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,   0, 18,  256,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,   0, 34,  256,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,   0, 50,  256,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,  12, 18,  320,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,  32, 34,  320,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,  52, 50,  320,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,  12, 18,  512,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100, 128, 34,  512,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100, 244, 50,  512,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100,  12, 18,  640,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100, 192, 34,  640,  240},
    {"\\XDATA\\STR\\MOV.STR;1", FALSE,    1, 100, 372, 50,  640,  240},
};
#define NMOVIE  (sizeof(movieInfo) / sizeof(MovieInfo))

/*  デコードする色数の指定(16bit/24bit) */
#define VRAMPIX(pixels, is24bit)    ((is24bit)? ((pixels) * 3) / 2: (pixels))   /* VRAM 上でのピクセル数 */
#define DCT_MODE(is24bit)           ((is24bit)? 3: 2)      /* decode mode */

/* strNext(),strNextVlc()
   がタイムアウトさせる繰り返しの数 */
#define MOVIE_WAIT 2000

/*  デコード環境変数 */
typedef struct {
    u_long  *vlcbuf[2]; /* VLC buffer (double) */
    int vlcid;          /* 現在 VLC デコード中バッファの ID */
    u_short *imgbuf[2]; /* decode image buffer (double) */
    int imgid;          /* 現在使用中の画像バッファのID */
    RECT    rect[2];    /* VRAM上座標値（ダブルバッファ） */
    int rectid;         /* 現在転送中のバッファ ID */
    RECT    slice;      /* １回の DecDCTout で取り出す領域 */
    int isdone;         /* １フレーム分のデータができたか */
    int is24bit;        /* 24ビットモードかどうか */
} DECENV;
static DECENV   dec;    /* デコード環境の実体 */

/*  ストリーミング用リングバッファ
 *  CD-ROMからのデータをストック
 *  最低２フレーム分の容量を確保する。
 */
#define RING_SIZE   32      /* 単位はセクタ */
static u_long   Ring_Buff[RING_SIZE*SECTOR_SIZE];

/*  VLCの係数テーブルを格納する領域
 *  DecDCTvlcBuild()で展開されて使用する。
 *  展開前は 約3Kbyte 展開後は 64Kbyte
 */
DECDCTTAB  vlc_table;

/*  VLCバッファ（ダブルバッファ）
 *  VLCデコード後の中間データをストック
 */
#define VLC_BUFF_SIZE (320/2*256)     /* とりあえず充分な大きさ */
static u_long   vlcbuf0[VLC_BUFF_SIZE];
static u_long   vlcbuf1[VLC_BUFF_SIZE];

/*  イメージバッファ（ダブルバッファ）
 *  DCTデコード後のイメージデータをストック
 *  横幅16ピクセルの矩形毎にデコード＆転送
 */
#define MAX_SCR_HEIGHT 640
#define SLICE_IMG_SIZE (VRAMPIX(16, TRUE) * MAX_SCR_HEIGHT)
static u_short  imgbuf0[SLICE_IMG_SIZE];
static u_short  imgbuf1[SLICE_IMG_SIZE];

/* 16 の倍数でない大きさの動画を表示するためのマクロ */
#define bound(val, n)               ((((val) - 1) / (n) + 1) * (n))
#define bound16(val)                (bound((val),16))
static int isFirstSlice;    /* 最左端スライスを示すフラグ */

/* その他の変数 */  
static int  Rewind_Switch;  /* 終了フラグ:所定のフレームまで再生すると１になる */

/*  関数のプロトタイプ宣言 */
static int anim(MovieInfo *movie);
static void strSetDefDecEnv(DECENV *dec, int x0, int y0, int x1, int y1, MovieInfo *m);
static void strInit(CdlLOC *loc, void (*callback)(), MovieInfo *m);
static void strCallback();
static int strNextVlc(DECENV *dec, MovieInfo *m);
static void strSync(DECENV *dec, int mode);
static u_long *strNext(DECENV *dec, MovieInfo *m);
static void strKickCD(CdlLOC *loc);

int main( void )
{
    int i;

    ResetCallback();
    CdInit();
    PadInit(0);
    ResetGraph(0);
    SetGraphDebug(0);
    
    while (1) {
        for (i = 0; i < NMOVIE; i++) {
            if(anim(movieInfo + i)==0)
	       return 0;     /* アニメーションサブルーチン */
        }
    }
}


/*  アニメーションサブルーチン フォアグラウンドプロセス */
static int anim(MovieInfo *movie)
{
    DISPENV disp;       /* 表示バッファ */
    DRAWENV draw;       /* 描画バッファ */
    int id;     /* 表示バッファの ID */
    CdlFILE file;
    RECT    clearRect;
    CdlLOC  save_loc;		/* 現在のロケーション */
    int frame_no;		/* 現在のframe_no */
    
    /* 最左端スライスフラグをセット */
    isFirstSlice = 1;
    
    /* ファイルをサーチ */
    if (CdSearchFile(&file, movie->fileName) == 0) {
        printf("file not found\n");
        PadStop();
	ResetGraph(3);
        StopCallback();
	return 0;
    }
    
    /* VRAM上の座標値を設定 */
    strSetDefDecEnv(&dec, VRAMPIX(movie->posX, movie->is24bit), movie->posY,
                VRAMPIX(movie->posX, movie->is24bit), movie->posY+movie->scrHeight, movie);

    /* ストリーミング初期化＆開始 */
    strInit(&file.pos, strCallback, movie);
    
    /* VLCテーブルの展開 */
    DecDCTvlcBuild(vlc_table);
    
    /* 最初のフレームの VLCデコードを行なう */
    while(strNextVlc(&dec, movie) == -1)
      {
	save_loc = file.pos; /* start position */
	strKickCD(&save_loc);
      }
    
    Rewind_Switch = 0;

    SetDispMask(0);		/* 表示禁止 */
    /* 画面クリア */
    setRECT(&clearRect, 0, 0, VRAMPIX(movie->scrWidth, movie->is24bit), movie->scrHeight*2);
    if (movie->is24bit) {
        ClearImage(&clearRect, 0, 0, 0);        /* 24 ビットのときは黒でクリア */
    } else {
        ClearImage(&clearRect, 64, 64, 64); /* 16 ビットのときは灰色でクリア */
    }

    while (1) {
        /* VLCの完了したデータをDCTエンコード開始（MDECへ送信） */
        DecDCTin(dec.vlcbuf[dec.vlcid], DCT_MODE(movie->is24bit));
        
        /* DCTデコード結果の受信の準備をする            */
        /* この後の処理はコールバックルーチンで行なう */
        DecDCTout((u_long *)dec.imgbuf[dec.imgid], dec.slice.w * bound16(dec.slice.h)/2);
        
        /* 次のフレームのデータの VLC デコード */
	while(strNextVlc(&dec, movie)== -1)
	  {
	    frame_no = StGetBackloc(&save_loc);	/* get current location */
	    printf("time out in strNext() %d\n",frame_no);
	    if(frame_no>movie->endFrame || frame_no<=0) /* invalid frame no */
	      save_loc = file.pos; /* start position */
	    strKickCD(&save_loc);
	  }
        
        /* １フレームのデコードが終了するのを待つ */
        strSync(&dec, 0);
        
        /* V-BLNK を待つ */
        VSync(0);
        
        /* 表示バッファをスワップ        */
        /* 表示バッファはデコード中バッファの逆であることに注意 */
        id = dec.rectid? 0: 1;
        SetDefDispEnv(&disp, dec.rect[id].x - VRAMPIX(movie->posX, movie->is24bit),
            dec.rect[id].y - movie->posY,
            VRAMPIX(movie->scrWidth, movie->is24bit), movie->scrHeight);
        /* SetDefDrawEnv(&draw, dec.rect[id].x, dec.rect[id].y, movie->scrWidth*PPW, movie->scrHeight); */
        
        if (movie->is24bit) {
            disp.isrgb24 = movie->is24bit;
            disp.disp.w = disp.disp.w * 2/3;
        }
        PutDispEnv(&disp);
        /* PutDrawEnv(&draw); */
        SetDispMask(1);     /* 表示許可 */
        
        if(Rewind_Switch == 1)
            break;
        
        if(PadRead(1) & PADk)   /* ストップボタンが押されたらアニメーションを
				   抜ける */
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


/* init DECENV    buffer0(x0,y0),buffer1(x1,y1):
 * デコード環境を初期化
 *  x0,y0 デコードした画像の転送先座標（バッファ０）
 *  x1,y1 デコードした画像の転送先座標（バッファ１）
 *
 */
static void strSetDefDecEnv(DECENV  *dec, int x0, int y0, int x1, int y1, MovieInfo *movie)
{
  static int isFirst = 1;

  if(isFirst == 1)
    {
      dec->vlcbuf[0] = vlcbuf0;
      dec->vlcbuf[1] = vlcbuf1;
      dec->vlcid     = 0;

      dec->imgbuf[0] = imgbuf0;
      dec->imgbuf[1] = imgbuf1;
      dec->imgid     = 0;
      dec->rectid    = 0;
      dec->isdone = 0;
      isFirst = 0;
    }
  
    /* rect[]の幅／高さはSTRデータの値によってセットされる */
  dec->rect[0].x = x0;
  dec->rect[0].y = y0;
  dec->rect[1].x = x1;
  dec->rect[1].y = y1;
  dec->slice.w = VRAMPIX(16, movie->is24bit);  
  dec->is24bit = movie->is24bit;   
  
  if(dec->rectid == 0)
    {
      dec->slice.x = x0;
      dec->slice.y = y0;
    }
  else
    {
      dec->slice.x = x1;
      dec->slice.y = y1;
    }
}


/* ストリーミング環境を初期化して開始 */
static void strInit(CdlLOC  *loc, void (*callback)(), MovieInfo *movie)
{
    /* MDEC をリセット */
    DecDCTReset(0);

    /* MDECが１デコードブロックを処理した時のコールバックを定義する */
    DecDCToutCallback(callback);
    
    /* リングバッファの設定 */
    StSetRing(Ring_Buff, RING_SIZE);
    
    /* ストリーミングをセットアップ */
    /* 終了フレーム=∞に設定   */
    StSetStream(movie->is24bit, movie->startFrame, 0xffffffff, 0, 0);
    
    /* ストリーミングリード開始 */
    strKickCD(loc);
}


/*  バックグラウンドプロセス 
 *  (DecDCTout() が終った時に呼ばれるコールバック関数)
 */
static void strCallback()
{
    int mod;
    int id;
    RECT snap_rect;
      
    if (dec.is24bit) {
        extern StCdIntrFlag;
        if(StCdIntrFlag) {
	  StCdInterrupt();  /* RGB24の時はここで起動する */
	  StCdIntrFlag = 0;
        }
    }
/*    
    LoadImage(&dec.slice, (u_long *)dec.imgbuf[dec.imgid]);
*/
    id = dec.imgid;
    snap_rect = dec.slice;
    
    /* 画像デコード領域の切替え */
    dec.imgid = dec.imgid? 0:1;

    /* スライス（短柵矩形）領域をひとつ右に更新 */
    if (isFirstSlice && (mod = dec.rect[dec.rectid].w % dec.slice.w)) {
        dec.slice.x += mod;
        isFirstSlice = 0;
    } else {
        dec.slice.x += dec.slice.w;
    }
    
    /* 残りのスライスがあるか？ */
    if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w) {
        /* 次のスライスをデコード開始 */
        DecDCTout((u_long *)dec.imgbuf[dec.imgid], dec.slice.w * bound16(dec.slice.h)/2);
    }
    /* 最終スライス＝１フレーム終了 */
    else {
        /* 終ったことを通知 */
        dec.isdone = 1;
        isFirstSlice = 1;
        
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
 */
static int strNextVlc(DECENV  *dec, MovieInfo *movie)
{
    int cnt = MOVIE_WAIT;
    u_long  *next;
    static u_long *strNext();
    
    /* データを１フレーム分取り出す */
    while ((next = strNext(dec, movie)) == 0) {
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
static u_long *strNext(DECENV  *dec, MovieInfo *movie)
{
    static int  strWidth  = 0;  /* １つ前のフレームの大きさ（幅） */
    static int  strHeight = 0;  /* １つ前のフレームの大きさ（高さ） */
    u_long      *addr;
    StHEADER    *sector;
    int cnt = MOVIE_WAIT;

    /* データを取り出す（タイムアウト付き） */  
    while(StGetNext((u_long **)&addr,(u_long **)&sector)) {
        if (--cnt == 0)
            return(0);
    }

    /* 現在のフレーム番号が指定値なら終了  */
    if(sector->frameCount >= movie->endFrame) {
        Rewind_Switch = 1;
    }
    
    /* 画面の解像度が前のフレームと違うならば ClearImage を実行 */
    if(strWidth != sector->width || strHeight != sector->height) {
        
        RECT    rect;
        setRECT(&rect, 0, 0, VRAMPIX(movie->scrWidth, movie->is24bit), movie->scrHeight*2);
        if (movie->is24bit) {
            ClearImage(&rect, 0, 0, 0);     /* 24 ビットのときは黒でクリア */
        } else {
            ClearImage(&rect, 64, 64, 64);  /* 16 ビットのときは灰色でクリア */
        }

        strWidth  = sector->width;
        strHeight = sector->height;
    }
    
    /* STRフォーマットのヘッダに合わせてデコード環境を変更する */
    dec->rect[0].w = dec->rect[1].w = VRAMPIX(strWidth, movie->is24bit);
    dec->rect[0].h = dec->rect[1].h = strHeight;
    dec->slice.h   = strHeight;
    
    return(addr);
}


/*  １フレームのデコード終了を待つ
 *  時間を監視してタイムアウトをチェック
 */
static void strSync(DECENV  *dec, int mode /* VOID */)
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
