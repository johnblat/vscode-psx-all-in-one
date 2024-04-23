/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *	rcube: PS-X Demonstration program
 *
 *	"main.c" Main routine
 *
 *		Version 3.02	Jan, 9, 1995
 *
 *		Copyright (C) 1993,1994,1995 by Sony Computer Entertainment
 *			All rights Reserved
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "table.h"
#include "pos.h"

/* �e�N�X�`����� */
#define TIM_ADDR 0x80020000		/* �g�p����TIM�t�@�C���̊i�[�A�h���X */

#define TIM_HEADER 0x00000010

/* ���f�����O�f�[�^��� */
#define TMD_ADDR 0x80010000		/* �g�p����TMD�t�@�C���̊i�[�A�h���X */

u_long *TmdBase;			/* TMD�̂����A�I�u�W�F�N�g���̃A�h���X */

int CurrentTmd; 			/* �g�p����TMD�ԍ� */

/* �I�[�_�����O�e�[�u�� (OT) */
#define OT_LENGTH  7			/* OT�𑜓x�i�傫���j */
GsOT WorldOT[2];			/* OT���i�_�u���o�b�t�@�j */
GsOT_TAG OTTags[2][1<<OT_LENGTH];	/* OT�̃^�O�̈�i�_�u���o�b�t�@�j */

/* GPU�p�P�b�g�����̈� */
#define PACKETMAX 1000			/* 1�t���[���̍ő�p�P�b�g�� */

PACKET GpuPacketArea[2][PACKETMAX*64];	/* �p�P�b�g�̈�i�_�u���o�b�t�@�j */

/* �I�u�W�F�N�g�i�L���[�u�j�ϐ� */
#define NCUBE 44
#define OBJMAX NCUBE
int nobj;				/* �L���[�u�� */
GsDOBJ2 object[OBJMAX];			/* 3D�I�u�W�F�N�g�ϐ� */
GsCOORDINATE2 objcoord[OBJMAX];		/* ���[�J�����W�ϐ� */

SVECTOR Rot[OBJMAX];			/* ��]�p */
SVECTOR RotV[OBJMAX];			/* ��]�X�s�[�h�i�p���x�j */

VECTOR Trns[OBJMAX];			/* �L���[�u�ʒu�i���s�ړ��ʁj */

VECTOR TrnsV[OBJMAX];			/* �ړ��X�s�[�h */

/* ���_�iVIEW�j */
GsRVIEW2  View;			/* ���_�ϐ� */
int ViewAngleXZ;		/* ���_�̍��� */
int ViewRadial;			/* ���_����̋��� */
#define DISTANCE 600		/* Radial�̏����l */

/* ���� */
GsF_LIGHT pslt[3];			/* �������ϐ��~3 */

/* ���̑�... */
int Bakuhatu;				/* ���������t���O */
u_long PadData;				/* �R���g���[���p�b�h�̏�� */
u_long oldpad;				/* �P�t���[���O�̃p�b�h��� */
GsFOGPARAM dq;				/* �f�v�X�L���[(�t�H�O)�p�p�����[�^ */
int dqf;				/* �t�H�O��ON���ǂ��� */
int back_r, back_g, back_b;		/* �o�b�N�O���E���h�F */

#define FOG_R 160
#define FOG_G 170
#define FOG_B 180

/* �֐��̃v���g�^�C�v�錾 */
void drawCubes();
int moveCubes();
void initModelingData();
void allocCube();
void initSystem();
void initAll();
void initTexture();
void initView();
void initLight();
void changeFog();
void changeTmd();

/* ���C�����[�`�� */
main()
{
	/* �V�X�e���̏����� */
	ResetCallback();
	initSystem();

	/* ���̑��̏����� */
	Bakuhatu = 0;
	PadData = 0;
	CurrentTmd = 0;
	dqf = 0;
	back_r = back_g = back_b = 0;
	initView();
	initLight(0, 0xc0);
	initModelingData(TMD_ADDR);
	initTexture(TIM_ADDR);
	allocCube(NCUBE);
	
	/* ���C�����[�v */
	while(1) {
		if(moveCubes()==0)
		  return 0;
		GsSetRefView2(&View);
		drawCubes();
	}
}


