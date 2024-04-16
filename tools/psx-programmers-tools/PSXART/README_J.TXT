$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
                                                                Oct. 1998

                                          PlayStation(R) Programmer Tool
                                             Runtime Library Release 4.4

                            Release 4.3 からのツールの変更, 新規追加情報
                                                          およびバグ情報
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                      Copyright(C) 1998 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.
────────────────────────────────────

SGI 用のファイルは、/psxart/sgi/graphics/sgiconv.taz を展開した後の
ファイル名で記述してあります。


ツールバグ情報および使用上の注意
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
無し。

既存ツールの改良,変更,修正
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
1. mkmime	1.12     MIMe 情報を含む HMD 作成ツール
	x86: /psxart/x86/graphics/mkmime.exe
	SGI: /psxart/sgi/graphics/mkmime

	- ごくまれに、assertion failed で異常終了するバグが修正されました。

2. xhmd		1.20     HMD 逆アセンブラ
	x86: /psxart/x86/graphics/xhmd.exe
	SGI: /psxart/sgi/graphics/xhmd

	- ポリゴンデータの逆アセンブルが可能になりました。
	- 同一アドレスに複数ラベルが割り当てられている場合にも、
	  処理が可能になりました。

3. mc32     3.2      Movie Converter
	x86: /psxart/x86/graphics/mc32.exe

    - 画像の縦・横サイズが16の倍数でなければ変換できなかった不具合を修正
      しました。

    - video+sound のダイアログで、CD-ROM SpeedをNormalにしてコンバートす
      ると異常終了する不具合を修正しました。

4. aiff2vag 3.2      Sound data converter
	x86: /psxart/x86/sound/aiff2vag.exe

    - ループ設定のあるサウンドデータを変換すると、ノイズが入ることがある
      不具合を修正しました。

    - 変換が正常に終了しても、エラーコードを返していた不具合を修正しまし
      た。

5. smf2seq  2.4      Sequence data coverter
	x86: /psxart/x86/sound/smf2seq.exe

    - 変換が正常に終了しても、エラーコードを返していた不具合を修正しまし
      た。

