/* $PSLibId: Run-time Library Release 4.4$ */
/*			tuto6 simplest sample
 */
/* バックグラウンド動画の再生（フレームダブルバッファ付き）
 *		ただし、VLC のデコードは表で実行する
 */	
/*		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libpress.h>

/* コントローラレコーダ用マクロ */
#define PadRead(x)	myPadRead(x)

/* デコード環境: メインメモリ上の解凍画像バッファはひとつなことに注意 */
typedef struct {
	/* VLC バッファ（ダブルバッファ） */
	u_long	*vlcbuf[2];	
	
	/* 現在 VLC デコード中バッファの ID */
	int	vlcid;		
	
	/* デコード画像バッファ（シングル）*/
	u_long	*imgbuf;	
	
	/* 転送エリア（ダブルバッファ） */
	RECT	rect[2];	
	
	/* 現在転送中のバッファ ID */
	int	rectid;		
	
	/* １回の DecDCTout で取り出す領域 */
	RECT	slice;		
	
} DECENV;
static DECENV		dec;		
	
/* フレームの最後になると 1 になる。 volatile 指定は必須 */
static volatile int	isEndOfFlame;	

/* バックグラウンドプロセス 
 * (DecDCTout() が終った時に呼ばれるコールバック関数)
 */
static void out_callback(void)
{
	/* loadデコード結果をフレームバッファに転送 */
	LoadImage(&dec.slice, dec.imgbuf);
	
	/* 短柵矩形領域をひとつ右に更新 */
	dec.slice.x += 16;

	/* まだ足りなければ、*/
	if (dec.slice.x < dec.rect[dec.rectid].x + dec.rect[dec.rectid].w) {
		/* 次の短柵を受信 */
		DecDCTout(dec.imgbuf, dec.slice.w*dec.slice.h/2);
	}
	/* １フレーム分終ったら、*/
	else {
		/* 終ったことを通知 */
		isEndOfFlame = 1;
		
		/* ID を更新 */
		dec.rectid = dec.rectid? 0: 1;
		dec.slice.x = dec.rect[dec.rectid].x;
		dec.slice.y = dec.rect[dec.rectid].y;
	}
}		

/* フォアグラウンドプロセス */	
/* 以下の作業領域は malloc() を使用して動的に割り付けるべきです。
 * バッファのサイズは DecDCTvlcBufSize() で獲得できます。
 */	
static u_long	vlcbuf0[256*256];	
static u_long	vlcbuf1[256*256];	
static u_long	imgbuf[16*240/2];	/* 短柵１個 */

static void StrRewind(void);
static u_long *StrNext(void);

/* VLC のデコードワード数:
 * VLC デコードが長時間他の処理をブロックすると不都合な場合は、一回の
 * デコードワードの最大値をここで指定する。
 * DecDCTvlc() は VLC_SIZEワードの VLC をデコードすると一旦制御をアプリ
 * ケーションに戻す。
 */
#define VLC_SIZE	1024	

void tuto6(void)
{
	DISPENV	disp;			/* 表示環境 */
	DRAWENV	draw;			/* 描画環境 */
	RECT	rect;			/* LoadImage Rectangle */
	void	out_callback();		/* コールバック */
	int	id;			/* buffer ID */
	u_long	padd;			/* controller response */
	u_long	*next, *StrNext();	/* CD-ROM の代わり */
	int	isvlcLeft;		/* VLC flag */	
	int	isend = 0;		/* end of program */
	DECDCTTAB	table;		/* VLC table */
	
	DecDCTvlcBuild(table);	/* VLC テーブルを生成 */
	isEndOfFlame = 0;	/* 旗を下げる */

	/* フレームバッファをクリア */
	setRECT(&rect, 0, 0, 640,480);
	ClearImage(&rect, 0, 0, 0);

	/* フォントロード */
	FntLoad(960, 256);
	SetDumpFnt(FntOpen(16, 16, 256, 16, 1, 512));
	
	/* デコード構造体に値を設定 */
	dec.vlcbuf[0] = vlcbuf0;
	dec.vlcbuf[1] = vlcbuf1;
	dec.vlcid     = 0;
	dec.imgbuf    = imgbuf;
	dec.rectid    = 0;
	
	setRECT(&dec.rect[0], 0,  32, 256, 176);
	setRECT(&dec.rect[1], 0, 272, 256, 176);
	setRECT(&dec.slice,   0,  32,  16, 176);
		
	/* コールバックを定義する */
	DecDCToutCallback(out_callback);
	
	/* フレームを巻き戻す */
	StrRewind();
	
	/* まず最初の VLC を解く */
	DecDCTvlcSize2(0);
	DecDCTvlc2(StrNext(), dec.vlcbuf[dec.vlcid], table);

	/* メインループ */
	while (isend == 0) {
		/* VLC の結果（ランレベル）を送信する */
		DecDCTin(dec.vlcbuf[dec.vlcid], 0);	
	
		/* ID をスワップ */
		dec.vlcid = dec.vlcid? 0: 1;		

		/* 最初の短柵の受信の準備をする 
		 * ２発目からは、callback() 内で行なう
		 */
		DecDCTout(dec.imgbuf, dec.slice.w*dec.slice.h/2);
	
		/* 次の BS を疑似 CD-ROM より読み出す。*/
		if ((next = StrNext()) == 0)	
			break;
		
		/* vlc の最初の VLC_SIZE ワードをデコードする */ 
		DecDCTvlcSize2(VLC_SIZE);
		isvlcLeft = DecDCTvlc2(next, dec.vlcbuf[dec.vlcid], table);

		/* 経過時間を表示 */
		FntPrint("slice=%d,", VSync(1));
		
		/* データが出来上がるのを待つ */
		do {
			/* vlc の残りの VLC_SIZE ワードをデコードする */ 
			if (isvlcLeft) {
				isvlcLeft = DecDCTvlc2(0, 0, table);
				FntPrint("%d,", VSync(1));
			}

			/* コントローラを監視する */
			if ((padd = PadRead(1)) & PADselect) 
				isend = 1;
			
		} while (isvlcLeft || isEndOfFlame == 0);
		
		FntPrint("%d\n", VSync(1));
		isEndOfFlame = 0;
			
		/* V-BLNK を待つ */
		VSync(0);
		
		/* 表示バッファをスワップ 
		 * 表示バッファは、描画バッファの反対側なことに注意
		 */
		id = dec.rectid? 0: 1;
		
		PutDispEnv(SetDefDispEnv(&disp, 0, id==0? 0:240, 256, 240));
		PutDrawEnv(SetDefDrawEnv(&draw, 0, id==0? 0:240, 256, 240));
		FntFlush(-1);
	}

	/* restore status */
	DrawSync(0);
	DecDCTinSync(0);
	DecDCToutCallback(0);
	DecDCTvlcSize2(0);
	return;
}

/* 次のビットストリームを読み込む（本当は CD-ROM から来る） */
static int frame_no = 0;
static void StrRewind(void)
{
	frame_no = 0;
}

static u_long *StrNext(void)
{
	extern	u_long	*mdec_frame[];

	FntPrint("%4d: %4d byte\n",
		 frame_no,
		 mdec_frame[frame_no+1]-mdec_frame[frame_no]);
	
	if (mdec_frame[frame_no] == 0) 
		return(mdec_frame[frame_no = 0]);
	else
		return(mdec_frame[frame_no++]);
}

