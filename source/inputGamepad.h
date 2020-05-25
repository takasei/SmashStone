//==================================================================================================================
//
// ゲームパッド処理 [inputGamepad.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _GAMEPAD_H_
#define _GAMEPAD_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"
#include "input.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define NUM_JOYPAD_MAX 1			// パッド最大数
#define JOY_MAX_RANGE 32768.0f		// パッド有効範囲

//==================================================================================================================
// ゲームパッドクラス定義
//==================================================================================================================
class CInputGamepad : public CInput
{
public:
	//=============================================================================
	// 列挙型定義
	//=============================================================================
	// キーの種類
	typedef enum
	{
		JOYPADKEY_X = 0,			// [0] Xボタン
		JOYPADKEY_Y,				// [1] Yボタン
		JOYPADKEY_A,				// [2] Aボタン
		JOYPADKEY_B,				// [3] Bボタン
		JOYPADKEY_L1,				// [4] 左手前ボタン
		JOYPADKEY_R1,				// [5] 右手前ボタン
		JOYPADKEY_L2,				// [6] 左トリガー
		JOYPADKEY_R2,				// [7] 右トリガー
		JOYPADKEY_LEFT_THUMB,		// [8] 左スティックボタン
		JOYPADKEY_RIGHT_THUMB,		// [9] 右スティックボタン
		JOYPADKEY_BACK,				// [10] バックボタン
		JOYPADKEY_START,			// [11] スタートボタン
		JOYPADKEY_LEFT,				// [12] 方向キー[左]
		JOYPADKEY_RIGHT,			// [13] 方向キー[右]
		JOYPADKEY_UP,				// [14] 方向キー[上]
		JOYPADKEY_DOWN,				// [15] 方向キー[下]
		JOYPADKEY_MAX				// 最大数
	} JOYPADKEY;

	CInputGamepad();
	~CInputGamepad();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);	//初期化
	void Uninit(void);								//終了
	void Update(void);								//更新

	bool GetPress(int nIDPad, JOYPADKEY key);							//プレス処理
	bool GetTrigger(int nIDPad, JOYPADKEY key);							//トリガー処理
	bool GetRelease(int nIDPad, JOYPADKEY key);							//リリース処理
	void GetStickLeft(int nIDPad, float *pValueH, float *pValueV);		//左スティック値取得
	void GetStickRight(int nIDPad, float *pValueH, float *pValueV);		//右スティック値取得
	void StartForeFeedback();											//振動開始
	void StopForeFeedback();											//振動停止

private:
	static LPDIRECTINPUTDEVICE8	m_apDIDevGamepad[NUM_JOYPAD_MAX];						// IDirectInputDevice8へのポインタ(ジョイパッド)
	LPDIRECTINPUTEFFECT		m_pDIEffect = NULL;											// 振動用
	DIJOYSTATE				m_aGamepadState[NUM_JOYPAD_MAX];							// ジョイパッド状態を受け取るワーク
	bool					m_aKeyStateGamepad[NUM_JOYPAD_MAX][NUM_KEY_MAX];			// ジョイパッドの押下状態を保持するワーク
	bool					m_aKeyStateTriggerGamepad[NUM_JOYPAD_MAX][NUM_KEY_MAX];		// ジョイパッドのトリガー状態を保持するワーク
	bool					m_aKeyStateReleaseGamepad[NUM_JOYPAD_MAX][NUM_KEY_MAX];		// ジョイパッドのリリース状態を保持するワーク
	D3DXVECTOR3             m_aKeyStateAxis[NUM_JOYPAD_MAX];
	DWORD					m_dwNumForceFeedbackAxis;									//フォースフィードバック（振動）

	static int				m_nCntPad;

	void SetKeyStateGamepad(int nIDPad);		
	int GetTriggerLeft(int nIDPad);				
	int GetTriggerRight(int nIDPad);			
	static BOOL CALLBACK EnumJoyCallbackGamepad(const DIDEVICEINSTANCE* lpddi, VOID* pvRef);
	static BOOL CALLBACK EnumAxesCallbackGamepad(const DIDEVICEOBJECTINSTANCE  *pdidoi, VOID* pvRef);
	BOOL CreateEffect(HWND hWnd);
};

#endif