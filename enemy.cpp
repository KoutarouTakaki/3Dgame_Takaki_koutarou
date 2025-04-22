//=============================================================================
//
// エネミーモデル処理 [enemy.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "enemy.h"
#include "shadow.h"
#include "player.h"
#include "e_bullet.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_ENEMY		"data/MODEL/robot/robo_body.obj"	// 読み込むモデル名

#define	MODEL_ENEMY_HIP			"data/MODEL/robot/robo_hip.obj"			// 読み込むモデル名 0
#define	MODEL_ENEMY_RARM_DOWN	"data/MODEL/robot/robo_Rarm_down.obj"	// 読み込むモデル名 1
#define	MODEL_ENEMY_RARM_UP		"data/MODEL/robot/robo_Rarm_up.obj"		// 読み込むモデル名 2
#define	MODEL_ENEMY_RHAND		"data/MODEL/robot/robo_Rhand.obj"		// 読み込むモデル名 3
#define	MODEL_ENEMY_LARM_DOWN	"data/MODEL/robot/robo_Larm_down.obj"	// 読み込むモデル名 4
#define	MODEL_ENEMY_LARM_UP		"data/MODEL/robot/robo_Larm_up.obj"		// 読み込むモデル名 5
#define	MODEL_ENEMY_LHAND		"data/MODEL/robot/robo_Lhand.obj"		// 読み込むモデル名 6
#define	MODEL_ENEMY_RLEG_DOWN	"data/MODEL/robot/robo_Rleg_down.obj"	// 読み込むモデル名 7
#define	MODEL_ENEMY_RLEG_UP		"data/MODEL/robot/robo_Rleg_up.obj"		// 読み込むモデル名 8
#define	MODEL_ENEMY_LLEG_DOWN	"data/MODEL/robot/robo_Lleg_down.obj"	// 読み込むモデル名 9
#define	MODEL_ENEMY_LLEG_UP		"data/MODEL/robot/robo_Lleg_up.obj"		// 読み込むモデル名 10

#define	VALUE_MOVE			(5.0f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

#define ENEMY_SHADOW_SIZE	(7.0f)						// 影の大きさ
#define ENEMY_OFFSET_Y		(50.0f)						// エネミーの足元をあわせる

#define ENEMY_PARTS_MAX	(12)								// プレイヤーのパーツの数
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY			g_Enemy[MAX_ENEMY];				// エネミー
static ENEMY			g_Parts[MAX_ENEMY][ENEMY_PARTS_MAX];		// プレイヤーのパーツ用

int g_Enemy_load = 0;

