//=============================================================================
//
// �؏��� [bakuha.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitLookon(void);
void UninitLookon(void);
void UpdateLookon(void);
void DrawLookon(void);

int SetLookon(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);

int SortEnemyDis();
int GetNearEnemy();

