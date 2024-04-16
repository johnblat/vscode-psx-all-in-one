/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	MIMe Animation
 *
 *
 *	"nmime.c" ******** routine
 *
 *		Version 1.**	Mar,  14, 1994
 *
 *		Version ??	Aug/3/1994 S. Aoki
 *			 multi TMD handling
 *
 *		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>  
#define NMIMEMAIN
#include "nmime.h"  

/* TMD file の オブジェクト情報  */
typedef struct {
    SVECTOR *vtxtop;	/* 頂点情報のスタートポインタ */
    u_long vtxtotal;	/* 頂点情報の総数 */
    SVECTOR *nrmtop;	/* 法線情報のスタートポインタ */
    u_long nrmtotal;	/* 法線情報の総数 */
    u_long *prmtop;		/* ポリゴン情報のスタートポインタ（未使用）*/
    u_long prmtotal;	/* ポリゴン情報の総数（未使用）*/
    u_long scale;		/* オブジェクトのスケール（未使用）*/
} TMDOBJECT;

/* MDF file（差分ファイル）の オブジェクト情報  */
typedef struct {
    SVECTOR		*top;	/* 差分情報のスタートポインタ */
    u_long		offset;	/* 差分情報のオフセット */
    u_long		total;	/* 差分情報の総数 */
    u_long		object;	/* 対応するのＴＭＤオブジェクト番号 */
} MDFOBJECT;

/* TMD file の MIMe計算用構造体   */
typedef struct {
    u_long 		objtotal;	/* ＴＭＤ内のオブジェクトの総数 */
    SVECTOR		*orgvtx;	/* オリジナル頂点情報の退避エリアポインタ */
    SVECTOR		*orgnrm;	/* オリジナル法線情報の退避エリアポインタ */
    TMDOBJECT 	*tmdobj;	/* オブジェクト情報配列のポインタ */
} TMDDATA;

/* MDF file の MIMe計算用構造体   */
typedef struct {
    u_long 		objtotal;	/* ＭＤＦ内のオブジェクトの総数 */
    MDFOBJECT	*mdfvct;	/* オブジェクト情報配列のポインタ */		
    long		*mime;		/* MIMeの重み係数(制御変数)配列のポインタ */
} MDFDATA;

static void set_tmd_info(u_long *model, TMDDATA *tmddt);
static void original_vertex_save(TMDDATA *tmddt, SVECTOR *orgvtxbf);
static void original_normal_save(TMDDATA *tmddt, SVECTOR *orgnrmbf);
static void reset_mime_vertex(TMDDATA *tmddt);
static void reset_mime_norm(TMDDATA *tmddt);
static void set_mime_vertex(TMDDATA *tmddt, MDFDATA *vtxdt);
static void set_mime_norm(TMDDATA *tmddt, MDFDATA *nrmdt);
static void set_mdf_vertex(u_long *mdfdtvtx, MDFDATA *vtxdt);
static void set_mdf_normal(u_long *mdfdtnrm, MDFDATA *nrmdt);


TMDOBJECT tmdobj0[MIMEMODELMAX][OBJMAX]; /* MIMe計算用TMDオブジェクト情報配列 */

MDFOBJECT mdfvtx0[MIMEMODELMAX][MIMEMAX]; /* MIMe計算用頂点差分データ */

MDFOBJECT mdfnrm0[MIMEMODELMAX][MIMEMAX]; /* MIMe計算用法線差分データ */

/* MIMe計算用TMDデータ */
TMDDATA tmddt0[MIMEMODELMAX];
/* differential vertex data for MIMe */
MDFDATA vtxdt0[MIMEMODELMAX];
/* differential normal data for MIMe */
MDFDATA nrmdt0[MIMEMODELMAX];

/*==MIME Function==================*/

int init_mime_data(int num, u_long *modeladdr, u_long *mdfdatavtx, u_long *mdfdatanrm, u_long *orgvtxbuf, u_long *orgnrmbuf)
{
    int i;
/*    printf("in init_mime_data\n");*/

    tmddt0[num].objtotal=1;
    tmddt0[num].orgvtx=(SVECTOR *)orgvtxbuf;
    tmddt0[num].orgnrm=(SVECTOR *)orgnrmbuf;
    tmddt0[num].tmdobj=tmdobj0[num];
    vtxdt0[num].objtotal=1;
    vtxdt0[num].mdfvct=mdfvtx0[num];
    vtxdt0[num].mime= &mimepr[num][0];
    nrmdt0[num].objtotal=1;
    nrmdt0[num].mdfvct=mdfnrm0[num];
    nrmdt0[num].mime= &mimepr[num][0];

    set_tmd_info(modeladdr, &tmddt0[num]);	/* TMDデータのセット */

    if (mdfdatavtx){
	/* オリジナル頂点情報の退避 */
	original_vertex_save(&tmddt0[num], (SVECTOR *)orgvtxbuf);
	/* 頂点差分データのセット */
	set_mdf_vertex(mdfdatavtx,&vtxdt0[num]);
    }
    if (mdfdatanrm){
	/* オリジナル法線情報の退避 */
	original_normal_save(&tmddt0[num], (SVECTOR *)orgnrmbuf); 
	/* 法線差分データのセット */ 
	set_mdf_normal(mdfdatanrm,&nrmdt0[num]);
    }
#if 0
    for (i=0; i<MIMEMAX; i++){
	mimepr[num][i] = 0;
    }
#endif /* 0 */
    return vtxdt0[num].objtotal;
}

