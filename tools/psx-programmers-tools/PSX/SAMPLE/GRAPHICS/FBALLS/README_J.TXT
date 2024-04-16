$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
fballs：GPU 高速化サンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.



＜サンプルの解説＞
	このサンプルプログラムは、balls をもとにした、gpu の描画を高速化する
	サンプルプログラムです。

	balls からの変更点は、

	・display モードを 640x480 にした。

	・ball の sprite を半透明にした。

	です。これによって、balls をインターレースにして、描画ネックになる
	ようにしました。さらに、高速化のため、

	・V-BLNK を短くするために、DISPENV の screen の h に、242 
	  をいれた。これにより、メモリ上の表示位置と、ディスプレイ上の
	　表示位置を変えることによって、V-BLNK を短くし、描画できる
	　時間を長くた。

	・描画領域を縦 464 にして、書き込む量を減らした。
	  これによって、クリップされる領域が広くなり、描画される
	  量が減る。一般の TV では、下の部分は見えない。

	・PutDrawEnv と DrawOTag を同時に行う関数 DrawOTagEnv() を
	　使うことにより、割り込みによるオーバーヘッドを減らした。

	の 3 点を変更しました。

	この変更により、以下のように若干ながら、高速化できます。

			玉の数
	Normal		2021
	Fast		2085	

＜起動方法＞ 
        >psymake	コンパイル
        >run balls      実行

＜コントローラの操作方法＞ 
        ↑,↓       ：玉の数の増減
        L1          ：ポーズ
        SELECT      ：終了

