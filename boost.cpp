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
#include "boost.h"
#include "player.h"
#include "enemy.h"
#include "noise.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)			// �e�N�X�`���̐�

#define	BOOST_SIZE_X		(40.0f)		// ���_�T�C�Y
#define	BOOST_SIZE_Y		(40.0f)		// ���_�T�C�Y
#define	VALUE_MOVE_BOOST	(5.0f)		// �ړ����x

#define	MAX_BOOST		(2024)		// �p�[�e�B�N���ő吔

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

} PARTICLE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexBoost(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;					// �e�N�X�`���ԍ�

static PARTICLE					g_aBoost[MAX_BOOST];		// �p�[�e�B�N�����[�N
static XMFLOAT3					g_posBase;						// �r���{�[�h�����ʒu
static float					g_fWidthBase = 5.0f;			// ��̕�
static float					g_fHeightBase = 10.0f;			// ��̍���
static float					g_roty = 0.0f;					// �ړ�����
static float					g_spd = 0.0f;					// �ړ��X�s�[�h

static char* g_TextureName[] =
{
	"data/TEXTURE/effect000.jpg",
};

static BOOL							g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitBoost(void)
{
	// ���_���̍쐬
	MakeVertexBoost();

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
	for (int nCntBoost = 0; nCntBoost < MAX_BOOST; nCntBoost++)
	{
		g_aBoost[nCntBoost].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aBoost[nCntBoost].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aBoost[nCntBoost].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aBoost[nCntBoost].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_aBoost[nCntBoost].material, sizeof(g_aBoost[nCntBoost].material));
		g_aBoost[nCntBoost].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aBoost[nCntBoost].fSizeX = BOOST_SIZE_X;
		g_aBoost[nCntBoost].fSizeY = BOOST_SIZE_Y;
		g_aBoost[nCntBoost].nIdxShadow = -1;
		g_aBoost[nCntBoost].nLife = 0;
		g_aBoost[nCntBoost].use = FALSE;
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
void UninitBoost(void)
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
void UpdateBoost(void)
{
	PLAYER* pPlayer = GetPlayer();
	g_posBase = pPlayer->pos;
	ENEMY* enemy = GetEnemy();
	g_posBase = enemy[0].pos;

	{
		for (int nCntBoost = 0; nCntBoost < MAX_BOOST; nCntBoost++)
		{
			if (g_aBoost[nCntBoost].use)
			{// �g�p��
				g_aBoost[nCntBoost].pos.x += g_aBoost[nCntBoost].move.x;
				g_aBoost[nCntBoost].pos.z += g_aBoost[nCntBoost].move.z;
				g_aBoost[nCntBoost].pos.y += g_aBoost[nCntBoost].move.y;

				if (g_aBoost[nCntBoost].pos.y <= g_aBoost[nCntBoost].fSizeY / 2)
				{// ���n����
					//g_aBoost[nCntBoost].pos.y = g_aBoost[nCntBoost].fSizeY / 2;
					//g_aBoost[nCntBoost].move.y = -g_aBoost[nCntBoost].move.y * 0.9f;
				}


				g_aBoost[nCntBoost].scale.x *= 0.8f;
				g_aBoost[nCntBoost].scale.y *= 0.8f;
#ifdef DISP_SHADOW
				if (g_aBoost[nCntBoost].nIdxShadow != -1)
				{// �e�g�p��
					float colA;

					// �e�̈ʒu�ݒ�
					SetPositionShadow(g_aBoost[nCntBoost].nIdxShadow, XMFLOAT3(g_aBoost[nCntBoost].pos.x, 0.1f, g_aBoost[nCntBoost].pos.z));

					// �e�̐F�̐ݒ�
					colA = g_aBoost[nCntBoost].material.Diffuse.w;
					SetColorShadow(g_aBoost[nCntBoost].nIdxShadow, XMFLOAT4(0.5f, 0.5f, 0.5f, colA));
				}
#endif

				g_aBoost[nCntBoost].nLife--;
				if (g_aBoost[nCntBoost].nLife <= 0)
				{
					g_aBoost[nCntBoost].use = FALSE;
					ReleaseShadow(g_aBoost[nCntBoost].nIdxShadow);
					g_aBoost[nCntBoost].nIdxShadow = -1;
				}
				else
				{
					if (g_aBoost[nCntBoost].nLife <= 8)
					{
						g_aBoost[nCntBoost].material.Diffuse.x = 0.8f - (float)(80 - g_aBoost[nCntBoost].nLife) / 80 * 0.8f;
						g_aBoost[nCntBoost].material.Diffuse.y = 0.6f - (float)(80 - g_aBoost[nCntBoost].nLife) / 80 * 0.6f;
						g_aBoost[nCntBoost].material.Diffuse.z = 0.1f - (float)(80 - g_aBoost[nCntBoost].nLife) / 80 * 0.1f;
					}

					if (g_aBoost[nCntBoost].nLife <= 2)
					{
						// ���l�ݒ�
						g_aBoost[nCntBoost].material.Diffuse.w -= 0.05f;
						if (g_aBoost[nCntBoost].material.Diffuse.w < 0.0f)
						{
							g_aBoost[nCntBoost].material.Diffuse.w = 0.0f;
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
void DrawBoost(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
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

	for (int nCntBoost = 0; nCntBoost < MAX_BOOST; nCntBoost++)
	{
		if (g_aBoost[nCntBoost].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

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
			mtxScl = XMMatrixScaling(g_aBoost[nCntBoost].scale.x, g_aBoost[nCntBoost].scale.y, g_aBoost[nCntBoost].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_aBoost[nCntBoost].pos.x, g_aBoost[nCntBoost].pos.y, g_aBoost[nCntBoost].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_aBoost[nCntBoost].material);

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
HRESULT MakeVertexBoost(void)
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
		vertex[0].Position = XMFLOAT3(-BOOST_SIZE_X / 2, BOOST_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(BOOST_SIZE_X / 2, BOOST_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-BOOST_SIZE_X / 2, -BOOST_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(BOOST_SIZE_X / 2, -BOOST_SIZE_Y / 2, 0.0f);

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
void SetColorBoost(int nIdxBoost, XMFLOAT4 col)
{
	g_aBoost[nIdxBoost].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
int SetBoost(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for (int nCntBoost = 0; nCntBoost < MAX_BOOST; nCntBoost++)
	{
		if (!g_aBoost[nCntBoost].use)
		{
			g_aBoost[nCntBoost].pos = pos;
			g_aBoost[nCntBoost].rot = { 0.0f, 0.0f, 0.0f };
			g_aBoost[nCntBoost].scale = { fSizeX, fSizeY, fSizeY };
			g_aBoost[nCntBoost].move = move;
			g_aBoost[nCntBoost].material.Diffuse = col;
			g_aBoost[nCntBoost].fSizeX = fSizeX;
			g_aBoost[nCntBoost].fSizeY = fSizeY;
			g_aBoost[nCntBoost].nLife = nLife;
			g_aBoost[nCntBoost].use = TRUE;

			nIdxParticle = nCntBoost;

#ifdef DISP_SHADOW
			// �e�̐ݒ�
			g_aBoost[nCntBoost].nIdxShadow = CreateShadow(XMFLOAT3(pos.x, 0.1f, pos.z), 0.8f, 0.8f);		// �e�̐ݒ�
			if (g_aBoost[nCntBoost].nIdxShadow != -1)
			{
				SetColorShadow(g_aBoost[nCntBoost].nIdxShadow, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
			}
#endif

			break;
		}
	}

	return nIdxParticle;
}
