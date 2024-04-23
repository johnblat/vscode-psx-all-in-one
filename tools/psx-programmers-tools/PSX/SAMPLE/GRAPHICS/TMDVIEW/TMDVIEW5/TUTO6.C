/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *	tmdview5: GsDOBJ5 object viewing rotine
 *
 *	"tuto6.c" ******** GsDOBJ5 Viewing routine (cocpit view)
 *
 *		Version 1.00	Jul,  14, 1995
 *
 *		Copyright (C) 1995 by Sony Computer Entertainment
 *		All rights Reserved
 */
#include <sys/types.h>

#include <libetc.h>		/* PAD���g�����߂ɃC���N���[�h����K�v���� */
#include <libgte.h>		/* LIGS���g�����߂ɃC���N���[�h����K�v����*/
#include <libgpu.h>		/* LIGS���g�����߂ɃC���N���[�h����K�v���� */
#include <libgs.h>		/* �O���t�B�b�N���C�u���� ���g�����߂�
				   �\���̂Ȃǂ���`����Ă��� */

#define OBJECTMAX 100		/* �RD�̃��f���͘_���I�ȃI�u�W�F�N�g��
                                   �������邱�̍ő吔 ���`���� */

#define TEX_ADDR   0x80010000	/* �e�N�X�`���f�[�^�iTIM�t�H�[�}�b�g�j
				   ���������A�h���X */

#define TEX_ADDR1   0x80020000  /* Top Address of texture data1 (TIM FORMAT) */
#define TEX_ADDR2   0x80030000  /* Top Address of texture data2 (TIM FORMAT) */


#define MODEL_ADDR 0x80040000	/* ���f�����O�f�[�^�iTMD�t�H�[�}�b�g�j
				   ���������A�h���X */

#define OT_LENGTH  12		/* �I�[�_�����O�e�[�u���̉𑜓x */


GsOT            Wot[2];		/* �I�[�_�����O�e�[�u���n���h��
				   �_�u���o�b�t�@�̂��߂Q�K�v */

GsOT_TAG	zsorttable[2][1<<OT_LENGTH]; /* �I�[�_�����O�e�[�u������ */

GsDOBJ5		object[OBJECTMAX]; /* �I�u�W�F�N�g�n���h��
				      �I�u�W�F�N�g�̐������K�v */

u_long          Objnum;		/* ���f�����O�f�[�^�̃I�u�W�F�N�g�̐���
				   �ێ����� */


GsCOORDINATE2   DWorld;  /* �I�u�W�F�N�g���Ƃ̍��W�n */

GsCOORDINATE2   DView;  /* ���_���Ԃ牺������W�n */

SVECTOR         PWorld; /* ���W�n����邽�߂̃��[�e�[�V�����x�N�^�[ */

GsRVIEW2  view;			/* ���_��ݒ肷�邽�߂̍\���� */
GsF_LIGHT pslt[3];		/* ���s������ݒ肷�邽�߂̍\���� */
u_long padd;			/* �R���g���[���̃f�[�^��ێ����� */

/* �p�P�b�g�f�[�^���쐬���邽�߂̃��[�N �_�u���o�b�t�@�̂���2�{�K�v */
u_long PacketArea[0x10000];

