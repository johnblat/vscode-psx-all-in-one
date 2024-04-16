$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

グラフィックスと動画とのコンビネーション

<サンプルの解説>
	動画を BG もしくは テクスチャ領域に転送して BGないしテクスチャとして
	利用するプログラム

	TMDVIEWとANIMをコンビさせたプログラムである。TMDVIEWの方がメインで
	ANIMの方が サブルーチンとして呼び出されている。

	tuto0.cで BGをデファインすると 動画がBGに張られます
	tuto0.cで TEXTURE16をデファインすると 動画がテクスチャとして
	張られます。

	ver3.2 以降のlibpressでは DecDCTvlcSize()が使えるようになります。
	これは VLCのデコードを任意のサイズで中断できるもので movieの
	負荷を分散しコントロールすることができます。

	DecDCTvlcSize()で与えるサイズを大きくすると アニメーションは滑らか
	になりますが ３Dのグラフィックスはコマ落ちします。
	サイズを小さくすると アニメーションはコマ落ちしますが ３Dの部分は
	滑らかに動きます。 ケースバイケースで使い分けて下さい。

<起動方法>
	> psymake	
	> psymake load
	> run tuto0

<コントローラの操作方法>
        ○ :	回転（右）
	△ :	回転（上）
	□ :	回転（左）
	× :	回転（下）
        → :	平行移動（右）
        ← :	平行移動（左）
        ↑ :	平行移動（上）
        ↓ :	平行移動（下）
	R1 :    オブジェクトを手前に移動
        R2 :    オブジェクトを奥に移動
        SELECT : 終了