// エネミーの階層アニメーションデータ
// エネミーの左右パーツを動かしているアニメデータ
static INTERPOLATION_DATA move_tbl_hip[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,-12.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(0.0f,-12.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Rarm_up[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-7.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 4, 0.0f, XM_PI / 2),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-7.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, XM_PI / 2),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Rarm_down[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Rhand[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(-8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f,0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Larm_up[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(7.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f,-XM_PI / 2),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(7.0f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 4, 0.0f,-XM_PI / 2),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Larm_down[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Lhand[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(8.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl_Rleg_up[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-3.5f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(-3.5f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 4, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_Rleg_down[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,-6.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f,-6.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_Lleg_up[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(3.5f, 0.0f, 0.0f), XMFLOAT3(-XM_PI / 4, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(3.5f, 0.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_Lleg_down[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f,-6.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },
	{ XMFLOAT3(0.0f,-6.0f, 0.0f), XMFLOAT3(XM_PI / 4, 0.0f, 0.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f), 30 },

};

static INTERPOLATION_DATA move_tbl_cam[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(10.0f, 10.0f,-10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(10.0f, 10.0f,-10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f),    XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA* g_MoveTblAdrPart[] =
{
	move_tbl_hip,
	move_tbl_Rarm_up,
	move_tbl_Rarm_down,
	move_tbl_Rhand,
	move_tbl_Larm_up,
	move_tbl_Larm_down,
	move_tbl_Lhand,
	move_tbl_Rleg_up,
	move_tbl_Rleg_down,
	move_tbl_Lleg_up,
	move_tbl_Lleg_down,
	move_tbl_cam,
};

float TexBlendRate = 1.0f;
int atkMax = 100;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	TexBlendRate = 1.0f;
	atkMax = 100;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		LoadModel(MODEL_ENEMY, &g_Enemy[i].model);
		g_Enemy[i].load = TRUE;

		g_Enemy[i].pos = XMFLOAT3(0.0f, 100.0f, 0.0f);
		g_Enemy[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = XMFLOAT3(3.0f, 3.0f, 3.0f);

		g_Enemy[i].spd = 0.0f;			// 移動スピードクリア
		g_Enemy[i].size = ENEMY_SIZE;	// 当たり判定の大きさ
		g_Enemy[i].hp = 10000;
		g_Enemy[i].atkCnt = 0;

		g_Enemy[i].dissUse = FALSE;
		g_Enemy[i].dissCnt = 0;
		g_Enemy[i].TexUV = XMFLOAT2(0.0f, 0.0f);
		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Enemy[i].model, &g_Enemy[i].diffuse[0]);

		XMFLOAT3 pos = g_Enemy[i].pos;
		pos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_Enemy[i].time = 0.0f;			// 線形補間用のタイマーをクリア
		g_Enemy[i].tblNo = 0;			// 再生する行動データテーブルNoをセット
		g_Enemy[i].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット

		g_Enemy[i].use = TRUE;			// TRUE:生きてる

		// 階層アニメーション用の初期化処理
		g_Enemy[i].parent = NULL;			// 本体（親）なのでNULLを入れる

		// パーツの初期化
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_Parts[i][j].use = FALSE;

			// 位置・回転・スケールの初期設定
			g_Parts[i][j].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_Parts[i][j].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

			// 親子関係
			g_Parts[i][j].parent = &g_Enemy[i];		// ← ここに親のアドレスを入れる
			//	g_Parts[腕].parent= &g_Enemy[i];		// 腕だったら親は本体（プレイヤー）
			//	g_Parts[手].parent= &g_Paerts[腕];	// 指が腕の子供だった場合の例

				// 階層アニメーション用のメンバー変数の初期化
			g_Parts[i][j].time = 0.0f;			// 線形補間用のタイマーをクリア
			g_Parts[i][j].tblNo = 0;			// 再生する行動データテーブルNoをセット
			g_Parts[i][j].tblMax = 0;			// 再生する行動データテーブルのレコード数をセット
			g_Parts[i][j].dissCnt = 0.0f;
			// パーツの読み込みはまだしていない
			g_Parts[i][j].load = 0;
		}

		g_Parts[i][0].use = TRUE;
		g_Parts[i][0].parent = &g_Enemy[i];	// 親をセット
		g_Parts[i][0].tblNo = 0;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][0].tblMax = sizeof(move_tbl_hip) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][0].load = 1;
		LoadModel(MODEL_ENEMY_HIP, &g_Parts[i][0].model);

		g_Parts[i][1].use = TRUE;
		g_Parts[i][1].parent = &g_Enemy[i];	// 親をセット
		g_Parts[i][1].tblNo = 1;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][1].tblMax = sizeof(move_tbl_Rarm_up) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][1].load = 1;
		LoadModel(MODEL_ENEMY_RARM_UP, &g_Parts[i][1].model);

		g_Parts[i][2].use = TRUE;
		g_Parts[i][2].parent = &g_Parts[i][1];	// 親をセット
		g_Parts[i][2].tblNo = 2;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][2].tblMax = sizeof(move_tbl_Rarm_down) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][2].load = 1;
		LoadModel(MODEL_ENEMY_RARM_DOWN, &g_Parts[i][2].model);

		g_Parts[i][3].use = TRUE;
		g_Parts[i][3].parent = &g_Parts[i][2];	// 親をセット
		g_Parts[i][3].tblNo = 3;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][3].tblMax = sizeof(move_tbl_Rhand) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][3].load = 1;
		LoadModel(MODEL_ENEMY_RHAND, &g_Parts[i][3].model);

		g_Parts[i][4].use = TRUE;
		g_Parts[i][4].parent = &g_Enemy[i];	// 親をセット
		g_Parts[i][4].tblNo = 4;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][4].tblMax = sizeof(move_tbl_Larm_up) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][4].load = 1;
		LoadModel(MODEL_ENEMY_LARM_UP, &g_Parts[i][4].model);

		g_Parts[i][5].use = TRUE;
		g_Parts[i][5].parent = &g_Parts[i][4];	// 親をセット
		g_Parts[i][5].tblNo = 5;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][5].tblMax = sizeof(move_tbl_Larm_down) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][5].load = 1;
		LoadModel(MODEL_ENEMY_LARM_DOWN, &g_Parts[i][5].model);

		g_Parts[i][6].use = TRUE;
		g_Parts[i][6].parent = &g_Parts[i][5];// 親をセット
		g_Parts[i][6].tblNo = 6;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][6].tblMax = sizeof(move_tbl_Lhand) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][6].load = 1;
		LoadModel(MODEL_ENEMY_LHAND, &g_Parts[i][6].model);

		g_Parts[i][7].use = TRUE;
		g_Parts[i][7].parent = &g_Parts[i][0];	// 親をセット
		g_Parts[i][7].tblNo = 7;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][7].tblMax = sizeof(move_tbl_Rleg_up) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][7].load = 1;
		LoadModel(MODEL_ENEMY_RLEG_UP, &g_Parts[i][7].model);

		g_Parts[i][8].use = TRUE;
		g_Parts[i][8].parent = &g_Parts[i][7];	// 親をセット
		g_Parts[i][8].tblNo = 8;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][8].tblMax = sizeof(move_tbl_Rleg_down) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][8].load = 1;
		LoadModel(MODEL_ENEMY_RLEG_DOWN, &g_Parts[i][8].model);

		g_Parts[i][9].use = TRUE;
		g_Parts[i][9].parent = &g_Parts[i][0];	// 親をセット
		g_Parts[i][9].tblNo = 9;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][9].tblMax = sizeof(move_tbl_Lleg_up) / sizeof(INTERPOLATION_DATA);		// 再生するアニメデータのレコード数をセット
		g_Parts[i][9].load = 1;
		LoadModel(MODEL_ENEMY_LLEG_UP, &g_Parts[i][9].model);

		g_Parts[i][10].use = TRUE;
		g_Parts[i][10].parent = &g_Parts[i][9];	// 親をセット
		g_Parts[i][10].tblNo = 10;			// 再生するアニメデータの先頭アドレスをセット
		g_Parts[i][10].tblMax = sizeof(move_tbl_Lleg_down) / sizeof(INTERPOLATION_DATA);	// 再生するアニメデータのレコード数をセット
		g_Parts[i][10].load = 1;
		LoadModel(MODEL_ENEMY_LLEG_DOWN, &g_Parts[i][10].model);


		// モデルの色を変更できるよ！半透明にもできるよ。
		for (int j = 0; j < g_Enemy[0].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Enemy[0].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
		}

	}
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].load)
		{
			UnloadModel(&g_Enemy[i].model);
			g_Enemy[i].load = FALSE;

		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	PLAYER* player = GetPlayer();

#ifdef _DEBUG
	if (GetKeyboardPress(DIK_5))
	{
		TexBlendRate -= 0.01f;
		if (TexBlendRate <= 0.0f)TexBlendRate = 0.0f;
	}
	if (GetKeyboardPress(DIK_6))
	{
		TexBlendRate += 0.01f;
		if (TexBlendRate >= 1.0f)TexBlendRate = 1.0f;
	}
	if (GetKeyboardPress(DIK_7))
	{

		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			g_Parts[0][j].dissCnt += 0.01f;
		}
	}
	PrintDebugProc("TexBlendRate%f\n", TexBlendRate);
