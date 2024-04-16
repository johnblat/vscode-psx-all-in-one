$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

clip: Divide 関数サンプルプログラム

                       Copyright(C) 1995,1997 Sony Computer Entertainment Inc.
                                                     All rights reserved.

＜サンプルの解説＞
	libgte の Divide* 関数のサンプルプログラムです。
	Divide 関数はポリゴン分割によってテクスチャの歪みを抑制します。
	このサンプルではフラットテクスチャ四角形 POLY_FT4 を DivideFT4() 
	によって分割しています。

＜起動方法＞ 
	> psymake all	コンパイル
	> psymake load	必要なデータのロード
	> run tuto0	実行


＜コントローラの操作方法＞
	L1,L2 : 前後の移動
	R1,R2 : 回転
        ○,△,□,×： 回転
	方向キー：	上下左右の移動
	
	START + 方向キー :	分割数の変更
	SELECT + 方向キー :	picture area の変更



＜備考＞
	分割の深さのの初期値は 2 (4x4 分割), picture area の初期値は 
	540(幅) x 170(高さ) です。


	通常 picture area は画面いっぱいに設定してもちいますが、このサ
	ンプルでは、分割の様子をわかりやすくするために画面よりも内側に
	なるように設定してあります。インジケータの白線で囲まれた領域
	が picture area です。Divide 関数では、この領域からはみ出した
	部分はそれ以上分割されません。

	Divide 関数は、分割したポリゴンを関数呼び出し時に指定した OT 
	エントリに直接登録します。すなわち分割後の otz に関わりなくす
	べて同じエントリに登録されます。

	Divide 関数は、分割前に一度だけ法線クリップの判定をおこないま
	す。このため、視点に接近した大きなポリゴンなどでは、分割前に全
	体がクリップされてしまい、期待した部分が描画されない場合があり
	ます。これを避けるには、Divide 関数の代わりに RCpoly 関数を用
	いてください。RCpoly の使い方は以下を参考にしてください。

u_long *DivideFT4(v0,v1,v2,v3,uv0,uv1,uv2,uv3,rgbc,s,ot,divp)
SVECTOR *v0,*v1,*v2,*v3;
u_long *uv0,*uv1,*uv2,*uv3;
CVECTOR *rgbc;
POLY_FT4 *s;
u_long *ot;
DIVPOLYGON4 *divp;  
{
    int i;
    u_short sz;
    long p,flag;
    RVECTOR *r0,*r1,*r2,*r3;
    
    r0= &divp->r0;
    r1= &divp->r1;
    r2= &divp->r2;
    r3= &divp->r3;
    divp->cr[0].r0 = r0;		
    divp->cr[0].r1 = r1;		
    divp->cr[0].r2 = r2;		
    divp->cr[0].r3 = r3;		
    r0->v = *v0;
    r1->v = *v1;
    r2->v = *v2;
    r3->v = *v3;

    if(RotAverageNclip4
       (
	&r0->v,&r1->v,&r2->v,&r3->v,
	&r0->sxy,&r1->sxy,&r2->sxy,&r3->sxy,
	&p,&sz,&flag)>0 )
	{
	    ReadSZfifo4(&r0->sz,&r1->sz,&r2->sz,&r3->sz);
	    divp->ot = ot;
	    divp->rgbc = *rgbc;
	    divp->clut = (*uv0)>>16;
	    divp->tpage = (*uv1)>>16;
	    *(u_long *)(r0->uv) = *uv0;
	    *(u_long *)(r1->uv) = *uv1;
	    *(u_long *)(r2->uv) = *uv2;
	    *(u_long *)(r3->uv) = *uv3;
	    s =(POLY_FT4 *)RCpolyFT4(s,divp);
	}
    return((u_long *)s);
}
