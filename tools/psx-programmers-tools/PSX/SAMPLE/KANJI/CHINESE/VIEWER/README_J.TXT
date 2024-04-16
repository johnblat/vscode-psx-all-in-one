$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

viewer	中国語コードビューアプログラム

                             Copyright(C) 1998 Sony Computer Entertainment Inc.
                                                           All rights reserved.


＜サンプルの解説＞
	中国語フォントデータのビューアです。
	メモリ上にダウンロードされた中国語フォントを一覧することができます。

	一つのソースから 15dot フォントデータ用と 8dot フォントデータ用の
	2 種類のビューアを作成します。

	15dot フォントデータには、簡体字と繁体字の 2 種類のフォントデータが
	あり、ダウンロードするデータを変えることにより、それぞれのフォントを
	一覧することができます。
	また、15dot フォントデータ用のビューアでは、選択された文字の拡大および
	Shift-JIS コード、CX(GB) コード番号の表示を行います。

＜起動方法＞
	psymake			view8 および view15 のコンパイル

	psymake loadkan		簡体字フォントデータおよびコード対応表の
				ダウンロード
	run view15		15dot フォントデータ用のビューアを起動

	psymake loadhan		繁体字フォントデータおよびコード対応表の
				ダウンロード
	run view15		15dot フォントデータ用のビューアを起動

	psymake loadhalf	半角フォントデータのダウンロード
	run view8		8dot フォントデータ用のビューアを起動
	

＜コントローラの操作方法＞
	↑,↓		:	画面上下方向スクロール	
	○,△,□,×	:	カーソルの上下左右方向移動
	L1,L2,R1,R2	:	カーソルで選択された文字の拡大とコード表示

	START & SELECT	:	終了


	Key を押し続けることにより、標準速、倍速、4 倍速と
	3 段階にスピードが変化します。
