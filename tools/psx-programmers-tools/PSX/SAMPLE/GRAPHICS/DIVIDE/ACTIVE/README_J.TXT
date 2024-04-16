$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

能動的自動分割を行うサンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

<サンプルの解説>


このサンプルは ４角形ポリゴンを ポリゴンの大きさに 応じて
適当な分割数で 分割するものです。

分割時に ポリゴンのすき間を埋める処理を 行なっています。

予め bload earth8.tim 80100000 を行なって テクスチャをロードしておいてから
プログラムをはしらせて下さい。

このプログラムは 速度優先でつくってあるので 歪みを減らしたい場合は	
#define MAX_POLY_WIDTH (1023/4)	/* max polygon distance X */
#define MAX_POLY_HIGHT (511/4)	/* max polygon distance Y */
MAX_POLY_WIDTH,MAX_POLY_HIGHTを小さくして分割数をあげてください。


<起動方法>

	> psymake tuto0
	> run tuto0


<コントローラの操作方法>
        ○ : オブジェクトの回転（右）
	△ : オブジェクトの回転（上）
	□ : オブジェクトの回転（左）
	× : オブジェクトの回転（下）
	方向キー : オブジェクトの平行移動
	R1 : オブジェクトの平行移動（手前）
	R2 : オブジェクトの平行移動（奥）
        SELECT : プログラム終了



						以上
