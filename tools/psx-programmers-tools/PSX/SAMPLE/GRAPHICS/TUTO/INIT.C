/* $PSLibId: Run-time Library Release 4.4$ */
/*				init
 *
 *         Copyright (C) 1993-1995 by Sony Computer Entertainment
 *			All rights Reserved
 */
/* �O���t�B�b�N�������������� */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

/* int x, y;	/* GTE �I�t�Z�b�g	*/
/* int z;	/* �X�N���[���܂ł̋���	*/
/* int level;	/* �f�o�b�O���x��	*/

extern char	*progname;

void init_system(int x, int y, int z, int level)
{
	/* �`��E�\�����̃��Z�b�g */
	FntLoad(960, 256);	
	SetDumpFnt(FntOpen(32, 32, 320, 64, 0, 512));
	
	/* �O���t�B�b�N�V�X�e���̃f�o�b�O (0:�Ȃ�, 1:�`�F�b�N, 2:�v�����g) */
	SetGraphDebug(level);	

	/* �f�s�d�̏����� */
	InitGeom();			
	
	/* �I�t�Z�b�g�̐ݒ� */
	SetGeomOffset(x, y);	
	
	/* ���_����X�N���[���܂ł̋����̐ݒ� */
	SetGeomScreen(z);		
}