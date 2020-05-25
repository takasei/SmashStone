//==================================================================================================================
//
// ゲームパッド処理 [inputGamepad.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================

//==================================================================================================================
//インクルード
//==================================================================================================================
#include "inputGamepad.h"

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
int CInputGamepad::m_nCntPad = 0;
LPDIRECTINPUTDEVICE8 CInputGamepad::m_apDIDevGamepad[NUM_JOYPAD_MAX] = {};

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CInputGamepad::CInputGamepad()
{
	m_nCntPad = 0;

	//DEffectの開放
	m_pDIEffect = NULL;

	// ジョイパッドの終了処理
	for (int nCntPad = 0; nCntPad < NUM_JOYPAD_MAX; nCntPad++)
	{
		m_apDIDevGamepad[nCntPad] = NULL;
	}
}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CInputGamepad::~CInputGamepad()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
HRESULT CInputGamepad::Init(HINSTANCE hInstance, HWND hWnd)
{
	HRESULT hr;

	if (FAILED(CInput::Init(hInstance, hWnd)))
	{
		MessageBox(hWnd, "InputInit失敗", "警告", MB_ICONWARNING);
		return E_FAIL;
	}


	for (m_nCntPad = 0; m_nCntPad < NUM_JOYPAD_MAX; m_nCntPad++)
	{
		//ジョイパッドを探す
		hr = m_pInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoyCallbackGamepad, NULL, DIEDFL_FORCEFEEDBACK | DIEDFL_ATTACHEDONLY);
		if (FAILED(hr) || m_apDIDevGamepad[m_nCntPad] == NULL)
		{
			//MessageBox(hWnd, "ジョイパッドがありません", "警告", MB_ICONWARNING);
			return hr;
		}

		// データフォーマットを設定
		hr = m_apDIDevGamepad[m_nCntPad]->SetDataFormat(&c_dfDIJoystick);
		if (FAILED(hr))
		{

			MessageBox(hWnd, "ジョイパッドのデータフォーマットを設定できませんでした。", "警告", MB_ICONWARNING);
			return hr;
		}

		// 協調モードを設定（フォアグラウンド＆排他モード）
		hr = m_apDIDevGamepad[m_nCntPad]->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "ジョイパッドの協調モードを設定できませんでした。", "警告", MB_ICONWARNING);
			return hr;
		}

		hr = m_apDIDevGamepad[m_nCntPad]->EnumObjects(EnumAxesCallbackGamepad, (VOID*)&m_dwNumForceFeedbackAxis, DIDFT_AXIS);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "ジョイパッドが見つかりませんでした", "警告", MB_ICONWARNING);
			return hr;
		}

		if (m_dwNumForceFeedbackAxis > 2)
		{
			m_dwNumForceFeedbackAxis = 2;
		}

		//エフェクトの生成
		hr = CreateEffect(hWnd);
		if (FAILED(hr))
		{
			MessageBox(hWnd, "エフェクトが生成できませんでした", "警告", MB_ICONWARNING);
			return hr;
		}
	}
	return TRUE;
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CInputGamepad::Uninit(void)
{
	//DEffectの開放
	if (m_pDIEffect != NULL)
	{
		m_pDIEffect->Release();
		m_pDIEffect = NULL;
	}

	// ジョイパッドの終了処理
	for (int nCntPad = 0; nCntPad < NUM_JOYPAD_MAX; nCntPad++)
	{
		if (m_apDIDevGamepad[nCntPad] != NULL)
		{// デバイスオブジェクトの開放
			m_apDIDevGamepad[nCntPad]->Release();
			m_apDIDevGamepad[nCntPad] = NULL;
		}
	}
	CInput::Uninit();
}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CInputGamepad::Update(void)
{
	HRESULT hr;
	bool aKeyStateOld[JOYPADKEY_MAX];

	// パッドの最大数までカウント
	for (int nCntPad = 0; nCntPad < NUM_JOYPAD_MAX; nCntPad++)
	{
		if (m_apDIDevGamepad[nCntPad] == NULL)
		{
			return;
		}
		// 前回のデータを保存
		for (int nCntKey = 0; nCntKey < JOYPADKEY_MAX; nCntKey++)
		{
			aKeyStateOld[nCntKey] = m_aKeyStateGamepad[nCntPad][nCntKey];
		}

		// デバイスからデータを取得
		hr = m_apDIDevGamepad[nCntPad]->GetDeviceState(sizeof(m_aGamepadState[nCntPad]), &m_aGamepadState[nCntPad]);
		if (SUCCEEDED(hr))
		{
			// キー情報設定
			SetKeyStateGamepad(nCntPad);

			// キーの最大数までカウント
			for (int nCntKey = 0; nCntKey < JOYPADKEY_MAX; nCntKey++)
			{
				// トリガー・リリース情報の作成
				m_aKeyStateTriggerGamepad[nCntPad][nCntKey] = (aKeyStateOld[nCntKey] ^ m_aKeyStateGamepad[nCntPad][nCntKey]) & m_aKeyStateGamepad[nCntPad][nCntKey];
				m_aKeyStateReleaseGamepad[nCntPad][nCntKey] = (aKeyStateOld[nCntKey] ^ m_aKeyStateGamepad[nCntPad][nCntKey]) & !m_aKeyStateGamepad[nCntPad][nCntKey];
			}
		}
		else
		{
			m_apDIDevGamepad[nCntPad]->Acquire();
		}
	}
}

