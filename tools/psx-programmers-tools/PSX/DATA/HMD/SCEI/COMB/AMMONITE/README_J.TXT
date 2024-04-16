$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			     Copyright(C) 1998 Sony Computer Entertainment Inc.
			     			 	   All rights reserved.


ammonite: アニメーションと MIMe の組合せサンプル

＜サンプルの解説＞
	このサンプルは、汎用アニメーションにて MIMe パラメータをコントロール
	するデモです。
	HMD サンプルプログラムディレクトリの anim/animview を用いて
	再生することができます。

	このデータは、以下の手順で作成されました。

	1. 基本モデルは、ニチメングラフィックス「N-World」に含まれる
	   「N-Geometry」を用いて作成されました。
	   作成方法は「Self Study Guide」に従ったものです。
	2. モデルデータは、Alias|wavefront の OBJ 形式に変換されました。
	3. OBJ ファイルが NewTek「LightWave 3D」に読み込まれました。
	4. モデルの色やテクスチャ等が修正されました。
	5. MIMe キーを作成するために、3 つのバリエーションが作成されました。
	6. "Animation Saver for PlayStation" を用い、3 つの HMD ファイルが
	   作成されました。
	7. "mkmime" で、MIMe 情報を含む HMD ファイルが作成されました。
	8. "xhmd" を用いて、逆アセンブルすることで LAB ファイルを作成しました。
	9. 手順 8 で作成された LAB ファイルに、アニメーションプリミティブが
	   追加されました。これらのアニメーションプリミティブは、座標値と
	   MIMe パラメータを制御するものです。
