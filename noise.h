#pragma once
#include<time.h>
#include<stdio.h>
#include<stdlib.h>

#define HASH_CODE_MAX		(256)
#define HASH_CODE_TABLE_NUM 	(HASH_CODE_MAX*2)

//*****************************************************************************
// ノイズ用
//*****************************************************************************
//指定範囲でクリップする.
#define CLIP(e,l,h) (min(max(e,l),h))

//配列の要素数取得.
#define COUNTOF(a) ( sizeof( a ) / sizeof( a[0] ) )

void SettingHash(unsigned int seed);
unsigned int GetHash(int x, int y);
float GetValue(int x, int y);
float Fade(float t);
float Lerp(float a, float b, float t);
float ValueNoise(float x, float y);
float OctaveValueNoise(float x, float y);
float Grad(unsigned int hash, float a, float b);
float PerlinNoise(float x, float y);
float OctavePerlinNoise(float x, float y);
