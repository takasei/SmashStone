//==================================================================================================================
//
// カメラ[camera.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================
#include "camera.h"
#include "scene.h"
#include "renderer.h"
#include "inputKeyboard.h"
#include "debugProc.h"
#include "player.h"
#include "game.h"
#include "tutorial.h"
#include "title.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define camera_move 3.1f							// カメラの移動スピード
#define CAMERA_ROT_SPEED 0.05f						// カメラの回転スピード
#define cameraLength 100.0f							// カメラからの距離
#define posR_Length 3.0f							// モデルと注視点の位置
#define posR_distance 80.0f							// モデルと注視点の距離
#define coefficient 0.05f							// 係数
#define posV_distance 100.0f						// モデルと視点の距離
#define posV_Height 70.0f							// 視点の高さ
#define SHRINK_SPEED_Y 0.2f							// posVのYの縮める速度
#define ROT_SHRINK 0.1f								// 回転の縮める速度
#define TITLE_FADE_COUNT 60							// タイトルフェードカウント
#define TITLE_POSV_X 0.0f							// タイトルでのカメラ視点位置X
#define TITLE_POSV_Y 20.0f							// タイトルでのカメラ視点位置Y
#define TITLE_POSV_Z 50.0f							// タイトルでのカメラ視点位置Z
#define TITLE_POSR_X 0.0f							// タイトルでのカメラ注視点位置X
#define TITLE_POSR_Y 1.0f							// タイトルでのカメラ注視点位置Y
#define TITLE_POSR_Z 50.0f							// タイトルでのカメラ注視点位置Z
#define DISTANCE 100.0f								// 視点と注視点の距離
#define ROT_COUNT 5									// 回転を始めるカウント

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
CCamera *CCamera::m_pCamera = NULL;					// カメラ情報

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CCamera::CCamera()
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CCamera::~CCamera()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
void CCamera::Init(void)
{
	// 変数の初期化
	m_posV = D3DXVECTOR3(0.0f, 100.0f, -200.0f);		// 視点
	m_posVDest = D3DXVECTOR3(0.0f, 100.0f, -200.0f);	// 視点の目標地点
	m_posR = D3DXVECTOR3(0.0f, 10.0f, 0.0f);			// 注視点
	m_posRDest = D3DXVECTOR3(0.0f, 10.0f, 0.0f);		// 注視点の目標地点
	m_posU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				// 上方向ベクトル
	m_rotDest = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 回転の目標地点
	m_fDistance = DISTANCE;								// 視点と注視点の距離
	nCntRot = 0;										// 回転を始めるカウンタ
	m_nCntTitleFade = 0;								// タイトルフェードカウンタ
	m_bCameraMode = false;								// カメラモードかどうか

	// ゲームモードがタイトル 又は チュートリアル 又は ゲームのとき
	if (CRenderer::GetMode() == CRenderer::MODE_TITLE || CRenderer::GetMode() == CRenderer::MODE_TUTORIAL || CRenderer::GetMode() == CRenderer::MODE_GAME)
	{
		m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 回転
	}
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CCamera::Uninit(void)
{

}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CCamera::Update(void)
{
	// キーボード取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// ゲームモードがタイトルのとき
	if (CRenderer::GetMode() == CRenderer::MODE_TITLE)
	{

	}

	// ゲームモードがゲームのとき 又は チュートリアルのとき
	if (CRenderer::GetMode() == CRenderer::MODE_GAME || CRenderer::GetMode() == CRenderer::MODE_TUTORIAL)
	{
		// ゲームモードがチュートリアルのとき
		if (CRenderer::GetMode() == CRenderer::MODE_TUTORIAL)
		{

		}

		// ゲームモードがゲームのとき
		if (CRenderer::GetMode() == CRenderer::MODE_GAME)
		{

		}

		// 視点の最終目的座標の計算
		//m_posVDest.x = sinf(m_rot.y) * m_fDistance;
		//m_posVDest.y = cosf(D3DX_PI + m_rot.x) * (-m_fDistance + posV_Height);
		//m_posVDest.z = cosf(m_rot.y) * m_fDistance;
		//m_posVDest.x = 0.0f;
		//m_posVDest.y = 100.0f;
		//m_posVDest.z = -200.0f;

		//// 注視点の最終目的座標の計算
		//m_posRDest.x = 0.0f;
		//m_posRDest.y = 10.0f;
		//m_posRDest.z = 0.0f;

		//// カメラの位置適応
		//m_posV.x += (m_posVDest.x - m_posV.x) * 1.0f;
		//m_posV.y += (m_posVDest.y - m_posV.y) * SHRINK_SPEED_Y;
		//m_posV.z += (m_posVDest.z - m_posV.z) * 1.0f;
		//m_posR += (m_posRDest - m_posR) * 1.0f;
	}

#ifdef _DEBUG

	// カメラモードがtrueのとき
	if (m_bCameraMode)
	{
		// キーボードの[X]が押されたとき
		if (pInputKeyboard->GetKeyboardTrigger(DIK_X))
		{
			m_bCameraMode = false;
		}

		// キーボードの[←]が押されたとき
		if (pInputKeyboard->GetKeyboardPress(DIK_LEFTARROW))
		{
			// 視点を左に動かす
			m_posV.x += sinf(-D3DX_PI * 0.5f + m_rot.y) * camera_move;
			m_posV.z += cosf(-D3DX_PI * 0.5f + m_rot.y) * camera_move;

			// 視点を左に動かす
			m_posR.x += sinf(-D3DX_PI * 0.5f + m_rot.y) * camera_move;
			m_posR.z += cosf(-D3DX_PI * 0.5f + m_rot.y) * camera_move;
		}

		// キーボードの[→]が押されたとき
		if (pInputKeyboard->GetKeyboardPress(DIK_RIGHTARROW))
		{
			// 視点を右に動かす
			m_posV.x += sinf(D3DX_PI * 0.5f + m_rot.y) * camera_move;
			m_posV.z += cosf(D3DX_PI * 0.5f + m_rot.y) * camera_move;
			// 視点を右に動かす
			m_posR.x += sinf(D3DX_PI * 0.5f + m_rot.y) * camera_move;
			m_posR.z += cosf(D3DX_PI * 0.5f + m_rot.y) * camera_move;
		}

		// キーボードの[↑]が押されたとき
		if (pInputKeyboard->GetKeyboardPress(DIK_UP))
		{
			// 視点を前に動かす
			m_posV.x += sinf(D3DX_PI * 0.0f + m_rot.y) * camera_move;
			m_posV.z += cosf(D3DX_PI * 0.0f + m_rot.y) * camera_move;
			// 視点を前に動かす
			m_posR.x += sinf(D3DX_PI * 0.0f + m_rot.y) * camera_move;
			m_posR.z += cosf(D3DX_PI * 0.0f + m_rot.y) * camera_move;
		}

		// キーボードの[↓]が押されたとき
		if (pInputKeyboard->GetKeyboardPress(DIK_DOWN))
		{
			// 視点を後ろに動かす
			m_posV.x += sinf(-D3DX_PI * 1.0f + m_rot.y) * camera_move;
			m_posV.z += cosf(-D3DX_PI * 1.0f + m_rot.y) * camera_move;
			// 視点を後ろに動かす
			m_posR.x += sinf(-D3DX_PI * 1.0f + m_rot.y) * camera_move;
			m_posR.z += cosf(-D3DX_PI * 1.0f + m_rot.y) * camera_move;
		}

		if (pInputKeyboard->GetKeyboardPress(DIK_1))
		{// キーボードの[1]が押されたとき
		 // 視点を上に動かす
			m_posV.y += cosf(D3DX_PI * 1.0f + m_rot.y) * camera_move;
			// 視点を上に動かす
			m_posR.y += cosf(D3DX_PI * 1.0f + m_rot.y) * camera_move;
		}

		// キーボードの[2]が押されたとき
		if (pInputKeyboard->GetKeyboardPress(DIK_2))
		{
			// 視点を下に動かす
			m_posV.y += cosf(D3DX_PI * 0.0f + m_rot.y) * camera_move;
			// 視点を下に動かす
			m_posR.y += cosf(D3DX_PI * 0.0f + m_rot.y) * camera_move;
		}

		// キーボードの[Q]が押されたとき
		if (pInputKeyboard->GetKeyboardPress(DIK_Q))
		{
			// 視点を中心に回転する(左回り)
			m_rot.y -= CAMERA_ROT_SPEED;

			// 回転情報が-D3DX_PIより小さくなったとき
			if (m_rot.y < -D3DX_PI)
			{
				// 一周回転させる
				m_rot.y += D3DX_PI * 2;
			}
		}

		// キーボードの[E]が押されたとき
		if (pInputKeyboard->GetKeyboardPress(DIK_E))
		{
			// 視点を中心に回転する(右回り)
			m_rot.y += CAMERA_ROT_SPEED;

			// 回転情報がD3DX_PIより大きくなったとき
			if (m_rot.y > D3DX_PI)
			{
				// 一周戻す
				m_rot.y -= D3DX_PI * 2;
			}
		}

		// キーボードの[C]が押されたとき
		if (pInputKeyboard->GetKeyboardPress(DIK_C))
		{
			// 注視点を中心に回転する(右回り)
			m_rot.y -= CAMERA_ROT_SPEED;

			// 回転情報が-D3DX_PIより小さくなったとき
			if (m_rot.y < -D3DX_PI)
			{
				// 一周回転させる
				m_rot.y += D3DX_PI * 2;
			}
		}

		// キーボードの[Z]が押されたとき
		if (pInputKeyboard->GetKeyboardPress(DIK_Z))
		{
			// 注視点を中心に回転する(左回り)
			m_rot.y += CAMERA_ROT_SPEED;

			// 回転情報がD3DX_PIより大きくなったとき
			if (m_rot.y > D3DX_PI)
			{
				// 一周戻す
				m_rot.y -= D3DX_PI * 2;
			}
		}
	}
	else
	{// カメラモードがfalseのとき
		// キーボードの[X]が押されたとき
		if (pInputKeyboard->GetKeyboardTrigger(DIK_X))
		{// カメラを操作するモード
			// カメラモードをtrueにする
			m_bCameraMode = true;
		}
	}


#endif // _DEBUG

	// 回転情報設定
	SetRot(m_rot);

	// 視点位置設定
	SetPosV(m_posV);
}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CCamera::Draw(void)
{
#ifdef _DEBUG
	// デバッグ表示
	CDebugProc::Print("注視点位置：%.2f, %.2f, %.2f\n", m_posR.x, m_posR.y, m_posR.z);
	CDebugProc::Print("視点位置 ：%.2f, %.2f, %.2f\n", m_posV.x, m_posV.y, m_posV.z);
	CDebugProc::Print("カメラ回転 ：%.2f, %.2f, %.2f\n", m_rot.x, m_rot.y, m_rot.z);
	CDebugProc::Print("カメラモード ：X\n");
	CDebugProc::Print("カメラモード ：%s", m_bCameraMode ? "true\n" : "false\n");
#endif // _DEBUG
}

//==================================================================================================================
// カメラの設定
//==================================================================================================================
void CCamera::SetCamera(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();					// レンダラーの情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスを取得する

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxProjection);

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(
		&m_mtxProjection,
		D3DXToRadian(45.0f),										// 視野角
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,					// アスペクト比
		10.0f,														// NearZ値
		6000.0f);													// FarZ値

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &m_mtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxView);

	// ビューマトリックスを作成
	D3DXMatrixLookAtLH(
		&m_mtxView,
		&m_posV,													// カメラの視点
		&m_posR,													// カメラの注視点
		&m_posU);													// カメラの上方向ベクトル

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &m_mtxView);
}

//==================================================================================================================
// カメラの回転情報設定
//==================================================================================================================
void CCamera::SetRot(D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//==================================================================================================================
// 視点位置設定処理
//==================================================================================================================
void CCamera::SetPosV(D3DXVECTOR3 posV)
{
	m_posV = posV;
}

//==================================================================================================================
// カメラの回転情報取得
//==================================================================================================================
D3DXVECTOR3 CCamera::GetRot(void)
{
	return m_rot;
}

//==================================================================================================================
// 視点位置取得
//==================================================================================================================
D3DXVECTOR3 CCamera::GetPosV(void)
{
	return m_posV;
}

//==================================================================================================================
// カメラの生成
//==================================================================================================================
CCamera * CCamera::Create(void)
{
	// メモリを動的に確保
	m_pCamera = new CCamera();

	// カメラの初期化
	m_pCamera->Init();

	// 値を返す
	return m_pCamera;
}