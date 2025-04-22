//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSnow(void);
void UninitSnow(void);
void UpdateSnow(void);
void DrawSnow(void);

int SetSnow(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife);
void SetColorSnow(int nIdxParticle, XMFLOAT4 col);