//==================================================================================================================
// ジョイスティック問い合わせ用コールバック関数
//==================================================================================================================
BOOL CInputGamepad::EnumJoyCallbackGamepad(const DIDEVICEINSTANCE * lpddi, VOID * pvRef)
{
	static GUID pad_discrimination[NUM_JOYPAD_MAX];	// 各デバイスの識別子を格納
	DIDEVCAPS	diDevCaps;				// デバイス情報
	HRESULT		hRes;


	// ジョイスティック用デバイスオブジェクトを作成
	hRes = m_pInput->CreateDevice(lpddi->guidInstance, &m_apDIDevGamepad[m_nCntPad], NULL);
	if (FAILED(hRes))
	{
		return DIENUM_CONTINUE;		// 列挙を続ける
	}

	// ジョイスティックの能力を調べる
	diDevCaps.dwSize = sizeof(DIDEVCAPS);
	hRes = m_apDIDevGamepad[m_nCntPad]->GetCapabilities(&diDevCaps);
	if (FAILED(hRes))
	{
		m_apDIDevGamepad[m_nCntPad]->Release();
		m_apDIDevGamepad[m_nCntPad] = NULL;
		// 列挙を続ける
		return DIENUM_CONTINUE;
	}

	// デバイスの識別子を保存
	pad_discrimination[0] = lpddi->guidInstance;

	// このデバイスを使うので列挙を終了する
	return DIENUM_STOP;
}

//==================================================================================================================
// ジョイスティック問い合わせ用コールバック関数
//==================================================================================================================
BOOL CInputGamepad::EnumAxesCallbackGamepad(const DIDEVICEOBJECTINSTANCE  *pdidoi, VOID * pvRef)
{
	HRESULT hr;

	for (int nCntPad = 0; nCntPad < NUM_JOYPAD_MAX; nCntPad++)
	{
		DIPROPRANGE diprg;

		// スティック軸の値の範囲を設定（-32768～32767）
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwObj = pdidoi->dwType;
		diprg.diph.dwHow = DIPH_BYID;
		diprg.lMin = -32768;
		diprg.lMax = 32767;

		hr = m_apDIDevGamepad[nCntPad]->SetProperty(DIPROP_RANGE, &diprg.diph);
		if (FAILED(hr))
		{
			return DIENUM_STOP;
		}

		//フォースフィードバック
		DWORD *pdwNumForceFeedbackAxis = (DWORD*)pvRef;
		if ((pdidoi->dwFlags & DIDOI_FFACTUATOR) != 0)
		{
			(*pdwNumForceFeedbackAxis)++;
		}
	}

	return DIENUM_CONTINUE;
}

//==================================================================================================================
// エフェクト生成
//==================================================================================================================
BOOL CInputGamepad::CreateEffect(HWND hWnd)
{
	DWORD           rgdwAxes[2] = { DIJOFS_X , DIJOFS_Y };
	LONG            rglDirection[2] = { 1 , 1 };
	DICONSTANTFORCE cf;
	DIEFFECT        eff;
	HRESULT         hr;

	//エフェクト設定
	ZeroMemory(&eff, sizeof(eff));
	eff.dwSize = sizeof(DIEFFECT);
	eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	eff.dwDuration = INFINITE;
	eff.dwSamplePeriod = 0;
	eff.dwGain = DI_FFNOMINALMAX;
	eff.dwTriggerButton = DIEB_NOTRIGGER;
	eff.dwTriggerRepeatInterval = 0;
	eff.cAxes = m_dwNumForceFeedbackAxis;
	eff.rgdwAxes = rgdwAxes;
	eff.rglDirection = rglDirection;
	eff.lpEnvelope = 0;
	eff.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
	eff.lpvTypeSpecificParams = &cf;
	eff.dwStartDelay = 0;

	//エフェクトのクリエイト
	hr = m_apDIDevGamepad[m_nCntPad]->CreateEffect(GUID_ConstantForce, &eff, &m_pDIEffect, NULL);
	if (FAILED(hr)) {
		MessageBox(hWnd, "エフェクトのクリエイトに失敗", "Error", MB_OK);
		return FALSE;
	}

	return TRUE;
}

