//==================================================================================================================
//
// モデルタイプ別描画[motionModel.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================
#include "renderer.h"
#include "motionModel.h"
#include "debugProc.h"
#include "game.h"
#include "player.h"

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
LPDIRECT3DTEXTURE9 CMotionModel::m_pTex[MODEL_MAX] = {};		// テクスチャ情報
LPD3DXMESH CMotionModel::m_pMesh[MODEL_MAX] = {};				// メッシュ情報
LPD3DXBUFFER CMotionModel::m_pBuffMat[MODEL_MAX] = {};			// バッファ情報
DWORD CMotionModel::m_nNumMat[MODEL_MAX] = {};					// マテリアル情報の数
CMotionModel *CMotionModel::m_pMotionModel[MODEL_MAX] = {};		// モデル情報
CPlayer *CMotionModel::m_pPlayer = NULL;						// プレイヤー情報
char *CMotionModel::m_apFileName[MODEL_MAX] =					// 読み込むモデルのソース先
{
	{ "data/MODEL/PenguinBody.x" },								// ペンギン体
	{ "data/MODEL/PenguinHead.x" },								// ペンギン頭
	{ "data/MODEL/PenguinLArm.x" },								// ペンギン左腕
	{ "data/MODEL/PenguinRArm.x" },								// ペンギン右腕
	{ "data/MODEL/PenguinLLeg.x" },								// ペンギン左足
	{ "data/MODEL/PenguinRLeg.x" },								// ペンギン右足
};

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CMotionModel::CMotionModel()
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CMotionModel::~CMotionModel()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
void CMotionModel::Init(void)
{
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 位置
	m_size = D3DXVECTOR3(1.0f, 1.0f, 1.0f);				// 大きさ
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 回転
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 移動量
	m_pParent = NULL;									// 親モデル番号
	m_type = MODEL_PENGUIN_BODY;						// モデルタイプ
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CMotionModel::Uninit(void)
{
	// 全てのモデルカウント
	for (int nCnt = 0; nCnt < MODEL_MAX; nCnt++)
	{
		// メッシュがあるとき
		if (m_pMesh[nCnt] != NULL)
		{
			m_pMesh[nCnt]->Release();				// 開放
			m_pMesh[nCnt] = NULL;					// NULLにする
		}

		// マテリアルがあるとき
		if (m_pBuffMat[nCnt] != NULL)
		{
			m_pBuffMat[nCnt]->Release();			// 開放
			m_pBuffMat[nCnt] = NULL;				// NULLにする
		}
	}
}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CMotionModel::Update(void)
{
	m_rot = GetRot();				// 回転情報取得
	m_size = GetSize();				// 大きさ取得
	m_pos = GetPos();				// 位置取得
	m_move = GetMove();				// 移動量取得

	SetRot(m_rot);					// 回転情報設定
	SetSize(m_size);				// 大きさ設定
	SetPos(m_pos);					// 位置設定
	SetMove(m_move);				// 移動量設定
}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CMotionModel::Draw(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();			// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;					// 計算用格納変数
	D3DXMATERIAL *pMat;										// 現在のマテリアル保存用
	D3DMATERIAL9 matDef;									// マテリアル情報ポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, m_size.x, m_size.y, m_size.z);
	// 2つの行列の積
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	// 2つの行列の積
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	// 2つの行列の積
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックス情報
	m_pos.x = m_mtxWorld._41;
	m_pos.y = m_mtxWorld._42;
	m_pos.z = m_mtxWorld._43;

	//親子関係
	if (m_pParent == NULL)
	{
		// 2つの行列の積
		D3DXMatrixMultiply(&m_mtxWorld,	&m_mtxWorld, &m_mtxWorldCenter);
	}
	else
	{// 親がNULLじゃないとき
		// 2つの行列の積
		D3DXMatrixMultiply(&m_mtxWorld,	&m_mtxWorld, &m_pParent->m_mtxWorld);
	}

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// マテリアル情報に対するポインタを取得
	pMat = (D3DXMATERIAL*)m_pBuffMat[m_type]->GetBufferPointer();

	// モデルの描画
	for (int nCnt = 0; nCnt < (int)m_nNumMat[m_type]; nCnt++)
	{
		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCnt].MatD3D);

		// 描画
		m_pMesh[m_type]->DrawSubset(nCnt);
	}

	// マテリアルをデフォルトに戻す
	pDevice->SetMaterial(&matDef);

	// ポリゴンの描画
	pDevice->SetTexture(0, NULL);
}

