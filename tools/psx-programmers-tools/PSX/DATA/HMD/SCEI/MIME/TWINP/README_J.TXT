$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			Copyright(C) 1997,1998 Sony Computer Entertainment Inc.
			     			 	   All rights reserved.


twinp: 双子のぷよで

＜サンプルの解説＞
	このサンプルは、以下の特徴を持つデモです。
	1. MIMe オブジェクトの複製
	2. "MIMeID" の使い方
	HMD サンプルプログラムディレクトリの mime/tuto2 を用いて
	再生することができます。

	このデータは、以下の手順で作成されました。

	1. "xhmd" により、../puyo.hmd を逆アセンブルし、編集可能ファイルが
	   生成されました。
	2. テキストエディタで、MIMe キーの設定、オブジェクトの複製、
	   "MIMeID" の設定のため、1 で生成されたファイルが編集されました。
	3. "labp" コマンドにより、2 で作られたファイルから MIMe キーとなる
	   HMD ファイルが生成されました。
	4. "mkmime" コマンドにより、twinp.hmd が生成されました。
