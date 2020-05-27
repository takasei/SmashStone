//==================================================================================================================
//
// プレイヤー[player.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"
#include "sceneX.h"
#include "fade.h"
#include "character.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define TEXTURE_PLAYER "data/TEXTURE/field000.jpg"		// 読み込むテクスチャのソース先
#define PLAYER_MODEL "data/MODEL/testmodel.x"			// 読み込むモデルのソース先
#define PLAYER_MAX_MODEL (6)							// player.cpp内のモデルのパーツ数

//==================================================================================================================
// 前方宣言
//==================================================================================================================
class CCamera;
class CMotionModel;
class CMeshField;
class CInputKeyboard;
class CInputGamepad;
class CFade;

//==================================================================================================================
//
// プレイヤークラスの定義
//
//==================================================================================================================
class CPlayer : public CCharacter
{
public:
	//=============================================================================
	// 構造体定義
	//=============================================================================
	// キー要素
	typedef struct
	{
		D3DXVECTOR3 pos;				// キー要素の位置
		D3DXVECTOR3 rot;				// キー要素の回転
	}KEY;

	// キー情報
	typedef struct
	{
		KEY key[PLAYER_MAX_MODEL];		// パーツ数
		int nFram;						// フレーム
	}KEY_INFO;

	// プレイヤーのモーション
	typedef struct
	{
		KEY_INFO key_info[10];				// キーが何セットあるか
		D3DXVECTOR3 CollisionOfset;			// オフセット
		int nLoop;							// ループ
		int nNumKey;						// キー情報の数
		int Collision_nParts;				// パーツ番号
		int CollisionStartFram;				// 開始フレーム
		int CollisionEndFram;				// 終了フレーム
		int Collision_Damage;				// ダメージ数
		float CollisionRadius;				// 円の半径
	}MOTION_PLAYER;

	// モーションのタイプ
	typedef enum
	{
		PLAYER_MOTION_NORMAL = 0,			// ニュートラル
		PLAYER_MOTION_WALK,					// 歩き
		PLAYER_MOTION_ACTION,				// アクション
		PLAYER_MOTION_JUMP,					// ジャンプ
		PLAYER_MOTION_LANDING,				// 着地
		PLAYER_MOTION_DAMAGE,				// ダメージ
		PLAYER_MOTION_MAX					// モーションの最大数
	}PLAYER_MOTION_STATE;

	CPlayer();										// コンストラクタ
	~CPlayer();										// デストラクタ
	void Init(void);								// 初期化処理
	void Uninit(void);								// 終了処理
	void Update(void);								// 更新処理
	void Draw(void);								// 描画処理

	static CPlayer *Create(void);					// 生成処理

	void SetPos(D3DXVECTOR3 pos);					// 位置設定処理
	void SetRot(D3DXVECTOR3 rot);					// 回転設定処理
	void SetSize(D3DXVECTOR3 size);					// 大きさ設定処理

	D3DXVECTOR3 GetPos(void) { return m_pos; }		// 位置取得処理
	D3DXVECTOR3 GetRot(void) { return m_rot; }		// 回転取得処理
	D3DXVECTOR3 GetMove(void) { return m_move; }	// 移動量取得処理
	D3DXVECTOR3 GetSize(void) { return m_size; }	// 大きさ取得処理

protected:

private:
	void LoadMotion(void);							// モーション読み込み
	void Moation(void);								// モーションさせる

	CMotionModel *m_MotionModel[PLAYER_MAX_MODEL];	// モデル情報
	MOTION_PLAYER m_PlayerMotion[PLAYER_MOTION_MAX];// モーション情報
	D3DXMATRIX  m_mtxWorld;							// マトリックス

	static CCamera *m_pCamera;				// カメラの情報ポインタ

	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_posOld;					// 前回の位置
	D3DXVECTOR3 m_rot;						// 回転
	D3DXVECTOR3 m_move;						// 移動
	D3DXVECTOR3 m_size;						// 大きさ
	D3DXVECTOR3 m_difference;				// 回転の目標地点
	D3DXVECTOR3 posBET[PLAYER_MAX_MODEL];	// 各パーツの位置
	D3DXVECTOR3 rotBET[PLAYER_MAX_MODEL];	// 各パーツの回転

	int m_MotionType;						// モーションの種類
	int m_MotionOld;						// 前のモーション
	int m_Fram;								// フレーム
	int m_nCntKeySet;						// キーセットのカウント
	int m_nCntState;						// ステータスのカウント

	float m_RotModel;						// モデルの回転情報
};
#endif