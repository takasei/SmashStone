//==================================================================================================================
//
// 描画2D[scene2D.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _SCENE2D_H_
#define _SCENE2D_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"
#include "scene.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define POLGON_X (100)																	// テクスチャのX座標
#define POLGON_Y (100)																	// テクスチャのY座標
#define CENTERPOS_NORMAL (2)															// 中心決める

//==================================================================================================================
//
// 描画2Dクラスの定義
//
//==================================================================================================================
class CScene2D : public CScene
{
public:
	CScene2D(PRIORITY type);															// コンストラクタ
	~CScene2D();																		// デストラクタ
	void Init(void);																	// 初期化処理
	void Uninit(void);																	// 終了処理
	void Update(void);																	// 更新処理
	void Draw(void);																	// 描画処理

	static CScene2D *Create(void);														// 生成処理

	void BindTex(LPDIRECT3DTEXTURE9 tex);												// テクスチャ結合処理
	void SetAnimation(float fTexX, float fTexY,float fTexY2, int nPatternAnim);			// アニメーション設定処理
	void SetMove(float fTexX, float fTexY, float fMoveX);								// 背景スクロール設定処理
	void SetCol(D3DXCOLOR col);															// 色設定処理
	void SetPos(D3DXVECTOR3 pos);														// 位置設定処理
	void SetSize(D3DXVECTOR3 size);														// 大きさ設定処理
	void SetRot(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fAngle, float fLength);			// 回転設定処理

	D3DXVECTOR3 GetPos(void) { return m_pos; }											// 位置取得処理
	D3DXVECTOR3 GetSize(void) { return m_size; }										// 大きさ取得処理

	bool SetCollision(D3DXVECTOR3 posDo, int sizeX, int sizeY, D3DXVECTOR3 posDone);	// 当たり判定処理

protected:

private:
	static CScene2D *m_pScene2D;														// scene2D情報

	LPDIRECT3DTEXTURE9 m_pTexturePolygon;												// テクスチャへのポインタ
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;													// バッファ
	VERTEX_2D *m_pVtx;																	// 頂点格納

	D3DXVECTOR3 m_pos;																	// 位置
	D3DXVECTOR3 m_size;																	// 大きさ
	D3DXVECTOR3 m_rot;																	// 回転

	float m_fAngle;																		// 角度
	float m_fLength;																	// 長さ
};
#endif