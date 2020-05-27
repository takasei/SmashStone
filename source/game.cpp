//==================================================================================================================
//
// Gameの描画 [game.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================

//==================================================================================================================
//	インクルードファイル
//==================================================================================================================
#include "game.h"
#include "player.h"
#include "number.h"
#include "fade.h"
#include "inputKeyboard.h"
#include "meshField.h"
#include "scene3D.h"
#include "camera.h"
#include "light.h"
#include "time.h"
#include "renderer.h"
#include "motionModel.h"
#include "pause.h"
#include "inputGamepad.h"
#include "meshSphere.h"
#include "character.h"

//==================================================================================================================
//	マクロ定義
//==================================================================================================================
#define PLAYER_START_POS_X 390.0f									// プレイヤーの初期位置X
#define PLAYER_START_POS_Z -585.0f									// プレイヤーの初期位置Z
#define RESPAWN_SIZE 0.000001f										// リスポーンモデルの大きさ

//==================================================================================================================
//	静的メンバ変数宣言
//==================================================================================================================
CPlayer *CGame::m_pPlayer = NULL;									// プレイヤー情報
CMeshField *CGame::m_pMeshField = NULL;								// メッシュフィールド情報
CCamera *CGame::m_pCamera = NULL;									// カメラ情報
CLight *CGame::m_pLight = NULL;										// ライト情報
CLogo *CGame::m_pLogo = NULL;										// ロゴ情報
CPause *CGame::m_pPause = NULL;										// ポーズ情報
CMeshSphere *CGame::m_pMeshSphere = NULL;							// メッシュ球の情報
CGame::GAMESTATE CGame::m_gameState = CGame::GAMESTATE_NONE;		// ゲーム状態
int CGame::m_nCounterGameState = NULL;								// ゲームの状態管理カウンター

//==================================================================================================================
//	コンストラクタ
//==================================================================================================================
CGame::CGame()
{

}

//==================================================================================================================
//	デストラクタ
//==================================================================================================================
CGame::~CGame()
{

}

//==================================================================================================================
//	初期化処理
//==================================================================================================================
void CGame::Init(void)
{
	CNumber::Load();						// 数字テクスチャロード
	CMeshField::Load();						// 床テクスチャロード
	CMotionModel::Load();					// モーション用モデルロード
	CPause::Load();							// ポーズテクスチャロード
	CMeshSphere::Load();					// メッシュ球のテクスチャロード

	// カメラの生成処理
	m_pCamera = CCamera::Create();

	// ライトの生成処理
	m_pLight = CLight::Create();

	// メッシュ球の生成処理
	m_pMeshSphere = CMeshSphere::Create();

	// プレイヤー生成処理
	m_pPlayer = CPlayer::Create();
	// 位置設定
	m_pPlayer->SetPos(D3DXVECTOR3(0, WhileY, 0));
	// 回転設定
	m_pPlayer->SetRot(D3DXVECTOR3(0.0f, -D3DX_PI / 2, 0.0f));

	// メッシュフィールド生成
	m_pMeshField = CMeshField::Create();

	// ポーズの生成処理
	m_pPause = CPause::Create();

	SetGameState(GAMESTATE_NORMAL);			// 通常状態に設定
	m_nCounterGameState = 0;				// ゲームの状態管理カウンターを0にする
}

//==================================================================================================================
//	終了処理
//==================================================================================================================
void CGame::Uninit(void)
{
	// ゲーム状態を通常にする
	m_gameState = GAMESTATE_NORMAL;

	CScene2D::ReleaseAll();				// 2Dのもの全て破棄
	CScene3D::ReleaseAll();				// 3Dのもの全て破棄

	CNumber::Unload();					// 数字テクスチャアンロード
	CMeshField::Unload();				// 床テクスチャアンロード
	CMotionModel::Unload();				// モーション用モデルアンロード

	// ポーズの終了処理
	m_pPause->Uninit();

	delete m_pPause;					// メモリ削除
	m_pPause = nullptr;					// ポインタNULL

	delete m_pCamera;					// メモリ削除
	m_pCamera = nullptr;				// ポインタNULL

	delete m_pLight;					// メモリ削除
	m_pLight = nullptr;					// ポインタNULL
}

//==================================================================================================================
//	更新処理
//==================================================================================================================
void CGame::Update(void)
{
	// キーボード取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// コントローラー取得
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	// フェード取得
	CFade::FADE fade = CFade::GetFade();

	// ゲームの状態取得
	m_gameState = GetGameState();

	// ゲーム状態がポーズのとき
	if (m_gameState == GAMESTATE_PAUSE)
	{
		// ポーズの更新処理
		m_pPause->Update();

		// ゲーム状態が初めからやり直すとき
		if (m_gameState == GAMESTATE_START_OVER)
		{
			// フェードを設定する
			CFade::SetFade(CRenderer::MODE_GAME);
		}
		else if (m_gameState == GAMESTATE_BREAK)
		{// ゲーム状態が中断のとき
			// フェードを設定する
			CFade::SetFade(CRenderer::MODE_TITLE);
		}
	}
	else
	{
		// カメラの更新処理
		m_pCamera->Update();

		// ライトの更新処理
		m_pLight->Update();
	}

	// キーボードの[P] 又は コントローラーの[START]ボタンが押されたとき
	if (pInputKeyboard->GetKeyboardTrigger(DIK_P) || pInputGamepad->GetTrigger(0, CInputGamepad::JOYPADKEY_START))
	{// ポーズ切り替え
		// ゲーム状態がポーズのとき
		if (m_gameState == GAMESTATE_PAUSE)
		{
			// ゲーム状態をNORMALにする
			m_gameState = GAMESTATE_NORMAL;

			// ポーズ状態の設定
			m_pPause->SetPause(false);
		}
		else
		{// ゲーム状態がポーズじゃないとき
			// ゲーム状態をポーズにする
			m_gameState = GAMESTATE_PAUSE;

			// ポーズ状態の設定
			m_pPause->SetPause(true);
		}
	}
#ifdef _DEBUG
	// キーボードの[0]を押したとき
	if (pInputKeyboard->GetKeyboardTrigger(DIK_RETURN))
	{
		// フェードが何もない時
		if (fade == CFade::FADE_NONE)
		{
			// フェードを設定する
			CFade::SetFade(CRenderer::MODE_RANKING);
		}
	}
#endif // _DEBUG
}

//==================================================================================================================
//	描画処理
//==================================================================================================================
void CGame::Draw(void)
{
	// カメラの設定
	m_pCamera->SetCamera();

	// カメラの描画処理
	m_pCamera->Draw();

	// ポーズ状態がtrueのとき
	if (m_pPause->GetPause() == true)
	{
		// ポーズの更新処理
		m_pPause->Draw();
	}
}

//==================================================================================================================
//	生成処理
//==================================================================================================================
CGame * CGame::Create(void)
{
	CGame *pGame = NULL;		// ゲーム変数NULL

	pGame = new CGame;			// 動的に確保
	pGame->Init();				// 初期化処理

	return pGame;				// 値を返す
}

//==================================================================================================================
//	ゲームの状態設定
//==================================================================================================================
void CGame::SetGameState(GAMESTATE state)
{
	m_gameState = state;
}

//==================================================================================================================
//	ゲームの状態取得
//==================================================================================================================
CGame::GAMESTATE CGame::GetGameState(void)
{
	return m_gameState;
}

//==================================================================================================================
//	プレイヤー情報取得
//==================================================================================================================
CPlayer * CGame::GetPlayer(void)
{
	return m_pPlayer;
}
