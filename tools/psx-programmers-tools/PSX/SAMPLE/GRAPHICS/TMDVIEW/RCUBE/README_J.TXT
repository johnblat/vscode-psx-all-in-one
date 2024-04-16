$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

PS-X Demonstration Program "rcube"

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

<サンプルの解説>

・rcube は44個の自転する色つきキューブを表示するプログラムです。
  GsDOBJ2を複数使い、独立して移動させるサンプルプログラムです。

  また、プログラム内で次のような処理を行っています。
	視点の移動
	ＴＭＤデータの切り替え
	フォグの設定

<起動方法>
	>psymake
	>resetps
	>runcube


<コントローラの操作方法>
	
        ○ :	縮小（キューブから離れる）
	△ :	材質の切替え（通常→半透明→グーロー→テクスチャ）
	□ :	FOGのON/OFF
	× :	拡大（キューブに近づく）
        L1 : 	爆発
	R1 : 	爆発のリセット
        SELECT :   終了
	方向キー : キューブ群の回転

<備考>

・ファイルの説明
	makefile.mak	メイクファイル
	runcube.bat	実行用バッチファイル

	main.c		メインルーチン
	table.h		回転を行うためのテーブル
	pos.h		各キューブの初期位置の定義

	rcube.tmd	モデリングデータ（４つのモデルを格納）
	rcube.tim	テクスチャデータ

