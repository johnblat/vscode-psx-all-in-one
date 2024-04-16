$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$


	tuto0:	GsDOBJ5（プリセットパケット）を使った階層座標系のサンプル


                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.


<サンプルの解説>

	このプログラムは 階層座標系を用いたサンプルです。
	階層座標系は シャトルのフレームが 親となり その子どもとして
	左右のハッチ 衛星を定義しています。シャトルの動きには その子供である
	ハッチや 衛星が 連動しますが、子供の動きにシャトルは連動しません。


<起動方法>

	>psymake
	>psymake load
	>run tuto0

<コントローラの操作方法>

        ○ :		シャトルの回転
	△ :		シャトルの回転
	□ :		シャトルの回転
	× :		シャトルの回転
        L1 :		衛星の送出
        L2 :		衛星の送出
	R1 :		ハッチの開閉
	R2：		ハッチの開閉
        SELECT :	プログラムの終了
	方向キー :	シャトルの平行移動

<備考>

【モデリングデータ】

	shuttle1.tmd	シャトルデータ
			ID.0	シャトルのフレーム
			ID.1	ハッチ
			ID.2	ハッチ
			ID.3	衛星
			ID.4	衛星のアンテナ（未使用）


				以上
