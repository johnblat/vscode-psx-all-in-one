/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *				main
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved;
 *
 *	 Version	Date		Design
 *	--------------------------------------------------
 *	1.00		Jun,19,1995	suzu	
 *	2.00		Feg,15,1996	suzu	
 */
/* �n�� */

#include "sys.h"

/*
 * mesh environment
 */
#define OTSIZE	4096			/* ordering table size */
#define MAXHEAP	1024			/* primitive buffer */
#define CL_UX	(1<<11)			/* cell size (widht) */	
#define CL_UY	(1<<11)			/* cell size (height) */

/* GEOMENV: �J�����̈ʒu�ƕ����Ɋւ���f�[�^�x�[�X
 * ���[�J���X�N���[���}�g���N�X�́AgeomUpdate() ����angle ����v�Z�����B
 * �J�����̌��݈ʒu(home)�̓��[���h���W�n�ŋL�q�����B
 * �J�����̈ړ��� (dvs) �̓X�N���[�����W�n�ŋL�q�����B
 * �ړ��ʂ̓��[���h���W�n�ɕϊ����ꂽ��	 home �ɉ��Z�����B���̎� z 
 * �����i�㉺�����j�̈ړ��ʂ͋����I�� 0 �ɐݒ肳���ɃJ�����̍�������
 * ��ɂȂ�悤�ɕۂ����B
 * �n�ʂ̓��b�V���\���ŕ\������� x, y �����̈ړ��ʂ����b�V���̃Z����
 * �� (CL_UX,CL_UY) ���z����ƑS�̂̃}�b�v�̃C���f�N�X���t�����Ɉړ���
 * �� home �̈ʒu��␳����B���̂��߁Ahome �̈ړ��ʂ͎��� 
 * (0,0)-(CL_UX-1,CL_UY-1) ���z���Ȃ��B 
 */
static GEOMENV	genv = {
	0,		0,	0,   0,	/* moving vector */
	1024,		0,	512, 0,	/* local-screen angle */
	1024,		1024, 	2048,0,	/* current position */
	
	CL_UX,		CL_UY,		/* geometry wrap-round unit */
	16000,		16000*3/5,	/* fog near and fog end */
};


/* �n�ʂ�\�����郁�b�V���f�[�^�\��
 * �n�ʂ� POLY_FT4 �̂Q�����z��i�Z���j�ŕ\�������B�Z���͂܂����[��
 * �h���W�n�ł̃N���b�v�͈� clipw ���ɂ�����̂����������ϊ�����A����
 * �ɃX�N���[���͈� clips �Ɏ��܂���̂����� OT �ɐڑ������B 
 * ���_�ɋ߂��Z���́A�e�N�X�`���̘c�݂�����邽�߂ɓK���I�ɕ��������B
 * �e�Z���̃e�N�X�`����i�q�_�̍����i�΍��j�Ɋւ�����͕ʂ� map �z��
 * �ɋL�^�����B�ʏ� map �̑傫���̓Z���z������͂邩�ɑ傫�����̂�
 * �g�p�����B�J�����̈ʒu�ɉ����Ċe�Z���ɑΉ����� map �̗v�f���ړ���
 * �Ă����B
 */
static RECT	_clips;		/* screen clip window */
static RECT32	_clipw;		/* world clip window */

static MESH	mesh = {
	0,				/* map (set later) */
	&_clipw,			/* clip window (world) */
	&_clips,			/* clip window (screen) */
	CL_UX,          CL_UY,		/* cell unit size */
	0,		0,		/* map size (set later) */
	0,		0,		/* map mask (set later) */
	0,	        0,		/* messh offset */
	SCR_Z*2,			/* sub-dividison start point */
	3,				/* max subdivision point */
	128*128,			/* division threshold */
	0,				/* debug mode */
};
	
/*
 * double buffer
 */
typedef struct {			
	u_long		ot[OTSIZE];		/* ordering table */
	POLY_FT4	heap[MAXHEAP];		/* heap */
} DB;

