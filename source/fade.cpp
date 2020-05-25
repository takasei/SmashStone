//==================================================================================================================
//
// fadeの描画 [fade.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================
#define _CRT_SECURE_NO_WARNINGS							// 警告回避文

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "fade.h"
#include "inputKeyboard.h"
#include "debugProc.h"
#include "sound.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define MAX_TEST_COUNT 40		// テスト用カウンタの最大値

//==================================================================================================================
// 静的メンバ変数宣言
//==================================================================================================================
LPDIRECT3DTEXTURE9 CFade::m_pTexture = NULL;					// テクスチャ情報
CRenderer::MODE CFade::m_modeNext = CRenderer::MODE_TITLE;		// 次のモード
CFade::FADE CFade::m_fade = FADE_NONE;							// フェードの状態
D3DXCOLOR CFade::m_colorFade = D3DXCOLOR(0.0, 0.0, 0.0, 1.0);	// 色情報
float CFade::m_nCntFade = 0.05f;								// フェードカウント

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CFade::CFade()
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CFade::~CFade()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
void CFade::Init(CRenderer::MODE modeNext)
{
	// 初期化
	m_pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0);			// 位置
	m_fade = FADE_IN;														// フェードの状態
	m_modeNext = modeNext;													// 次のモード
	m_nCntTest = 0;															// テスト用カウンタ
	m_nMode = CRenderer::MODE_TITLE;										// モード用カウンタ
	m_bTest = false;														// テストをするかどうか

	CRenderer *pRenderer = CManager::GetRenderer();							// レンダラーの情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();						// デバイスを取得する

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVertexBuff,
		NULL);

	VERTEX_2D *pVtx;														// 頂点情報へのポインタ

	// 頂点データの範囲をロックし、頂点バッファへのポインタ取得
	m_pVertexBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定(右回りで設定する)
	pVtx[0].pos.x = m_pos.x + (-SCREEN_WIDTH / 2);
	pVtx[0].pos.y = m_pos.y + (-SCREEN_HEIGHT / 2);
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = m_pos.x + (+SCREEN_WIDTH / 2);
	pVtx[1].pos.y = m_pos.y + (-SCREEN_HEIGHT / 2);
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = m_pos.x + (-SCREEN_WIDTH / 2);
	pVtx[2].pos.y = m_pos.y + (+SCREEN_HEIGHT / 2);
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = m_pos.x + (+SCREEN_WIDTH / 2);
	pVtx[3].pos.y = m_pos.y + (+SCREEN_HEIGHT / 2);
	pVtx[3].pos.z = 0.0f;

	// 同次座標(1.0で固定)
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 色の設定
	pVtx[0].col = m_colorFade;
	pVtx[1].col = m_colorFade;
	pVtx[2].col = m_colorFade;
	pVtx[3].col = m_colorFade;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点データをアンロック
	m_pVertexBuff->Unlock();
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CFade::Uninit(void)
{
	// テクスチャ情報廃棄
	Unload();

	// 頂点バッファがあるとき
	if (m_pVertexBuff != NULL)
	{
		m_pVertexBuff->Release();					// 頂点バッファの開放
		m_pVertexBuff = NULL;						// NULLにする
	}
}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CFade::Update(void)
{
	VERTEX_2D *pVtx;									// 頂点情報へのポインタ

	// テストをするとき
	if (m_bTest)
	{
		// テスト用カウンタ加算
		m_nCntTest++;
	}

	// カウンタが規定値を超えたとき
	if (m_nCntTest >= MAX_TEST_COUNT)
	{
		// 現在のモードによって切り替える
		switch (m_modeNext)
		{
			// タイトルのとき
		case CRenderer::MODE_TITLE:
			m_modeNext = CRenderer::MODE_TUTORIAL;
			break;

			// チュートリアルのとき
		case CRenderer::MODE_TUTORIAL:
			m_modeNext = CRenderer::MODE_GAME;
			break;

			// ゲームのとき
		case CRenderer::MODE_GAME:
			m_modeNext = CRenderer::MODE_RANKING;
			break;

			// ランキングのとき
		case CRenderer::MODE_RANKING:
			m_modeNext = CRenderer::MODE_TITLE;
			break;
		}

		// カウンタの初期化
		m_nCntTest = 0;

		// フェードの設定
		SetFade(m_modeNext);
	}

	// フェードがあるとき
	if (m_fade != FADE_NONE)
	{
		// フェード状態がインのとき
		if (m_fade == FADE_IN)
		{
			// フェードイン処理
			m_colorFade.a -= m_nCntFade;				// 透明度減らしていく

			// 透明度が0以下だったとき
			if (m_colorFade.a <= 0.0f)
			{
				// フェード処理終了
				m_colorFade.a = 0.0f;					// 透明度を0にする
				m_fade = FADE_NONE;						// フェード状態を何もない状態にする
			}
		}
		else if (m_fade == FADE_OUT)
		{// フェード状態がアウトのとき
			//フェードアウト処理
			m_colorFade.a += m_nCntFade;				// 画面を不透明にしていく

			// 透明度が1より大きくなったとき
			if (m_colorFade.a >= 1.0f)
			{
				//フェードイン処理に切り替え
				m_colorFade.a = 1.0f;					// 透明度を1にする

				m_fade = FADE_IN;					// フェード状態をインの状態にする

				// モードを設定
				CRenderer::SetMode(m_modeNext);
			}
		}

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		m_pVertexBuff->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点カラーの設定
		pVtx[0].col = m_colorFade;
		pVtx[1].col = m_colorFade;
		pVtx[2].col = m_colorFade;
		pVtx[3].col = m_colorFade;

		// 頂点データをアンロックする
		m_pVertexBuff->Unlock();
	}
}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CFade::Draw(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();							// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();						// デバイスを取得する

	// 頂点バッファをデバイスのデータストリームにバインド
	pDevice->SetStreamSource(0, m_pVertexBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, m_pTexture);

	// ポリゴン描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);
}

