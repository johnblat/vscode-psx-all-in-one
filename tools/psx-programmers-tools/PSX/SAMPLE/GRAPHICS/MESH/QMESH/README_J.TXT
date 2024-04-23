$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
�Q�������b�V���p�֐�(RotMeshPrimQ_T)�̃T���v���v���O����

                      Copyright(C) 1997 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.


���T���v���̉����
	�Q�������b�V���f�[�^�����W�ϊ��A�����ϊ����p�P�b�g������n�s��
	�����N���܂��B�Q�������b�V���f�[�^��QMESH�\���̂Ŏw�肳��܂��B


		0---1---2---3---4---5---6---7---8
		|   |   |   |   |   |   |   |   |
		9---10--11--12--13--14--15--16--17
		|   |   |   |   |   |   |   |   |
		18--19--20--21--22--23--24--25--26
		|   |   |   |   |   |   |   |   |
		27--28--29--30--31--32--33--34--35

			�Q�������b�V���f�[�^


	typedef struct {
       		SVECTOR         *v;             /*shared vertices*/
        	SVECTOR         *n;             /*shared normals*/
        	SVECTOR         *u;             /*shared texture addresses*/
        	CVECTOR         *c;             /*shared colors*/
        	u_long          lenv;           /*mesh length_V(=#vertex_V)*/
        	u_long          lenh;           /*mesh length_H(=#vertex_H)*/
	} QMESH;

			QMESH�\����

	��̐}�ł�
		lenv = 4
		lenh = 9
		v,n,u,c�͏�̔ԍ����̔z��

	�T���v���v���O����tuto0�ł�
		lenh = 42
		lenv = 43
	�T���v���v���O����tuto1�ł�
		lenh = 42
		lenv = 46
	�ł��B
	�R�_�����v�Z�֐����g���Ă��邽��lenh�͂R�̔{���łȂ���΂Ȃ�܂���B
	RotMeshPrimQ_T�̓e�N�X�`�������̃��[�h�ł���̂Ō����v�Z��
	�f�v�X�L���[�C���O�ȊO�s�Ȃ��܂���B�]����QMESH��n,c�͖�������܂��B

	RotMeshPrimQ_T�͂���N���b�v�̈�̊O���ɂ���|���S����`�悵�Ȃ�
	�X�N���[���N���b�v���s�Ȃ��܂��B
	tuto0�̓N���b�v�̈���X�N���[����菭���������ݒ肵��
	�N���b�v�̗l�q���킩��₷�����Ă��܂��B
	tuto1�̓N���b�v�̈���X�N���[���ƈ�v�����Ă��܂��B

	�\������鐔���͂P�t���[�����̌v�Z���ԂŁA�P�ʂ͂g�ł��B

���N�����@��
        >psymake    	�R���p�C��
        >run tuto0       ���s

        >run tuto1       ���s

<������@>
	tuto0
	        ��	�FY���܂���]
       		��	�FZ���܂���]
        	��	�FY���܂���]
        	�~	�FZ���܂���]
        	��	�FX���������s�ړ�
        	��	�FX���������s�ړ�
        	��	�F�f�v�X�L���[�C���OON/OFF
        	��	�F�f�v�X�L���[�C���OON/OFF
        	L1	�FZ���������s�ړ�
        	L2	�Fbackface clip ON/OFF
        	R1	�FZ���������s�ړ�
        	R2	�Fbackface clip ON/OFF
        	SELECT	�F�I��

	tuto1
	        ��	�FY���܂���]
       		��	�F�f�v�X�L���[�C���OON
        	��	�FY���܂���]
        	�~	�F�f�v�X�L���[�C���OOFF
        	��	�FX���������s�ړ�
        	��	�FX���������s�ړ�
        	��	�FY���������s�ړ�
        	��	�FY���������s�ړ�
        	L1	�FZ���������s�ړ�
        	L2	�Fbackface clip ON/OFF
        	R1	�FZ���������s�ړ�
        	R2	�Fbackface clip ON/OFF
        	SELECT	�F�I��
