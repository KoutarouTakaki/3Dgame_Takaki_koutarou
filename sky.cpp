//=============================================================================
//
// ���b�V���n�ʂ̏��� [meshfield.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "sky.h"
#include "renderer.h"
#include "collision.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX		(1)				// �e�N�X�`���̐�


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11Buffer* g_IndexBuffer = NULL;	// �C���f�b�N�X�o�b�t�@

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;				// �e�N�X�`���ԍ�

static XMFLOAT3		g_posSky;								// �|���S���\���ʒu�̒��S���W
static XMFLOAT3		g_rotSky;								// �|���S���̉�]�p

static int			g_nNumBlockXSky, g_nNumBlockZSky;	// �u���b�N��
static int			g_nNumVertexSky;						// �����_��	
static int			g_nNumVertexIndexSky;					// ���C���f�b�N�X��
static int			g_nNumPolygonSky;						// ���|���S����
static float		g_fBlockSizeXSky, g_fBlockSizeZSky;	// �u���b�N�T�C�Y

static char* g_TextureName[] = {
	"data/TEXTURE/7759268baebef8d8b258b96682fd2766--lava-grandkids.jpg",
	"data/TEXTURE/field002.jpg",
};


// �g�̏���

static VERTEX_3D* g_Vertex = NULL;

// �g�̍��� = sin( -�o�ߎ��� * ���g�� + ���� * �����␳ ) * �U��
static XMFLOAT3		g_Center;					// �g�̔����ꏊ
static float		g_Time = 0.0f;				// �g�̌o�ߎ���
static float		g_wave_frequency = 2.0f;	// �g�̎��g��
static float		g_wave_correction = 0.02f;	// �g�̋����␳
static float		g_wave_amplitude = 20.0f;	// �g�̐U��

static BOOL			g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitSky(XMFLOAT3 pos, XMFLOAT3 rot,
	int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	// �|���S���\���ʒu�̒��S���W��ݒ�
	g_posSky = pos;

	g_rotSky = rot;

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// �u���b�N���̐ݒ�
	g_nNumBlockXSky = nNumBlockX;
	g_nNumBlockZSky = nNumBlockZ;

	// ���_���̐ݒ�
	g_nNumVertexSky = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// �C���f�b�N�X���̐ݒ�
	g_nNumVertexIndexSky = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	// �|���S�����̐ݒ�
	g_nNumPolygonSky = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// �u���b�N�T�C�Y�̐ݒ�
	g_fBlockSizeXSky = fBlockSizeX;
	g_fBlockSizeZSky = fBlockSizeZ;


	// ���_�����������ɍ���Ă����i�g�ׁ̈j
	// �g�̏���
	// �g�̍��� = sin( -�o�ߎ��� * ���g�� + ���� * �����␳ ) * �U��
	g_Vertex = new VERTEX_3D[g_nNumVertexSky];
	g_Center = XMFLOAT3(0.0f, 0.0f, 0.0f);		// �g�̔����ꏊ
	g_Time = 0.0f;								// �g�̌o�ߎ���(�{�Ɓ[�Ƃœ����O���ɂȂ�)
	g_wave_frequency = 1.0f;					// �g�̎��g���i�㉺�^���̑����j
	g_wave_correction = 0.02f;					// �g�̋����␳�i�ς��Ȃ��Ă��ǂ��Ǝv���j
	g_wave_amplitude = 15.0f;					// �g�̐U��(�g�̍���)

	float radius = 1000.0f;

	for (int z = 0; z < g_nNumBlockZSky + 1; z++)
	{
		for (int x = 0; x < g_nNumBlockXSky + 1; x++)
		{
			int index = z * (g_nNumBlockXSky + 1) + x;

			float t = (float)x / g_nNumBlockXSky * XM_PI;

			float p = (float)z / g_nNumBlockZSky * XM_PI;

			g_Vertex[index].Position.x = radius * sinf(p) * cosf(t);
			g_Vertex[index].Position.y = -radius * cosf(p); // �㉺�t�ɂ������ꍇ�A�����𕉂ɂ���
			g_Vertex[index].Position.z = radius * sinf(p) * sinf(t);

			XMFLOAT3 normal = XMFLOAT3(sinf(p) * cosf(t), cosf(p), sinf(p) * sinf(t));
			g_Vertex[index].Normal = normal;

			g_Vertex[index].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

			g_Vertex[index].TexCoord.x = static_cast<float>(x) / g_nNumBlockXSky;
			g_Vertex[index].TexCoord.y = static_cast<float>(z) / g_nNumBlockZSky;
		}
	}

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * g_nNumVertexSky;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �C���f�b�N�X�o�b�t�@����
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * g_nNumVertexIndexSky;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_IndexBuffer);


	{//���_�o�b�t�@�̒��g�𖄂߂�

		// ���_�o�b�t�@�ւ̃|�C���^���擾
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

		memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * g_nNumVertexSky);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	{//�C���f�b�N�X�o�b�t�@�̒��g�𖄂߂�

		// �C���f�b�N�X�o�b�t�@�̃|�C���^���擾
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		unsigned short* pIdx = (unsigned short*)msr.pData;

		int nCntIdx = 0;
		for (int nCntVtxZ = 0; nCntVtxZ < g_nNumBlockZSky; nCntVtxZ++)
		{
			if (nCntVtxZ > 0)
			{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXSky + 1);
				nCntIdx++;
			}

			for (int nCntVtxX = 0; nCntVtxX < (g_nNumBlockXSky + 1); nCntVtxX++)
			{
				pIdx[nCntIdx] = (nCntVtxZ + 1) * (g_nNumBlockXSky + 1) + nCntVtxX;
				nCntIdx++;
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXSky + 1) + nCntVtxX;
				nCntIdx++;
			}

			if (nCntVtxZ < (g_nNumBlockZSky - 1))
			{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
				pIdx[nCntIdx] = nCntVtxZ * (g_nNumBlockXSky + 1) + g_nNumBlockXSky;
				nCntIdx++;
			}
		}

		GetDeviceContext()->Unmap(g_IndexBuffer, 0);
	}

	return S_OK;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSky(void)
{
	if (g_Load == FALSE) return;

	// �C���f�b�N�X�o�b�t�@�̉��
	if (g_IndexBuffer) {
		g_IndexBuffer->Release();
		g_IndexBuffer = NULL;
	}

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// �e�N�X�`���̉��
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	if (g_Vertex)
	{
		delete[] g_Vertex;
		g_Vertex = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSky(void)
{

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	// �S���_���𖈉�㏑�����Ă���̂�DX11�ł͂��̕�����������ł�
	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * g_nNumVertexSky);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSky(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�ݒ�
	GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetMaterial(material);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);


	XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_rotSky.x, g_rotSky.y, g_rotSky.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_posSky.x, g_posSky.y, g_posSky.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);


	// �|���S���̕`��
	GetDeviceContext()->DrawIndexed(g_nNumVertexIndexSky, 0, 0);
}



