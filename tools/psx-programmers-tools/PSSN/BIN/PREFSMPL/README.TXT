
$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.



    ライブラリをオーバレイ側にリンクするツールおよびサンプル


このサンプルでは、\psx\sample\module\overmenu を例にとって、libgsを
メイン側ではなくrcube.binにリンクしています。


作業は大まかに２つに分けられます。

１）libgs.lib を再構築する
    libgs.libはそのままではメイン側にリンクされてしまう為、変更する必要が
    あります。そのためには、libgs.libに含まれる各OBJ内のgroup,section情報を
    書き換えなければなりません。
    このサンプルでは、rcube側へのリンク用の libgs2.lib を作成します。
    prefsect.exe は、objファイル内部のgroup,section情報を書き換えるツール
    です。prefsectの解説は文末に添付します。

      cd libgs2
      psylib /x libgs.lib   libgsをobj単位にばらす。psylib ver.2.04以降使用
      gmake                 libgs2.libを作る。gmake等の一般的なmakeを使用
                            して下さい。
      copy libgs2.lib ..

２）再構築したlibgs.libをリンクする。
    通常のlibgsではなく、再構築したもの(rcube用の)libgs2をリンクします。
    今回のサンプルでは、menu.lnk を一行書き換えただけです。

      cd ..
      psymake


（補足）
このサンプルでは、libgs2が完全にグループrcubeとしてリンクされますので、
他のオーバレイモジュール(balls,anim)内でlibgs関数をコールする事は
意味を持ちません。


------------------------------------------------------------------------------


（prefsect 使用法）

prefsect [options] infile outfile prefix
options : -g create a group of the same name as the prefix and put all
          sections in this group

（例）
prefsect aaa.obj bbb.obj grp1
aaa.obj内の全てのセクションにプレフィクスgrp1を付ける。
例えば .text は grp1.text という様に置き換えられる。

prefsect -g aaa.obj bbb.obj grp1
aaa.obj内の全てのセクションにプレフィクス grp1 を付け、さらに
全てのセクションをまとめてグループ grp1 を生成する。


以上

