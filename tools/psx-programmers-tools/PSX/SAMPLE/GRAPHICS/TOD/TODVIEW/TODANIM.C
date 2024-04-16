/* $PSLibId: Run-time Library Release 4.4$ */
/* TODアニメーション処理関数群（その２） */
/*
 *		Version 1.30	Apr, 17, 1996
 *		Version 1.31	Oct, 14, 1997
 *			- New branch "TOD_MATRIX" is added.
 *
 *		Copyright (C) 1995-1997 by Sony Computer Entertainment Inc.
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "tod.h"

extern GsF_LIGHT pslt[3];		/* 光源×３個 */

/* プロトタイプ宣言 */
u_long *TodSetFrame();
u_long *TodSetPacket();

/* １フレーム分のTODデータの処理 */
u_long *TodSetFrame(currentFrame, todP, objTblP, tmdIdP, tmdTblP, mode)
int currentFrame;	/* 現在の時刻（フレーム番号） */
u_long *todP;		/* 再生中のTODデータへのポインタ */
TodOBJTABLE *objTblP;	/* オブジェクトテーブルへのポインタ */
int *tmdIdP;		/* モデリングデータIDリスト */
u_long *tmdTblP;	/* TMDデータへのポインタ */
int mode;		/* 再生モード */
{
	u_long hdr;
	u_long nPacket;
	u_long frame;
	int i;

	/* フレーム情報を得る */
	hdr = *todP;			/* フレームヘッダを得る */
	nPacket = (hdr>>16)&0x0ffff;	/* パケット個数 */
	frame = *(todP+1);		/* 時刻（フレーム番号） */

	/* 現在時刻のフレームでなければ何も処理しない */
	if(frame > currentFrame) return todP;

	/* フレームの中の各パケットを処理する */
	todP += 2;
	for(i = 0; i < nPacket; i++) {
		todP = TodSetPacket(todP, objTblP, tmdIdP, tmdTblP, mode);
	}

	/* 次のフレームへのポインタを返す */
	return todP;	
}

