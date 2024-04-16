$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
TrnsRot...関数のサンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

＜サンプルの解説＞
	このプログラムは隣接したポリゴンが異なる座標系に属するとき
	に発生するスキマを解消するための関数TransRot...のサンプルです。
	ここでは、９つの正方形が辺の長さだけずらした座標系に置かれていると
	します。全体を含む座標系（ワールド）が回転していないとき、隣接する
	頂点の座標は一致するので９つの正方形の間にスキマは発生しません。
	しかし、ワールドが回転すると通常のRotTransPersなどでは、
	平行移動量を回転したものが別個に足されるため計算誤差により
	頂点が一致せずスキマが発生する場合があります。
	TransRot...関数は最初に平行移動量を足しておいて次に回転を
	くわえるためスキマが発生しません。

＜起動方法＞
        >psymake 	コンパイル
        >run main       実行

	
＜コントローラの操作方法＞
        ○,△,□,×	：オブジェクトの回転
        →		：TransRot_32によるスキマなし（32bit空間まで計算可）
        ←		：TransRotPersによるスキマなし
        ↑		：RotTransPersによるスキマあり
        ↓		：TransRotPers3によるスキマなし
        L1,R1		：オブジェクトの移動	
        SELECT		：終了


＜備考＞
