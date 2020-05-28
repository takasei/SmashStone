//==================================================================================================================
//
// タイトル処理[title.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _TITLE_H
#define _TITLE_H

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"

//==================================================================================================================
// 前方宣言
//==================================================================================================================
class CCamera;
class CLight;
class CPolygon;
class CMeshField;
class CLogo;
class CCharacter;
class CMeshSphere;
class CModel;
class CTitleFade;

//==================================================================================================================
//
// タイトルクラスの定義
//
//==================================================================================================================
class CTitle
{
public:
	
	CTitle();									// コンストラクタ
	~CTitle();									// デストラクタ
	void Init(void);							// 初期化処理
	void Uninit(void);							// 終了処理
	void Update(void);							// 更新処理
	void Draw(void);							// 描画処理

	static CTitle *Create(void);				// 生成処理
	static CCharacter *GetCharacter(void);		// キャラクターの情報取得処理
	static CCamera *GetCamera(void);			// カメラの情報取得処理

protected:

private:

	static LPDIRECT3DTEXTURE9 m_pTexture;		// テクスチャ情報
	static CCamera *m_pCamera;					// カメラの情報
	static CLight *m_pLight;					// ライトの情報
	static CPolygon *m_pPolygon;				// ポリゴン情報
	static CMeshField *m_pMeshField;			// メッシュフィールドの情報
	static CLogo *m_pLogo;						// ロゴの情報
	static CCharacter *m_pCharacter;			// キャラクターの情報ポインタ
	static CMeshSphere *m_pMeshSphere;			// メッシュ球の情報ポインタ
	static CTitleFade *m_pTitleFade;			// タイトルフェードの情報ポインタ

	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuff;		// 頂点バッファ

	D3DXVECTOR3 m_pos;							// 位置

	int m_nCntRanking;							// ランキングへのカウンタ
};

#endif
