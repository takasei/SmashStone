//==================================================================================================================
//
// キャラクター[character.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _CHARACTER_H_
#define _CHARACTER_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"
#include "sceneX.h"
#include "fade.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define MAX_CHARA 2							// キャラクターの最大数

//==================================================================================================================
// 前方宣言
//==================================================================================================================
class CCamera;
class CMotionModel;
class CMeshField;
class CInputKeyboard;
class CInputGamepad;
class CPlayer;

//==================================================================================================================
//
// プレイヤークラスの定義
//
//==================================================================================================================
class CCharacter : public CScene
{
public:

	// キャラクターの種類
	typedef enum
	{
		CHARACTER_PENGUIN = 0,						// ペンギン
		CHARACTER_MAX								// 最大
	}CHARACTER;										// キャラクター変数名

	CCharacter();									// コンストラクタ
	CCharacter(PRIORITY type);						// コンストラクタ
	~CCharacter();									// デストラクタ
	void Init(void);								// 初期化処理
	void Uninit(void);								// 終了処理
	void Update(void);								// 更新処理
	void Draw(void);								// 描画処理

	static CCharacter *Create(CHARACTER type);		// 生成処理

	void SetPos(D3DXVECTOR3 pos);					// 位置設定処理
	void SetRot(D3DXVECTOR3 rot);					// 回転設定処理
	void SetMtx(D3DMATRIX mtxWorld);				// ワールドマトリックス設定処理

	D3DXVECTOR3 GetPos(void) { return m_pos; }		// 位置取得処理
	D3DXVECTOR3 GetRot(void) { return m_rot; }		// 回転取得処理
	D3DXVECTOR3 GetMove(void) { return m_move; }	// 移動量取得処理

protected:

private:

	void OperationMove(CInputKeyboard *pInputKeyboard,
		CInputGamepad *pInputGamepad);				// 操作処理

	D3DXMATRIX  m_mtxWorld;							// マトリックス
	CHARACTER m_Type;								// キャラクターの種類

	static CCharacter *m_pCharacter[MAX_CHARA][CHARACTER_MAX];	// キャラクターの情報ポインタ
	static CCamera *m_pCamera;									// カメラの情報ポインタ
	static CPlayer *m_pPenguin;									// ペンギンの情報ポインタ
	static int m_nNumAll;										// プレイヤーの人数

	D3DXVECTOR3 m_pos;			// 位置
	D3DXVECTOR3 m_posOld;		// 前回の位置
	D3DXVECTOR3 m_rot;			// 回転
	D3DXVECTOR3 m_move;			// 移動
	D3DXVECTOR3 m_size;			// 大きさ
	D3DXVECTOR3 m_difference;	// 回転の目標地点

	int m_nCntGravity;			// 重力用カウント

	bool m_bJump;				// ジャンプしたかどうか
	bool m_bWalk;				// 歩いてるかどうか
};
#endif