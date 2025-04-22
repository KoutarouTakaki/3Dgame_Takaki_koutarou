//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBoost(void);
void UninitBoost(void);
void UpdateBoost(void);
void DrawBoost(void);

int SetBoost(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetColorBoost(int nIdxBoost, XMFLOAT4 col);

