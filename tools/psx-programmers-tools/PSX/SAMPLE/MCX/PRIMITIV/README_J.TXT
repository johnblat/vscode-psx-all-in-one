$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

PDA(libmcx)を使うサンプル

                      Copyright(C) 1998 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

<サンプルの解説>

	  このサンプルはPDAライブラリを使うサンプルで、PDAライブラリにある
	処理登録関数の機能を確かめることが出来る。

	○ メイン画面の説明

	 1: ALL INFO		McxAllInfo() のテスト
	 2: CURRENT CONTROL	McxCurrCtrl() のテスト
	 3: FLUSH ACCESS	McxFlushAcs() のテスト
	 4: SET EXEC FLAG	McxExecFlag() のテスト
	 5: EXEC APPLI		McxExecApr() のテスト
	 6: GET APPLI		McxGetApr() のテスト
	 7: SET TIME		McxSetTime() のテスト
	 8: GET TIME		McxGetTime() のテスト
	 9: SET LED		McxSetLED() のテスト
	10: GET SERIAL NO	McxGetSerial() のテスト
	11: WRITE DEVICE	McxWriteDev() のテスト
	12: READ DEVICE		McxReadDev() のテスト
	13: SHOW TRANS		McxShowTrans() のテスト
	14: HIDE TRANS		McxHideTrans() のテスト
	15: SET MEMORY		McxSetMem() のテスト
	16: GET MEMORY		McxGetMem() のテスト
	17: SET USER IFS	McxGetUIFS() のテスト
	18: GET USER IFS	McxGetUIFS() のテスト

	L1: 次の項目を選択
	L2: 前の項目を選択
	START: 通信開始
	SELECT: このプログラムを終了


	○ 個別画面の説明

	・共通メッセージ

	NO PARAMETER NEEDED : パラメータ設定が不要なテスト項目

	PRESS START TO EXEC COMMAND : 通信結果がないのでテスト結果を
		表示できない状態を示す。スタートボタンで通信開始


	・通信結果のメッセージ(最下行)

	COMMAND SUCCEEDED : 通信成功
	COMMAND SUCCEEDED (CARD CHANGED) : 通信成功（ただしメモリカードの
		未確認フラグがクリアされていないか またはメモリカードが
		交換されたことを示す）
	CARD NOT CONNECTED : メモリカードまたはPDAがささっていない
	COMMAND FAILED : 通信の途中で失敗した（PDAでなくメモリカードが
		ささっているか、PDAが不良）

	
	・1: ALL INFO	PDA情報の一斉取得

		CURRENT APPLICATION : xx
			5:EXEC APPLI によって設定される(実行中のPDAアプリ番号)
		PDA APPLI FLUSH ACCESS : xxxxxx
			3:FLUSH ACCESS によって設定
			（PDAアプリケーションのフラッシュ優先書き込み設定）
		CURRENT CONTROL    SOUND: xx   INFRED: xx  LED: xx
			2:CURRENT CONTROL によって設定(電源容量制限の設定)
		SERIAL NO. : xxxxxxxx
			PDAのシリアル番号
		DATA: 199x/xx/xx xxx xx:xx:xx
			現在の年月日曜時分秒が表示される


	・2: CURRENT CONTROL	高電流容量の機能制限

		R2: スピーカーON
		R1: スピーカーOFF
		△: 赤外線送信許可
		□: 赤外線送信禁止
		○: LED点灯許可
		×: LED点灯禁止


	･ 3: FLUSH ACCESS	PDAアプリケーションのフラッシュ優先書き込み

		○: フラッシュ優先（SUPERIOR)
		×: PSとの通信優先（フラッシュ書込み禁止、INFERIOR)


	・4: SET EXEC FLAG	メモリカードのファイルにPDAアプリ/データを
				識別するためのフラグを(PDAアプリ側に)セット
				クリア(データにセット)する

		△: アプリ(ブロック)番号増加
		□: アプリ(ブロック)番号減少
		○: フラグセット（PDAアプリケーション）
		×: フラグクリア（メモリカードデータ）


	・5: EXEC APPLI		実行するPDAアプリケーションを選択

		矢印キー：値を変化させる場所選択
		△: +0x10
		□: -0x10
		○: +1
		×: -1

		起動するアプリ番号(保存されているブロック番号)と
		渡す引数をセットしてスタートボタンを押す


	・6: GET APPLI	実行中のPDAアプリケーション番号取得

		CURRENT APPLICATION : xx
			5:EXEC APPLI などで起動されたPDAアプリ番号


	・7: SET TIME	現在時刻設定

		矢印キー：値を変化させる場所選択
		△: +0x10
		□: -0x10
		○: +1
		×: -1


	・8: GET TIME	現在時刻情報取得

		DATA: 199x/xx/xx xxx xx:xx:xx


	・9: SET LED		LEDの点灯/消灯

		○: 点灯
		×: 消灯


	・10: GET SERIAL	PDAのシリアル番号取得

		SERIAL NO. : xxxxxxxx


	・11: WRITE DEVICE	PDAデバイスドライバへの書き込み

		矢印キー：値を変化させる場所選択
		△: +0x10
		□: -0x10
		○: +1
		×: -1

		DEVICE NO : xx のところに呼び出したいデバイス番号を入れ、
		（予約済みデバイスは 0:時計デバイス、1:PADメモリ読み書き
		デバイス、2:ユーザーインターフェースステータス読み書き、
		ユーザー定義デバイスは 80以降。16進値で入れる）
		FIX:に(固定部)パラメータ、VAR:に(可変部)データをセットし
		てスタートボタンを押すと、可変部のデータがデバイスに書き
		込まれる。

		パラメータ長はデバイスによって異なる。パラメータ内容によ
		ってデータ長が変化する。


	・12: READ DEVICE	PDAデバイスドライバからの読み出し

		矢印キー：値を変化させる場所選択
		△: +0x10
		□: -0x10
		○: +1
		×: -1

		DEVICE NO : xx のところに呼び出したいデバイス番号を入れ、
		（予約済みデバイスは 0:時計デバイス、1:PADメモリ読み書き
		デバイス、2:ユーザーインターフェースステータス読み書き、
		ユーザー定義デバイスは 80以降。16進値で入れる）
		FIX:に(固定部)パラメータ、セットしてスタートボタンを押す
		と読み込み結果が表示される。

		パラメータ長はデバイスによって異なる。


	・13: SHOW TRANS	「PS - PDA 間でのデータ転送中」表示

		△: 転送方向を PS→PDA にセット
		□: 転送方向を PDA→PS にセット
		○: タイムアウト時間番号増加
		×: タイムアウト時間番号減少


	・14: HIDE TRANS	「PS - PDA 間でのデータ転送中」表示解除

		表示なし


	・15: SET MEMORY	PDAメモリへの書き込み

		矢印キー：値を変化させる場所選択
		△: +0x10
		□: -0x10
		○: +1
		×: -1

		書き込み開始アドレスと、書き込みサイズ、データ内容を
		セットしてスタートボタンを押す。
		データ内容の設定領域の大きさは LEN の設定により変化する。

		書き込み領域の中に 0x0xxxxxx,0x6xxxxxx,0xA000000～0xDFFFFFF
		以外のアドレスが含まれる設定をすると書き込みは実行されない。


	・16: GET MEMORY	PDAメモリからの読み出し

		矢印キー：値を変化させる場所選択
		△: +0x10
		□: -0x10
		○: +1
		×: -1

		読み込み開始アドレスと、読み込みサイズをセットしてスタート
		ボタンを押す。

		読み込み領域の中に 0x0xxxxxx,0x2xxxxxx,0x4xxxxxx,0x6xxxxxx,
		0xA000000～0xDFFFFFF 以外のアドレスが含まれる設定をすると
		読み込みは実行されない。

		仮想フラッシュメモリを設定しないで 0x2xxxxxx を読もうとする
		とPDAでバスエラーを起こし、停止する。


	・17: SET USER IFS	PDAの User I/F status 書き込み

		矢印キー：値を変化させる場所選択

		ALARM TIME 選択中のとき
		△: +0x10
		□: -0x10
		○: +1
		×: -1

		それ以外選択中の時
		R2: アラーム音 ON
		R1: アラーム音 OFF
		△: キーロック設定
		□: キーロック解除
		○: スピーカー OFF → 音量 小 → 音量 大
		×: 音量 大 → 音量 小 → スピーカー OFF

	・18: GET USER IFS	PDAの User I/F status の読み出し

		ALARM TIME:  xx:xx	アラーム設定時刻
		ALARM:		xx	アラーム ON/OFF状態
		KEY LOCK:	xx	キーロック状態
		SPEAKER:	xx	スピーカー音量
		AREA CODE:	xx	エリアコード
		RTC SET:        xx	リアルタイムクロック設定済みフラグ
		FONT ADDRESS:	xx	フォント格納アドレス



<起動方法> 
	> psymake	:コンパイル
	> run main	:実行
