#include "main.h"
#include "input.h"
#include "noise.h"
#include "renderer.h"
#include "collision.h"

int g_HashCode[HASH_CODE_TABLE_NUM] = {};

//*****************************************************************************
// �m�C�Y�p�֐�
//*****************************************************************************
//�����e�[�u���쐬
void SettingHash(unsigned int seed)
{
	//�������C�u����������
	srand(seed);

	//�n�b�V���R�[�h������
	memset(g_HashCode, 0, sizeof(unsigned int) * COUNTOF(g_HashCode));

	//�����_���e�[�u������
	const int TABLE_NUM = HASH_CODE_MAX;
	unsigned int randomTable[TABLE_NUM] = {};
	for (int i = 0; i < COUNTOF(randomTable); ++i) {
		randomTable[i] = rand() % HASH_CODE_MAX;
	}

	//�n�b�V���R�[�h����.
	for (int i = 0; i < COUNTOF(g_HashCode); ++i) {
		g_HashCode[i] = randomTable[i % TABLE_NUM];
	}
}

//�����l�擾
unsigned int GetHash(int x, int y)
{
	x %= HASH_CODE_MAX;
	y %= HASH_CODE_MAX;
	return g_HashCode[x + g_HashCode[y]];
}

//������0.0f~1.0f�ɐ��K���������̂��擾����
float GetValue(int x, int y)
{
	return (float)GetHash(x, y) / (float)(HASH_CODE_MAX - 1);
}

//�܎���Ԋ֐�
float Fade(float t)
{
	//Ken Perlin��(�p�[�����m�C�Y��������l)���l����������Ԋ֐�
	//6x^5 - 15x^4 + 10x^3.
	return (6 * powf(t, 5) - 15 * powf(t, 4) + 10 * powf(t, 3));
}

//���`���
float Lerp(float a, float b, float t)
{
	return (a + (b - a) * t);
}

//�o�����[�m�C�Y�擾
float ValueNoise(float x, float y)
{
	//�������Ə������ɕ�����
	int xi = (int)floorf(x);
	int yi = (int)floorf(y);
	float xf = x - xi;
	float yf = y - yi;

	//�i�q�_�����o��
	float a00 = GetValue(xi, yi);
	float a10 = GetValue(xi + 1, yi);
	float a01 = GetValue(xi, yi + 1);
	float a11 = GetValue(xi + 1, yi + 1);

	//�����������g���Ă��̂܂ܐ��`��Ԃ��Ă��܂��Ɛ܂���O���t�ɂȂ��Ă��܂��̂�
	//���`��Ԃ���O�ɏ����������܎���Ԋ֐��Řc�߂��Ⴄ
	xf = Fade(xf);
	yf = Fade(yf);

	//�ʒu����ɁC�e�i�q�_����̉e�����l�������l���Z�o����
	return Lerp(Lerp(a00, a10, xf), Lerp(a01, a11, xf), yf);
}


float OctaveValueNoise(float x, float y)
{
	float a = 1.0f;
	float f = 1.0f;
	float maxValue = 0.0f;
	float totalValue = 0.0f;
	float per = 0.5f;
	for (int i = 0; i < 5; ++i) {
		totalValue += a * ValueNoise(x * f, y * f);
		maxValue += a;
		a *= per;
		f *= 2.0f;
	}
	return totalValue / maxValue;
}

float Grad(unsigned int hash, float a, float b)
{
	unsigned int key = hash % 0x4;
	switch (key)
	{
	case 0x0:	return a;	//a * 1.0f + b * 0.0f
	case 0x1:	return -a;	//a * -1.0f + b * 0.0f
	case 0x2:	return -b;	//a * 0.0f + b * -1.0f
	case 0x3:	return b;	//a * 0.0f + b * 1.0f
	};
	return 0.0f;
}

float PerlinNoise(float x, float y)
{
	//�������Ə������ɕ�����
	int xi = (int)floorf(x);
	int yi = (int)floorf(y);
	float xf = x - xi;
	float yf = y - yi;

	//�i�q�_����n�b�V�������o���C���̒l����Ɍ��z���擾����
	float a00 = Grad(GetHash(xi, yi), xf, yf);
	float a10 = Grad(GetHash(xi + 1, yi), xf - 1.0f, yf);
	float a01 = Grad(GetHash(xi, yi + 1), xf, yf - 1.0f);
	float a11 = Grad(GetHash(xi + 1, yi + 1), xf - 1.0f, yf - 1.0f);

	//��Ԃ�������
	xf = Fade(xf);
	yf = Fade(yf);

	//�ʒu�ɍ��킹�Ċi�q�_�̂ǂ̓_�����ԉe�����󂯂邩�����߂�
	//(���z�֐����œ��ς�����Ă���̂ŁC�x�N�g���̌����ɂ���Ă͕��̒l���o��D�͈͂�-1.0f~1.0f)
	//(�Ȃ̂ŁC���̒l�ɂ��邽�߂�1.0f�𑫂���2.0f�Ŋ����Ă���)
	return (Lerp(Lerp(a00, a10, xf), Lerp(a01, a11, xf), yf) + 1.0f) / 2.0f;
}

float OctavePerlinNoise(float x, float y)
{
	float a = 1.0f;
	float f = 1.0f;
	float maxValue = 0.0f;
	float totalValue = 0.0f;
	float per = 0.5f;
	for (int i = 0; i < 5; ++i) {
		totalValue += a * PerlinNoise(x * f, y * f);
		maxValue += a;
		a *= per;
		f *= 2.0f;
	}
	return totalValue / maxValue;
}

