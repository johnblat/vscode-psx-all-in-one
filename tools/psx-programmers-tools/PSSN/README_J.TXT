
$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.



                  Software Development Tool Chain (SDevTC)   98/10/1版



pssn以下のディレクトリ構成が変更になりました。

  1) 従来 \pssn にあったツール群を、\pssn\bin に移動しました。
  2) 従来の \compiler の内容を \pssn\bin に移動しました。
  3) SN Debugger for Windows を \pssn\windbg に追加しました。
  4) GCC 2.8.1 ソース を \pssn\gnusrc に追加しました。

つきましては、新ディレクトリ構成への移行作業を行って頂きますようお願い致しま
す。移行手順については readme.1st を参照下さい。


========================================================================
\pssn には以下のツールが含まれています。

● pssn\windbg
Psx1604.EXE           SN Debugger for Windows Ver.2.4
dset95_j.txt          Windows95用セットアップマニュアル
dsetNT_j.txt          WindowsNT用セットアップマニュアル
dbgman_j.txt          日本語簡易マニュアル

● pssn\gnusrc
gcc281s.zip           GCC 2.8.1 ソース（zip圧縮形式ファイル）

● pssn\bin
各ツールに関する解説は、ドキュメント(\PSX\DOC)の方を参照して下さい。

ASMPSX.EXE     2.45   R3000 アセンブラ（SDevTC シンタックス）(Win32)
ASPSX.EXE      2.79   R3000 アセンブラ（MIPS 互換アセンブラ）(Win32)
BV.EXE                VGAユーテリィティ
CCPSX.EXE      3.06.0015   Ｃコンパイラフロントエンド(Win32)
DBUGPSX.EXE    4.113  R3000 デバッガ
DBG_READ.TXT          デバッガのREADME（英語）
DELAY.COM             ディレイユーテリィティ
DEXBIOS.COM    1.27   H2000 用 SDevTC TSR BIOS
DUMPCPE.EXE    2.01   CPEファイルダンプツール
DUMPOBJ.EXE    2.00   OBJファイルダンプツール
DUMPSYM.EXE    2.02   SYMファイルダンプツール
H25BIOS.COM    1.44   H2500 用 SDevTC TSR BIOS
MAIN.C                メッセージ出力サンプルプログラム（ソース）
MAIN.CPE              メッセージ出力サンプルプログラム（実行ファイル）
MESS1.COM      1.04   メッセージハンドラ
NEWDEX.CPE            カーネルパッチモジュール
PQALOAD.COM           COFF形式の実行ファイルローダー
PQBLOAD.COM           バイナリ形式ファイルローダー
PQGOPSX.COM           実行ユーテリィティ（指定番地から実行）
PREFSECT.EXE   1.01   グループ情報書き換えツール（PREFSMPL\README.TXT参照）
PSYLIB.EXE     2.09   SDevTC ライブラリアン(Win32)
PSYLIB2.EXE    2.05   long file name 対応版。使用法は PSYLIB に準じる。
PSYLINK.EXE    2.74   SDevTC リンカ(Win32)
PSYMAKE.COM    1.14   SDevTC Make ユーティリティ
README.1ST            重要な変更についてのテキストファイル
README_J.TXT          このファイル
RESETPS.EXE    1.08   ターゲットボードブートプログラム
RUN.EXE        2.36   実行ファイルローダー
SN.CB                 Brief マクロ（ソース）
SN.CM                 Brief マクロ（コンパイル済）
SN.INI                定義ファイルのサンプル
SN_SCE.INI            保存用のSN.INI
SNPATCHJ.CPE          H2000用 パッチモジュール（SNSystems版）
SNPATCHW.CPE          H2000用 パッチモジュール（SNSystems版）
SNPATCH.DOC           H2000用 パッチモジュール ドキュメント（英語）
SYMMUNGE.EXE   1.4    シンボルファイル圧縮ユーティリティ(後述)
TESTMESS.COM          メッセージ出力をPCのstdoutからにするユーティリティ
CPPPSX.EXE     GNU2.8.1 SN32 4.0.0010   GNU プリプロセッサ (Win32)
CC1PSX.EXE     GNU2.8.1 SN32 4.0.0010   GNU Cコンパイラ (Win32)
CC1PLPSX.EXE   GNU2.8.1 SN32 4.0.0010   GNU C++コンパイラ (Win32)
COPYING                                 GNU GENERAL PUBLIC LICENSE
DOS\ASMPSXD.EXE    2.45D   R3000 アセンブラ（SDevTC シンタックス）(DOS)
DOS\ASPSXD.EXE     2.72D   R3000 アセンブラ（MIPS 互換アセンブラ）(DOS)
DOS\CCPSXD.EXE     3.05.0009   Ｃコンパイラフロントエンド(DOS)
DOS\PSYLIBD.EXE    2.09D   SDevTC ライブラリアン(DOS)
DOS\PSYLINKD.EXE   2.73D   SDevTC リンカ（DOS）
DOS\CPPPSX.EXE     GNU2.7.2 SN16 3.7.0003   GNU プリプロセッサ (DOS)
DOS\CC1PSX.EXE     GNU2.7.2 SN16 3.7.0003   GNU Cコンパイラ (DOS)
DOS\CC1PLPSX.EXE   GNU2.7.2 SN16 3.7.0003   GNU C++コンパイラ (DOS)
DOS\SNGNUBUG.TXT                            バグ情報
DOS\SNGNUVER.TXT                            バージョンアップ情報
FLASH25\           H2500用ファームウェアキット（FLASH25\README_J.TXT 参照）
FLASH27\           H2700用ファームウェアキット（FLASH27\README_J.TXT 参照）
PREFSMPL\          PREFSECT用サンプル（PREFSMPL\README.TXT 参照）


