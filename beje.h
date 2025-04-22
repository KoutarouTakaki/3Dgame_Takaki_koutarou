//=============================================================================
//
// �n�ʏ��� [field.h]
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
// �}�N����`
//*****************************************************************************
struct BEJE
{
	XMFLOAT3		pos;		// �|���S���̈ʒu
	XMFLOAT3		rot;		// �|���S���̌���(��])
	XMFLOAT3		scl;		// �|���S���̑傫��(�X�P�[��)

	XMVECTOR		point[4];
	XMVECTOR		vec[3];


	BOOL			use;
	BOOL			load;
	DX11_MODEL		model;				// ���f�����
	XMFLOAT4		diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	float			spd;				// �ړ��X�s�[�h
	float			size;				// �����蔻��̑傫��
	int				shadowIdx;			// �e�̃C���f�b�N�X�ԍ�
	float			radian;
	float			len;
	float			count;
	float			time;
	int				hound;

	XMFLOAT4X4		mtxWorld;	// ���[���h�}�g���b�N�X
};

#define BEJE_SIZE	(5.0f)
#define BEJE_MAX		(100)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBeje(void);
void UninitBeje(void);
void UpdateBeje(void);
void DrawBeje(void);


void SetBeje(XMFLOAT3 pos, XMFLOAT3 epos);
void SetBejeFive(XMFLOAT3 spos, XMFLOAT3 epos);
BEJE* GetBeje(void);

