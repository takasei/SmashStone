//==================================================================================================================
//
// ゲーム処理[game.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _GAME_H
#define _GAME_H

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"

//==================================================================================================================
// 前方宣言
//==================================================================================================================
class CMeshField;
class CCamera;
class CLight;
class CLogo;
class CModel;
class CPause;
class CMeshSphere;
class CCharacter;

//==================================================================================================================
//
// ゲームクラスの定義
//
//==================================================================================================================
class CGame
{
public:
	// ==========================================================
	// ゲームの状態
	// ==========================================================
	typedef enum
	{
		GAMESTATE_NONE = 0,		// 何もしていない状態
		GAMESTATE_NORMAL,		// 通常状態
		GAMESTATE_PAUSE,		// ポーズ状態
		GAMESTATE_START_OVER,	// 初めからやり直す
		GAMESTATE_BREAK,		// ゲーム中断
		GAMESTATE_END,			// ゲームの終了
		GAMESTATE_MAX			// 最大
	} GAMESTATE;				// ゲームの状態

	CGame();									// コンストラクタ
	~CGame();									// デストラクタ
	void Init(void);							// 初期化処理
	void Uninit(void);							// 終了処理
	void Update(void);							// 更新処理
	void Draw(void);							// 描画処理
	static CGame *Create(void);					// 生成処理

	static void SetGameState(GAMESTATE state);	// ゲームの状態設定
	static GAMESTATE GetGameState(void);		// ゲームの状態取得
	static CCharacter *GetCharacter(void);		// キャラクターの情報取得処理

protected:

private:
	static GAMESTATE m_gameState;				// ゲーム状態
	static CCharacter *m_pCharacter;			// キャラクターの情報ポインタ
	static CMeshField *m_pMeshField;			// メッシュフィールドの情報ポインタ
	static CCamera *m_pCamera;					// カメラの情報ポインタ
	static CLight *m_pLight;					// ライトの情報ポインタ
	static CLogo *m_pLogo;						// ロゴの情報ポインタ
	static CPause *m_pPause;					// ポーズの情報ポインタ
	static CMeshSphere *m_pMeshSphere;			// メッシュ球の情報ポインタ
	static int m_nCounterGameState;				// ゲームの状態管理カウンター

	D3DXMATRIX  m_mtxWorld;						// マトリックス
};
#endif
