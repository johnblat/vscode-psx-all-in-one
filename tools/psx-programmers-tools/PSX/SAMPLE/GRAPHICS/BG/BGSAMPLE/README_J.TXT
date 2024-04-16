$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
bgsample: BG面表示のサンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

＜サンプルの解説＞

bgsampleはlibgsのＢＧ面の描画関数を使用して、スプライトエディタで作成
したＢＧデータの内容を表示するプログラムです。


＜起動方法＞

>psymake	コンパイル
>psymake load	必要なデータをダウンロード
>run main	実行


＜コントローラの操作方法＞ 

←	：左方向スクロール
→	：右方向スクロール
↓	：下方向スクロール
↑	：上方向スクロール
L1	：左回転※
R1	：右回転※
L2	：縮小※
R2	：拡大※
○	：ＢＧサイズ変更：横幅縮小
□	：ＢＧサイズ変更：横幅拡大
×	：ＢＧサイズ変更：高さ縮小
△	：ＢＧサイズ変更：高さ拡大
SELECT	：終了

※(GsSortFastBG使用時は回転/拡大/縮小の操作は不可)


＜備考＞ 

ファイルの説明

makefile.mak	：メイクファイル

main.c		：メインルーチン

sample.bgd	：BGマップデータ
sample.cel	：BGセルデータ
sample.tim	：テクスチャデータ
