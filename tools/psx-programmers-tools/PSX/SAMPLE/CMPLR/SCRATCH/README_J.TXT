$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

scratch: スクラッチパッド使用サンプル

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.


＜解説＞

スクラッチパッドエリア（CPU内RAM）の使用例サンプルプログラムです。
簡単なメモリアクセステストを行います。

 (A) SCRACH PAD ACCESS 1 ( test0() )
    スクラッチパッドエリアへのアクセスです。
    ポインタを用いた間接アクセスを行っています。

 (B) SCRACH PAD ACCESS 2 ( test1() )
    スクラッチパッドエリアへのアクセスです。
    変数のattribute指定を用いて、直接アクセスを行っています。

 (C) MAIN MEMORY ACCESS ( test2() )
    通常のメインメモリへのアクセスです。

高速に実行されている程、LOOP COUNT の値は大きくなります。


＜起動方法＞ 

  >psymake
  >run main


＜コントローラの操作方法＞

	○又は×：テストの種類が変わります。


＜備考＞

このサンプルは -O0（最適化なし）でコンパイルすると（makefile.mak参照）、
実行速度の速い順に 
    (A) > (B) > (C) 
となりますが、-O3 でコンパイルすると
    (A) = (C) > (B)
となります。
このように、スクラッチパッドエリアを使用する方法が、必ずしも実行速度の向上に
つながるとは言えません。このサンプルは一つの例であり、どの方法がベストで
あるかは、個々のコードや最適化レベル によります。


以上
