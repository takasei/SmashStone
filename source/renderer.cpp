//==================================================================================================================
//
// レンダラー[renderer.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================
#include "renderer.h"
#include "scene.h"
#include "debugProc.h"
#include "camera.h"
#include "light.h"
#include "fade.h"
#include "game.h"
#include "title.h"
#include "tutorial.h"
#include "ranking.h"
#include "sound.h"

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
CFade *CRenderer::m_pFade = NULL;						// Fadeの情報
CDebugProc *CRenderer::m_pDebugProc = NULL;				// デバッグ表示情報
CCamera *CRenderer::m_pCamera = NULL;					// カメラの情報
CLight *CRenderer::m_pLight = NULL;						// ライトの情報
CGame *CRenderer::m_pGame = NULL;						// ゲーム情報
CTitle *CRenderer::m_pTitle = NULL;						// タイトル情報
CResult *CRenderer::m_pResult = NULL;					// リザルト情報
CRanking *CRenderer::m_pRanking = NULL;					// ランキング情報
CTutorial *CRenderer::m_pTutorial = NULL;				// チュートリアル情報
CSound *CRenderer::m_pSound = NULL;						// 音情報
CRenderer::MODE CRenderer::m_mode = MODE_GAME;			// 最初の画面

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CRenderer::CRenderer()
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CRenderer::~CRenderer()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;			// ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;	// プレゼンテーションパラメータ

	// Direct3Dオブジェクトの生成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));							// ワークをゼロクリア
	d3dpp.BackBufferWidth = SCREEN_WIDTH;						// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;						// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;						// バックバッファの形式
	d3dpp.BackBufferCount = 1;									// バックバッファの数
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;					// ダブルバッファの切り替え(映像信号に同期)
	d3dpp.EnableAutoDepthStencil = TRUE;						// デプスバッファ(Ｚバッファ)とステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;					// デプスバッファとして16bitを使う
	d3dpp.Windowed = bWindow;									// ウィンドウモード
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	// リフレッシュレート(現在の速度に合わせる)
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// インターバル(VSyncを待って描画)
	//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	// クライアント領域を直ちに更新する

	// Direct3Dデバイスの生成
	// [デバイス作成制御]<描画>と<頂点処理>をハードウェアで行なう
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,			// ディスプレイアダプタ
		D3DDEVTYPE_HAL,											// デバイスタイプ
		hWnd,													// フォーカスするウインドウへのハンドル
		D3DCREATE_HARDWARE_VERTEXPROCESSING,					// デバイス作成制御の組み合わせ
		&d3dpp,													// デバイスのプレゼンテーションパラメータ
		&m_pD3DDevice)))										// デバイスインターフェースへのポインタ
	{
		// 上記の設定が失敗したら
		// [デバイス作成制御]<描画>をハードウェアで行い、<頂点処理>はCPUで行なう
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&m_pD3DDevice)))
		{
			// 上記の設定が失敗したら
			// [デバイス作成制御]<描画>と<頂点処理>をCPUで行なう
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&m_pD3DDevice)))
			{
				// 初期化失敗
				return E_FAIL;
			}
		}
	}
#ifdef _DEBUG
	//デバッグロゴの初期化処理
	m_pDebugProc->Init();
