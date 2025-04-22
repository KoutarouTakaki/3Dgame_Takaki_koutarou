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
#include "beje.h"
#include "enemy.h"
#include "player.h"
#include "collision.h"
#include "shadow.h"
#include "lookon.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_BEJE			"data/MODEL/ball.obj"		// �ǂݍ��ރ��f����

#define	VALUE_MOVE		(5.0f)					// �ړ���
#define	VALUE_ROTATE	(XM_PI * 0.02f)			// ��]��

#define	SIZE_WH			(100.0f)				// �n�ʂ̃T�C�Y

#define SPEED			(0.01f)
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
//HRESULT MakeVertexStraight(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// ���_���

static BEJE						g_Beje[BEJE_MAX];				// �|���S���f�[�^
static int							g_TexNo;				// �e�N�X�`���ԍ�

static char* g_TextureName[] = {
	"data/TEXTURE/field003.jpg",
	"data/TEXTURE/bakuha.jpg",
};

static int cntMax = 100;
static int FiveCnt = 0;
static int n = 0;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBeje(void)
{
	LoadModel(MODEL_BEJE, &g_Beje[0].model);

	for (int i = 0; i < BEJE_MAX; i++)
	{
		g_Beje[i].load = TRUE;

		// �ʒu�E��]�E�X�P�[���̏����ݒ�
		g_Beje[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Beje[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Beje[i].scl = XMFLOAT3(0.2f, 0.2f, 0.2f);

		g_Beje[i].use = FALSE;
		g_Beje[i].size;				// �����蔻��̑傫��
		g_Beje[i].spd = SPEED;

		g_Beje[i].radian = 0.0f;
		g_Beje[i].len = 0.0f;
		g_Beje[i].count = 0.0f;
		g_Beje[i].time = 0.0f;
		g_Beje[i].hound = 0;

		// ���f���̐F��ύX�ł����I�������ɂ��ł����B
		for (int j = 0; j < g_Beje[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Beje[0].model, j, XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
		}

	}

	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBeje(void)
{
	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < BEJE_MAX; i++)
	{
		if (g_Beje[i].load)
		{
			g_Beje[i].load = FALSE;
		}
	}

	if(g_Beje[0].load == TRUE)	UnloadModel(&g_Beje[0].model);


}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBeje(void)
{
	ENEMY* enemy = GetEnemy();
	for (int i = 0; i < BEJE_MAX; i++)
	{
		if (g_Beje[i].load == FALSE) return;
		int bulletCount = 0;				// ���������o���b�g�̐�

		for (int i = 0; i < BEJE_MAX; i++)
		{
			if (g_Beje[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
			{								// Yes
				g_Beje[i].point[3] = XMLoadFloat3(&enemy[g_Beje[i].hound].pos);//�I�_
				g_Beje[i].vec[2] = g_Beje[i].point[3] - g_Beje[i].point[2];//2->end


				//calc
				XMVECTOR Pos2 = g_Beje[i].vec[0] * g_Beje[i].count
					+ g_Beje[i].vec[1] * g_Beje[i].count * g_Beje[i].count
					+ g_Beje[i].vec[2] * g_Beje[i].count * g_Beje[i].count * g_Beje[i].count;

				//pos�̃A�h���X�Ƀx�N�^�[������
				XMStoreFloat3(&g_Beje[i].pos, g_Beje[i].point[0] + Pos2);

				g_Beje[i].count += (SPEED / cntMax);// *(SPEED / cntMax);    // ���Ԃ�i�߂Ă���
				g_Beje[i].time -= 0.01f;
				PrintDebugProc("Beje:cnt:%f\n", g_Beje[i].count);



				if (g_Beje[i].count >= 1)            // �o�^�e�[�u���Ō�܂ňړ��������H
				{
					g_Beje[i].count = 0;                // �O�ԖڂɃ��Z�b�g�������������������p���ł���
					g_Beje[i].spd = SPEED;
					g_Beje[i].use = FALSE;
					g_Beje[i].time = 1.0f;
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
void DrawBeje(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < BEJE_MAX; i++)
	{
		if (g_Beje[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Beje[i].scl.x, g_Beje[i].scl.y, g_Beje[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Beje[i].rot.x, g_Beje[i].rot.y + XM_PI, g_Beje[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Beje[i].pos.x, g_Beje[i].pos.y, g_Beje[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Beje[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Beje[0].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

void SetBeje(XMFLOAT3 spos, XMFLOAT3 epos)
{
	ENEMY* enemy = GetEnemy();
	PLAYER* player = GetPlayer();
	CAMERA* cam = GetCamera();

	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < BEJE_MAX; i++)
	{
		if (g_Beje[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Beje[i].use = TRUE;			// �g�p��Ԃ֕ύX����


			g_Beje[i].hound = GetNearEnemy();

			if (g_Beje[i].hound == -1)
			{
				g_Beje[i].radian = (player->dir) + cam->rot.y;
				g_Beje[i].len = 300.0f;

				XMFLOAT3 p1 = XMFLOAT3(player->pos.x - sinf(-g_Beje[i].radian) * 100.0f, 50.0f, player->pos.z - cosf(-g_Beje[i].radian) * 100.0f);
				XMFLOAT3 p3 = XMFLOAT3(player->pos.x - sinf(g_Beje[i].radian) * g_Beje[i].len, 0.0f, player->pos.z - cosf(g_Beje[i].radian) * g_Beje[i].len);
				XMFLOAT3 p2 = XMFLOAT3((p1.x + p3.x) / 2, 70.0f, (p1.z + p3.z) / 2);

				g_Beje[i].point[0] = XMLoadFloat3(&spos);//�n�_
				g_Beje[i].point[1] = XMLoadFloat3(&p1);//1
				g_Beje[i].point[2] = XMLoadFloat3(&p2);//2
				g_Beje[i].point[3] = XMLoadFloat3(&p3);//�I�_
			}
			else
			{
				g_Beje[i].radian = atan2f(enemy[g_Beje[i].hound].pos.x - g_Beje[i].pos.x, enemy[g_Beje[i].hound].pos.z - g_Beje[i].pos.z);
				g_Beje[i].len = sqrt(((player->pos.x - enemy[g_Beje[i].hound].pos.x) * (player->pos.x - enemy[g_Beje[i].hound].pos.x)) + ((player->pos.z - enemy[g_Beje[i].hound].pos.z) * (player->pos.z - enemy[g_Beje[i].hound].pos.z)));

				XMFLOAT3 p1 = XMFLOAT3(player->pos.x - sinf(-g_Beje[i].radian) * 100.0f, 50.0f, player->pos.z - cosf(-g_Beje[i].radian) * 100.0f);
				XMFLOAT3 p2 = XMFLOAT3((p1.x + enemy[g_Beje[i].hound].pos.x) / 2, 70.0f, (p1.z + enemy[g_Beje[i].hound].pos.z) / 2);

				g_Beje[i].point[0] = XMLoadFloat3(&spos);//�n�_
				g_Beje[i].point[1] = XMLoadFloat3(&p1);//1
				g_Beje[i].point[2] = XMLoadFloat3(&p2);//2
				g_Beje[i].point[3] = XMLoadFloat3(&epos);//�I�_
			}


			g_Beje[i].vec[0] = g_Beje[i].point[1] - g_Beje[i].point[0];//�n�߁[���P
			g_Beje[i].vec[1] = g_Beje[i].point[2] - g_Beje[i].point[1];//1->2
			g_Beje[i].vec[2] = g_Beje[i].point[3] - g_Beje[i].point[2];//2->end

			g_Beje[i].count = 0.0f;

			return;							// 1���Z�b�g�����̂ŏI������
		}
		else
		{
			continue;
		}
	}
}


void SetBejeFive(XMFLOAT3 spos, XMFLOAT3 epos)
{
	ENEMY* enemy = GetEnemy();
	PLAYER* player = GetPlayer();
	CAMERA* cam = GetCamera();

	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < BEJE_MAX; i++)
	{
		if (g_Beje[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Beje[i].use = TRUE;			// �g�p��Ԃ֕ύX����


			g_Beje[i].hound = GetNearEnemy();

			if (g_Beje[i].hound == -1)
			{
				g_Beje[i].radian = (player->dir) + cam->rot.y;
				g_Beje[i].len = 300.0f;

				int x = rand() % (int)(200) - 200;
				int z = rand() % (int)(200) - 200;

				int ex = rand() % (int)(20) - 20;
				int ez = rand() % (int)(20) - 20;

				XMFLOAT3 p1 = XMFLOAT3(player->pos.x - sinf(-g_Beje[i].radian) * 100.0f + x, FiveCnt * 50.0f, player->pos.z - cosf(-g_Beje[i].radian) * 100.0f + z);
				XMFLOAT3 p3 = XMFLOAT3(player->pos.x - sinf(g_Beje[i].radian) * g_Beje[i].len + ex, 0.0f, player->pos.z - cosf(g_Beje[i].radian) * g_Beje[i].len + ez);
				XMFLOAT3 p2 = XMFLOAT3((p1.x + p3.x) / 2, 70.0f, (p1.z + p3.z) / 2);

				g_Beje[i].point[0] = XMLoadFloat3(&spos);//�n�_
				g_Beje[i].point[1] = XMLoadFloat3(&p1);//1
				g_Beje[i].point[2] = XMLoadFloat3(&p2);//2
				g_Beje[i].point[3] = XMLoadFloat3(&p3);//�I�_
			}
			else
			{
				g_Beje[i].radian = atan2f(enemy[g_Beje[i].hound].pos.x - g_Beje[i].pos.x, enemy[g_Beje[i].hound].pos.z - g_Beje[i].pos.z);
				g_Beje[i].len = sqrt(((player->pos.x - enemy[g_Beje[i].hound].pos.x) * (player->pos.x - enemy[g_Beje[i].hound].pos.x)) + ((player->pos.z - enemy[g_Beje[i].hound].pos.z) * (player->pos.z - enemy[g_Beje[i].hound].pos.z)));

				int x = rand() % (int)(200) - 200;
				int z = rand() % (int)(200) - 200;

				XMFLOAT3 p1 = XMFLOAT3(player->pos.x - sinf(-g_Beje[i].radian) * 100.0f + x, FiveCnt * 50.0f, player->pos.z - cosf(-g_Beje[i].radian) * 100.0f + z);
				XMFLOAT3 p2 = XMFLOAT3((p1.x + enemy[g_Beje[i].hound].pos.x) / 2, 70.0f, (p1.z + enemy[g_Beje[i].hound].pos.z) / 2);

				g_Beje[i].point[0] = XMLoadFloat3(&spos);//�n�_
				g_Beje[i].point[1] = XMLoadFloat3(&p1);//1
				g_Beje[i].point[2] = XMLoadFloat3(&p2);//2
				g_Beje[i].point[3] = XMLoadFloat3(&epos);//�I�_
			}


			g_Beje[i].vec[0] = g_Beje[i].point[1] - g_Beje[i].point[0];//�n�߁[���P
			g_Beje[i].vec[1] = g_Beje[i].point[2] - g_Beje[i].point[1];//1->2
			g_Beje[i].vec[2] = g_Beje[i].point[3] - g_Beje[i].point[2];//2->end

			g_Beje[i].count = 0.0f;

			FiveCnt++;

			if (FiveCnt == 5)
			{
				FiveCnt = 0;
				return;							// 5���Z�b�g�����̂ŏI������
			}
		}
	}
}

BEJE* GetBeje(void)
{
	return &(g_Beje[0]);
}
