/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			tmdview4: GsDOBJ4 object viewing rotine 
 *
 * "tuto7.c" ******** simple GsDOBJ4 Viewing routine using jump table 
 * 							(material attenuation supported) 
 * 
 * Version 1.00	Jul,  14, 1994 
 * 
 * Copyright (C) 1993 by Sony Computer Entertainment All rights Reserved 
 */

#include <sys/types.h>

#include <libetc.h>		/* PAD���g�����߂ɃC���N���[�h����K�v���� */
#include <libgte.h>		/* LIGS���g�����߂ɃC���N���[�h����K�v���� */
#include <libgpu.h>		/* LIGS���g�����߂ɃC���N���[�h����K�v���� */
#include <libgs.h>		/* �O���t�B�b�N���C�u���� ���g�����߂�
				   �\���̂Ȃǂ���`����Ă��� */

#define PACKETMAX 10000		/* Max GPU packets */

#define OBJECTMAX 100		/* �RD�̃��f���͘_���I�ȃI�u�W�F�N�g�� �������邱�̍ő吔
				   ���`���� */

#define PACKETMAX2 (PACKETMAX*24)	/* size of PACKETMAX (byte) paket
					   size may be 24 byte(6 word) */

#define MODEL_ADDR 0x80040000	/* ���f�����O�f�[�^�iTMD�t�H�[�}�b�g�j���������A�h���X */

#define OT_LENGTH  10		/* �I�[�_�����O�e�[�u���̉𑜓x */


GsOT    Wot[2];			/* �I�[�_�����O�e�[�u���n���h���_�u���o�b�t�@�̂��߂Q�K�v */

GsOT_TAG zsorttable[2][1 << OT_LENGTH];	/* �I�[�_�����O�e�[�u������ */

GsDOBJ2 object[OBJECTMAX];	/* �I�u�W�F�N�g�n���h���I�u�W�F�N�g�̐������K�v */

u_long  Objnum;			/* ���f�����O�f�[�^�̃I�u�W�F�N�g�̐���ێ����� */


GsCOORDINATE2 DWorld;		/* �I�u�W�F�N�g���Ƃ̍��W�n */

