//==================================================================================================================
//
// フェード処理[fade.h]
// Author  :Seiya Takahashi
//
//==================================================================================================================
#ifndef _FADE_H
#define _FADE_H

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"
#include "renderer.h"

//==================================================================================================================
//
// フェードクラスの定義
//
//==================================================================================================================
class CFade
{
public:
	// ==========================================================
	// フェードの状態
	// ==========================================================
	typedef enum
	{
		FADE_NONE = 0,								// 何もない状態
		FADE_IN,									// フェードイン処理
		FADE_OUT,									// フェードアウト処理
		FADE_MAX									// 最大数
	} FADE;

	CFade();										// コンストラクタ
	~CFade();										// デストラクタ
	void Init(CRenderer::MODE modeNext);			// 初期化処理
	void Uninit(void);								// 終了処理
	void Update(void);								// 更新処理
	void Draw(void);								// 描画処理

	static void SetFade(CRenderer::MODE modeNext);	// フェードの設定処理
	static FADE GetFade(void);						// フェードの取得処理
	static CFade *Create(CRenderer::MODE modeNext);	// 生成処理
	static HRESULT Load(void);						// フェードの情報ロード
	static void Unload(void);						// フェードの情報アンロード
	static void FadeIN(void);						// フェードインさせる処理
	static void FadeOUT(void);						// フェードアウトさせる処理

protected:

private:
	static LPDIRECT3DTEXTURE9 m_pTexture;			// テクスチャ情報
	static CRenderer::MODE m_modeNext;				// 次のモード変数
	static FADE m_fade;								// フェードの情報
	static D3DXCOLOR m_colorFade;					// フェードの色情報
	static float m_nCntFade;								// フェードカウント

	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuff;			// 頂点バッファ

	D3DXVECTOR3 m_pos;								// 位置

	int m_nCntTest;									// テスト用カウンタ
	int m_nMode;									// モード用カウンタ

	bool m_bTest;									// テストをするかどうか
};
#endif
