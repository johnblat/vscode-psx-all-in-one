/* $PSLibId: Run-time Library Release 4.4$ */
/*
 * "tuto.c" - texture address modulation sample
 * 
 * Version 1.00 July 18, 1997
 * 
 * Copyright (C) 1993-97 Sony Computer Entertainment Inc.
 * All rights Reserved 
 */

#include <sys/types.h>
#include <libetc.h>		/* for Control Pad */
#include <libgte.h>		/* LIBGS uses libgte */
#include <libgpu.h>		/* LIBGS uses libgpu */
#include <libgs.h>		/* for LIBGS */
#include <libcd.h>		/* for LIBCD */
#include <inline_c.h>
#include <gtemac.h>
#include "spadstk.h"		/* for Scratch Pad Stack */

/*#define CDROM /* Define if exec from CD-ROM */

extern _GsFCALL GsFCALL4;	/* GsSortObject4J Func Table */
extern PACKET *FastG3L(TMD_P_G3*, VERT*, VERT*, PACKET*, int, int, GsOT*);
extern PACKET *FastG3L2(TMD_P_G3*, VERT*, VERT*, PACKET*, int, int, GsOT*);
extern PACKET *FastG3L2L3(TMD_P_G3*, VERT*, VERT*, PACKET*, int, int, GsOT*);

/***********************************************************/ 
/* �I�[�_�����O�e�[�u��(OT)          */
/***********************************************************/ 
/* �e�N�X�`���f�[�^�iTIM�t�H�[�}�b�g�j���������A�h���X */
#define TEX0_ADDR   0x80018000
#define TEX1_ADDR   0x80010000
#define TEX2_ADDR   0x80068000
#define TEX3_ADDR   0x80040000
#define TEX4_ADDR   0x80090000

/* ���f�����O�f�[�^�iTMD�t�H�[�}�b�g�j���������A�h���X */
#define MODEL_ADDR  0x800D0000
#define MODEL2_ADDR 0x800C8000

/***********************************************************/
/* �I�[�_�����O�e�[�u��(OT)          */
/***********************************************************/
GsOT    Wot0[2];	/* Handler of OT Uses 2 Hander for Dowble buffer */
#define OT_LENGTH  8					/* bit length of OT */
GsOT_TAG zsorttable0[2][1<<OT_LENGTH];	/* Area of OT */

GsOT    *ot2;		/* another OT */
GsOT    Wot00[2];	/* Handler of OT Uses 2 Hander for Dowble buffer */
GsOT_TAG zsorttable00[2][1<<OT_LENGTH];	/* Area of OT */

/***********************************************************/
/* 3D�I�u�W�F�N�g�ϐ�                         */
/***********************************************************/
#define OBJECTMAX 2		/* �I�u�W�F�N�g�̍ő吔���`���� */
GsDOBJ2 object[OBJECTMAX];	/* �I�u�W�F�N�g�n���h���I�u�W�F�N�g�̐������K�v */

u_long  Objnum;			/* ���f�����O�f�[�^�̃I�u�W�F�N�g�̐���ێ����� */

GsCOORDINATE2 DWorld,DWorld2;	/* Coordinate for GsDOBJ2 */
SVECTOR Rotation,Rotation2;	/* rotation vector for each object */
VECTOR Scale,Scale2;		/* scaling vector for each object */

/***********************************************************/ 
/* ���̑��ϐ�                            */
/***********************************************************/ 
GsRVIEW2 view;			/* ���_��ݒ肷�邽�߂̍\���� */
GsF_LIGHT pslt[3];		/* ���s������ݒ肷�邽�߂̍\���� */
u_long padd;			/* �R���g���[���̃f�[�^��ێ����� */
u_long padd2;

extern DRAWENV GsDRAWENV;	/* Original drawenv */

/***********************************************************/ 
/* GPU�p�P�b�g�����̈�                   */
/***********************************************************/ 
#define PACKETMAX 10000		/* Max GPU packets */
#define SIZEOFPACKETAREA (PACKETMAX*24)	/* size of PACKETMAX (byte) paket
					   size may be 24 byte(6 word) */
PACKET  out_packet[2][SIZEOFPACKETAREA];	/* GPU PACKETS AREA */


/***********************************************************/ 
/* ���߁^���܊��}�b�v�̂��߂̕ϐ��Q */
/***********************************************************/ 
GsOT	Wot1[2];		/* Handler of OT */
GsOT_TAG zsorttable1[2][1<<OT_LENGTH];	/* �I�[�_�����O�e�[�u������ */
DRAWENV drenv1;			/* Drawenv to create Env-map pattern  */
RECT	envrect1;
DR_STP  pr0, pr1;		/* To use stp flag */
int	refract;		/* ���ܗ� */

/***********************************************************/ 
/* ���ˊ��}�b�v�̂��߂̕ϐ��Q */
/***********************************************************/ 
GsOT	Wot2[2];		/* Handler of OT */
GsOT_TAG zsorttable2[2][1<<OT_LENGTH];	/* �I�[�_�����O�e�[�u������ */
DRAWENV	drenv2;			/* Drawenv to create Env-map pattern  */
RECT	envrect2;
GsRVIEW2 view2;			/* ���_��ݒ肷�邽�߂̍\���� */
int	reflect;			/* ���˗� */

/*****************  Type Definitions  ***************************/

/* struct of object */
typedef struct  {
  u_long *vert_top;
  u_long n_vert;
  u_long *normal_top;
  u_long n_normal;
  u_long *primitive_top;
  u_long n_primitive;
  long scale;
} Object;

/* flag */
int f_Back = 0;

/*****************  Global variables  ***************************/

