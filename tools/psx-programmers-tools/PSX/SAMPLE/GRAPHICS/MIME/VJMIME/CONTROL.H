/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *		Copyright (C) 1994 by Sony Corporation
 *			All rights Reserved
 *		
 *		"control.h"
*/

#define CTLMAX 	10	/* ����g�`�̍ő�`�����l���� */

#define CTLTIME 32	/* �`�B�֐��g�`�̍ő咷�� */

/* ����g�`�����v���O�����p�\���� */
typedef struct {
	int  *cnv;	/* �`�B�֐��g�`�f�[�^�̃|�C���^ */

	int num;	/* �`�B�֐��g�`�f�[�^�̒��� */

	int in;		/* ���� */
	int out;	/* �o�� */

} CTLFUNC;

extern CTLFUNC ctlfc[];