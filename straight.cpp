//=============================================================================
//
// �t�B�[���h�\������ [field.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "straight.h"
#include "enemy.h"
#include "player.h"
#include "collision.h"
#include "shadow.h"
#include "lookon.h"
#include "boost.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_STRAIGHT			"data/MODEL/bullet_001.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE		(5.0f)					// �ړ���
#define	VALUE_ROTATE	(XM_PI * 0.02f)			// ��]��

#define	SIZE_WH			(100.0f)				// �n�ʂ̃T�C�Y

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// ���_���

static STRAIGHT						g_Straight[STRAIGHT_MAX];				// �|���S���f�[�^
static int							g_TexNo;				// �e�N�X�`���ԍ�

static char* g_TextureName[] = {
	"data/TEXTURE/field003.jpg",
	"data/TEXTURE/bakuha.jpg",
};

int cntMax = 100;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitStraight(void)
{
	// ���_�o�b�t�@�̍쐬
	//MakeVertexStraight();
	LoadModel(MODEL_STRAIGHT, &g_Straight[0].model);

	for (int i = 0; i < STRAIGHT_MAX; i++)
	{
		g_Straight[i].load = TRUE;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Straight[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Straight[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Straight[i].scl = XMFLOAT3(0.1f, 0.1f, 0.1f);
		g_Straight[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Straight[i].Bmove = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Straight[i].use = FALSE;
		g_Straight[i].size;				// �����蔻��̑傫��

		g_Straight[i].radian = 0.0f;
		g_Straight[i].count = 0;

		// ���f���̐F��ύX�ł����I�������ɂ��ł����B
		for (int j = 0; j < g_Straight[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Straight[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		}
	}

	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitStraight(void)
{
	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < STRAIGHT_MAX; i++)
	{
		if (g_Straight[i].load)
		{
			g_Straight[i].load = FALSE;
		}
	}

	if (g_Straight[0].load == TRUE)	UnloadModel(&g_Straight[0].model);

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateStraight(void)
{
	for (int i = 0; i < STRAIGHT_MAX; i++)
	{
		if (g_Straight[i].load == FALSE) return;
		int bulletCount = 0;				// ���������o���b�g�̐�

		for (int i = 0; i < STRAIGHT_MAX; i++)
		{
			if (g_Straight[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
			{								// Yes

			g_Straight[i].count += 1;    // ���Ԃ�i�߂Ă���

			if ((int)g_Straight[i].count % 20 == 0)
			{
				g_Straight[i].pos.x += g_Straight[i].move.x;
				g_Straight[i].pos.y += g_Straight[i].move.y;
				g_Straight[i].pos.z += g_Straight[i].move.z;


				float fSize = (float)(rand() % 1 + 0.5);
				int nLife = rand() % 2 + 4;
				for (int j = 0; j < 3; j++)
				{
					SetBoost(g_Straight[i].pos, g_Straight[i].Bmove, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), fSize, fSize, nLife);
				}
			}


			bulletCount++;
			}
		}




//
//#ifdef _DEBUG	// �f�o�b�O����\������
// 
//#endif

	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawStraight(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < STRAIGHT_MAX; i++)
	{
		if (g_Straight[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Straight[i].scl.x, g_Straight[i].scl.y, g_Straight[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Straight[i].rot.x, g_Straight[i].rot.y + XM_PI, g_Straight[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Straight[i].pos.x, g_Straight[i].pos.y, g_Straight[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Straight[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Straight[0].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


void SetStraight(XMFLOAT3 spos, XMFLOAT3 epos)
{
	ENEMY* enemy = GetEnemy();
	PLAYER* player = GetPlayer();
	CAMERA* cam = GetCamera();
	int n = GetNearEnemy();

	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < STRAIGHT_MAX; i++)
	{
		if (g_Straight[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Straight[i].use = TRUE;			// �g�p��Ԃ֕ύX����

			g_Straight[i].sPoint[0] = XMLoadFloat3(&spos);//�n�_

			for (int j = 0; j < STRAIGHT_MAX; j++)
			{
				if (j != i)continue;
				else g_Straight[j].sPoint[1] = XMLoadFloat3(&epos);//1

			}

			//���K��
			XMVECTOR dir = g_Straight[i].sPoint[1] - g_Straight[i].sPoint[0];
			dir = XMVector3NormalizeEst(dir);


			g_Straight[i].pos = spos;			// ���W���Z�b�g

			g_Straight[i].vec = dir;
			XMStoreFloat3(&g_Straight[i].move, g_Straight[i].vec);
			g_Straight[i].vec = XMVectorNegate(g_Straight[i].vec);
			XMStoreFloat3(&g_Straight[i].Bmove, g_Straight[i].vec);

			g_Straight[i].count = 0;

			if (n == -1)g_Straight[i].radian = player->rot.y;// + XM_PI) + cam->rot.y;
			else g_Straight[i].radian = atan2f(g_Straight[i].pos.x - enemy[n].pos.x, g_Straight[i].pos.z - enemy[n].pos.z);

			g_Straight[i].rot.x = XM_PI / 2;
			g_Straight[i].rot.y = g_Straight[i].radian;// - (90.0f * 3.14f / 180.0f);
			return;							// 1���Z�b�g�����̂ŏI������
		}
		else
		{
			continue;
		}
	}
}

STRAIGHT* GetStraight(void)
{
	return &(g_Straight[0]);
}
