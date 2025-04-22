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
#include "snow.h"
#include "player.h"
#include "noise.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)			// テクスチャの数

#define	SNOW_SIZE_X		(4.0f)		// 頂点サイズ
#define	SNOW_SIZE_Y		(4.0f)		// 頂点サイズ
#define	VALUE_MOVE_SNOW	(0.5f)		// 移動速度

#define	MAX_SNOW		(2048)		// パーティクル最大数

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
	int				snowCnt;
	BOOL			use;			// 使用しているかどうか

} SNOW;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexSnow(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点バッファ

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;					// テクスチャ番号

static SNOW						g_aSnow[MAX_SNOW];		// パーティクルワーク
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
HRESULT InitSnow(void)
{
	// 頂点情報の作成
	MakeVertexSnow();

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
// 終了処理
//=============================================================================
void UninitSnow(void)
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
void UpdateSnow(void)
{
	PLAYER* pPlayer = GetPlayer();
	g_posBase = pPlayer->pos;

	{
		for (int nCntSnow = 0; nCntSnow < MAX_SNOW; nCntSnow++)
		{
			if (g_aSnow[nCntSnow].use)
			{// 使用中
				g_aSnow[nCntSnow].pos.x += g_aSnow[nCntSnow].move.x;
				g_aSnow[nCntSnow].pos.z += g_aSnow[nCntSnow].move.z;
				g_aSnow[nCntSnow].pos.y -= g_aSnow[nCntSnow].move.y;

				if (g_aSnow[nCntSnow].pos.y <= g_aSnow[nCntSnow].fSizeY / 2)
				{// 着地した
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
						// α値設定
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

		// パーティクル発生
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

			// ビルボードの設定
			SetSnow(pos, move, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.85f), fSize, fSize, nLife);
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSnow(void)
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

	for (int nCntSnow = 0; nCntSnow < MAX_SNOW; nCntSnow++)
	{
		if (g_aSnow[nCntSnow].use)
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
			mtxScl = XMMatrixScaling(g_aSnow[nCntSnow].scale.x, g_aSnow[nCntSnow].scale.y, g_aSnow[nCntSnow].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_aSnow[nCntSnow].pos.x, g_aSnow[nCntSnow].pos.y, g_aSnow[nCntSnow].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			// マテリアル設定
			SetMaterial(g_aSnow[nCntSnow].material);

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
HRESULT MakeVertexSnow(void)
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
		vertex[0].Position = XMFLOAT3(-SNOW_SIZE_X / 2, SNOW_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(SNOW_SIZE_X / 2, SNOW_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-SNOW_SIZE_X / 2, -SNOW_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(SNOW_SIZE_X / 2, -SNOW_SIZE_Y / 2, 0.0f);

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
void SetColorSnow(int nIdxParticle, XMFLOAT4 col)
{
	g_aSnow[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// パーティクルの発生処理
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