/* (�O���` �X�O��) 64���� */
static short G_sinT[] =
{    0,   100,   200,   301,   401,   501,   601,   700,
   799,   897,   995,  1092,  1189,  1284,  1379,  1474,
  1567,  1659,  1751,  1841,  1930,  2018,  2105,  2191,
  2275,  2358,  2439,  2519,  2598,  2675,  2750,  2824,
  2896,  2966,  3034,  3101,  3166,  3229,  3289,  3348,
  3405,  3460,  3513,  3563,  3612,  3658,  3702,  3744,
  3784,  3821,  3856,  3889,  3919,  3947,  3973,  3996,
  4017,  4035,  4051,  4065,  4076,  4084,  4091,  4094
};

Object  G_obj;             /* struct of object */

/* sin
      n : (�O�x�`�R�T�X�x)
*/   
short m_sin(unsigned char n)
{
  if (n <= 63)
    return G_sinT[n];

  else if (n <= 127)
    return G_sinT[127 - n];

  else if (n <= 191)
    return -G_sinT[n-128];

  else
    return -G_sinT[255-n];
}

/*********************************************************************
	RIPPLE
 *********************************************************************/
#include "link15.c"		/* include additional data */
#define NRING 128
int SetTmdVertexY(int n, short y);

/* define wave type */
typedef struct {
	int h;				/* height */
	int aa, a1, a2;			/* amplitude */
	int ws;				/* speed */
	int a0, a;
	int top;
	short h_ring[NRING];		/* ring height buffer */
	unsigned char d_tbl[NVERT];	/* distance table */
} RIPPLE;

RIPPLE r0 = {12, 5, 256*6,  256*12, 3, 0,0,0};
RIPPLE r1 = { 8, 8, 256*12, 256*12, 3, 256*4,0,0};

/***********************************************************/ 
/* ���C�����[�`��                           */
/***********************************************************/ 
main()
{
	int     i;
	GsDOBJ2 *op;		/* �I�u�W�F�N�g�n���h���ւ̃|�C���^ */
	int     outbuf_idx;	/* double buffer index */
	MATRIX  tmpls;
	VECTOR  tmpv;

	ResetCallback();

	init_all();

	SetDrawStp(&pr0, 0);
	SetDrawStp(&pr1, 1);
	reflect = 70;
	refract = 150;

	reflect = 25;
	refract = 55;
	Scale.vx = Scale.vy = Scale.vz = 4096;
	DWorld.coord.t[0] = 0;
	DWorld.coord.t[1] = 0;
	DWorld.coord.t[2] = -2800;
	Rotation.vx = -524;
	Rotation.vy = 3056;
	Rotation.vz = 2072;

	Scale2.vx = Scale2.vy = Scale2.vz = 10096;
	DWorld2.coord.t[0] = 0;
	DWorld2.coord.t[1] = 100;
	DWorld2.coord.t[2] = -4200;
	Rotation2.vx = 3800;
	Rotation2.vy = 29704;
	Rotation2.vz = 31604;

	while (1) {

		/* �p�b�h�f�[�^���瓮���̃p�����[�^������ */
#if TUNE > 1
		SetSpadStack(SPAD_STACK_ADDR);
#endif
		if (moveObjectByPad() == 0) {
#if TUNE > 1
			ResetSpadStack();
#endif
			return 0;
		}
#if TUNE > 1
		ResetSpadStack();
#endif

		/* ���_���̍X�V */
		GsSetProjection(1000);	/* �v���W�F�N�V�����ݒ� */
		GsSetRefView2(&view);	/* ���[���h�X�N���[���}�g���b�N�X�v�Z */

		/* �_�u���o�b�t�@�̂ǂ��炩�𓾂� */
		outbuf_idx = GsGetActiveBuff();
		ot2 = &Wot00[outbuf_idx];

		/* Set top address of Packet Area for output of GPU PACKETS */
		GsSetWorkBase((PACKET *) out_packet[outbuf_idx]);

		/* �I�[�_�����O�e�[�u�����N���A���� */
		GsClearOt(0, 0, &Wot0[outbuf_idx]);
		GsClearOt(0, 0, &Wot00[outbuf_idx]);
		GsClearOt(0, (1<<OT_LENGTH)-1, &Wot1[outbuf_idx]);
		GsClearOt(0, (1<<OT_LENGTH)-1, &Wot2[outbuf_idx]);

		for (i = 0, op = object; i < OBJECTMAX; i++) {
			/* ���[���h�^���[�J���}�g���b�N�X���v�Z���� */
			GsGetLw(op->coord2, &tmpls);

			/* ���C�g�}�g���b�N�X��GTE�ɃZ�b�g���� */
			GsSetLightMatrix(&tmpls);

			/* �X�N���[���^���[�J���}�g���b�N�X���v�Z���� */
			GsGetLs(op->coord2, &tmpls);

			/* �X�N���[���^���[�J���}�g���b�N�X��GTE�ɃZ�b�g���� */
			GsSetLsMatrix(&tmpls);

			/* �I�u�W�F�N�g�𓧎��ϊ����I�[�_�����O�e�[�u���ɓo�^���� */
			switch(i) {
				case 0:
#if TUNE > 1
					SetSpadStack(SPAD_STACK_ADDR);
#endif
					GsSortObject4J(op,&Wot0[outbuf_idx],
						14-OT_LENGTH,getScratchAddr(0));
#if TUNE > 1
					ResetSpadStack();
#endif
					break;
				case 1:
				case 2:
#if TUNE > 1
					SetSpadStack(SPAD_STACK_ADDR);
#endif
					GsSortObject4(op,&Wot0[outbuf_idx],
						14-OT_LENGTH,getScratchAddr(0));
#if TUNE > 1
					ResetSpadStack();
#endif
					break;
			}
			op++;
		}

		/* ���ܗp���}�b�v�f�[�^�𐶐� */
		GsSetRefView2(&view);
		op = object+1;
		GsGetLw(op->coord2, &tmpls);
		GsSetLightMatrix(&tmpls);
		GsGetLs(op->coord2, &tmpls);
		tmpv.vx = ONE/2;
		tmpv.vy = ONE;
		tmpv.vz = ONE;
		ScaleMatrixL(&tmpls, &tmpv);
		tmpls.t[0] = tmpls.t[0]/2;
		GsSetLsMatrix(&tmpls);
#if TUNE > 1
		SetSpadStack(SPAD_STACK_ADDR);
#endif
		if(DWorld2.coord.t[2] < DWorld.coord.t[2]+750) { 
			GsSortObject4(op, &Wot1[outbuf_idx],
				14 - OT_LENGTH, getScratchAddr(0));
		}
#if TUNE > 1
		ResetSpadStack();
#endif

		/* ���˗p���}�b�v�f�[�^�𐶐� */
		view2_update();
		GsSetProjection(50);
		GsSetRefView2(&view2);
		op = object+1;
		GsGetLw(op->coord2, &tmpls);
		GsSetLightMatrix(&tmpls);
		GsGetLs(op->coord2, &tmpls);
		GsSetLsMatrix(&tmpls);
#if TUNE > 1
		SetSpadStack(SPAD_STACK_ADDR);
#endif
		GsSortObject4(op, &Wot2[outbuf_idx], 14-OT_LENGTH, getScratchAddr(0));
#if TUNE > 1
		ResetSpadStack();
#endif

		AddPrim(Wot2[outbuf_idx].tag, &pr1);

		/* V-BLNK ������̂�҂� */
		VSync(0);
		/* GPU�̕`��I����҂� */
		DrawSync(0);

		/* �p�b�h�̃f�[�^��ǂݍ��� */
		padd = PadRead(1);
		padd2 = padd>>16;

		/* �_�u���o�b�t�@��ؑւ��� */
		GsSwapDispBuff();

		AddPrim(Wot0[outbuf_idx].tag, &pr0);

		/* ��ʂ̃N���A���I�[�_�����O�e�[�u���̍ŏ��ɓo�^���� */
		GsSortClear(0x0, 0x0, 0x0, &Wot0[outbuf_idx]);

		/* ���}�b�v�C���[�W�f�[�^��]������ */
		switch(f_Back) {
			case 0:
				LoadImage(&envrect1, (u_long *)(TEX2_ADDR+12));
				break;
			case 1:
				LoadImage(&envrect1, (u_long *)(TEX4_ADDR+12));
				break;
		}
		LoadImage(&envrect2, (u_long *)(TEX0_ADDR+12));

		/* �I�[�_�����O�e�[�u���ɓo�^����Ă���p�P�b�g�̕`����J�n���� */
		PutDrawEnv(&drenv1);
		GsDrawOt(&Wot1[outbuf_idx]);
		PutDrawEnv(&drenv2);
		GsDrawOt(&Wot2[outbuf_idx]);
		GsSetDrawBuffClip();
		GsDrawOt(&Wot0[outbuf_idx]);
		GsDrawOt(&Wot00[outbuf_idx]);

#if TUNE > 1
		SetSpadStack(SPAD_STACK_ADDR);
#endif
		ripple();
		g3_normal();
#if TUNE > 1
		ResetSpadStack();
#endif
	}
}

