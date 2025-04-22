//=============================================================================
//
// パーティクル処理 [particle.cpp]
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)			// テクスチャの数

#define	BOMB_SIZE_X		(40.0f)		// 頂点サイズ
#define	BOMB_SIZE_Y		(40.0f)		// 頂点サイズ
#define	VALUE_MOVE_BOMB	(5.0f)		// 移動速度

#define	MAX_BOMB		(100000)		// パーティクル最大数

#define	DISP_SHADOW						// 影の表示
//#undef DISP_SHADOW

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;			// 位置
	XMFLOAT3		rot;			// 回転
	XMFLOAT3		scale;			// スケール
	XMFLOAT3		move;			// 移動量
	MATERIAL		material;		// マテリアル
	float			fSizeX;			// 幅
	float			fSizeY;			// 高さ
	int				nIdxShadow;		// 影ID
	int				nLife;			// 寿命
	BOOL			use;			// 使用しているかどうか

} BOMB;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexBomb(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点バッファ

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;					// テクスチャ番号

static BOMB						g_Bomb[MAX_BOMB];		// パーティクルワーク
static XMFLOAT3					g_posBase;						// ビルボード発生位置
static float					g_fWidthBase = 5.0f;			// 基準の幅
static float					g_fHeightBase = 1.0f;			// 基準の高さ
static float					g_roty = 0.0f;					// 移動方向
static float					g_spd = 0.0f;					// 移動スピード

static char* g_TextureName[] =
{
	"data/TEXTURE/effect000.jpg",
	//"data/TEXTURE/forster.png",
};

static BOOL							g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBomb(void)
{
	// 頂点情報の作成
	MakeVertexBomb();

	// テクスチャ生成
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

	// パーティクルワークの初期化
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
// 終了処理
//=============================================================================
void UninitBomb(void)
{
	if (g_Load == FALSE) return;

	//テクスチャの解放
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
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
			{// 使用中
				g_Bomb[nCntBomb].pos.x += g_Bomb[nCntBomb].move.x;
				g_Bomb[nCntBomb].pos.z += g_Bomb[nCntBomb].move.z;
				g_Bomb[nCntBomb].pos.y += g_Bomb[nCntBomb].move.y;

				g_Bomb[nCntBomb].rot.z += 0.1f;

				if (g_Bomb[nCntBomb].pos.y <= g_Bomb[nCntBomb].fSizeY / 2)
				{// 着地した
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
						// α値設定
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
// 描画処理
//=============================================================================
void DrawBomb(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView, mtxViewRot;
	CAMERA* cam = GetCamera();

	// ライティングを無効に
	SetLightEnable(FALSE);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(FALSE);

	// フォグ無効
	SetFogEnable(FALSE);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for (int nCntBomb = 0; nCntBomb < MAX_BOMB; nCntBomb++)
	{
		if (g_Bomb[nCntBomb].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			// 回転を反映
			mtxViewRot = XMMatrixRotationRollPitchYaw(g_Bomb[nCntBomb].rot.x, g_Bomb[nCntBomb].rot.y, g_Bomb[nCntBomb].rot.z);
			mtxView = XMMatrixMultiply(mtxView, mtxViewRot);

			// 処理が速いしお勧め
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Bomb[nCntBomb].scale.x, g_Bomb[nCntBomb].scale.y, g_Bomb[nCntBomb].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Bomb[nCntBomb].pos.x, g_Bomb[nCntBomb].pos.y, g_Bomb[nCntBomb].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_Bomb[nCntBomb].material);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(TRUE);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(TRUE);

	// フォグ有効
	SetFogEnable(GetFogEnable());

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexBomb(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = XMFLOAT3(-BOMB_SIZE_X / 2, BOMB_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(BOMB_SIZE_X / 2, BOMB_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-BOMB_SIZE_X / 2, -BOMB_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(BOMB_SIZE_X / 2, -BOMB_SIZE_Y / 2, 0.0f);

		// 法線の設定
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// 反射光の設定
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorBomb(int nIdxParticle, XMFLOAT4 col)
{
	g_Bomb[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// パーティクルの発生処理
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
