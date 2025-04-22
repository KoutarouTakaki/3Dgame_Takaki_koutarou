//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : 
//
//=============================================================================
#pragma once
#include <time.h>
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitLightning(void);
void UninitLightning(void);
void UpdateLightning(void);
void DrawLightning(void);

int SetLightning(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetColorLightning(int nIdxParticle, XMFLOAT4 col);

