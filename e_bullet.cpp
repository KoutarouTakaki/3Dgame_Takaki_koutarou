//=============================================================================
//
// フィールド表示処理 [field.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "e_bullet.h"
#include "enemy.h"
#include "player.h"
#include "collision.h"
#include "shadow.h"
#include "lookon.h"
#include "boost.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_EBULLET			"data/MODEL/bullet_001.obj"		// 読み込むモデル名

#define	VALUE_MOVE		(5.0f)					// 移動量
#define	VALUE_ROTATE	(XM_PI * 0.02f)			// 回転量

#define	SIZE_WH			(100.0f)				// 地面のサイズ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// 頂点情報

static EBULLET						g_Ebullet[EBULLET_MAX];				// ポリゴンデータ
static int							g_TexNo;				// テクスチャ番号

static char* g_TextureName[] = {
	"data/TEXTURE/field003.jpg",
	"data/TEXTURE/bakuha.jpg",
};

int EcntMax = 100;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEbullet(void)
{
	// 頂点バッファの作成
	//MakeVertexStraight();
	LoadModel(MODEL_EBULLET, &g_Ebullet[0].model);

	for (int i = 0; i < EBULLET_MAX; i++)
	{
		g_Ebullet[i].load = TRUE;

		// 位置・回転・スケールの初期設定
		g_Ebullet[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ebullet[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ebullet[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);
		g_Ebullet[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Ebullet[i].Bmove = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Ebullet[i].use = FALSE;
		g_Ebullet[i].size;				// 当たり判定の大きさ

		g_Ebullet[i].radian = 0.0f;
		g_Ebullet[i].count = 0;

		// モデルの色を変更できるよ！半透明にもできるよ。
		for (int j = 0; j < g_Ebullet[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Ebullet[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		}
	}

	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEbullet(void)
{
	// 頂点バッファの解放
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_Ebullet[i].load)
		{
			g_Ebullet[i].load = FALSE;
		}
	}
	if (g_Ebullet[0].load == TRUE)UnloadModel(&g_Ebullet[0].model);

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEbullet(void)
{
	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_Ebullet[i].load == FALSE) return;
		int bulletCount = 0;				// 処理したバレットの数

		for (int i = 0; i < EBULLET_MAX; i++)
		{
			if (g_Ebullet[i].use == TRUE)	// このバレットが使われている？
			{								// Yes


				g_Ebullet[i].count += 1.0f;    // 時間を進めている
				if ((int)g_Ebullet[i].count % 20 == 0)
				{
					g_Ebullet[i].pos.x += g_Ebullet[i].move.x;
					g_Ebullet[i].pos.y += g_Ebullet[i].move.y;
					g_Ebullet[i].pos.z += g_Ebullet[i].move.z;


					float fSize = (float)(rand() % 2 + 1);
					int nLife = rand() % 3 + 6;
					for (int j = 0; j < 3; j++)
					{
						SetBoost(g_Ebullet[i].pos, g_Ebullet[i].Bmove, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), fSize, fSize, nLife);
					}
				}


				bulletCount++;
			}
		}
		//
		//#ifdef _DEBUG	// デバッグ情報を表示する
		// 
		//#endif

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEbullet(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_Ebullet[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Ebullet[i].scl.x, g_Ebullet[i].scl.y, g_Ebullet[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Ebullet[i].rot.x, g_Ebullet[i].rot.y + XM_PI, g_Ebullet[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Ebullet[i].pos.x, g_Ebullet[i].pos.y, g_Ebullet[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Ebullet[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Ebullet[0].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


void SetEbullet(XMFLOAT3 spos, XMFLOAT3 epos)
{
	PrintDebugProc("bullet\n");

	ENEMY* enemy = GetEnemy();
	PLAYER* player = GetPlayer();
	CAMERA* cam = GetCamera();
	int n = GetNearEnemy();

	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < EBULLET_MAX; i++)
	{
		if (g_Ebullet[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Ebullet[i].use = TRUE;			// 使用状態へ変更する

			g_Ebullet[i].sPoint[0] = XMLoadFloat3(&spos);//始点

			for (int j = 0; j < EBULLET_MAX; j++)
			{
				if (j != i)continue;
				else g_Ebullet[j].sPoint[1] = XMLoadFloat3(&epos);//1

			}

			//正規化
			XMVECTOR dir = g_Ebullet[i].sPoint[1] - g_Ebullet[i].sPoint[0];
			dir = XMVector3NormalizeEst(dir);


			g_Ebullet[i].pos = spos;			// 座標をセット

			g_Ebullet[i].vec = dir;
			XMStoreFloat3(&g_Ebullet[i].move, g_Ebullet[i].vec);
			g_Ebullet[i].vec = XMVectorNegate(g_Ebullet[i].vec);
			XMStoreFloat3(&g_Ebullet[i].Bmove, g_Ebullet[i].vec);


			g_Ebullet[i].count = 0;

			g_Ebullet[i].radian = atan2f(g_Ebullet[i].pos.x - player->pos.x, g_Ebullet[i].pos.z - player->pos.z);

			g_Ebullet[i].rot.x = XM_PI / 6 * 4;
			g_Ebullet[i].rot.y = g_Ebullet[i].radian;// - (90.0f * 3.14f / 180.0f);
			return;							// 1発セットしたので終了する
		}
		else
		{
			continue;
		}
	}
}

EBULLET* GetEbullet(void)
{
	return &(g_Ebullet[0]);
}
