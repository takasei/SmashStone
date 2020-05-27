//==================================================================================================================
//
// カメラ[camera.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _CAMERA_H_
#define _CAMERA_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"

//==================================================================================================================
// 前方宣言
//==================================================================================================================
class CCamera;

//==================================================================================================================
//
// カメラクラスの定義
//
//==================================================================================================================
class CCamera
{
public:
	CCamera();							// コンストラクタ
	~CCamera();							// デストラクタ
	void Init();						// 初期化処理
	void Uninit(void);					// 終了処理
	void Update(void);					// 更新処理
	void Draw(void);					// 描画処理

	static CCamera *Create(void);		// 生成処理

	void SetCamera(void);				// カメラの設定処理
	void SetRot(D3DXVECTOR3 rot);		// 回転設定処理
	void SetPosV(D3DXVECTOR3 posV);		// 視点位置設定処理

	D3DXVECTOR3 GetRot(void);			// 回転取得処理
	D3DXVECTOR3 GetPosV(void);			// 視点位置設定処理

protected:

private:
	static CCamera *m_pCamera;			// カメラ情報

	D3DXMATRIX m_mtxProjection;			// プロジェクションマトリックス
	D3DXMATRIX m_mtxView;				// ビューマトリックス

	D3DXVECTOR3 m_posV;					// 視点
	D3DXVECTOR3 m_posVDest;				// 視点の目標地点
	D3DXVECTOR3 m_posR;					// 注視点(参照点)
	D3DXVECTOR3 m_posRDest;				// 注視点(参照点)の目標地点
	D3DXVECTOR3 m_posU;					// 上方向ベクトル
	D3DXVECTOR3 m_rot;					// 回転
	D3DXVECTOR3 m_rotDest;				// 回転の目標地点

	int nCntRot;						// 回転を始めるカウンタ
	int m_nCntTitleFade;				// タイトルフェードのカウンタ

	float m_fDistance;					// 距離

	bool m_bCameraMode;					// カメラモードかどうか
};
#endif