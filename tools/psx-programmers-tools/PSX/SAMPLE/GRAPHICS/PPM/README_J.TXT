$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
FLAT TEXTURE 4角形の完全透視変換サンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.



＜サンプルの解説＞
	四角形のピクセルごとに透視変換を行ない、歪まないテクスチャ
	マッピングを行なうサンプルです。
	現在ではjimenサンプルの
	
		draw_general_square

	という関数を使ったほうが高速にテクスチャマッピングできます。

＜起動方法＞
        >psymake    	コンパイル
        >psymake load   必要なデータをダウンロード
        >run main       実行

＜コントローラの操作方法＞
	△,×	:Ｚ軸まわりの回転
	□,○	:Ｙ軸まわりの回転
	L1,R1	:Ｚ軸方向の移動
	↑,↓	:Ｙ軸方向の移動
	→,←	:Ｘ軸方向の移動
	SELECT	:終了

＜備考＞