/***********************************************************/
/* �I�u�W�F�N�g�𓮂���                     */
/***********************************************************/
moveObjectByPad()
{
	SVECTOR v1;
	MATRIX  tmp1;
	static GsCOORDINATE2 *pDWorld = &DWorld2;
	static VECTOR *pScale = &Scale2;
	static int f_OnStart = 0;
	static int f_Obj = 0;
	static int f_OnSelect = 0;
	static int f_Render = 0;

	/* �I�u�W�F�N�g��X����]������ */
	if ((padd & PADRup) > 0)
		Rotation.vx += 5 * ONE / 360;
	if ((padd & PADRdown) > 0)
		Rotation.vx -= 5 * ONE / 360;

#if 0
	/* �I�u�W�F�N�g��Y����]������ */
	if ((padd & PADRleft) > 0) {
		Rotation.vz -= 5 * ONE / 360;
		Rotation.vy += 5 * ONE / 360;
	}
	if ((padd & PADRright) > 0) {
		Rotation.vz += 5 * ONE / 360;
		Rotation.vy -= 5 * ONE / 360;
	}
#else
	/* �I�u�W�F�N�g��Y����]������ */
	if ((padd & PADRleft) > 0)
		Rotation.vy -= 5 * ONE / 360;
	if ((padd & PADRright) > 0)
		Rotation.vy += 5 * ONE / 360;
#endif

	/* �I�u�W�F�N�g��Z���ɂ����ē����� */
	if ((padd & PADm) > 0)
		DWorld.coord.t[2] -= 200;
	if ((padd & PADl) > 0)
		DWorld.coord.t[2] += 200;

	/* �I�u�W�F�N�g��X���ɂ����ē����� */
	if ((padd & PADLleft) > 0)
		DWorld.coord.t[0] += 100;
	if ((padd & PADLright) > 0)
		DWorld.coord.t[0] -= 100;

	/* �I�u�W�F�N�g��Y���ɂ����ē����� */
	if ((padd & PADLdown) > 0)
		DWorld.coord.t[1] += 100;
	if ((padd & PADLup) > 0)
		DWorld.coord.t[1] -= 100;

	/* �I�u�W�F�N�g�Q��X���ɂ����ē����� */
	if ((padd2 & PADLleft) > 0)
		pDWorld->coord.t[0] += 100;
	if ((padd2 & PADLright) > 0)
		pDWorld->coord.t[0] -= 100;

	/* �I�u�W�F�N�g�Q��Y���ɂ����ē����� */
	if ((padd2 & PADLdown) > 0)
		pDWorld->coord.t[1] += 100;
	if ((padd2 & PADLup) > 0)
		pDWorld->coord.t[1] -= 100;

	/* �I�u�W�F�N�g�Q��Z���ɂ����ē����� */
	if ((padd2 & PADm) > 0)
		pDWorld->coord.t[2] -= 200;
	if ((padd2 & PADl) > 0)
		pDWorld->coord.t[2] += 200;

	/* �I�u�W�F�N�g�Q���g��^�k�������� */
	if ((padd2 & PADRup) > 0) {
		pScale->vx += 100;
		pScale->vy += 100;
		pScale->vz += 100;
	}
	if ((padd2 & PADRdown) > 0) {
		pScale->vx -= 100;
		pScale->vy -= 100;
		pScale->vz -= 100;
	}

	if ((padd2 & PADRleft) > 0) {
		r0.h = 0;
		r1.h = 0;
	}
	if ((padd2 & PADRright) > 0) {
		r0.h = 12;
		r1.h = 8;
	}

	if ((padd2 & PADstart) > 0) {
		if(!f_OnStart) {
			f_Back ^= 1;
		}
		f_OnStart = 1;
	} else {
		f_OnStart = 0;
	}

	if ((padd2 & PADselect) > 0) {
		if(!f_OnSelect) {
			f_Render ^= 1;
			if(f_Render) {
				GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = FastG3L2L3;
			} else {
#if TUNE == 3
				GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = FastG3L2;
#else
				GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = FastG3L;
#endif
			}
		}
		f_OnSelect = 1;
	} else {
		f_OnSelect = 0;
	}

	/* �I�u�W�F�N�g�Q����]���� */
	Rotation2.vx -= 2 * ONE / 360;
	Rotation2.vy -= 3 * ONE / 360;
	Rotation2.vz -= 4 * ONE / 360;

	/* ���ܗ��̕ω� */
	if ((padd & PADL1) > 0)
		if(refract < 1023) refract += 5;
	if ((padd & PADL2) > 0)
		if(refract > 0) refract -= 5;

	/* ���˗��̕ω� */
	if ((padd2 & PADL1) > 0)
		if(reflect < 255) reflect += 5;
	if ((padd2 & PADL2) > 0)
		if(reflect > 0) reflect -= 5;

#if 0
	if ((padd & PADstart) > 0) {
		printf("reflect:%d refract:%d\n", reflect, refract);
		printf("OBJ1(%d,%d,%d)(%d,%d,%d)(%d,%d,%d)\n", 
				Scale.vx, Scale.vy, Scale.vz, 
				DWorld.coord.t[0], DWorld.coord.t[1], DWorld.coord.t[2],
				Rotation.vx, Rotation.vy, Rotation.vz);
		printf("OBJ2(%d,%d,%d)(%d,%d,%d)(%d,%d,%d)\n", 
				Scale2.vx, Scale2.vy, Scale2.vz, 
				DWorld2.coord.t[0], DWorld2.coord.t[1], DWorld2.coord.t[2],
				Rotation2.vx, Rotation2.vy, Rotation2.vz);
	}
#endif

	/* �v���O�������I�����ă��j�^�ɖ߂� */
	if ((padd & PADk) > 0) {
		PadStop();
		ResetGraph(3);
		StopCallback();
		return 0;
	}
	/* �I�u�W�F�N�g�̃p�����[�^����}�g���b�N�X���v�Z�����W�n�ɃZ�b�g */
	set_coordinate(&Rotation, &Scale, &DWorld);
	set_coordinate(&Rotation2, &Scale2, &DWorld2);

	return 1;
}