/* 頂点のMIMe計算処理 */
void vertex_mime(int num)
{
    reset_mime_vertex(&tmddt0[num]);	/* オリジナル頂点情報の復帰　*/
    set_mime_vertex(&tmddt0[num],&vtxdt0[num]); /* MIMe計算処理（頂点）　*/

}

/* 法線のMIMe計算処理 */
void normal_mime(int num)
{
    reset_mime_norm(&tmddt0[num]);		/* オリジナル法線情報の復帰　*/
    set_mime_norm(&tmddt0[num],&nrmdt0[num]); /* MIMe計算処理（法線）　*/
}


/* TMDデータのセッティング */
static void set_tmd_info(u_long *model, TMDDATA *tmddt)
{
    u_long size,*dop;
    int i,n;

    dop= model;
    dop++;   			/* ファイルヘッダの読み飛ばし */
    dop++;   			/* フラグの読み飛ばし */
    n = tmddt->objtotal = *dop;  	/* TMDデータのオブジェクト数 */

    dop++;
    for(i = 0; i < n; i++){
	tmddt->tmdobj[i].vtxtop = (SVECTOR *)*dop++ ;	/* 頂点情報のポインタ */	
	tmddt->tmdobj[i].vtxtotal = *dop++ ;		/* 頂点情報の総数 */
	tmddt->tmdobj[i].nrmtop = (SVECTOR *)*dop++ ;	/* 法線情報のポインタ */
	tmddt->tmdobj[i].nrmtotal = *dop++ ;		/* 法線情報の総数 */
	tmddt->tmdobj[i].prmtop = (u_long *)*dop++ ;	/* ポリゴン情報ポインタ */
	tmddt->tmdobj[i].prmtotal = *dop++ ;		/* ポリゴン情報の総数 */
	tmddt->tmdobj[i].scale  = *dop++ ;		/* オブジェクトのスケール */
    } 

#ifdef VERBOSE
    printf("objtotal = %d\n", tmddt->objtotal);
    for(i = 0; i < n; i++){
	printf("v_top%d   = %x\n", 	i, 	tmddt->tmdobj[i].vtxtop );
	printf("v_num%d   = %d\n",	i,  	tmddt->tmdobj[i].vtxtotal );
	printf("nrm_top%d = %x\n",	i,	tmddt->tmdobj[i].nrmtop );
	printf("nrm_num%d = %d\n",	i, 	tmddt->tmdobj[i].nrmtotal );
	printf("prm_top%d = %x\n",	i, 	tmddt->tmdobj[i].prmtop );
	printf("prm_num%d = %d\n",	i, 	tmddt->tmdobj[i].prmtotal );
	printf("scale%d   = %d\n",	i, 	tmddt->tmdobj[i].scale );
    } 
#endif /* VERBOSE */
}

/* 頂点差分データのセット */
static void set_mdf_vertex(u_long *mdfdtvtx, MDFDATA *vtxdt)
{
    int i,n;
    u_long *dop2;

    dop2=mdfdtvtx;
    n = vtxdt->objtotal = *dop2++;
/*	n = vtxdt->objtotal = 5;*/
/*	printf("vtxdt->objtotal=%d\n",n); */

    for(i = 0; i < n; i++){
	vtxdt->mdfvct[i].object = *dop2++;		/* オブジェクト番号 */ 		
	vtxdt->mdfvct[i].offset = *dop2++;		/* 差分情報のオフセット */ 	
	vtxdt->mdfvct[i].total 	= *dop2++;		/* 差分情報の総数 */ 	
	vtxdt->mdfvct[i].top 	= (SVECTOR *)dop2; 	/* 差分情報のポインタ */
	dop2 	+= vtxdt->mdfvct[i].total*2;
    }
}

/* 法線差分データのセット */
static void set_mdf_normal(u_long *mdfdtnrm, MDFDATA *nrmdt)
{
    int i,n;
    u_long  *dop2;


    dop2= mdfdtnrm;
    n = nrmdt->objtotal = *dop2++;
    for(i = 0; i < n; i++){
	nrmdt->mdfvct[i].object = *dop2++;		/* オブジェクト番号 */ 	
	nrmdt->mdfvct[i].offset = *dop2++;		/* 差分情報のオフセット */ 	
	nrmdt->mdfvct[i].total 	= *dop2++;		/* 差分情報の総数 */ 	 
	nrmdt->mdfvct[i].top 	= (SVECTOR *)dop2; 	/* 差分情報のポインタ */
	dop2 	+= nrmdt->mdfvct[i].total*2;
    }
}


