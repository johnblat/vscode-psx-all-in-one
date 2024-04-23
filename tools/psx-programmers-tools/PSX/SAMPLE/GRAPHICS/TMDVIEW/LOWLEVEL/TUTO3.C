/* $PSLibId: Run-time Library Release 4.4$ */
/*
 * low_level: GsDOBJ4 object viewing rotine with low_level access 
 *
 * "tuto3.c" ******** simple GsDOBJ4 Viewing routine using GsTMDfast only for
 * gouraud polygon model (mipmap version)
 *
 * Copyright (C) 1996 by Sony Computer Entertainment All rights Reserved 
 */

#include <sys/types.h>

#include <libetc.h>		/* PAD���g�����߂ɃC���N���[�h����K�v���� */
#include <libgte.h>		/* LIGS���g�����߂ɃC���N���[�h����K�v���� */
#include <libgpu.h>		/* LIGS���g�����߂ɃC���N���[�h����K�v���� */
#include <libgs.h>		/* �O���t�B�b�N���C�u���� ���g�����߂�
				  �\���̂Ȃǂ���`����Ă��� */

#define PACKETMAX 1000		/* Max GPU packets */

#define OBJECTMAX 100		/* �RD�̃��f���͘_���I�ȃI�u�W�F�N�g�� �������邱�̍ő吔���`���� */

#define PACKETMAX2 (PACKETMAX*24)	/* size of PACKETMAX (byte) 
										packet size may be 24 byte(6 word) */

#define TEX_ADDR   0x80120000	/* �e�N�X�`���f�[�^�iTIM�t�H�[�}�b�g�j���������A�h���X */

#define TEX_ADDR1   0x80140000	/* Top Address of texture data1 (TIM FORMAT) */
#define TEX_ADDR2   0x80160000	/* Top Address of texture data1 (TIM FORMAT) */


#define MODEL_ADDR 0x80100000	/* ���f�����O�f�[�^�iTMD�t�H�[�}�b�g�j���������A�h���X */

#define OT_LENGTH  7		/* �I�[�_�����O�e�[�u���̉𑜓x */


GsOT    Wot[2];			/* �I�[�_�����O�e�[�u���n���h���_�u���o�b�t�@�̂��߂Q�K�v */

GsOT_TAG zsorttable[2][1 << OT_LENGTH];	/* �I�[�_�����O�e�[�u������ */

GsDOBJ2 object[OBJECTMAX];	/* �I�u�W�F�N�g�n���h��. �I�u�W�F�N�g�̐������K�v */

u_long  Objnum;			/* ���f�����O�f�[�^�̃I�u�W�F�N�g�̐���ێ����� */


GsCOORDINATE2 DWorld;		/* �I�u�W�F�N�g���Ƃ̍��W�n */

SVECTOR PWorld;			/* ���W�n����邽�߂̃��[�e�[�V�����x�N�^�[ */

extern MATRIX GsIDMATRIX;	/* �P�ʍs�� */

GsRVIEW2 view;			/* ���_��ݒ肷�邽�߂̍\���� */
GsF_LIGHT pslt[3];		/* ���s������ݒ肷�邽�߂̍\���� */
u_long  padd;			/* �R���g���[���̃f�[�^��ێ����� */

PACKET  out_packet[2][PACKETMAX2];	/* GPU PACKETS AREA */

