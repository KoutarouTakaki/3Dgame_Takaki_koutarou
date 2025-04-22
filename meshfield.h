//=============================================================================
//
// メッシュ地面の処理 [meshfield.h]
// Author : 
//
//=============================================================================
#pragma once

//#define HASH_CODE_MAX		(256)
//#define HASH_CODE_TABLE_NUM 	(HASH_CODE_MAX*2)

////*****************************************************************************
//// ノイズ用
////*****************************************************************************
////指定範囲でクリップする.
//#define CLIP(e,l,h) (min(max(e,l),h))
//
////配列の要素数取得.
//#define COUNTOF(a) ( sizeof( a ) / sizeof( a[0] ) )

////ベクトル構造体.
//#define VECTOR SVector2D<float>
//template <class T>
//struct SVector2D
//{
//	typedef T DataType;
//	T x;
//	T y;
//	SVector2D() { Init(); }
//	void Init()
//	{
//		x = T();
//		y = T();
//	}
//	SVector2D	operator +  (const SVector2D& e) const { SVector2D tmp; tmp.x = x + e.x; tmp.y = y + e.y; return tmp; }
//	SVector2D& operator += (const SVector2D& e) { x += e.x; y += e.y; return (*this); }
//	SVector2D	operator -  (const SVector2D& e) const { SVector2D tmp; tmp.x = x - e.x; tmp.y = y - e.y; return tmp; }
//	SVector2D& operator -= (const SVector2D& e) { x -= e.x; y -= e.y; return (*this); }
//	T			operator *  (const SVector2D& e) const { return (x * e.x) + (y * e.y); }
//	SVector2D& operator *= (const int e) { x *= e; y *= e; return (*this); }
//	SVector2D& operator *= (const float e) { x *= e; y *= e; return (*this); }
//	SVector2D& operator /= (const int e) { x /= e; y /= e; return (*this); }
//	SVector2D& operator /= (const float e) { x /= e; y /= e; return (*this); }
//};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitMeshField(void);
void UpdateMeshField(void);
void DrawMeshField(void);

BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);

