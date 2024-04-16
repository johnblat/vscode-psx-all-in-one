$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
			     texture animation

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

<サンプルの解説>

	ここには、動画をテクスチャパターンとして使用するサンプルが収め
	られています。 ここでは、メインメモリに１フレーム分の解凍画像
	バッファを持たずにテクスチャパターンを転送する方法がとられてい
	ます。

	動画再生は、cdadd.c の
	
		animInit();
		animPoll();

	によって実行されます。animPoll() は内部で DecDCTvlc() をコール
	しており処理時間がかかります。そのためにコールバック内でこれを
	呼び出すのはお避け下さい。

	また、動画転送に使用する LoadImage() を他の描画処理に優先して
	行なうために、 BreakDraw() 関数を使用しています。現在 
	BreakDraw() は DrawOTag() による描画処理は中断できますが、他の
	LoadImage()/StoreImage() を中断することができません。アプリケー
	ションで他の LoadImage() をバックグラウンドで使用する場合には
	ご注意下さい。
	
	関連するサンプルプログラムとしては、

		sample/graphics/pmd
		sample/cd/movie
		sample/cd/str3d

	を参照下さい。

	詳細は、

		doc/jp/format/pmd.txt
		doc/jp/press/{abstract,function}.txt
		doc/jp/cd/{abstract,function}.txt
<起動方法> 
	
	>resetps 1
	>psymake load
	>psymake tuto0
	>run earth

<コントローラの操作方法>
	↑,↓: オブジェクトを増やす/減らす
        L1 :   オブジェクトを奥に移動 
        L2 :   オブジェクトを手前に移動



