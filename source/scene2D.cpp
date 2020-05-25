//==================================================================================================================
//
// 描画2D[scene2D.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================
#include "manager.h"
#include "scene2D.h"
#include "renderer.h"
#include "inputKeyboard.h"

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
CScene2D *CScene2D::m_pScene2D;				// pScene2Dの情報

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CScene2D::CScene2D(PRIORITY type = CScene::PRIORITY_UI) : CScene(type)
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CScene2D::~CScene2D()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
void CScene2D::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();					// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスの取得

	// 初期化
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);							// 位置
	m_size = GetSize();												// 大きさ取得
	m_fAngle = atan2f(POLGON_X, POLGON_Y);							// 角度
	m_fLength = sqrtf(POLGON_X * POLGON_X + POLGON_Y * POLGON_Y);	// 長さ

	// オブジェクトの頂点バッファを生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);
	
	// 座標の設定
	m_pVtx[0].pos = D3DXVECTOR3(m_pos.x - (m_size.x / 2), m_pos.y - (m_size.y / 2), 0);
	m_pVtx[1].pos = D3DXVECTOR3(m_pos.x + (m_size.x / 2), m_pos.y - (m_size.y / 2), 0);
	m_pVtx[2].pos = D3DXVECTOR3(m_pos.x - (m_size.x / 2), m_pos.y + (m_size.y / 2), 0);
	m_pVtx[3].pos = D3DXVECTOR3(m_pos.x + (m_size.x / 2), m_pos.y + (m_size.y / 2), 0);

	// 1.0で固定、同次座標
	m_pVtx[0].rhw = 1.0f;
	m_pVtx[1].rhw = 1.0f;
	m_pVtx[2].rhw = 1.0f;
	m_pVtx[3].rhw = 1.0f;

	// 頂点カラー
	m_pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

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
void CScene2D::Uninit(void)
{
	// 頂点バッファがあるとき
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();				// 開放
		m_pVtxBuff = NULL;					// NULLにする
	}
}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CScene2D::Update(void)
{

}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CScene2D::Draw(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();			// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスの取得

	// 頂点バッファをデバイスのデータストリームにバインド
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// テクスチャの設定
	pDevice->SetTexture(0, m_pTexturePolygon);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// ポリゴン描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);
}

//==================================================================================================================
// テクスチャを貼る
//==================================================================================================================
void CScene2D::BindTex(LPDIRECT3DTEXTURE9 tex)
{
	m_pTexturePolygon = tex;
}

