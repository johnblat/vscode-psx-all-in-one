$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

マウスインターフェースのサンプル

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.

＜サンプルの解説＞
	サンプルでは、マウスの移動にあわせてボールが動くようになっています。
	マウス左ボタンでデフォルトの左位置に、右ボタンで右位置にボールを移動さ
	せます。画面には、現在のマウスの位置をリアルタイムで表示しています。

	サンプルファイル main.c の関数 main() の中に記述されているように、関数
	InitPAD() を呼んだ後に、関数 InitMouse() を呼びます。そのあと、
	SenseMouse() でマウスの感度、SetMouse() でマウスの位置、RengeMouse() で
	マウスの移動範囲が初期化されます。これらの値は、その後に対応する関数を
	呼び出すことで自由に再設定できます。そのあと、VSync 毎に MouseRead() を
	呼び出すことで、マウスの現在位置を獲得できます。また、MouseState() を呼
	び出すことで、VSync 間におけるマウスの移動量を獲得することができます。

＜起動方法＞ 
	>psymake	
	>run test

＜コントローラの操作方法＞
        左ボタン：	デフォルトの左位置にボールを移動する
        右ボタン：	デフォルトの右位置にボールを移動する

