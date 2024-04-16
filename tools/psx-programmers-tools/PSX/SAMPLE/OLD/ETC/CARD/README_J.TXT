$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

メモリーカードサンプルプログラム

【概要】

　このサンプルプログラムは、プレイステーションに内蔵されますメモリーカー
ド管理ツールを抜き出したものです。(北米向け仕様の画面デザインです。)
メモリーカードアクセス処理の作成の参考にして頂くために、ソースファイル
の一部をサンプルプログラムとして公開しましたので、ご活用ください。

  画面イメージとメモリーカードの操作方法は、プレイステーションと同じに
なっています。

　メニューシステムなど、メモリーカード操作部以外につきましてはソースを
公開できませんので、ライブラリーとして添付しました。ライブラリーの詳細に
つきましては、添付しましたドキュメントをご参照ください。

　さらに、メモリーカード管理画面で使用されるアイコンを生成するツールも
同梱されていますので、併せてご活用ください。


【配布物】

	配布されたファイルの構成は以下の通りです。
	（＊印のファイルが重要です。）

	readme_j.txt		このドキュメントです。

	doc\
		superx_j.txt	super-x library ドキュメント
		turtle_j.txt	turtle graphics library ドキュメント
		malloc_j.txt	malloc 関連ドキュメント
	lib\
		superx.lib	super-x library
		turtle.lib	turtle graphics library
	max\
		cardio.h	メモリーカード制御
	　　＊	card.c			〃
		makefile.mak	psymake 用 makefile
		main.lnk	(for psylink)
		menu.lnk	(for PCMENU)
		makefile.6x	NEWS 用 makefile
		psxload.h	super-x library 用 header
		spritex.h		〃
		menux.h			〃
		turtle.h	turtle library 用 header
		memeng.h	イメージデータファイル
		memjpn.h		〃
		pad.h			〃
		mouse.h			〃
		pencila.h		〃
		libps.h		psx library header
		const.h		メニューシステム用 header
		maxtypes.h		〃
		maxstat.h		〃
		maxproto.h		〃
		menuset.h		〃
		menuset2.h		〃
		vsync.h			〃
		emouse.h			〃
		maxconf.h	英語／日本語切り替え用 header

		max.c		メインルーチン
		vsync.c		基本描画ルーチン
		pad.c		コントロールパッド制御
		emouse.c	コントロールパッド/マウス制御
		maxinit.c	メニューシステム初期設定
		drawmax.c	メニューイメージ描画ルーチン
		menuset.c	メニューシステムハンドラー
		
	makecard\
		makefile.mak	psymake 用 makefile
		makefile.lnk		〃
		makefile.6x	NEWS 用 makefile
		hand1.h		じゃんけん　ぐー
		hand2.h		じゃんけん　ちょき
		hand3.h		じゃんけん　ぱー
	　　＊	makecard.c	アイコン生成プログラム


【使用方法】

　１．メモリーカード管理画面の実行

　max\ に移動してサンプルプログラムをコンパイルしてください。
コンパイルの環境設定はデフォルトを使用していますが、変更が必要な場合は
makefile.mak および main.lnk を書き換えてください。

	> cd max
	> psymake

　コンパイルが終了しましたら、プログラムを起動してください。

	> run main

　数秒のブラックアウトの後、メモリーカード管理画面が現れます。メモリー
カードが装着されてない場合でも、管理画面は現れます。プログラムの起動後は、
メモリーカードを随時着脱可能ですが、アイコンの書き換えに多少時間がかかり
ますので、ご了承ください。

　メモリーカード管理画面のキーアサインは、次の通りです。

	左十字キー	メニュー、アイコンの移動
	右十字キー	決定ボタン（４つとも）
	START キー	取り消しボタン
	SELECT キー	サンプル終了
	
	奥４つ		DELETE の取り消し（同時）

　操作方法は、始めにメニューから操作を選択します。左十字キーでカーソルを
上下に動かし、COPY/COPY ALL/DELETE を右十字キーの何れかを押して選択します。
　操作が選択されると、Card1 のアイコンの上にカーソルが移動します。左十字キー
の上下キーで、カーソルを移動させアイコンを選択します。Card2 側にカーソルを
移動させたい場合は、左右キーの右キーを押します。操作の選択に戻りたい場合は
START キーを押します。
　アイコンが選択されると操作確認メニューが現れますので、YES/NO を選択して
操作を実行します。実行中は、アイコンが操作毎に異なるアニメーションを演じます
のでお楽しみください。
　DELETE 実行の場合のみ、奥４つキーを同時に押すことで、処理が取り消され
アイコンが復活します。

　マウスのキーアサインは、次の通りです。

	左ボタン	決定ボタン
	右ボタン	取消しボタン

	左右同時	DELETE の取消し	

　マウスの場合も、操作方法は同じです。初めに操作を決定してから、アイコンを
選択してください。

　コントローラおよびマウスは、１、２どちらに装着しても使用できますが、両側に
装着されている場合には、１側だけが反応します。


　２．アイコン生成のサンプルプログラムの実行

　makecard に移動して同様にコンパイルを行ってください。

	> cd makecard
	> psymake
	> run makecard

　makecard 実行中は何も表示されませんが、run コマンドで終了を確認できます。

	Cannot connect to target

と表示される場合は、まだ makecard を実行中ですのでしばらくお待ちください。

	No file to process

と表示される場合は終了しています。


【メモリーカードアクセス関数について】

　このサンプルプログラムでは、メモリーカードアクセスに関する処理はすべて
max\card.c に記述されています。メモリーカードアクセスに関するドキュメントは
準備しませんでしたが、処理の内容につきましては card.c のコメントを参考にして
ください。
