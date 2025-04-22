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
#include "snow.h"
#include "player.h"
#include "noise.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)			// �e�N�X�`���̐�

#define	SNOW_SIZE_X		(4.0f)		// ���_�T�C�Y
#define	SNOW_SIZE_Y		(4.0f)		// ���_�T�C�Y
#define	VALUE_MOVE_SNOW	(0.5f)		// �ړ����x

#define	MAX_SNOW		(2048)		// �p�[�e�B�N���ő吔

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
	int				snowCnt;
	BOOL			use;			// �g�p���Ă��邩�ǂ���

} SNOW;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexSnow(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;					// �e�N�X�`���ԍ�

static SNOW						g_aSnow[MAX_SNOW];		// �p�[�e�B�N�����[�N
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
HRESULT InitSnow(void)
{
	// ���_���̍쐬
	MakeVertexSnow();

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
	for (int nCntSnow = 0; nCntSnow < MAX_SNOW; nCntSnow++)
	{
		g_aSnow[nCntSnow].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aSnow[nCntSnow].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aSnow[nCntSnow].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aSnow[nCntSnow].move = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aSnow[nCntSnow].snowCnt = 0;

		ZeroMemory(&g_aSnow[nCntSnow].material, sizeof(g_aSnow[nCntSnow].material));
		g_aSnow[nCntSnow].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aSnow[nCntSnow].fSizeX = SNOW_SIZE_X;
		g_aSnow[nCntSnow].fSizeY = SNOW_SIZE_Y;
		g_aSnow[nCntSnow].nIdxShadow = -1;
		g_aSnow[nCntSnow].nLife = 0;
		g_aSnow[nCntSnow].use = FALSE;
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
void UninitSnow(void)
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
void UpdateSnow(void)
{
	PLAYER* pPlayer = GetPlayer();
	g_posBase = pPlayer->pos;

	{
		for (int nCntSnow = 0; nCntSnow < MAX_SNOW; nCntSnow++)
		{
			if (g_aSnow[nCntSnow].use)
			{// �g�p��
				g_aSnow[nCntSnow].pos.x += g_aSnow[nCntSnow].move.x;
				g_aSnow[nCntSnow].pos.z += g_aSnow[nCntSnow].move.z;
				g_aSnow[nCntSnow].pos.y -= g_aSnow[nCntSnow].move.y;

				if (g_aSnow[nCntSnow].pos.y <= g_aSnow[nCntSnow].fSizeY / 2)
				{// ���n����
					g_aSnow[nCntSnow].pos.y = g_aSnow[nCntSnow].fSizeY / 2;
					g_aSnow[nCntSnow].move.x = 0.0f;
					//g_aSnow[nCntSnow].move.y = -g_aSnow[nCntSnow].move.y * 0.9f;
				}
				else
				{
					g_aSnow[nCntSnow].move.x = (float)sin(3.14f * 2 / 500 * g_aSnow[nCntSnow].snowCnt) * 0.3f;

				}

				//g_aSnow[nCntSnow].move.y *= 0.95f;
				g_aSnow[nCntSnow].move.z += (0.0f - g_aSnow[nCntSnow].move.z) * 0.015f;


				g_aSnow[nCntSnow].nLife--;
				if (g_aSnow[nCntSnow].nLife <= 0)
				{
					g_aSnow[nCntSnow].use = FALSE;
					//ReleaseShadow(g_aSnow[nCntSnow].nIdxShadow);
					g_aSnow[nCntSnow].nIdxShadow = -1;
				}
				else
				{
					if (g_aSnow[nCntSnow].nLife <= 8)
					{
						g_aSnow[nCntSnow].material.Diffuse.x = 0.8f - (float)(80 - g_aSnow[nCntSnow].nLife) / 80 * 0.8f;
						g_aSnow[nCntSnow].material.Diffuse.y = 0.6f - (float)(80 - g_aSnow[nCntSnow].nLife) / 80 * 0.6f;
						g_aSnow[nCntSnow].material.Diffuse.z = 0.1f - (float)(80 - g_aSnow[nCntSnow].nLife) / 80 * 0.1f;
					}

					if (g_aSnow[nCntSnow].nLife <= 2)
					{
						// ���l�ݒ�
						g_aSnow[nCntSnow].material.Diffuse.w -= 0.05f;
						if (g_aSnow[nCntSnow].material.Diffuse.w < 0.0f)
						{
							g_aSnow[nCntSnow].material.Diffuse.w = 0.0f;
						}
					}
				}
				g_aSnow[nCntSnow].snowCnt++;
			}
		}

		// �p�[�e�B�N������
		{
			XMFLOAT3 pos;
			XMFLOAT3 move;
			float fAngle, fLength;
			int nLife;
			float fSize;

			pos = g_posBase;

			//srand(rand());
			//SettingHash(rand());

			pos.x = (float)(rand() % 1500 - 750);
			pos.y = 200.0f;
			pos.z = (float)(rand() % 1500 - 750);


			fAngle = (float)(rand() % 628 - 314) / 100.0f;
			fLength = rand() % (int)(g_fWidthBase * 200) / 100.0f - g_fWidthBase;
			move.x = 0.0f;// sinf(fAngle)* fLength;
			move.y = VALUE_MOVE_SNOW;
			move.z = 0.0f;// cosf(fAngle)* fLength;

			nLife = rand() % 200 + 300;

			fSize = (float)(rand() % 3 + 2);

			pos.y -= fSize / 2;

			// �r���{�[�h�̐ݒ�
			SetSnow(pos, move, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.85f), fSize, fSize, nLife);
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSnow(void)
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

	for (int nCntSnow = 0; nCntSnow < MAX_SNOW; nCntSnow++)
	{
		if (g_aSnow[nCntSnow].use)
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
			mtxScl = XMMatrixScaling(g_aSnow[nCntSnow].scale.x, g_aSnow[nCntSnow].scale.y, g_aSnow[nCntSnow].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_aSnow[nCntSnow].pos.x, g_aSnow[nCntSnow].pos.y, g_aSnow[nCntSnow].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_aSnow[nCntSnow].material);

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
HRESULT MakeVertexSnow(void)
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
		vertex[0].Position = XMFLOAT3(-SNOW_SIZE_X / 2, SNOW_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(SNOW_SIZE_X / 2, SNOW_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-SNOW_SIZE_X / 2, -SNOW_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(SNOW_SIZE_X / 2, -SNOW_SIZE_Y / 2, 0.0f);

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
void SetColorSnow(int nIdxParticle, XMFLOAT4 col)
{
	g_aSnow[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
int SetSnow(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for (int nCntSnow = 0; nCntSnow < MAX_SNOW; nCntSnow++)
	{
		if (!g_aSnow[nCntSnow].use)
		{
			g_aSnow[nCntSnow].pos = pos;
			g_aSnow[nCntSnow].rot = { 0.0f, 0.0f, 0.0f };
			g_aSnow[nCntSnow].scale = { 1.0f, 1.0f, 1.0f };
			g_aSnow[nCntSnow].move = move;
			g_aSnow[nCntSnow].material.Diffuse = col;
			g_aSnow[nCntSnow].fSizeX = fSizeX;
			g_aSnow[nCntSnow].fSizeY = fSizeY;
			g_aSnow[nCntSnow].nLife = nLife;
			g_aSnow[nCntSnow].use = TRUE;

			nIdxParticle = nCntSnow;


			break;
		}
	}

	return nIdxParticle;
}
