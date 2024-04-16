$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

clut: clut を使ったテクスチャへのフォグ(デプスキューイング)効果サンプル

                      Copyright(C) 1995-1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.
	

＜サンプルの解説＞
	clut を使ってテクスチャにきれいなフォグ(デプスキューイング)効
	果をかけるサンプルプログラムです。
	/PSX/doc/jp/text/overview/gte.txt に、デプスキューイング効果のため
	の全般的な解説がありますので、そちらもご覧ください。


	・tuto0
		フレームごとにフォグをかけた CLUT を生成し、VRAM 上にロードして
		フォグ効果を出します。この方法は余計な VRAM 領域を必要としませ
		んが、フレーム当たりのフォグ解像度は高くできません。
	
	・tuto1
		あらかじめ数段階のフォグのかかった CLUT を VRAM 上に用意してお
		き、描画するポリゴンごとに適当なものを選択してフォグ効果を出し
		ます。ポリゴンプリミティブ内の clut タグを変更するため、libgs 
		を使った描画には不向きです。
	
	・tuto2
		あらかじめ数段階のフォグのかかった CLUT を VRAM 上に用意してお
		き、OT にぶらさげた DR_MOVE プリミティブで深さに応じた CLUT を 
		CLUT 領域に転送することによって、フォグ効果のかかった描画をおこ
		ないます。LoadImage() を使用しないため tuto0 よりも速く、フォグ
		解像度を高くすることができます。また、libgs を使った描画にも適
		用できます。
	
	・tuto3
		描画するポリゴンごとに対応する深さの CLUT を作成し、 DR_LOAD
		プリミティブによってポリゴン描画の直前に VRAM へ転送し、フォグ
		効果を出します。tuto2 と同じくらい速く、libgs を使った描画にも
		適用できます。また、余計な VRAM 領域を必要としません。

＜起動方法＞ 

	・tuto0
	> psymake tuto0	コンパイル
	> psymake load	必要なデータをダウンロード
	> run tuto0	実行

	・tuto1
	> psymake tuto1	コンパイル
	> psymake load	必要なデータをダウンロード
	> run tuto1	実行

	・tuto2
	> psymake tuto2	コンパイル
	> psymake load	必要なデータをダウンロード
	> run tuto2	実行

	・tuto3
	> psymake tuto3	コンパイル
	> psymake load	必要なデータをダウンロード
	> run tuto3	実行


＜コントローラの操作方法＞
	方向キー：上下左右の移動
        L1,L2：前後の移動
	R1,R2：回転 (tuto0 を除く)
        ○,△,□,×：回転 (tuto0 を除く)

＜備考＞ 
	Z (otz) の値とフォグ係数 p の関係は関数 otz2p() もしくは 
	p2otz() を使って計算することができます。