BOOL RaySky(XMFLOAT3 pos, XMFLOAT3* HitPosition, XMFLOAT3* Normal)
{
	XMFLOAT3 start = pos;
	XMFLOAT3 end = pos;

	// ���_���������̃e�X�g
	if (start.x == 0.0f)
	{
		start.x += 1.0f;
		end.x += 1.0f;
	}
	if (start.z == 0.0f)
	{
		start.z -= 1.0f;
		end.z -= 1.0f;
	}

	// �����ォ��A�Y�h�[���Ɖ��փ��C���΂�
	start.y += 100.0f;
	end.y -= 1000.0f;

	// ����������������ׂɑS�����ł͂Ȃ��āA���W����|���S��������o���Ă���
	float fz = (g_nNumBlockXSky / 2.0f) * g_fBlockSizeXSky;
	float fx = (g_nNumBlockZSky / 2.0f) * g_fBlockSizeZSky;
	int sz = (int)((-start.z + fz) / g_fBlockSizeZSky);
	int sx = (int)((start.x + fx) / g_fBlockSizeXSky);
	int ez = sz + 1;
	int ex = sx + 1;

	if ((sz < 0) || (sz > g_nNumBlockZSky - 1) ||
		(sx < 0) || (sx > g_nNumBlockXSky - 1))
	{
		*Normal = { 0.0f, 1.0f, 0.0f };
		return FALSE;
	}


	// �K�v�����������J��Ԃ�
	for (int z = sz; z < ez; z++)
	{
		for (int x = sx; x < ex; x++)
		{
			XMFLOAT3 p0 = g_Vertex[z * (g_nNumBlockXSky + 1) + x].Position;
			XMFLOAT3 p1 = g_Vertex[z * (g_nNumBlockXSky + 1) + (x + 1)].Position;
			XMFLOAT3 p2 = g_Vertex[(z + 1) * (g_nNumBlockXSky + 1) + x].Position;
			XMFLOAT3 p3 = g_Vertex[(z + 1) * (g_nNumBlockXSky + 1) + (x + 1)].Position;

			// �O�p�|���S��������Q�����̓����蔻��
			BOOL ans = RayCast(p0, p2, p1, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}

			ans = RayCast(p1, p2, p3, start, end, HitPosition, Normal);
			if (ans)
			{
				return TRUE;
			}
		}
	}


	return FALSE;
}



