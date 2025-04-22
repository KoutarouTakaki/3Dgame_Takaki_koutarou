//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : 
//
//=============================================================================
#pragma once
#include <time.h>
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitLightning(void);
void UninitLightning(void);
void UpdateLightning(void);
void DrawLightning(void);

int SetLightning(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetColorLightning(int nIdxParticle, XMFLOAT4 col);