/***********************************************************/ 
/* ���������[�`���Q                  */
/***********************************************************/ 
init_all()
{
	GsFOGPARAM fgp;

#ifdef CDROM
	SsInit();
	CdInit();
#endif CDROM

	ResetGraph(0);		/* GPU���Z�b�g */

	/* �R���g���[���̃��Z�b�g */
	PadInit(0);

	padd = 0;		/* �R���g���[���l������ */

	GsInitGraph(640, 240, GsOFSGPU, 1, 0);
	GsDefDispBuff(0, 0, 0, 240);

	GsInit3D();		/* �RD�V�X�e�������� */

	Wot0[0].length = OT_LENGTH;	/* Set bit length of OT handler */
	Wot0[0].org = zsorttable0[0];	/* Set Top address of OT */
	Wot0[1].length = OT_LENGTH;
	Wot0[1].org = zsorttable0[1];

	Wot00[0].length = OT_LENGTH;	/* Set bit length of OT handler */
	Wot00[0].org = zsorttable00[0];	/* Set Top address of OT */
	Wot00[1].length = OT_LENGTH;
	Wot00[1].org = zsorttable00[1];

	Wot1[0].length = OT_LENGTH;
	Wot1[0].org = zsorttable1[0];
	Wot1[1].length = OT_LENGTH;
	Wot1[1].org = zsorttable1[1];

	Wot2[0].length = OT_LENGTH;
	Wot2[0].org = zsorttable2[0];
	Wot2[1].length = OT_LENGTH;
	Wot2[1].org = zsorttable2[1];

	coord_init();		/* ���W��` */

#ifdef CDROM
	/* Read data file from CD-ROM */
	readFile("\\VENUS3G.TMD;1", MODEL_ADDR);
	readFile("\\RCUBE.TMD;1", MODEL2_ADDR);
	readFile("\\ENV.TIM;1", TEX0_ADDR);
	readFile("\\BACK1.TIM;1", TEX1_ADDR);
	readFile("\\BACK2.TIM;1", TEX2_ADDR);
	CdControl(CdlStop, (void *)0, 0);
#endif CDROM

	model_init();		/* ���f�����O�f�[�^�ǂݍ��� */
	view_init();		/* ���_�ݒ� */
	light_init();		/* ���s�����ݒ� */

	texture_init(TEX0_ADDR); /* texture load of TEX_ADDR */
	texture_init(TEX1_ADDR); /* texture load of TEX_ADDR */
	texture_init(TEX2_ADDR); /* texture load of TEX_ADDR */

	ripple_init();

	/* setting FOG parameters */
	fgp.dqa = -10000 * ONE / 64 / 1000;
	fgp.dqb = 5 / 4 * ONE * ONE;
	fgp.rfc = fgp.gfc = fgp.bfc = 0;
	GsSetFogParam(&fgp);

	/* ���}�b�v�ϐ��̏����� */
	env1_init();
	env2_init();

	/* setting jumptable for GsSortObject4J() */
	jt_init4();
}