#endif


	// エネミーを動かく場合は、影も合わせて動かす事を忘れないようにね！
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes
			float radian = atan2f((player->pos.x - g_Enemy[i].pos.x), (player->pos.z - g_Enemy[i].pos.z));
			g_Enemy[i].rot.y = radian + (180.0f * 3.14f / 180.0f);//プレイヤーの方を向く

			g_Enemy[i].atkCnt++;
			if (g_Enemy[i].atkCnt % atkMax == 0 && g_Enemy[i].hp > 0)
			{
				XMFLOAT3 pos = g_Enemy[i].pos;
				pos.x -= sinf(g_Enemy[i].rot.y + XM_PI / 2) * 20.0f;
				pos.y += 20.0f;
				pos.z -= cosf(g_Enemy[i].rot.y + XM_PI / 2) * 20.0f;

				SetEbullet(pos, player->pos);

				pos = g_Enemy[i].pos;
				pos.x -= sinf(g_Enemy[i].rot.y - XM_PI / 2) * 20.0f;
				pos.y += 20.0f;
				pos.z -= cosf(g_Enemy[i].rot.y - XM_PI / 2) * 20.0f;

				SetEbullet(pos, player->pos);
			}

			// テクスチャブレンド用（ブレンド用のテクスチャの座標を変えている）
			g_Enemy[i].TexUV.x += 0.001f;
			g_Enemy[i].TexUV.y += 0.001f;

			// 階層アニメーション
			for (int j = 0; j < ENEMY_PARTS_MAX; j++)
			{
				// 使われているなら処理する
				//if (g_Parts[i][j].use == TRUE)
				if ((g_Parts[i][j].use == TRUE) && (g_Parts[i][j].tblMax > 0))
				{	// 線形補間の処理
					int nowNo = (int)g_Parts[i][j].time;			// 整数分であるテーブル番号を取り出している
					int maxNo = g_Parts[i][j].tblMax;				// 登録テーブル数を数えている
					int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
					INTERPOLATION_DATA* tbl = g_MoveTblAdrPart[g_Parts[i][j].tblNo];	// 行動テーブルのアドレスを取得

					XMVECTOR nowPos = XMLoadFloat3(&tbl[nowNo].pos);	// XMVECTORへ変換
					XMVECTOR nowRot = XMLoadFloat3(&tbl[nowNo].rot);	// XMVECTORへ変換
					XMVECTOR nowScl = XMLoadFloat3(&tbl[nowNo].scl);	// XMVECTORへ変換

					XMVECTOR Pos = XMLoadFloat3(&tbl[nextNo].pos) - nowPos;	// XYZ移動量を計算している
					XMVECTOR Rot = XMLoadFloat3(&tbl[nextNo].rot) - nowRot;	// XYZ回転量を計算している
					XMVECTOR Scl = XMLoadFloat3(&tbl[nextNo].scl) - nowScl;	// XYZ拡大率を計算している

					float nowTime = g_Parts[i][j].time - nowNo;	// 時間部分である少数を取り出している

					Pos *= nowTime;								// 現在の移動量を計算している
					Rot *= nowTime;								// 現在の回転量を計算している
					Scl *= nowTime;								// 現在の拡大率を計算している

					// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
					XMStoreFloat3(&g_Parts[i][j].pos, nowPos + Pos);

					// 計算して求めた回転量を現在の移動テーブルに足している
					XMStoreFloat3(&g_Parts[i][j].rot, nowRot + Rot);

					// 計算して求めた拡大率を現在の移動テーブルに足している
					XMStoreFloat3(&g_Parts[i][j].scl, nowScl + Scl);

					// frameを使て時間経過処理をする
					g_Parts[i][j].time += 1.0f / tbl[nowNo].frame;	// 時間を進めている
					if ((int)g_Parts[i][j].time >= maxNo)			// 登録テーブル最後まで移動したか？
					{
						g_Parts[i][j].time -= maxNo;				// ０番目にリセットしつつも小数部分を引き継いでいる
					}

					//ディゾルブ***********************************************************************************************
					//HPによってディゾルブとテクスチャブレンドを進める
					if (g_Enemy[i].hp < 9000)
					{
						g_Parts[i][10].dissCnt += 0.01f;
						TexBlendRate = 0.9f;
					}
					if (g_Enemy[i].hp < 8000)
					{
						g_Parts[i][9].dissCnt += 0.01f;
						TexBlendRate = 0.8f;
						atkMax = 95;
					}
					if (g_Enemy[i].hp < 7000)
					{
						g_Parts[i][8].dissCnt += 0.01f;
						TexBlendRate = 0.7f;
						atkMax = 90;
					}
					if (g_Enemy[i].hp < 6500)
					{
						g_Parts[i][7].dissCnt += 0.01f;
						TexBlendRate = 0.6f;
						atkMax = 85;
					}
					if (g_Enemy[i].hp < 6000)
					{
						g_Parts[i][6].dissCnt += 0.01f;
						TexBlendRate = 0.5f;
						atkMax = 80;
					}
					if (g_Enemy[i].hp < 5500)
					{
						g_Parts[i][5].dissCnt += 0.01f;
						TexBlendRate = 0.4f;
						atkMax = 70;
					}
					if (g_Enemy[i].hp < 5000)
					{
						g_Parts[i][4].dissCnt += 0.01f;
						TexBlendRate = 0.3f;
						atkMax = 60;
					}
					if (g_Enemy[i].hp < 4000)
					{
						g_Parts[i][3].dissCnt += 0.01f;
						TexBlendRate = 0.25f;
						atkMax = 50;
					}
					if (g_Enemy[i].hp < 3000)
					{
						g_Parts[i][2].dissCnt += 0.01f;
						TexBlendRate = 0.2f;
						atkMax = 40;
					}
					if (g_Enemy[i].hp < 2000)
					{
						g_Parts[i][1].dissCnt += 0.01f;
						TexBlendRate = 0.15f;
						atkMax = 30;
					}
					if (g_Enemy[i].hp < 1000)
					{
						g_Parts[i][0].dissCnt += 0.01f;
						TexBlendRate = 0.1f;
						atkMax = 20;
					}
					if (g_Enemy[i].hp <= 0)
					{
						g_Enemy[i].dissCnt += 0.01f;
						TexBlendRate = 0.0f;
						atkMax = 10;
					}

					if (g_Parts[i][j].dissCnt != 0.0f)
					{
						g_Parts[i][j].dissCnt += 0.005f;
					}
					if (g_Parts[i][j].dissCnt >= 1.0f)
					{
						g_Parts[i][j].use = FALSE;
					}
				}
			}

			// 影もプレイヤーの位置に合わせる
			XMFLOAT3 pos = g_Enemy[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Enemy[i].shadowIdx, pos);

			if (g_Enemy[i].dissCnt >= 1.0f || g_Enemy[i].hp <= 0.0f)
			{
				g_Enemy[i].use = FALSE;
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_Enemy[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Enemy[i].rot.x, g_Enemy[i].rot.y + XM_PI, g_Enemy[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Enemy[i].mtxWorld, mtxWorld);

		if (i == 0)SetFuchi(1, g_Enemy[i].TexUV, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

		SetDissolve(1, g_Enemy[0].dissCnt);

		SetTexBrend(1, TexBlendRate, g_Enemy[i].TexUV);
		//SetTexBrend(1, TexBlendRate, 0);

		// モデル描画
		DrawModel(&g_Enemy[i].model);

		SetTexBrend(0, TexBlendRate, g_Enemy[i].TexUV);
		//SetTexBrend(1, TexBlendRate, 0);

		SetDissolve(0, 0.0f);

		//
		SetFuchi(1, g_Enemy[i].TexUV, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

		// パーツの階層アニメーション
		for (int j = 0; j < ENEMY_PARTS_MAX; j++)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Parts[i][j].scl.x, g_Parts[i][j].scl.y, g_Parts[i][j].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 回転を反映
			mtxRot = XMMatrixRotationRollPitchYaw(g_Parts[i][j].rot.x, g_Parts[i][j].rot.y, g_Parts[i][j].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Parts[i][j].pos.x, g_Parts[i][j].pos.y, g_Parts[i][j].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			if (g_Parts[i][j].parent != NULL)	// 子供だったら親と結合する
			{
				mtxWorld = XMMatrixMultiply(mtxWorld, XMLoadFloat4x4(&g_Parts[i][j].parent->mtxWorld));
				// ↑
				// g_Enemy[i].mtxWorldを指している
			}

			XMStoreFloat4x4(&g_Parts[i][j].mtxWorld, mtxWorld);

			// 使われているなら処理する。ここまで処理している理由は他のパーツがこのパーツを参照している可能性があるから。
			if (g_Parts[i][j].use == FALSE) continue;

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			SetDissolve(1, g_Parts[i][j].dissCnt);

			SetTexBrend(1, TexBlendRate, g_Enemy[i].TexUV);

			// モデル描画
			DrawModel(&g_Parts[i][j].model);

			SetTexBrend(0, TexBlendRate, g_Enemy[i].TexUV);

			SetDissolve(0, 0.0f);

		}
		SetFuchi(0, g_Enemy[i].TexUV, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	}


	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
ENEMY* GetEnemy()
{
	return &g_Enemy[0];
}
