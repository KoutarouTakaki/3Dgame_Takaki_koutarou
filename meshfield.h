//=============================================================================
//
// ���b�V���n�ʂ̏��� [meshfield.h]
// Author : 
//
//=============================================================================
#pragma once

//#define HASH_CODE_MAX		(256)
//#define HASH_CODE_TABLE_NUM 	(HASH_CODE_MAX*2)

////*****************************************************************************
//// �m�C�Y�p
////*****************************************************************************
////�w��͈͂ŃN���b�v����.
//#define CLIP(e,l,h) (min(max(e,l),h))
//
////�z��̗v�f���擾.
//#define COUNTOF(a) ( sizeof( a ) / sizeof( a[0] ) )

////�x�N�g���\����.
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
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
							int nNumBlockX, int nNumBlockZ, float nBlockSizeX, float nBlockSizeZ);
void UninitMeshField(void);
void UpdateMeshField(void);
void DrawMeshField(void);

BOOL RayHitField(XMFLOAT3 pos, XMFLOAT3 *HitPosition, XMFLOAT3 *Normal);