/* 3D�I�u�W�F�N�g�i�L���[�u�j�̕`�� */
void drawCubes()
{
	int i;
	GsDOBJ2 *objp;
	int activeBuff;
	MATRIX LsMtx;

	/* �_�u���o�b�t�@�̂����ǂ��炪�A�N�e�B�u���H */
	activeBuff = GsGetActiveBuff();

	/* GPU�p�P�b�g�����A�h���X���G���A�̐擪�ɐݒ� */
	GsSetWorkBase((PACKET*)GpuPacketArea[activeBuff]);

	/* OT�̓��e���N���A */
	GsClearOt(0, 0, &WorldOT[activeBuff]);

	/* 3D�I�u�W�F�N�g�i�L���[�u�j��OT�ւ̓o�^ */
	objp = object;
	for(i = 0; i < nobj; i++) {

		/* ��]�p->�}�g���N�X�ɃZ�b�g */
		RotMatrix(Rot+i, &(objp->coord2->coord));
		
		/* �}�g���N�X���X�V�����̂Ńt���O�����Z�b�g */
		objp->coord2->flg = 0;

		/* ���s�ړ���->�}�g���N�X�ɃZ�b�g */
		TransMatrix(&(objp->coord2->coord), &Trns[i]);
		
		/* �����ϊ��̂��߂̃}�g���N�X���v�Z���Ăf�s�d�ɃZ�b�g */
		GsGetLs(objp->coord2, &LsMtx);
		GsSetLsMatrix(&LsMtx);
		GsSetLightMatrix(&LsMtx);

		/* �����ϊ�����OT�ɓo�^ */
		GsSortObject4(objp, &WorldOT[activeBuff], 14-OT_LENGTH,getScratchAddr(0));
		objp++;
	}

	/* �p�b�h�̓��e���o�b�t�@�Ɏ�荞�� */
	oldpad = PadData;
	PadData = PadRead(1);
	
	/* V-BLANK��҂� */
 	VSync(0);
	
	/* �O�̃t���[���̕`���Ƃ������I�� */
	ResetGraph(1);

	/* �_�u���o�b�t�@����ꊷ���� */
	GsSwapDispBuff();

	/* OT�̐擪�ɉ�ʃN���A���߂�}�� */
	GsSortClear(back_r, back_g, back_b, &WorldOT[activeBuff]);

	/* OT�̓��e���o�b�N�O���E���h�ŕ`��J�n */
	GsDrawOt(&WorldOT[activeBuff]);
}

/* �L���[�u�̈ړ� */
int moveCubes()
{
	int i;
	GsDOBJ2   *objp;
	
	/* �v���O�������I�����ă��j�^�ɖ߂� */
/*	if((PadData & PADk)>0) {PadStop();ResetGraph(3);StopCallback();return 0;}*/
	if((PadData & PADk)>0) {
		PadStop();
		ResetGraph(3);
		StopCallback();
		return 0;
	}
	
	/* �p�b�h�̒l�ɂ���ď��� */
	if((PadData & PADLleft)>0) {
		ViewAngleXZ++;
		if(ViewAngleXZ >= 72) {
			ViewAngleXZ = 0;
		}
	}
	if((PadData & PADLright)>0) {
		ViewAngleXZ--;
		if(ViewAngleXZ < 0) {
		  ViewAngleXZ = 71;
		}
	}
	if((PadData & PADLup)>0) View.vpy += 100;
	if((PadData & PADLdown)>0) View.vpy -= 100;
	if((PadData & PADRdown)>0) {
		ViewRadial-=3;
		if(ViewRadial < 8) {
			ViewRadial = 8;
		}
	}
	if((PadData & PADRright)>0) {
		ViewRadial+=3;
		if(ViewRadial > 450) {
			ViewRadial = 450;
		}
	}
	if((PadData & PADk)>0) return(-1);
	if(((PadData & PADRleft)>0)&&((oldpad&PADRleft) == 0)) changeFog();
	if(((PadData & PADRup)>0)&&((oldpad&PADRup) == 0)) changeTmd();
	if(((PadData & PADn)>0)&&((oldpad&PADn) == 0)) Bakuhatu = 1;
	if(((PadData & PADl)>0)&&((oldpad&PADl) == 0)) allocCube(NCUBE);

	View.vpx = rotx[ViewAngleXZ]*ViewRadial;
	View.vpz = roty[ViewAngleXZ]*ViewRadial;

	/* �L���[�u�̈ʒu���X�V */
	objp = object;
	for(i = 0; i < nobj; i++) {

		/* �����̊J�n */
		if(Bakuhatu == 1) {

			/* ���]���x up */
			RotV[i].vx *= 3;
			RotV[i].vy *= 3;
			RotV[i].vz *= 3;

			/* �ړ�����&���x�ݒ� */
			TrnsV[i].vx = objp->coord2->coord.t[0]/4+
						(rand()-16384)/200;	
			TrnsV[i].vy = objp->coord2->coord.t[1]/6+
						(rand()-16384)/200-200;
			TrnsV[i].vz = objp->coord2->coord.t[2]/4+
						(rand()-16384)/200;
		}
		/* �������̏��� */
		else if(Bakuhatu > 1) {
			if(Trns[i].vy > 3000) {
				Trns[i].vy = 3000-(Trns[i].vy-3000)/2;
				TrnsV[i].vy = -TrnsV[i].vy*6/10;
			}
			else {
				TrnsV[i].vy += 20*2;	/* ���R���� */
			}

			if((TrnsV[i].vy < 70)&&(TrnsV[i].vy > -70)&&
			   (Trns[i].vy > 2800)) {
				Trns[i].vy = 3000;
				TrnsV[i].vy = 0;

				RotV[i].vx *= 95/100;
				RotV[i].vy *= 95/100;
				RotV[i].vz *= 95/100;
			}


			TrnsV[i].vx = TrnsV[i].vx*97/100;
			TrnsV[i].vz = TrnsV[i].vz*97/100;
		}

		/* ��]�p(Rotation)�̍X�V */
 		Rot[i].vx += RotV[i].vx;
		Rot[i].vy += RotV[i].vy;
		Rot[i].vz += RotV[i].vz;

		/* ���s�ړ���(Transfer)�̍X�V */
 		Trns[i].vx += TrnsV[i].vx;
		Trns[i].vy += TrnsV[i].vy;
		Trns[i].vz += TrnsV[i].vz;

		objp++;
	}

	if(Bakuhatu == 1)
		Bakuhatu++;

	return(1);
}

