//==================================================================================================================
//
// ランキング処理[ranking.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================
#include "inputKeyboard.h"
#include "renderer.h"
#include "ranking.h"
#include "number.h"
#include "fade.h"
#include "sound.h"
#include "inputGamepad.h"

//==================================================================================================================
//マクロ的偽
//==================================================================================================================
#define RANKING_COUNT_MAX 600			// ランキングカウントの最大数

//==================================================================================================================
// 静的メンバー変数の初期化
//==================================================================================================================
int CRanking::m_nTime = NULL;					// タイム変数

//==================================================================================================================
//
// コンストラクタ
//
//==================================================================================================================
CRanking::CRanking()
{

}

//==================================================================================================================
//
// デストラクタ
//
//==================================================================================================================
CRanking::~CRanking()
{

}

//==================================================================================================================
// 初期化
//==================================================================================================================
void CRanking::Init(void)
{
	// 変数の初期化
	m_nCntTitle = 0;										// タイトルへのカウンタ

	// 初期化
	m_nTime = 0;											// タイム
}

//==================================================================================================================
// 終了
//==================================================================================================================
void CRanking::Uninit(void)
{
	// Scene2D全削除
	CScene2D::ReleaseAll();
}

//==================================================================================================================
// 更新
//==================================================================================================================
void CRanking::Update(void)
{
	// キーボード取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// ゲームパッド取得
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	// フェード取得
	CFade::FADE Fade = CFade::GetFade();

	// 音取得
	CSound *pSound = CRenderer::GetSound();

	// タイム加算
	m_nTime++;

	// キーボードの[Enter]が押されたとき 又は タイムカウントがMAX超えた 又は コントローラーの[START]が押されたとき
	if (pInputKeyboard->GetKeyboardTrigger(DIK_RETURN) || m_nTime > TIME_COUNT || pInputGamepad->GetPress(0, CInputGamepad::JOYPADKEY_START))
	{
		// フェード状態がない時
		if (Fade == CFade::FADE_NONE)
		{
			CFade::SetFade(CRenderer::MODE_TITLE);					// フェードの設定
		}
	}

	// タイトルへのカウンタ加算
	m_nCntTitle++;

	// ランキングカウンタが規定値を超えたら
	if (m_nCntTitle >= RANKING_COUNT_MAX)
	{
		// フェードが何もない時
		if (Fade == CFade::FADE_NONE)
		{
			// フェードの設定
			CFade::SetFade(CRenderer::MODE_TITLE);
		}
	}
}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CRanking::Draw(void)
{

}

//==================================================================================================================
// 生成
//==================================================================================================================
CRanking *CRanking::Create(void)
{
	CRanking *pRanking;					// ランキング変数
	pRanking = new CRanking;			// 動的確保
	pRanking->Init();					// 初期化処理
	return pRanking;					// 値を返す
}