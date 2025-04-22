//=============================================================================
//
// �T�E���h���� [sound.h]
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum
{
	//�T���v��
	SOUND_LABEL_BGM_sample000,	// BGM0
	SOUND_LABEL_BGM_sample001,	// BGM1
	SOUND_LABEL_BGM_sample002,	// BGM2
	SOUND_LABEL_SE_bomb000,		// ������
	SOUND_LABEL_SE_defend000,	// 
	SOUND_LABEL_SE_defend001,	// 
	SOUND_LABEL_SE_hit000,		// 
	SOUND_LABEL_SE_laser000,	// 
	SOUND_LABEL_SE_lockon000,	// 
	SOUND_LABEL_SE_shot000,		// 
	SOUND_LABEL_SE_shot001,		// 
	SOUND_LABEL_BGM_maou,		// BGM Maou
	SOUND_LABEL_BGM_maou_bgm_orchestra26,

	//�����œ��ꂽ����
	//BGM
	SOUND_LABEL_BGM_TITLE,
	SOUND_LABEL_BGM_STAGESERECT,
	SOUND_LABEL_BGM_STAGE,
	SOUND_LABEL_BGM_BOSS,
	SOUND_LABEL_BGM_RESULT,
	SOUND_LABEL_BGM_GAMEOVER,

	//SE
	SOUND_LABEL_SE_SHOT,		// �e���ˉ�
	SOUND_LABEL_SE_DASH,		// �_�b�V����
	SOUND_LABEL_SE_EXP,			// ������

	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