/* ���_�ݒ� ---*/
view_init()
{
	/* ���_�p�����[�^�ݒ� */
	view.vpx = 0;
	view.vpy = 0;
	view.vpz = 2000;

	/* �����_�p�����[�^�ݒ� */
	view.vrx = 0;
	view.vry = 0;
	view.vrz = 0;

	/* ���_�̔P��p�����[�^�ݒ� */
	view.rz = 0;

	/* ���_���W�p�����[�^�ݒ� */
	view.super = WORLD;
}

/* ���_�ݒ� ---*/
view2_update()
{
	/* ���_�p�����[�^�ݒ� */
	view2.vpx = DWorld.coord.t[0];
	view2.vpy = DWorld.coord.t[1];
	view2.vpz = DWorld.coord.t[2];

	/* �����_�p�����[�^�ݒ� */
	view2.vrx = 0;
	view2.vry = 0;
	view2.vrz = 10000;

	/* ���_�̔P��p�����[�^�ݒ� */
	view2.rz = 0;

	/* ���_���W�p�����[�^�ݒ� */
	view2.super = WORLD;
}

/* �����ݒ� ---*/
light_init()
{
	/* ���C�gID�O �ݒ� */
	/* ���s���������p�����[�^�ݒ� */
	pslt[0].vx = 0;
	pslt[0].vy = 100;
	pslt[0].vz = -100;

	/* ���s�����F�p�����[�^�ݒ� */
	pslt[0].r = 0xf0;
	pslt[0].g = 0xf0;
	pslt[0].b = 0xf0;

	/* �����p�����[�^��������ݒ� */
	GsSetFlatLight(0, &pslt[0]);


	/* ���C�gID�P �ݒ� */
	pslt[1].vx = 0;
	pslt[1].vy = 100;
	pslt[1].vz = 100;
	pslt[1].r = 0xf0;
	pslt[1].g = 0xf0;
	pslt[1].b = 0xf0;
	GsSetFlatLight(1, &pslt[1]);

	/* ���C�gID�Q �ݒ� */
	pslt[2].vx = -30;
	pslt[2].vy = 0;
	pslt[2].vz = -100;
	pslt[2].r = 0;
	pslt[2].g = 0;
	pslt[2].b = 0;
	GsSetFlatLight(2, &pslt[2]);

	/* �A���r�G���g�ݒ� */
	GsSetAmbient(1800, 1800, 1800);

	/* �����v�Z�̃f�t�H���g�̕����ݒ� */
	GsSetLightMode(0);
}

/* �e�I�u�W�F�N�g�̍��W�n�����l�ݒ� ---*/
coord_init()
{
	/* ���W�̒�` */
	GsInitCoordinate2(WORLD, &DWorld);
	GsInitCoordinate2(WORLD, &DWorld2);

	/* �}�g���b�N�X�v�Z���[�N�̃��[�e�[�V�����x�N�^�[������ */
	Rotation.vx = -ONE/4; Rotation.vy = ONE/2; Rotation.vz = 0;
	Rotation2.vx = Rotation2.vy = Rotation2.vz = 0;

	/* �e�I�u�W�F�N�g�̏����ʒu��ݒ� */
	DWorld.coord.t[2] = -12000;
	DWorld2.coord.t[0] = -2000;
	DWorld2.coord.t[2] = -8000;

	Scale.vx = Scale.vy = Scale.vz = ONE;
	Scale2.vx = Scale2.vy = Scale2.vz = ONE;
}

/* ���[�e�V�����x�N�^����}�g���b�N�X���쐬�����W�n�ɃZ�b�g���� */
set_coordinate(rot, scale, coor)
	SVECTOR *rot;		/* ���[�e�V�����x�N�^ */
	VECTOR *scale;		/* ���[�e�V�����x�N�^ */
	GsCOORDINATE2 *coor;	/* ���W�n */
{
	MATRIX  tmp1;

	/* ���s�ړ����Z�b�g���� */
	tmp1.t[0] = coor->coord.t[0];
	tmp1.t[1] = coor->coord.t[1];
	tmp1.t[2] = coor->coord.t[2];

	/* �}�g���b�N�X�Ƀ��[�e�[�V�����x�N�^����p������ */
	RotMatrix(rot, &tmp1);
	ScaleMatrix(&tmp1, scale);

	/* ���߂��}�g���b�N�X�����W�n�ɃZ�b�g���� */
	coor->coord = tmp1;

	/* �}�g���b�N�X�L���b�V�����t���b�V������ */
	coor->flg = 0;
}


/* �e�N�X�`���f�[�^��VRAM�Ƀ��[�h���� ---*/
texture_init(addr)
	u_long  addr;
{
	RECT    rect1;
	GsIMAGE tim1;

	/* TIM�f�[�^�̃w�b�_����e�N�X�`���̃f�[�^�^�C�v�̏��𓾂� */
	GsGetTimInfo((u_long *) (addr + 4), &tim1);

	rect1.x = tim1.px;	/* �e�N�X�`�������VRAM�ł�X���W */
	rect1.y = tim1.py;	/* �e�N�X�`�������VRAM�ł�Y���W */
	rect1.w = tim1.pw;	/* �e�N�X�`���� */
	rect1.h = tim1.ph;	/* �e�N�X�`������ */

	/* VRAM�Ƀe�N�X�`�������[�h���� */
	LoadImage(&rect1, tim1.pixel);

	/* �J���[���b�N�A�b�v�e�[�u�������݂��� */
	if ((tim1.pmode >> 3) & 0x01) {
		rect1.x = tim1.cx;	/* �N���b�g�����VRAM�ł�X���W */
		rect1.y = tim1.cy;	/* �N���b�g�����VRAM�ł�Y���W */
		rect1.w = tim1.cw;	/* �N���b�g�̕� */
		rect1.h = tim1.ch;	/* �N���b�g�̍��� */

		/* VRAM�ɃN���b�g�����[�h���� */
		LoadImage(&rect1, tim1.clut);
	}
}