#endif

	// フェードのロード
	CFade::Load();

	// フェード生成処理
	m_pFade = CFade::Create(m_mode);

	// 動的確保
	m_pSound = new CSound;

	// 音初期化
	m_pSound->Init(hWnd);

	// レンダーステートパラメータの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面(左回り)をカリングする
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);						// Zバッファ(カメラとオブジェクトの距離を記憶)を使用
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);						// ライティングモード有効
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// αブレンドを行う
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// αディスティネーションカラーの指定

	// サンプラーステートの設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	switch (m_mode)
	{
		// タイトルのとき
	case MODE_TITLE:
		// タイトルの生成
		m_pTitle = CTitle::Create();
		// BGM再生
		//m_pSound->PlaySound(CSound::SOUND_LABEL_BGMTITLE);
		break;

		// チュートリアルのとき
	case MODE_TUTORIAL:
		// チュートリアルの生成
		m_pTutorial = CTutorial::Create();
		// BGM再生
		//m_pSound->PlaySound(CSound::SOUND_LABEL_BGMTUTORIAL);
		break;

		// ゲームのとき
	case MODE_GAME:
		// ゲームの生成
		m_pGame = CGame::Create();
		// BGM再生
		//m_pSound->PlaySound(CSound::SOUND_LABEL_BGMGAME);
		break;

		// ランキングのとき
	case MODE_RANKING:
		// ランキングの生成
		m_pRanking = CRanking::Create();
		break;
	}

	// 値を返す
	return S_OK;
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CRenderer::Uninit(void)
{
#ifdef _DEBUG
	// デバッグロゴの終了処理
	m_pDebugProc->Uninit();
#endif

	if (m_pD3DDevice != NULL)
	{
		m_pD3DDevice->Release();						// 開放
		m_pD3DDevice = NULL;							// NULLにする
	}
	if (m_pD3D != NULL)
	{
		m_pD3D->Release();								// 開放
		m_pD3D = NULL;									// NULLにする
	}

	// フェードがあるとき
	if (m_pFade != NULL)
	{
		// 終了処理
		m_pFade->Uninit();

		// メモリ削除
		delete m_pFade;

		// ポインタNULL
		m_pFade = nullptr;
	}

	// 音があるとき
	if (m_pSound != NULL)
	{
		// 音の終了処理
		m_pSound->Uninit();

		// メモリ削除
		delete m_pSound;

		// ポインタNULL
		m_pSound = nullptr;
	}

	switch (m_mode)
	{
		// タイトルのとき
	case MODE_TITLE:
		// タイトルの終了処理
		m_pTitle->Uninit();
		delete m_pTitle;		// メモリの削除
		m_pTitle = nullptr;		// ポインタNULL
		break;

		// チュートリアルのとき
	case MODE_TUTORIAL:
		// チュートリアルの終了処理
		m_pTutorial->Uninit();
		delete m_pTutorial;		// メモリの削除
		m_pTutorial = nullptr;	// ポインタNULL
		break;

		// ゲームのとき
	case MODE_GAME:
		// ゲームの終了処理
		m_pGame->Uninit();
		delete m_pGame;			// メモリの削除
		m_pGame = nullptr;		// ポインタNULL
		break;

		// ランキングのとき
	case MODE_RANKING:
		// ランキングの終了処理
		m_pRanking->Uninit();
		delete m_pRanking;		// メモリの削除
		m_pRanking = nullptr;	// ポインタNULL
		break;
	}

}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CRenderer::Update(void)
{
	// ゲームの状態取得
	CGame::GAMESTATE gameState = CGame::GetGameState();

	// ゲーム状態がポーズじゃないとき
	if (gameState != CGame::GAMESTATE_PAUSE)
	{
		// Sceneで管理するすべての更新処理
		CScene::UpdateAll();
	}

	// フェードの更新処理
	m_pFade->Update();

	switch (m_mode)
	{
		// タイトルのとき
	case MODE_TITLE:
		// 更新処理
		m_pTitle->Update();
		break;

		// チュートリアルのとき
	case MODE_TUTORIAL:
		// 更新処理
		m_pTutorial->Update();
		break;

		// ゲームのとき
	case MODE_GAME:
		// 更新処理
		m_pGame->Update();
		break;

		// ランキングのとき
	case MODE_RANKING:
		// 更新処理
		m_pRanking->Update();
		break;
	}
}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CRenderer::Draw(void)
{
	// バックバッファ&Zバッファのクリア
	m_pD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// Direc3Dによる描画開始
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{
		// Sceneで管理するすべての描画処理
		CScene::DrawAll();

		switch (m_mode)
		{
			// タイトルのとき
		case MODE_TITLE:
			// 描画処理
			m_pTitle->Draw();
			break;

			// チュートリアルのとき
		case MODE_TUTORIAL:
			// 描画処理
			m_pTutorial->Draw();
			break;

			// ゲームのとき
		case MODE_GAME:
			// 描画処理
			m_pGame->Draw();
			break;

			// ランキングのとき
		case MODE_RANKING:
			// 描画処理
			m_pRanking->Draw();
			break;
		}

		// フェードがあるとき
		if (m_pFade != NULL)
		{
			// フェードの描画処理
			m_pFade->Draw();
		}

#ifdef _DEBUG
		// FPSカウンタ取得
		m_nCountFPS = GetFPS();

		// FPS表示
		m_pDebugProc->Print("FPS:%d\n", m_nCountFPS);

		// デバッグロゴの描画
		m_pDebugProc->Draw();
#endif

		// 開始されたシーンの終了
		m_pD3DDevice->EndScene();
	}

	// バックバッファとフロントバッファの入れ替え
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

//==================================================================================================================
// モードの設定
//==================================================================================================================
void CRenderer::SetMode(MODE mode)
{
	// 指定した音を止める
	switch (mode)
	{
		// タイトルのとき
	case MODE_TITLE:
		// 音を止める
		//m_pSound->StopSound(CSound::SOUND_LABEL_BGMRESULT);
		//m_pSound->StopSound(CSound::SOUND_LABEL_BGMGAME);
		break;

		// チュートリアルのとき
	case MODE_TUTORIAL:
		// 音を止める
		//m_pSound->StopSound(CSound::SOUND_LABEL_BGMTITLE);
		break;

		// ゲームとき
	case MODE_GAME:
		// 音を止める
		//m_pSound->StopSound(CSound::SOUND_LABEL_BGMTUTORIAL);
		break;

		// ランキングのとき
	case MODE_RANKING:
		// 音を止める
		//m_pSound->StopSound(CSound::SOUND_LABEL_BGMGAME);
		//m_pSound->StopSound(CSound::SOUND_LABEL_BGMTITLE);

		break;
	}

	switch (m_mode)
	{
		// タイトルのとき
	case MODE_TITLE:
		// 終了処理
		m_pTitle->Uninit();
		// 破棄
		delete m_pTitle;
		// NULLにする
		m_pTitle = nullptr;
		break;

		// チュートリアルのとき
	case MODE_TUTORIAL:
		// 終了処理
		m_pTutorial->Uninit();
		// 破棄
		delete m_pTutorial;
		// NULLにする
		m_pTutorial = nullptr;
		break;

		// ゲームのとき
	case MODE_GAME:
		// 終了処理
		m_pGame->Uninit();
		// 破棄
		delete m_pGame;
		// NULLにする
		m_pGame = nullptr;
		break;

		// ランキングのとき
	case MODE_RANKING:
		// 終了処理
		m_pRanking->Uninit();
		// 破棄
		delete m_pRanking;
		// NULLにする
		m_pRanking = nullptr;
		break;
	}

	// モード変数を代入
	m_mode = mode;

	switch (mode)
	{
		// タイトルのとき
	case MODE_TITLE:
		// 生成処理
		m_pTitle = CTitle::Create();
		//m_pSound->PlaySound(CSound::SOUND_LABEL_BGMTITLE);
		break;

		// チュートリアルのとき
	case MODE_TUTORIAL:
		// 生成処理
		m_pTutorial = CTutorial::Create();
		//m_pSound->PlaySound(CSound::SOUND_LABEL_BGMTUTORIAL);
		break;

		// ゲームのとき
	case MODE_GAME:
		// 生成処理
		m_pGame = CGame::Create();
		//m_pSound->PlaySound(CSound::SOUND_LABEL_BGMGAME);
		break;

		// ランキングのとき
	case MODE_RANKING:
		// 生成処理
		m_pRanking = CRanking::Create();
		//m_pSound->PlaySound(CSound::SOUND_LABEL_BGMRESULT);
		break;
	}
}

//==================================================================================================================
// モードの取得
//==================================================================================================================
CRenderer::MODE CRenderer::GetMode(void)
{
	return m_mode;
}

//==================================================================================================================
// ゲーム情報取得
//==================================================================================================================
CGame * CRenderer::GetGame(void)
{
	return m_pGame;
}

//==================================================================================================================
// 音情報取得
//==================================================================================================================
CSound * CRenderer::GetSound(void)
{
	return m_pSound;
}

//==================================================================================================================
// デバイス取得
//==================================================================================================================
LPDIRECT3DDEVICE9 CRenderer::GetDevice(void)
{
	// 値を返す
	return m_pD3DDevice;
}