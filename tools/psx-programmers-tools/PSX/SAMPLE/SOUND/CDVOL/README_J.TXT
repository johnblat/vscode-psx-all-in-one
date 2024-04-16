$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
cdvol: CD ピークレベルメータ

		      Copyright(C) 1997 Sony Computer Entertainment Inc.
						    All Rights Reserved.

<サンプルの解説>

  CD で演奏されている曲を SPU から読み込み、そのボリューム状況を表示さ
  せます。

  これは主として SpuReadDecodedData() と SpuSetTransferCallback() のた
  めのサンプルです。

  **** 注意 ****
  本サンプルプログラムはお手持ちの CD/DA ディスク (一般に売られている
  音楽 CD) を弊社 CD-ROM ドライブ (DTL-H2010 or DTL-H2510) に入れてお
  かないと正常動作致しません。

<起動方法>

  > psymake	コンパイル
  > run main	実行

<コントローラの操作方法>

	なし
