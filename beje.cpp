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
#include "beje.h"
#include "enemy.h"
#include "player.h"
#include "collision.h"
#include "shadow.h"
#include "lookon.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BEJE			"data/MODEL/ball.obj"		// 読み込むモデル名

#define	VALUE_MOVE		(5.0f)					// 移動量
#define	VALUE_ROTATE	(XM_PI * 0.02f)			// 回転量

#define	SIZE_WH			(100.0f)				// 地面のサイズ

#define SPEED			(0.01f)
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
//HRESULT MakeVertexStraight(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// 頂点情報

static BEJE						g_Beje[BEJE_MAX];				// ポリゴンデータ
static int							g_TexNo;				// テクスチャ番号

static char* g_TextureName[] = {
	"data/TEXTURE/field003.jpg",
	"data/TEXTURE/bakuha.jpg",
};

static int cntMax = 100;
static int FiveCnt = 0;
static int n = 0;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBeje(void)
{
	LoadModel(MODEL_BEJE, &g_Beje[0].model);

	for (int i = 0; i < BEJE_MAX; i++)
	{
		g_Beje[i].load = TRUE;

		// 位置・回転・スケールの初期設定
		g_Beje[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Beje[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Beje[i].scl = XMFLOAT3(0.2f, 0.2f, 0.2f);

		g_Beje[i].use = FALSE;
		g_Beje[i].size;				// 当たり判定の大きさ
		g_Beje[i].spd = SPEED;

		g_Beje[i].radian = 0.0f;
		g_Beje[i].len = 0.0f;
		g_Beje[i].count = 0.0f;
		g_Beje[i].time = 0.0f;
		g_Beje[i].hound = 0;

		// モデルの色を変更できるよ！半透明にもできるよ。
		for (int j = 0; j < g_Beje[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Beje[0].model, j, XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
		}

	}

	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBeje(void)
{
	// 頂点バッファの解放
	if (g_VertexBuffer) {
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < BEJE_MAX; i++)
	{
		if (g_Beje[i].load)
		{
			g_Beje[i].load = FALSE;
		}
	}

	if(g_Beje[0].load == TRUE)	UnloadModel(&g_Beje[0].model);


}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBeje(void)
{
	ENEMY* enemy = GetEnemy();
	for (int i = 0; i < BEJE_MAX; i++)
	{
		if (g_Beje[i].load == FALSE) return;
		int bulletCount = 0;				// 処理したバレットの数

		for (int i = 0; i < BEJE_MAX; i++)
		{
			if (g_Beje[i].use == TRUE)	// このバレットが使われている？
			{								// Yes
				g_Beje[i].point[3] = XMLoadFloat3(&enemy[g_Beje[i].hound].pos);//終点
				g_Beje[i].vec[2] = g_Beje[i].point[3] - g_Beje[i].point[2];//2->end


				//calc
				XMVECTOR Pos2 = g_Beje[i].vec[0] * g_Beje[i].count
					+ g_Beje[i].vec[1] * g_Beje[i].count * g_Beje[i].count
					+ g_Beje[i].vec[2] * g_Beje[i].count * g_Beje[i].count * g_Beje[i].count;

				//posのアドレスにベクターを入れる
				XMStoreFloat3(&g_Beje[i].pos, g_Beje[i].point[0] + Pos2);

				g_Beje[i].count += (SPEED / cntMax);// *(SPEED / cntMax);    // 時間を進めている
				g_Beje[i].time -= 0.01f;
				PrintDebugProc("Beje:cnt:%f\n", g_Beje[i].count);



				if (g_Beje[i].count >= 1)            // 登録テーブル最後まで移動したか？
				{
					g_Beje[i].count = 0;                // ０番目にリセットしつつも小数部分を引き継いでいる
					g_Beje[i].spd = SPEED;
					g_Beje[i].use = FALSE;
					g_Beje[i].time = 1.0f;
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
void DrawBeje(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < BEJE_MAX; i++)
	{
		if (g_Beje[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Beje[i].scl.x, g_Beje[i].scl.y, g_Beje[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Beje[i].rot.x, g_Beje[i].rot.y + XM_PI, g_Beje[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Beje[i].pos.x, g_Beje[i].pos.y, g_Beje[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Beje[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Beje[0].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

void SetBeje(XMFLOAT3 spos, XMFLOAT3 epos)
{
	ENEMY* enemy = GetEnemy();
	PLAYER* player = GetPlayer();
	CAMERA* cam = GetCamera();

	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < BEJE_MAX; i++)
	{
		if (g_Beje[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Beje[i].use = TRUE;			// 使用状態へ変更する


			g_Beje[i].hound = GetNearEnemy();

			if (g_Beje[i].hound == -1)
			{
				g_Beje[i].radian = (player->dir) + cam->rot.y;
				g_Beje[i].len = 300.0f;

				XMFLOAT3 p1 = XMFLOAT3(player->pos.x - sinf(-g_Beje[i].radian) * 100.0f, 50.0f, player->pos.z - cosf(-g_Beje[i].radian) * 100.0f);
				XMFLOAT3 p3 = XMFLOAT3(player->pos.x - sinf(g_Beje[i].radian) * g_Beje[i].len, 0.0f, player->pos.z - cosf(g_Beje[i].radian) * g_Beje[i].len);
				XMFLOAT3 p2 = XMFLOAT3((p1.x + p3.x) / 2, 70.0f, (p1.z + p3.z) / 2);

				g_Beje[i].point[0] = XMLoadFloat3(&spos);//始点
				g_Beje[i].point[1] = XMLoadFloat3(&p1);//1
				g_Beje[i].point[2] = XMLoadFloat3(&p2);//2
				g_Beje[i].point[3] = XMLoadFloat3(&p3);//終点
			}
			else
			{
				g_Beje[i].radian = atan2f(enemy[g_Beje[i].hound].pos.x - g_Beje[i].pos.x, enemy[g_Beje[i].hound].pos.z - g_Beje[i].pos.z);
				g_Beje[i].len = sqrt(((player->pos.x - enemy[g_Beje[i].hound].pos.x) * (player->pos.x - enemy[g_Beje[i].hound].pos.x)) + ((player->pos.z - enemy[g_Beje[i].hound].pos.z) * (player->pos.z - enemy[g_Beje[i].hound].pos.z)));

				XMFLOAT3 p1 = XMFLOAT3(player->pos.x - sinf(-g_Beje[i].radian) * 100.0f, 50.0f, player->pos.z - cosf(-g_Beje[i].radian) * 100.0f);
				XMFLOAT3 p2 = XMFLOAT3((p1.x + enemy[g_Beje[i].hound].pos.x) / 2, 70.0f, (p1.z + enemy[g_Beje[i].hound].pos.z) / 2);

				g_Beje[i].point[0] = XMLoadFloat3(&spos);//始点
				g_Beje[i].point[1] = XMLoadFloat3(&p1);//1
				g_Beje[i].point[2] = XMLoadFloat3(&p2);//2
				g_Beje[i].point[3] = XMLoadFloat3(&epos);//終点
			}


			g_Beje[i].vec[0] = g_Beje[i].point[1] - g_Beje[i].point[0];//始めー＞１
			g_Beje[i].vec[1] = g_Beje[i].point[2] - g_Beje[i].point[1];//1->2
			g_Beje[i].vec[2] = g_Beje[i].point[3] - g_Beje[i].point[2];//2->end

			g_Beje[i].count = 0.0f;

			return;							// 1発セットしたので終了する
		}
		else
		{
			continue;
		}
	}
}


void SetBejeFive(XMFLOAT3 spos, XMFLOAT3 epos)
{
	ENEMY* enemy = GetEnemy();
	PLAYER* player = GetPlayer();
	CAMERA* cam = GetCamera();

	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < BEJE_MAX; i++)
	{
		if (g_Beje[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Beje[i].use = TRUE;			// 使用状態へ変更する


			g_Beje[i].hound = GetNearEnemy();

			if (g_Beje[i].hound == -1)
			{
				g_Beje[i].radian = (player->dir) + cam->rot.y;
				g_Beje[i].len = 300.0f;

				int x = rand() % (int)(200) - 200;
				int z = rand() % (int)(200) - 200;

				int ex = rand() % (int)(20) - 20;
				int ez = rand() % (int)(20) - 20;

				XMFLOAT3 p1 = XMFLOAT3(player->pos.x - sinf(-g_Beje[i].radian) * 100.0f + x, FiveCnt * 50.0f, player->pos.z - cosf(-g_Beje[i].radian) * 100.0f + z);
				XMFLOAT3 p3 = XMFLOAT3(player->pos.x - sinf(g_Beje[i].radian) * g_Beje[i].len + ex, 0.0f, player->pos.z - cosf(g_Beje[i].radian) * g_Beje[i].len + ez);
				XMFLOAT3 p2 = XMFLOAT3((p1.x + p3.x) / 2, 70.0f, (p1.z + p3.z) / 2);

				g_Beje[i].point[0] = XMLoadFloat3(&spos);//始点
				g_Beje[i].point[1] = XMLoadFloat3(&p1);//1
				g_Beje[i].point[2] = XMLoadFloat3(&p2);//2
				g_Beje[i].point[3] = XMLoadFloat3(&p3);//終点
			}
			else
			{
				g_Beje[i].radian = atan2f(enemy[g_Beje[i].hound].pos.x - g_Beje[i].pos.x, enemy[g_Beje[i].hound].pos.z - g_Beje[i].pos.z);
				g_Beje[i].len = sqrt(((player->pos.x - enemy[g_Beje[i].hound].pos.x) * (player->pos.x - enemy[g_Beje[i].hound].pos.x)) + ((player->pos.z - enemy[g_Beje[i].hound].pos.z) * (player->pos.z - enemy[g_Beje[i].hound].pos.z)));

				int x = rand() % (int)(200) - 200;
				int z = rand() % (int)(200) - 200;

				XMFLOAT3 p1 = XMFLOAT3(player->pos.x - sinf(-g_Beje[i].radian) * 100.0f + x, FiveCnt * 50.0f, player->pos.z - cosf(-g_Beje[i].radian) * 100.0f + z);
				XMFLOAT3 p2 = XMFLOAT3((p1.x + enemy[g_Beje[i].hound].pos.x) / 2, 70.0f, (p1.z + enemy[g_Beje[i].hound].pos.z) / 2);

				g_Beje[i].point[0] = XMLoadFloat3(&spos);//始点
				g_Beje[i].point[1] = XMLoadFloat3(&p1);//1
				g_Beje[i].point[2] = XMLoadFloat3(&p2);//2
				g_Beje[i].point[3] = XMLoadFloat3(&epos);//終点
			}


			g_Beje[i].vec[0] = g_Beje[i].point[1] - g_Beje[i].point[0];//始めー＞１
			g_Beje[i].vec[1] = g_Beje[i].point[2] - g_Beje[i].point[1];//1->2
			g_Beje[i].vec[2] = g_Beje[i].point[3] - g_Beje[i].point[2];//2->end

			g_Beje[i].count = 0.0f;

			FiveCnt++;

			if (FiveCnt == 5)
			{
				FiveCnt = 0;
				return;							// 5発セットしたので終了する
			}
		}
	}
}

BEJE* GetBeje(void)
{
	return &(g_Beje[0]);
}
