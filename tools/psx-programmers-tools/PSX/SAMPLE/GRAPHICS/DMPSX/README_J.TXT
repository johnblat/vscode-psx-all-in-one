$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
DMPSXによる高速化サンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.


＜サンプルの解説＞
	tuto0.cはDMPSXを使ってプログラムを高速化する例を示したものです。
	libgteの低レベル関数を使って書いた場合とそれをDMPSXによって
	書き直しさらに最適化した場合のスピードの比較が行なえます。
	
	プログラムはそのままでDMPSX版になっています。先頭にある
		#define DMPSX_MACRO
	をコメントアウトすることによりlibgte低レベル関数版にすることが
	できます。

	画面に表示される数字は計算と描画の合計時間をＨ単位で表したものです。

＜起動方法＞
        >psymake   	コンパイル
        >run tuto0      実行

＜コントローラの操作方法＞
        ○	：Ｙ軸まわり回転
        △	：Ｘ軸まわり回転
        □	：Ｙ軸まわり回転
        ×	：Ｘ軸まわり回転
        →	：フォグニアーの移動
        ←	：フォグニアーの移動
        ↑	：フォグＯＮ
        ↓	：フォグＯＦＦ
        L1	：Ｚ軸まわり回転
        L2	：Ｚ軸方向移動
        R1	：Ｚ軸まわり回転
        R2	：Ｚ軸方向移動
        SELECT	：終了

