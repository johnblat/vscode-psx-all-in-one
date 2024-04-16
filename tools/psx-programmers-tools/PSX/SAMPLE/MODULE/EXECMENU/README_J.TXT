$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

module\execmenu: EXEファイルの実行

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.
                                            Aug 15, 1997 Ver.1.7 by yoshi


＜サンプルの解説＞

実行中の親EXE（MENU）から、別のEXE（BALLS,RCUBE,ANIM,BALLS2,RCUBE2,ANIM2）を
ロードして実行するサンプルです。
balls\main.c は、sample\graphics\basic\balls.c を若干変更したものです。
rcube\main.c は、sample\graphics\tmdview\rcube\main.c を若干変更したものです。
anim\main.c は、sample\cd\movie\tuto0.c を若干変更したものです。


＜起動方法＞

DOS> psymake all
DOS> cd balls
DOS> psymake all
DOS> cd ..\rcube
DOS> psymake all
DOS> cd ..\anim
DOS> psymake all

でコンパイルし、以下の様な構造のCD-Rを作成して下さい。
データはライブラリ CD-ROM の以下の場所にあります。

	RCUBE.TIM:	\PSX\DATA\TIM\RCUBE.TIM
	RCUBE.TMD:	\PSX\DATA\TMD\RCUBE.TMD
	MOV.STR:	\XDATA\STR\MOV.STR

ブートさせれば MENU が立ち上がります。
上下キー＆スタートで、各EXEファイルが読み込まれ、実行されます。
セレクトで MENU に戻ります。

\ ------+--- SYSTEM.CNF				後述
        |
        |
        +--- EXECMENU --+--- EXECMENU.EXE
        |               +--- BALLS.EXE
        |               +--- RCUBE.EXE
        |               +--- ANIM.EXE
        |               +--- BALLS2.EXE
        |               +--- RCUBE2.EXE
        |               +--- ANIM2.EXE
        |
        +--- DATA ------+--- RCUBE.TIM
                        +--- RCUBE.TMD
                        +--- MOV.STR

file: SYSTEM.CNF（注：イコールの両側に空白を入れる事）
---------------------------------------------------
BOOT = cdrom:\EXECMENU\EXECMENU.EXE;1
TCB = 4
EVENT = 16
STACK = 801fff00
---------------------------------------------------


＜コントローラの操作方法＞

	↑： 子プログラム選択
        ↓： 子プログラム選択
	START： 子プログラム読み込み、実行
        SELECT：子プログラム終了


＜備考＞

（１）スタックポインタ(SP)初期値について

  通常のモジュール(親モジュール)を生成する際は、LIBSN.LIB の中の
  スタートアップルーチンがリンクされます。
  スタートアップルーチンは、起動時にSP初期値(デフォルト0x801ffff8)を設定
  したり、InitHeap()を呼んだりしています。

  しかし、子モジュールを生成する際には、親と同様にSPが設定されたり、
  InitHeap()されたりしてしまうと、親モジュールの使用する領域を壊してしまう
  事が考えられます。
  このため、スタートアップルーチンの代替OBJ、PSX\LIB\none3.obj を用意して
  います。子モジュールには none3.obj をリンクして下さい。
  none3.obj は、SPの設定や InitHeap() を行ないません。
  (詳しくは PSX\DOC\JP\WORD\PSYQ\0intro.doc 参照)


（２）EXEファイル読み込み＆実行について

