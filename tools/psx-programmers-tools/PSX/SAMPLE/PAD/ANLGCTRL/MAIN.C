/* $PSLibId: Run-time Library Release 4.4$ */
/* 
 *			  extap.c
 *
 *	拡張コントローラ用アクチュエータ（振動子）制御プログラム
 *
 *	Copyright (C) 1997 by Sony Computer Entertainment Inc.
 *			All rights Reserved
 *
 *	 Version	Date		Design
 *	-----------------------------------------
 *	1.00        August 1,1997	Honda
 *	1.01        October 1,1997	Honda	added gun test
 *	1.10        January 13,1998	Honda	tab fix
 *
 *
 * --------------------------- キー操作の説明 -------------------------------
 *	L1:アクチュエータ０の値－１
 *	L2:アクチュエータ０の値＋１
 *	R1:アクチュエータ１の値－１０
 *	R2:アクチュエータ１の値＋１０
 *	□ :反対側のポートの通信を停止／再開する
 *	○:端末種別切替スイッチ ロック／アンロック
 *	←:端末種別を「標準コントローラ」に切り替える
 *	→:端末種別を「アナログコントローラ」に切り替える
 *
 * --------------------------- 画面表示の説明 -------------------------------
 *  [MULTI TAP]		直接接続の時:[DIRECT]、マルチタップ経由の時:[MULTI TAP]
 *  1A [EX]=7		現在の端末種別
 *  G=0300,156		ガンのターゲット座標（ガン:ID=3 接続時のみ）
 *  B=FFFF		ボタンの押下状態（各ビット １：リリース、０：プッシュ）
 *  A=80,80,80,80	アナログレベル（４チャンネル）
 *  ID(4,7)		サポートしている端末種別のID
 *  AC(1,2,0,10)=0	アクチュエータ0は連続回転型振動、高速回転、
 *			データ長1ビット、消費電流100mA、現在の値0
 *    (1,1,1,20)=90	アクチュエータ1は連続回転型振動、低速回転
 *			データ長1バイト、消費電流200mA、現在の値90
 *  CM(0,1)		同時に制御できるアクチュエータは 0番と1番である。
 *  SW(UNLOCK)		コントローラの切り替えスイッチは使用可能状態
 */

#include <r3000.h>
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include "libpad.h"
#include <kernel.h>


typedef struct
{
	DRAWENV		draw;		/* 描画環境 */
	DISPENV		disp;		/* 表示環境 */
} DB;

#define MultiTap	1
#define CtrlTypeMtap	8
#define PortPerMtap	4

#define BtnM0		1
#define BtnM1		2
#define BtnMode		4
#define BtnStart	8
#define BtnEnable	0x10


typedef struct
{
	/* 前回のボタン押下情報 */
	unsigned char Button;

	/* コントローラの切り替えスイッチロック状態 */
	unsigned char Lock;

	/* アクチュエータ(振動子)にセットする値*/
	unsigned char Motor0,Motor1;

	/* アクチュエータ(振動子)制御データ送信位置設定関数コール済みフラグ */
	unsigned char Send;

	/* 割り込み発生式ガン使用時の銃口の座標 */
	int X,Y;
} HISTORY;


static HISTORY history[2][4];
static unsigned char padbuff[2][34];
int padEnable = 3;

typedef struct
{
	short Vert;
	short Horz;
} POINT;

typedef struct
{
	unsigned char Port;
	unsigned char Size;
	POINT Pos[10];
} GUNDATA;

GUNDATA gunPos;


void dispPad(int, unsigned char *);
int setPad(int, unsigned char *);
void sprintf();
void *bzero(unsigned char *, int);
int PadChkMtap(int);


