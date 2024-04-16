/* $PSLibId: Run-time Library Release 4.4$ */
/***********************************************
 *	a_intr.c
 *
 *	Copyright (C) 1996,1997 Sony Computer Entertainment Inc.
 *		All Rights Reserved.
 ***********************************************/
#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "model.h"
#include "matrix.h"
#include "a_intr.h"

static void rotate_matrix(VECTOR *av, long delta, MATRIX *m);
#define mul(x,y) (((x)*(y))/4096)

/* 軸 MIMe
 ***********************************************/
/* キーごとの差分回転軸を求める (mh->axis) */
void axis_interpolate_start(void)
{
    int mime, part;
    MATRIX rotm;
    long *bv, *mv, *dv;
    MODEL *mh;

    for (mime=0; mime<MIMENUM; mime++){		/* キーごとに */
	for (part=0; part<PARTNUM; part++){	/* 関節ごとに */
	    mh= &model_hand[ARR_MIME[mime]][part];

	    TransposeMatrix(&model_hand[ARR_BASE][part].modelcoord.coord, &rotm);
	    MulMatrix(&rotm, &mh->modelcoord.coord);
	    /* このとき rotm は基本形からキーへの回転マトリクスとなっている */
	    if(IsIdMatrix(&rotm)==1){		/* 差分回転なしのとき */
		mh->axis.vx=0;
		mh->axis.vy=0;
		mh->axis.vz=0;

	    } else{				/* 差分回転ありのとき */
		MATRIX r1, eig, ieig, xrot;
		long theta;

		/* 固有ベクトルと差分回転軸は同じ向き */
		EigenVector(&rotm, &mh->axis);
		/* 回転角度を求める */
			/* rotm を X 軸周りの回転マトリクス xrot に変換する */
		EigenVec2Mat(&mh->axis, &eig); /* eig は X軸を差分回転軸に移すマトリクス */ 
		TransposeMatrix(&eig,&ieig);	/* ieig は eig の逆 */
		MulMatrix0(&ieig,&rotm,&r1);
		MulMatrix0(&r1,&eig,&xrot);
		theta=ratan2(xrot.m[1][2], xrot.m[1][1]); /* theta が差分回転角 */

		/* 差分回転軸の長さが theta となるようにする */
		mh->axis.vx= mul(mh->axis.vx,theta); /* mul(a,b)=a*b/4096 */
		mh->axis.vy= mul(mh->axis.vy,theta);
		mh->axis.vz= mul(mh->axis.vz,theta);
	    }

	    /* set translation diff */
	    bv= &model_hand[ARR_BASE][part].modelcoord.coord.t[0];
	    mv= &model_hand[ARR_MIME[mime]][part].modelcoord.coord.t[0];
	    dv= model_hand[ARR_MIME[mime]][part].dtrans;
	    dv[0] = mv[0] - bv[0];
	    dv[1] = mv[1] - bv[1];
	    dv[2] = mv[2] - bv[2];
	}
    }

}


/* 座標系をもとの状態へ戻す */
void  axis_interpolate_reset(void)
{
    int part;

    for (part=0; part<PARTNUM; part++){
	/* 基本形の座標マトリクスをコピーする */
	model_hand[ARR_INTR][part].modelcoord.coord=
	    model_hand[ARR_BASE][part].modelcoord.coord;

	model_hand[ARR_INTR][part].modelcoord.flg=0;
    }
}


/* 実際の軸回転 MIMe の演算をおこなう */
void axis_interpolate(long *pr)
{
    int mime, part;
    VECTOR ax_mime;


    for (part=0; part<PARTNUM; part++){    /* 各関節ごとに */

	/* 差分回転軸を多重内挿する */
	ax_mime.vx=0;
	ax_mime.vy=0;
	ax_mime.vz=0;
	for (mime=0; mime<MIMENUM; mime++){	/* key ごとに */
	    MODEL *mh;

	    if (pr[mime]){
		mh= &model_hand[ARR_MIME[mime]][part];
		/* ax_mime.vx/vy/vz format: (1,7,24) */
		ax_mime.vx+=mh->axis.vx*pr[mime]; /* 軸に重みをつけて足し込む*/
		ax_mime.vy+=mh->axis.vy*pr[mime];
		ax_mime.vz+=mh->axis.vz*pr[mime];
	    }
	}

	if (ax_mime.vx||ax_mime.vy||ax_mime.vz){ /* 内挿された軸が0でなければ*/
	    MATRIX m;
	    long delta;
	    VECTOR ax_norm;

	    /* 内挿された軸の長さ delta (回転角)を求め */
	    /* また正規化された軸 ax_norm を求める */
			    /* (delta=ax_mime/ax_norm, format: (1,19,12)) */
	    delta=newVectorNormal(&ax_mime, &ax_norm);
	    /* 内挿された軸のとおりの回転をするマトリクス m を求める */
	    rotate_matrix(&ax_norm, delta, &m);

	    /* 座標マトリクスを回転させる */
	    MulMatrix(&model_hand[ARR_INTR][part].modelcoord.coord, &m);
	}


	/* 平行移動部の内挿 */
	for (mime=0; mime<MIMENUM; mime++){
	    long *iv, *dv;

	    iv= &model_hand[ARR_INTR][part].modelcoord.coord.t[0];
	    dv= model_hand[ARR_MIME[mime]][part].dtrans;
	    iv[0] += (dv[0]*pr[mime]/4096);
	    iv[1] += (dv[1]*pr[mime]/4096);
	    iv[2] += (dv[2]*pr[mime]/4096);
	}
    }
}

/* **********************************************
 *
 *	void rotate_matrix(VECTOR *av, long delta, MATRIX *m)
 *
 *	  正規化された軸 av の回りを角度 delta 回転させるマトリクスを求める関数
 *
 *
 * 軸 {x,y,z} 周りに角度 dの回転マトリクス m は、s=sin(d), c=cos(d) として
 *
 *  m =  {[ (1-c) x^2 + c,	   (1-c) x y + z s,	   (1-c) x z - y s ],
 *	  [ (1-c) x y - z s,	   (1-c) y^2 + c,	   (1-c) y z + x s ],
 *	  [ (1-c) x z + y s,	   (1-c) y z - x s, 	   (1-c) z^2 + c ]}
 *
 *	と計算できる
 *
 ***********************************************/
static void rotate_matrix(VECTOR *av, long delta, MATRIX *m)
{
    long c,s, c1, s1, tmp;
    extern unsigned long rcossin_tbl[];

    c=rcossin_tbl[delta];
    s=(c<<16)>>16;
    c>>=16;

    c1= 4096-c;

    /* mul(a,b)=a*b/4096 */
    m->m[0][0]= mul(c1,mul(av->vx,av->vx)) + c;
    m->m[1][1]= mul(c1,mul(av->vy,av->vy)) + c;
    m->m[2][2]= mul(c1,mul(av->vz,av->vz)) + c;

    s1=mul(av->vz,s);
    tmp=mul(av->vx,av->vy);
    m->m[0][1]= mul(c1,tmp) + s1;
    m->m[1][0]= mul(c1,tmp) - s1;

    s1=mul(av->vy,s);
    tmp=mul(av->vx,av->vz);
    m->m[0][2]= mul(c1,tmp) - s1;
    m->m[2][0]= mul(c1,tmp) + s1;

    s1=mul(av->vx,s);
    tmp=mul(av->vy,av->vz);
    m->m[1][2]= mul(c1,tmp) + s1;
    m->m[2][1]= mul(c1,tmp) - s1;
}
