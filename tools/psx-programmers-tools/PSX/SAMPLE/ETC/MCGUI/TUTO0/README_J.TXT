$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

メモリーカードGUIモジュール チュートリアル

                      Copyright(C) 1998 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.


TUTO0:
======

＜サンプルの解説＞
	メモリーカード管理モジュールを使ったサンプル

	スロット１またはスロット２のメモリーカードに
	以下の形式のカードデータをセーブまたはロード
	します。

	ファイル名「HAND」
	使用ブロック数：１
	アニメーション：３枚（グー・チョキ・パー）

＜起動方法＞
	>psymake	コンパイル
	>psymake load	リソースデータアップロード
	>run main	実行

＜コントローラの操作方法＞

	[BALLS画面]

	↑, ↓：SAVE/LOADの選択
	○    ：実行
	SELECT：終了

	[McGUI画面]

	↑, ↓：差込口の選択
	←, →：「はい」「いいえ」の選択
	○    ：実行
	×    ：キャンセル

＜備考＞
	マクロBG(#define BG)の値を変更することで、BGモードを
	変更することができます。
	BG = 0：mcgui.timのタイルパターンによるスクロールBG
	BG = 1：mcgui_bg.timを背景とする静止画BG