//==================================================================================================================
// 位置設定
//==================================================================================================================
void CScene2D::SetPos(D3DXVECTOR3 pos)
{
	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	// 位置取得
	m_pos = pos;

	// 移動座標の設定
	m_pVtx[0].pos = D3DXVECTOR3(m_pos.x - (m_size.x / 2), m_pos.y - (m_size.y / 2), 0);
	m_pVtx[1].pos = D3DXVECTOR3(m_pos.x + (m_size.x / 2), m_pos.y - (m_size.y / 2), 0);
	m_pVtx[2].pos = D3DXVECTOR3(m_pos.x - (m_size.x / 2), m_pos.y + (m_size.y / 2), 0);
	m_pVtx[3].pos = D3DXVECTOR3(m_pos.x + (m_size.x / 2), m_pos.y + (m_size.y / 2), 0);

	// 頂点データをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================================================
// 大きさ設定
//==================================================================================================================
void CScene2D::SetSize(D3DXVECTOR3 size)
{
	// 大きさ取得
	m_size = size;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	// 移動座標の設定
	m_pVtx[0].pos = D3DXVECTOR3(m_pos.x - (m_size.x / 2), m_pos.y - (m_size.y / 2), 0);
	m_pVtx[1].pos = D3DXVECTOR3(m_pos.x + (m_size.x / 2), m_pos.y - (m_size.y / 2), 0);
	m_pVtx[2].pos = D3DXVECTOR3(m_pos.x - (m_size.x / 2), m_pos.y + (m_size.y / 2), 0);
	m_pVtx[3].pos = D3DXVECTOR3(m_pos.x + (m_size.x / 2), m_pos.y + (m_size.y / 2), 0);

	// 頂点データをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================================================
// アニメーション設定
//==================================================================================================================
void CScene2D::SetAnimation(float fTexX, float fTexY, float fTexY2, int nPatternAnim)
{
	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	// テクスチャ座標の設定
	m_pVtx[0].tex = D3DXVECTOR2(0.0f + nPatternAnim * fTexX, fTexY2);
	m_pVtx[1].tex = D3DXVECTOR2(fTexX + nPatternAnim * fTexX, fTexY2);
	m_pVtx[2].tex = D3DXVECTOR2(0.0f + nPatternAnim * fTexX, fTexY);
	m_pVtx[3].tex = D3DXVECTOR2(fTexX + nPatternAnim * fTexX, fTexY);

	// 頂点データをアンロックする
	m_pVtxBuff->Unlock();
}

//==================================================================================================================
// スクロール設定
//==================================================================================================================
void CScene2D::SetMove(float fTexX, float fTexY, float fMoveX)
{
	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);
	
	// テクスチャ座標の設定
	m_pVtx[0].tex = D3DXVECTOR2(0.0f + fMoveX, 0.0f);
	m_pVtx[1].tex = D3DXVECTOR2(fTexX + fMoveX, 0.0f);
	m_pVtx[2].tex = D3DXVECTOR2(0.0f + fMoveX, fTexY);
	m_pVtx[3].tex = D3DXVECTOR2(fTexX + fMoveX, fTexY);

	// 頂点データをアンロックする
	m_pVtxBuff->Unlock();
}

//==================================================================================================================
// 色設定
//==================================================================================================================
void CScene2D::SetCol(D3DXCOLOR col)
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
// 2D生成
//==================================================================================================================
CScene2D *CScene2D::Create(void)
{
	// シーン動的に確保
	m_pScene2D = new CScene2D(CScene::PRIORITY_UI);

	// シーン初期化
	m_pScene2D->Init();

	// 値を返す
	return m_pScene2D;
}

//==================================================================================================================
// 当たり判定設定
//==================================================================================================================
bool CScene2D::SetCollision(D3DXVECTOR3 posDo, int sizeX, int sizeY, D3DXVECTOR3 posDone)
{
	bool bCollide = false;			// 当たり判定用変数

	//テクスチャ内に入ったら
	if ((posDone.x - sizeX / 2 <= posDo.x) &&
		(posDone.x + sizeX / 2 >= posDo.x) &&
		(posDone.y - sizeY / 2 <= posDo.y) &&
		(posDone.y + sizeY / 2 >= posDo.y))
	{
		bCollide = true;			// 当たっている
	}

	// 値を返す
	return bCollide;
}

//==================================================================================================================
// 回転設定
//==================================================================================================================
void CScene2D::SetRot(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fAngle, float fLength)
{
	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	// 位置を代入
	m_pos = pos;

	// 移動座標の設定
	m_pVtx[0].pos = D3DXVECTOR3(pos.x - sinf(fAngle + rot.x) * fLength, pos.y - cosf(fAngle) * fLength, 0.0f);
	m_pVtx[1].pos = D3DXVECTOR3(pos.x + sinf(fAngle + rot.x) * fLength, pos.y - cosf(fAngle) * fLength, 0.0f);
	m_pVtx[2].pos = D3DXVECTOR3(pos.x - sinf(fAngle + rot.x) * fLength, pos.y + cosf(fAngle) * fLength, 0.0f);
	m_pVtx[3].pos = D3DXVECTOR3(pos.x + sinf(fAngle + rot.x) * fLength, pos.y + cosf(fAngle) * fLength, 0.0f);

	// 頂点データをアンロック
	m_pVtxBuff->Unlock();
}