/************* MAIN START ******************************************/
main()
{
	int     i;
	GsDOBJ2 *op;		/* �I�u�W�F�N�g�n���h���ւ̃|�C���^ */
	int     outbuf_idx;	/* double buffer index */
	MATRIX  tmpls, tmplw;
	int     vcount;
	int     p;

	ResetCallback();
	GsInitVcount();

	init_all();
	GsSetFarClip(1000);

	FntLoad(960, 256);
	SetDumpFnt(FntOpen(0, -64, 256, 200, 0, 512));

	while (1) {
		if (obj_interactive() == 0)
			return 0;	/* �p�b�h�f�[�^���瓮���̃p�����[�^������ */
		GsSetRefView2(&view);	/* ���[���h�X�N���[���}�g���b�N�X�v�Z */
		outbuf_idx = GsGetActiveBuff();	/* �_�u���o�b�t�@�̂ǂ��炩�𓾂� */

		/* Set top address of Packet Area for output of GPU PACKETS */
		GsSetWorkBase((PACKET *) out_packet[outbuf_idx]);

		GsClearOt(0, 0, &Wot[outbuf_idx]);	/* �I�[�_�����O�e�[�u�����N���A���� */

		for (i = 0, op = object; i < Objnum; i++) {
			/* ���[�J���^���[���h�E�X�N���[���}�g���b�N�X���v�Z���� */
			GsGetLws(op->coord2, &tmplw, &tmpls);

			/* ���C�g�}�g���b�N�X��GTE�ɃZ�b�g���� */
			GsSetLightMatrix(&tmplw);

			/* �X�N���[���^���[�J���}�g���b�N�X��GTE�ɃZ�b�g���� */
			GsSetLsMatrix(&tmpls);

			/* �I�u�W�F�N�g�𓧎��ϊ����I�[�_�����O�e�[�u���ɓo�^���� */
			SortTMDobject(op, &Wot[outbuf_idx], 14 - OT_LENGTH);
			op++;
		}

		DrawSync(0);
		VSync(0);	/* V�u�����N��҂� */
		/* ResetGraph(1); */
		padd = PadRead(1);	/* �p�b�h�̃f�[�^��ǂݍ��� */
		GsSwapDispBuff();/* �_�u���o�b�t�@��ؑւ��� */

		/* ��ʂ̃N���A���I�[�_�����O�e�[�u���̍ŏ��ɓo�^���� */
		GsSortClear(0x0, 0x0, 0x0, &Wot[outbuf_idx]);

		/* �I�[�_�����O�e�[�u���ɓo�^����Ă���p�P�b�g�̕`����J�n���� */
		GsDrawOt(&Wot[outbuf_idx]);
		FntFlush(-1);
	}
}

obj_interactive()
{
	SVECTOR v1;
	MATRIX  tmp1;

	/* �I�u�W�F�N�g��Y����]������ */
	if ((padd & PADRleft) > 0)
		PWorld.vy -= 5 * ONE / 360;
	/* �I�u�W�F�N�g��Y����]������ */
	if ((padd & PADRright) > 0)
		PWorld.vy += 5 * ONE / 360;
	/* �I�u�W�F�N�g��X����]������ */
	if ((padd & PADRup) > 0)
		PWorld.vx += 5 * ONE / 360;
	/* �I�u�W�F�N�g��X����]������ */
	if ((padd & PADRdown) > 0)
		PWorld.vx -= 5 * ONE / 360;

	/* �I�u�W�F�N�g��Z���ɂ����ē����� */
	if ((padd & PADm) > 0)
		DWorld.coord.t[2] -= 100;
	/* �I�u�W�F�N�g��Z���ɂ����ē����� */
	if ((padd & PADl) > 0)
		DWorld.coord.t[2] += 100;
	/* �I�u�W�F�N�g��X���ɂ����ē����� */
	if ((padd & PADLleft) > 0)
		DWorld.coord.t[0] += 10;
	/* �I�u�W�F�N�g��X���ɂ����ē����� */
	if ((padd & PADLright) > 0)
		DWorld.coord.t[0] -= 10;
	/* �I�u�W�F�N�g��Y���ɂ����ē����� */
	if ((padd & PADLdown) > 0)
		DWorld.coord.t[1] += 10;
	/* �I�u�W�F�N�g��Y���ɂ����ē����� */
	if ((padd & PADLup) > 0)
		DWorld.coord.t[1] -= 10;

	/* �v���O�������I�����ă��j�^�ɖ߂� */
	if ((padd & PADk) > 0) {
		PadStop();
		ResetGraph(3);
		StopCallback();
		return 0;
	}
	/* �I�u�W�F�N�g�̃p�����[�^����}�g���b�N�X���v�Z�����W�n�ɃZ�b�g */
	set_coordinate(&PWorld, &DWorld);
	return 1;
}