● DOS版ツールについて

DOS環境でご使用の場合は、PSSN\BIN\DOS に収められているDOS版ツールを使用して
下さい。このとき、CCPSXDによって呼び出されるGNUコンパイラもDOS版を使用する
必要がありますのでご注意下さい。
一般的に、以下の変更によってDOS版ツールが使用可能になります。

１）makefile等に記述されているWin32版ツールのコマンド名を、それぞれ対応する
    DOS版のものに置き換える。
　　（例）
　　旧：ccpsx -O3 -c main.c
　　新：ccpsxd -O3 -c main.c

２）PSSN\BIN\DOS にパスを通す。

３）PSSN\BIN\sn.ini 中で以下のように指定されている事を確認する。
    （例）
     [ccpsxd]
     library_path=c:\psx\lib;c:\pssn\lib
     c_include_path=c:\psx\include;c:\pssn\include
     cplus_include_path=c:\psx\include;c:\pssn\include
     compiler_path=c:\pssn\bin\dos
     assembler_path=c:\pssn\bin\dos
     linker_path=c:\pssn\bin\dos

４）環境変数 PSYQ_PATH を定義する（バグ対策）
    ccpsxd ver.3.05.0009は、PSYQ_PATH が定義されていないとエラーを返します。
    だだし実際に参照するのは SN_PATH で指定されたディレクトリです。
　　（例）
    > set PSYQ_PATH=C:\TEMP


========================================================================
SYMMUNGE シンボルファイル圧縮ユーティリティ

Usage   : symmunge [options] infile [outfile]
Options : /i - ignore differences in structure definitions
          /u - unmangle c++ names

SYMMUNGEは、同一structure/union/enumの多重定義を削除し、ソースレベル
デバッグ情報を再構築する事によって、シンボルファイル(.SYM)のサイズを
小さくします。これによってデバッガの使用するメモリ容量が軽減され、起
動も早くなります。また、/uを指定する事によりC++にも対応します。
シンボルファイル中に、同一名で内容の異なる構造体が見られた場合、
デフォルトではSYMMUNGEはエラーを返しますが、/iを指定すると、該当する
構造体の情報はそのまま残して処理を継続します。

（例）
あるファイル中に

struct xxx
{
   int a;
   int b;
};

という定義があり、別のファイルで

struct xxx
{
   int a;
   short b;
};

というような定義がある場合、SYMMUNGEは以下のような処理をします。

DOS> symmunge main.sym
Error : non-matching definitions for struct 'xxx' in field(s) b - b

DOS> symmunge /i main.sym
Warning : non-matching definitions for struct 'xxx' in field(s) b - b
Symbol file compacted OK
Total allocated 262144 bytes


========================================================================
変更履歴

＜SDevTC ツール 98/10/1版＞
------------------------------------------------------------------------
従来 \pssn にあったツール群は、\pssn\bin に移動した。
従来の \compiler の内容も \pssn\bin に移動した。