//==================================================================================================================
// 生成処理
//==================================================================================================================
CMotionModel *CMotionModel::CreateObject(CHARA_MODEL type)
{
	// シーン動的に確保
	m_pMotionModel[type] = new CMotionModel();

	// モデルがあるとき
	if (m_pMotionModel[type] != NULL)
	{
		m_pMotionModel[type]->Init();				// モデルの初期化
		m_pMotionModel[type]->m_type = type;		// モデルタイプ設定
	}

	// 値を返す
	return m_pMotionModel[type];
}

//==================================================================================================================
// モデル情報取得
//==================================================================================================================
CMotionModel * CMotionModel::GetModel(CHARA_MODEL type)
{
	return m_pMotionModel[type];
}

//==================================================================================================================
// モデルのロード
//==================================================================================================================
HRESULT CMotionModel::Load(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();				// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();			// デバイスの取得

	// モデルの最大数までカウント
	for (int nCnt = 0; nCnt < MODEL_MAX; nCnt++)
	{
		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice, "", &m_pTex[nCnt]);

		// Xファイルの読み込み
		D3DXLoadMeshFromX(
			m_apFileName[nCnt],
			D3DXMESH_SYSTEMMEM,
			pDevice,
			NULL,
			&m_pBuffMat[nCnt],
			NULL,
			&m_nNumMat[nCnt],
			&m_pMesh[nCnt]);
	}

	// 値を返す
	return S_OK;
}

//==================================================================================================================
// テクスチャとモデル破棄
//==================================================================================================================
void CMotionModel::Unload(void)
{
	for (int nCnt = 0; nCnt < MODEL_MAX; nCnt++)
	{
		// テクスチャがあるとき
		if (m_pTex[nCnt] != NULL)
		{
			m_pTex[nCnt]->Release();					// テクスチャ解放
			m_pTex[nCnt] = NULL;						// NULLにする
		}

		// メッシュがあるとき
		if (m_pMesh[nCnt] != NULL)
		{
			m_pMesh[nCnt]->Release();					// メッシュの開放
			m_pMesh[nCnt] = NULL;						// NULLにする
		}

		// バッファがあるとき
		if (m_pBuffMat[nCnt] != NULL)
		{
			m_pBuffMat[nCnt]->Release();				// マテリアルの開放
			m_pBuffMat[nCnt] = NULL;					// NULLにする
		}
	}
}

//==================================================================================================================
// 位置取得
//==================================================================================================================
D3DXVECTOR3 CMotionModel::GetPos(void)
{
	return m_pos;
}

//==================================================================================================================
// 回転取得
//==================================================================================================================
D3DXVECTOR3 CMotionModel::GetRot(void)
{
	return m_rot;
}

//==================================================================================================================
// 大きさ取得
//==================================================================================================================
D3DXVECTOR3 CMotionModel::GetSize(void)
{
	return m_size;
}

//==================================================================================================================
// 移動量取得
//==================================================================================================================
D3DXVECTOR3 CMotionModel::GetMove(void)
{
	return m_move;
}

//==================================================================================================================
// 位置設定
//==================================================================================================================
void CMotionModel::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//==================================================================================================================
// 回転設定
//==================================================================================================================
void CMotionModel::SetRot(D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//==================================================================================================================
// 大きさ設定
//==================================================================================================================
void CMotionModel::SetSize(D3DXVECTOR3 size)
{
	m_size = size;
}

//==================================================================================================================
// 移動量設定
//==================================================================================================================
void CMotionModel::SetMove(D3DXVECTOR3 move)
{
	m_move = move;
}

//==================================================================================================================
// 親のモデル設定
//==================================================================================================================
void CMotionModel::SetParent(CMotionModel * pMotionModel)
{
	m_pParent = pMotionModel;
}

//==================================================================================================================
// 中心のマトリックス設定
//==================================================================================================================
void CMotionModel::SetMtxCenter(D3DXMATRIX mtxWorld)
{
	m_mtxWorldCenter = mtxWorld;
}