//==================================================================================================================
// フェードの設定
//==================================================================================================================
void CFade::SetFade(CRenderer::MODE modeNext)
{
	// 音取得
	CSound *pSound = CRenderer::GetSound();

	m_fade = FADE_OUT;									// フェード状態をアウトにする
	m_modeNext = modeNext;								// モードを次のモードにする
	//pSound->PlaySound(CSound::SOUND_LABEL_SE_DECISION);	// 決定ボタン
}

//==================================================================================================================
// フェードの取得
//==================================================================================================================
CFade::FADE CFade::GetFade(void)
{
	return m_fade;
}

//==================================================================================================================
// オブジェクトの生成
//==================================================================================================================
CFade * CFade::Create(CRenderer::MODE modeNext)
{
	CFade *pFade = NULL;					// フェードをNULLにする

	pFade = new CFade;						// 動的に確保
	pFade->Init(modeNext);					// 初期化処理

	return pFade;							// 値を返す
}

//==================================================================================================================
// テクスチャ情報ロード
//==================================================================================================================
HRESULT CFade::Load(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();			// レンダラーの情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスを取得する

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,						// デバイスへのポインタ
		"data/TEXTURE/",									// ファイルの名前
		&m_pTexture);										// 読み込むメモリー

	return S_OK;											// 値を返す
}

//==================================================================================================================
// テクスチャ情報破棄
//==================================================================================================================
void CFade::Unload(void)
{
	// テクスチャがあるとき
	if (m_pTexture != NULL)
	{
		m_pTexture->Release();					// テクスチャの開放
		m_pTexture = NULL;						// NULLにする
	}
}

//==================================================================================================================
// フェードイン処理
//==================================================================================================================
void CFade::FadeIN(void)
{
	m_fade = FADE_IN;
}

//==================================================================================================================
// フェードアウト処理
//==================================================================================================================
void CFade::FadeOUT(void)
{
	m_fade = FADE_OUT;
	m_colorFade = D3DXCOLOR(0.0, 0.0, 0.0, 0.0);	// 色初期化
}
