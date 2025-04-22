//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "game.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "shadow.h"
#include "score.h"
#include "particle.h"
#include "collision.h"
#include "debugproc.h"
#include "lightning.h"
#include "renderer.h"
#include "snow.h"
#include "straight.h"
#include "lookon.h"
#include "beje.h"
#include "e_bullet.h"
#include "sky.h"
#include "life.h"
#include "noise.h"
#include "boost.h"
#include "bomb.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static BOOL	g_bPause = TRUE;	// �|�[�YON/OFF


//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, .0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// ��̏�����
	InitSky(XMFLOAT3(0.0f, -100.0f, 0.0f), XMFLOAT3(-XM_PI / 2, 0.0f, 0.0f), 100, 100, 50.0f, 50.0f);

	//// �e�̏���������
	InitShadow();

	// �v���C���[�̏�����
	InitPlayer();

	// �G�l�~�[�̏�����
	InitEnemy();

	//// �ǂ̏�����
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	//// ��(�����p�̔�����)
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// �X�R�A�̏�����
	InitScore();

	// �p�[�e�B�N���̏�����
	InitParticle();

	// ���C�g�j���O�̏�����
	InitLightning();

	// �����̏�����
	InitBomb();

	//�΂̕��̏�����
	InitSnow();

	//�����e�̏�����
	InitStraight();

	//���b�N�I���̏�����
	InitLookon();
	
	//�x�W�F�e�̏�����
	InitBeje();

	//�G�̒e�̏�����
	InitEbullet();

	//�G��HP�\���̏�����
	InitLife();

	//�e�̉��̏�����
	InitBoost();
	
	// BGM�Đ�

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{
	// �p�[�e�B�N���̏I������
	UninitParticle();

	// ���C�g�j���O�̏I������
	UninitLightning();

	// �X�R�A�̏I������
	UninitScore();

	//// �ǂ̏I������
	//UninitMeshWall();

	// �n�ʂ̏I������
	UninitMeshField();

	// ��̏I������
	UninitSky();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �v���C���[�̏I������
	UninitPlayer();

	// �e�̏I������
	UninitShadow();

	//�΂̕��̏I������
	UninitSnow();

	//�����e�̏I������
	UninitStraight();

	//���b�N�I���̏I������
	UninitLookon();

	//�x�W�F�e�̏I������
	UninitBeje();

	//�G�̒e�̏I������
	UninitEbullet();

	//�G��HP�̏I������
	UninitLife();

	//�e�̉��̏I������
	UninitBoost();

	//�����̏I������
	UninitBomb();
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}


#endif

	if(g_bPause == FALSE)
		return;

	// �n�ʏ����̍X�V
	UpdateMeshField();

	// �󏈗��̍X�V
	UpdateSky();

	// �v���C���[�̍X�V����
	UpdatePlayer();

	// �G�l�~�[�̍X�V����
	UpdateEnemy();

	//// �Ǐ����̍X�V
	//UpdateMeshWall();

	//���b�N�I���̍X�V����
	UpdateLookon();
	
	//�x�W�F�e�̍X�V����
	UpdateBeje();

	//�����e�̍X�V����
	UpdateStraight();

	//�G�̒e�̍X�V����
	UpdateEbullet();

	// �p�[�e�B�N���̍X�V����
	UpdateParticle();

	//�e�̉��̍X�V����
	UpdateBoost();

	// ���C�g�j���O�̍X�V����
	UpdateLightning();

	//�����̍X�V����
	UpdateBomb();

	//�΂̕��̍X�V����
	UpdateSnow();

	// �e�̍X�V����
	UpdateShadow();

	// �����蔻�菈��
	CheckHit();

	// �X�R�A�̍X�V����
	UpdateScore();

	// �G��HP�̍X�V����
	UpdateLife();

	//rand�l�̍X�V
	SettingHash(rand());
	srand(rand());
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame0(void)
{
	// 3D�̕���`�悷�鏈��
	// �n�ʂ̕`�揈��
	DrawMeshField();

	// �󏈗��̍X�V
	DrawSky();

	// �e�̕`�揈��
	DrawShadow();

	// �G�l�~�[�̕`�揈��
	DrawEnemy();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	//�x�W�F�̕`�揈��
	DrawBeje();

	//�����e�̕`�揈��
	DrawStraight();

	//�G�̒e�̕`�揈��
	DrawEbullet();

	// �ǂ̕`�揈��
	DrawMeshWall();

	//���b�N�I��
	//DrawLookon();

	// �p�[�e�B�N���̕`�揈��
	DrawParticle();

	//�e�̉��̕`�揈��
	DrawBoost();

	// ���C�g�j���O�̕`�揈��
	DrawLightning();

	//�����̕`�揈��
	DrawBomb();

	//�΂̕��̕`�揈��
	DrawSnow();

	// 2D�̕���`�悷�鏈��
	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	// �X�R�A�̕`�揈��
	DrawScore();

	// �G��HP�̕`�揈��
	DrawLife();

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// Z��r����
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// �f�o�b�O�\��
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	//// �v���C���[���_
	//pos = GetPlayer()->pos;
	//pos.y = 0.0f;			// �J����������h�����߂ɃN���A���Ă���
	//SetCameraAT(pos);
	//SetCamera();

	// �v���C���[���_
	ENEMY* enemy = GetEnemy();
	pos = enemy[0].pos;
	SetCameraAT(pos);
	SetCamera();

	PLAYER* player = GetPlayer();	// �v���C���[�̃|�C���^�[��������
	CAMERA* camera = GetCamera();
	PrintDebugProc("Ppos:x %f,y %f,z %f\n", player->pos.x, player->pos.y, player->pos.z);
	PrintDebugProc("Cpos:x %f,y %f,z %f\n", camera->pos.x, camera->pos.y, camera->pos.z);

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// �G�l�~�[���_
		pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// �����蔻�菈��
//=============================================================================
void CheckHit(void)
{
	ENEMY *enemy = GetEnemy();		// �G�l�~�[�̃|�C���^�[��������
	PLAYER *player = GetPlayer();	// �v���C���[�̃|�C���^�[��������
	STRAIGHT* straight = GetStraight();
	BEJE* beje = GetBeje();
	EBULLET* ebullet = GetEbullet();

	// �G�ƃv���C���[�L����
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//�G�̗L���t���O���`�F�b�N����
		if (enemy[i].use == FALSE)
			continue;

		//BC�̓����蔻��
		if (CollisionBC(player->pos, enemy[i].pos, player->size, enemy[i].size))
		{
			// �G�L�����N�^�[�͓|�����
			//enemy[i].use = FALSE;
			//DISSOLVE dissolve;
			//dissolve.DissolveFlug = 1;
			//SetDissolve(dissolve.DissolveFlug, &dissolve);
			ReleaseShadow(enemy[i].shadowIdx);

			// �X�R�A�𑫂�
		}
	}


	// �v���C���[�̒e(straight)�ƓG
	for (int i = 0; i < STRAIGHT_MAX; i++)
	{
		//�e�̗L���t���O���`�F�b�N����
		if (straight[i].use == FALSE) continue;

		// �G�l�~�[�̐��������蔻����s��
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			// �����Ă�G�l�~�[�Ɠ����蔻�������
			if (enemy[j].use == TRUE)
			{
				BOOL ans = CollisionBC(straight[i].pos, enemy[j].pos, STRAIGHT_SIZE, ENEMY_SIZE);
				// �������Ă���H
				if (ans == TRUE)
				{
					// �����������̏���
					straight[i].use = FALSE;
					enemy[j].hp -= 100;
					PlaySound(SOUND_LABEL_SE_EXP);
					// �p�[�e�B�N������
					for(int n = 0; n < 100; n++)
					{
						XMFLOAT3 pos;
						XMFLOAT3 move;
						float fAngle, fLength;
						int nLife;
						float fSize;
						float g_fWidthBase = 5.0f;			// ��̕�
						float g_fHeightBase = 1.0f;			// ��̍���

						pos = enemy[0].pos;
						fAngle = (float)(rand() % 628) / 100.0f;

						fLength = rand() % (int)(g_fWidthBase * 20) / 100.0f - g_fWidthBase;
						move.x = sinf(fAngle) * fLength;
						move.y = (float)(rand() % 5 - 3);
						move.z = cosf(fAngle) * fLength;

						nLife = rand() % 20 + 10;
						fSize = (float)(rand() % 100) / 100;

						// �r���{�[�h�̐ݒ�
						SetBomb(pos, move, XMFLOAT4(0.9f, 0.2f, 0.1f, 1.0f), fSize, fSize, nLife);
					}
					if (enemy[j].hp == 0)
					{
						//enemy[j].use = FALSE;
						ReleaseShadow(enemy[j].shadowIdx);

					}
				}
			}
		}
	}

	// �v���C���[�̒e(beje)�ƓG
	for (int i = 0; i < BEJE_MAX; i++)
	{
		//�e�̗L���t���O���`�F�b�N����
		if (beje[i].use == FALSE) continue;

		// �G�l�~�[�̐��������蔻����s��
		for (int j = 0; j < MAX_ENEMY; j++)
		{
			// �����Ă�G�l�~�[�Ɠ����蔻�������
			if (enemy[j].use == TRUE)
			{
				BOOL ans = CollisionBC(beje[i].pos, enemy[j].pos, BEJE_SIZE, ENEMY_SIZE);
				// �������Ă���H
				if (ans == TRUE)
				{
					// �����������̏���
					beje[i].use = FALSE;
					enemy[j].hp -= 100;
					PlaySound(SOUND_LABEL_SE_EXP);
					for (int n = 0; n < 100; n++)
					{
						XMFLOAT3 pos;
						XMFLOAT3 move;
						float fAngle, fLength;
						int nLife;
						float fSize;
						float g_fWidthBase = 5.0f;			// ��̕�
						float g_fHeightBase = 1.0f;			// ��̍���

						pos = enemy[0].pos;
						fAngle = (float)(rand() % 628) / 100.0f;

						fLength = rand() % (int)(g_fWidthBase * 20) / 100.0f - g_fWidthBase;
						move.x = sinf(fAngle) * fLength;
						move.y = (float)(rand() % 5 - 3);
						move.z = cosf(fAngle) * fLength;

						nLife = rand() % 20 + 10;
						fSize = (float)(rand() % 100) / 100;

						// �r���{�[�h�̐ݒ�
						SetBomb(pos, move, XMFLOAT4(0.9f, 0.2f, 0.1f, 1.0f), fSize, fSize, nLife);
					}
					if (enemy[j].hp == 0)
					{
						//enemy[j].use = FALSE;
						ReleaseShadow(enemy[j].shadowIdx);

					}
				}
			}
		}

	}

	// �v���C���[�̒e(straight)�ƓG
	for (int i = 0; i < EBULLET_MAX; i++)
	{
		//�e�̗L���t���O���`�F�b�N����
		if (ebullet[i].use == FALSE) continue;

		// �G�l�~�[�̐��������蔻����s��
		for (int j = 0; j < MAX_PLAYER; j++)
		{
			// �����Ă�G�l�~�[�Ɠ����蔻�������
			if (player->use == TRUE)
			{
				BOOL ans = CollisionBC(ebullet[i].pos, player->pos, EBULLET_SIZE, PLAYER_SIZE);
				// �������Ă���H
				if (ans == TRUE)
				{
					// �����������̏���
					ebullet[i].use = FALSE;

					player->hp--;

					PlaySound(SOUND_LABEL_SE_EXP);

					if (player->hp == 0)
					{
						player->use = FALSE;
						ReleaseShadow(player->shadowIdx);

					}
				}
			}
		}
	}

	// �G�l�~�[���S�����S�������ԑJ��
	int enemy_count = 0;
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (enemy[i].use == FALSE) continue;
		enemy_count++;
	}

	// �G�l�~�[���O�C�H
	if (enemy_count == 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}

	// �v���C���[���S�����S�������ԑJ��
	int player_count = 0;
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		if (player->use == FALSE) continue;
		player_count++;
	}

	// �v���C���[���O�C�H
	if (player_count == 0)
	{
		SetFade(FADE_OUT, MODE_GAMEOVER);
	}

}


