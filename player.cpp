//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "enemy.h"
#include "collision.h"
#include "straight.h"
#include "beje.h"
#include "lookon.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/houdai.obj"			// 読み込むモデル名

#define	VALUE_MOVE			(0.01f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(10.0f)							// プレイヤーの足元をあわせる


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER				g_Player;						// プレイヤー

BOOL						camSet = TRUE;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;

	ENEMY* enemy = GetEnemy();
	g_Player.len = 150.0f;
	g_Player.rad = atan2f(enemy[0].pos.x - g_Player.pos.x, enemy[0].pos.z - g_Player.pos.z);

	g_Player.pos = { float(enemy[0].pos.x + sin(g_Player.rad) * g_Player.len), PLAYER_OFFSET_Y, float(enemy[0].pos.z + cos(g_Player.rad) * g_Player.len) };
	g_Player.rot = { XM_PI / 8 * 3, 0.0f, 0.0f };
	g_Player.scl = { 0.5f, 0.5f, 0.5f };
	g_Player.moveRot = PLAYER_RIGHT;

	g_Player.hp = 30;			// 移動スピードクリア

	g_Player.spd = 0.0f;			// 移動スピードクリア
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	g_Player.dashing = FALSE;
	g_Player.dashCnt = 0;

	g_Player.use = TRUE;

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}


}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA* cam = GetCamera();
	ENEMY* enemy = GetEnemy();

	if (GetKeyboardTrigger(DIK_SPACE) || IsButtonPressed(0, BUTTON_Z))
	{	// ダッシュ
		g_Player.dashing = TRUE;
	}

	// 移動させちゃう
	if (GetKeyboardPress(DIK_LEFT) || IsButtonPressed(0,BUTTON_LEFT))
	{	// 左へ移動
		g_Player.spd = VALUE_MOVE;
		if (g_Player.dashing)g_Player.spd *= 3;

		g_Player.rad += g_Player.spd;
		g_Player.moveRot = PLAYER_LEFT;
	}
	if (GetKeyboardPress(DIK_RIGHT) || IsButtonPressed(0, BUTTON_RIGHT))
	{	// 右へ移動
		g_Player.spd = VALUE_MOVE;
		if (g_Player.dashing)g_Player.spd *= 3;

		g_Player.rad -= g_Player.spd;
		g_Player.moveRot = PLAYER_RIGHT;
	}


#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.rot.y = g_Player.dir = 0.0f;
		g_Player.spd = 0.0f;
	}
#endif


	if (g_Player.dashing)
	{
		g_Player.dashCnt++;

		if (g_Player.dashCnt >= 10)
		{
			g_Player.dashCnt = 0;
			g_Player.dashing = FALSE;
		}
	}
	//	// Key入力があったら移動処理する
	if (g_Player.spd > 0.0f)
	{
		float radian = atan2f((g_Player.pos.x - enemy[0].pos.x), (g_Player.pos.z - enemy[0].pos.z));
		g_Player.rot.y = radian;// +(180.0f * 3.14f / 180.0f);//プレイヤーの向きを代入

		// 入力のあった方向へプレイヤーを向かせて移動させる
		g_Player.pos = { float(enemy[0].pos.x + sin(g_Player.rad) * g_Player.len), PLAYER_OFFSET_Y, float(enemy[0].pos.z + cos(g_Player.rad) * g_Player.len) };
	}


	XMFLOAT3 p_pos = g_Player.pos;
	p_pos.x -= sinf(g_Player.rot.y) * 20.0f;
	p_pos.y += 10.0f;
	p_pos.z -= cosf(g_Player.rot.y) * 20.0f;

	int n = GetNearEnemy();
	XMFLOAT3 e_pos = enemy[n].pos;

	if (GetKeyboardTrigger(DIK_H) || IsButtonTriggered(0, BUTTON_B))
	{	// 直線弾発射
		XMFLOAT3 p_pos = g_Player.pos;
		p_pos.x -= sinf(g_Player.rot.y) * 20.0f;
		p_pos.y += 10.0f;
		p_pos.z -= cosf(g_Player.rot.y) * 20.0f;

		SetStraight(p_pos, e_pos);
		PlaySound(SOUND_LABEL_SE_SHOT);
	}

	if (GetKeyboardTrigger(DIK_J) || IsButtonTriggered(0, BUTTON_A))
	{	// ベジェ弾1発発射
		XMFLOAT3 p_pos = g_Player.pos;
		p_pos.x -= sinf(g_Player.rot.y) * 25.0f;
		p_pos.y += 12.0f;
		p_pos.z -= cosf(g_Player.rot.y) * 25.0f;

		SetBeje(p_pos, e_pos);
		PlaySound(SOUND_LABEL_SE_SHOT);
	}

	if (GetKeyboardTrigger(DIK_K) || IsButtonTriggered(0, BUTTON_X))
	{	// ベジェ弾5発発射
		XMFLOAT3 p_pos = g_Player.pos;
		p_pos.x -= sinf(g_Player.rot.y) * 25.0f;
		p_pos.y += 12.0f;
		p_pos.z -= cosf(g_Player.rot.y) * 25.0f;

		SetBejeFive(p_pos, e_pos);
		PlaySound(SOUND_LABEL_SE_SHOT);
	}

	// 影もプレイヤーの位置に合わせる
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);


	g_Player.spd = 0.0f;


	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE)continue;

		if (CollisionBC(g_Player.pos, enemy[i].pos, g_Player.size, enemy[i].size))
		{
			enemy[i].use = FALSE;
			ReleaseShadow(enemy[i].shadowIdx);
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

	SetFuchi(1, XMFLOAT2(0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

	// モデル描画
	DrawModel(&g_Player.model);

	SetFuchi(0, XMFLOAT2(0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 01.0f, 0.0f));

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player;
}

