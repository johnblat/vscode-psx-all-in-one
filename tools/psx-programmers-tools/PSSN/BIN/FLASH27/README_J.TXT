$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
		DTL-H2700 SDevTC Firmware Kit


ファームウェア書き換え

  DTL-H2700 のフラッシュメモリに、ROMイメージを書き込みます。

    書式： flash <I/Oアドレス>

    <I/Oアドレス> は、DTL-H2700で設定してあるI/Oアドレスを指定します。

    例
        DOS> cd \pssn\bin\flash27
        DOS> flash 1340

    注意

    *正常に動作せず、DOSプロンプトに戻ってこない場合は F10 F2 を押し
    て下さい。
    *実行時には、必ず DEXBIOS は Remove した状態にして下さい。


PAL/NTSCの画面モードの切り替え

  電源投入時には、DTL-H2700は画面モードがNTSCモードとなってます。
  PALに切り替えるには reset27.exe を使用してください。
  その後は再設定を行う必要はありません。
  
    書式： reset27 [-p|-n] [-a<I/Oアドレス>]

    -n 画面モードをNTSCモードに設定します。
    -p 画面モードをPALモードに設定します。

    *指定を省略すると、以前の mode が継続されます。

    -a<I/Oアドレス>は、DTL-H2700で設定してあるI/Oアドレスを指定します。

    *I/Oアドレスを指定しないときは、ＰＣの環境変数 DEX2000 
    に設定してあるアドレスを利用します。

    例
        DOS> reset27 -p -a0x1340

    例
        DOS> set DEX2000=0x1340
        DOS> reset27 -p


h2700.img(SN) ver1.20
-----------------------------------------------------------------------

Copyright (C) 1996-1998 by Sony Computer Entertainment Inc.
		All Rights Reserved.
