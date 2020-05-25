//==================================================================================================================
//
// キーボード[inputKeyboard.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _INPUTKEYBOARD_H_
#define _INPUTKEYBOARD_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"
#include "input.h"

//==================================================================================================================
//
// キーボードクラスの定義
//
//==================================================================================================================
class CInputKeyboard : public CInput
{
public:

	CInputKeyboard();								// コンストラクタ
	~CInputKeyboard();								// デストラクタ
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);	// 初期化処理
	void Uninit(void);								// 終了処理
	void Update(void);								// 更新処理

	static bool GetKeyboardPress(int nKey);			// プレス状態取得関数
	static bool GetKeyboardTrigger(int nKey);		// トリガー状態取得関数

protected:

private:
	LPDIRECTINPUTDEVICE8 m_pDevKeyboard;			// 入力デバイス(キーボード)へのポインタ
	static BYTE m_aKeyState[NUM_KEY_MAX];			// キーボードの入力情報ワーク
	static BYTE m_aKeyStateTngger[NUM_KEY_MAX];		// キーボードトリガーの入力情報ワーク
};
#endif