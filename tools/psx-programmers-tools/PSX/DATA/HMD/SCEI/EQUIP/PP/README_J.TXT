$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			     Copyright(C) 1998 Sony Computer Entertainment Inc.
			     			 	   All rights reserved.


pp: カメラプリミティブを使用したサンプルデータ

＜サンプルの解説＞
	当データは、HMD の装置プリミティブを使用した、カメラ（視点）やプロ
	ジェクションを設定するデモンストレーションです。
	赤と青の紙飛行機が、6 つの半透明の輪をくぐり抜けながら、跳び周りま
	す。

	HMD サンプルプログラムディレクトリの anim/animview を用いて
	再生することができます。

	カメラの種類によって、以下の 3 つのデモンストレーションがあります。

	1. pp_world
		WORLD 座標系上にカメラを設定し、プロジェクションを設定しま
		す。

	2. pp_fix
		赤い紙飛行機の座標系上にカメラを設定し、プロジェクションを
		設定します。

	3. pp_aim
		赤い紙飛行機から青い紙飛行機に向かってカメラを設定し、プロ
		ジェクションを設定します。


	このデータは、以下の手順で作成されました。

	1. Softimage 3D でモデルデータとアニメーションが作成されました。
	2. xhmd を用いて、LAB ファイル "pp_*.lab" が作成されました。
	3. テキストエディタを用いて、装置プリミティブタイプが編集されました。
	4. 以上の結果から、HMD アセンブラ (labp) により HMD ファイル
	   "pp_*.hmd" が生成されました。
