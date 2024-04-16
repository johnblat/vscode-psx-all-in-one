$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

libcomb チュートリアル

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.


tuto1: 同期通信サンプル
======

＜サンプルの解説＞
	一定間隔で垂直同期を合わせながら同期通信を行います。

＜起動方法＞
	>psymake	コンパイル
	>run main	実行

＜備考＞
	最大通信速度で通信の行える最も軽い通信方法ですが、相手局との
	同期が必要となります。


tuto2: 非同期受信サンプル
======

＜サンプルの解説＞
	同期write と非同期read を用いて通信を行います

＜起動方法＞
	>psymake	コンパイル
	>run main	実行

＜備考＞
	最もポピュラーな通信方式ですが、通信速度が速くなるにつれて、
	受信割り込みによるオーバーヘッドが大きくなります。


tuto3: 非同期送信サンプル
======

＜サンプルの解説＞
	非同期write と非同期read を用いて通信を行います

＜起動方法＞
	>psymake	コンパイル
	>run main	実行

＜備考＞
	最も重い通信方式です。最大通信速度に制限があります。