/* ���������[�`���Q  */
init_all()
{
	GsFOGPARAM  fgp;

	ResetGraph(0);		/* GPU���Z�b�g */
	PadInit(0);		/* �R���g���[�������� */
	padd = 0;		/* �R���g���[���l������ */

	GsInitGraph(640, 480, GsINTER | GsOFSGPU, 1, 0);
	/* �𑜓x�ݒ�i�C���^�[���[�X���[�h�j */

	GsDefDispBuff(0, 0, 0, 0);/* �_�u���o�b�t�@�w�� */

#if 0
	GsInitGraph(640, 240, GsINTER | GsOFSGPU, 1, 0);
	/* �𑜓x�ݒ�i�m���C���^�[���[�X���[�h�j */
	GsDefDispBuff(0, 0, 0, 240); /* �_�u���o�b�t�@�w�� */
#endif

	GsInit3D();		/* �RD�V�X�e�������� */

	Wot[0].length = OT_LENGTH;	/* �I�[�_�����O�e�[�u���n���h���ɉ𑜓x�ݒ� */
	Wot[0].offset = 0x7ff;

	Wot[0].org = zsorttable[0];	/* �I�[�_�����O�e�[�u���n���h���ɃI�[�_�����O�e�[�u��
					 �̎��̐ݒ� */

	/* �_�u���o�b�t�@�̂��߂�������ɂ������ݒ� */
	Wot[1].length = OT_LENGTH;
	Wot[1].org = zsorttable[1];

	Wot[1].offset = 0x7ff;

	coord_init();		/* ���W��` */
	model_init();		/* ���f�����O�f�[�^�ǂݍ��� */
	view_init();		/* ���_�ݒ� */
	light_init();		/* ���s�����ݒ� */

	texture_init(TEX_ADDR);/* texture load of TEX_ADDR */
	texture_init(TEX_ADDR1);/* texture load of TEX_ADDR1 */
	texture_init(TEX_ADDR2);/* texture load of TEX_ADDR2 */

}


view_init()
{				/* ���_�ݒ� */
	/*---- Set projection,view ----*/
	GsSetProjection(1000);	/* �v���W�F�N�V�����ݒ� */

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

	/* ���_�p�����[�^���Q���王�_��ݒ肷�郏�[���h�X�N���[��
	 �}�g���b�N�X���v�Z���� */
	GsSetRefView2(&view);

	GsSetNearClip(100);	/* �j�A�N���b�v�ݒ� */

}


light_init()
{				/* ���s�����ݒ� */

	/* ���C�gID�O �ݒ� */
	/* ���s���������p�����[�^�ݒ� */
	pslt[0].vx = 20;
	pslt[0].vy = -100;
	pslt[0].vz = -100;

	/* ���s�����F�p�����[�^�ݒ� */
	pslt[0].r = 0xd0;
	pslt[0].g = 0xd0;
	pslt[0].b = 0xd0;

	/* �����p�����[�^��������ݒ� */
	GsSetFlatLight(0, &pslt[0]);

	/* ���C�gID�P �ݒ� */
	pslt[1].vx = 20;
	pslt[1].vy = -50;
	pslt[1].vz = 100;
	pslt[1].r = 0x80;
	pslt[1].g = 0x80;
	pslt[1].b = 0x80;
	GsSetFlatLight(1, &pslt[1]);

	/* ���C�gID�Q �ݒ� */
	pslt[2].vx = -20;
	pslt[2].vy = 20;
	pslt[2].vz = -100;
	pslt[2].r = 0x60;
	pslt[2].g = 0x60;
	pslt[2].b = 0x60;
	GsSetFlatLight(2, &pslt[2]);

	/* �A���r�G���g�ݒ� */
	GsSetAmbient(0, 0, 0);

	/* �����v�Z�̃f�t�H���g�̕����ݒ� */
	GsSetLightMode(0);
}

coord_init()
{				/* ���W�n�ݒ� */
	/* ���W�̒�` */
	GsInitCoordinate2(WORLD, &DWorld);

	/* �}�g���b�N�X�v�Z���[�N�̃��[�e�[�V�����x�N�^�[������ */
	PWorld.vx = PWorld.vy = PWorld.vz = 0;

	/* �I�u�W�F�N�g�̌��_�����[���h��Z = -4000�ɐݒ� */
	DWorld.coord.t[2] = -4000;
}


/* ���[�e�V�����x�N�^����}�g���b�N�X���쐬�����W�n�ɃZ�b�g���� */
set_coordinate(pos, coor)
	SVECTOR *pos;		/* ���[�e�V�����x�N�^ */
	GsCOORDINATE2 *coor;	/* ���W�n */
{
	MATRIX  tmp1;

	/* ���s�ړ����Z�b�g���� */
	tmp1.t[0] = coor->coord.t[0];
	tmp1.t[1] = coor->coord.t[1];
	tmp1.t[2] = coor->coord.t[2];

	/* �}�g���b�N�X�Ƀ��[�e�[�V�����x�N�^����p������ */
	RotMatrix(pos, &tmp1);

	/* ���߂��}�g���b�N�X�����W�n�ɃZ�b�g���� */
	coor->coord = tmp1;

	/* �}�g���b�N�X�L���b�V�����t���b�V������ */
	coor->flg = 0;
}

