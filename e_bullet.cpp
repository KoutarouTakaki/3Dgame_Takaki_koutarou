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
#include "e_bullet.h"
#include "enemy.h"
#include "player.h"
#include "collision.h"
#include "shadow.h"
#include "lookon.h"
#include "boost.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_EBULLET			"data/MODEL/bullet_001.obj"		// �ǂݍ��ރ��f����

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

static EBULLET						g_Ebullet[EBULLET_MAX];				// �|���S���f�[�^
static int							g_TexNo;				// �e�N�X�`���ԍ�

static char* g_TextureName[] = {
	"data/TEXTURE/field003.jpg",
	"data/TEXTURE/bakuha.jpg",
};

int EcntMax = 100;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitEbullet(void)
{
	// ���_�o�b�t�@�̍쐬
	//MakeVertexStraight();
	LoadModel(MODEL_EBULLET, &g_Ebullet[0].model);

	for (int i = 0; i < EBULLET_MAX; i++)
	{
		g_Ebullet[i].load = TRUE;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Ebullet[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ebullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ebullet[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);
		g_Ebullet[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ebullet[i].Bmove = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Ebullet[i].use = FALSE;
		g_Ebullet[i].size;				// �����蔻��̑傫��

		g_Ebullet[i].radian = 0.0f;
		g_Ebullet[i].count = 0;

		// ���f���̐F��ύX�ł����I�������ɂ��ł����B
		for (int j = 0; j < g_Ebullet[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Ebullet[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		}
	}

	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEbullet(void)
{
	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_Ebullet[i].load)
		{
			g_Ebullet[i].load = FALSE;
		}
	}
	if (g_Ebullet[0].load == TRUE)UnloadModel(&g_Ebullet[0].model);

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEbullet(void)
{
	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_Ebullet[i].load == FALSE) return;
		int bulletCount = 0;				// ���������o���b�g�̐�

		for (int i = 0; i < EBULLET_MAX; i++)
		{
			if (g_Ebullet[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
			{								// Yes


				g_Ebullet[i].count += 1.0f;    // ���Ԃ�i�߂Ă���
				if ((int)g_Ebullet[i].count % 20 == 0)
				{
					g_Ebullet[i].pos.x += g_Ebullet[i].move.x;
					g_Ebullet[i].pos.y += g_Ebullet[i].move.y;
					g_Ebullet[i].pos.z += g_Ebullet[i].move.z;


					float fSize = (float)(rand() % 2 + 1);
					int nLife = rand() % 3 + 6;
					for (int j = 0; j < 3; j++)
					{
						SetBoost(g_Ebullet[i].pos, g_Ebullet[i].Bmove, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), fSize, fSize, nLife);
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
void DrawEbullet(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_Ebullet[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Ebullet[i].scl.x, g_Ebullet[i].scl.y, g_Ebullet[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Ebullet[i].rot.x, g_Ebullet[i].rot.y + XM_PI, g_Ebullet[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Ebullet[i].pos.x, g_Ebullet[i].pos.y, g_Ebullet[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Ebullet[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Ebullet[0].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


void SetEbullet(XMFLOAT3 spos, XMFLOAT3 epos)
{
	PrintDebugProc("bullet\n");

	ENEMY* enemy = GetEnemy();
	PLAYER* player = GetPlayer();
	CAMERA* cam = GetCamera();
	int n = GetNearEnemy();

	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_Ebullet[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Ebullet[i].use = TRUE;			// �g�p��Ԃ֕ύX����

			g_Ebullet[i].sPoint[0] = XMLoadFloat3(&spos);//�n�_

			for (int j = 0; j < EBULLET_MAX; j++)
			{
				if (j != i)continue;
				else g_Ebullet[j].sPoint[1] = XMLoadFloat3(&epos);//1

			}

			//���K��
			XMVECTOR dir = g_Ebullet[i].sPoint[1] - g_Ebullet[i].sPoint[0];
			dir = XMVector3NormalizeEst(dir);


			g_Ebullet[i].pos = spos;			// ���W���Z�b�g

			g_Ebullet[i].vec = dir;
			XMStoreFloat3(&g_Ebullet[i].move, g_Ebullet[i].vec);
			g_Ebullet[i].vec = XMVectorNegate(g_Ebullet[i].vec);
			XMStoreFloat3(&g_Ebullet[i].Bmove, g_Ebullet[i].vec);


			g_Ebullet[i].count = 0;

			g_Ebullet[i].radian = atan2f(g_Ebullet[i].pos.x - player->pos.x, g_Ebullet[i].pos.z - player->pos.z);

			g_Ebullet[i].rot.x = XM_PI / 6 * 4;
			g_Ebullet[i].rot.y = g_Ebullet[i].radian;// - (90.0f * 3.14f / 180.0f);
			return;							// 1���Z�b�g�����̂ŏI������
		}
		else
		{
			continue;
		}
	}
}

EBULLET* GetEbullet(void)
{
	return &(g_Ebullet[0]);
}
