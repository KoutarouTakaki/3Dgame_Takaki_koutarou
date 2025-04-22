//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "shadow.h"
#include "player.h"
#include "e_bullet.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY		"data/MODEL/robot/robo_body.obj"	// �ǂݍ��ރ��f����

#define	MODEL_ENEMY_HIP			"data/MODEL/robot/robo_hip.obj"			// �ǂݍ��ރ��f���� 0
#define	MODEL_ENEMY_RARM_DOWN	"data/MODEL/robot/robo_Rarm_down.obj"	// �ǂݍ��ރ��f���� 1
#define	MODEL_ENEMY_RARM_UP		"data/MODEL/robot/robo_Rarm_up.obj"		// �ǂݍ��ރ��f���� 2
#define	MODEL_ENEMY_RHAND		"data/MODEL/robot/robo_Rhand.obj"		// �ǂݍ��ރ��f���� 3
#define	MODEL_ENEMY_LARM_DOWN	"data/MODEL/robot/robo_Larm_down.obj"	// �ǂݍ��ރ��f���� 4
#define	MODEL_ENEMY_LARM_UP		"data/MODEL/robot/robo_Larm_up.obj"		// �ǂݍ��ރ��f���� 5
#define	MODEL_ENEMY_LHAND		"data/MODEL/robot/robo_Lhand.obj"		// �ǂݍ��ރ��f���� 6
#define	MODEL_ENEMY_RLEG_DOWN	"data/MODEL/robot/robo_Rleg_down.obj"	// �ǂݍ��ރ��f���� 7
#define	MODEL_ENEMY_RLEG_UP		"data/MODEL/robot/robo_Rleg_up.obj"		// �ǂݍ��ރ��f���� 8
#define	MODEL_ENEMY_LLEG_DOWN	"data/MODEL/robot/robo_Lleg_down.obj"	// �ǂݍ��ރ��f���� 9
#define	MODEL_ENEMY_LLEG_UP		"data/MODEL/robot/robo_Lleg_up.obj"		// �ǂݍ��ރ��f���� 10

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(7.0f)						// �e�̑傫��
#define ENEMY_OFFSET_Y		(50.0f)						// �G�l�~�[�̑��������킹��

#define ENEMY_PARTS_MAX	(12)								// �v���C���[�̃p�[�c�̐�
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// �G�l�~�[
static ENEMY			g_Parts[MAX_ENEMY][ENEMY_PARTS_MAX];		// �v���C���[�̃p�[�c�p

int g_Enemy_load = 0;

// �G�l�~�[�̊K�w�A�j���[�V�����f�[�^
// �G�l�~�[�̍��E�p�[�c�𓮂����Ă���A�j���f�[�^
static INTERPOLATION_DATA move_tbl_hip[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,-12.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f,-12.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Rarm_up[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-7.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 4, 0.0f, XM_PI / 2),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-7.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, XM_PI / 2),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Rarm_down[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Rhand[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Larm_up[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(7.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f,-XM_PI / 2),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(7.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 4, 0.0f,-XM_PI / 2),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Larm_down[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Lhand[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Rleg_up[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-3.5f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-3.5f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 4, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_Rleg_down[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,-6.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f,-6.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_Lleg_up[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(3.5f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 4, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(3.5f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_Lleg_down[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,-6.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f,-6.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_cam[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(10.0f, 10.0f,-10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(10.0f, 10.0f,-10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA* g_MoveTblAdrPart[] =
{
	move_tbl_hip,
	move_tbl_Rarm_up,
	move_tbl_Rarm_down,
	move_tbl_Rhand,
	move_tbl_Larm_up,
	move_tbl_Larm_down,
	move_tbl_Lhand,
	move_tbl_Rleg_up,
	move_tbl_Rleg_down,
	move_tbl_Lleg_up,
	move_tbl_Lleg_down,
	move_tbl_cam,
};

float TexBlendRate = 1.0f;
int atkMax = 100;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	TexBlendRate = 1.0f;
	atkMax = 100;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(0.0f, 100.0f, 0.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

		g_Enemy[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Enemy[i].size = ENEMY_SIZE;	// �����蔻��̑傫��
		g_Enemy[i].hp = 10000;
		g_Enemy[i].atkCnt = 0;

		g_Enemy[i].dissUse = FALSE;
		g_Enemy[i].dissCnt = 0;
		g_Enemy[i].TexUV = XMFLOAT2(0.0f, 0.0f);
		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
		g_Enemy[i].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
		g_Enemy[i].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g

		g_Enemy[i].use = TRUE;			// TRUE:�����Ă�

		// �K�w�A�j���[�V�����p�̏���������
		g_Enemy[i].parent = NULL;			// �{�́i�e�j�Ȃ̂�NULL������

		// �p�[�c�̏�����
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_Parts[i][j].use = FALSE;

			// �ʒu�E��]�E�X�P�[���̏����ݒ�
			g_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// �e�q�֌W
			g_Parts[i][j].parent = &g_Enemy[i];		// �� �����ɐe�̃A�h���X������
			//	g_Parts[�r].parent= &g_Enemy[i];		// �r��������e�͖{�́i�v���C���[�j
			//	g_Parts[��].parent= &g_Paerts[�r];	// �w���r�̎q���������ꍇ�̗�

				// �K�w�A�j���[�V�����p�̃����o�[�ϐ��̏�����
			g_Parts[i][j].time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
			g_Parts[i][j].tblNo = 0;			// �Đ�����s���f�[�^�e�[�u��No���Z�b�g
			g_Parts[i][j].tblMax = 0;			// �Đ�����s���f�[�^�e�[�u���̃��R�[�h�����Z�b�g
			g_Parts[i][j].dissCnt = 0.0f;
			// �p�[�c�̓ǂݍ��݂͂܂����Ă��Ȃ�
			g_Parts[i][j].load = 0;
		}

		g_Parts[i][0].use = TRUE;
		g_Parts[i][0].parent = &g_Enemy[i];	// �e���Z�b�g
		g_Parts[i][0].tblNo = 0;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][0].tblMax = sizeof(move_tbl_hip) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][0].load = 1;
		LoadModel(MODEL_ENEMY_HIP, &g_Parts[i][0].model);

		g_Parts[i][1].use = TRUE;
		g_Parts[i][1].parent = &g_Enemy[i];	// �e���Z�b�g
		g_Parts[i][1].tblNo = 1;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][1].tblMax = sizeof(move_tbl_Rarm_up) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][1].load = 1;
		LoadModel(MODEL_ENEMY_RARM_UP, &g_Parts[i][1].model);

		g_Parts[i][2].use = TRUE;
		g_Parts[i][2].parent = &g_Parts[i][1];	// �e���Z�b�g
		g_Parts[i][2].tblNo = 2;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][2].tblMax = sizeof(move_tbl_Rarm_down) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][2].load = 1;
		LoadModel(MODEL_ENEMY_RARM_DOWN, &g_Parts[i][2].model);

		g_Parts[i][3].use = TRUE;
		g_Parts[i][3].parent = &g_Parts[i][2];	// �e���Z�b�g
		g_Parts[i][3].tblNo = 3;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][3].tblMax = sizeof(move_tbl_Rhand) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][3].load = 1;
		LoadModel(MODEL_ENEMY_RHAND, &g_Parts[i][3].model);

		g_Parts[i][4].use = TRUE;
		g_Parts[i][4].parent = &g_Enemy[i];	// �e���Z�b�g
		g_Parts[i][4].tblNo = 4;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][4].tblMax = sizeof(move_tbl_Larm_up) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][4].load = 1;
		LoadModel(MODEL_ENEMY_LARM_UP, &g_Parts[i][4].model);

		g_Parts[i][5].use = TRUE;
		g_Parts[i][5].parent = &g_Parts[i][4];	// �e���Z�b�g
		g_Parts[i][5].tblNo = 5;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][5].tblMax = sizeof(move_tbl_Larm_down) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][5].load = 1;
		LoadModel(MODEL_ENEMY_LARM_DOWN, &g_Parts[i][5].model);

		g_Parts[i][6].use = TRUE;
		g_Parts[i][6].parent = &g_Parts[i][5];// �e���Z�b�g
		g_Parts[i][6].tblNo = 6;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][6].tblMax = sizeof(move_tbl_Lhand) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][6].load = 1;
		LoadModel(MODEL_ENEMY_LHAND, &g_Parts[i][6].model);

		g_Parts[i][7].use = TRUE;
		g_Parts[i][7].parent = &g_Parts[i][0];	// �e���Z�b�g
		g_Parts[i][7].tblNo = 7;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][7].tblMax = sizeof(move_tbl_Rleg_up) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][7].load = 1;
		LoadModel(MODEL_ENEMY_RLEG_UP, &g_Parts[i][7].model);

		g_Parts[i][8].use = TRUE;
		g_Parts[i][8].parent = &g_Parts[i][7];	// �e���Z�b�g
		g_Parts[i][8].tblNo = 8;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][8].tblMax = sizeof(move_tbl_Rleg_down) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][8].load = 1;
		LoadModel(MODEL_ENEMY_RLEG_DOWN, &g_Parts[i][8].model);

		g_Parts[i][9].use = TRUE;
		g_Parts[i][9].parent = &g_Parts[i][0];	// �e���Z�b�g
		g_Parts[i][9].tblNo = 9;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][9].tblMax = sizeof(move_tbl_Lleg_up) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][9].load = 1;
		LoadModel(MODEL_ENEMY_LLEG_UP, &g_Parts[i][9].model);

		g_Parts[i][10].use = TRUE;
		g_Parts[i][10].parent = &g_Parts[i][9];	// �e���Z�b�g
		g_Parts[i][10].tblNo = 10;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Parts[i][10].tblMax = sizeof(move_tbl_Lleg_down) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g
		g_Parts[i][10].load = 1;
		LoadModel(MODEL_ENEMY_LLEG_DOWN, &g_Parts[i][10].model);


		// ���f���̐F��ύX�ł����I�������ɂ��ł����B
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		}

	}
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;

		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	PLAYER* player = GetPlayer();

#ifdef _DEBUG
	if (GetKeyboardPress(DIK_5))
	{
		TexBlendRate -= 0.01f;
		if (TexBlendRate <= 0.0f)TexBlendRate = 0.0f;
	}
	if (GetKeyboardPress(DIK_6))
	{
		TexBlendRate += 0.01f;
		if (TexBlendRate >= 1.0f)TexBlendRate = 1.0f;
	}
	if (GetKeyboardPress(DIK_7))
	{

		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_Parts[0][j].dissCnt += 0.01f;
		}
	}
	PrintDebugProc("TexBlendRate%f\n", TexBlendRate);
#endif


	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes
			float radian = atan2f((player->pos.x - g_Enemy[i].pos.x), (player->pos.z - g_Enemy[i].pos.z));
			g_Enemy[i].rot.y = radian + (180.0f * 3.14f / 180.0f);//�v���C���[�̕�������

			g_Enemy[i].atkCnt++;
			if (g_Enemy[i].atkCnt % atkMax == 0 && g_Enemy[i].hp > 0)
			{
				XMFLOAT3 pos = g_Enemy[i].pos;
				pos.x -= sinf(g_Enemy[i].rot.y + XM_PI / 2) * 20.0f;
				pos.y += 20.0f;
				pos.z -= cosf(g_Enemy[i].rot.y + XM_PI / 2) * 20.0f;

				SetEbullet(pos, player->pos);

				pos = g_Enemy[i].pos;
				pos.x -= sinf(g_Enemy[i].rot.y - XM_PI / 2) * 20.0f;
				pos.y += 20.0f;
				pos.z -= cosf(g_Enemy[i].rot.y - XM_PI / 2) * 20.0f;

				SetEbullet(pos, player->pos);
			}

			// �e�N�X�`���u�����h�p�i�u�����h�p�̃e�N�X�`���̍��W��ς��Ă���j
			g_Enemy[i].TexUV.x += 0.001f;
			g_Enemy[i].TexUV.y += 0.001f;

			// �K�w�A�j���[�V����
			for (int j = 0; j < ENEMY_PARTS_MAX; j++)
			{
				// �g���Ă���Ȃ珈������
				//if (g_Parts[i][j].use == TRUE)
				if ((g_Parts[i][j].use == TRUE) && (g_Parts[i][j].tblMax > 0))
				{	// ���`��Ԃ̏���
					int nowNo = (int)g_Parts[i][j].time;			// �������ł���e�[�u���ԍ������o���Ă���
					int maxNo = g_Parts[i][j].tblMax;				// �o�^�e�[�u�����𐔂��Ă���
					int nextNo = (nowNo + 1) % maxNo;			// �ړ���e�[�u���̔ԍ������߂Ă���
					INTERPOLATION_DATA* tbl = g_MoveTblAdrPart[g_Parts[i][j].tblNo];	// �s���e�[�u���̃A�h���X���擾

					XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTOR�֕ϊ�
					XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTOR�֕ϊ�
					XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTOR�֕ϊ�

					XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ�ړ��ʂ��v�Z���Ă���
					XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ��]�ʂ��v�Z���Ă���
					XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ�g�嗦���v�Z���Ă���

					float nowTime = g_Parts[i][j].time - nowNo;	// ���ԕ����ł��鏭�������o���Ă���

					Pos *= nowTime;								// ���݂̈ړ��ʂ��v�Z���Ă���
					Rot *= nowTime;								// ���݂̉�]�ʂ��v�Z���Ă���
					Scl *= nowTime;								// ���݂̊g�嗦���v�Z���Ă���

					// �v�Z���ċ��߂��ړ��ʂ����݂̈ړ��e�[�u��XYZ�ɑ����Ă��遁�\�����W�����߂Ă���
					XMStoreFloat3(&g_Parts[i][j].pos, nowPos + Pos);

					// �v�Z���ċ��߂���]�ʂ����݂̈ړ��e�[�u���ɑ����Ă���
					XMStoreFloat3(&g_Parts[i][j].rot, nowRot + Rot);

					// �v�Z���ċ��߂��g�嗦�����݂̈ړ��e�[�u���ɑ����Ă���
					XMStoreFloat3(&g_Parts[i][j].scl, nowScl + Scl);

					// frame���g�Ď��Ԍo�ߏ���������
					g_Parts[i][j].time += 1.0f / tbl[nowNo].frame;	// ���Ԃ�i�߂Ă���
					if ((int)g_Parts[i][j].time >= maxNo)			// �o�^�e�[�u���Ō�܂ňړ��������H
					{
						g_Parts[i][j].time -= maxNo;				// �O�ԖڂɃ��Z�b�g�������������������p���ł���
					}

					//�f�B�]���u***********************************************************************************************
					//HP�ɂ���ăf�B�]���u�ƃe�N�X�`���u�����h��i�߂�
					if (g_Enemy[i].hp < 9000)
					{
						g_Parts[i][10].dissCnt += 0.01f;
						TexBlendRate = 0.9f;
					}
					if (g_Enemy[i].hp < 8000)
					{
						g_Parts[i][9].dissCnt += 0.01f;
						TexBlendRate = 0.8f;
						atkMax = 95;
					}
					if (g_Enemy[i].hp < 7000)
					{
						g_Parts[i][8].dissCnt += 0.01f;
						TexBlendRate = 0.7f;
						atkMax = 90;
					}
					if (g_Enemy[i].hp < 6500)
					{
						g_Parts[i][7].dissCnt += 0.01f;
						TexBlendRate = 0.6f;
						atkMax = 85;
					}
					if (g_Enemy[i].hp < 6000)
					{
						g_Parts[i][6].dissCnt += 0.01f;
						TexBlendRate = 0.5f;
						atkMax = 80;
					}
					if (g_Enemy[i].hp < 5500)
					{
						g_Parts[i][5].dissCnt += 0.01f;
						TexBlendRate = 0.4f;
						atkMax = 70;
					}
					if (g_Enemy[i].hp < 5000)
					{
						g_Parts[i][4].dissCnt += 0.01f;
						TexBlendRate = 0.3f;
						atkMax = 60;
					}
					if (g_Enemy[i].hp < 4000)
					{
						g_Parts[i][3].dissCnt += 0.01f;
						TexBlendRate = 0.25f;
						atkMax = 50;
					}
					if (g_Enemy[i].hp < 3000)
					{
						g_Parts[i][2].dissCnt += 0.01f;
						TexBlendRate = 0.2f;
						atkMax = 40;
					}
					if (g_Enemy[i].hp < 2000)
					{
						g_Parts[i][1].dissCnt += 0.01f;
						TexBlendRate = 0.15f;
						atkMax = 30;
					}
					if (g_Enemy[i].hp < 1000)
					{
						g_Parts[i][0].dissCnt += 0.01f;
						TexBlendRate = 0.1f;
						atkMax = 20;
					}
					if (g_Enemy[i].hp <= 0)
					{
						g_Enemy[i].dissCnt += 0.01f;
						TexBlendRate = 0.0f;
						atkMax = 10;
					}

					if (g_Parts[i][j].dissCnt != 0.0f)
					{
						g_Parts[i][j].dissCnt += 0.005f;
					}
					if (g_Parts[i][j].dissCnt >= 1.0f)
					{
						g_Parts[i][j].use = FALSE;
					}
				}
			}

			// �e���v���C���[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);

			if (g_Enemy[i].dissCnt >= 1.0f || g_Enemy[i].hp <= 0.0f)
			{
				g_Enemy[i].use = FALSE;
			}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		if (i == 0)SetFuchi(1, g_Enemy[i].TexUV, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

		SetDissolve(1, g_Enemy[0].dissCnt);

		SetTexBrend(1, TexBlendRate, g_Enemy[i].TexUV);
		//SetTexBrend(1, TexBlendRate, 0);

		// ���f���`��
		DrawModel(&g_Enemy[i].model);

		SetTexBrend(0, TexBlendRate, g_Enemy[i].TexUV);
		//SetTexBrend(1, TexBlendRate, 0);

		SetDissolve(0, 0.0f);

		//
		SetFuchi(1, g_Enemy[i].TexUV, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

		// �p�[�c�̊K�w�A�j���[�V����
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Parts[i][j].scl.x, g_Parts[i][j].scl.y, g_Parts[i][j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i][j].rot.x, g_Parts[i][j].rot.y, g_Parts[i][j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Parts[i][j].pos.x, g_Parts[i][j].pos.y, g_Parts[i][j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[i][j].parent != NULL)	// �q����������e�ƌ�������
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i][j].parent->mtxWorld));
				// ��
				// g_Enemy[i].mtxWorld���w���Ă���
			}

			XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

			// �g���Ă���Ȃ珈������B�����܂ŏ������Ă��闝�R�͑��̃p�[�c�����̃p�[�c���Q�Ƃ��Ă���\�������邩��B
			if (g_Parts[i][j].use == FALSE) continue;

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			SetDissolve(1, g_Parts[i][j].dissCnt);

			SetTexBrend(1, TexBlendRate, g_Enemy[i].TexUV);

			// ���f���`��
			DrawModel(&g_Parts[i][j].model);

			SetTexBrend(0, TexBlendRate, g_Enemy[i].TexUV);

			SetDissolve(0, 0.0f);

		}
		SetFuchi(0, g_Enemy[i].TexUV, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	}


	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY* GetEnemy()
{
	return &g_Enemy[0];
}