/*#main--- main routine */
int main()
{
	DB db[2],*cdb=0;

	bzero((unsigned char *)history, sizeof(history));
	bzero((unsigned char *)padbuff, sizeof(padbuff));

	ResetCallback();

#if MultiTap
	PadInitMtap(padbuff[0],padbuff[1]);
#else
	PadInitDirect(padbuff[0],padbuff[1]);
#endif
	PadInitGun((unsigned char *)&gunPos, 10);
	PadStartCom();

	ResetGraph(0);		
	SetGraphDebug(0);	

	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(4, 20, 312, 240, 0, 1024));	

	db[0].draw.isbg = 1;
	setRGB0(&db[0].draw, 60, 120, 120);
	db[1].draw.isbg = 1;
	setRGB0(&db[1].draw, 60, 120, 120);
	SetDispMask(1);

	VSync(0);
	while (TRUE)
	{
		cdb  = (cdb==db)? db+1: db;	

		if (setPad(0,padbuff[0]) == 1){
			break;
		};
		setPad(0x10,padbuff[1]);

		/* 
		   ポート1のコントローラ情報表示 */
		dispPad(0,padbuff[0]);

		/* 
		   ポート2のコントローラ情報表示 */
		dispPad(0x10,padbuff[1]);

		VSync(0);		/* wait for V-BLNK (1/60) */

		/* ダブルバッファの切替え */
		PutDispEnv(&cdb->disp);
		PutDrawEnv(&cdb->draw); 
		FntFlush(-1);
	}
	PadRemoveGun();
        PadStopCom();
        ResetGraph(3);
        StopCallback();
        return 0;
}


/* ボタンの押下状態により動作設定 */
int setPad(int port, unsigned char *rxbuf)
{
	HISTORY *hist;
	int button,count;

	if(rxbuf[1]>>4 == CtrlTypeMtap)
	{
		for(count=0;count<PortPerMtap;++count)
		{
			if (setPad(port+count, rxbuf + 2 +count*8) == 1){
				VSync(2);
				return 1;
			}
		}
		return 0;
	}

	/* 通信に失敗しているときはボタン情報を読みに行かない */
	if(*rxbuf)
	{
		return 0;
	}

	button = ~((rxbuf[2]<<8) | rxbuf[3]);
	hist = &history[port>>4][port & 3];
	if (button & PADselect ){
		if (PadInfoMode(port,2,0) != 0){
                	PadSetMainMode(port,0,2);
                	VSync(2);
                	while (PadGetState(port) != 6){
                	}
		}
                return 1;
        }

#if 0
	/* 操作しているコントローラの反対側のポートを通信中断／再開させる */
	if(!(hist->Button & BtnEnable) && button & PADRleft)
	{
		padEnable ^= (1 << (!(port>>4)));
		PadEnableCom(padEnable);
	}
#endif

	/* PadStopCom(), PadStartCom() の連続呼びでコントローラ情報を
	   リロードしに行くことの確認 */
	if(!(hist->Button & BtnStart) && button & PADstart)
	{
		PadStopCom();
		PadStartCom();
	}

	if(PadInfoMode(port,InfoModeCurExID,0))
	{
		/* 拡張コントローラのアクチュエータ(振動子)0の値設定 */
		if(!(hist->Button & BtnM0))
		{
			if(button & PADL1 && hist->Motor0)
			{
				hist->Motor0 -= 1;
			}
			else if(button & PADL2 && hist->Motor0 < 255)
			{
				hist->Motor0 += 1;
			}
		}

		/* 拡張コントローラのアクチュエータ(振動子)1の値設定 */
		if(!(hist->Button & BtnM1))
		{
			if(button & PADR1 && hist->Motor1)
			{
				hist->Motor1 -= 10;
			}
			else if(button & PADR2 && hist->Motor1 < 246)
			{
				hist->Motor1 += 10;
			}
		}
		/* 端末種別、切り替えスイッチロック状態 変更 */
		if(!(hist->Button & BtnMode))
		{
			if(button & PADLleft)
			{
				PadSetMainMode(port,0,hist->Lock);
			}
			else if(button & PADLright)
			{
				PadSetMainMode(port,1,hist->Lock);
			}
			else if(button & PADRright)
			{
				switch(hist->Lock)
				{
					case 0:
					case 2:
						hist->Lock = 3;
						break;
					case 3:
						hist->Lock = 2;
						break;
				}
				PadSetMainMode(port,
					PadInfoMode(port,InfoModeCurExOffs,0),
					hist->Lock);
			}
		}
	}
	else
	{
		/* SCPH-1150のアクチュエータ(振動子)の値設定 */
		if(!(hist->Button & BtnM0))
		{
			if(button & PADL1 && hist->Motor0)
			{
				hist->Motor0 = 0x40;
				hist->Motor1 -= 1;
			}
			else if(button & PADL2 && hist->Motor0 < 255)
			{
				hist->Motor0 = 0x40;
				hist->Motor1 += 1;
			}
		}
	}

	/* 今回のボタン情報を保存*/
	if(button & (PADLright|PADLleft|PADRright))
		hist->Button |= BtnMode; else hist->Button &= ~BtnMode;
	if(button & (PADL1 | PADL2))
		hist->Button |= BtnM0; else hist->Button &= ~BtnM0;
	if(button & (PADR1 | PADR2))
		hist->Button |= BtnM1; else hist->Button &= ~BtnM1;
	if(button & PADRleft)
		hist->Button |= BtnEnable; else hist->Button &= ~BtnEnable;
	if(button & PADstart)
		hist->Button |= BtnStart; else hist->Button &= ~BtnStart;
	return 0;
}