/* �L���[�u�������ʒu�ɔz�u */
void allocCube(n)
int n;
{	
	int x, y, z;
	int i;
	int *posp;
	GsDOBJ2 *objp;
	GsCOORDINATE2 *coordp;

	posp = cube_def_pos;
	objp = object;
	coordp = objcoord;
	nobj = 0;
	for(i = 0; i < NCUBE; i++) {

		/* �I�u�W�F�N�g�\���̂̏����� */
		GsLinkObject4((u_long)TmdBase, objp, CurrentTmd);
		GsInitCoordinate2(WORLD, coordp);
		objp->coord2 = coordp;
		objp->attribute = 0;
		coordp++;
		objp++;

		/* �����ʒu�̐ݒ�(pos.h����ǂ�) */
		Trns[i].vx = *posp++;
		Trns[i].vy = *posp++;
		Trns[i].vz = *posp++;
		Rot[i].vx = 0;
		Rot[i].vy = 0;
		Rot[i].vz = 0;

		/* ���x�̏����� */
		TrnsV[i].vx = 0;
		TrnsV[i].vy = 0;
		TrnsV[i].vz = 0;
		RotV[i].vx = rand()/300;
		RotV[i].vy = rand()/300;
		RotV[i].vz = rand()/300;

		nobj++;
	}
	Bakuhatu = 0;
}

/* �C�j�V�����C�Y�֐��Q */
void initSystem()
{
	int i;

	/* �p�b�h�̏����� */
	PadInit(0);

	/* �O���t�B�b�N�̏����� */
	GsInitGraph(640, 480, 2, 0, 0);
	GsDefDispBuff(0, 0, 0, 0);

	/* OT�̏����� */
	for(i = 0; i < 2; i++) {	
		WorldOT[i].length = OT_LENGTH;
		WorldOT[i].org = OTTags[i];
	}	

	/* 3D�V�X�e���̏����� */
	GsInit3D();
}

void initModelingData(tmdp)
u_long *tmdp;
{
	u_long size;
	int i;
	int tmdobjnum;
	
	/* �w�b�_���X�L�b�v */
	tmdp++;

	/* ���A�h���X�փ}�b�s���O */
	GsMapModelingData(tmdp);

	tmdp++;
	tmdobjnum = *tmdp;
	tmdp++; 		/* �擪�̃I�u�W�F�N�g���|�C���g */
	TmdBase = tmdp;
}

/* �e�N�X�`���̓ǂݍ��݁iVRAM�ւ̓]���j */
void initTexture(tex_addr)
u_long *tex_addr;
{
	RECT rect1;
	GsIMAGE tim1;
	int i;
	
	while(1) {
		if(*tex_addr != TIM_HEADER) {
			break;
		}
		tex_addr++;	/* �w�b�_�̃X�L�b�v(1word) */
		GsGetTimInfo(tex_addr, &tim1);
		tex_addr += tim1.pw*tim1.ph/2+3+1;	/* ���̃u���b�N�܂Ői�߂� */

		rect1.x=tim1.px;
		rect1.y=tim1.py;
		rect1.w=tim1.pw;
		rect1.h=tim1.ph;
		LoadImage(&rect1,tim1.pixel);
		if((tim1.pmode>>3)&0x01) {	/* CLUT������Γ]�� */

			rect1.x=tim1.cx;
			rect1.y=tim1.cy;
			rect1.w=tim1.cw;
			rect1.h=tim1.ch;
			LoadImage(&rect1,tim1.clut);
			tex_addr += tim1.cw*tim1.ch/2+3;
		}
	}
}