/* オリジナル頂点情報の退避　*/
static void original_vertex_save(TMDDATA *tmddt, SVECTOR *orgvtxbf)
{
    SVECTOR *otp,*bsp,*dfp;
    int i,j,n,m;

    bsp= tmddt->orgvtx = orgvtxbf;
    m = tmddt->objtotal;
    n=0;
    for( j = 0; j < m; j++){
  	otp = tmddt->tmdobj[j].vtxtop;
	bsp += n;
	n = tmddt->tmdobj[j].vtxtotal;
        for( i = 0; i < n; i++) *(bsp+i) = *(otp+i);
    }
}

/* オリジナル法線情報の退避　*/
static void original_normal_save(TMDDATA *tmddt, SVECTOR *orgnrmbf)
{
    SVECTOR *otp,*bsp,*bspb,*dfp;
    int i,j,n,m;

    bsp= tmddt->orgnrm = orgnrmbf;
    m = tmddt->objtotal;
    n=0;
    for( j = 0; j < m; j++){
  	otp = tmddt->tmdobj[j].nrmtop;
	bsp  += n;
	n = tmddt->tmdobj[j].nrmtotal;
        for( i = 0; i < n; i++) *(bsp+i) = *(otp+i);
    }
}

/* オリジナル頂点情報の復帰　*/
static void reset_mime_vertex(TMDDATA *tmddt)
{
    SVECTOR *otp,*bsp;
    int i,j,n,m;

    bsp= tmddt->orgvtx;
    m = tmddt->objtotal;
    n=0;
    for( j = 0; j < m; j++){
  	otp = tmddt->tmdobj[j].vtxtop;
	bsp += n;
	n = tmddt->tmdobj[j].vtxtotal;
        for( i = 0; i < n; i++) *(otp+i) = *(bsp+i);
    }
}

/* オリジナル法線情報の復帰　*/
static void reset_mime_norm(TMDDATA *tmddt)
{
    SVECTOR *otp,*bsp;
    int i,j,n,m;

    bsp= tmddt->orgnrm;
    m = tmddt->objtotal;
    n=0;
    for( j = 0; j < m; j++){
  	otp = tmddt->tmdobj[j].nrmtop;
	bsp += n;
	n = tmddt->tmdobj[j].nrmtotal;
        for( i = 0; i < n; i++) *(otp+i) = *(bsp+i);
    }
}

/* MIMe計算処理（頂点）　*/
static void set_mime_vertex(TMDDATA *tmddt, MDFDATA *vtxdt)
{
    SVECTOR *otp,*bsp,*dfp;
    int i,n;

    n = vtxdt->objtotal;

    for( i = 0; i < n; i++){
  	otp = tmddt->tmdobj[vtxdt->mdfvct[i].object].vtxtop+vtxdt->mdfvct[i].offset;
	dfp = vtxdt->mdfvct[i].top;
	if( vtxdt->mime[i] !=0 ) gteMIMefunc(otp,dfp, vtxdt->mdfvct[i].total,vtxdt->mime[i]);  
    }
}


/* MIMe計算処理（法線）　*/
static void set_mime_norm(TMDDATA *tmddt, MDFDATA *nrmdt)
{
    SVECTOR *otp,*bsp,*dfp;
    VECTOR tmp;
    int i,n;

    n = nrmdt->objtotal;
    for( i = 0; i < n; i++){
  	otp = tmddt->tmdobj[nrmdt->mdfvct[i].object].nrmtop+nrmdt->mdfvct[i].offset;
	dfp = nrmdt->mdfvct[i].top;
	if( nrmdt->mime[i] !=0 ) 
	    {
	 	gteMIMefunc(otp,dfp, nrmdt->mdfvct[i].total,nrmdt->mime[i]);  
#if 0
		tmp.vx = (otp+i)->vx;
 		tmp.vy = (otp+i)->vy;
		tmp.vz = (otp+i)->vz;
		VectorNormal(&tmp,&tmp);	/* 正規化処理　*/
		(otp+i)->vx = tmp.vx;
 		(otp+i)->vy = tmp.vy;
		(otp+i)->vz = tmp.vz;
#endif
	    }
    }
}


/* 頂点情報の復帰のみおこなうインタフェース関数 */
void reset_mime_vdf(int num)
{
    reset_mime_vertex(&tmddt0[num]);
}

/* 頂点情報の復帰のみおこなうインタフェース関数 */
void reset_mime_ndf(int num)
{
    reset_mime_norm(&tmddt0[num]);
}
