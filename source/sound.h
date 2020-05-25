//==================================================================================================================
//
// 音処理 [sound.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"

//==================================================================================================================
//
// クラスの定義
//
//==================================================================================================================
class CSound
{
public:
	// ==========================================================
	// 音の種類
	// ==========================================================
	typedef enum
	{
		SOUND_LABEL_BGMTITLE = 0,	// タイトル
		SOUND_LABEL_BGMTUTORIAL,	// チュートリアル
		SOUND_LABEL_BGMGAME,		// ゲーム
		SOUND_LABEL_BGMRESULT,		// リザルト
		SOUND_LABEL_SE_DECISION,	// 決定
		SOUND_LABEL_MAX,			// 最大
	} SOUND_LABEL;					// 音の種類

	CSound();									// コンストラクタ
	~CSound();									// デストラクタ
	HRESULT Init(HWND nWnd);					// 初期化処理
	void Uninit(void);							// 終了処理
	void StopSound(SOUND_LABEL label);			// 指定した音を止める処理
	void StopSound(void);						// 全ての音を止める処理

	HRESULT PlaySound(SOUND_LABEL label);															// 音を鳴らす処理
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);	// チャンクチェック処理
	HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);	// チャンクデータロード処理

protected:

private:
	IXAudio2 *m_pXAudio2;											// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_pMasteringVoice;						// マスターボイス
	IXAudio2SourceVoice *m_apSourceVoice[SOUND_LABEL_MAX];			// ソースボイス
	BYTE *m_apDataAudio[SOUND_LABEL_MAX];							// オーディオデータ
	DWORD m_aSizeAudio[SOUND_LABEL_MAX];							// オーディオデータサイズ
};
#endif
