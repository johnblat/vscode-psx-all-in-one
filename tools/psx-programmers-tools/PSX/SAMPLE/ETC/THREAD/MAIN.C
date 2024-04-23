/* $PSLibId: Run-time Library Release 4.4$ */
/*        balls & thread:
 *
 *	  ��ʓ����o�E���h���镡���̃{�[����`�悵�����ƁA
 *	  �󂫎��ԂɁA�J�E���^�[���A�b�v����
 *
 *        Copyright  (C)  1996 by Sony Corporation
 *             All rights Reserved
 *
 *    Version  Date      Design
 *   -----------------------------------------
 *   1.00      Apr.25.1996    hakama   (based balls 4.00) 
 *   -----------------------------------------
 */

#include <r3000.h>
#include <asm.h>
#include <libapi.h>

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* �������v�����g���邽�߂̒�` */
#define KANJI

/*#define DEBUG */
/* �v���~�e�B�u�֘A�̃o�b�t�@ */
#define OTSIZE		1		/* �I�[�_�����O�e�[�u���̐� */
#define MAXOBJ		4000		/* �X�v���C�g�i�{�[���j���̏�� */
typedef struct {
	DRAWENV		draw;		/* �`��� */
	DISPENV		disp;		/* �\���� */
	u_long		ot[OTSIZE];	/* �I�[�_�����O�e�[�u�� */
	SPRT_16		sprt[MAXOBJ];	/* 16x16�Œ�T�C�Y�̃X�v���C�g */
} DB;

/* �X�v���C�g�̓����Ɋւ���o�b�t�@ */
typedef struct {
	u_short x, y;			/* ���݂̈ʒu */
	u_short dx, dy;			/* ���x */
} POS;

/* �\���̈� */
#define	FRAME_X		320		/* �\���̈�T�C�Y(320x240)*/
#define	FRAME_Y		240
#define WALL_X		(FRAME_X-16)	/* �X�v���C�g�̉��̈�T�C�Y */
#define WALL_Y		(FRAME_Y-16)

/* �v���~�e�B�u�o�b�t�@�̏����ݒ� */
static void init_prim(DB *db);	

/* �R���g���[���̉�� */
static int  pad_read(int n);	

/* V-Sync���̃R�[���o�b�N���[�`�� */
static void  cbvsync(void);	

/* �{�[���̃X�^�[�g�ʒu�ƈړ������̐ݒ� */
static int  init_point(POS *pos);

/* thread �p�̃f�[�^��` */
#define THREAD_SAMPLE

#ifdef THREAD_SAMPLE
/* thread �p�̃f�[�^��` */
#define SUB_STACK 0x80180000 /* sub-thread�p�̃X�^�b�N�B�K���ɕύX */
static volatile unsigned long count1,count2; /* �J�E���^*/
struct ToT *sysToT = (struct ToT *) 0x100 ; /* Table of Tabbles  */
struct TCBH *tcbh ; /* �^�X�N��ԃL���[�A�h���X*/
struct TCB *master_thp,*sub_thp; /* thread �R���e�N�X�g�̐擪�A�h���X*/
static long sub_func() ; /* sub thread �֐�*/
#endif

main()
{
	/* �{�[���̍��W�l�ƈړ��������i�[����o�b�t�@*/
	POS	pos[MAXOBJ];	
	
	/* �_�u���o�b�t�@�̂��߂Q�p�ӂ��� */
	DB	db[2];		
	
	/* ���݂̃_�u���o�b�t�@�o�b�t�@�̃A�h���X */
	DB	*cdb;		

	/* �\������X�v���C�g�̐��i�ŏ��͂P����j*/
	int	nobj = 1;	
	
	/* ���݂̂n�s�̃A�h���X */
	u_long	*ot;		
	
	SPRT_16	*sp;		/* work */
	POS	*pp;		/* work */
	int	i, cnt, x, y;	/* work */
	
#ifdef THREAD_SAMPLE
	/* thread data (local)*/
	unsigned long sub_th,gp;

	/* thread �p�̏�����*/

	/* �R���t�B�M�����[�V�����̂�蒼��*/
	/* ���̍�Ƃ́A�{���K�v�Ȃ��̂ł����A�Q�O�O�O���g�p���Ă����
	   �K�v�ɂȂ�܂��B�ڍׂ� 2190 �� \bata\setconf ���Q�Ƃ��Ă���
	   �����B������ SetConf2 �́Aversion3.4 �ł́ASetConf �ɂ�����
	   ����Ă��܂� */
	SetConf(16,4,0x80200000);

	/* �^�X�N��ԃL���[�A�h���X�� �� thread �R���e�N�X�g�A�h���X�̎擾*/
	tcbh = (struct TCBH *) sysToT[1].head;
	master_thp = tcbh->entry;

	/* sub thread �̍쐬*/
	/* ������́A���C�u�������t�@�����XDTL-S2150A vol-1 p25-p29*/
	/* vol-2 p22-p24 ���Q�Ƃ��Ă�������*/
	gp = GetGp();
	EnterCriticalSection();
	sub_th = OpenTh(sub_func, SUB_STACK, gp);
	ExitCriticalSection();
	/* sub thread �R���e�N�X�g�A�h���X�̎擾�Ɗ��荞�ݏ�Ԃ̐ݒ�*/
	sub_thp = (struct TCB *) sysToT[2].head + (sub_th & 0xffff);
	sub_thp->reg[R_SR] = 0x404;
#endif	
	/* �R���g���[���̃��Z�b�g*/
	PadInit(0);		
	
	/* �`��E�\�����̃��Z�b�g(0:cold,1:warm)  */
	ResetGraph(0);		
	
	/* �f�o�b�O���[�h�̐ݒ�(0:off,1:monitor,2:dump)*/
	SetGraphDebug(0);	
	
	/* V-sync���̃R�[���o�b�N�֐��̐ݒ�*/
	VSyncCallback(cbvsync);	

	/* �`��E�\�������_�u���o�b�t�@�p�ɐݒ�     
	(0,  0)-(320,240)�ɕ`�悵�Ă���Ƃ���(0,240)-(320,480)��\��(db[0])
	(0,240)-(320,480)�ɕ`�悵�Ă���Ƃ���(0,  0)-(320,240)��\��(db[1])
*/
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0,   0, 320, 240);

	/* �t�H���g�̐ݒ� */
