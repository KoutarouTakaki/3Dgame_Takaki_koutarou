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
#include "boost.h"
#include "player.h"
#include "enemy.h"
#include "noise.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)			// テクスチャの数

#define	BOOST_SIZE_X		(40.0f)		// 頂点サイズ
#define	BOOST_SIZE_Y		(40.0f)		// 頂点サイズ
#define	VALUE_MOVE_BOOST	(5.0f)		// 移動速度

#define	MAX_BOOST		(2024)		// パーティクル最大数

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

} PARTICLE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexBoost(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点バッファ

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;					// テクスチャ番号

static PARTICLE					g_aBoost[MAX_BOOST];		// パーティクルワーク
static XMFLOAT3					g_posBase;						// ビルボード発生位置
static float					g_fWidthBase = 5.0f;			// 基準の幅
static float					g_fHeightBase = 10.0f;			// 基準の高さ
static float					g_roty = 0.0f;					// 移動方向
static float					g_spd = 0.0f;					// 移動スピード

static char* g_TextureName[] =
{
	"data/TEXTURE/effect000.jpg",
};

static BOOL							g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBoost(void)
{
	// 頂点情報の作成
	MakeVertexBoost();

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
// 終了処理
//=============================================================================
void UninitBoost(void)
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
			{// 使用中
				g_aBoost[nCntBoost].pos.x += g_aBoost[nCntBoost].move.x;
				g_aBoost[nCntBoost].pos.z += g_aBoost[nCntBoost].move.z;
				g_aBoost[nCntBoost].pos.y += g_aBoost[nCntBoost].move.y;

				if (g_aBoost[nCntBoost].pos.y <= g_aBoost[nCntBoost].fSizeY / 2)
				{// 着地した
					//g_aBoost[nCntBoost].pos.y = g_aBoost[nCntBoost].fSizeY / 2;
					//g_aBoost[nCntBoost].move.y = -g_aBoost[nCntBoost].move.y * 0.9f;
				}


				g_aBoost[nCntBoost].scale.x *= 0.8f;
				g_aBoost[nCntBoost].scale.y *= 0.8f;
#ifdef DISP_SHADOW
				if (g_aBoost[nCntBoost].nIdxShadow != -1)
				{// 影使用中
					float colA;

					// 影の位置設定
					SetPositionShadow(g_aBoost[nCntBoost].nIdxShadow, XMFLOAT3(g_aBoost[nCntBoost].pos.x, 0.1f, g_aBoost[nCntBoost].pos.z));

					// 影の色の設定
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
						// α値設定
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
// 描画処理
//=============================================================================
void DrawBoost(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
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

	for (int nCntBoost = 0; nCntBoost < MAX_BOOST; nCntBoost++)
	{
		if (g_aBoost[nCntBoost].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

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
			mtxScl = XMMatrixScaling(g_aBoost[nCntBoost].scale.x, g_aBoost[nCntBoost].scale.y, g_aBoost[nCntBoost].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_aBoost[nCntBoost].pos.x, g_aBoost[nCntBoost].pos.y, g_aBoost[nCntBoost].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_aBoost[nCntBoost].material);

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
HRESULT MakeVertexBoost(void)
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
		vertex[0].Position = XMFLOAT3(-BOOST_SIZE_X / 2, BOOST_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(BOOST_SIZE_X / 2, BOOST_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-BOOST_SIZE_X / 2, -BOOST_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(BOOST_SIZE_X / 2, -BOOST_SIZE_Y / 2, 0.0f);

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
void SetColorBoost(int nIdxBoost, XMFLOAT4 col)
{
	g_aBoost[nIdxBoost].material.Diffuse = col;
}

//=============================================================================
// パーティクルの発生処理
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
			// 影の設定
			g_aBoost[nCntBoost].nIdxShadow = CreateShadow(XMFLOAT3(pos.x, 0.1f, pos.z), 0.8f, 0.8f);		// 影の設定
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
