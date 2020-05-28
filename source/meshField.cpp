//==================================================================================================================
//
// メッシュフィールド処理[meshField.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================
#include "renderer.h"
#include "meshField.h"
#include "inputKeyboard.h"
#include "debugProc.h"
#include "player.h"
#include "tutorial.h"
#include "game.h"
#include "title.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define MASS_WIDTH 100										// 横のマス
#define MASS_DEPTH 100										// 縦のマス
#define FIELD_ALPHA 0.1f									// メッシュフィールドアルファ

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
LPDIRECT3DTEXTURE9 CMeshField::m_pTexture = NULL;			// テクスチャ変数
CMeshField *CMeshField::m_pMeshField = NULL;				// メッシュフィールドの情報
CPlayer *CMeshField::m_pPlayer = NULL;						// プレイヤーの情報

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CMeshField::CMeshField(PRIORITY type = CScene::PRIORITY_FIELD) : CScene(type)
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CMeshField::~CMeshField()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
void CMeshField::Init(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();		// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();	// デバイスの取得

	m_nDepth = MASS_DEPTH;												// 縦のブロック数
	m_nWidth = MASS_WIDTH;												// 横のブロック数

	// 位置・回転の初期設定
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);								// 位置
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);								// 回転
	m_nNumVertex = ((m_nWidth + 1) * m_nDepth * 2) + (m_nDepth - 1) * 2;// 総頂点数
	m_nNumIndex = (m_nDepth + 1) * (m_nWidth + 1);						// 総インデックス数
	m_nNumPolygon = m_nNumVertex - 2;									// 三角形の数
	m_aVecA[m_nDepth * m_nWidth] = {};									// 法線ベクトルを面の数分一時的に格納
	m_aVecB[m_nDepth * m_nWidth] = {};									// 法線ベクトルを面の数分一時的に格納
	m_vectorA = D3DXVECTOR3(0, 0, 0);									// 一つ目の外積用変数
	m_vectorB = D3DXVECTOR3(0, 0, 0);									// 二つ目の外積用変数
	m_vectorC = D3DXVECTOR3(0, 0, 0);									// 三つ目の外積用変数
	m_nNumber = 0;														// 配列の番号
	m_nStartBox = m_nWidth + 1;											// 始まる箱
	m_nEndBox = 0;														// 引かれる箱
	fDivide = 0;														// sinの中身を割る変数

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
	for (int nDepth = 0; nDepth < m_nDepth + 1; nDepth++)
	{
		// 横をカウント
		for (int nWide = 0; nWide < m_nWidth + 1; nWide++)
		{
			// 頂点座標の設定
			m_pVtx[0].pos.x = (-WhileX * m_nWidth) / 2 + WhileX * nWide;
			m_pVtx[0].pos.y = 0.0f;
			m_pVtx[0].pos.z = (WhileZ / 2 * m_nDepth) - WhileZ * nDepth;

			// 頂点カラー
			m_pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, FIELD_ALPHA);

			// テクスチャ描写の位置
			m_pVtx[0].tex = D3DXVECTOR2(1.0f * nWide, 1.0f * nDepth);

			// 頂点番号加算
			m_pVtx++;
		}
	}

	// 頂点データをアンロック
	m_pVtxBuff->Unlock();

	// 頂点データの範囲をロックし、頂点バッファへのポインタ取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	//////////////////////////////法線ベクトル//////////////////////////////
	// 縦をカウント
	for (int nDepth = 0; nDepth < m_nDepth + 1; nDepth++)
	{
		// 横をカウント
		for (int nWide = 0; nWide < m_nWidth + 1; nWide++)
		{
			// インデックス番号
			int nIdx = nDepth * (m_nWidth + 1) + nWide;

			// 端は計算させない
			if (nWide != m_nWidth && nDepth != m_nDepth)
			{
				m_vectorA = m_pVtx[m_nStartBox].pos - m_pVtx[m_nEndBox].pos;								// 一つ目のベクトル
				m_vectorB = m_pVtx[m_nStartBox + 1].pos - m_pVtx[m_nEndBox].pos;							// 二つ目のベクトル
				m_vectorC = m_pVtx[m_nEndBox + 1].pos - m_pVtx[m_nEndBox].pos;								// 三つ目のベクトル
				D3DXVec3Cross(&m_aVecA[m_nNumber], &m_vectorA, &m_vectorB);									// 二つのベクトルの直交ベクトル
				D3DXVec3Normalize(&m_aVecA[m_nNumber], &m_aVecA[m_nNumber]);								// 正規化する
				D3DXVec3Cross(&m_aVecB[m_nNumber], &m_vectorB, &m_vectorC);									// 二つのベクトルの直交ベクトル
				D3DXVec3Normalize(&m_aVecB[m_nNumber], &m_aVecB[m_nNumber]);								// 正規化する

				// 配列の番号1プラスする
				m_nStartBox++;
				m_nEndBox++;
				m_nNumber++;
			}

			// 右上だったとき
			if (nIdx == m_nWidth)
			{
				// 法線ベクトル
				m_pVtx[nIdx].nor = m_aVecB[m_nWidth - 1];
			}
			else if (nIdx == m_nDepth * (m_nWidth + 1))
			{// 左下だったとき
				// 法線ベクトル
				m_pVtx[nIdx].nor = m_aVecA[(m_nDepth - 1) * m_nWidth];
			}
			else if (nIdx == 0)
			{// 左上だったとき
				// 法線ベクトル
				m_pVtx[nIdx].nor = (m_aVecA[nIdx] + m_aVecB[nIdx]) / 2;
			}
			else if (nIdx == (m_nDepth + 1) * (m_nWidth + 1) - 1)
			{// 右下だったとき
				// 法線ベクトル
				m_pVtx[nIdx].nor = (m_aVecA[m_nDepth * m_nWidth - 1] + m_aVecB[m_nDepth * m_nWidth - 1]) / 2;
			}
			else if (nWide != 0 && nWide != m_nWidth)
			{// 横の列が端じゃないとき
				// 縦が一列目だったとき
				if (nDepth == 0)
				{
					// 法線ベクトル
					m_pVtx[nIdx].nor = (m_aVecB[nWide - 1] + m_aVecA[nWide] + m_aVecB[nWide]) / 3;
				}
				else if (nDepth == m_nDepth)
				{// 縦が最後尾だったとき
					// 法線ベクトル
					m_pVtx[nIdx].nor = (m_aVecB[(m_nDepth - 1) * m_nWidth - 1 + nWide] +
						m_aVecA[(m_nDepth - 1) * m_nWidth + nWide] +
						m_aVecB[(m_nDepth - 1) * m_nWidth + nWide]) / 3;
				}
				else
				{// それ以外
					// 法線ベクトル
					m_pVtx[nIdx].nor = (m_aVecA[(nWide - 1) + (nDepth - 1) * m_nWidth] +
						m_aVecB[(nWide - 1) + (nDepth - 1) * m_nWidth] +
						m_aVecA[(nWide - 1) + (nDepth - 1) * m_nWidth + 1] +
						m_aVecB[(nWide - 1) + (nDepth - 1) * m_nWidth + m_nWidth] +
						m_aVecA[(nWide - 1) + (nDepth - 1) * m_nWidth + m_nWidth + 1] +
						m_aVecB[(nWide - 1) + (nDepth - 1) * m_nWidth + m_nWidth + 1]) / 6;
				}
			}
			else if (nDepth != 0 && nDepth != m_nDepth)
			{// 縦の列が端じゃないとき
				// 先頭のとき
				if (nWide == 0)
				{
					// 法線ベクトル
					m_pVtx[nIdx].nor = (m_aVecA[(nDepth - 1) * m_nWidth] +
						m_aVecB[m_nWidth + (nDepth - 1) * m_nWidth] +
						m_aVecA[m_nWidth + (nDepth - 1) * m_nWidth]) / 3;
				}
				else if (nWide == m_nWidth)
				{// 最後尾のとき
					// 法線ベクトル
					m_pVtx[nIdx].nor = (m_aVecB[m_nWidth - 1 + m_nWidth * (nDepth - 1)] +
						m_aVecA[m_nWidth - 1 + m_nWidth * (nDepth - 1)] +
						m_aVecB[m_nWidth - 1 + m_nWidth * nDepth]) / 3;
				}
			}
		}
		// 配列の番号1プラスする
		m_nStartBox++;
		m_nEndBox++;
	}

	// 頂点データをアンロック
	m_pVtxBuff->Unlock();

	WORD *pIdx;				// インデックスバッファへのポインタ

	// インデックスバッファをロックしインデックスデータへのポインタを取得
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	// 縦をカウント
	for (int nCntY = 0; nCntY < m_nDepth; nCntY++)
	{
		// 一行目じゃないとき
		if (nCntY != 0)
		{
			pIdx[0] = (m_nWidth + 1) + nCntY * (m_nWidth + 1);
			pIdx++;
		}

		// 横をカウント
		for (int nCntX = 0; nCntX < m_nWidth + 1; nCntX++)
		{
			pIdx[0] = (m_nWidth + 1) + nCntX + nCntY * (m_nWidth + 1);
			pIdx[1] = nCntX + nCntY * (m_nWidth + 1);
			pIdx += 2;
		}

		// 縮退ポリゴン
		if (nCntY + 1 < m_nDepth)
		{
			pIdx[0] = m_nWidth + nCntY * (m_nWidth + 1);
			pIdx++;
		}
	}

	// インデックスバッファをアンロックする
	m_pIdxBuff->Unlock();
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CMeshField::Uninit(void)
{
	// 頂点バッファがNULLじゃないとき
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();				// 頂点バッファの開放
		m_pVtxBuff = NULL;					// NULLにする
	}

	// インデックスバッファがNULLじゃないとき
	if (m_pIdxBuff != NULL)
	{
		m_pIdxBuff->Release();				// インデックスバッファの開放
		m_pIdxBuff = NULL;					// NULLにする
	}
}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CMeshField::Update(void)
{
	// キーボード取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	VERTEX_3D *m_pVtx;									// 頂点情報

	// 再初期化
	m_nNumber = 0;										// 配列の番号
	m_nStartBox = m_nWidth + 1;							// 始まる箱
	m_nEndBox = 0;										// 引かれる箱

	// 動かす
	fDivide -= 0.05f;

	// 頂点データの範囲をロックし、頂点バッファへのポインタ取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	// 縦をカウント
	for (int nDepth = 0; nDepth < m_nDepth + 1; nDepth++)
	{
		// 横をカウント
		for (int nWide = 0; nWide < m_nWidth + 1; nWide++)
		{
			// 頂点座標の設定
			m_pVtx[0].pos.x = (-WhileX * m_nWidth) / 2 + WhileX * nWide;
			m_pVtx[0].pos.y = 0.0f;
			m_pVtx[0].pos.z = (WhileZ / 2 * m_nDepth) - WhileZ * nDepth;

			// 頂点カラー
			m_pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, FIELD_ALPHA);

			// 頂点情報加算
			m_pVtx++;
		}
	}

	// 頂点データをアンロック
	m_pVtxBuff->Unlock();

	// 頂点データの範囲をロックし、頂点バッファへのポインタ取得
	m_pVtxBuff->Lock(0, 0, (void**)&m_pVtx, 0);

	//////////////////////////////法線ベクトル//////////////////////////////
	// 縦をカウント
	for (int nDepth = 0; nDepth < m_nDepth + 1; nDepth++)
	{
		// 横をカウント
		for (int nWide = 0; nWide < m_nWidth + 1; nWide++)
		{
			// インデックス番号
			int nIdx = nDepth * (m_nWidth + 1) + nWide;

			// 端は計算させない
			if (nWide != m_nWidth && nDepth != m_nDepth)
			{
				m_vectorA = m_pVtx[m_nStartBox].pos - m_pVtx[m_nEndBox].pos;								// 一つ目のベクトル
				m_vectorB = m_pVtx[m_nStartBox + 1].pos - m_pVtx[m_nEndBox].pos;							// 二つ目のベクトル
				m_vectorC = m_pVtx[m_nEndBox + 1].pos - m_pVtx[m_nEndBox].pos;								// 三つ目のベクトル
				D3DXVec3Cross(&m_aVecA[m_nNumber], &m_vectorA, &m_vectorB);									// 二つのベクトルの直交ベクトル
				D3DXVec3Normalize(&m_aVecA[m_nNumber], &m_aVecA[m_nNumber]);								// 正規化する
				D3DXVec3Cross(&m_aVecB[m_nNumber], &m_vectorB, &m_vectorC);									// 二つのベクトルの直交ベクトル
				D3DXVec3Normalize(&m_aVecB[m_nNumber], &m_aVecB[m_nNumber]);								// 正規化する

				// 配列の番号1プラスする
				m_nStartBox++;
				m_nEndBox++;
				m_nNumber++;
			}

			// 右上だったとき
			if (nIdx == m_nWidth)
			{
				// 法線ベクトル
				m_pVtx[nIdx].nor = m_aVecB[m_nWidth - 1];
			}
			else if (nIdx == m_nDepth * (m_nWidth + 1))
			{// 左下だったとき
				// 法線ベクトル
				m_pVtx[nIdx].nor = m_aVecA[(m_nDepth - 1) * m_nWidth];
			}
			else if (nIdx == 0)
			{// 左上だったとき
				// 法線ベクトル
				m_pVtx[nIdx].nor = (m_aVecA[nIdx] + m_aVecB[nIdx]) / 2;
			}
			else if (nIdx == (m_nDepth + 1) * (m_nWidth + 1) - 1)
			{// 右下だったとき
				// 法線ベクトル
				m_pVtx[nIdx].nor = (m_aVecA[m_nDepth * m_nWidth - 1] + m_aVecB[m_nDepth * m_nWidth - 1]) / 2;
			}
			else if (nWide != 0 && nWide != m_nWidth)
			{// 横の列が端じゃないとき
				// 縦が一列目だったとき
				if (nDepth == 0)
				{
					// 法線ベクトル
					m_pVtx[nIdx].nor = (m_aVecB[nWide - 1] + m_aVecA[nWide] + m_aVecB[nWide]) / 3;
				}
				else if (nDepth == m_nDepth)
				{// 縦が最後尾だったとき
					// 法線ベクトル
					m_pVtx[nIdx].nor = (m_aVecB[(m_nDepth - 1) * m_nWidth - 1 + nWide] +
						m_aVecA[(m_nDepth - 1) * m_nWidth + nWide] +
						m_aVecB[(m_nDepth - 1) * m_nWidth + nWide]) / 3;
				}
				else
				{// それ以外
					// 法線ベクトル
					m_pVtx[nIdx].nor = (m_aVecA[(nWide - 1) + (nDepth - 1) * m_nWidth] +
						m_aVecB[(nWide - 1) + (nDepth - 1) * m_nWidth] +
						m_aVecA[(nWide - 1) + (nDepth - 1) * m_nWidth + 1] +
						m_aVecB[(nWide - 1) + (nDepth - 1) * m_nWidth + m_nWidth] +
						m_aVecA[(nWide - 1) + (nDepth - 1) * m_nWidth + m_nWidth + 1] +
						m_aVecB[(nWide - 1) + (nDepth - 1) * m_nWidth + m_nWidth + 1]) / 6;
				}
			}
			else if (nDepth != 0 && nDepth != m_nDepth)
			{// 縦の列が端じゃないとき
				 // 先頭のとき
				if (nWide == 0)
				{
					// 法線ベクトル
					m_pVtx[nIdx].nor = (m_aVecA[(nDepth - 1) * m_nWidth] +
						m_aVecA[m_nWidth + (nDepth - 1) * m_nWidth] +
						m_aVecB[m_nWidth + (nDepth - 1) * m_nWidth]) / 3;
				}
				else if (nWide == m_nWidth)
				{// 最後尾のとき
					// 法線ベクトル
					m_pVtx[nIdx].nor = (m_aVecB[(m_nWidth - 1) + m_nWidth * nDepth] +
						m_aVecA[(m_nWidth - 1) + m_nWidth * nDepth] +
						m_aVecB[m_nWidth - 1 + m_nWidth * nDepth]) / 3;
				}
			}
		}
		// 配列の番号を1プラスする
		m_nStartBox++;
		m_nEndBox++;
	}

	// 頂点データをアンロック
	m_pVtxBuff->Unlock();
}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CMeshField::Draw(void)
{
	// レンダラー情報取得
	CRenderer *pRenderer = CManager::GetRenderer();			// レンダラーの情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;							// 計算用マトリックス

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

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

	// インデックスバッファをデータストリームに設定
	pDevice->SetIndices(m_pIdxBuff);

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, m_pTexture);

	// ポリゴンの描画
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_nNumVertex, 0, m_nNumPolygon);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);
}

