$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
fix32: GsSortFixBg32()を用いたBG面表示のサンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

＜サンプルの解説＞

fix32はlibgsのＢＧ面の高速描画関数であるGsSortFixBg32を使用して、スプ
ライトエディタで作成したＢＧデータの内容を表示するプログラムです。
bgsampleよりも多くの面数を表示することが可能です。


＜起動方法＞

>psymake
>psymake load
>run main


＜コントローラの操作方法＞ 

↑	：ＢＧ面数増加
↓	：ＢＧ面数減少

SELECT：	終了


＜備考＞ 

ファイルの説明

makefile.mak	：メイクファイル

main.c		：メインルーチン

sample.tim	：テクスチャデータ
sample.cel	：BGセルデータ
sample0.bgd	：BGマップデータ
sample1.bgd	：  〃
sample2.bgd	：  〃
sample3.bgd	：  〃

