$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

GsDOBJ3 型 オブジェクトを使ったTMDビューワー

<サンプルの解説>
	最もsimpleなGsDOBJ3ビューワー

	GsDOBJ3は PMDフォーマットに対応したオブジェクトハンドラです。
	GsSortObject3()は PMDフォーマットのデータを透視変換し
	オーダリングテーブルに割り付けます。
	GsLinkObject3()はPMDデータとオブジェクトハンドラを
	関連付けます。


<起動方法>

	> psymake
	> psymake load
	> run tuto0

<コントローラの操作方法>

        ○ : オブジェクトの回転（右）
	△ : オブジェクトの回転（上）
	□ : オブジェクトの回転（左）
	× : オブジェクトの回転（下）
	方向キー : オブジェクトの平行移動	
	R1 : オブジェクトの平行移動（奥）
	R2 : オブジェクトの平行移動（手前）
        SELECT : プログラム終了


＜備考＞

【モデリングデータ】

		モデリングデータは、以下の１６種類が用意されています。
		ファイル名の gは、 グーローシェーディングデータを
			     tは、 半透明データを
			     lは、 光源計算なしデータを表します。

		モデリングの形状は、すべて立方体でそのうち３面には
		テクスチャは張っておらず、残りの３面には、15bit,8bit,4bit
		のテクスチャが張ってあります。

		このプログラムは、モデリングデータを入れ換えるだけで
		ここにある１６種類すべてのデータをビューイングできます。

		cube3.tmd		フラット３角ポリゴン
		cube3g.tmd		グーロー３角ポリゴン
		cube3gl.tmd		グーロー光源計算なし３角ポリゴン
		cube3gt.tmd		グーロー半透明３角ポリゴン
		cube3gtl.tmd		グーロー半透明光源計算なし３角ポリゴン
		cube3l.tmd		フラット光源計算なし３角ポリゴン
		cube3t.tmd		フラット半透明３角ポリゴン
		cube3tl.tmd		フラット半透明光源計算なし３角ポリゴン

		cube4.tmd		フラット4角ポリゴン
		cube4g.tmd		グーロー4角ポリゴン
		cube4gl.tmd		グーロー光源計算なし4角ポリゴン
		cube4gt.tmd		グーロー半透明4角ポリゴン
		cube4gtl.tmd		グーロー半透明光源計算なし4角ポリゴン
		cube4l.tmd		フラット光源計算なし4角ポリゴン
		cube4t.tmd		フラット半透明4角ポリゴン
		cube4tl.tmd		フラット半透明光源計算なし4角ポリゴン

		また 一般的な モデリングデータ shuttle1.tmdも用意しました。


【テクスチャデータ】
		テクスチャデータは、6種類が用意されている。

		pat4.tim		4bitテクスチャデータ
		pat8.tim		8bitテクスチャデータ
		pat16.tim		16bitテクスチャデータ
		pat4t.tim		4bitテクスチャデータ(半透明）
		pat8t.tim		8bitテクスチャデータ（半透明）
		pat16t.tim		16bitテクスチャデータ（半透明）


