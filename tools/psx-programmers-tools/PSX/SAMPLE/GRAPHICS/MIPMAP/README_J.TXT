$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
mipmap：mip-map を使用したサンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

＜サンプルの解説＞
	このサンプルは、mip-map を使用したサンプルプログラムです。

	使用するためには GsSortObject4J を使用し、GsFCALL4に下位関数と
	して GsTMDfastTF4LM を登録してから使用します。

	この関数は、TMD データに含まれる、フラットテクスチャ 4 角形に
	たいして、ポリゴンのサイズに応じて、テクスチャをきりかえる
	mip-map を行います。

	テクスチャは V-RAM 上に以下のように配置してください。

		+--------------+-------+
		|              |       |
		|              |       |
		|              |       |
		|              +---+---+
		|              | +-+   |
		|              | +-+---+
		|              |
		+--------------+

	テクスチャのサイズは 1, 1/4, 1/16, 1/256 の 4 段階です。
	どのテクスチャを使用するかは、そのポリゴンの外積値を用いて割りだ
	します。

	また、ポリゴンの頂点は、以下のような順番である必要があります。

		0        1
		+--------+
		|        |
		|        |
		|        |
		|        |
		+--------+
		2        3


＜起動方法＞ 
	>psymake	コンパイル
	>psymake load	必要なデータをダウンロード
	>run tuto0	実行

＜コントローラの操作方法＞
	
        →,←,↑,↓,○,□ ：オブジェクトの回転
	L1                ：回転のリセット
	START	          ：mip-map/original の切り替え
        SELECT            ：終了

＜備考＞
