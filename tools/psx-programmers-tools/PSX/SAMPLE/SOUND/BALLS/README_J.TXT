$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
balls：サウンドとグラフィックスを組み合わせたサンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

＜サンプルの解説＞
	サウンドとグラフィックスを組み合わせたサンプルです。
	グラフィックスは sample\graphics\balls を使用しています。
	ボールが壁にぶつかった時、それぞれに割り当てた効果音が鳴ります。
	バックグラウンドでは曲(SEQ)が流れています。

＜起動方法＞ 
        >psymake	コンパイル
        >psymake load   必要なデータをダウンロード
        >run main      実行

＜コントローラの操作方法＞
        ○：コンガ(高)
	△：タムタム(高)
	□：コンガ(低)
	×：フロアタム
        →：ハンドクラップ
	←：バスドラム
	↑：汽笛(低)
	↓：汽笛(高)
        L1：人の声(「ごめんね」)
        R1：タムタム（低）
        SELECT：終了