このサンプルでは、２種類の方法で子EXEの起動を行っています。
以下、それぞれの処理について詳しく説明します。


 [2-1] 親EXEが子EXEをロードする場合

      子EXEには none3.obj をリンクして下さい。

  [2-1-A] CD関数を使用する場合

        サンプルメニューの BALLS,RCUBE,ANIM がこれにあたります。
	CD関数を使用してLoad()と同様の処理をする関数「cdload()」を
	使用します。
	cdload()のソースコードは文末に挙げますので参考にして下さい。	


	＜親EXE＞

	struct EXEC exe;
				[前処理]
	ResetCallback();	内部で_96_remove()を呼んでいる。
	CdInit();		CD関数初期化
	CdSearchFile();		[データファイル読み]
	CdRead();
		:
		:
		:
		:
	cdload("\\GAME.EXE;1",&exe);	[子EXEをロード]
					オリジナル関数。詳細[4-3]。
	DrawSync(0);			[後処理]
	StopRCnt(RCntCNT0);		この他にも、サウンド関連、イベント等、
	StopRCnt(RCntCNT1);		全ての割り込み処理を止めて下さい。
	StopRCnt(RCntCNT2);
	StopRCnt(RCntCNT3);
	ResetGraph(3)			ResetGraph(0)でも良い。
	PadStop();
	StopCallback();			
	exe.s_addr = 0x8017fff0;	[子のスタックポインタ設定]
	exe.s_size = 0;
	EnterCriticalSection():
	Exec(&exe,1,0);			[子EXEを実行]
	ResetCallback();		[再び前処理を実行]
	CdInit();
		:
		:


	＜子EXE＞

	ResetCallback();	[前処理]
	CdInit();
	CdSearchFile();		[データファイル読み]
	CdRead();
		:
		:
		:
		:
	DrawSync(0);		[後処理]
	StopRCnt(RCntCNT0);
	StopRCnt(RCntCNT1);
	StopRCnt(RCntCNT2);
	StopRCnt(RCntCNT3);
	ResetGraph(3)		ResetGraph(0)でも良い。
	PadStop();
	StopCallback();

	return();		[メインからリターンする]



  [2-1-B] 96ファイルシステムを使用する場合

	本サンプル MENU.C では採用していませんが、
	96ファイルシステムを使用する例を挙げておきます。
	96のファイルシステム（Load(),open(),read(),LoadExec()等）を使用
	する場合は、必ず_96_init()をしてから実行し、その後すぐ_96_remove()を
	すること。_96_init()をした状態では、96のファイルのアクセス以外の
        処理は保証されない。_96_remove()をすれば保証される。
	また、_96_init()は処理時間が長いので、多用するとゲームの進行を
	非常に遅くします。前述の [2-1-A] の方法を推奨します。


	＜親EXE＞

	struct EXEC exe;
				[前処理]
	ResetCallback();	内部で_96_remove()を呼んでいる。
	CdInit();		CD関数初期化
				[データファイル読み]
	_96_init();		96のファイルシステムをアクセスする場合。
	read();			_96_init()と_96_remove()でサンドイッチ
	_96_remove();		にする。
		:
		:
		:
		:
	DrawSync(0);		[後処理]
	StopRCnt(RCntCNT0);	この他にも、サウンド関連、イベント等、
	StopRCnt(RCntCNT1);	全ての割り込み処理を止めて下さい。
	StopRCnt(RCntCNT2);
	StopRCnt(RCntCNT3);
	ResetGraph(3)		ResetGraph(0)でも良い。
	PadStop();
	StopCallback();
	_96_init();		StopCallback()の後に_96_init()
	Load("cdrom:\\GAME.EXE;1",&exe);[子EXEをロード]
	exe.s_addr = 0x8017fff0;	[子のスタックポインタ設定]
	exe.s_size = 0;
	EnterCriticalSection():
	Exec(&exe,1,0);		[子EXEを実行]
	ResetCallback();	[再び前処理を実行]
	CdInit();
		:
		:
	

	＜子EXE＞

	ResetCallback();	[前処理]
	CdInit();
				
	_96_init();		[データファイル読み]
	read();			
	_96_remove();		
		:
		:
		:
		:
	DrawSync(0);		[後処理]
	StopRCnt(RCntCNT0);
	StopRCnt(RCntCNT1);
	StopRCnt(RCntCNT2);
	StopRCnt(RCntCNT3);
	ResetGraph(3)		ResetGraph(0)でも良い。
	PadStop();
	StopCallback();

	return();		[メインからリターンする]




 [2-2] メモリ上のEXEを別のEXEに入れ替える場合

      サンプルメニューの BALLS2,RCUBE2,ANIM2 がこれにあたります。
      システムコール LoadExec() を使用します。

	＜PSX.EXE＞

	ResetCallback();	[前処理]
	CdInit();
	CdSearchFile();		[データファイル読み]
	CdRead();
		:
		:
		:
		:
	DrawSync(0);		[後処理]
	StopRCnt(RCntCNT0);	この他にも、サウンド関連、イベント等、
	StopRCnt(RCntCNT1);	全ての割り込み処理を止めて下さい。
	StopRCnt(RCntCNT2);
	StopRCnt(RCntCNT3);
	ResetGraph(3)		ResetGraph(0)でも良い。
	PadStop();
	StopCallback();		StopCallback()の後に_96_init()
	_96_init();
	LoadExec("cdrom:\\GAME.EXE;1",0,0);	[EXEをロード、実行]


	＜GAME.EXE＞

	ResetCallback();	[前処理]
	CdInit();
	CdSearchFile();		[データファイル読み]
	CdRead();
		:
		:
		:
		:		親に戻る場合は、もう一度親をロードする。
	DrawSync(0);		[後処理]
	StopRCnt(RCntCNT0);
	StopRCnt(RCntCNT1);
	StopRCnt(RCntCNT2);
	StopRCnt(RCntCNT3);
	ResetGraph(3)		ResetGraph(0)でも良い。
	PadStop();
	StopCallback();
	_96_init();
	LoadExec("cdrom:\\PSX.EXE;1",0,0);	[EXEをロード、実行]




 [2-3] EXEファイルロード関数　cdload()

      CD関数を用いて、Load()と同様の動作をする関数 cdload() の
      ソースを挙げます。

	（使用例）
	struct EXEC exe;

	CdInit();
	cdload("\\GAME\\GAME.EXE;1",&exe);


