$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			Copyright(C) 1997,1998 Sony Computer Entertainment Inc.
			     			 	   All rights reserved.


interp: 様々な補間方法によるアニメーション

＜サンプルの解説＞
	Linear, Bezier, B-Spline の各補間方法によるアニメーションのデモです。
	HMD サンプルプログラムディレクトリの anim/animview を用いて
	再生することができます。

	このデータは、以下の手順で作成されました。

	1. 3D Studio MAX と GAMUT-PSm を用いて、旧 RSD ファイルが作成され
	   ました。
	2. "rsdup" により、新 RSD ファイルが生成されました。
	3. "rsd2hmd" により、上記で生成されたファイルから HMD ファイルが
	   生成されました。
	4. "xhmd" により、5 で生成された HMD ファイルが逆アセンブルされ、
	   編集可能ファイルが生成されました。
	5. テキストエディタで、アニメーションや座標系など、モデルデータ以外の
	   部分が作成され、interp.lab として保存されました。
	6. 5 で生成されたファイルと組み合わせるために、4 で生成されたファイルを
	   をテキストエディタで編集し、*.inc ファイルが作成されました。
	7. "labp" コマンドにより、5 で作られたファイルから HMD ファイルが
	   生成されました。

	1 においては、Nichimen Graphics N-World と PlayStation Express を
	用いて、別のバージョンの RSD も作成されました。
	デフォルトの interp.lab では、3D Studio MAX を用いて作成されたデータを
	用いるようになっていますが、ファイル冒頭の USE_NWORLD_OBJS を 1 にし、
	USE_3DMAX_OBJS を 0 に変更することで、N-World によるデータが
	用いられるようになります。

	画面に表示される 4 つの青い球体は、設定されたキーフレームの位置に
	あります。
	そして、3D Studio MAX によるデータ be.inc (べ), ri.inc (り),
	su.inc (す)
	はそれぞれ、Bezier, Linear, B-Spline で補間されています。
	N-Geometry によるデータの場合は、bezier.inc (Z), linear.inc (L),
	bspline.inc (B) がそれぞれ、Bezier, Linear, B-Spline で補間されて
	います。
