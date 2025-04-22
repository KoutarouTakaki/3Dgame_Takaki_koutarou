//=============================================================================
//
// メッシュ地面の処理 [meshfield.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "sky.h"
#include "renderer.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX		(1)				// テクスチャの数


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11Buffer* g_IndexBuffer = NULL;	// インデックスバッファ

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;				// テクスチャ番号

static XMFLOAT3		g_posSky;								// ポリゴン表示位置の中心座標
static XMFLOAT3		g_rotSky;								// ポリゴンの回転角

static int			g_nNumBlockXSky, g_nNumBlockZSky;	// ブロック数
static int			g_nNumVertexSky;						// 総頂点数	
static int			g_nNumVertexIndexSky;					// 総インデックス数
static int			g_nNumPolygonSky;						// 総ポリゴン数
static float		g_fBlockSizeXSky, g_fBlockSizeZSky;	// ブロックサイズ

static char* g_TextureName[] = {
	"data/TEXTURE/7759268baebef8d8b258b96682fd2766--lava-grandkids.jpg",
	"data/TEXTURE/field002.jpg",
};


// 波の処理

static VERTEX_3D* g_Vertex = NULL;

// 波の高さ = sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
static XMFLOAT3		g_Center;					// 波の発生場所
static float		g_Time = 0.0f;				// 波の経過時間
static float		g_wave_frequency = 2.0f;	// 波の周波数
static float		g_wave_correction = 0.02f;	// 波の距離補正
static float		g_wave_amplitude = 20.0f;	// 波の振幅

static BOOL			g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSky(XMFLOAT3 pos, XMFLOAT3 rot,
	int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	// ポリゴン表示位置の中心座標を設定
	g_posSky = pos;

	g_rotSky = rot;

	// テクスチャ生成
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

	// ブロック数の設定
	g_nNumBlockXSky = nNumBlockX;
	g_nNumBlockZSky = nNumBlockZ;

	// 頂点数の設定
	g_nNumVertexSky = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// インデックス数の設定
	g_nNumVertexIndexSky = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	// ポリゴン数の設定
	g_nNumPolygonSky = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// ブロックサイズの設定
	g_fBlockSizeXSky = fBlockSizeX;
	g_fBlockSizeZSky = fBlockSizeZ;


	// 頂点情報をメモリに作っておく（波の為）
	// 波の処理
	// 波の高さ = sin( -経過時間 * 周波数 + 距離 * 距離補正 ) * 振幅
	g_Vertex = new VERTEX_3D[g_nNumVertexSky];
	g_Center = XMFLOAT3(0.0f, 0.0f, 0.0f);		// 波の発生場所
	g_Time = 0.0f;								// 波の経過時間(＋とーとで内側外側になる)
	g_wave_frequency = 1.0f;					// 波の周波数（上下運動の速さ）
	g_wave_correction = 0.02f;					// 波の距離補正（変えなくても良いと思う）
	g_wave_amplitude = 15.0f;					// 波の振幅(波の高さ)

	float radius = 1000.0f;

	for (int z = 0; z < g_nNumBlockZSky + 1; z++)
	{
		for (int x = 0; x < g_nNumBlockXSky + 1; x++)
		{
			int index = z * (g_nNumBlockXSky + 1) + x;

			float t = (float)x / g_nNumBlockXSky * XM_PI;

			float p = (float)z / g_nNumBlockZSky * XM_PI;

			g_Vertex[index].Position.x = radius * sinf(p) * cosf(t);
			g_Vertex[index].Position.y = -radius * cosf(p); // 上下逆にしたい場合、ここを負にする
			g_Vertex[index].Position.z = radius * sinf(p) * sinf(t);

			XMFLOAT3 normal = XMFLOAT3(sinf(p) * cosf(t), cosf(p), sinf(p) * sinf(t));
			g_Vertex[index].Normal = normal;

			g_Vertex[index].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

			g_Vertex[index].TexCoord.x = static_cast<float>(x) / g_nNumBlockXSky;
			g_Vertex[index].TexCoord.y = static_cast<float>(z) / g_nNumBlockZSky;
		}
	}

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * g_nNumVertexSky;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// インデックスバッファ生成
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned short) * g_nNumVertexIndexSky;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_IndexBuffer);


	{//頂点バッファの中身を埋める

		// 頂点バッファへのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

		memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * g_nNumVertexSky);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	{//インデックスバッファの中身を埋める

		// インデックスバッファのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		unsigned short* pIdx = (unsigned short*)msr.pData;

		int nCntIdx = 0;
		for (int nCntVtxZ = 0; nCntVtxZ < g_nNumBlockZSky; nCntVtxZ++)
		{
			if (nCntVtxZ > 0)
			{// 縮退ポリゴンのためのダブりの設定
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
			{// 縮退ポリゴンのためのダブりの設定
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
// 終了処理
//=============================================================================
void UninitSky(void)
{
	if (g_Load == FALSE) return;

	// インデックスバッファの解放
	if (g_IndexBuffer) {
		g_IndexBuffer->Release();
		g_IndexBuffer = NULL;
	}

	// 頂点バッファの解放
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// テクスチャの解放
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
// 更新処理
//=============================================================================
void UpdateSky(void)
{

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	// 全頂点情報を毎回上書きしているのはDX11ではこの方が早いからです
	memcpy(pVtx, g_Vertex, sizeof(VERTEX_3D) * g_nNumVertexSky);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSky(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);


	XMMATRIX mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_rotSky.x, g_rotSky.y, g_rotSky.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_posSky.x, g_posSky.y, g_posSky.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);


	// ポリゴンの描画
	GetDeviceContext()->DrawIndexed(g_nNumVertexIndexSky, 0, 0);
}



BOOL RaySky(XMFLOAT3 pos, XMFLOAT3* HitPosition, XMFLOAT3* Normal)
{
	XMFLOAT3 start = pos;
	XMFLOAT3 end = pos;

	// 原点だった時のテスト
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

	// 少し上から、ズドーンと下へレイを飛ばす
	start.y += 100.0f;
	end.y -= 1000.0f;

	// 処理を高速化する為に全検索ではなくて、座標からポリゴンを割り出している
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


	// 必要数分検索を繰り返す
	for (int z = sz; z < ez; z++)
	{
		for (int x = sx; x < ex; x++)
		{
			XMFLOAT3 p0 = g_Vertex[z * (g_nNumBlockXSky + 1) + x].Position;
			XMFLOAT3 p1 = g_Vertex[z * (g_nNumBlockXSky + 1) + (x + 1)].Position;
			XMFLOAT3 p2 = g_Vertex[(z + 1) * (g_nNumBlockXSky + 1) + x].Position;
			XMFLOAT3 p3 = g_Vertex[(z + 1) * (g_nNumBlockXSky + 1) + (x + 1)].Position;

			// 三角ポリゴンだから２枚分の当たり判定
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