/************* MAIN START ******************************************/
main()
{
  int     i;
  GsDOBJ5 *op;			/* �I�u�W�F�N�g�n���h���ւ̃|�C���^ */
  int     outbuf_idx;
  MATRIX  tmpls,tmplw;

  ResetCallback();
  init_all();
    
  while(1)
    {
      if(obj_interactive()==0)
        return 0;	/* �p�b�h�f�[�^���瓮���̃p�����[�^������ */
      GsSetRefView2(&view);	/* ���[���h�X�N���[���}�g���b�N�X�v�Z */
      
      outbuf_idx=GsGetActiveBuff();/* �_�u���o�b�t�@�̂ǂ��炩�𓾂� */
      
      GsClearOt(0,0,&Wot[outbuf_idx]); /* �I�[�_�����O�e�[�u�����N���A���� */
      
      for(i=0,op=object;i<Objnum;i++)
	{
	  /* LW�^LS�}�g���b�N�X���v�Z���� */
	  GsGetLws(op->coord2,&tmplw,&tmpls);
	  /* ���C�g�}�g���b�N�X��GTE�ɃZ�b�g���� */
	  GsSetLightMatrix(&tmplw);
	  /* �X�N���[���^���[�J���}�g���b�N�X��GTE�ɃZ�b�g���� */
	  GsSetLsMatrix(&tmpls);
	  /* �I�u�W�F�N�g�𓧎��ϊ����I�[�_�����O�e�[�u���ɓo�^���� */
	  GsSortObject5(op,&Wot[outbuf_idx],14-OT_LENGTH,getScratchAddr(0));
	  op++;
	}
      padd=PadRead(1);		/* �p�b�h�̃f�[�^��ǂݍ��� */
      VSync(0);			/* V�u�����N��҂� */
      ResetGraph(1);		/* GPU�����Z�b�g���� */
      GsSwapDispBuff();		/* �_�u���o�b�t�@��ؑւ��� */
      SetDispMask(1);
      
      /* ��ʂ̃N���A���I�[�_�����O�e�[�u���̍ŏ��ɓo�^���� */
      GsSortClear(0x0,0x0,0x0,&Wot[outbuf_idx]);
      
      /* �I�[�_�����O�e�[�u���ɓo�^����Ă���p�P�b�g�̕`����J�n���� */
      GsDrawOt(&Wot[outbuf_idx]);
    }
}

obj_interactive()
{
  SVECTOR dd;
  SVECTOR ax; /* ���W�n����邽�߂̃��[�e�[�V�����x�N�^�[ */
  MATRIX  tmp1;
  VECTOR  tmpv;
  static  int count = 0;

  count++;
  
  dd.vx = dd.vy = dd.vz = 0;
  ax.vx = ax.vy = ax.vz = 0;
  
  /* �I�u�W�F�N�g��Y����]������ */
  if((padd & PADRleft)>0) ax.vy -=1*ONE/360;
  
  /* �I�u�W�F�N�g��Y����]������ */
  if((padd & PADRright)>0) ax.vy +=1*ONE/360;
  
  /* �I�u�W�F�N�g��X����]������ */
  if((padd & PADRup)>0) ax.vx+=1*ONE/360;
  
  /* �I�u�W�F�N�g��X����]������ */
  if((padd & PADRdown)>0) ax.vx-=1*ONE/360;
  
  /* �I�u�W�F�N�g��Z���ŉ� */
  if((padd & PADo)>0) ax.vz+=1*ONE/360/2;
  
  /* �I�u�W�F�N�g��Z���ŉ� */
  if((padd & PADn)>0) ax.vz-=1*ONE/360/2;
  
  /* �I�u�W�F�N�g��Z���ɂ����ē����� */
  if((padd & PADm)>0) dd.vz=100;
  
  /* �I�u�W�F�N�g��Z���ɂ����ē����� */
  if((padd & PADl)>0) dd.vz= -100;
  
  /* �I�u�W�F�N�g��X���ɂ����ē����� */
  if((padd & PADLright)>0) dd.vx=20;
  
  /* �I�u�W�F�N�g��X���ɂ����ē����� */
  if((padd & PADLleft)>0) dd.vx= -20;
  
  /* �I�u�W�F�N�g��Y���ɂ����ē����� */
  if((padd & PADLdown)>0) dd.vy=20;
  
  /* �I�u�W�F�N�g��Y���ɂ����ē����� */
  if((padd & PADLup)>0) dd.vy= -20;

  /* ��ψړ���World�̈ړ��ɒ��� */
  ApplyMatrix(&DView.coord,&dd,&tmpv);
  DView.coord.t[0] += tmpv.vx;
  DView.coord.t[1] += tmpv.vy;
  DView.coord.t[2] += tmpv.vz;

  /* �v���O�������I�����ă��j�^�ɖ߂� */
/*  if((padd & PADk)>0) {PadStop();ResetGraph(3);StopCallback();return 0;}*/
  if((padd & PADk)>0) {
	PadStop();
	ResetGraph(3);
	StopCallback();
	return 0;
  }
  
  /* �I�u�W�F�N�g�̃p�����[�^����}�g���b�N�X���v�Z�����W�n�ɃZ�b�g */
  set_shukan_coordinate(&ax,&DView);
  return 1;
}

