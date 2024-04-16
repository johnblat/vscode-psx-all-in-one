/* $PSLibId: Run-time Library Release 4.4$ */
/* TODアニメーション処理関数群（その１） */
/*
 *		Version 1.30	Apr,  17, 1996
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment Inc.
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "tod.h"

/* プロトタイプ宣言 */
void TodInitObjTable();
GsDOBJ2 *TodSearchObjByID();
GsDOBJ2 *TodCreateNewObj();
GsDOBJ2 *TodRemoveObj();
u_long *TodSearchTMDByID();

/* オブジェクトテーブルの初期化
 *       GsDOBJ2,GsCOORDINATE2, GsCOORD2PARAM を組にして用意
 */
void TodInitObjTable(tblP, objAreaP, objCoordP, objParamP, nObj)
TodOBJTABLE *tblP;		/* テーブルへのポインタ */
GsDOBJ2 *objAreaP;		/* オブジェクト(DOBJ2)領域 */
GsCOORDINATE2 *objCoordP;	/* ローカル座標領域 */
GsCOORD2PARAM *objParamP;	/* ローカル座標のパラメータ領域 */
int nObj;			/* 最大OBJ数（テーブルの大きさ） */
{
	int i;

	tblP->nobj = 0;		/* 有効オブジェクト数 */
	tblP->maxobj = nObj;	/* 最大オブジェクト数 */
	tblP->top = objAreaP;	/* オブジェクト領域 */
	i = 0;
	for(i = 0; i < nObj; i++) {
		/* オブジェクト構造体のメンバを初期化 */
		objAreaP->attribute = 0x80000000;
		objAreaP->id = TOD_OBJ_UNDEF;
		objAreaP->coord2 = objCoordP;
		objAreaP->coord2->param = objParamP;
		objAreaP->tmd = NULL;
		objAreaP++;
		objCoordP++;
		objParamP++;
	}
}

/* オブジェクトテーブルからID番号で検索 */
GsDOBJ2 *TodSearchObjByID(tblP, id)
TodOBJTABLE *tblP;	/* オブジェクトテーブルへポインタ */
u_long id;		/* 検索するオブジェクトのID番号 */
{
	GsDOBJ2 *objP;
	int i;

	objP = tblP->top;
	for(i = 0; i < tblP->nobj; i++) {
		if(id == objP->id) break;
		objP++;
	}
	if(i == tblP->nobj) {
		/* Not Found */
		return(NULL);
	}
	else {
		/* Return pointer */
		return(objP);
	}
}

/* 新しいオブジェクトをテーブル内に生成 */
GsDOBJ2 *TodCreateNewObj(tblP, id)
TodOBJTABLE *tblP;	/* オブジェクトテーブルへポインタ */
u_long id;		/* 生成するオブジェクトのID番号 */
{
	GsDOBJ2 *objP;
	int i;

	/* 空き領域(ID==TOD_OBJ_UNDEF)を探す */
	objP = tblP->top;
	for(i = 0; i < tblP->nobj; i++) {
		if(objP->id == TOD_OBJ_UNDEF) break;
		objP++;
	}

	/* Deletedの領域(隙間)があるか？ */
	if(i < tblP->nobj) {
		objP->id = id;
		objP->attribute = 0;
		GsInitCoordinate2(WORLD, objP->coord2);
		objP->tmd = NULL;
		return(objP);
	}
	else {
		/* テーブルの最後尾に新規オブジェクトを追加 */
		if(i < tblP->maxobj) {		/* Table is Full? */
			objP = tblP->top+tblP->nobj;
			tblP->nobj++;
			objP->id = id;
			objP->attribute = 0;
			GsInitCoordinate2(WORLD, objP->coord2);
			objP->tmd = NULL;
			return(objP);
		}
		else {
			/* テーブルがいっぱいならNULLを返す */
			return(NULL);
		}
	}
}

/* テーブルからオブジェクトを削除 */
GsDOBJ2 *TodRemoveObj(tblP, id)
TodOBJTABLE *tblP;	/* オブジェクトテーブルへポインタ */
u_long id;		/* 削除するオブジェクトのID番号 */
{
	GsDOBJ2 *objP;
	int i;

	/* ID番号でオブジェクトを探す */
	objP = tblP->top;
	for(i = 0; i < tblP->nobj; i++) {
		if(objP->id == id) break;
		objP++;
	}

	/* 該当オブジェクトを探して削除 */
	if(i < tblP->nobj) {
		objP->id = TOD_OBJ_UNDEF;
		if(i == tblP->nobj-1) {
			while(objP->id == TOD_OBJ_UNDEF) {
				tblP->nobj--;
				objP--;
			}
		}

		/* アドレスを返す */
		return(objP);
	}
	else {
		/* 見つからなければNULLを返す */
		return(NULL);
	}
}

/* TMD内のモデリングデータをID番号で検索 */
u_long *TodSearchTMDByID(tmdP, idListP, id)
u_long *tmdP;
int *idListP;
u_long id;
{
	int n;

	tmdP++;		/* ヘッダを読み飛ばす */
	n = *tmdP++;	/* モデリングデータの数 */

	while(n > 0) {
		if(id == *idListP) break;
		tmdP += 7;	/* ポインタを進める */
		idListP++;
		n--;
	}
	if(n == 0) {
		/* 見つからなければNULLを返す */
		return(NULL);
	}
	else {
		/* アドレスを返す */
		return(tmdP);
	}
}
