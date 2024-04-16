$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
ラウンドメッシュ用関数(RotMeshPrimR_?)のサンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.


＜サンプルの解説＞
	ラウンドメッシュデータを座標変換、透視変換しパケットをつくりＯＴに
	リンクします。ラウンドメッシュデータはTMESH構造体で指定されます。


		    2-------3
		   / \     / \
		  /   \   /   \
		 /     \ /     \
		1-------0-------4
			 \     /
			  \   /
			   \ /
			    5

		ラウンドメッシュデータ


	typedef struct {
        	SVECTOR         *v;             /*shared vertices*/
        	SVECTOR         *n;             /*shared normals*/
        	SVECTOR         *u;             /*shared texture addresses*/
        	CVECTOR         *c;             /*shared colors*/
        	u_long          len;            /*mesh length(=#vertex)*/
	} TMESH;

			TMESH構造体

	上の図では
		len = 6
		v,n,u,cは上の番号順の配列



	tuto0はいろいろなモードを一度に表示します。
	表示するポリゴンの数は一定です。
	G3, F3, FT3, GT3
	GC3,FC3,FCT3,GCT3,T3


＜起動方法＞
	>psymake    	コンパイル
        >psymake load   必要なデータをダウンロード
        >run tuto0      実行

＜コントローラの操作方法＞
	tuto0
       		○		：Y軸まわり回転
        	△		：X軸まわり回転
        	□		：Y軸まわり回転
        	×		：X軸まわり回転
        	→		：X軸方向平行移動
        	←		：X軸方向平行移動
        	↑		：Y軸方向平行移動
        	↓		：Y軸方向平行移動
        	L1		：Z軸方向平行移動
        	L2		：Z軸まわり回転&デプスキューイング OFF
        	R1		：Z軸方向平行移動
        	R2		：Z軸まわり回転&デプスキューイング ON
        	START&SELECT	：backface clip ON/OFF
        	L2&R2SELECT	：終了