/* ���f�����O�f�[�^�̓ǂݍ��� ---*/
model_init()
{
	u_long *dop;
	GsDOBJ2 *objp;		/* ���f�����O�f�[�^�n���h�� */
	int     i;

	dop = (u_long *) MODEL_ADDR;/* ���f�����O�f�[�^���i�[����Ă���A�h���X */

	dop++;			/* hedder skip */

	GsMapModelingData(dop);	/* ���f�����O�f�[�^�iTMD�t�H�[�}�b�g�j�����A�h���X�Ƀ}�b�v���� */

	dop++;
	Objnum = *dop;		/* �I�u�W�F�N�g����TMD�̃w�b�_���瓾�� */

	dop++;			/* GsLinkObject4J�Ń����N���邽�߂�TMD�̃I�u�W�F�N�g�̐擪�ɂ����Ă��� */

	G_obj = *((Object*)dop); /**/

	/* TMD�f�[�^�ƃI�u�W�F�N�g�n���h����ڑ����� */

	GsLinkObject4((u_long) dop, &object[0], 0);

	objp = object;

	/* �f�t�H���g�̃I�u�W�F�N�g�̍��W�n�̐ݒ� */
	objp->coord2 = &DWorld;

	/* material attenuation setting */
	objp->attribute =  GsLLMOD | GsMATE | GsLDIM4;


	/********************************/
	/*	for second TMD		*/
	/********************************/

	dop = (u_long *) MODEL2_ADDR;/* ���f�����O�f�[�^���i�[����Ă���A�h���X */

	dop++;			/* hedder skip */

	GsMapModelingData(dop);	/* ���f�����O�f�[�^�iTMD�t�H�[�}�b�g�j�����A�h���X�Ƀ}�b�v���� */

	dop++;
	Objnum = *dop;		/* �I�u�W�F�N�g����TMD�̃w�b�_���瓾�� */

	dop++;			/* GsLinkObject4J�Ń����N���邽�߂�TMD�̃I�u�W�F�N�g�̐擪�ɂ����Ă��� */

	/* TMD�f�[�^�ƃI�u�W�F�N�g�n���h����ڑ����� */

	GsLinkObject4((u_long) dop, &object[1], 0);

	objp = object+1;

	/* �f�t�H���g�̃I�u�W�F�N�g�̍��W�n�̐ݒ� */
	objp->coord2 = &DWorld2;

	/* material attenuation setting */
	objp->attribute =  0;
}

env1_init()
{
	drenv1 = GsDRAWENV;
	drenv1.clip.x = 768;
	drenv1.clip.y = 256;
	drenv1.clip.w = 256;
	drenv1.clip.h = 256;
	drenv1.ofs[0] = 768+128;
	drenv1.ofs[1] = 256+128;

	envrect1.x = 768;
	envrect1.y = 256;
	envrect1.w = 256;
	envrect1.h = 256;
}

env2_init()
{
	drenv2 = GsDRAWENV;
	drenv2.clip.x = 768;
	drenv2.clip.y = 0;
	drenv2.clip.w = 256;
	drenv2.clip.h = 256;
	drenv2.ofs[0] = 768+128;
	drenv2.ofs[1] = 0+128;

	envrect2.x = 768;
	envrect2.y = 0;
	envrect2.w = 256;
	envrect2.h = 256;
}

