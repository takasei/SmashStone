//==================================================================================================================
//
// モデルクラス[model.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _MOTIONMODEL_H_
#define _MOTIONMODEL_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"
#include "sceneX.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define MODEL_SIZE_X	(100)								// Xサイズ
#define MODEL_SIZE_Y	(100)								// Yサイズ

//==================================================================================================================
// 前方宣言
//==================================================================================================================
class CPlayer;

//==================================================================================================================
//
// アニメーションモデルクラスの定義
//
//==================================================================================================================
class CMotionModel
{
public:

	//=============================================================================
	// オブジェクトの種類
	//=============================================================================
	typedef enum
	{
		MODEL_PENGUIN_BODY = 0,			// プレイヤーの体
		MODEL_PENGUIN_HEAD,				// プレイヤーの頭
		MODEL_PENGUIN_LARM,				// プレイヤーの左腕
		MODEL_PENGUIN_RARM,				// プレイヤーの右腕
		MODEL_PENGUIN_LLEG,				// プレイヤーの左足
		MODEL_PENGUIN_RLEG,				// プレイヤーの右足
		MODEL_MAX						// 最大数
	}CHARA_MODEL;

	CMotionModel();												// コンストラクタ
	~CMotionModel();											// デストラクタ
	void Init(void);											// 初期化処理
	void Uninit(void);											// 終了処理
	void Update(void);											// 更新処理
	void Draw(void);											// 描画処理

	static CMotionModel *CreateObject(CHARA_MODEL type);		// 生成処理
	static CMotionModel *GetModel(CHARA_MODEL type);			// モデル情報取得処理
	static HRESULT Load(void);									// テクスチャとモデルの情報ロード
	static void Unload(void);									// テクスチャとモデルの情報アンロード

	void SetParent(CMotionModel *pMotionModel);						// 親設定処理
	void SetMtxCenter(D3DXMATRIX mtxWorld);						// 中心のマトリックス設定処理
	void SetPos(D3DXVECTOR3 pos);								// 位置設定処理
	void SetRot(D3DXVECTOR3 rot);								// 回転設定処理
	void SetSize(D3DXVECTOR3 size);								// 大きさ設定処理
	void SetMove(D3DXVECTOR3 move);								// 移動量設定処理

	D3DXVECTOR3 GetPos(void);									// 位置取得処理
	D3DXVECTOR3 GetRot(void);									// 回転取得処理
	D3DXVECTOR3 GetSize(void);									// 大きさ取得処理
	D3DXVECTOR3 GetMove(void);									// 移動量取得処理

protected:

private:
	static LPDIRECT3DTEXTURE9 m_pTex[MODEL_MAX];				// テクスチャ情報
	static LPD3DXMESH m_pMesh[MODEL_MAX];						// メッシュ情報へのポインタ
	static LPD3DXBUFFER m_pBuffMat[MODEL_MAX];					// マテリアル情報へのポインタ
	static DWORD m_nNumMat[MODEL_MAX];							// マテリアル情報の数
	static CMotionModel *m_pMotionModel[MODEL_MAX];				// モデルの情報ポインタ
	static CPlayer *m_pPlayer;									// プレイヤー情報ポインタ
	static char *m_apFileName[MODEL_MAX];						// モデルのファイル名

	D3DXMATRIX m_mtxWorld;										// マトリックス
	D3DXMATRIX m_mtxWorldCenter;								// 中心のマトリックス
	CMotionModel *m_pParent;									// 親のポインタ
	CHARA_MODEL m_type;											// モデルの種類

	D3DXVECTOR3 m_pos;											// 位置
	D3DXVECTOR3 m_rot;											// 回転
	D3DXVECTOR3 m_size;											// 大きさ
	D3DXVECTOR3 m_move;											// 移動量

	bool m_bRespawn;											// リスポーンしたかどうか
};
#endif