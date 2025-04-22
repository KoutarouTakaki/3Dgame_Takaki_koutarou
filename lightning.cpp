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
#include "lightning.h"
#include "player.h"
#include "noise.h"
#include "debugproc.h"
#include "enemy.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)			// �e�N�X�`���̐�

#define	LIGHTNING_SIZE_X		(40.0f)		// ���_�T�C�Y
#define	LIGHTNING_SIZE_Y		(40.0f)		// ���_�T�C�Y
#define	VALUE_MOVE_LIGHTNING	(5.0f)		// �ړ����x

#define	MAX_LIGHTNING_P		(100)		// �p�[�e�B�N���ő吔
#define	MAX_LIGHTNING		(1)		// �p�[�e�B�N���ő吔

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
} LIGHTNING;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexLightning(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;					// �e�N�X�`���ԍ�

static LIGHTNING				g_aLightning[MAX_LIGHTNING_P];		// �p�[�e�B�N�����[�N
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

BOOL			Luse;
BOOL			Lgene;


XMFLOAT3 inazuma[10];

static int timecnt = 0;
static int sponetime = 0;

static XMFLOAT3					g_pos;						// �r���{�[�h�����ʒu
//=============================================================================
// ����������
//=============================================================================
HRESULT InitLightning(void)
{
	// ���_���̍쐬
	MakeVertexLightning();

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
	Luse = TRUE;
	Lgene = FALSE;

	// �p�[�e�B�N�����[�N�̏�����
	for (int nCntLightning = 0; nCntLightning < MAX_LIGHTNING_P; nCntLightning++)
	{
		g_aLightning[nCntLightning].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aLightning[nCntLightning].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aLightning[nCntLightning].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aLightning[nCntLightning].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_aLightning[nCntLightning].material, sizeof(g_aLightning[nCntLightning].material));
		g_aLightning[nCntLightning].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aLightning[nCntLightning].fSizeX = LIGHTNING_SIZE_X;
		g_aLightning[nCntLightning].fSizeY = LIGHTNING_SIZE_Y;
		g_aLightning[nCntLightning].nIdxShadow = -1;
		g_aLightning[nCntLightning].nLife = 0;
		g_aLightning[nCntLightning].use = FALSE;
	}

	for (int i = 0; i < 10; i++)
	{
		inazuma[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);
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
void UninitLightning(void)
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
void UpdateLightning(void)
{
	PLAYER* pPlayer = GetPlayer();
	ENEMY* enemy = GetEnemy();
	g_posBase = enemy[0].pos;

	if (Luse == FALSE)
	{
		sponetime++;
		if (sponetime % 10 == 0)Luse = TRUE;
	}

	{
		for (int nCntLightning = 0; nCntLightning < MAX_LIGHTNING_P; nCntLightning++)
		{
			if (g_aLightning[nCntLightning].use)
			{// �g�p��

				g_aLightning[nCntLightning].nLife--;
				if (g_aLightning[nCntLightning].nLife <= 0)
				{
					g_aLightning[nCntLightning].use = FALSE;
					//ReleaseShadow(g_aLightning[nCntLightning].nIdxShadow);
					g_aLightning[nCntLightning].nIdxShadow = -1;
					Luse = FALSE;
					Lgene = FALSE;
				}
				else
				{
					if (g_aLightning[nCntLightning].nLife <= 80)
					{
						g_aLightning[nCntLightning].material.Diffuse.x = 0.1f - (float)(80 - g_aLightning[nCntLightning].nLife) / 80 * 0.1f;
						g_aLightning[nCntLightning].material.Diffuse.y = 0.1f - (float)(80 - g_aLightning[nCntLightning].nLife) / 80 * 0.1f;
						g_aLightning[nCntLightning].material.Diffuse.z = 1.0f - (float)(80 - g_aLightning[nCntLightning].nLife) / 80 * 1.0f;
					}

					if (g_aLightning[nCntLightning].nLife <= 20)
					{
						// ���l�ݒ�
						g_aLightning[nCntLightning].material.Diffuse.w -= 0.05f;
						if (g_aLightning[nCntLightning].material.Diffuse.w < 0.0f)
						{
							g_aLightning[nCntLightning].material.Diffuse.w = 0.0f;
						}
					}
				}
			}
		}

		if (Luse == TRUE && Lgene == FALSE)
		{
			// �p�[�e�B�N������
			{
				Lgene = TRUE;
				XMFLOAT3 pos[100];
				XMFLOAT3 move;
				float fAngle, fLength;
				int nLife;
				float fSize;

				fAngle = (float)(rand() % 628 - 314) / 100.0f;
				fLength = rand() % (int)(g_fWidthBase * 20) / 100.0f - g_fWidthBase;

				const float RECT_SIZE = 16.0f;
				const float	COLOR_MAX = (float)(rand() % 300 + 100);
				const int 	IMAGE_SIZE = 200;


				g_posBase.x += (float)(rand() % 200 - 100);
				g_posBase.y = 0.0f;
				g_posBase.z += (float)(rand() % 200 - 100);


				PrintDebugProc("COLOR_MAX:%f\n", COLOR_MAX);

				for (int i = 0; i < MAX_LIGHTNING_P; i++)
				{
					float n = (float)(rand() % 30 + 20);


					float fx = (float)i / RECT_SIZE;
					float fz = (float)1 / RECT_SIZE;

					float noise = ((float)COLOR_MAX * PerlinNoise(fx, fz)) - (COLOR_MAX / 2);

					pos[i].x = g_posBase.x - sinf(enemy[0].rot.y + (XM_PI / 2)) * noise;
					pos[i].y = g_posBase.y + i * 10.0f;
					pos[i].z = g_posBase.z - cosf(enemy[0].rot.y + (XM_PI / 2)) * noise;

					move.x = 0.0f;
					move.y = 0.0f;
					move.z = 0.0f;

					nLife = 100;// rand() % 100 + 150;

					fSize = (float)(rand() % 100 / 100);

					// �r���{�[�h�̐ݒ�
					SetLightning(pos[i], move, XMFLOAT4(0.1f, 0.1f, 1.0f, 0.85f), fSize, fSize, nLife);
					PrintDebugProc("pos%f\n", pos[i].y);

					g_pos = g_posBase;
				}
			}
		}
		else if (timecnt % 5 == 0)
		{
			const float RECT_SIZE = 16.0f;
			const float	COLOR_MAX = 300;// (float)(rand() % 200 + 20);
			const int 	IMAGE_SIZE = 200;

			for (int i = 0; i < MAX_LIGHTNING_P; i++)
			{
				float fx = (float)i / RECT_SIZE;
				float fz = (float)1 / RECT_SIZE;

				float noise = ((float)COLOR_MAX * OctavePerlinNoise(fx, fz)) - (COLOR_MAX / 2);

				g_aLightning[i].pos.x = g_pos.x - sinf(enemy[0].rot.y + (XM_PI / 2)) * noise;
				g_aLightning[i].pos.y = g_pos.y + i * 10.0f;
				g_aLightning[i].pos.z = g_pos.z - cosf(enemy[0].rot.y + (XM_PI / 2)) * noise;

			}
		}
	}
	timecnt += 1;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawLightning(void)
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

	for (int nCntLightning = 0; nCntLightning < MAX_LIGHTNING_P; nCntLightning++)
	{
		if (g_aLightning[nCntLightning].use)
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
			mtxScl = XMMatrixScaling(g_aLightning[nCntLightning].scale.x, g_aLightning[nCntLightning].scale.y, g_aLightning[nCntLightning].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_aLightning[nCntLightning].pos.x, g_aLightning[nCntLightning].pos.y, g_aLightning[nCntLightning].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_aLightning[nCntLightning].material);

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
HRESULT MakeVertexLightning(void)
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
		vertex[0].Position = XMFLOAT3(-LIGHTNING_SIZE_X / 2, LIGHTNING_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(LIGHTNING_SIZE_X / 2, LIGHTNING_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-LIGHTNING_SIZE_X / 2, -LIGHTNING_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(LIGHTNING_SIZE_X / 2, -LIGHTNING_SIZE_Y / 2, 0.0f);

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
void SetColorLightning(int nIdxParticle, XMFLOAT4 col)
{
	g_aLightning[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
int SetLightning(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for (int nCntLightning = 0; nCntLightning < MAX_LIGHTNING_P; nCntLightning++)
	{
		if (!g_aLightning[nCntLightning].use)
		{
			g_aLightning[nCntLightning].pos = pos;
			g_aLightning[nCntLightning].rot = { 0.0f, 0.0f, 0.0f };
			g_aLightning[nCntLightning].scale = { 1.0f, 1.0f, 1.0f };
			g_aLightning[nCntLightning].move = move;
			g_aLightning[nCntLightning].material.Diffuse = col;
			g_aLightning[nCntLightning].fSizeX = fSizeX;
			g_aLightning[nCntLightning].fSizeY = fSizeY;
			g_aLightning[nCntLightning].nLife = nLife;
			g_aLightning[nCntLightning].use = TRUE;

			nIdxParticle = nCntLightning;


			break;
		}
	}

	return nIdxParticle;
}
