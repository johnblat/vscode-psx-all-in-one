$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

SCREEN	VRAM ビューワ


			Copyright(C) 1997,1998 Sony Computer Entertainment Inc.
			     			 	   All rights reserved.


＜サンプルの解説＞
	VRAM の状態を見るビューワです。
	VRAM にロードされているテクスチャや、クラットなどをみることができます。

＜起動方法＞
	psymake 	コンパイル
	run main	実行

＜コントローラの操作方法＞
	○,△,□,× 	： 画面領域の拡大縮小
        L1+→,←,↑,↓ 	： 画面領域の移動
	→,←,↑,↓ 	： 表示領域の移動 (2pixel 単位)
        L2+→,←,↑,↓ 	： 表示領域の移動 (1pixel 単位)

	R1		： DISPLAY 情報の表示

	SELECT		： 終了