/* �e�N�X�`���f�[�^��VRAM�Ƀ��[�h���� */
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


/* ���f�����O�f�[�^�̓ǂݍ��� */
model_init()
{
	u_long *dop;
	GsDOBJ2 *objp;		/* ���f�����O�f�[�^�n���h�� */
	int     i;

	dop = (u_long *) MODEL_ADDR;	/* ���f�����O�f�[�^���i�[����Ă���A�h���X */
					
	dop++;			/* hedder skip */

	GsMapModelingData(dop);	/* ���f�����O�f�[�^�iTMD�t�H�[�}�b�g�j�����A�h���X�Ƀ}�b�v���� */

	dop++;
	Objnum = *dop;		/* �I�u�W�F�N�g����TMD�̃w�b�_���瓾�� */

	dop++;			/* GsLinkObject4�Ń����N���邽�߂�TMD�̃I�u�W�F�N�g
				 �̐擪�ɂ����Ă��� */

	/* TMD�f�[�^�ƃI�u�W�F�N�g�n���h����ڑ����� */
	for (i = 0; i < Objnum; i++)
		GsLinkObject4((u_long) dop, &object[i], i);

	for (i = 0, objp = object; i < Objnum; i++) {
		/* �f�t�H���g�̃I�u�W�F�N�g�̍��W�n�̐ݒ� */
		objp->coord2 = &DWorld;
		objp->attribute = 0;	/* Normal Light */
		objp++;
	}
}

extern PACKET *FastG4L();	/* see g4l.c */
extern PACKET *FastTG4Lmip(); /* see tg4lmip.c */

/************************* SAMPLE for GsTMDobject *****************/
SortTMDobject(objp, otp, shift)
	GsDOBJ2 *objp;
	GsOT   *otp;
	int     shift;
{
	u_long *vertop, *nortop, *primtop, primn;
	int     code;		/* polygon type */
	int		light_mode;

	/* get various informations from TMD foramt */
	vertop = ((struct TMD_STRUCT *) (objp->tmd))->vertop;
	nortop = ((struct TMD_STRUCT *) (objp->tmd))->nortop;
	primtop = ((struct TMD_STRUCT *) (objp->tmd))->primtop;
	primn = ((struct TMD_STRUCT *) (objp->tmd))->primn;

	/* attribute decoding */
	/* GsMATE_C=objp->attribute&0x07; not use */
	GsLMODE = (objp->attribute >> 3) & 0x03;
	GsLIGNR = (objp->attribute >> 5) & 0x01;
	GsLIOFF = (objp->attribute >> 6) & 0x01;
	GsNDIV = (objp->attribute >> 9) & 0x07;
	GsTON = (objp->attribute >> 30) & 0x01;

	if(GsLIOFF == 1)
		light_mode = GsLMODE_LOFF;
	else
		if((GsLIGNR == 0 && GsLIGHT_MODE == 1) ||
			(GsLIGNR == 1 && GsLMODE == 1))
			light_mode = GsLMODE_FOG;
		else
			light_mode = GsLMODE_NORMAL;

	/* primn > 0 then loop (making packets) */
	while (primn) {
		code = ((*primtop) >> 24 & 0xfd);	/* pure polygon type */
		switch (code) {
			case GPU_COM_G4:
				GsOUT_PACKET_P = (PACKET *) FastG4L
					((TMD_P_G4 *)primtop,(VERT *)vertop,(VERT *)nortop,
					GsOUT_PACKET_P,*((u_short *) primtop),shift,otp);
				primn -= *((u_short *) primtop);
				primtop += sizeof(TMD_P_G4)/4 * *((u_short *)primtop);
				break;
			case GPU_COM_TG4:	
				GsOUT_PACKET_P = (PACKET *) FastTG4Lmip
					((TMD_P_TG4 *)primtop,(VERT *)vertop,(VERT *)nortop,
					GsOUT_PACKET_P,*((u_short *) primtop),shift,otp);
				primn -= *((u_short *) primtop);
				primtop += sizeof(TMD_P_TG4)/4 * *((u_short *)primtop);
				break;
			default:
				printf("This program supports only gouraud polygon.\n");
				printf("<%x,%x,%x>\n", code, GPU_COM_G4, GPU_COM_TG4);
				break;
		}
	}
}