void FNAME (void)
{
	extern 	u_long	mudtex[];	/* MUD (64x64: 4bit) */
	extern 	u_long	bgtex[];	/* BG cell texture pattern */
	static DB	db[2];		/* double buffer */
	DB	*cdb;			/* current double buffer */
	u_long	*ot;			/* current OT */
	u_long	padd;
	int	cnt;
	int	ox, oy;
	
	POLY_FT4	*heap;
	
	/* initialize double buffer */
	db_init(SCR_W, SCR_H, SCR_Z, 0, 0, 0);

	/* initialize debug menu */
	menu_init();

	/* initialize fog parameters */
	SetFarColor(0, 0, 0);
	SetFogNearFar(genv.fog_near, genv.fog_far, SCR_Z);	

	/* load texture */
	mesh.tpage = LoadTPage(bgtex+0x80, 0, 0, 640, 0, 256, 256);
	mesh.clut  = LoadClut(bgtex, 0, 480);
	
	/* �}�b�v��������������B�}�b�v�ɂ̓��b�V���̊e�Z���ɑΉ���
	 * ��e�N�X�`���p�^�[����A�Z���̍��W�̕΍����L�^�����
	 */
	initMap(&mesh);
	
	/* �v���~�e�B�u�̈ʒu�� mesh.divz �����߂��Ȃ�ƓK���������J
	 * �n�����B�K���I�������s�Ȃ�Ȃ��ꍇ�́A������ 0 ���w�肷��
	 */
#ifdef NO_DIV
	mesh.divz = 0;
#endif
	
	/* ���ۂ̃}�b�v�̒l�� mesh.msk_x, mesh.msk_y �ŏ�ʃr�b�g���}
	 * �X�N���ꂽ�l�ŃC���f�N�X�����B����ɂ��A���ۂ̃}�b�v�f�[
	 *�^���J��Ԃ��ėp���ĉ��z�I�ɑ傫�ȃ}�b�v���������邱�Ƃ��ł���B
	 * �����ł́A���ۂ̃}�b�v�f�[�^�͂��̂܂܂ŁA���z�}�b�v�T�C�Y
	 * �� 256 �{���ă}�b�v�̌J��Ԃ�����������
	 */
#ifdef RAP_ROUND
	mesh.mx *= 256;
	mesh.my *= 256;
#endif
	
	/* �̈�N���b�v�����i�ɍs�Ȃ��Ă��邩�́A�X�N���[���̈����
	 * �ۂɃf�B�X�v���C�ɕ\�������̈�����������̈���w�肷��
	 * ���ƂŁA���邱�Ƃ��ł���B�����ł́A�N���b�v�X�N���[���̑�
	 * ������\���̈�� 1/2 �ɐݒ肷��
	 */
#ifdef VIEW_CLIP
	setRECT(mesh.clips, -SCR_X/2, -SCR_Y/2, SCR_W/2, SCR_H/2);
#else
	setRECT(mesh.clips, -SCR_X, -SCR_Y, SCR_W, SCR_H);
#endif	
	
	/* notify clip paraters to divPolyFT4() */
	divPolyClip(mesh.clips, mesh.size, mesh.ndiv);

	/* display start */
	SetDispMask(1);		
	
	/* main loop */
	while (((padd = PadRead(1))&PADselect) == 0) {
		
		cdb = (cdb==db)? db+1: db;	/* swap double buffer ID */
		ClearOTagR(cdb->ot, OTSIZE);	/* clear ordering table */
		
		cnt = VSync(1);
		
		/* get world clip window */
		areaClipZ(mesh.clips, mesh.clipw, genv.fog_far);

		/*set map offset */
		mesh.ox = genv.home.vx&~(genv.rx-1);
		mesh.oy = genv.home.vy&~(genv.ry-1);

		/* rot-trans-pers MESH with subdivision */
		heap = meshRotTransPers(cdb->ot, OTSIZE, &mesh, cdb->heap);

		FntPrint("t=%d,poly=%d\n", VSync(1)-cnt, heap-cdb->heap);
		
		/* read controller and set parameters to genv */
		padRead(&genv);

		/* flush geometry */
		geomUpdate(&genv);

		/* limit home position (z > 0) */
		if (genv.home.vz < SCR_Y) genv.home.vz = SCR_Y;
		
		/* change config */
		padd = PadRead(1);
		
		if (padd&PADstart) 
			setMeshConfig(&mesh, &genv);
		
		/* debug */
		if (mesh.clips->w != SCR_W) {
			disp_clips(mesh.clips);
			disp_clipw(mesh.clipw);
		}

		/* swap double buffer */
		db_swap(cdb->ot+OTSIZE-1);
	}
	
	/* end */
	DrawSync(0);
	return;
}
