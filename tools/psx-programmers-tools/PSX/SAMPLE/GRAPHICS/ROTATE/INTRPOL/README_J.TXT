$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

intrpol: 各種の回転の内挿デモ


                      Copyright(C) 1994-1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.
	
＜サンプルの解説＞

	任意角から任意角への回転を、3種類の内挿法で内挿するサンプルです。

	START キーを押すと、内挿がはじまります。
	内挿は、左端の立方体Aから右端の立方体Bへ向けておこなわれます。

	中列の3つの立方体は、それぞれ違う方法によって回転の内挿をおこないます。

	上段：	RPY (ロール・ピッチ・ヨー)角の内挿
	中段：  回転マトリクスの内挿
	下段：	回転軸を使った内挿 (axesmime で使う内挿方式)

	(各内挿法の詳細は下の解説参照)

	内挿をすると、頂点の軌跡も表示されます。

＜起動方法＞ 

	> psymake all	コンパイル
	> run main	実行


＜コントローラの操作方法＞
	START：		内挿の開始
        SELECT：	次のプリセット状態へ
	方向キー,L1,L2：	内挿開始のRPY角Aの操作
        ○,△,□,×,R1,R2：	内挿終点のRPY角Bの操作

＜解説＞
【共通事項】
	内挿関数 func_intrpol() を、 a は初期値、b は終値、p は内挿パ
	ラメータとして、以下のように定義します。

		func_intrpol(a, b, p) = ((1-p) * a) + (p * b)

【RPY角 (ロール・ピッチ・ヨー角) の内挿】
	A のRPY 角 VA (={vax,vay,vaz}) と B のRPY角
	 VB (={vbx,vby,vbz}) を要素ごとに内挿します。すなわち、C のRPY
	角 VC (={vcx,vcy,vcz}) を
		vcx = func_intrpol(vax, vbx, p);
		vcy = func_intrpol(vay, vby, p);
		vcz = func_intrpol(vaz, vbz, p);
	として求めます。

【回転マトリクスの内挿】
	A の回転マトリクス MA (=ma[3][3]) と B の回転マトリクス MB (=mb[3][3])
	を要素ごとに内挿します。

		mt[i][j]= func_intrpol(ma[i][j], mb[i][j], p);
		   (i,j=0..2)

	この単純に内挿したマトリクスはゆがんでいるので、
	回転マトリクスとなるように正規化を施します。

		MC = NormalMatrix(MT);


【回転軸を使った内挿】
	A の回転マトリクス MA (=ma[3][3]) と B の回転マトリクス MB (=mb[3][3])
	を、一つの回転軸を中心とした回転で内挿します。

	ME = EigenMatrix of ( MA^t MB )

	      {1,       0,      0}
	MT =  {0,  cos(k), sin(k)}
	      {0, -sin(k), cos(k)}

	MC = MA ME MT (ME^t)

	この内挿法については graphics/rotate/arot サンプル および 
	graphics/mime/jointを参考にしてください。
	arot サンプルは単位マトリクスから任意角への内挿例が示されてい
	ます。この intrpol サンプルではこれを発展して任意角から任意角
	への内挿をおこなうようにしてあります。
	さらにこの内挿を多重化したもの (joint-axes-mime) が 
	grahics/mime/joint の a_intr.c です。