cdload.c
-----------------------------------------------------------------
#include <kernel.h>
#include <libcd.h>

#define H_SIZE 2048


int
cdload(file,exep)

char *file;
struct EXEC *exep;
{
  CdlFILE	fp;			/* ファイルの位置・サイズ情報 */
  int	mode = CdlModeSpeed;	
  char hbuf[H_SIZE];			/* EXEヘッダ用バッファ */
  unsigned long offset;			/* 絶対セクタ位置 */
  int cnt,i;
  struct XF_HDR *head;

  for (i = 0; i < 10; i++) {	/* リトライは 10 回 */
    if (CdSearchFile(&fp, file) == 0) 
      continue;

    /* ターゲットポジションを設定 */
    CdControl(CdlSetloc, (u_char *)&(fp.pos), 0);
    cnt = _read1(H_SIZE,(void *)hbuf,mode); 	/* EXEヘッダリード */
    if(cnt)
      continue;

    head = (struct XF_HDR *)hbuf;		
    memcpy((void *)exep, (void *)&head->exec, sizeof(struct EXEC));

    /* LOC情報を絶対セクタに直し、１セクタ（EXEファイルのヘッダ分）
    　インクリメントした後、再びLOC情報に直す */
    offset = CdPosToInt(&fp.pos);
    offset++;
    CdIntToPos(offset,&fp.pos);

    /* ターゲットポジションを設定 */
    CdControlB(CdlSetloc, (u_char *)&(fp.pos), 0);
    /* プログラムをリード */
    cnt = _read1(exep->t_size, (void *)exep->t_addr, mode);

    /* 正常終了ならばブレーク */
    if (cnt == 0)
    	return(0);	/* 成功 */
  }

  return(-1);		/* 失敗 */
}


int
_read1(byte,sectbuf,mode)

long byte;
void *sectbuf;
int mode;
{	
  int nsector,cnt;
	
  nsector = (byte+2047) / 2048;
	
  /* リード開始 */
  CdRead(nsector, sectbuf, mode);
	
  /* リードの裏で通常の処理は実行できる。*/
  /* ここでは、Read が終了するまで残りのセクタ数を監視する */
  while ((cnt = CdReadSync(1, 0)) > 0 ) {
    VSync(0);
  }

  return(cnt);
}
-----------------------------------------------------------------

以上





