$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
				TMD and PMD

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

<サンプルの解説>
	
	- ここには、標準 3D 描画パケット TMD/PMD のビューアを低レベル
	  関数で構成する関するサンプルが収められています。 
	
	- 各プログラムはメニューから起動されます。詳細は menu.h,
	  main.c を参照下さい。
	
	- 検討用に各デモを個別に切り出して使用したい場合は、main.c 内
	  の記述をご参照下さい。
	

	TMD データの処理に関する低レベルの処理は DMPSX による GTE マク
	ロを使用して高速化を行なっています。

	PMD は複数のプリミティブを一括して座標変換・透視変換するための、
	高速な３Ｄオブジェクトの描画を行なうことができます。

	PMD データパッケージは、従来の TMD データと異なり、光源計算に必
	要な情報をもたず、頂点輝度値は前もってプリセットされたものが使用
	されます。リアルタイムの光源計算が必要な場合は TMD データパッケ
	ージを御使用下さい。
	
<起動方法> 
	
	>psymake load
	>psymake main
	>run main

<コントローラの操作方法>
	
	方向キー : カーソルの移動
        ○ :       決定
        SELECT :   終了

<備考>
	- カーソルを「DEMO」に合わせて○を押すとデモが起動されます。
	- カーソルを「HELP」に合わせて○を押すとデモ内のキー配置が表示
	  されます。各デモのキー配置は HELP を参照下さい。

	