//==================================================================================================================
// ジョイパッドのキー情報設定
//==================================================================================================================
void CInputGamepad::SetKeyStateGamepad(int nIDPad)
{
	if (m_aGamepadState[nIDPad].rgdwPOV[0] >= 225 * 100 && m_aGamepadState[nIDPad].rgdwPOV[0] <= 315 * 100)
	{// 十字キー[左]が押されている
		m_aKeyStateGamepad[nIDPad][JOYPADKEY_LEFT] = true;
	}
	else
	{
		m_aKeyStateGamepad[nIDPad][JOYPADKEY_LEFT] = false;
	}

	if (m_aGamepadState[nIDPad].rgdwPOV[0] >= 45 * 100 && m_aGamepadState[nIDPad].rgdwPOV[0] <= 135 * 100)
	{// 十字キー[右]が押されている
		m_aKeyStateGamepad[nIDPad][JOYPADKEY_RIGHT] = true;
	}
	else
	{
		m_aKeyStateGamepad[nIDPad][JOYPADKEY_RIGHT] = false;
	}

	if ((m_aGamepadState[nIDPad].rgdwPOV[0] >= 315 * 100 && m_aGamepadState[nIDPad].rgdwPOV[0] <= 360 * 100)
		|| (m_aGamepadState[nIDPad].rgdwPOV[0] >= 0 * 100 && m_aGamepadState[nIDPad].rgdwPOV[0] <= 45 * 100))
	{// 十字キー[上]が押されている
		m_aKeyStateGamepad[nIDPad][JOYPADKEY_UP] = true;
	}
	else
	{
		m_aKeyStateGamepad[nIDPad][JOYPADKEY_UP] = false;
	}

	if (m_aGamepadState[nIDPad].rgdwPOV[0] >= 135 * 100 && m_aGamepadState[nIDPad].rgdwPOV[0] <= 225 * 100)
	{// 十字キー[下]が押されている
		m_aKeyStateGamepad[nIDPad][JOYPADKEY_DOWN] = true;
	}
	else
	{
		m_aKeyStateGamepad[nIDPad][JOYPADKEY_DOWN] = false;
	}

	for (int nKey = JOYPADKEY_X; nKey <= JOYPADKEY_START; nKey++)
	{
		if (m_aGamepadState[nIDPad].rgbButtons[nKey])
		{// ボタンが押されている
			m_aKeyStateGamepad[nIDPad][nKey] = true;
		}
		else
		{
			m_aKeyStateGamepad[nIDPad][nKey] = false;
		}
	}
}

//==================================================================================================================
// キー情報取得（プレス）
//==================================================================================================================
bool CInputGamepad::GetPress(int nIDPad, JOYPADKEY key)
{
	return m_aKeyStateGamepad[nIDPad][key];
}

//==================================================================================================================
// キー情報取得（トリガー）
//==================================================================================================================
bool CInputGamepad::GetTrigger(int nIDPad, JOYPADKEY key)
{
	return m_aKeyStateTriggerGamepad[nIDPad][key];
}

//==================================================================================================================
// キー情報取得（リリース）
//==================================================================================================================
bool CInputGamepad::GetRelease(int nIDPad, JOYPADKEY key)
{
	return m_aKeyStateReleaseGamepad[nIDPad][key];
}

//==================================================================================================================
// キー取得処理（左トリガー）
//==================================================================================================================
int CInputGamepad::GetTriggerLeft(int nIDPad)
{
	return m_aGamepadState[nIDPad].rgbButtons[JOYPADKEY_L2];
}

//==================================================================================================================
// キー情報取得（右トリガー）
//==================================================================================================================
int CInputGamepad::GetTriggerRight(int nIDPad)
{
	return m_aGamepadState[nIDPad].rgbButtons[JOYPADKEY_R2];
}
//==================================================================================================================
// キー情報取得（左スティック）
//==================================================================================================================
void CInputGamepad::GetStickLeft(int nIDPad, float * pValueH, float * pValueV)
{
	*pValueH = (float)m_aGamepadState[nIDPad].lX;
	*pValueV = (float)-m_aGamepadState[nIDPad].lY;
}

//==================================================================================================================
// キー情報取得（右スティック）
//==================================================================================================================
void CInputGamepad::GetStickRight(int nIDPad, float * pValueH, float * pValueV)
{
	*pValueH = (float)m_aGamepadState[nIDPad].lZ;
	*pValueV = (float)-m_aGamepadState[nIDPad].lRz;
}

//==================================================================================================================
// フォースフィードバックスタート
//==================================================================================================================
void CInputGamepad::StartForeFeedback()
{
	//
	cout << "GPAD振動関数に入った\n";

	if (m_pDIEffect != NULL)
	{
		//
		cout << "GPAD振動呼び出しに入った\n";

		HRESULT hr = m_pDIEffect->Start(1, DIES_SOLO);

		if (FAILED(hr))
		{
			cout << "振動失敗\n";
		}
		else
		{
			cout << "振動成功\n";
			cout << hr;
		}
	}
}

//==================================================================================================================
// フォースフィードバックストップ
//==================================================================================================================
void CInputGamepad::StopForeFeedback()
{
	if (m_pDIEffect != NULL)
	{
		m_pDIEffect->Stop();
	}
}
