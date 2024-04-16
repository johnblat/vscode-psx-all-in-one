$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
mutual: CD からのサウンドデータの読み込み

		      Copyright(C) 1997 Sony Computer Entertainment Inc.
						    All Rights Reserved.

<サンプルの解説>

  CD からデータを読み込み、2 つの曲を演奏します。さまざまな状態を 
  main.c 内の #define で変更できます。

  **** 注意 ****
  本サンプルプログラムは本来

	\PSX\DATA\VAB\MUTUAL.VH
	\PSX\DATA\VAB\MUTUAL.VB
	\PSX\DATA\SEQ\MUTUAL1.SEQ
	\PSX\DATA\SEQ\MUTUAL2.SEQ

  のデータを CD から読み込むことによって動作させることを目的としていま
  す。このディレクトリにある各ファイルを CD-ROM ファイルとして作成し、
  ご利用下さい。

  コンパイル時に main.c 内で #define するオプションにつきましては、
  main.c の先頭部分のコメントをお読みください。

<起動方法>

  1.  > psymake		コンパイル

  2.a. プログラム中で #define START_PLAY かあるいは #undef WITH_CDROM
       を指定した場合:
      > psymake load	必要なデータを転送

  2.b. プログラム中で #define WITH_CDROM を指定した場合:
       上記ファイルが入った CD を CD-ROM ドライブ (DTL-H2010 or
       DTL-H2510) に挿入

  3.  > run main	実行

<コントローラの操作方法>

	↑:	サウンド 1 のデータ転送
	→:	サウンド 1 の演奏開始
	←:	サウンド 1 の演奏停止
	△:	サウンド 2 のデータ転送
	○:	サウンド 2 の演奏開始
	□:	サウンド 2 の演奏停止
	L1:	ボール減少
	R1:	ボール増加
	SELECT:	終了
