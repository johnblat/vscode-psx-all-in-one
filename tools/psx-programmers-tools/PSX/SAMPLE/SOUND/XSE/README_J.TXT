$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
xse：単発効果音のサンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

＜サンプルの解説＞
	Key On された効果音に対して、タイルを移動させることにより、画
	面をみながらパンニングやボリュームやピッチベンドをかけることが
	できます。

	main.c の TONENO を 9 にすることにより、vibrato のかかった効果
	音になります。	

＜起動方法＞ 
        >psymake        コンパイル
        >psymake load   必要なデータをダウンロード
        >run main       実行

＜コントローラの操作方法＞
        ↑ : 動かせるタイルの UP
        ↓ : 動かせるタイルの DOWN
	L1 : タイルを右に動かす
	R1 : タイルを左に動かす
	△ : autovol(0 -> 127)
	× : autopan(0 -> 127)
        START：キーオン
        SELECT：キーオフ
