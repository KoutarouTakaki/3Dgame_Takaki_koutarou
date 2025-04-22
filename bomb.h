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
HRESULT InitBomb(void);
void UninitBomb(void);
void UpdateBomb(void);
void DrawBomb(void);

int SetBomb(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetColorBomb(int nIdxParticle, XMFLOAT4 col);