#ifdef KANJI	/* KANJI */
	KanjiFntOpen(160, 16, 256, 200, 704, 0, 768, 256, 0, 512);
#endif	
	/* ��{�t�H���g�p�^�[�����t���[���o�b�t�@�Ƀ��[�h*/
	FntLoad(960, 256);	
	
	/* �t�H���g�̕\���ʒu�̐ݒ� */
	SetDumpFnt(FntOpen(16, 16, 256, 200, 0, 512));	

	/* �v���~�e�B�u�o�b�t�@�̏����ݒ�(db[0])*/
	init_prim(&db[0]);	
	
	/* �v���~�e�B�u�o�b�t�@�̏����ݒ�(db[1])*/
	init_prim(&db[1]);	
	
	/* �{�[���̓����Ɋւ��鏉���ݒ� */
	init_point(pos);	

	/* �f�B�X�v���C�ւ̕\���J�n */
	SetDispMask(1);		/* �O:�s��  �P:�� */

	/* ���C�����[�v*/
	while ((nobj = pad_read(nobj)) > 0) {
		/* �_�u���o�b�t�@�|�C���^�̐؂�ւ� */
		cdb  = (cdb==db)? db+1: db;	
#ifdef DEBUG
		/* dump DB environment */
		DumpDrawEnv(&cdb->draw);
		DumpDispEnv(&cdb->disp);
		DumpTPage(cdb->draw.tpage);
#endif

 		/* �I�[�_�����O�e�[�u���̃N���A */
		ClearOTag(cdb->ot, OTSIZE);

		/* �{�[���̈ʒu���P���v�Z���Ăn�s�ɓo�^���Ă��� */
		ot = cdb->ot;
		sp = cdb->sprt;
		pp = pos;
		for (i = 0; i < nobj; i++, sp++, pp++) {
			/* ���W�l�̍X�V����щ�ʏ�ł̈ʒu�̌v�Z */
			if ((x = (pp->x += pp->dx) % WALL_X*2) >= WALL_X)
				x = WALL_X*2 - x;
			if ((y = (pp->y += pp->dy) % WALL_Y*2) >= WALL_Y)
				y = WALL_Y*2 - y;

			/* �v�Z�������W�l���Z�b�g */
			setXY0(sp, x, y);	
			
			/* �n�s�֓o�^ */
			AddPrim(ot, sp);	
		}
		/* �`��̏I���҂� */
		DrawSync(0);		
		
		/* cnt = VSync(1);	/* check for count */
		/* cnt = VSync(2);	/* wait for V-BLNK (1/30) */
		/* cnt = VSync(0);	/* wait for V-BLNK (1/60) */
#ifdef THREAD_SAMPLE
		/* sub thread �ֈڂ�Bsub thread �ł́A�O�񊄂荞�݂�
		   ��������ꂽ�ꏊ����n�܂�܂��i�R���[�`���j�B */
		ChangeTh(sub_th);
#else
		cnt = VSync(0);		/* wait for V-BLNK (1/60) */
#endif		
		/* �_�u���o�b�t�@�̐ؑւ�*/
		/* �\�����̍X�V */
		PutDispEnv(&cdb->disp); 
		
		/* �`����̍X�V */
		PutDrawEnv(&cdb->draw); 
		
		/* �n�s�ɓo�^���ꂽ�v���~�e�B�u�̕`��*/
		DrawOTag(cdb->ot);	
#ifdef DEBUG
		DumpOTag(cdb->ot);
#endif
		/* �{�[���̐��ƌo�ߎ��Ԃ̃v�����g*/
#ifdef KANJI
		KanjiFntPrint("�ʂ̐���%d\n", nobj);
#ifdef THREAD_SAMPLE
		/* ���荞�݂܂ł̎��Ԃɉ��񃋁[�v�ł�������\������*/
		KanjiFntPrint("�󂫎���=%d\n", count2-count1);
#else
		KanjiFntPrint("����=%d\n", cnt);
#endif
		KanjiFntFlush(-1);
#endif
		
		FntPrint("sprite = %d\n", nobj);
#ifdef THREAD_SAMPLE
		/* ���荞�݂܂ł̊Ԃɉ��񃋁[�v�ł�������\������E���̂Q*/
		FntPrint("free time = %d\n", count2 - count1);
#else
		FntPrint("total time = %d\n", cnt);
#endif
		FntFlush(-1);
		
#ifdef THREAD_SAMPLE
		/* �J�E���^��Ҕ�*/
		count1 = count2;
#endif
	}
