//=============================================================================
//
// �p�[�e�B�N������ [particle.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "light.h"
#include "bomb.h"
#include "player.h"
#include "enemy.h"
#include "noise.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)			// �e�N�X�`���̐�

#define	BOMB_SIZE_X		(40.0f)		// ���_�T�C�Y
#define	BOMB_SIZE_Y		(40.0f)		// ���_�T�C�Y
#define	VALUE_MOVE_BOMB	(5.0f)		// �ړ����x

#define	MAX_BOMB		(100000)		// �p�[�e�B�N���ő吔

#define	DISP_SHADOW						// �e�̕\��
//#undef DISP_SHADOW

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;			// �ʒu
	XMFLOAT3		rot;			// ��]
	XMFLOAT3		scale;			// �X�P�[��
	XMFLOAT3		move;			// �ړ���
	MATERIAL		material;		// �}�e���A��
	float			fSizeX;			// ��
	float			fSizeY;			// ����
	int				nIdxShadow;		// �eID
	int				nLife;			// ����
	BOOL			use;			// �g�p���Ă��邩�ǂ���

} BOMB;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexBomb(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;					// �e�N�X�`���ԍ�

static BOMB						g_Bomb[MAX_BOMB];		// �p�[�e�B�N�����[�N
static XMFLOAT3					g_posBase;						// �r���{�[�h�����ʒu
static float					g_fWidthBase = 5.0f;			// ��̕�
static float					g_fHeightBase = 1.0f;			// ��̍���
static float					g_roty = 0.0f;					// �ړ�����
static float					g_spd = 0.0f;					// �ړ��X�s�[�h

static char* g_TextureName[] =
{
	"data/TEXTURE/effect000.jpg",
	//"data/TEXTURE/forster.png",
};

static BOOL							g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBomb(void)
{
	// ���_���̍쐬
	MakeVertexBomb();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// �p�[�e�B�N�����[�N�̏�����
	for (int nCntBomb = 0; nCntBomb < MAX_BOMB; nCntBomb++)
	{
		g_Bomb[nCntBomb].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bomb[nCntBomb].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bomb[nCntBomb].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Bomb[nCntBomb].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_Bomb[nCntBomb].material, sizeof(g_Bomb[nCntBomb].material));
		g_Bomb[nCntBomb].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Bomb[nCntBomb].fSizeX = BOMB_SIZE_X;
		g_Bomb[nCntBomb].fSizeY = BOMB_SIZE_Y;
		g_Bomb[nCntBomb].nIdxShadow = -1;
		g_Bomb[nCntBomb].nLife = 0;
		g_Bomb[nCntBomb].use = FALSE;
	}

	g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBomb(void)
{
	if (g_Load == FALSE) return;

	//�e�N�X�`���̉��
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBomb(void)
{
	PLAYER* pPlayer = GetPlayer();
	g_posBase = pPlayer->pos;
	ENEMY* enemy = GetEnemy();
	g_posBase = enemy[0].pos;

	{
		for (int nCntBomb = 0; nCntBomb < MAX_BOMB; nCntBomb++)
		{
			if (g_Bomb[nCntBomb].use)
			{// �g�p��
				g_Bomb[nCntBomb].pos.x += g_Bomb[nCntBomb].move.x;
				g_Bomb[nCntBomb].pos.z += g_Bomb[nCntBomb].move.z;
				g_Bomb[nCntBomb].pos.y += g_Bomb[nCntBomb].move.y;

				g_Bomb[nCntBomb].rot.z += 0.1f;

				if (g_Bomb[nCntBomb].pos.y <= g_Bomb[nCntBomb].fSizeY / 2)
				{// ���n����
					g_Bomb[nCntBomb].pos.y = g_Bomb[nCntBomb].fSizeY / 2;
					//g_Bomb[nCntBomb].move.y = -g_Bomb[nCntBomb].move.y * 0.9f;
				}

				g_Bomb[nCntBomb].move.x += (0.0f - g_Bomb[nCntBomb].move.x) * 0.015f;
				g_Bomb[nCntBomb].move.y += 0.15f;
				g_Bomb[nCntBomb].move.z += (0.0f - g_Bomb[nCntBomb].move.z) * 0.015f;


				g_Bomb[nCntBomb].nLife--;
				if (g_Bomb[nCntBomb].nLife <= 0)
				{
					g_Bomb[nCntBomb].use = FALSE;
					//ReleaseShadow(g_Bomb[nCntBomb].nIdxShadow);
					g_Bomb[nCntBomb].nIdxShadow = -1;
				}
				else
				{
					if (g_Bomb[nCntBomb].nLife <= 8)
					{
						g_Bomb[nCntBomb].material.Diffuse.x = 0.8f - (float)(80 - g_Bomb[nCntBomb].nLife) / 80 * 0.8f;
						g_Bomb[nCntBomb].material.Diffuse.y = 0.6f - (float)(80 - g_Bomb[nCntBomb].nLife) / 80 * 0.6f;
						g_Bomb[nCntBomb].material.Diffuse.z = 0.1f - (float)(80 - g_Bomb[nCntBomb].nLife) / 80 * 0.1f;
					}

					if (g_Bomb[nCntBomb].nLife <= 2)
					{
						// ���l�ݒ�
						g_Bomb[nCntBomb].material.Diffuse.w -= 0.05f;
						if (g_Bomb[nCntBomb].material.Diffuse.w < 0.0f)
						{
							g_Bomb[nCntBomb].material.Diffuse.w = 0.0f;
						}
					}
				}
			}
		}

	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBomb(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView, mtxViewRot;
	CAMERA* cam = GetCamera();

	// ���C�e�B���O�𖳌���
	SetLightEnable(FALSE);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ADD);

	// Z��r����
	SetDepthEnable(FALSE);

	// �t�H�O����
	SetFogEnable(FALSE);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for (int nCntBomb = 0; nCntBomb < MAX_BOMB; nCntBomb++)
	{
		if (g_Bomb[nCntBomb].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			// ��]�𔽉f
			mtxViewRot = XMMatrixRotationRollPitchYaw(g_Bomb[nCntBomb].rot.x, g_Bomb[nCntBomb].rot.y, g_Bomb[nCntBomb].rot.z);
			mtxView = XMMatrixMultiply(mtxView, mtxViewRot);

			// ������������������
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Bomb[nCntBomb].scale.x, g_Bomb[nCntBomb].scale.y, g_Bomb[nCntBomb].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Bomb[nCntBomb].pos.x, g_Bomb[nCntBomb].pos.y, g_Bomb[nCntBomb].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_Bomb[nCntBomb].material);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r�L��
	SetDepthEnable(TRUE);

	// �t�H�O�L��
	SetFogEnable(GetFogEnable());

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexBomb(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = XMFLOAT3(-BOMB_SIZE_X / 2, BOMB_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(BOMB_SIZE_X / 2, BOMB_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-BOMB_SIZE_X / 2, -BOMB_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(BOMB_SIZE_X / 2, -BOMB_SIZE_Y / 2, 0.0f);

		// �@���̐ݒ�
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// ���ˌ��̐ݒ�
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// �}�e���A���J���[�̐ݒ�
//=============================================================================
void SetColorBomb(int nIdxParticle, XMFLOAT4 col)
{
	g_Bomb[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
int SetBomb(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for (int nCntBomb = 0; nCntBomb < MAX_BOMB; nCntBomb++)
	{
		if (!g_Bomb[nCntBomb].use)
		{
			g_Bomb[nCntBomb].pos = pos;
			g_Bomb[nCntBomb].rot = { 0.0f, 0.0f, 0.0f };
			g_Bomb[nCntBomb].scale = { fSizeX, fSizeY, fSizeY };
			g_Bomb[nCntBomb].move = move;
			g_Bomb[nCntBomb].material.Diffuse = col;
			g_Bomb[nCntBomb].fSizeX = fSizeX;
			g_Bomb[nCntBomb].fSizeY = fSizeY;
			g_Bomb[nCntBomb].nLife = nLife;
			g_Bomb[nCntBomb].use = TRUE;

			nIdxParticle = nCntBomb;


			break;
		}
	}

	return nIdxParticle;
}