変更されたファイル：
pssn\readme.1st               pssnディレクトリ構成の変更について
pssn\readme_j.txt             このファイル
pssn\bin\aspsx.exe            バージョンアップ
pssn\bin\psylink.exe          バージョンアップ
pssn\bin\sn.ini               ディレクトリ変更に伴う修正、libmcxの追加
pssn\bin\dos\aspsx.exe        バージョンアップ
pssn\bin\prefsmpl\readme.txt  ディレクトリsnの削除に伴う修正

追加されたファイル：
pssn\windbg\Psx1604.EXE     SN Debugger for Windows Ver.2.4 パッケージ
pssn\windbg\dset95_j.txt    Windows95用セットアップマニュアル
pssn\windbg\dsetNT_j.txt    WindowsNT用セットアップマニュアル
pssn\windbg\dbgman_j.txt    日本語簡易マニュアル
pssn\gnusrc\gcc281s.zip     GCC 2.8.1 ソース圧縮ファイル
pssn\bin\sn_SCE.ini         sn.ini と同じ内容(保存用)
pssn\bin\COPYING            GNU GENERAL PUBLIC LICENSE  \compilerから移動
pssn\bin\cpppsx.exe         バージョンアップ GNU 2.8.1  \compilerから移動
pssn\bin\cc1psx.exe         バージョンアップ GNU 2.8.1  \compilerから移動
pssn\bin\cc1plpsx.exe       バージョンアップ GNU 2.8.1  \compilerから移動
pssn\bin\dos\cpppsx.exe     バージョンアップ GNU 2.8.1  \compiler\dosから移動
pssn\bin\dos\cc1psx.exe     バージョンアップ GNU 2.8.1  \compiler\dosから移動
pssn\bin\dos\cc1plpsx.exe   バージョンアップ GNU 2.8.1  \compiler\dosから移動
pssn\bin\dos\SNGNUBUG.TXT   バグ情報                    \compiler\dosから移動
pssn\bin\dos\SNGNUVER.TXT   バージョンアップ情報        \compiler\dosから移動

削除されたファイル：
pssn\bin\prefsmpl\sn        古いライブラリに沿った内容なので削除した


＜SDevTC ツール 98/5/5版＞
------------------------------------------------------------------------
変更されたファイル：
pssn\aspsx.exe          バージョンアップ
pssn\ccpsx.exe          ファイル検索方法の変更(readme.1st参照)
pssn\dbugpsx.exe        バージョンアップ
pssn\dbg_read.txt       デバッガのREADME（英語）
pssn\psylink.exe        warningでもコード-1(失敗)を返すバグを修正
pssn\snpatchj.cpe       バージョンアップ
pssn\readme.1st         DOS環境変数の変更について
pssn\readme_j.txt       このファイル
pssn\sn.ini             ccpsxの変更に伴う修正
pssn\flash25            バージョンアップ
pssn\flash27            バージョンアップ
pssn\dos\psylinkd.exe   psylinkと同様の変更
compiler\cpppsx.exe     バージョンアップ GNU 2.8.0
compiler\cc1psx.exe     バージョンアップ GNU 2.8.0
compiler\cc1plpsx.exe   バージョンアップ GNU 2.8.0

追加されたファイル：
pssn\psylib2.exe        psylib.exeの long file name 対応版。
                        使用法はpsylibに準じる。


＜SDevTC ツール 98/1/12版＞
------------------------------------------------------------------------
変更されたファイル：
PSSN\ASMPSX.EXE         バージョンアップ
PSSN\ASPSX.EXE          バージョンアップ
PSSN\PSYLIB.EXE         バージョンアップ
PSSN\PSYLINK.EXE        バージョンアップ
PSSN\SN.INI        	libpad.lib を追加
PSSN\DOS\ASMPSXD.EXE    バージョンアップ
PSSN\DOS\ASPSXD.EXE     バージョンアップ
PSSN\DOS\PSYLIBD.EXE    バージョンアップ
PSSN\DOS\PSYLINKD.EXE   バージョンアップ
COMPILER\CPPPSX.EXE     バージョンアップ
COMPILER\CC1PSX.EXE     バージョンアップ
COMPILER\CC1PLPSX.EXE   バージョンアップ

追加されたファイル：
PSSN\SYMMUNGE.EXE       シンボルファイル圧縮ユーティリティ
                        詳細は末尾参照。


