//==================================================================================================================
//
// 描画3D[scene3D.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================
#include "manager.h"
#include "scene3D.h"
#include "renderer.h"
#include "inputKeyboard.h"
#include "debugProc.h"

//==================================================================================================================
// 静的メンバー変数の初期化
//==================================================================================================================
CScene3D *CScene3D::m_pScene3D = NULL;			// Scene3Dの情報

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define SHADOW_SIZE 20.0f		// 影のサイズ

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CScene3D::CScene3D(PRIORITY type) : CScene(type)
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CScene3D::~CScene3D()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
void CScene3D::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();					// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスの取得

	// 初期化
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);							// 位置
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);							// 回転
	m_size = D3DXVECTOR3(1.0f, 1.0f, 1.0f);							// 大きさ
	m_fAngle = atan2f(POLGON_X, POLGON_Y);							// 角度
	m_fLength = sqrtf(POLGON_X * POLGON_X + POLGON_Y * POLGON_Y);	// 長さ
	D3DXVECTOR3 vectorA = D3DXVECTOR3(0, 0, 0);						// Aベクトル
	D3DXVECTOR3 vectorB = D3DXVECTOR3(0, 0, 0);						// Bベクトル
	D3DXVECTOR3 vectorC = D3DXVECTOR3(0, 0, 0);						// Cベクトル
	D3DXVECTOR3 vectorD = D3DXVECTOR3(0, 0, 0);						// Dベクトル
	m_NorA = D3DXVECTOR3(0, 0, 0);									// A法線
	m_NorB = D3DXVECTOR3(0, 0, 0);									// B法線
	m_Height = 0.0f;												// 接地面の高さ

	// オブジェクトの頂点バッファを生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	// 座標の設定
	// [0]頂点
	m_pVtx[0].pos.x = m_pos.x + (-SHADOW_SIZE / 2);
	m_pVtx[0].pos.y = 0;
	m_pVtx[0].pos.z = m_pos.z + (SHADOW_SIZE / 2);

	// [1]頂点
	m_pVtx[1].pos.x = m_pos.x + (SHADOW_SIZE / 2);
	m_pVtx[1].pos.y = 0;
	m_pVtx[1].pos.z = m_pos.z + (SHADOW_SIZE / 2);

	// [2]頂点
	m_pVtx[2].pos.x = m_pos.x + (-SHADOW_SIZE / 2);
	m_pVtx[2].pos.y = 0;
	m_pVtx[2].pos.z = m_pos.z + (-SHADOW_SIZE / 2);

	// [3]頂点
	m_pVtx[3].pos.x = m_pos.x + (SHADOW_SIZE / 2);
	m_pVtx[3].pos.y = 0;
	m_pVtx[3].pos.z = m_pos.z + (-SHADOW_SIZE / 2);

	vectorA = m_pVtx[1].pos - m_pVtx[2].pos;		// Aベクトル算出
	vectorB = m_pVtx[0].pos - m_pVtx[1].pos;		// Bベクトル算出
	D3DXVec3Cross(&m_NorA, &vectorA, &vectorB);		// 直交ベクトル算出
	D3DXVec3Normalize(&m_NorA, &m_NorA);			// 正規化する

	vectorC = m_pVtx[2].pos - m_pVtx[1].pos;		// Cベクトル算出
	vectorD = m_pVtx[3].pos - m_pVtx[2].pos;		// Dベクトル算出
	D3DXVec3Cross(&m_NorB, &vectorC, &vectorD);		// 直交ベクトル算出
	D3DXVec3Normalize(&m_NorB, &m_NorB);			// 正規化する

	// 法線ベクトルの設定
	m_pVtx[0].nor = m_NorA;							// そのまま代入
	m_pVtx[1].nor = (m_NorA + m_NorB) / 2;			// 三角形が重なっているところは割る2
	m_pVtx[2].nor = (m_NorA + m_NorB) / 2;			// 三角形が重なっているところは割る2
	m_pVtx[3].nor = m_NorB;							// そのまま代入

	// 頂点カラー
	m_pVtx[0].col = D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.5f);
	m_pVtx[1].col = D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.5f);
	m_pVtx[2].col = D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.5f);
	m_pVtx[3].col = D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.5f);

	// テクスチャ座標の設定
	m_pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	m_pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	m_pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	m_pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点データをアンロックする
	m_pVtxBuff->Unlock();
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CScene3D::Uninit(void)
{
	// 頂点バッファがあるとき
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();		// 開放
		m_pVtxBuff = NULL;			// NULLにする
	}
}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CScene3D::Update(void)
{
	// 位置取得
	m_pos = GetPos();

	// 大きさ取得
	m_size = GetSize();

	// 位置設定
	SetPos(m_pos);
}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CScene3D::Draw(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();			// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;					// 計算用マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, m_size.x, m_size.y, m_size.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	pDevice->SetTexture(0, m_pTexturePolygon);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//テクスチャの設定
	pDevice->SetTexture(0, NULL);
}

//==================================================================================================================
// テクスチャ結合
//==================================================================================================================
void CScene3D::BindTex(LPDIRECT3DTEXTURE9 tex)
{
	m_pTexturePolygon = tex;
}

//==================================================================================================================
// 位置設定
//==================================================================================================================
void CScene3D::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//==================================================================================================================
// 大きさ設定
//==================================================================================================================
void CScene3D::SetSize(D3DXVECTOR3 size)
{
	m_size = size;
}

//==================================================================================================================
// 色設定
//==================================================================================================================
void CScene3D::SetCol(D3DXCOLOR col)
{
	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	// 頂点カラー
	m_pVtx[0].col = col;
	m_pVtx[1].col = col;
	m_pVtx[2].col = col;
	m_pVtx[3].col = col;

	// 頂点データをアンロックする
	m_pVtxBuff->Unlock();
}

//==================================================================================================================
// 生成処理
//==================================================================================================================
CScene3D *CScene3D::Create(void)
{
	// シーン動的に確保
	m_pScene3D = new CScene3D(CScene::PRIORITY_FIELD);

	// シーン初期化
	m_pScene3D->Init();

	// 値を返す
	return m_pScene3D;
}

//==================================================================================================================
// CScene3Dの取得
//==================================================================================================================
CScene3D *CScene3D::GetScene3D(void)
{
	return m_pScene3D;
}