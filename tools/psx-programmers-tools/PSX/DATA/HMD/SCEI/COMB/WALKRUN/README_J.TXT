$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

			     Copyright(C) 1998 Sony Computer Entertainment Inc.
			     			 	   All rights reserved.


walkrun: 自動 Joint MIMe により、歩いたり走ったりする足

＜サンプルの解説＞
	このサンプルは、以下の特徴を持つデモです。
	1. 汎用アニメーションにより自動化された Joint-MIMe
	2. SID によるモーション切替 (SID=1:走る, SID=2:歩く)
	HMD サンプルプログラムディレクトリの anim/animview を用いて
	再生することができます。

	このデータは、以下の手順で作成されました。

	1. xhmd を用いて、"mime/legs/axes.hmd" から "mime.lab" が
	   作成されました。
	2. "MIMePr" セクションと汎用アニメーションを "mime.lab" に
	   追加し、"run.lab" と "walk.lab" が作成されました。
	3. これら二つのファイル ("run.lab" と "walk.lab") を合成して、
	   "switch.lab" が作成されました。このとき、SID によるモーション
	   切替えも付加されています。