＜SDevTC ツール 97/9/5版＞
------------------------------------------------------------------------
変更されたファイル：
PSSN\ASPSX.EXE         新名称SDevTCに関する変更
PSSN\ASMPSX.EXE        新名称SDevTCに関する変更
PSSN\CCPSX.EXE         -kanjiを指定した場合の不具合を解消
                       新名称SDevTCに関する変更
PSSN\DBUGPSX.EXE       バージョンアップ、新名称SDevTCに関する変更
PSSN\DBG_READ.TXT      デバッガのREADME（英語）  
PSSN\DEXBIOS.COM       H2700でIRQが効かない不具合を解消
PSSN\DUMPSYM.EXE       バージョンアップ
PSSN\H25BIOS.COM       バージョンアップ、新名称SDevTCに関する変更
PSSN\MESS1.COM         新名称SDevTCに関する変更
PSSN\PREFSECT.EXE      バージョンアップ
PSSN\PSYLIB.EXE        バージョンアップ
PSSN\PSYLINK.EXE       バージョンアップ
PSSN\README_J.TXT      このファイル
PSSN\RESETPS.EXE       H2500,H2700でprint出力が出ないバグを解消
PSSN\TESTMESS.COM      新名称SDevTCに関する変更
PSSN\DOS\ASPSXD.EXE    新名称SDevTCに関する変更
PSSN\DOS\ASMPSXD.EXE   新名称SDevTCに関する変更
PSSN\DOS\CCPSXD.EXE    新名称SDevTCに関する変更
PSSN\DOS\PSYLINKD.EXE  新名称SDevTCに関する変更
COMPILER\CPPPSX.EXE    kanji.comの機能を内包した
COMPILER\CC1PSX.EXE    CPPPSXのバージョンアップに伴う変更
COMPILER\CC1PLPSX.EXE  CPPPSXのバージョンアップに伴う変更

追加されたファイル：
PSSN\DUMPCPE.EXE       CPEファイルをダンプする
PSSN\README.1ST        新名称SDevTCについてのテキストファイル
PSSN\SN.CB             旧PSYQ.CB。SDevTCに伴う名称変更
PSSN\SN.CM             旧PSYQ.CM。SDevTCに伴う名称変更
PSSN\SN.INI            PSYQ.INIに代わるファイル。
                       CCPSXはPSYQ.INIより先にこちらを参照する。

削除されたファイル：
PSSN\KANJI.COM         GNUプリプロセッサ(CPPPSX.EXE)に吸収された
PSSN\PSYQ.CB           SDevTCに伴う名称変更、SN.CBに。
PSSN\PSYQ.CM           SDevTCに伴う名称変更、SN.CMに。
PSSN\PSYQ.INI          SDevTCに伴う名称変更、SN.INIに。

その他：
PSSN\PREFSMPL\         各ソースファイル及びlibgsを最近のものに合わせた
PSSN\FLASH25\          最新の環境にアップデートした
PSSN\FLASH27\          新規追加。H2700用モジュール類
COMPILER\DOS\          32bit版 GNUコンパイラと同様の変更が行われた。


＜Psy-Q ツール 97/4/25版＞
------------------------------------------------------------------------
変更されたファイル：
PSYQ\README_J.TXT
PSYQ\ASMPSX.EXE         32bit版に変更
PSYQ\ASPSX.EXE          32bit版に変更
PSYQ\CCPSX.EXE          32bit版に変更
PSYQ\PSYLIB.EXE         32bit版に変更
PSYQ\PSYLINK.EXE        32bit版に変更
PSYQ\PSYMAKE.EXE
PSYQ\DBUGPSX.EXE
PSYQ\DBG_READ.TXT
PSYQ\PSYQ.INI
COMPILER\CPPPSX.EXE     GNU プリプロセッサ 32bit版
COMPILER\CC1PSX.EXE     GNU Cコンパイラ 32bit版
COMPILER\CC1PLPSX.EXE   GNU C++コンパイラ 32bit版

追加されたファイル：
PSYQ\DOS\ASMPSXD.EXE
PSYQ\DOS\ASPSXD.EXE
PSYQ\DOS\CCPSXD.EXE
PSYQ\DOS\PSYLIBD.EXE
PSYQ\DOS\PSYLINKD.EXE
COMPILER\DOS\CPPPSX.EXE   GNU プリプロセッサ 16bit版
COMPILER\DOS\CC1PSX.EXE   GNU Cコンパイラ 16bit版
COMPILER\DOS\CC1PLPSX.EXE GNU C++コンパイラ 16bit版


以上
