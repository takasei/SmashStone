//==================================================================================================================
//
// ポーズ処理[pause.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _PAUSE_H
#define _PAUSE_H

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"

//==================================================================================================================
//
// ポーズクラスの定義
//
//==================================================================================================================
class CPause
{
public:
	// ==========================================================
	// 構造体定義
	// ==========================================================
	// テクスチャの種類
	typedef enum
	{
		PAUSETEXTURE_BG = 0,		// ポーズ背景
		PAUSETEXTURE_FRAME,			// ポーズフレーム
		PAUSETEXTURE_SELECT,		// 選択
		PAUSETEXTURE_CONTINUE,		// 続ける
		PAUSETEXTURE_START,			// スタートからやり直す
		PAUSETEXTURE_TITLE,			// タイトルにいく
		PAUSETEXTURE_MAX			// 最大数
	} PAUSETEXTURE;					// ポーズテクスチャ

	CPause();						// コンストラクタ
	~CPause();						// デストラクタ
	void Init(void);				// 初期化処理
	void Uninit(void);				// 終了処理
	void Update(void);				// 更新処理
	void Draw(void);				// 描画処理

	static CPause *Create(void);	// 生成処理
	static HRESULT Load(void);		// フェードの情報ロード
	static void Unload(void);		// フェードの情報アンロード

	void SetPause(bool bPause);		// ポーズ状態の設定処理

	bool GetPause(void);			// ポーズの状態取得

protected:

private:
	void SetVertexPause(PAUSETEXTURE number, D3DXVECTOR3 pos,
		D3DXCOLOR col, float fWidth, float fHeight);		// ポーズのテクスチャ設定処理
	static LPDIRECT3DTEXTURE9 m_pTexture[PAUSETEXTURE_MAX];	// テクスチャ情報
	static char *m_apFileName[PAUSETEXTURE_MAX];			// テクスチャのファイル名

	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuff;					// 頂点バッファ
	D3DXVECTOR3 m_pos;										// 位置
	D3DXCOLOR m_color;										// 色情報

	int m_nSelect;											// 選択番号

	bool m_bPause;											// ポーズ中かどうか
};
#endif