//==================================================================================================================
// 位置設定
//==================================================================================================================
void CMeshField::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//==================================================================================================================
// ポリゴン生成
//==================================================================================================================
CMeshField *CMeshField::Create(void)
{
	// メモリを動的に確保
	m_pMeshField = new CMeshField(CScene::PRIORITY_FIELD);

	// メッシュ球があるとき
	if (m_pMeshField != NULL)
	{
		// シーン初期化
		m_pMeshField->Init();
	}

	// 値を返す
	return m_pMeshField;
}

//==================================================================================================================
// テクスチャ情報ロード
//==================================================================================================================
HRESULT CMeshField::Load(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();						// レンダラーの情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();					// デバイスを取得する

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,			// デバイスへのポインタ
		FIELD_TEXTUE,							// ファイルの名前
		&m_pTexture);							// 読み込むメモリー

	// 値を返す
	return S_OK;
}

//==================================================================================================================
// テクスチャ情報破棄
//==================================================================================================================
void CMeshField::Unload(void)
{
	// テクスチャ情報があるとき
	if (m_pTexture != NULL)
	{
		m_pTexture->Release();			// テクスチャ情報の開放
		m_pTexture = NULL;				// NULLにする
	}
}

//==================================================================================================================
// メッシュフィールド情報取得
//==================================================================================================================
CMeshField * CMeshField::GetMeshField(void)
{
	return m_pMeshField;
}

//==================================================================================================================
// 位置取得
//==================================================================================================================
D3DXVECTOR3 CMeshField::GetPos(void)
{
	return m_pos;
}

//==================================================================================================================
// 移動量取得
//==================================================================================================================
D3DXVECTOR3 CMeshField::GetMove(void)
{
	return m_move;
}

//==================================================================================================================
// 移動量設定
//==================================================================================================================
void CMeshField::SetMove(D3DXVECTOR3 move)
{
	m_move = move;
}