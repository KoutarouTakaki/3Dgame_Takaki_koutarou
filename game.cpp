//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "score.h"
#include "particle.h"
#include "collision.h"
#include "debugproc.h"
#include "lightning.h"
#include "renderer.h"
#include "snow.h"
#include "straight.h"
#include "lookon.h"
#include "beje.h"
#include "e_bullet.h"
#include "sky.h"
#include "life.h"
#include "noise.h"
#include "boost.h"
#include "bomb.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// ポーズON/OFF


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, .0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// 空の初期化
	InitSky(XMFLOAT3(0.0f, -100.0f, 0.0f), XMFLOAT3(-XM_PI / 2, 0.0f, 0.0f), 100, 100, 50.0f, 50.0f);

	//// 影の初期化処理
	InitShadow();

	// プレイヤーの初期化
	InitPlayer();

	// エネミーの初期化
	InitEnemy();

	//// 壁の初期化
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	//// 壁(裏側用の半透明)
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// スコアの初期化
	InitScore();

	// パーティクルの初期化
	InitParticle();

	// ライトニングの初期化
	InitLightning();

	// 爆発の初期化
	InitBomb();

	//火の粉の初期化
	InitSnow();

	//直線弾の初期化
	InitStraight();

	//ロックオンの初期化
	InitLookon();
	
	//ベジェ弾の初期化
	InitBeje();

	//敵の弾の初期化
	InitEbullet();

	//敵のHP表示の初期化
	InitLife();

	//弾の炎の初期化
	InitBoost();
	
	// BGM再生

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	// パーティクルの終了処理
	UninitParticle();

	// ライトニングの終了処理
	UninitLightning();

	// スコアの終了処理
	UninitScore();

	//// 壁の終了処理
	//UninitMeshWall();

	// 地面の終了処理
	UninitMeshField();

	// 空の終了処理
	UninitSky();

	// エネミーの終了処理
	UninitEnemy();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

	//火の粉の終了処理
	UninitSnow();

	//直線弾の終了処理
	UninitStraight();

	//ロックオンの終了処理
	UninitLookon();

	//ベジェ弾の終了処理
	UninitBeje();

	//敵の弾の終了処理
	UninitEbullet();

	//敵のHPの終了処理
	UninitLife();

	//弾の炎の終了処理
	UninitBoost();

	//爆発の終了処理
	UninitBomb();
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}


