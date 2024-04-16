$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			     Copyright(C) 1998 Sony Computer Entertainment Inc.
			     			 	   All rights reserved.


rots: 6 種類の回転順序によるアニメーション

＜サンプルの解説＞
	当データは、HMD アニメーションにおける 6 種類の回転順序の
	デモンストレーションです。画面には、以下の文字列オブジェクトが
	表示されます。

		XYZ	YXZ	ZXY

		XZY	YZX	ZYX

	これらの文字列は、回転行列の順序をあらわしています。
	例えば、"XYZ" オブジェクトは、X 軸まわり、Y 軸まわり、Z 軸まわりの
	回転行列が適用されていることを示しています。
	これは回転順序としては、最初に Z 軸、次に Y 軸、最後に X 軸の
	まわりに回転することを意味します。

	HMD サンプルプログラムディレクトリの anim/animview を用いて
	再生することができます。

	このデータは、以下の手順で作成されました。

	1. 3D Studio MAX R2 と GAMUT-PSm を用いて、もとになるデータが
	   作成されました。
	2. xhmd を用いて、LAB ファイル "rots.lab" が作成されました。
	3. テキストエディタを用いて、補間プリミティブタイプが編集されました。
	4. 以上の結果から、HMD アセンブラ (labp) により HMD ファイル
	   "rots.hmd" が生成されました。
