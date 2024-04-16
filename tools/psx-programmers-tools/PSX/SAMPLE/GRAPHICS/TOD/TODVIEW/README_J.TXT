$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
todview: TODフォーマットのデータを使用したアニメーションのサンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

＜サンプルの解説＞

todview は、TODフォーマットのアニメーションデータを再生する簡単なプロ
グラムの例です。階層構造を持った手のモデリングデータを用いて「じゃんけ
ん」のアニメーションを表示します。

また、プログラム内で次のような処理を行っています。
・視点の移動
・フレームごとの TODデータの読み取りと描画


＜起動方法＞

>psymake	コンパイル
>psymake load	必要なデータをダウンロード
>run main	実行


＜コントローラの操作方法＞ 

←	：視点が X軸のプラス方向に移動
→	：視点が X軸のマイナス方向に移動
↓	：視点が Y軸のプラス方向に移動
↑	：視点が Y軸のマイナス方向に移動
L1	：視点が Z軸のプラス方向に移動
L2	：視点が Z軸のマイナス方向に移動
○, ×, □, △：	アニメーションの再生
START	：視点を初期位置に戻す
SELECT	：終了


＜備考＞ 

ファイルの説明

makefile.mak	：メイクファイル

main.c		：メインルーチン
te.h		：TMD ID テーブル

te.tmd		：モデリングデータ
jaanken.tod	：TODデータ