#endif

	if(g_bPause == FALSE)
		return;

	// 地面処理の更新
	UpdateMeshField();

	// 空処理の更新
	UpdateSky();

	// プレイヤーの更新処理
	UpdatePlayer();

	// エネミーの更新処理
	UpdateEnemy();

	//// 壁処理の更新
	//UpdateMeshWall();

	//ロックオンの更新処理
	UpdateLookon();
	
	//ベジェ弾の更新処理
	UpdateBeje();

	//直線弾の更新処理
	UpdateStraight();

	//敵の弾の更新処理
	UpdateEbullet();

	// パーティクルの更新処理
	UpdateParticle();

	//弾の炎の更新処理
	UpdateBoost();

	// ライトニングの更新処理
	UpdateLightning();

	//爆発の更新処理
	UpdateBomb();

	//火の粉の更新処理
	UpdateSnow();

	// 影の更新処理
	UpdateShadow();

	// 当たり判定処理
	CheckHit();

	// スコアの更新処理
	UpdateScore();

	// 敵のHPの更新処理
	UpdateLife();

	//rand値の更新
	SettingHash(rand());
	srand(rand());
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame0(void)
{
	// 3Dの物を描画する処理
	// 地面の描画処理
	DrawMeshField();

	// 空処理の更新
	DrawSky();

	// 影の描画処理
	DrawShadow();

	// エネミーの描画処理
	DrawEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	//ベジェの描画処理
	DrawBeje();

	//直線弾の描画処理
	DrawStraight();

	//敵の弾の描画処理
	DrawEbullet();

	// 壁の描画処理
	DrawMeshWall();

	//ロックオン
	//DrawLookon();

	// パーティクルの描画処理
	DrawParticle();

	//弾の炎の描画処理
	DrawBoost();

	// ライトニングの描画処理
	DrawLightning();

	//爆発の描画処理
	DrawBomb();

	//火の粉の描画処理
	DrawSnow();

	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);

	// スコアの描画処理
	DrawScore();

	// 敵のHPの描画処理
	DrawLife();

	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	//// プレイヤー視点
	//pos = GetPlayer()->pos;
	//pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
	//SetCameraAT(pos);
	//SetCamera();

	// プレイヤー視点
	ENEMY* enemy = GetEnemy();
	pos = enemy[0].pos;
	SetCameraAT(pos);
	SetCamera();

	PLAYER* player = GetPlayer();	// プレイヤーのポインターを初期化
	CAMERA* camera = GetCamera();
	PrintDebugProc("Ppos:x %f,y %f,z %f\n", player->pos.x, player->pos.y, player->pos.z);
	PrintDebugProc("Cpos:x %f,y %f,z %f\n", camera->pos.x, camera->pos.y, camera->pos.z);

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();		// エネミーのポインターを初期化
	PLAYER *player = GetPlayer();	// プレイヤーのポインターを初期化
	STRAIGHT* straight = GetStraight();
	BEJE* beje = GetBeje();
	EBULLET* ebullet = GetEbullet();

	// 敵とプレイヤーキャラ
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//敵の有効フラグをチェックする
		if (enemy[i].use == FALSE)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
		{
			// 敵キャラクターは倒される
			//enemy[i].use = FALSE;
			//DISSOLVE dissolve;
			//dissolve.DissolveFlug = 1;
			//SetDissolve(dissolve.DissolveFlug, &dissolve);
			ReleaseShadow(enemy[i].shadowIdx);

			// スコアを足す
		}
	}


	// プレイヤーの弾(straight)と敵
	for (int i = 0; i < STRAIGHT_MAX; i++)
	{
		//弾の有効フラグをチェックする
		if (straight[i].use == FALSE) continue;

		// エネミーの数分当たり判定を行う
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			// 生きてるエネミーと当たり判定をする
			if (enemy[j].use == TRUE)
			{
				BOOL ans = CollisionBC(straight[i].pos, enemy[j].pos, STRAIGHT_SIZE, ENEMY_SIZE);
				// 当たっている？
				if (ans == TRUE)
				{
					// 当たった時の処理
					straight[i].use = FALSE;
					enemy[j].hp -= 100;
					PlaySound(SOUND_LABEL_SE_EXP);
					// パーティクル発生
					for(int n = 0; n < 100; n++)
					{
						XMFLOAT3 pos;
						XMFLOAT3 move;
						float fAngle, fLength;
						int nLife;
						float fSize;
						float g_fWidthBase = 5.0f;			// 基準の幅
						float g_fHeightBase = 1.0f;			// 基準の高さ

						pos = enemy[0].pos;
						fAngle = (float)(rand() % 628) / 100.0f;

						fLength = rand() % (int)(g_fWidthBase * 20) / 100.0f - g_fWidthBase;
						move.x = sinf(fAngle) * fLength;
						move.y = (float)(rand() % 5 - 3);
						move.z = cosf(fAngle) * fLength;

						nLife = rand() % 20 + 10;
						fSize = (float)(rand() % 100) / 100;

						// ビルボードの設定
						SetBomb(pos, move, XMFLOAT4(0.9f, 0.2f, 0.1f, 1.0f), fSize, fSize, nLife);
					}
					if (enemy[j].hp == 0)
					{
						//enemy[j].use = FALSE;
						ReleaseShadow(enemy[j].shadowIdx);

					}
				}
			}
		}
	}

	// プレイヤーの弾(beje)と敵
	for (int i = 0; i < BEJE_MAX; i++)
	{
		//弾の有効フラグをチェックする
		if (beje[i].use == FALSE) continue;

		// エネミーの数分当たり判定を行う
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			// 生きてるエネミーと当たり判定をする
			if (enemy[j].use == TRUE)
			{
				BOOL ans = CollisionBC(beje[i].pos, enemy[j].pos, BEJE_SIZE, ENEMY_SIZE);
				// 当たっている？
				if (ans == TRUE)
				{
					// 当たった時の処理
					beje[i].use = FALSE;
					enemy[j].hp -= 100;
					PlaySound(SOUND_LABEL_SE_EXP);
					for (int n = 0; n < 100; n++)
					{
						XMFLOAT3 pos;
						XMFLOAT3 move;
						float fAngle, fLength;
						int nLife;
						float fSize;
						float g_fWidthBase = 5.0f;			// 基準の幅
						float g_fHeightBase = 1.0f;			// 基準の高さ

						pos = enemy[0].pos;
						fAngle = (float)(rand() % 628) / 100.0f;

						fLength = rand() % (int)(g_fWidthBase * 20) / 100.0f - g_fWidthBase;
						move.x = sinf(fAngle) * fLength;
						move.y = (float)(rand() % 5 - 3);
						move.z = cosf(fAngle) * fLength;

						nLife = rand() % 20 + 10;
						fSize = (float)(rand() % 100) / 100;

						// ビルボードの設定
						SetBomb(pos, move, XMFLOAT4(0.9f, 0.2f, 0.1f, 1.0f), fSize, fSize, nLife);
					}
					if (enemy[j].hp == 0)
					{
						//enemy[j].use = FALSE;
						ReleaseShadow(enemy[j].shadowIdx);

					}
				}
			}
		}

	}

	// プレイヤーの弾(straight)と敵
	for (int i = 0; i < EBULLET_MAX; i++)
	{
		//弾の有効フラグをチェックする
		if (ebullet[i].use == FALSE) continue;

		// エネミーの数分当たり判定を行う
		for (int j = 0; j < MAX_PLAYER; j++)
		{
			// 生きてるエネミーと当たり判定をする
			if (player->use == TRUE)
			{
				BOOL ans = CollisionBC(ebullet[i].pos, player->pos, EBULLET_SIZE, PLAYER_SIZE);
				// 当たっている？
				if (ans == TRUE)
				{
					// 当たった時の処理
					ebullet[i].use = FALSE;

					player->hp--;

					PlaySound(SOUND_LABEL_SE_EXP);

					if (player->hp == 0)
					{
						player->use = FALSE;
						ReleaseShadow(player->shadowIdx);

					}
				}
			}
		}
	}

	// エネミーが全部死亡したら状態遷移
	int enemy_count = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE) continue;
		enemy_count++;
	}

	// エネミーが０匹？
	if (enemy_count == 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}

	// プレイヤーが全部死亡したら状態遷移
	int player_count = 0;
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (player->use == FALSE) continue;
		player_count++;
	}

	// プレイヤーが０匹？
	if (player_count == 0)
	{
		SetFade(FADE_OUT, MODE_GAMEOVER);
	}

}