#ifdef THREAD_SAMPLE
	/* thread �̏���*/
	EnterCriticalSection();
	CloseTh(sub_th);
	ExitCriticalSection();
#endif
	PadStop();	/* �R���g���[���̃N���[�Y*/
	StopCallback();
	return(0);
}

/* �v���~�e�B�u�o�b�t�@�̏����ݒ� */
#include "balltex.h"	/* �{�[���̃e�N�X�`���p�^�[���������Ă���t�@�C�� */

/* �v���~�e�B�u�o�b�t�@*/
static void init_prim(DB *db)
{
	u_short	clut[32];		/* �e�N�X�`�� CLUT */
	SPRT_16	*sp;			/* work */
	int	i;			/* work */

	/* �w�i�F�̃Z�b�g */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 60, 120, 120);

	/* �e�N�X�`���̃��[�h */
	db->draw.tpage = LoadTPage(ball16x16, 0, 0, 640, 0, 16, 16);
#ifdef DEBUG
	DumpTPage(db->draw.tpage);
#endif
	/* �e�N�X�`�� CLUT�̃��[�h */
	for (i = 0; i < 32; i++) {
		clut[i] = LoadClut(ballcolor[i], 0, 480+i);
#ifdef DEBUG
		DumpClut(clut[i]);
#endif
	}

	/* �X�v���C�g�̏����� */
	for (sp = db->sprt, i = 0; i < MAXOBJ; i++, sp++) {
		/* 16x16�X�v���C�g�v���~�e�B�u�̏����� */
		SetSprt16(sp);		
		
		/* �����������I�t */
		SetSemiTrans(sp, 0);	
		
		/* �V�F�[�f�B���O���s��Ȃ� */
		SetShadeTex(sp, 1);	
		
		/* u,v��(0,0)�ɐݒ� */
		setUV0(sp, 0, 0);	
		
		/* CLUT �̃Z�b�g */
		sp->clut = clut[i%32];	
	}
}

/* �{�[���̃X�^�[�g�ʒu�ƈړ��ʂ�ݒ肷�� */

/* �{�[���̓����Ɋւ���\����*/
static init_point(POS *pos)	
{
	int	i;
	for (i = 0; i < MAXOBJ; i++) {
		pos->x  = rand();		/* �X�^�[�g���W �w*/
		pos->y  = rand();		/* �X�^�[�g���W �x*/
		pos->dx = (rand() % 4) + 1;	/* �ړ����� �w (1<=x<=4)*/
		pos->dy = (rand() % 4) + 1;	/* �ړ����� �x (1<=y<=4)*/
		pos++;
	}
}

/* �R���g���[���̉�� */
/* �X�v���C�g�̐�*/
static int pad_read(int	n)		
{
	u_long	padd = PadRead(1);		/* �R���g���[���̓ǂݍ���*/

	if(padd & PADLup)	n += 4;		/* ���̏\���L�[�A�b�v*/
	if(padd & PADLdown)	n -= 4;		/* ���̏\���L�[�_�E��*/

	if (padd & PADL1) 			/* pause */
		while (PadRead(1)&PADL1);

	if(padd & PADselect) 	return(-1);	/* �v���O�����̏I��*/

	limitRange(n, 1, MAXOBJ-1);	/* n��1<=n<=(MAXOBJ-1)�̒l�ɂ���*/
					/* libgpu.h�ɋL��*/
	return(n);
}

/* �R�[���o�b�N */

static void cbvsync(void)
{
	/* print absolute VSync count */
	FntPrint("V-BLNK(%d)\n", VSync(-1));
#ifdef THREAD_SAMPLE
	/* ���荞�݂̖߂�� main thread �ɐݒ肷��B��������Ȃ��ƁA
	   sub thread(sub_func() ��) �ɖ߂��Ă��܂��B */
	tcbh->entry = master_thp;
#endif
}

#ifdef THREAD_SAMPLE
/* �󂫎��Ԃ̊ԁA���[�v���ăJ�E���g�A�b�v����v���O�����B
   ChangeTh() �ŃR�[�����ꂽ�֐��� return ���Ă��߂�ꏊ���Ȃ��̂Œ��� */
static long sub_func()
{
  count1 = 0;
  count2 = 0;
  while(1){
    /* ���� while loop �̂ǂ����� Vsync���荞�݂�������A���䂪�����B
       ���� ChangeTh() ���ꂽ�Ƃ��́A��������n�܂�B */
    count2 ++;
  }
}
#endif