/* ���[�e�V�����x�N�^����}�g���b�N�X���쐬�����W�n�ɃZ�b�g���� */
set_shukan_coordinate(pos,coor)
SVECTOR *pos;			/* ���[�e�V�����x�N�^ */
GsCOORDINATE2 *coor;		/* ���W�n */
{
  MATRIX tmp1;

  /* �}�g���b�N�X�L���b�V�����t���b�V������ */
  coor->flg = 0;
  
  /* ��]���Ȃ��ꍇ�͉��������ɂʂ��� */
  if(pos->vx==0 && pos->vy==0 && pos->vz==0)
    return;
  
  /* ��]�}�g���b�N�X�����߂� */
  RotMatrix(pos,&tmp1);
  
  /* ��]�}�g���b�N�X���|���� */
  MulMatrix(&coor->coord,&tmp1);
  
  /* �덷�̒~�ς̂��߂� �`���c��ł��܂��̂𐮌`���� */
  MatrixNormal(&coor->coord,&coor->coord);
}

init_all()			/* ���������[�`���Q */
{
  ResetGraph(0);		/* GPU���Z�b�g */
  PadInit(0);			/* �R���g���[�������� */
  padd=0;			/* �R���g���[���l������ */

#if 0
  GsInitGraph(640,480,GsINTER|GsOFSGPU,1,0);
  /* �𑜓x�ݒ�i�C���^�[���[�X���[�h�j */
  
  GsDefDispBuff(0,0,0,0);	/* �_�u���o�b�t�@�w�� */
#endif
  GsInitGraph(640,240,GsNONINTER|GsOFSGPU,1,0);
  /* �𑜓x�ݒ�i�m���C���^�[���[�X���[�h�j */
  GsDefDispBuff(0,0,0,240);   /* �_�u���o�b�t�@�w�� */
  GsInit3D();                 /* �RD�V�X�e�������� */
  
  Wot[0].length=OT_LENGTH;    /* OT�P�ɉ𑜓x�ݒ� */
  Wot[0].org=zsorttable[0];   /* OT�P�ɃI�[�_�����O�e�[�u���̎��̐ݒ� */
  /* �_�u���o�b�t�@�̂��߂�������ɂ������ݒ� */
  Wot[1].length=OT_LENGTH;
  Wot[1].org=zsorttable[1];
  
  coord_init();			/* ���W��` */
  model_init();			/* ���f�����O�f�[�^�ǂݍ��� */
  view_init();			/* ���_�ݒ� */
  light_init();			/* ���s�����ݒ� */
  
  texture_init(TEX_ADDR);	/* 16bit texture load */
  texture_init(TEX_ADDR1);	/* 8bit  texture load */
  texture_init(TEX_ADDR2);	/* 4bit  texture load */
}



view_init()			/* ���_�ݒ� */
{
  /*---- Set projection,view ----*/
  GsSetProjection(1000);	/* �v���W�F�N�V�����ݒ� */
  
  /* ���_�p�����[�^�ݒ� */
  view.vpx = 0; view.vpy = 0; view.vpz = 2000;
  /* �����_�p�����[�^�ݒ� */  
  view.vrx = 0; view.vry = 0; view.vrz = -4000;
  /* ���_�̔P��p�����[�^�ݒ� */
  view.rz=0;
  GsInitCoordinate2(WORLD,&DView); /* ���_��ݒ肷����W�n�̏����� */
  view.super = &DView;		   /* ���_���W�p�����[�^�ݒ� */
  
  /* ���_�p�����[�^���Q���王�_��ݒ肷��
     ���[���h�X�N���[���}�g���b�N�X���v�Z���� */
  GsSetRefView2(&view);
}


light_init()			/* ���s�����ݒ� */
{
  /* ���C�gID�O �ݒ� */
  /* ���s���������p�����[�^�ݒ� */
  pslt[0].vx = 20; pslt[0].vy= -100; pslt[0].vz= -100;
  
  /* ���s�����F�p�����[�^�ݒ� */
  pslt[0].r=0xd0; pslt[0].g=0xd0; pslt[0].b=0xd0;
  
  /* �����p�����[�^��������ݒ� */
  GsSetFlatLight(0,&pslt[0]);
  
  /* ���C�gID�P �ݒ� */
  pslt[1].vx = 20; pslt[1].vy= -50; pslt[1].vz= 100;
  pslt[1].r=0x80; pslt[1].g=0x80; pslt[1].b=0x80;
  GsSetFlatLight(1,&pslt[1]);
  
  /* ���C�gID�Q �ݒ� */
  pslt[2].vx = -20; pslt[2].vy= 20; pslt[2].vz= -100;
  pslt[2].r=0x60; pslt[2].g=0x60; pslt[2].b=0x60;
  GsSetFlatLight(2,&pslt[2]);
  
  /* �A���r�G���g�ݒ� */
  GsSetAmbient(ONE/4,ONE/4,ONE/4);

  /* �����v�Z�̃f�t�H���g�̕����ݒ� */
  GsSetLightMode(0);
}

