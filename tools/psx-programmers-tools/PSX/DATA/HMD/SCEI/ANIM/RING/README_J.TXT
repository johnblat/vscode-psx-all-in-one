$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			     Copyright(C) 1998 Sony Computer Entertainment Inc.
			     			 	   All rights reserved.


ring: ほとんど全てのプリミティブタイプを含むリング

＜サンプルの解説＞
	HMD サンプルプログラムディレクトリの anim/animview を用いて
	再生することができます。

	このデータは、以下の手順で作成されました。

	1. LightWave 3D 5.5、AnimationSaver 1.41、PGLeditor、"ring.cfg" を用いて、
	   "ring.lab" が作成されました。
	2. シェルスクリプト "conv.sh" を用いて、"ring.lab" が "tiled.lab" に
	   変換されました。これにより、このサンプルに必要なプリミティブタイプを
	   生成しています。
	3. "labp" を用いて、"tiled.hmd" が、"tiled.lab" から生成されました。

	"mac.def" ファイルには、"XDRV" と "DIVNUM" の二つのマクロがあらかじめ
	定義されています。これらの値を変更することにより、各プリミティブの
	DRV ビットや、DIV や ADV が指定されたときの自動分割パラメータを
	変更することができます。

	画面に表示される灰色の円柱体にはテクスチャで文字が描かれています。
	これらは、対応する半球体のプリミティブタイプを示しています。
