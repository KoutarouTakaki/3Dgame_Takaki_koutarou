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
#include <stdio.h>
#include <math.h>



//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct BEJE
{
	XMFLOAT3		pos;		// ポリゴンの位置
	XMFLOAT3		rot;		// ポリゴンの向き(回転)
	XMFLOAT3		scl;		// ポリゴンの大きさ(スケール)

	XMVECTOR		point[4];
	XMVECTOR		vec[3];


	BOOL			use;
	BOOL			load;
	DX11_MODEL		model;				// モデル情報
	XMFLOAT4		diffuse[MODEL_MAX_MATERIAL];	// モデルの色

	float			spd;				// 移動スピード
	float			size;				// 当たり判定の大きさ
	int				shadowIdx;			// 影のインデックス番号
	float			radian;
	float			len;
	float			count;
	float			time;
	int				hound;

	XMFLOAT4X4		mtxWorld;	// ワールドマトリックス
};

#define BEJE_SIZE	(5.0f)
#define BEJE_MAX		(100)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBeje(void);
void UninitBeje(void);
void UpdateBeje(void);
void DrawBeje(void);


void SetBeje(XMFLOAT3 pos, XMFLOAT3 epos);
void SetBejeFive(XMFLOAT3 spos, XMFLOAT3 epos);
BEJE* GetBeje(void);

