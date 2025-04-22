//=============================================================================
//
// ���f������ [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "enemy.h"
#include "collision.h"
#include "straight.h"
#include "beje.h"
#include "lookon.h"
#include "sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/houdai.obj"			// �ǂݍ��ރ��f����

#define	VALUE_MOVE			(0.01f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define PLAYER_SHADOW_SIZE	(0.4f)							// �e�̑傫��
#define PLAYER_OFFSET_Y		(10.0f)							// �v���C���[�̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static PLAYER				g_Player;						// �v���C���[

BOOL						camSet = TRUE;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;

	ENEMY* enemy = GetEnemy();
	g_Player.len = 150.0f;
	g_Player.rad = atan2f(enemy[0].pos.x - g_Player.pos.x, enemy[0].pos.z - g_Player.pos.z);

	g_Player.pos = { float(enemy[0].pos.x + sin(g_Player.rad) * g_Player.len), PLAYER_OFFSET_Y, float(enemy[0].pos.z + cos(g_Player.rad) * g_Player.len) };
	g_Player.rot = { XM_PI / 8 * 3, 0.0f, 0.0f };
	g_Player.scl = { 0.5f, 0.5f, 0.5f };
	g_Player.moveRot = PLAYER_RIGHT;

	g_Player.hp = 30;			// �ړ��X�s�[�h�N���A

	g_Player.spd = 0.0f;			// �ړ��X�s�[�h�N���A
	g_Player.size = PLAYER_SIZE;	// �����蔻��̑傫��

	g_Player.dashing = FALSE;
	g_Player.dashCnt = 0;

	g_Player.use = TRUE;

	// �����Ńv���C���[�p�̉e���쐬���Ă���
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ��
	//        ���̃����o�[�ϐ������������e��Index�ԍ�


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	// ���f���̉������
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}


}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	CAMERA* cam = GetCamera();
	ENEMY* enemy = GetEnemy();

	if (GetKeyboardTrigger(DIK_SPACE) || IsButtonPressed(0, BUTTON_Z))
	{	// �_�b�V��
		g_Player.dashing = TRUE;
	}

	// �ړ��������Ⴄ
	if (GetKeyboardPress(DIK_LEFT) || IsButtonPressed(0,BUTTON_LEFT))
	{	// ���ֈړ�
		g_Player.spd = VALUE_MOVE;
		if (g_Player.dashing)g_Player.spd *= 3;

		g_Player.rad += g_Player.spd;
		g_Player.moveRot = PLAYER_LEFT;
	}
	if (GetKeyboardPress(DIK_RIGHT) || IsButtonPressed(0, BUTTON_RIGHT))
	{	// �E�ֈړ�
		g_Player.spd = VALUE_MOVE;
		if (g_Player.dashing)g_Player.spd *= 3;

		g_Player.rad -= g_Player.spd;
		g_Player.moveRot = PLAYER_RIGHT;
	}


#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.rot.y = g_Player.dir = 0.0f;
		g_Player.spd = 0.0f;
	}
#endif


	if (g_Player.dashing)
	{
		g_Player.dashCnt++;

		if (g_Player.dashCnt >= 10)
		{
			g_Player.dashCnt = 0;
			g_Player.dashing = FALSE;
		}
	}
	//	// Key���͂���������ړ���������
	if (g_Player.spd > 0.0f)
	{
		float radian = atan2f((g_Player.pos.x - enemy[0].pos.x), (g_Player.pos.z - enemy[0].pos.z));
		g_Player.rot.y = radian;// +(180.0f * 3.14f / 180.0f);//�v���C���[�̌�������

		// ���͂̂����������փv���C���[���������Ĉړ�������
		g_Player.pos = { float(enemy[0].pos.x + sin(g_Player.rad) * g_Player.len), PLAYER_OFFSET_Y, float(enemy[0].pos.z + cos(g_Player.rad) * g_Player.len) };
	}


	XMFLOAT3 p_pos = g_Player.pos;
	p_pos.x -= sinf(g_Player.rot.y) * 20.0f;
	p_pos.y += 10.0f;
	p_pos.z -= cosf(g_Player.rot.y) * 20.0f;

	int n = GetNearEnemy();
	XMFLOAT3 e_pos = enemy[n].pos;

	if (GetKeyboardTrigger(DIK_H) || IsButtonTriggered(0, BUTTON_B))
	{	// �����e����
		XMFLOAT3 p_pos = g_Player.pos;
		p_pos.x -= sinf(g_Player.rot.y) * 20.0f;
		p_pos.y += 10.0f;
		p_pos.z -= cosf(g_Player.rot.y) * 20.0f;

		SetStraight(p_pos, e_pos);
		PlaySound(SOUND_LABEL_SE_SHOT);
	}

	if (GetKeyboardTrigger(DIK_J) || IsButtonTriggered(0, BUTTON_A))
	{	// �x�W�F�e1������
		XMFLOAT3 p_pos = g_Player.pos;
		p_pos.x -= sinf(g_Player.rot.y) * 25.0f;
		p_pos.y += 12.0f;
		p_pos.z -= cosf(g_Player.rot.y) * 25.0f;

		SetBeje(p_pos, e_pos);
		PlaySound(SOUND_LABEL_SE_SHOT);
	}

	if (GetKeyboardTrigger(DIK_K) || IsButtonTriggered(0, BUTTON_X))
	{	// �x�W�F�e5������
		XMFLOAT3 p_pos = g_Player.pos;
		p_pos.x -= sinf(g_Player.rot.y) * 25.0f;
		p_pos.y += 12.0f;
		p_pos.z -= cosf(g_Player.rot.y) * 25.0f;

		SetBejeFive(p_pos, e_pos);
		PlaySound(SOUND_LABEL_SE_SHOT);
	}

	// �e���v���C���[�̈ʒu�ɍ��킹��
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);


	g_Player.spd = 0.0f;


	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE)continue;

		if (CollisionBC(g_Player.pos, enemy[i].pos, g_Player.size, enemy[i].size))
		{
			enemy[i].use = FALSE;
			ReleaseShadow(enemy[i].shadowIdx);
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("Player:�� �� �� ���@Space\n");
	PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

	SetFuchi(1, XMFLOAT2(0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));

	// ���f���`��
	DrawModel(&g_Player.model);

	SetFuchi(0, XMFLOAT2(0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 01.0f, 0.0f));

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// �v���C���[�����擾
//=============================================================================
PLAYER* GetPlayer(void)
{
	return &g_Player;
}

