//=============================================================================
//
// 木処理 [tree.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "lookon.h"
#include "enemy.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(5)				// テクスチャの数

#define	TREE_WIDTH			(50.0f)			// 頂点サイズ
#define	TREE_HEIGHT			(80.0f)			// 頂点サイズ

#define	MAX_LOOKON			(1)			// 木最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;			// 位置
	XMFLOAT3	scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			nIdxShadow;		// 影ID
	BOOL		bUse;			// 使用しているかどうか
	int			cnt;
	
	BOOL		rock;
} LOOKON;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexLookon(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static LOOKON				g_aLookon[MAX_LOOKON];	// 木ワーク
static int					g_TexNo;			// テクスチャ番号
static BOOL					g_bAlpaTest;		// アルファテストON/OFF
//static int					g_nAlpha;			// アルファテストの閾値

static char* g_TextureName[] =
{
	"data/TEXTURE/carsol_01.png",
	"data/TEXTURE/tree002.png",
	"data/TEXTURE/tree003.png",
	"data/TEXTURE/tree004.png",
	"data/TEXTURE/tree005.png",
};

static int count;
static int NearEnemy;
static int OldNear;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitLookon(void)
{
	MakeVertexLookon();

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

	// 木ワークの初期化
	for (int nCntBakuha = 0; nCntBakuha < MAX_LOOKON; nCntBakuha++)
	{
		ZeroMemory(&g_aLookon[nCntBakuha].material, sizeof(g_aLookon[nCntBakuha].material));
		g_aLookon[nCntBakuha].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aLookon[nCntBakuha].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aLookon[nCntBakuha].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aLookon[nCntBakuha].fWidth = TREE_WIDTH;
		g_aLookon[nCntBakuha].fHeight = TREE_HEIGHT;
		g_aLookon[nCntBakuha].bUse = FALSE;
		g_aLookon[nCntBakuha].rock = FALSE;
	}

	g_bAlpaTest = TRUE;
	//g_nAlpha = 0x0;

	ENEMY* enemy = GetEnemy();
	SetLookon(enemy[SortEnemyDis()].pos, 50.0f, 50.0f, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));

	NearEnemy = 0;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitLookon(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateLookon(void)
{
	ENEMY* enemy = GetEnemy();
	//SetLookon(enemy[SortEnemyDis()].pos, 50.0f, 50.0f, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));

	if (GetKeyboardTrigger(DIK_R))
	{
		SetLookon(enemy[SortEnemyDis()].pos, 50.0f, 50.0f, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	}

	if (GetKeyboardTrigger(DIK_L))
	{
		g_aLookon[0].rock = g_aLookon[0].rock ? FALSE : TRUE;
	}

	for (int nCntBakuha = 0; nCntBakuha < MAX_LOOKON; nCntBakuha++)
	{
		OldNear = NearEnemy;
		NearEnemy = SortEnemyDis();
		if (NearEnemy != -1)g_aLookon[nCntBakuha].bUse = TRUE;
		else g_aLookon[nCntBakuha].bUse = FALSE;


		if (g_aLookon[nCntBakuha].bUse)
		{

			if (g_aLookon[0].rock)
			{
				NearEnemy = OldNear;
				if (enemy[OldNear].use == FALSE)
				{
					NearEnemy = SortEnemyDis();
					g_aLookon[0].rock = FALSE;
				}
			}
			g_aLookon[nCntBakuha].pos = enemy[NearEnemy].pos;
			g_aLookon[nCntBakuha].pos.x -= sinf(enemy[NearEnemy].rot.y) * 80.0f;
			g_aLookon[nCntBakuha].pos.y += (float)(35.0f + sin(3.14f * 2 / 60 * count) * 5);
			g_aLookon[nCntBakuha].pos.z -= cosf(enemy[NearEnemy].rot.y) * 80.0f;
			count++;
		}
	}


#ifdef _DEBUG
	// アルファテストON/OFF
	if (GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? FALSE : TRUE;
	}

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawLookon(void)
{
	// αテスト設定
	if (g_bAlpaTest == TRUE)
	{
		// αテストを有効に
		SetAlphaTestEnable(TRUE);
	}

	// ライティングを無効
	SetLightEnable(FALSE);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_LOOKON; i++)
	{
		if (g_aLookon[i].bUse)
		{
			for (int j = 0; j < 10; j++)
			{
				// ワールドマトリックスの初期化
				mtxWorld = XMMatrixIdentity();

				// ビューマトリックスを取得
				mtxView = XMLoadFloat4x4(&cam->mtxView);


				// なにかするところ
				// 正方行列（直交行列）を転置行列させて逆行列を作ってる版(速い)
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
				mtxScl = XMMatrixScaling(g_aLookon[i].scl.x, g_aLookon[i].scl.y, g_aLookon[i].scl.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

				// 移動を反映
				mtxTranslate = XMMatrixTranslation(g_aLookon[i].pos.x, g_aLookon[i].pos.y, g_aLookon[i].pos.z);
				mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

				// ワールドマトリックスの設定
				SetWorldMatrix(&mtxWorld);


				// マテリアル設定
				SetMaterial(g_aLookon[i].material);

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

				// ポリゴンの描画
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

	// ライティングを有効に
	//SetLightEnable(FALSE);

	// ？テストを無効に

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexLookon(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 60.0f;
	float fHeight = 90.0f;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 拡散光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 木のパラメータをセット
//=============================================================================
int SetLookon(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxTree = -1;

	for (int nCntBakuha = 0; nCntBakuha < MAX_LOOKON; nCntBakuha++)
	{
		if (!g_aLookon[nCntBakuha].bUse)
		{
			g_aLookon[nCntBakuha].pos.x = pos.x;
			g_aLookon[nCntBakuha].pos.y = pos.y + 10.0f;
			g_aLookon[nCntBakuha].pos.z = pos.z;
			g_aLookon[nCntBakuha].scl = XMFLOAT3(0.2f, 0.2f, 0.2f);
			g_aLookon[nCntBakuha].fWidth = fWidth;
			g_aLookon[nCntBakuha].fHeight = fHeight;
			g_aLookon[nCntBakuha].bUse = TRUE;

			// 影の設定
			//g_aLookon[nCntBakuha].nIdxShadow = CreateShadow(g_aLookon[nCntBakuha].pos, 0.5f, 0.5f);

			nIdxTree = nCntBakuha;

			break;
		}
	}

	return nIdxTree;
}

int SortEnemyDis()
{
	PLAYER* player = GetPlayer();
	ENEMY* enemy = GetEnemy();
	float dis[MAX_ENEMY];
	float dismin;
	int min = 0;
	int count = 0;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE)
		{
			dis[i] = -1;
		}
		else
		{
			dis[i] = (float)sqrt(((enemy[i].pos.x - player->pos.x) * (enemy[i].pos.x - player->pos.x)) + ((enemy[i].pos.y - player->pos.y) * (enemy[i].pos.y - player->pos.y)) + ((enemy[i].pos.z - player->pos.z) * (enemy[i].pos.z - player->pos.z)));
		}
	}

	dismin = 500000;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (dis[i] == -1)continue;

		if (dis[i] < dismin)
		{
			dismin = dis[i];
			min = i;
		}
		count++;
	}

	if (count == 0 || dismin >= 300000)
	{
		return -1;
	}
	else
	{
		return min;
	}
}


int GetNearEnemy()
{
	return NearEnemy;
}