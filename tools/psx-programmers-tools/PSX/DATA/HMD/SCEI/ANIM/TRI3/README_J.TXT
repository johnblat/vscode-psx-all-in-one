$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			Copyright(C) 1997,1998 Sony Computer Entertainment Inc.
			     			 	   All rights reserved.


tri3: アニメーショントリケラトプス

＜サンプルの解説＞
	このサンプルデータは、HMD アニメーションを行うトリケラトプスです。
	HMD サンプルプログラムディレクトリの anim/tuto0 を用いて
	再生することができます。

	当サンプルデータは、以下の手順で作成されました。

	1. LightWave3D のレイアウトエディタを用いて、LightWave3D のサンプル
	   データ "triceratops" に、ボンとアニメーションが付加されました。
	2. NewTek/D-Storm 様により、開発/供給されている LightWave3D 用
	   プレイステーションプラグインで、旧 RSD ファイルが作成されました。
	3. tri.cod, tri.mot, tri.pvt が "lws2rsd" により、手順 1 で作成された
	   LWS ファイルから生成されました。
	4. 旧 RSD フォーマットファイルから、新 RSD ファイル (tri.rsd,
	   tri0.ply, tri0.mat, tri0.grp) を作成するために、"rsdup"
	   が用いられました。
	5. "meshconv" により、tri.msh が生成されました。
	6. 最後に、上記で作成された RSD ファイルと TIM ファイルから
	   "rsd2hmd" を用いて tri.hmd が作成されました。