coord_init()			/* ���W�n�ݒ� */
{
  /* ���W�̒�` */
  GsInitCoordinate2(WORLD,&DWorld);
  
  /* �}�g���b�N�X�v�Z���[�N�̃��[�e�[�V�����x�N�^�[������ */
  PWorld.vx=PWorld.vy=PWorld.vz=0;
  
  /* �I�u�W�F�N�g�̌��_�����[���h��Z = -4000�ɐݒ� */
  DWorld.coord.t[2] = -4000;
}


/* �e�N�X�`���f�[�^��VRAM�Ƀ��[�h���� */
texture_init(addr)
u_long addr;
{
  RECT rect1;
  GsIMAGE tim1;

  /* TIM�f�[�^�̃w�b�_����e�N�X�`���̃f�[�^�^�C�v�̏��𓾂� */
  GsGetTimInfo((u_long *)(addr+4),&tim1);
  
  rect1.x=tim1.px;		/* �e�N�X�`�������VRAM�ł�X���W */
  rect1.y=tim1.py;		/* �e�N�X�`�������VRAM�ł�Y���W */
  rect1.w=tim1.pw;		/* �e�N�X�`���� */
  rect1.h=tim1.ph;		/* �e�N�X�`������ */
  
  /* VRAM�Ƀe�N�X�`�������[�h���� */
  LoadImage(&rect1,tim1.pixel);
  
  /* �J���[���b�N�A�b�v�e�[�u�������݂��� */  
  if((tim1.pmode>>3)&0x01)
    {
      rect1.x=tim1.cx;		/* �N���b�g�����VRAM�ł�X���W */
      rect1.y=tim1.cy;		/* �N���b�g�����VRAM�ł�Y���W */
      rect1.w=tim1.cw;		/* �N���b�g�̕� */
      rect1.h=tim1.ch;		/* �N���b�g�̍��� */

      /* VRAM�ɃN���b�g�����[�h���� */
      LoadImage(&rect1,tim1.clut);
    }
}


model_init()
{				/* ���f�����O�f�[�^�̓ǂݍ��� */
  u_long *dop;
  GsDOBJ5 *objp;		/* ���f�����O�f�[�^�n���h�� */
  int i;
  u_long *oppp;
  
  dop=(u_long *)MODEL_ADDR;	/* ���f�����O�f�[�^���i�[����Ă���A�h���X */
  dop++;			/* hedder skip */
  
  GsMapModelingData(dop);	/* ���f�����O�f�[�^�iTMD�t�H�[�}�b�g�j��
				   ���A�h���X�Ƀ}�b�v���� */
  dop++;
  Objnum = *dop;		/* �I�u�W�F�N�g����TMD�̃w�b�_���瓾�� */
  dop++;			/* GsLinkObject5�Ń����N���邽�߂�TMD��
				   �I�u�W�F�N�g�̐擪�ɂ����Ă��� */

  for(i=0;i<Objnum;i++)		/* TMD�f�[�^�ƃI�u�W�F�N�g�n���h����ڑ����� */
    GsLinkObject5((u_long)dop,&object[i],i);
  
  oppp = PacketArea;		/* packet�̘g�g�����A�h���X */
  for(i=0,objp=object;i<Objnum;i++)
    { /* �f�t�H���g�̃I�u�W�F�N�g�̍��W�n�̐ݒ� */
      objp->coord2 =  &DWorld;
      
      objp->attribute = 0;	/* attribute init */
      /* objp->attribute |= GsLOFF;	/* attribute�̌����v�Z�𐧌� */
      
      oppp = GsPresetObject(objp,oppp);
      objp++;
    }
}