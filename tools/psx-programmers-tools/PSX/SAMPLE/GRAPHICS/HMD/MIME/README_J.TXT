$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

mime: HMD でのマイムサンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

<サンプルの解説>
	HMD データフォーマットを用いたマイムのサンプルです。

	トリケラトプスのモデルによるサンプルでは、関節の曲げを多重内挿
	します (Joint-MIMe)。内挿方法には、Axes-MIMe と RPY-MIMe の二
	種類があります。トリケラトプスは関節構造をもったワンスキンモデ
	ルです。

	ぷよでのモデルによるサンプルでは、従来型である頂点と法線の多重
	内挿をします (Vertex/Normal-MIMe)。ぷよでは関節構造を持ちませ
	ん。


<起動方法> 

	> psymake all	コンパイル

	モデル #0 (トリケラトプスによる Joint-Axes-MIMe サンプル) の場合
	> psymake load0
	> run tuto0

	モデル #1 (ぷよでによる Vertex/Normal-MIMe サンプル) の場合
	> psymake load1
	> run tuto0

	モデル #2 (トリケラトプスによる Joint-RPY-MIMe サンプル) の場合
	> psymake load2
	> run tuto0

	"run tuto0" の代わりに "run tuto1" とすると、アナログコントローラ
	対応となります。

	モデル #3 (クローン MIMe サンプル) の場合
	> psymake load3
	> run tuto2

	モデル #3 については、tuto2 を用いてください。
	tuto2 を用いた場合、二つのコントローラを用いて操作できます。

<コントローラの操作方法>
	* コントローラ 1:
		L1,L2,R1,R2,→,←,↑,↓: MIMe パラメータ操作
			L1	MIMe 動作 1
			L2	MIMe 動作 2
			R1	MIMe 動作 3
			R2	MIMe 動作 4
			←	MIMe 動作 5 (モデル #1 ぷよでを除く)
			→	MIMe 動作 6 (モデル #1 ぷよでを除く)
			↑	MIMe 動作 7 (モデル #1 ぷよでを除く)
			↓	MIMe 動作 8 (モデル #1 ぷよでを除く)
			画面にそれぞれの MIMe 係数(重み)が表示されます

		○,△,□,×: オブジェクトの回転
		SELECT, START: オブジェクトの Z 軸平行移動

	* アナログコントローラ:
		上記コントローラ1の操作に加え、アナログステイックで以下の
		重み係数が操作できます。

		右ステイックを右	MIMe 動作 1 に加算
		右ステイックを左	MIMe 動作 1 に減算
		右ステイックを上	MIMe 動作 2 に加算
		右ステイックを下	MIMe 動作 2 に減算
		左ステイックを右	MIMe 動作 3 に加算
		左ステイックを左	MIMe 動作 3 に減算
		左ステイックを上	MIMe 動作 4 に加算
		左ステイックを下	MIMe 動作 4 に減算

<備考>
・マイムデータを含む HMD データを扱うプログラム作成時の注意
	マイムプリミティブを持つ HMD を扱う場合には、通常の HMD の初期
	化の他、関数 GsInitRstVtxMIMe()/GsInitRstNrmMIMe() による初期化が
	必要になる場合があります。これらの関数は、頂点リセットマイムと
	法線リセットマイムを使用する場合に呼び出してください。
	GsInitRstVtxMIMe()/GsInitRstNrmMIMe() 関数を呼び出すタイミングは、
	GsScanUnit() 後、GsSortUnit() 前です。

	また、マイムパラメータ(内挿の重み係数)を HMD 内でなくプログラ
	ム側で持つ場合は、パラメータを置くアドレスを指定する必要があり
	ます ("tuto2.c" をご覧下さい)。

	サンプルでは、これらの初期化を model_init() 内でおこなっています。


【追記】
	本サンプルで使用しているモデルデータ「trirpy.hmd」「triaxes.hmd」
	は、米国 Viewpoint DataLabs により作成され、米国 Newtek	Incorporated
	により配布されているデータを元にして作成されました。


								以上