/* １パケットの処理 */
u_long *TodSetPacket(packetP, tblP, tmdIdP, tmdTblP, mode)
u_long *packetP;
TodOBJTABLE *tblP;
int *tmdIdP;
u_long *tmdTblP;
int mode;
{
	u_long *dP;		/* 処理中のTODへのポインタ */

	u_long hdr;
	u_long id;
	u_long flag;
	u_long type;
	u_long len;

	/* 処理中オブジェクト */
	GsDOBJ2 *objP;		/* オブジェクトへのポインタ */
	GsCOORD2PARAM *cparam;	/* 座標パラメータ(R,S,T) */
	MATRIX *coordp;		/* 座標マトリクス */
	GsDOBJ2 *parentP;	/* 親オブジェクト */
	VECTOR v;
	SVECTOR sv;

	/* ダミーオブジェクト変数 */
	GsDOBJ2 dummyObj;
	MATRIX dummyObjCoord;
	GsCOORD2PARAM dummyObjParam;


	/* パケット情報を得る */
	dP = packetP;
	hdr = *dP++;
	id = hdr&0x0ffff;	/* ID number */
	type = (hdr>>16)&0x0f;	/* Packet type (TOD_???) */
	flag = (hdr>>20)&0x0f;	/* Flags */
	len = (hdr>>24)&0x0ff;	/* パケットのワード長 */

	/* ID番号で該当するオブジェクトをサーチ */
	objP = TodSearchObjByID(tblP, id);
	if(objP == NULL) {
		/* ない場合はダミーオブジェクトを使う */
		objP = &dummyObj;
		coordp = &dummyObjCoord;
		cparam = &dummyObjParam;
	}
	else {
		coordp = &(objP->coord2->coord);
		cparam = (objP->coord2->param);
		objP->coord2->flg = 0;
	}

	/* パケットのタイプ別に処理 */
	switch(type) {
	    case TOD_ATTR:/* アトリビュート変更 */
		objP->attribute = (objP->attribute&*dP)|*(dP+1);	
		dP += 2;
		break;

	    case TOD_COORD:/* 座標変更（移動／回転／拡大） */

		/* 該当オブジェクトのcoordinateの内容を更新する
		   更新するパラメータはcparamメンバに保管してある */

		if(flag&0x01) {

			/* 前フレームからの差分の場合 */

			/* Rotation */
			if(flag&0x02) {
				cparam->rotate.vx += (*(((long *)dP)+0))/360;
				cparam->rotate.vy += (*(((long *)dP)+1))/360;
				cparam->rotate.vz += (*(((long *)dP)+2))/360;
				dP += 3;
			}
			/* Scaling */
			if(flag&0x04) {
				cparam->scale.vx
				 = (cparam->scale.vx**(((short *)dP)+0))/4096;
				cparam->scale.vy
				 = (cparam->scale.vy**(((short *)dP)+1))/4096;
				cparam->scale.vz
				 = (cparam->scale.vz**(((short *)dP)+2))/4096;
				dP += 2;
			}
			/* Transfer */
			if(flag&0x08) {
				cparam->trans.vx += *(((long *)dP)+0);
				cparam->trans.vy += *(((long *)dP)+1);
				cparam->trans.vz += *(((long *)dP)+2);
				dP += 3;
			}

			RotMatrix(&(cparam->rotate), coordp);
			ScaleMatrix(coordp, &(cparam->scale));
			TransMatrix(coordp, &(cparam->trans));
		}
		else {
			/* 絶対値の場合 */

			/* Rotation */
			if(flag&0x02) {
				cparam->rotate.vx = (*(((long *)dP)+0))/360;
				cparam->rotate.vy = (*(((long *)dP)+1))/360;
				cparam->rotate.vz = (*(((long *)dP)+2))/360;
				dP += 3;
				RotMatrix(&(cparam->rotate), coordp);
			}

			/* Scaling */
			if(flag&0x04) {
				cparam->scale.vx = *(((short *)dP)+0);
				cparam->scale.vy = *(((short *)dP)+1);
				cparam->scale.vz = *(((short *)dP)+2);
				dP += 2;
				if(!(flag&0x02))
					RotMatrix(&(cparam->rotate), coordp);
				ScaleMatrix(coordp, &(cparam->scale));
			}
			/* Transfer */
			if(flag&0x08) {
				cparam->trans.vx = *(((long *)dP)+0);
				cparam->trans.vy = *(((long *)dP)+1);
				cparam->trans.vz = *(((long *)dP)+2);
				dP += 3;
				TransMatrix(coordp, &(cparam->trans));
			}
		}
		break;

	    case TOD_MATRIX:
		*coordp = *(MATRIX *)dP;
		dP += 8;
		break;

	    case TOD_TMDID:	/* TMDとのリンクを設定 */
		if(tmdTblP != NULL) {
			GsLinkObject4((u_long)TodSearchTMDByID(tmdTblP, tmdIdP,
					(unsigned long)(*dP&0xffff)), objP, 0);

		}
		break;

	    case TOD_PARENT:	/* 親オブジェクトの設定 */
		if(mode != TOD_COORDONLY) {
			if((*dP == NULL)||(*dP == 0xffff)) {
				objP->coord2->super = NULL;
				dP++;
			}
			else {
				parentP = TodSearchObjByID(tblP, *dP++);
				objP->coord2->super = parentP->coord2;
			}
		}
		break;

	    case TOD_OBJCTL:
		/* テーブル内オブジェクトの管理 */
		if(tblP != NULL) {
			switch(flag) {
			    case 0:
				/* 新規オブジェクトの生成 */
				TodCreateNewObj(tblP, id);
				break;
			    case 1:
				/* オブジェクトの削除 */
				TodRemoveObj(tblP, id);
				break;
			}
		}
		break;

	    case TOD_LIGHT:
		/* 光源の設定 */
		if(flag&0x02) {
			if(flag&0x01) {
				pslt[id].vx += *(((long *)dP)+0);
				pslt[id].vy += *(((long *)dP)+1);
				pslt[id].vz += *(((long *)dP)+2);
			}
			else {
				pslt[id].vx = *(((long *)dP)+0);
				pslt[id].vy = *(((long *)dP)+1);
				pslt[id].vz = *(((long *)dP)+2);
			}
			dP += 3;
		}
		if(flag&0x04) {
			if(flag&0x01) {
				pslt[id].r += *(((u_char *)dP)+0);
				pslt[id].g += *(((u_char *)dP)+1);
				pslt[id].b += *(((u_char *)dP)+2);
			}
			else {
				pslt[id].r = *(((u_char *)dP)+0);
				pslt[id].g = *(((u_char *)dP)+1);
				pslt[id].b = *(((u_char *)dP)+2);
			}
			       dP++;
		}
		GsSetFlatLight(id, &pslt[id]);
		break;

	    case TOD_USER0:
		/* ここにユーザ定義パケットの処理を記述する  */
		break;
	}
	/* 次のパケットへのポインタを返す */
	return packetP+len;
}

