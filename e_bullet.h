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

//*****************************************************************************
// �}�N����`
//*****************************************************************************
struct EBULLET
{
	XMFLOAT3		pos;		// �|���S���̈ʒu
	XMFLOAT3		rot;		// �|���S���̌���(��])
	XMFLOAT3		scl;		// �|���S���̑傫��(�X�P�[��)
	XMFLOAT3		move;		// �|���S���̈ʒu
	XMFLOAT3		Bmove;		// �|���S���̈ʒu

	BOOL			use;
	BOOL			load;
	DX11_MODEL		model;				// ���f�����
	XMFLOAT4		diffuse[MODEL_MAX_MATERIAL];	// ���f���̐F

	float			spd;				// �ړ��X�s�[�h
	float			size;				// �����蔻��̑傫��
	int				shadowIdx;			// �e�̃C���f�b�N�X�ԍ�
	float			radian;
	float			count;

	XMVECTOR sPoint[2];
	XMVECTOR vec;

	XMFLOAT4X4		mtxWorld;	// ���[���h�}�g���b�N�X
};

#define EBULLET_SIZE	(15.0f)
#define EBULLET_MAX		(100)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEbullet(void);
void UninitEbullet(void);
void UpdateEbullet(void);
void DrawEbullet(void);

EBULLET* GetEbullet(void);

void SetEbullet(XMFLOAT3 spos, XMFLOAT3 epos);