/* コントローラの現状を表示 */
void dispPad(int port, unsigned char *rxbuf)
{
	unsigned char buff[20];
	int count,pos,initlevel,maxnum;
	HISTORY *hist;

	/* 送信パケット内の振動子制御データ位置指定
	 （本来はアクチュエータ情報の取得が完了してから内容を決定すべきで
	   あるが簡単のため固定データとしている） */
	static unsigned char align[6]={0,1,0xFF,0xFF,0xFF,0xFF};

	if(rxbuf[1]>>4 == CtrlTypeMtap)
	{
		for(count=0;count<PortPerMtap;++count)
		{
			dispPad(port+count, rxbuf + 2 +count*8);
		}
		return;
	}

	hist = &history[port>>4][port & 3];
	initlevel = PadGetState(port);

	/* ------------------------------------------------------------ */
	/* マルチタップの接続状態、表示                  */
	/* ------------------------------------------------------------ */

	if(!(port & 0xF))
	{
		if(PadChkMtap(port))
		{
			FntPrint("[~c686multi tap~c888]");
		}
		else
		{
			FntPrint("[~c686direct~c888]");
		}
		if( ( !(port & 0x10) && !(padEnable & 1) ) ||
		    (  (port & 0x10) && !(padEnable & 2) ) )
		{
			FntPrint("~c866 suspend~c888");
		}
	}

	FntPrint("\n");

	/* ------------------------------------------------------------ */
	/* ポート番号表示            */
	/* ------------------------------------------------------------ */

	if(PadChkMtap(port))
	{
		FntPrint("%d%c",(port>>4)+1,'A'+ (port & 0xF));
	}
	else
	{
		FntPrint("%d",(port>>4)+1);
	}

	/* ------------------------------------------------------------ */
	/* コントローラの接続状態表示 */
	/* ------------------------------------------------------------ */

	/* コントローラが接続されていないとき */
	if(initlevel==PadStateDiscon)
	{
		FntPrint("[~c866none~c888]\n\n\n");
		return;
	}

	if(PadInfoMode(port,InfoModeCurExID,0))
	{
		/* 拡張コントローラが接続されているとき */
		FntPrint("[~c668ex~c888]=~c686%x~c888 ",
			PadInfoMode(port,InfoModeCurExID,0));
	}
	else
	{
		/* 拡張コントローラ以外のコントローラが接続されているとき */
		FntPrint("[~c668ctrl~c888]=~c686%x~c888 ",
			PadInfoMode(port,InfoModeCurID,0));
	}

	/* ------------------------------------------------------------ */
	/* アクチュエータに設定する値を入れるバッファの登録および、アク
	   チュエータパラメータ設定位置情報(拡張コントローラのみ)の送信 */
	/* ------------------------------------------------------------ */

	/* コントローラが差し替えられたり、端末種別が切り替えられ
	   たりしてライブラリがコントローラ情報をアクセスしに行ったとき
	   アクチュエータの設定位置情報送信済みフラグをクリアする */
	if(initlevel == PadStateFindPad)
	{
		hist->Send = 0;
	}

	/* アクチュエータ情報取り込みが完了したらパケット内での
	   アクチュエータの制御データを設定する位置を指定する */
	if(!hist->Send)
	{
		PadSetAct(port,&hist->Motor0,2);

		/* 拡張コントローラ以外のとき */
		if(initlevel == PadStateFindCTP1)
		{
			hist->Send = 1;
		}
		/* 
		   拡張コントローラのとき
		 （アクチュエータ情報の取り込みが完了するまでは呼ばない） */
		else if(initlevel == PadStateStable)
		{
			/* 受け付けられたらフラグを立てる */
			if(PadSetActAlign(port,align))
			{
				hist->Send = 1;
			}
		}
	}

	/* ------------------------------------------------------------ */
	/* ガン接続時に銃口の座標を表示			*/
	/* ------------------------------------------------------------ */

	if(PadInfoMode(port, InfoModeCurID, 0) == 3)
	{
		if(gunPos.Port == port && gunPos.Size)
		{
			hist->X = hist->Y = 0;
			for(count=0; count<gunPos.Size; ++count)
			{
				hist->X += gunPos.Pos[count].Horz;
				hist->Y += gunPos.Pos[count].Vert;
			}
			hist->X /= gunPos.Size;
			hist->Y /= gunPos.Size;
		}
		sprintf(buff,"%04d,%03d",hist->X,hist->Y);
		FntPrint("g=~c686%s~c888 ",buff);
	}

	/* ------------------------------------------------------------ */
	/* ボタン押下情報、アナログチャネルのレベル表示         */
	/* ------------------------------------------------------------ */

	FntPrint("b=~c686%x~c888 ", rxbuf[2] << 8 | rxbuf[3]);
	switch(PadInfoMode(port,InfoModeCurID,0))
	{
		case 1:
			sprintf(buff,"%02x,%02x",rxbuf[4],rxbuf[5]);
			FntPrint("m=~c686%s~c888\n",buff);
			break;
		case 2:
			sprintf(buff,"%02x,%02x,%02x,%02x",
				rxbuf[4],rxbuf[5],rxbuf[6],rxbuf[7]);
			FntPrint("a=~c686%s~c888\n",buff);
			break;
		case 6:
			sprintf(buff,"%3d,%3d",
				rxbuf[4]+rxbuf[5]*256,rxbuf[6]+rxbuf[7]*256);
			FntPrint("g=~c686%s~c888\n",buff);
			break;
		case 5:
		case 7:
			sprintf(buff,"%02x,%02x,%02x,%02x",
				rxbuf[6],rxbuf[7],rxbuf[4],rxbuf[5]);
			FntPrint("a=~c686%s~c888\n",buff);
			break;
		default:
			FntPrint("\n");
	}

	/* ------------------------------------------------------------ */
	/* アクチュエータ情報の表示   */
	/* ------------------------------------------------------------ */

	/* アクチュエータ情報が取り込み完了するまでは情報を読みに行かない */
	if(initlevel < PadStateStable)
	{
		FntPrint("\n\n");
		return;
	}

	/* アクチュエータ情報を表示 */
	FntPrint(" ac");
	maxnum = PadInfoAct(port,-1,0);
	for(count=0;count<maxnum;++count)
	{
		FntPrint("(~c866%d,%d,%d,%d~c888)=~c686%d~c888 ",
			PadInfoAct(port,count,InfoActFunc),
			PadInfoAct(port,count,InfoActSub),
			PadInfoAct(port,count,InfoActSize),
			PadInfoAct(port,count,InfoActCurr),
			*(&hist->Motor0+count));
	}

	/* コントローラがサポートしているコントローラ ID を表示 */
	FntPrint("\n ID(~c866");
	maxnum = PadInfoMode(port,InfoModeIdTable,-1);
	for(count=0;count<maxnum;++count)
	{
		FntPrint("%x",PadInfoMode(port,InfoModeIdTable,count));
		if(count != maxnum-1)
		{
			FntPrint(",");
		}
	}

	/* 同時に制御可能なアクチュエータ番号の組み合わせリスト表示 */
	FntPrint("~c888) cm");
	maxnum = PadInfoComb(port,-1,0);
	for(count=0;count<maxnum;++count)
	{
		FntPrint("(~c866");
		for(pos=0;pos<PadInfoComb(port,count,-1);++pos)
		{
			FntPrint("%x",PadInfoComb(port,count,pos));
			if(pos != PadInfoComb(port,count,-1)-1)
			{
				FntPrint(",");
			}
		}
		FntPrint("~c888) ");
	}

	/* コントローラのモード切り替えスイッチロック状態表示 */
	FntPrint("sw(~c686");
	switch(hist->Lock)
	{
		case 0:
		case 2:
			FntPrint("unlock");
			break;
		case 3:			
			FntPrint(" lock ");
			break;
	}
	FntPrint("~c888)\n");
}

