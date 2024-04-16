$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
basic	SEQ,SEP 再生サンプルプログラム

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

＜サンプルの解説＞
	SEQ および SEP データの演奏をします。
	SEP である sample.sep は、

		brahms.seq
		sonata_loop.seq （曲中繰り返しデータが含まれています）
		fuga.seq

	を連結したものです。


＜起動方法＞ 
        >psymake 	コンパイル
        >psymake load   必要なデータをダウンロード
        >run main      実行

＜コントローラの操作方法＞
        ○：SEP track 3 再生，SEP track 2 デクレッシェンド
	△：SEP track 2 再生，SEQ デクレッシェンド
	×：SEQ, SEP track 2 停止
	START：SEQ 再生
        SELECT：終了
