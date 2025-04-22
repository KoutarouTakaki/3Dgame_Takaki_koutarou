//=============================================================================
//
// 地面処理 [field.h]
// Author : 
//
//=============================================================================
#pragma once

#include "renderer.h"
#include <random>
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct STRAIGHT
{
	XMFLOAT3		pos;		// ポリゴンの位置
	XMFLOAT3		rot;		// ポリゴンの向き(回転)
	XMFLOAT3		scl;		// ポリゴンの大きさ(スケール)
	XMFLOAT3		move;		// ポリゴンの位置
	XMFLOAT3		Bmove;		// ポリゴンの位置

	BOOL			use;
	BOOL			load;
	DX11_MODEL		model;				// モデル情報
	XMFLOAT4		diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	float			spd;				// 移動スピード
	float			size;				// 当たり判定の大きさ
	int				shadowIdx;			// 影のインデックス番号
	float			radian;
	float			count;

	XMVECTOR sPoint[2];
	XMVECTOR vec;

	XMFLOAT4X4		mtxWorld;	// ワールドマトリックス
};

#define STRAIGHT_SIZE	(15.0f)
#define STRAIGHT_MAX		(100)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitStraight(void);
void UninitStraight(void);
void UpdateStraight(void);
void DrawStraight(void);

STRAIGHT* GetStraight(void);

void SetStraight(XMFLOAT3 spos, XMFLOAT3 epos);

//int SortEnemyDis(XMFLOAT3 pos);
