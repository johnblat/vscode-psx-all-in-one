$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
simple(jump) : libsnd を使った SEQ 再生のサンプルプログラム 

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

＜サンプルの解説＞
	simple は、1 曲を無限回演奏します。
	画面には、現在のテンポやボリュームやステータスなどが
	表示されます。基本的な Ss 関数の使用例です。
	波形データはヘッダとボディとに分割し、分割転送を行ないます。
	
	jump は、simple のジャンプテーブル化版です。

＜起動方法＞ 

       (simple)
	> psymake 	コンパイル
	> psymake load  必要なデータをダウンロード
	> run simple    実行

       (jump)
	> psymake 	コンパイル
	> psymake load  必要なデータをダウンロード
	> run jump      実行

＜コントローラの操作方法＞
        △：演奏を一時停止する
        ×：演奏を再開する
        →：クレッシェンド
        ←：デクレッシェンド
        ↑：曲のボリュームを大きくする
        ↓：曲のボリュームを小さくする
        L1：曲の速度をだんだん遅くする
        R1：曲の速度をだんだん速くする
        START：演奏開始
        SELECT：演奏停止
