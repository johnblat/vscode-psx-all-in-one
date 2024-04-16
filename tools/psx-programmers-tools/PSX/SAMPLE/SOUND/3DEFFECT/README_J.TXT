$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
3deffect : libsnd を使った 3d 効果のサンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.  
						    All Rights Reserved.

<サンプルの解説>
3D 効果音を発音するサンプルです。発音中に、リバーブの on/off や音量の
変更も可能です。ヘッドホンで聴くことを推奨します。表示メニュー内容は以
下の通りです。

	KEY ON AMB		効果音の発音(雨音)
	KEY ON SPATIAL		効果音の発音(雷音)
	MOVE SPAT LOC		効果音(雷音)の発音位置の移動
	KEY ON SPU		libspu による発音(蛙の声)
	DEC AMB VOL		効果音(雨音)の音量ダウン
	INC AMB VOL		効果音(雨音)の音量アップ
	TOGGLE SPU REV		libspu 側のリバーブの on/off
	TOGGLE AMB REV		libsnd 側のリバーブの on/off
	ALL KEY OFF		すべてを KeyOff 

<起動方法> 
	> psymake 	コンパイル
	> psymake load	必要なデータをダウンロード
	> run main	実行

<コントローラの操作方法>
        ×    : メニュー決定,実行
        ↑,↓ : メニュー選択
        START : 終了
