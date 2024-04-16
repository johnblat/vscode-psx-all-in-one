$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
ストリップメッシュ用関数(RotMeshPrimS_?)のサンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

＜サンプルの解説＞
	ストリップメッシュデータを座標変換、透視変換しパケットをつくりＯＴに
	リンクします。ストリップメッシュデータはTMESH構造体で指定されます。


		    1-------3-------5-------7-------9
		   / \     / \     / \     / \     /
		  /   \   /   \   /   \   /   \   /
		 /     \ /     \ /     \ /     \ /
		0-------2-------4-------6-------8

			ストリップメッシュデータ


	typedef struct {
        	SVECTOR         *v;             /*shared vertices*/
        	SVECTOR         *n;             /*shared normals*/
        	SVECTOR         *u;             /*shared texture addresses*/
        	CVECTOR         *c;             /*shared colors*/
        	u_long          len;            /*mesh length(=#vertex)*/
	} TMESH;

			TMESH構造体

	上の図では
		len = 10
		v,n,u,cは上の番号順の配列


	tuto0はいろいろなモードごとに表示します。
	表示するポリゴンの数は可変です。
	F3->G3->FT3->FC3->GC3->GT3->FCT3->GCT3->T3┐
	└────────────────────┘
	表示される数字は１秒間に描画しているポリゴンの数です。
	それぞれのモードでポリゴンの数が MAX のときは
	コンパイラの種類、コンパイラオプションなど
	の条件によって６０フレーム／秒に間に合わない場合があります。

	tuto1はいろいろなモードを一度に表示します。
	表示するポリゴンの数は一定です。
	G3, F3, FT3, GT3
	GC3,FC3,FCT3,GCT3,T3

	tuto2は 
	RotMeshPrimS_F3
	RotMeshPrimR_F3 
	RotMeshPrimS_FT3
	RotMeshPrimR_FT3
	を使って正２０面体（イコサ）をつくる例です。

＜起動方法＞
        >psymake   	コンパイル
        >psymake load   必要なデータをダウンロード
        >run tuto0       実行

        >psymake load1   必要なデータをダウンロード
        >run tuto1       実行

        >psymake load2   必要なデータをダウンロード
        >run tuto2       実行

＜コントローラの操作方法＞
	tuto0
       		○	：Y軸まわり回転
        	△	：X軸まわり回転
        	□	：Y軸まわり回転
        	×	：X軸まわり回転
        	→	：ポリゴン数変更
        	←	：ポリゴン数変更
        	↑	：ポリゴンモード変更
        	↓	：ポリゴンモード変更
        	L1	：Z軸方向平行移動
        	L2	：デプスキューイング OFF
        	R1	：Z軸方向平行移動
        	R2	：デプスキューイング ON
        	START&SELECT	：backface clip ON/OFF
        	L1&R1&START	：ホップストップ
		L2&R2&SELECT	：終了

	tuto1
       		○	：Y軸まわり回転
        	△	：X軸まわり回転
        	□	：Y軸まわり回転
        	×	：X軸まわり回転
        	→	：X軸方向平行移動
        	←	：X軸方向平行移動
        	↑	：Y軸方向平行移動
        	↓	：Y軸方向平行移動
        	L1	：Z軸方向平行移動
        	L2	：デプスキューイング OFF
        	R1	：Z軸方向平行移動
        	R2	：デプスキューイング ON
        	START&SELECT	：backface clip ON/OFF
		L2&R2&SELECT	：終了

	tuto2
       		○	：Y軸まわり回転（全体）
        	△	：X軸まわり回転（全体）
        	□	：Y軸まわり回転（全体）
        	×	：X軸まわり回転（全体）
        	→	：テクスチャつきイコサ減少
        	←	：テクスチャつきイコサ増加
        	↑	：イコサ増加
        	↓	：イコサ減少
        	L1	：Z軸まわり回転（全体）
        	L2	：Z軸方向平行移動
        	R1	：Z軸まわり回転（全体）
        	R2	：Z軸方向平行移動
		↑&START：デプスキューイング ON
		↓&START：デプスキューイング OFF
		←&START：デプスキューイング遠ざかる
		→&START：デプスキューイング近づく
		SELECT	：終了

