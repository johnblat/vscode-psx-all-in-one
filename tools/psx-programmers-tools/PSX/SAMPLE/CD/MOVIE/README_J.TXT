$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

動画再生サンプルプログラム

このプログラムはSTRフォーマットのMDECムービーデータを再生するための処
理ルーチンのサンプルです。


tuto0
======

<サンプルの解説>

ムービー再生処理ルーチンの最小構成例です。
次のような制約があります。

	・再生するムービーの画像サイズは縦、横共に16の倍数でな
	  ければならない。
	・ムービーの途中で画像サイズは変更してはいけない。


	このプログラムは、STRフォーマットのアニメーションデータを
	再生することができます。再生のモードは、１ピクセル１６ビット
	の３万２０００色と、１ピクセル２４ビットの１６７０万色の
	２つのモードを選択できます。
	（プログラム中で RGB24をデファインすると24ビットモード、
	通常は １６ビットモード）

	アニメーションの再生フレーム数を変えるには、プログラム中の
	END_FRAMEを定義し直します。

！！重要！！
	ただし 最後のフレームが欠落する可能性もあるので END_FRAMEに
	設定する値は２、３フレーム前の値を入れておいて下さい。

	最終フレームまで確実に出したい場合は、最終フレームを ２、３
	フレーム余分に入れておき最終フレームの最初をEND_FRAMEに
	入れておけば良いでしょう。


<起動方法>

	>psymake tuto0 		プログラムのコンパイル
	>run tuto0

<コントローラの操作方法>

        SELECT : 終了


tuto1
=====

<サンプルの解説＞

tuto0に比べ、次の点が変更されています。

	・再生に必要なパラメータを構造体にして関数に渡せる様に
	  なった。
	・縦横16ドットの倍数以外でも再生可能。		
	  ただし24bitモードで再生する場合は横幅は偶数pixelでな
	  ければなりません。

<起動方法>

	>psymake tuto1 		プログラムのコンパイル
	>run tuto1

<コントローラの操作方法>

        SELECT : 終了


tuto2
========

<サンプルの解説＞

       tuto0にオンメモリストリーミングの機能が追加されています。
       ○ボタンを押すと StSetStream()のend_frameを０に設定するし
	これにより リングバッファにデータが完全に貯められた状態でそれ以上
	リングバッファのデータを更新しなくなります。この状態でStGetNextS()
	により貯められたデータを再生します。（オンメモリ再生）

       ×ボタンを押すと通常の再生に戻ります。

<起動方法>

	>psymake tuto2 		プログラムのコンパイル
	>run tuto2

<コントローラの操作方法>

        ○ : オンメモリ再生開始
	× : 通常の再生に戻る
        SELECT : 終了


tuto3
=======

<サンプルの解説＞

	tuto0にRingBuffer溢れによるフレーム落ちを抑制する機能が追加
	されています。

	基本原理はデコードが間に合わなくなるとCDのヘッドをを少し前に
	巻戻すことで平均転送レートを下げリングバッファ溢れを防ぎます。

	CDのアクセスを伴うためXAオーディオつきのデータではノイズが発生
	してしまうためこの手法は使えません。
	また このサンプルはCdModeStream2モードでのみ動作します。

	応用範囲としてはコマ落ちの許されないムービーおよびデータで
	XAオーディオが含まれてないものの再生に適します。
	

	プログラムの中では StRingStatus()関数とStGetBackloc()関数を
	使用します。(Ver 3.5 以上でサポート）
	StRingStatus()でリングバッファのフリーセクタ数が
	ある値以下になったらStGetBackloc()で巻き戻しポイントを得て
	アクセスを発行します。この部分が以下のコードになります。

        StRingStatus(&free,&over); /* added ring buffer status */
        if(free<MINIMUM_FREE_SECTORS) {
	    next_frame = StGetBackloc(&backloc); /* get the latest frame
						    loc and id on ring buf*/
	    StSetMask(1,next_frame,0xffffffff); /* masking from cdrom data */
            /* access backward to reduce trasfer rate */
	    strKickCD(&backloc);
	}

	上記コードを１フレームのループに１回呼んでいます。

	CDアクセス中であっても動画を止めないようにRING_SIZEは多めにとる
	必要があります。RING_SIZE - MINIMUM_SECTORSに7,8フレーム入る
	ようにすればアクセス中も動画は止まらないでしょう。

	MINIMUM_SECTORSは １フレームのセクタ数よりは大きくする必要が
	あります。そうしないとアクセス前にコマが飛んでしまいます。

<起動方法>

	>psymake tuto3 		プログラムのコンパイル
	>run tuto3

<コントローラの操作方法>

        SELECT : 終了