SVECTOR PWorld;			/* ���W�n����邽�߂̃��[�e�[�V�����x�N�^�[ */

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
	MATRIX  tmpls;

	ResetCallback();

	init_all();

	FntLoad(960, 256);
	SetDumpFnt(FntOpen(32, 32, 256, 200, 0, 512));

	while (1) {
		FntPrint("z = %d\n", DWorld.coord.t[2]);
		if (obj_interactive() == 0)
			return 0;	/* �p�b�h�f�[�^���瓮���̃p�����[�^������ */
		GsSetRefView2(&view);	/* ���[���h�X�N���[���}�g���b�N�X�v�Z */
		outbuf_idx = GsGetActiveBuff();	/* �_�u���o�b�t�@�̂ǂ��炩�𓾂� */

		/* Set top address of Packet Area for output of GPU PACKETS */
		GsSetWorkBase((PACKET *) out_packet[outbuf_idx]);

		GsClearOt(0, 0, &Wot[outbuf_idx]);	/* �I�[�_�����O�e�[�u�����N���A���� */

		for (i = 0, op = object; i < Objnum; i++) {
			/* ���[���h�^���[�J���}�g���b�N�X���v�Z���� */
			GsGetLw(op->coord2, &tmpls);

			/* ���C�g�}�g���b�N�X��GTE�ɃZ�b�g���� */
			GsSetLightMatrix(&tmpls);

			/* �X�N���[���^���[�J���}�g���b�N�X���v�Z���� */
			GsGetLs(op->coord2, &tmpls);

			/* �X�N���[���^���[�J���}�g���b�N�X��GTE�ɃZ�b�g���� */
			GsSetLsMatrix(&tmpls);

			/* �I�u�W�F�N�g�𓧎��ϊ����I�[�_�����O�e�[�u���ɓo�^���� */
			GsSortObject4J(op, &Wot[outbuf_idx], 14 - OT_LENGTH, getScratchAddr(0));
			op++;
		}
		VSync(0);	/* V�u�����N��҂� */
		DrawSync(0);
		padd = PadRead(1);	/* �p�b�h�̃f�[�^��ǂݍ��� */
		GsSwapDispBuff();	/* �_�u���o�b�t�@��ؑւ��� */

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
	/* if((padd & PADLleft)>0) DWorld.coord.t[0] +=10; */
	if ((padd & PADLleft) > 0)
		view.vrx += 10;

	/* �I�u�W�F�N�g��X���ɂ����ē����� */
	/* if((padd & PADLright)>0) DWorld.coord.t[0] -=10; */
	if ((padd & PADLright) > 0)
		view.vrx -= 10;

	/* �I�u�W�F�N�g��Y���ɂ����ē����� */
	/* if((padd & PADLdown)>0) DWorld.coord.t[1]+=10; */
	if ((padd & PADLdown) > 0)
		view.vry += 10;

	/* �I�u�W�F�N�g��Y���ɂ����ē����� */
	/* if((padd & PADLup)>0) DWorld.coord.t[1]-=10; */
	if ((padd & PADLup) > 0)
		view.vry -= 10;

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


/* ���������[�`���Q */
init_all()
{
	GsFOGPARAM fgp;
	ResetGraph(0);		/* GPU���Z�b�g */
	PadInit(0);		/* �R���g���[�������� */
	padd = 0;		/* �R���g���[���l������ */

#if 0
	GsInitGraph(640, 480, GsINTER | GsOFSGPU, 1, 0);
	/* �𑜓x�ݒ�i�C���^�[���[�X���[�h�j */

	GsDefDispBuff(0, 0, 0, 0);/* �_�u���o�b�t�@�w�� */
#endif

	GsInitGraph(640, 240, GsINTER | GsOFSGPU, 0, 0);
	/* �𑜓x�ݒ�i�m���C���^�[���[�X���[�h�j */
	GsDefDispBuff(0, 0, 0, 240);/* �_�u���o�b�t�@�w�� */


	GsInit3D();		/* �RD�V�X�e�������� */

	Wot[0].length = OT_LENGTH;	/* �I�[�_�����O�e�[�u���n���h���ɉ𑜓x�ݒ� */

	Wot[0].org = zsorttable[0];	/* �I�[�_�����O�e�[�u���n���h����
					   �I�[�_�����O�e�[�u���̎��̐ݒ� */

	/* �_�u���o�b�t�@�̂��߂�������ɂ������ݒ� */
	Wot[1].length = OT_LENGTH;
	Wot[1].org = zsorttable[1];

	coord_init();		/* ���W��` */
	model_init();		/* ���f�����O�f�[�^�ǂݍ��� */
	view_init();		/* ���_�ݒ� */
	light_init();		/* ���s�����ݒ� */

	/* setting FOG parameters */
	fgp.dqa = -10000 * ONE / 64 / 1000;
	fgp.dqb = 5 / 4 * ONE * ONE;
	fgp.rfc = fgp.gfc = fgp.bfc = 0;
	GsSetFogParam(&fgp);

	/* setting jumptable for GsSortObject4J() */
	jt_init4();
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

	/* ���_�p�����[�^���Q���王�_��ݒ肷��
	   ���[���h�X�N���[���}�g���b�N�X���v�Z���� */
	GsSetRefView2(&view);
}


light_init()
{				/* ���s�����ݒ� */
	/* ���C�gID�O �ݒ� */
	/* ���s���������p�����[�^�ݒ� */
	pslt[0].vx = 30;
	pslt[0].vy = 0;
	pslt[0].vz = -100;

	/* ���s�����F�p�����[�^�ݒ� */
	pslt[0].r = 0xf0;
	pslt[0].g = 0;
	pslt[0].b = 0;

	/* �����p�����[�^��������ݒ� */
	GsSetFlatLight(0, &pslt[0]);


	/* ���C�gID�P �ݒ� */
	pslt[1].vx = 0;
	pslt[1].vy = 30;
	pslt[1].vz = -100;
	pslt[1].r = 0;
	pslt[1].g = 0xf0;
	pslt[1].b = 0;
	GsSetFlatLight(1, &pslt[1]);

	/* ���C�gID�Q �ݒ� */
	pslt[2].vx = -30;
	pslt[2].vy = 0;
	pslt[2].vz = -100;
	pslt[2].r = 0;
	pslt[2].g = 0;
	pslt[2].b = 0xf0;
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

	dop = (u_long *) MODEL_ADDR;/* ���f�����O�f�[�^���i�[����Ă���A�h���X */

	dop++;			/* hedder skip */

	GsMapModelingData(dop);	/* ���f�����O�f�[�^�iTMD�t�H�[�}�b�g�j�����A�h���X�Ƀ}�b�v���� */

	dop++;
	Objnum = *dop;		/* �I�u�W�F�N�g����TMD�̃w�b�_���瓾�� */

	dop++;			/* GsLinkObject4J�Ń����N���邽�߂�TMD�̃I�u�W�F�N�g�̐擪�ɂ����Ă��� */

	/* TMD�f�[�^�ƃI�u�W�F�N�g�n���h����ڑ����� */
	for (i = 0; i < Objnum; i++)
		GsLinkObject4((u_long) dop, &object[i], i);

	for (i = 0, objp = object; i < Objnum; i++) {
		/* �f�t�H���g�̃I�u�W�F�N�g�̍��W�n�̐ݒ� */
		objp->coord2 = &DWorld;

		/* material attenuation setting */
		objp->attribute =  GsLLMOD | GsMATE | GsLDIM4;

		objp++;
	}
}

extern _GsFCALL GsFCALL4;	/* GsSortObject4J Func Table */
jt_init4()
{				/* Gs SortObject4J Fook Func (for material
				   attenuation) */
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
	GsFCALL4.g3[GsDivMODE_NDIV][GsLMODE_NORMAL] = GsTMDfastG3M;
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
