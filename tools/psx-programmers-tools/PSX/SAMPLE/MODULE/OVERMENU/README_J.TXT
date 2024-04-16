$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

module\overmenu : オーバレイについて

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.
                                            Aug 15, 1997 Ver.1.6 by yoshi


＜サンプルの解説＞

PlayStation におけるオーバレイの特徴を一言で表すと、

「親子プロセス間でライブラリを共有し、親（.EXEファイル）は子（.BINファイル）を
　動的に読み込んで実行できる。但し、読み込みはユーザ管理で行う必要がある」

というものです。
このサンプルは、親(menu)が子(balls,rcube,anim)をオーバレイによって呼び出して
実行させるものです。
BALLS.C は、sample\graphics\balls\main.c を若干変更したものです。
RCUBE.C は、sample\graphics\tmdview\rcube\main.c を若干変更したものです。
ANIM.C は、sample\cd\movie\tuto0.c を若干変更したものです。


＜起動方法＞

まず、ccpsx は v1.06 以上、aspsx は v2.10 以上である事を確認して下さい。

DOS> psymake all

でコンパイルし、以下の様な構造のCD-Rを作成して下さい。
データはライブラリ CD-ROM の以下の場所にあります。

	MUTUAL2.SEQ:	\PSX\DATA\SEQ\MUTUAL2.SEQ
	MUTUAL.VH:	\PSX\DATA\VAB\MUTUAL.VH
	MUTUAL.VB:	\PSX\DATA\VAB\MUTUAL.VB
        RCUBE.TIM:      \PSX\DATA\TIM\RCUBE.TIM
        RCUBE.TMD:      \PSX\DATA\TIM\RCUBE.TMD
        MOV.STR:        \XDATA\STR\MOV.STR

ブートさせれば MENU が立ち上がります。
上下キー＆スタートで、音楽を鳴らしたまま BALLS,RCUBE,ANIM が読み込まれ、
オーバレイ実行されます。セレクトで MENU に戻ります。

\ ------+--- SYSTEM.CNF
	|
	|
	+--- OVERMENU --+--- OVERMENU.EXE
	|		+--- BALLS.BIN
	|		+--- RCUBE.BIN
	|		+--- ANIM.BIN
	|
	+--- DATA ------+--- MUTUAL2.SEQ
			+--- MUTUAL.VH
			+--- MUTUAL.VB
			+--- RCUBE.TIM
			+--- RCUBE.TMD
			+--- MOV.STR

file: SYSTEM.CNF（注：イコールの両側に空白を入れる事）
---------------------------------------------------
BOOT = cdrom:\OVERMENU\OVERMENU.EXE;1
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

（１）子プロセス内の関数、変数の扱いについて

  子プロセスのロードはユーザ管理で行う必要があるので、それに伴って
  「子プロセス内に実体を持つ関数＆変数」の扱いにも注意が必要になります。

  (1) 子プロセス内の関数＆変数を、親からアクセスする場合には、前もって
  　　その子プロセスをメモリ上にロードしておく事が必須です。
  
  (2) メモリ上にロードされた子プロセス内の関数＆変数は自由にアクセスできます
      が、他の子プロセスがオーバレイされた時点で、その内容は壊されてしまい
      ます。この事を念頭に置いてコーディングして下さい。


（２）.BINファイルについて

　通常、EXEファイルに含まれるのは、

　ヘッダー
　.rdata セッション　
　.text セッション　
　.data セッション　
　.sdata セッション　

　である。.sbss 及び .bss は含まれない。

　これに対して、アセンブラによって出力される.BINファイルには(MENU.LNK参照)、
　子プロセスの総てのセッションが含まれている。
　このため、通常のEXEファイルに比べて冗長なファイルになっている。
　（.sbss や .bss の部分は全て 0 が入っている）

　ロード時間の短縮、ディスク容量節約等の理由により、このBINファイルの
　「.bss & .sbss 部分」を、ユーザの責任で削除して使用する事も可能である。
　もちろんその場合、ゼロクリアが必要なら、それはプログラム中で行う必要がある。


（３）ファイル一覧

  MAKEFILE.MAK	メイクファイル
  MENU.LNK	リンクファイル 　　　
  ADDRESS.S	アセンブラファイル
  MENU.C	親:MENU ソースファイル
  SND.C		親:MENU ソースファイル
  FONTTEX1.C	親:MENU ソースファイル
  BALLTEX.H	子：BALLS ヘッダファイル
  BALLS.C	子：BALLS ソースファイル
  TABLE.H	子：RCUBE ヘッダファイル
  RCUBE.C	子：RCUBE ソースファイル
  ANIM.C	子：ANIM ソースファイル

	
								以上

