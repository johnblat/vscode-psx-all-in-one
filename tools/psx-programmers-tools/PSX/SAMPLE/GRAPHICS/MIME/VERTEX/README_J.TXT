$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

vertex:	頂点マイムサンプル プログラム

                      Copyright(C) 1994-1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.
＜サンプルの解説＞
	多重内挿法: MIMe による頂点マイムアニメーションのサンプルです。
	詳細は備考欄を御覧ください。


＜起動方法＞ 
	> psymake all	コンパイル
	> psymake load	必要なデータのロード
	> run main	実行

＜コントローラの操作方法＞
	方向キー：オブジェクトの並行移動
	○,△,×,□:	オブジェクトの回転
	START:          遠ざかる
	SELECT:	        近づく 
	R1:		MIMe 動作１
	R2:		MIMe 動作２
	L1:		MIMe 動作３
	L2:		MIMe 動作４


＜備考＞
* モデリングデータ
	md0.tmd		十字形のモデル基本形		
	md1.tmd		十字形のモデル変形ポーズ１
	md2.tmd		十字形のモデル変形ポーズ２
	md3.tmd		十字形のモデル変形ポーズ３
	md4.tmd		十字形のモデル変形ポーズ４
	md0.vdf		MIMe用差分ファイル(頂点)		
	md0.ndf		MIMe用差分ファイル(法線)

	サンプルプログラムで必要なデータはmd0.tmd,md0.vdf,md0.ndfの３つ。	

* MIMe用差分ファイルmd0.vdf,md0.ndfは以下の処理で生成。
（Programer Tool: mimefilt 参照）

　mimefilt -n md0.tmd md1.tmd md2.tmd md3.tmd md4.tmd　	


  多重内挿法: MIMe (Multiple Inbetweening Method)はキイフレーム内挿法を
拡張したアニメーション作成法で対話的なリアルタイムアニメーションを合成
することができます。

　MIMe：多重内挿法は以下の式で示すように単純な積和演算で構成されています。

     _n          _n
 P = > Wi * Pi   > Wi = 1.0		：原理式 
     ~i=0        ~i=0
		|
          _n	V
 P = P0 + > Wi * dPi  ( dPi = Pi-P0 )	：MIMe sample program. 
          ~i=1

　このサンプルプログラムでは P は tmd ファイル内の頂点および法線
(vertex or normal)で、dPはMIMe用差分ファイル(vdf or ndf)となっています。
		     _n
内挿の積和演算: P0 + > Wi * dPi はgteMIMefuncを用いGTEで実行します。
                     ~i=1
