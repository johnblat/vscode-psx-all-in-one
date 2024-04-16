$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$       

module\cdexec : DTL-H2000 ブートモジュール

                       Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.


＜サンプルの解説＞

  DTL-H2000 の CD-ROM/CD-Emulator より PSX.EXE を実行させる
  モジュール（patchxした状態でブートさせる方法）

  現在、PCからH2000にプログラム(.CPEファイル)をロードして実行させる場合、
  前もって patchx.cpe を実行する事が必要です。

	（例）
	DOS> run patchx  
	DOS> run main
  
  これに対して、CD-ROM/CD-Emulator からブートプログラム(PSX.EXE)を読みだして
  実行させるには、patchx.cpe の後にこの cdexec.cpe を実行して下さい。

	DOS> run patchx
	DOS> run cdexec


＜起動方法＞ 

	> psymake
	> run patchx
	> run cdexec


＜備考＞

ブートさせる手段として「resetps 0」がありますが、DTL-H2000では、
resetpsを実行するとパッチが無効になってしまいます。
従って、H2000では「resetps 0」は正常に動作しません。
cdexec はこの問題の回避策として、サンプルとして収められました。

H2500,H2700では「resetps 0」で正常にブートできますので、cdexec を
使う必要はありません。

