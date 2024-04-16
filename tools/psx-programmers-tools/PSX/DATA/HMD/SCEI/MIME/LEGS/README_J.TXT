$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			Copyright(C) 1997,1998 Sony Computer Entertainment Inc.
			     			 	   All rights reserved.


legs: Joint MIMe による一対の足

＜サンプルの解説＞
	このサンプルは、以下の特徴を持つデモです。
	1. Joint-MIMe
	2. オブジェクトの複製
	3. "meshconv" コマンドによる MESH 構造生成結果
	HMD サンプルプログラムディレクトリの mime/tuto1 を用いて
	再生することができます。

	このデータは、以下の手順で作成されました。

	1. LightWave 3D のサンプルデータの一つから、LightWave 3D 用
	   PlayStation プラグインを用いて、旧 RSD ファイルが作成されました。
	2. "rsdup" により、新 RSD ファイルが生成されました。
	3. "meshconv" により、2 で作成した、新 RSD ファイルに MESH が
	   付加されました。
	4. テキストエディタにより生成された MESH の色が赤に変更されました。
	5. "rsd2hmd" により、上記で生成されたファイルから HMD ファイルが
	   生成されました。
	6. "xhmd" により、5 で生成された HMD ファイルが逆アセンブルされ、
	   編集可能ファイルが生成されました。
	7. 6 で生成されたファイルをテキストエディタで編集し、MIMe キーの
	   生成と、足の複製が行われました。(*.lab, *.inc)
	8. "labp" コマンドにより、7 で作られたファイルから MIMe キーとなる
	   HMD ファイルが生成されました。
	9. "mkmime" コマンドにより、axes.hmd と rpy.hmd が生成されました。
