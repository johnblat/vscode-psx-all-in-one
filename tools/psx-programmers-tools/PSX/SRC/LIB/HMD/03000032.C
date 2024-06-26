/* $PSLibId: Run-time Library Release 4.4$ */

/* 
 *	Copyright(C) 1998 Sony Computer Entertainment Inc.
 *  	All rights reserved. 
 */


#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <inline_c.h>
#include <gtemac.h>
#include <asm.h>
#include <libgs.h>
#include <libhmd.h>

typedef struct
{
    long vx0,vy0,vz0;
    long vx1,vy1,vz1;
    long vx2,vy2,vz2;
    short rx,ry,rz;
}  MYVECTOR;

typedef struct
{
  u_short pidx;
  u_char  tframe;
  u_char  tidx;
} ACTR;

typedef struct
{
  GsSEQ *base;
  MYVECTOR *src;
  MYVECTOR *dst;
  MYVECTOR *intr;
} GsARGANIM2;

extern setBezierCof(long ,long ,long ,long ,short *);
extern void setBetaCof(long ,long ,long ,long ,short *);


#if defined(XYZ)
#define FUNC		GsU_03000032
#define ROT_MATRIX	RotMatrix/*XYZ*/

#elif defined(XZY)
#define FUNC		GsU_03001032
#define ROT_MATRIX	RotMatrixXZY

#elif defined(YXZ)
#define FUNC		GsU_03002032
#define ROT_MATRIX	RotMatrixYXZ

#elif defined(YZX)
#define FUNC		GsU_03003032
#define ROT_MATRIX	RotMatrixYZX

#elif defined(ZXY)
#define FUNC		GsU_03004032
#define ROT_MATRIX	RotMatrixZXY

#elif defined(ZYX)
#define FUNC		GsU_03005032
#define ROT_MATRIX	RotMatrixZYX

#else
#error "unknown rotation order specified."
#endif

int
FUNC(GsARGUNIT_ANIM *sp0)
{
  GsCOORDUNIT *dtop;
  u_long ofs0,ofs1;
  GsSEQ *seq;
  GsARGANIM2 *sp;
  long  rframe,tframe;
  long t;
  short co[2][3][4];
  int i;
  ACTR *ctop;
  MYVECTOR **cpt;
  
  sp   = (GsARGANIM2 *)(&(sp0->header_size)+sp0->header_size);
  seq  = sp->base;
  
  ofs0 = ((seq->rewrite_idx)&0xff000000)>>24;
  ofs1 = (seq->rewrite_idx)&0x00ffffff;
  
  rframe = seq->rframe;
  tframe = seq->tframe;
  
  ctop = (ACTR *)sp0->ctop;	/* control top */
  cpt = (MYVECTOR **)(sp0->ptop+(ctop+seq->start)->pidx);
  
  if(seq->speed>=0) {
      if(sp->src == cpt[1]) {
	  cpt[2] = cpt[1];
	  cpt[1] = 0;
	  return 1;
      }
      if(tframe == 0 || rframe == tframe) {
	  cpt[0] = cpt[1];
	  cpt[1] = cpt[2];
	  cpt[2] = sp->src;
	  cpt[3] = sp->dst;
	  for(i=0;i<4;i++)
	      if(cpt[i] == 0)
		  return 1;
	  /*
	  printf("->%x %x %x %x %x\n",cpt[0],cpt[1],cpt[2],cpt[3]);
	  */
      }
  }
  if(seq->speed<0) {
      if(sp->src == cpt[1]) {
	  cpt[1] = cpt[2];
	  cpt[2] = 0;
	  return 1;
      }
      if(tframe == 0 || rframe == 0) {
	  cpt[3] = cpt[2];
	  cpt[2] = cpt[1];
	  cpt[0] = sp->src;
	  cpt[1] = sp->dst;
	  for(i=0;i<4;i++)
	      if(cpt[i] == 0)
		  return 1;
	  /*
	  printf("<-%x %x %x %x\n",cpt[0],cpt[1],cpt[2],cpt[3]);
	  */
      }
  }
  if(seq->tframe==0)
      return 1;
  
  t = (tframe-rframe)*0xffff/tframe;
  setBezierCof(sp->src->vx0,sp->src->vx1,sp->src->vx2,sp->dst->vx0,co[0][0]);
  setBezierCof(sp->src->vy0,sp->src->vy1,sp->src->vy2,sp->dst->vy0,co[0][1]);
  setBezierCof(sp->src->vz0,sp->src->vz1,sp->src->vz2,sp->dst->vz0,co[0][2]);
  setBetaCof(cpt[0]->rx,cpt[1]->rx,cpt[2]->rx,cpt[3]->rx,co[1][0]);
  setBetaCof(cpt[0]->ry,cpt[1]->ry,cpt[2]->ry,cpt[3]->ry,co[1][1]);
  setBetaCof(cpt[0]->rz,cpt[1]->rz,cpt[2]->rz,cpt[3]->rz,co[1][2]);
  
  dtop = (GsCOORDUNIT *)((u_long *)(*(&(sp0->header_size)+ofs0))+ofs1);
  for(i=0;i<3;i++)
      dtop->matrix.t[i] = ((((((((co[0][i][0]*t)>>16)+co[0][i][1])*t)>>16)+co[0][i][2])*t)>>16)+co[0][i][3];
  dtop->rot.vx = ((((((((co[1][0][0]*t)>>16)+co[1][0][1])*t)>>16)+co[1][0][2])
		   *t)>>16)+co[1][0][3];
  dtop->rot.vy = ((((((((co[1][1][0]*t)>>16)+co[1][1][1])*t)>>16)+co[1][1][2])
		   *t)>>16)+co[1][1][3];
  dtop->rot.vz = ((((((((co[1][2][0]*t)>>16)+co[1][2][1])*t)>>16)+co[1][2][2])
		   *t)>>16)+co[1][2][3];
  
  ROT_MATRIX(&dtop->rot,&dtop->matrix);
  dtop->flg = 0;
  
  if(sp->intr) {
      sp->intr->vx0 = dtop->matrix.t[0];
      sp->intr->vy0 = dtop->matrix.t[1];
      sp->intr->vz0 = dtop->matrix.t[2];
      sp->intr->rx  = dtop->rot.vx;
      sp->intr->ry  = dtop->rot.vy;
      sp->intr->rz  = dtop->rot.vz;
    }
  
  /*
  printf("ROT %d/%d (%d %d %d) (%d %d %D)/(%d %d %d)\n",
  rframe,tframe,dtop->rot.vx,dtop->rot.vy,dtop->rot.vz,
  sp->src->vx,sp->src->vy,sp->src->vz,
  sp->dst->vx,sp->dst->vy,sp->dst->vz);
  */
  return 0;
}
