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
#include "straight.h"
#include "enemy.h"
#include "player.h"
#include "collision.h"
#include "shadow.h"
#include "lookon.h"
#include "boost.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_STRAIGHT			"data/MODEL/bullet_001.obj"		// 読み込むモデル名

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

static STRAIGHT						g_Straight[STRAIGHT_MAX];				// ポリゴンデータ
static int							g_TexNo;				// テクスチャ番号

static char* g_TextureName[] = {
	"data/TEXTURE/field003.jpg",
	"data/TEXTURE/bakuha.jpg",
};

int cntMax = 100;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStraight(void)
{
	// 頂点バッファの作成
	//MakeVertexStraight();
	LoadModel(MODEL_STRAIGHT, &g_Straight[0].model);

	for (int i = 0; i < STRAIGHT_MAX; i++)
	{
		g_Straight[i].load = TRUE;

		// 位置・回転・スケールの初期設定
		g_Straight[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Straight[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Straight[i].scl = XMFLOAT3(0.1f, 0.1f, 0.1f);
		g_Straight[i].move = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Straight[i].Bmove = XMFLOAT3(0.0f, 0.0f, 0.0f);

		g_Straight[i].use = FALSE;
		g_Straight[i].size;				// 当たり判定の大きさ

		g_Straight[i].radian = 0.0f;
		g_Straight[i].count = 0;

		// モデルの色を変更できるよ！半透明にもできるよ。
		for (int j = 0; j < g_Straight[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Straight[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		}
	}

	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitStraight(void)
{
	// 頂点バッファの解放
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < STRAIGHT_MAX; i++)
	{
		if (g_Straight[i].load)
		{
			g_Straight[i].load = FALSE;
		}
	}

	if (g_Straight[0].load == TRUE)	UnloadModel(&g_Straight[0].model);

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateStraight(void)
{
	for (int i = 0; i < STRAIGHT_MAX; i++)
	{
		if (g_Straight[i].load == FALSE) return;
		int bulletCount = 0;				// 処理したバレットの数

		for (int i = 0; i < STRAIGHT_MAX; i++)
		{
			if (g_Straight[i].use == TRUE)	// このバレットが使われている？
			{								// Yes

			g_Straight[i].count += 1;    // 時間を進めている

			if ((int)g_Straight[i].count % 20 == 0)
			{
				g_Straight[i].pos.x += g_Straight[i].move.x;
				g_Straight[i].pos.y += g_Straight[i].move.y;
				g_Straight[i].pos.z += g_Straight[i].move.z;


				float fSize = (float)(rand() % 1 + 0.5);
				int nLife = rand() % 2 + 4;
				for (int j = 0; j < 3; j++)
				{
					SetBoost(g_Straight[i].pos, g_Straight[i].Bmove, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), fSize, fSize, nLife);
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
void DrawStraight(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < STRAIGHT_MAX; i++)
	{
		if (g_Straight[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Straight[i].scl.x, g_Straight[i].scl.y, g_Straight[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Straight[i].rot.x, g_Straight[i].rot.y + XM_PI, g_Straight[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Straight[i].pos.x, g_Straight[i].pos.y, g_Straight[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Straight[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Straight[0].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


void SetStraight(XMFLOAT3 spos, XMFLOAT3 epos)
{
	ENEMY* enemy = GetEnemy();
	PLAYER* player = GetPlayer();
	CAMERA* cam = GetCamera();
	int n = GetNearEnemy();

	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < STRAIGHT_MAX; i++)
	{
		if (g_Straight[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Straight[i].use = TRUE;			// 使用状態へ変更する

			g_Straight[i].sPoint[0] = XMLoadFloat3(&spos);//始点

			for (int j = 0; j < STRAIGHT_MAX; j++)
			{
				if (j != i)continue;
				else g_Straight[j].sPoint[1] = XMLoadFloat3(&epos);//1

			}

			//正規化
			XMVECTOR dir = g_Straight[i].sPoint[1] - g_Straight[i].sPoint[0];
			dir = XMVector3NormalizeEst(dir);


			g_Straight[i].pos = spos;			// 座標をセット

			g_Straight[i].vec = dir;
			XMStoreFloat3(&g_Straight[i].move, g_Straight[i].vec);
			g_Straight[i].vec = XMVectorNegate(g_Straight[i].vec);
			XMStoreFloat3(&g_Straight[i].Bmove, g_Straight[i].vec);

			g_Straight[i].count = 0;

			if (n == -1)g_Straight[i].radian = player->rot.y;// + XM_PI) + cam->rot.y;
			else g_Straight[i].radian = atan2f(g_Straight[i].pos.x - enemy[n].pos.x, g_Straight[i].pos.z - enemy[n].pos.z);

			g_Straight[i].rot.x = XM_PI / 2;
			g_Straight[i].rot.y = g_Straight[i].radian;// - (90.0f * 3.14f / 180.0f);
			return;							// 1発セットしたので終了する
		}
		else
		{
			continue;
		}
	}
}

STRAIGHT* GetStraight(void)
{
	return &(g_Straight[0]);
}