/***********************************************************/
/* GsSortObj4J()�̃W�����v�e�[�u���̏��������[�`��         */
/***********************************************************/
jt_init4()
{
	/* Gs SortObject4J Fook Func (for material attenuation) */
	PACKET *GsTMDfastF3NL(), *GsTMDfastF3MFG(), *GsTMDfastM3L(), *GsTMDfastNF3();
	PACKET *GsTMDdivF3NL(), *GsTMDdivF3LFG(), *GsTMDdivF3L(), *GsTMDdivNF3();
	PACKET *GsTMDfastG3NL(), *GsTMDfastG3MFG(), *GsTMDfastG3M(), *GsTMDfastNG3();
	PACKET *GsTMDdivG3NL(), *GsTMDdivG3LFG(), *GsTMDdivG3L(), *GsTMDdivNG3();
	PACKET *GsTMDfastTF3NL(), *GsTMDfastTF3MFG(), *GsTMDfastTF3M(), *GsTMDfastTNF3();
	PACKET *GsTMDdivTF3NL(), *GsTMDdivTF3LFG(), *GsTMDdivTF3L(), *GsTMDdivTNF3();
	PACKET *GsTMDfastTG3NL(), *GsTMDfastTG3MFG(), *GsTMDfastTG3M(), *GsTMDfastTNG3();
	PACKET *GsTMDdivTG3NL(), *GsTMDdivTG3LFG(), *GsTMDdivTG3L(), *GsTMDdivTNG3();
	PACKET *GsTMDfastF4NL(), *GsTMDfastF4MFG(), *GsTMDfastF4M(), *GsTMDfastNF4();
	PACKET *GsTMDdivF4NL(), *GsTMDdivF4LFG(), *GsTMDdivF4L(), *GsTMDdivNF4();
	PACKET *GsTMDfastG4NL(), *GsTMDfastG4MFG(), *GsTMDfastG4M(), *GsTMDfastNG4();
	PACKET *GsTMDdivG4NL(), *GsTMDdivG4LFG(), *GsTMDdivG4L(), *GsTMDdivNG4();
	PACKET *GsTMDfastTF4NL(), *GsTMDfastTF4MFG(), *GsTMDfastTF4M(), *GsTMDfastTNF4();
	PACKET *GsTMDdivTF4NL(), *GsTMDdivTF4LFG(), *GsTMDdivTF4L(), *GsTMDdivTNF4();
	PACKET *GsTMDfastTG4NL(), *GsTMDfastTG4MFG(), *GsTMDfastTG4M(), *GsTMDfastTNG4();
	PACKET *GsTMDdivTG4NL(), *GsTMDdivTG4LFG(), *GsTMDdivTG4L(), *GsTMDdivTNG4();
	PACKET *GsTMDfastF3GNL(), *GsTMDfastF3GLFG(), *GsTMDfastF3GL();
	PACKET *GsTMDfastG3GNL(), *GsTMDfastG3GLFG(), *GsTMDfastG3GL();

	/* flat triangle */
	GsFCALL4.f3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastF3M;
	GsFCALL4.f3[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastF3MFG;
	GsFCALL4.f3[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastF3NL;
	GsFCALL4.f3[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivF3L;
	GsFCALL4.f3[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivF3LFG;
	GsFCALL4.f3[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivF3NL;
	GsFCALL4.nf3[GsDivMODE_NDIV] = GsTMDfastNF3;
	GsFCALL4.nf3[GsDivMODE_DIV] = GsTMDdivNF3;

	/* gour triangle */
#if TUNE == 3
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = FastG3L2;
#else
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = FastG3L;
#endif
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastG3MFG;
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastG3NL;
	GsFCALL4.g3[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivG3L;
	GsFCALL4.g3[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivG3LFG;
	GsFCALL4.g3[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivG3NL;
	GsFCALL4.ng3[GsDivMODE_NDIV] = GsTMDfastNG3;
	GsFCALL4.ng3[GsDivMODE_DIV] = GsTMDdivNG3;
	/* texture flat triangle */
	GsFCALL4.tf3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTF3M;
	GsFCALL4.tf3[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastTF3MFG;
	GsFCALL4.tf3[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastTF3NL;
	GsFCALL4.tf3[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivTF3L;
	GsFCALL4.tf3[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivTF3LFG;
	GsFCALL4.tf3[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivTF3NL;
	GsFCALL4.ntf3[GsDivMODE_NDIV] = GsTMDfastTNF3;
	GsFCALL4.ntf3[GsDivMODE_DIV] = GsTMDdivTNF3;
	/* texture gour triangle */
	GsFCALL4.tg3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTG3M;
	GsFCALL4.tg3[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastTG3MFG;
	GsFCALL4.tg3[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastTG3NL;
	GsFCALL4.tg3[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivTG3L;
	GsFCALL4.tg3[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivTG3LFG;
	GsFCALL4.tg3[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivTG3NL;
	GsFCALL4.ntg3[GsDivMODE_NDIV] = GsTMDfastTNG3;
	GsFCALL4.ntg3[GsDivMODE_DIV] = GsTMDdivTNG3;
	/* flat quad */
	GsFCALL4.f4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastF4M;
	GsFCALL4.f4[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastF4MFG;
	GsFCALL4.f4[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastF4NL;
	GsFCALL4.f4[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivF4L;
	GsFCALL4.f4[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivF4LFG;
	GsFCALL4.f4[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivF4NL;
	GsFCALL4.nf4[GsDivMODE_NDIV] = GsTMDfastNF4;
	GsFCALL4.nf4[GsDivMODE_DIV] = GsTMDdivNF4;
	/* gour quad */
	GsFCALL4.g4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastG4M;
	GsFCALL4.g4[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastG4MFG;
	GsFCALL4.g4[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastG4NL;
	GsFCALL4.g4[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivG4L;
	GsFCALL4.g4[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivG4LFG;
	GsFCALL4.g4[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivG4NL;
	GsFCALL4.ng4[GsDivMODE_NDIV] = GsTMDfastNG4;
	GsFCALL4.ng4[GsDivMODE_DIV] = GsTMDdivNG4;
	/* texture flat quad */
	GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTF4M;
	GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastTF4MFG;
	GsFCALL4.tf4[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastTF4NL;
	GsFCALL4.tf4[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivTF4L;
	GsFCALL4.tf4[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivTF4LFG;
	GsFCALL4.tf4[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivTF4NL;
	GsFCALL4.ntf4[GsDivMODE_NDIV] = GsTMDfastTNF4;
	GsFCALL4.ntf4[GsDivMODE_DIV] = GsTMDdivTNF4;
	/* texture gour quad */
	GsFCALL4.tg4[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastTG4M;
	GsFCALL4.tg4[GsDivMODE_NDIV][GsLMODE_FOG] = GsTMDfastTG4MFG;
	GsFCALL4.tg4[GsDivMODE_NDIV][GsLMODE_LOFF] = GsTMDfastTG4NL;
	GsFCALL4.tg4[GsDivMODE_DIV][GsLMODE_NORMAL] = GsTMDdivTG4L;
	GsFCALL4.tg4[GsDivMODE_DIV][GsLMODE_FOG] = GsTMDdivTG4LFG;
	GsFCALL4.tg4[GsDivMODE_DIV][GsLMODE_LOFF] = GsTMDdivTG4NL;
	GsFCALL4.ntg4[GsDivMODE_NDIV] = GsTMDfastTNG4;
	GsFCALL4.ntg4[GsDivMODE_DIV] = GsTMDdivTNG4;
	/* gradation  triangle */
	GsFCALL4.f3g[GsLMODE_NORMAL] = GsTMDfastF3GL;
	GsFCALL4.f3g[GsLMODE_FOG] = GsTMDfastF3GLFG;
	GsFCALL4.f3g[GsLMODE_LOFF] = GsTMDfastF3GNL;
	GsFCALL4.g3g[GsLMODE_NORMAL] = GsTMDfastG3GL;
	GsFCALL4.g3g[GsLMODE_FOG] = GsTMDfastG3GLFG;
	GsFCALL4.g3g[GsLMODE_LOFF] = GsTMDfastG3GNL;
}

/*
 *      read data file from CD-ROM
 *
 */
readFile(char *fname, u_long addr)
{
	CdlFILE finfo;
	int r;
	int nsector;

	/* Search file on the CD-ROM */
	r = (int)CdSearchFile(&finfo, fname);

	/* Read data */
	nsector = (finfo.size+2047)/2048;
	CdControl(CdlSetloc, (void *)&(finfo.pos), 0);
	CdRead(nsector, (void *)addr, CdlModeSpeed);
	while(CdReadSync(1,0) > 0) {
		VSync(0);
	}
}

/*********************************************************************
	RIPPLE
 *********************************************************************/

ripple_init()
{
	int i;
	long r;
	SVECTOR v;
	long max;

	/* initialize height buffer */
	for(i = 0; i < NRING; i++) {
		r0.h_ring[i] = 0;
		r1.h_ring[i] = 0;
	}

	/* initialize distance table */
#define R  1024
#define R2 2048

	max = (long)((float)R*1.41421356)+1;
	for(i=0; i< G_obj.n_vert; i++) {
		GetTmdVertex(i, &v);
		r = v.vx*v.vx+v.vz*v.vz;
		r = SquareRoot0(r);
		r = (unsigned char)((float)r/max*NRING);
		r0.d_tbl[i] = r;
	}

	max = (long)((float)R2*1.41421356)+1;
	for(i=0; i< G_obj.n_vert; i++) {
		GetTmdVertex(i, &v);
		r = (v.vx+R)*(v.vx+R)+(v.vz+R)*(v.vz+R);
		r = SquareRoot0(r);
		r = (unsigned char)((float)r/max*NRING);
		r1.d_tbl[i] = r;
	}
}

ripple()
{
	RIPPLE *r;
	int j0, j1, j2;
	int i;
	int n;
	short z;

	r = &r0;
	for(i=0; i< r->ws; i++) {
		n = r->top - i;
		if(n < 0) n += NRING;
		r->h_ring[n] = (short)((float)(m_sin(r->a)/(32*16))*r->h);
		r->a0 += r->aa;
		if(r->a0 < r->a1) {		r->a = r->a0%256;
		} else if (r->a0 < r->a2) {	r->a = 0;
		} else {			r->a0 = r->a = 0;
		}
	}
	r->top -= r->ws; if(r->top < 0) r->top += NRING;

	r = &r1;
	for(i=0; i< r->ws; i++) {
		n = r->top - i;
		if(n < 0) n += NRING;
		r->h_ring[n] = (short)((float)(m_sin(r->a)/(32*16))*r->h);
		r->a0 += r->aa;
		if(r->a0 < r->a1) {		r->a = r->a0%256;
		} else if (r->a0 < r->a2) {	r->a = 0;
		} else {			r->a0 = r->a = 0;
		}
	}
	r->top -= r->ws; if(r->top < 0) r->top += NRING;

	for(i=0; i< G_obj.n_vert; i++) {
		j0 = r0.d_tbl[i] + r0.top; if(j0 >= NRING) j0 -= NRING;
		j1 = r1.d_tbl[i] + r1.top; if(j1 >= NRING) j1 -= NRING;
		z = r0.h_ring[j0] + r1.h_ring[j1];
		SetTmdVertexY(i, z);
	}
}

/**********************************************************************/

g3_normal()
{
	int i, j, k;
	SVECTOR vn;
	SVECTOR vn2;

	/* calculate normals of polygons */
	for(i=0;  i< G_obj.n_primitive; i++) {
		CalcTmdNormal(i, &Normal[i]);
	}

	/* calculate normals of vertices */
	for(i=0;  i< G_obj.n_normal; i++) {
		vn.vx = 0;
		vn.vy = 0;
		vn.vz = 0;
		for(j = 1; j < 7; j++) {
			k = G_link[i][j];
			if(k) {
				vn.vx += Normal[k-1].vx;
				vn.vy += Normal[k-1].vy;
				vn.vz += Normal[k-1].vz;
			}
		}
		VectorNormalSS(&vn, &vn2);
		SetTmdNormal(i, (SVECTOR *)&vn2);
	}
}

#define SHIFT 4
CalcTmdNormal(int n, SVECTOR *vn1)
{
	int n0, n1, n2;
	int nn;
	int nn0, nn1, nn2;

	SVECTOR v0, v1, v2;
	VECTOR a0, a1, vn0;

#if 0
	/* check PolyG3 */
	if( (long)(*(G_obj.primitive_top + n*5)) !=  0x30000406) {
		vn1->vx = 0;
		vn1->vy = 4096;
		vn1->vz = 0;
		return 0;
	}
#endif

	n0 = (0xffff0000 & (long)(*(G_obj.primitive_top + n*5+2))) >> 16;
	n1 = (0xffff0000 & (long)(*(G_obj.primitive_top + n*5+3))) >> 16;
	n2 = (0xffff0000 & (long)(*(G_obj.primitive_top + n*5+4))) >> 16;

	GetTmdVertex(n0, &v0);
	GetTmdVertex(n1, &v1);
	GetTmdVertex(n2, &v2);

	a0.vx = (v1.vx - v0.vx) >> SHIFT;
	a0.vy = (v1.vy - v0.vy) >> SHIFT;
	a0.vz = (v1.vz - v0.vz) >> SHIFT;
	a1.vx = (v2.vx - v0.vx) >> SHIFT;
	a1.vy = (v2.vy - v0.vy) >> SHIFT;
	a1.vz = (v2.vz - v0.vz) >> SHIFT;

#if TUNE > 1
	gte_OuterProduct0(&a0, &a1, &vn0);
#else
	OuterProduct0(&a0, &a1, &vn0);
#endif
	VectorNormalS(&vn0, vn1);

	vn1->vx = - vn1->vx;
	vn1->vy = - vn1->vy;
	vn1->vz = - vn1->vz;
}

SetTmdVertexY(int n, short y)
{
	*((short *)(G_obj.vert_top + n*2)+1) = y;
}

GetTmdVertex(int n, SVECTOR *v)
{
	*v = *(SVECTOR *)(G_obj.vert_top + n*2);
}

SetTmdNormal(int n, SVECTOR *v)
{
	*(SVECTOR *)(G_obj.normal_top + n*2) = *v;
}
