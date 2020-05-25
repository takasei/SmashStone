//==================================================================================================================
//
// 描画3D[scene3D.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _SCENE3D_H_
#define _SCENE3D_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"
#include "scene.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define POLGON_X (100)																// テクスチャのX座標
#define POLGON_Y (100)																// テクスチャのY座標
#define CENTERPOS_NORMAL (2)														

//==================================================================================================================
//
// 描画3Dクラスの定義
//
//==================================================================================================================
class CScene3D : public CScene
{
public:
	CScene3D(PRIORITY type);							// コンストラクタ
	~CScene3D();										// デストラクタ
	void Init(void);									// 初期化処理
	void Uninit(void);									// 終了処理
	void Update(void);									// 更新処理
	void Draw(void);									// 描画処理

	static CScene3D *Create(void);						// 生成処理
	static CScene3D *GetScene3D(void);					// Scene3D情報取得処理

	void BindTex(LPDIRECT3DTEXTURE9 tex);				// テクスチャ結合処理
	void SetCol(D3DXCOLOR col);							// 色設定処理
	void SetPos(D3DXVECTOR3 pos);						// 位置設定処理
	void SetSize(D3DXVECTOR3 size);						// 大きさ設定処理

	D3DXVECTOR3 GetPos(void) { return m_pos; }			// 位置取得処理
	D3DXVECTOR3 GetSize(void) { return m_size; }		// 大きさ取得処理

protected:

private:
	static CScene3D *m_pScene3D;

	LPDIRECT3DTEXTURE9 m_pTexturePolygon;				// テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;					// バッファ
	VERTEX_3D *m_pVtx;									// 頂点格納
	D3DXMATRIX m_mtxWorld;								// ワールドマトリックス

	D3DXVECTOR3 m_pos;									// 位置
	D3DXVECTOR3 m_rot;									// 回転
	D3DXVECTOR3 m_size;									// 大きさ
	D3DXVECTOR3 m_NorA;									// 一つ目の三角形の法線
	D3DXVECTOR3 m_NorB;									// 二つ目の三角形の法線

	float m_fAngle;										// 角度
	float m_fLength;									// 長さ
	float m_Height;										// 高さ
};
#endif