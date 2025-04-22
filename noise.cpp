#include "main.h"
#include "input.h"
#include "noise.h"
#include "renderer.h"
#include "collision.h"

int g_HashCode[HASH_CODE_TABLE_NUM] = {};

//*****************************************************************************
// ノイズ用関数
//*****************************************************************************
//乱数テーブル作成
void SettingHash(unsigned int seed)
{
	//乱数ライブラリ初期化
	srand(seed);

	//ハッシュコード初期化
	memset(g_HashCode, 0, sizeof(unsigned int) * COUNTOF(g_HashCode));

	//ランダムテーブル生成
	const int TABLE_NUM = HASH_CODE_MAX;
	unsigned int randomTable[TABLE_NUM] = {};
	for (int i = 0; i < COUNTOF(randomTable); ++i) {
		randomTable[i] = rand() % HASH_CODE_MAX;
	}

	//ハッシュコード生成.
	for (int i = 0; i < COUNTOF(g_HashCode); ++i) {
		g_HashCode[i] = randomTable[i % TABLE_NUM];
	}
}

//乱数値取得
unsigned int GetHash(int x, int y)
{
	x %= HASH_CODE_MAX;
	y %= HASH_CODE_MAX;
	return g_HashCode[x + g_HashCode[y]];
}

//乱数を0.0f~1.0fに正規化したものを取得する
float GetValue(int x, int y)
{
	return (float)GetHash(x, y) / (float)(HASH_CODE_MAX - 1);
}

//五次補間関数
float Fade(float t)
{
	//Ken Perlin氏(パーリンノイズを作った人)が考えだした補間関数
	//6x^5 - 15x^4 + 10x^3.
	return (6 * powf(t, 5) - 15 * powf(t, 4) + 10 * powf(t, 3));
}

//線形補間
float Lerp(float a, float b, float t)
{
	return (a + (b - a) * t);
}

//バリューノイズ取得
float ValueNoise(float x, float y)
{
	//整数部と小数部に分ける
	int xi = (int)floorf(x);
	int yi = (int)floorf(y);
	float xf = x - xi;
	float yf = y - yi;

	//格子点を取り出す
	float a00 = GetValue(xi, yi);
	float a10 = GetValue(xi + 1, yi);
	float a01 = GetValue(xi, yi + 1);
	float a11 = GetValue(xi + 1, yi + 1);

	//小数部分を使ってそのまま線形補間してしまうと折れ線グラフになってしまうので
	//線形補間する前に小数部分を五次補間関数で歪めちゃう
	xf = Fade(xf);
	yf = Fade(yf);

	//位置を基準に，各格子点からの影響を考慮した値を算出する
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
	//整数部と小数部に分ける
	int xi = (int)floorf(x);
	int yi = (int)floorf(y);
	float xf = x - xi;
	float yf = y - yi;

	//格子点からハッシュを取り出し，その値を基に勾配を取得する
	float a00 = Grad(GetHash(xi, yi), xf, yf);
	float a10 = Grad(GetHash(xi + 1, yi), xf - 1.0f, yf);
	float a01 = Grad(GetHash(xi, yi + 1), xf, yf - 1.0f);
	float a11 = Grad(GetHash(xi + 1, yi + 1), xf - 1.0f, yf - 1.0f);

	//補間をかける
	xf = Fade(xf);
	yf = Fade(yf);

	//位置に合わせて格子点のどの点から一番影響を受けるかを決める
	//(勾配関数内で内積を取っているので，ベクトルの向きによっては負の値が出る．範囲は-1.0f~1.0f)
	//(なので，正の値にするために1.0fを足して2.0fで割っている)
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

