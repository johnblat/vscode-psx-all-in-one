$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			     Copyright(C) 1998 Sony Computer Entertainment Inc.
			     			 	   All rights reserved.


tofu: アニメーションと MIMe の組合せサンプル

＜サンプルの解説＞
	このサンプルは、汎用アニメーションにで MIMe パラメータをコントロール
	するデモです。
	HMD サンプルプログラムディレクトリの anim/animview を用いて
	再生することができます。

	このデータは、以下の手順で作成されました。

	1. MIMe キーを作成するために、LightWave 3D を用い、3 つの豆腐状
	   物体のバリエーションが作成されました。
	2. "Animation Saver for PlayStation" を用い、3 つの HMD ファイルが
	   作成されました。
	3. "mkmime" で、MIMe 情報を含む HMD ファイルが作成されました。
	4. "xhmd" を用いて、逆アセンブルすることで LAB ファイルを作成しました。
	5. 手順 4 で作成された LAB ファイルに、アニメーションプリミティブが
	   追加されました。これらのアニメーションプリミティブは、
	   MIMe パラメータのみを制御します。
