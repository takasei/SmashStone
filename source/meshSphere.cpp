//==================================================================================================================
//
// メッシュ球描画[meshSphere.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================
#include "manager.h"
#include "renderer.h"
#include "meshSphere.h"
#include "debugProc.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define RADIUS 4000.0f;				// 半径
#define WIDTH 30					// 横のマスの数
#define DEPTH 15					// 縦のマスの数

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
LPDIRECT3DTEXTURE9 CMeshSphere::m_pTexture = NULL;			// テクスチャ変数
CMeshSphere *CMeshSphere::m_pMeshShere = NULL;				// メッシュ球の情報

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CMeshSphere::CMeshSphere(PRIORITY type = CScene::PRIORITY_MESH_SPHERE) :CScene(type)
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CMeshSphere::~CMeshSphere()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
void CMeshSphere::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();		// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得

	// 位置・回転の初期設定
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 位置
	m_nNumVertex = 2 * DEPTH * (WIDTH + 1);			// 実際の頂点数
	m_nNumIndex = (WIDTH + 1) * (DEPTH + 1);		// 見た目の頂点数
	m_nNumPolygon = m_nNumVertex - 2;				// 三角形の数
	m_vector = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// ベクトル
	m_Nor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 法線

	// 頂点情報の作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * m_nNumIndex,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// インデックスバッファを生成
	pDevice->CreateIndexBuffer(
		sizeof(WORD) * m_nNumVertex,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	// 頂点データの範囲をロックし、頂点バッファへのポインタ取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	// 縦をカウント
	for (int nDepth = 0; nDepth < DEPTH + 1; nDepth++)
	{
		// 横をカウント
		for (int nWidth = 0; nWidth < WIDTH + 1; nWidth++)
		{
			float theta = D3DX_PI / DEPTH * nDepth;		// ラジアン値計算
			float phi = 2 * D3DX_PI / WIDTH * nWidth;	// ラジアン値計算
			float x = sinf(theta) * cosf(phi) * RADIUS;	// X座標算出
			float y = cosf(theta) * RADIUS;				// Y座標算出
			float z = sinf(theta) * sinf(phi) *RADIUS;	// Z座標算出
			float fLength = 0.0f;						// ベクトルの大きさ

			// 頂点座標の設定
			m_pVtx[0].pos = D3DXVECTOR3(x, y, z);

			m_vector = m_pVtx[0].pos - m_pos;			// 円周上の座標 - 中心座標
			fLength = sqrtf((m_vector.x * m_vector.x) + // 長さ算出
				(m_vector.y * m_vector.y) + (m_vector.z * m_vector.z));
			m_Nor.x = m_vector.x / fLength;				// 正規化
			m_Nor.y = m_vector.y / fLength;				// 正規化
			m_Nor.z = m_vector.z / fLength;				// 正規化

			m_Nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 頂点情報に法線の計算結果代入
			m_pVtx[0].nor = m_Nor;

			// テクスチャ描写の位置
			m_pVtx[0].tex = D3DXVECTOR2(1.0f / WIDTH * nWidth, 1.0f / DEPTH * nDepth);

			// 頂点カラー
			m_pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// 頂点番号加算
			m_pVtx++;
		}
	}

	// 頂点データをアンロック
	m_pVtxBuff->Unlock();

	WORD *pIdx;				// インデックスバッファへのポインタ
	int nNumber = 0;		// インデックス番号

	// インデックスバッファをロックしインデックスデータへのポインタを取得
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	// 縦をカウント
	for (int nDepth = 0; nDepth < DEPTH; nDepth++)
	{
		// 横をカウント
		for (int nWidth = 0; nWidth < WIDTH + 1; nWidth++)
		{
			// 順にインデックス格納
			pIdx[nNumber++] = nDepth * (WIDTH + 1) + nWidth;
			pIdx[nNumber++] = pIdx[nNumber - 1] + WIDTH + 1;

		}
	}

	// インデックスバッファをアンロックする
	m_pIdxBuff->Unlock();
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CMeshSphere::Uninit(void)
{
	// 頂点バッファがNULLじゃないとき
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();				// 頂点バッファの開放
		m_pVtxBuff = NULL;					// NULLにする
	}
}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CMeshSphere::Update(void)
{

}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CMeshSphere::Draw(void)
{
	// レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();			// レンダラーの情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスの取得
	D3DXMATRIX mtxTrans;									// 計算用マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	// 第一引数は同じポリゴンに複数テクスチャを貼る場合に使用する
	pDevice->SetTexture(0, m_pTexture);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVertex, 0, m_nNumPolygon);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);
}

//==================================================================================================================
// 位置設定
//==================================================================================================================
void CMeshSphere::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//==================================================================================================================
// メッシュ球生成
//==================================================================================================================
CMeshSphere *CMeshSphere::Create(void)
{
	// メモリを動的に確保
	m_pMeshShere = new CMeshSphere(CScene::PRIORITY_MESH_SPHERE);

	// メッシュ球があるとき
	if (m_pMeshShere != NULL)
	{
		// 初期化処理
		m_pMeshShere->Init();
	}

	// 値を返す
	return m_pMeshShere;
}

//==================================================================================================================
// テクスチャ情報ロード
//==================================================================================================================
HRESULT CMeshSphere::Load(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();			// レンダラーの情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスを取得する

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,			// デバイスへのポインタ
		SKY_TEXTUE,								// ファイルの名前
		&m_pTexture);							// 読み込むメモリー

	// 値を返す
	return S_OK;
}

//==================================================================================================================
// テクスチャ情報破棄
//==================================================================================================================
void CMeshSphere::Unload(void)
{
	// テクスチャ情報があるとき
	if (m_pTexture != NULL)
	{
		m_pTexture->Release();			// テクスチャ情報の開放
		m_pTexture = NULL;				// NULLにする
	}
}
