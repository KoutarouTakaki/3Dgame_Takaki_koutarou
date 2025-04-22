//=============================================================================
//
// 木処理 [bakuha.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitLookon(void);
void UninitLookon(void);
void UpdateLookon(void);
void DrawLookon(void);

int SetLookon(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);

int SortEnemyDis();
int GetNearEnemy();