/* ���_�̏����� */
void initView()
{
	/* �����ʒu�����_�ϐ��ɃZ�b�g */
	ViewAngleXZ = 54;
	ViewRadial = 75;
	View.vpx = rotx[ViewAngleXZ]*ViewRadial;
	View.vpy = -100;
	View.vpz = roty[ViewAngleXZ]*ViewRadial;
	View.vrx = 0; View.vry = 0; View.vrz = 0;
	View.rz=0;

	/* ���_�̐e���W */
	View.super = WORLD;

	/* �ݒ� */
	GsSetRefView2(&View);

	/* Projection */
	GsSetProjection(1000);

	/* Mode = 'normal lighting' */
	GsSetLightMode(0);
}

/* �����̏����� */
void initLight(c_mode, factor)
int c_mode;	/* �O�̂Ƃ����F���A�P�̂Ƃ��J�N�e�����C�g */
int factor;	/* ���邳�̃t�@�N�^�[(0�`255) */
{
	if(c_mode == 0) {
		/* ���F���̃Z�b�g */
		pslt[0].vx = 200; pslt[0].vy= 200; pslt[0].vz= 300;
		pslt[0].r = factor; pslt[0].g = factor; pslt[0].b = factor;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -50; pslt[1].vy= -1000; pslt[1].vz= 0;
		pslt[1].r=0x20; pslt[1].g=0x20; pslt[1].b=0x20;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= 100;
		pslt[2].r=0x0; pslt[2].g=0x0; pslt[2].b=0x0;
		GsSetFlatLight(2,&pslt[2]);
	}
	else {
		/* �J�N�e�����C�g�iGouraud�Ŏg�p�j */
		pslt[0].vx = 200; pslt[0].vy= 100; pslt[0].vz= 0;
		pslt[0].r = factor; pslt[0].g = 0; pslt[0].b = 0;
		GsSetFlatLight(0,&pslt[0]);
		
		pslt[1].vx = -200; pslt[1].vy= 100; pslt[1].vz= 0;
		pslt[1].r=0; pslt[1].g=0; pslt[1].b=factor;
		GsSetFlatLight(1,&pslt[1]);
		
		pslt[2].vx = 0; pslt[2].vy= -200; pslt[2].vz= 0;
		pslt[2].r=0; pslt[2].g=factor; pslt[2].b=0;
		GsSetFlatLight(2,&pslt[2]);
	}	

	/* �A���r�G���g�i���ӌ��j */
	GsSetAmbient(ONE/2,ONE/2,ONE/2);
}

/* �t�H�O��ON/OFF */
void changeFog()
{
	if(dqf) {
		/* �t�H�O�̃��Z�b�g */
		GsSetLightMode(0);
		dqf = 0;
		back_r = 0;
		back_g = 0;
		back_b = 0;
	}
	else {
		/* �t�H�O�̐ݒ� */
		dq.dqa = -600;
		dq.dqb = 5120*4096;
		dq.rfc = FOG_R;
		dq.gfc = FOG_G;
		dq.bfc = FOG_B;
		GsSetFogParam(&dq);
		GsSetLightMode(1);
		dqf = 1;
		back_r = FOG_R;
		back_g = FOG_G;
		back_b = FOG_B;
	}
}

/* TMD�f�[�^�̐؂�ւ� */
void changeTmd()
{
	u_long *tmdp;
	GsDOBJ2 *objp;
	int i;

	/* TMD��؂�ւ� */
	CurrentTmd++;
	if(CurrentTmd == 4) {
		CurrentTmd = 0;
	}
	objp = object;
	for(i = 0; i < nobj; i++) {
		GsLinkObject4((u_long)TmdBase, objp, CurrentTmd);
		objp++;
	}

	/* TMD�̎�ނɂ��킹�Č����̐F/���邳��؂�ւ� */
	switch(CurrentTmd) {
	    case 0:
                /* �m�[�}�� (flat) */
		initLight(0, 0xc0);
		break;
	    case 1:
	        /* ������ (flat) */
		initLight(0, 0xc0);
		break;
	    case 2:
	        /* Gouraud */
		initLight(1, 0xff);
		break;
	    case 3:
	        /* �e�N�X�`���t�� */
		initLight(0, 0xff);
		break;
	}
}
