//==================================================================================================================
//
// ライト処理[light.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================
#include "light.h"
#include "scene.h"
#include "manager.h"
#include "renderer.h"
#include "inputKeyboard.h"
#include "debugProc.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define LIGHT_MOVE 0.1f			// ライトの移動量

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
CLight *CLight::m_pLight = NULL;									//ライトの情報

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CLight::CLight()
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CLight::~CLight()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
void CLight::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();					// レンダラーの情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスを取得する

	// ライトをクリアする
	ZeroMemory(&m_alight[0], sizeof(D3DLIGHT9));
	ZeroMemory(&m_alight[1], sizeof(D3DLIGHT9));
	ZeroMemory(&m_alight[2], sizeof(D3DLIGHT9));

	// ライトの種類を設定
	m_alight[0].Type = D3DLIGHT_DIRECTIONAL;
	m_alight[1].Type = D3DLIGHT_DIRECTIONAL;
	m_alight[2].Type = D3DLIGHT_DIRECTIONAL;

	// ライトの拡散光を設定
	m_alight[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_alight[1].Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 0.0f);
	m_alight[2].Diffuse = D3DXCOLOR(0.2f, 0.2f, 0.2f, 0.0f);

	// ライトの方向の設定
	m_avecDir[0] = D3DXVECTOR3(0.22f, -0.87f, 0.44f);				// 位置
	D3DXVec3Normalize(&m_avecDir[0], &m_avecDir[0]);				// 正規化する
	m_alight[0].Position = m_avecDir[0];

	// ライトの方向の設定
	m_avecDir[1] = D3DXVECTOR3(-0.18f, 0.87f, -0.44f);				// 位置
	D3DXVec3Normalize(&m_avecDir[1], &m_avecDir[1]);				// 正規化する
	m_alight[1].Direction = m_avecDir[1];

	// ライトの方向の設定
	m_avecDir[2] = D3DXVECTOR3(0.89f, -0.11f, 0.44f);				// 位置
	D3DXVec3Normalize(&m_avecDir[2], &m_avecDir[2]);				// 正規化する
	m_alight[2].Direction = m_avecDir[2];

	// ライトを設定する
	pDevice->SetLight(0, &m_alight[0]);
	pDevice->SetLight(1, &m_alight[1]);
	pDevice->SetLight(2, &m_alight[2]);

	// ライトを有効にする
	pDevice->LightEnable(0, TRUE);
	pDevice->LightEnable(1, TRUE);
	pDevice->LightEnable(2, TRUE);
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CLight::Uninit(void)
{

}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CLight::Update(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();					// レンダラーの情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスを取得する

	// ライトの方向の設定
	D3DXVec3Normalize(&m_avecDir[0], &m_avecDir[0]);				// 正規化する
	m_alight[0].Direction = m_avecDir[0];

	// ライトの方向の設定
	D3DXVec3Normalize(&m_avecDir[1], &m_avecDir[1]);				// 正規化する
	m_alight[1].Direction = m_avecDir[1];

	// ライトの方向の設定
	D3DXVec3Normalize(&m_avecDir[2], &m_avecDir[2]);				// 正規化する
	m_alight[2].Direction = m_avecDir[2];

	// ライトを設定する
	pDevice->SetLight(0, &m_alight[0]);
	pDevice->SetLight(1, &m_alight[1]);
	pDevice->SetLight(2, &m_alight[2]);
}

//==================================================================================================================
// ライトの生成
//==================================================================================================================
CLight * CLight::Create(void)
{
	// メモリを動的に確保
	m_pLight = new CLight();

	// ライトの初期化
	m_pLight->Init();

	// 値を返す
	return m_pLight;
}

