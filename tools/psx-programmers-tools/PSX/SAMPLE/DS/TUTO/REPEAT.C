/* $PSLibId: Run-time Library Release 4.4$ */
/*
 *			repeat: CD-DA/XA repeat
 *
 *	Copyright (C) 1994 by Sony Computer Entertainment
 *			All rights Reserved
 *
 *		 Version	Date		Design
 *		-----------------------------------------
 *		1.00		Sep.12,1994	suzu
 *		1.10		Oct,24,1994	suzu
 *		2.00		Feb,02,1995	suzu
 *		3.00		Apr.22,1997	makoto
*/
/*		     ���s�[�g�Đ����C�u�����T���v��
 *	    CD-DA/XA �g���b�N�̔C�ӂ̂Q�_�Ԃ��I�[�g���s�[�g����B
 *--------------------------------------------------------------------------
 * dsRepeat	CD-DA �̎������s�[�g�Đ�������B
 *
 * �`��		int dsRepeat(int startp, int endp)
 *
 * ����		startp	���t�J�n�ʒu
 *		endp	���t�I���ʒu
 *
 * ���		startp �� endp �Ŏw�肳�ꂽ�Ԃ� CD-DA �f�[�^���J��Ԃ�
 *		�ăo�b�N�O���E���h�ōĐ�����B
 *
 * �Ԃ�l	�˂� 0
 *
 * ���l		�����̈ʒu���o�́A���|�[�g���[�h���g�p���Ă��邽�ߍ���
 *
 *--------------------------------------------------------------------------
 * dsRepeatXA	CD-XA �̎������s�[�g�Đ�������B
 *
 * �`��		int dsRepeatXA(int startp, int endp)
 *
 * ����		startp	���t�J�n�ʒu
 *		endp	���t�I���ʒu
 *
 * ���		startp �� endp �Ŏw�肳�ꂽ�Ԃ� CD-XA �f�[�^���J��Ԃ�
 *		�ăo�b�N�O���E���h�ōĐ�����B
 *
 * �Ԃ�l	�˂� 0
 *
 * ���l		�����̈ʒu���o�́AVSyncCallback() ���g�p���čs�Ȃ��̂ŁA
 *		����̂ŁA���̃\�[�X�R�[�h�����̂܂܎g�p����ꍇ�͒���
 *		���邱�ƁB
 *		�Đ��͔{���x XA �̂݁B
 *		�}���`�`���l�����g�p����ꍇ�́A�O������ DslSetfilter
 *		���g�p���ă`���l�����w�肵�Ă������ƁB
 *--------------------------------------------------------------------------
 * dsGetPos	���ݍĐ����̈ʒu��m��
 *
 * �`��		int dsGetPos(void)
 *
 * ����		�Ȃ�
 *
 * ���		���ݍĐ����̈ʒu�i�Z�N�^�ԍ��j�𒲂ׂ�B
 *
 * �Ԃ�l	���ݍĐ����̃Z�N�^�ԍ�
 *
 *--------------------------------------------------------------------------
 * dsGetRepPos	���݂܂ł̌J��Ԃ��񐔂𒲂ׂ�
 *
 * �`��		int dsGetRepTime()
 *
 * ����		�Ȃ�
 *
 * ���		���݂܂ł̌J��Ԃ��񐔂𒲂ׂ�B�^�C���A�E�g�ɂ��G���[
 *		�̌��o�Ɏg�p����B
 *
 * �Ԃ�l	���݂܂ł̌J��Ԃ���
 *--------------------------------------------------------------------------
 */

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libds.h>

/* �A���_�[�t���[�������̕ی� */
#define SP_MARGIN	( 4 * 75 )	/* 4sec */

/* �|�[�����O�Ԋu */
#define	XA_FREQ		32

static int StartPos, EndPos;	/* �J�n�E�I���ʒu */
static int CurPos;		/* ���݈ʒu */
static int RepTime;		/* �J��Ԃ��� */

/* DslDataReady �������̃R�[���o�b�N */
static void cbready( u_char intr, u_char* result );

/* VSync �R�[���o�b�N */
static void cbvsync( void );

/* DslGetlocP �Ŏg�p����R�[���o�b�N */
static void cbsync( u_char intr, u_char* result );

static int dsplay( u_char mode, u_char com );

int dsRepeat( int startp, int endp )
{
	StartPos = startp;
	EndPos = endp;
	CurPos = StartPos;
	RepTime = 0;

	DsReadyCallback( cbready );
	dsplay( DslModeRept | DslModeDA, DslPlay );

	return 0;
}

int dsRepeatXA( int startp, int endp )
{
	StartPos = startp;
	EndPos = endp;
	CurPos = StartPos;
	RepTime = 0;

	VSyncCallback( cbvsync );
	dsplay( DslModeSpeed | DslModeRT | DslModeSF, DslReadS );

	return 0;
}

int dsGetPos( void )
{
	return CurPos;
}

int dsGetRepTime( void )
{
	return RepTime;
}

/* dsRepeat() �Ŏg�p����R�[���o�b�N */
static void cbready( u_char intr, u_char* result )
{
	DslLOC loc;

	if( intr == DslDataReady ) {
		if( ( result[ 4 ] & 0x80 ) == 0 ) {
			loc.minute = result[ 3 ];
			loc.second = result[ 4 ];
			loc.sector = 0;
			CurPos = DsPosToInt( &loc );
		}
		if( CurPos > EndPos || CurPos < StartPos - SP_MARGIN )
			dsplay( DslModeRept | DslModeDA, DslPlay );
	} else
		dsplay( DslModeRept | DslModeDA, DslPlay );
}

/* dsRepeatXA() �Ŏg�p����R�[���o�b�N */
static void cbvsync( void )
{
	if( VSync( -1 ) % XA_FREQ )
		return;

	if( CurPos > EndPos || CurPos < StartPos - SP_MARGIN )
		dsplay( DslModeSpeed | DslModeRT | DslModeSF, DslReadS );
	else
		DsCommand( DslGetlocP, 0, cbsync, 0 );
}

/* DslGetlocP �Ŏg�p����R�[���o�b�N */
static void cbsync( u_char intr, u_char* result )
{
	int cnt;

	if( intr == DslComplete ) {
		cnt = DsPosToInt( ( DslLOC* )&result[ 5 ] );
		if( cnt > 0 )
			CurPos = cnt;
	}
}

static int dsplay( u_char mode, u_char com )
{
	DslLOC loc;
	int id;

	DsIntToPos( StartPos, &loc );
	id = DsPacket( mode, &loc, com, 0, -1 );
	CurPos = StartPos;
	RepTime++;
	return id;
}