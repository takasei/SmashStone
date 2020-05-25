//==================================================================================================================
//
// ポーズの描画 [pause.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================
#define _CRT_SECURE_NO_WARNINGS							// 警告回避文

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "pause.h"
#include "renderer.h"
#include "manager.h"
#include "inputKeyboard.h"
#include "inputGamepad.h"
#include "game.h"
#include "fade.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define MAX_SELECT_TEXTURE 3	// 選択できるテクスチャの数
#define SIZE_X 250.0f			// テクスチャのサイズX
#define SIZE_Y 130.0f			// テクスチャのサイズY
#define POS_X 640.0f			// テクスチャのX座標の位置
#define POS_Y 100.0f			// 一番上のテクスチャY座標に位置
#define SPACE_TEXTURE 150.0f	// テクスチャとテクスチャの間隔
#define FREAME_POSY 300.0f		// フレームテクスチャ位置Y
#define FREAME_SIZEX 700.0f		// フレームテクスチャ大きさX
#define FREAME_SIZEY 720.0f		// フレームテクスチャ大きさY
#define SELECT_SIZEX 500.0f		// 選択テクスチャ大きさX
#define SELECT_SIZEY 100.0f		// 選択テクスチャ大きさY
#define SELECT_POSY 200.0f		// 選択テクスチャ位置Y

//==================================================================================================================
// 静的メンバ変数宣言
//==================================================================================================================
LPDIRECT3DTEXTURE9 CPause::m_pTexture[PAUSETEXTURE_MAX] = {};			// テクスチャ情報
char *CPause::m_apFileName[PAUSETEXTURE_MAX] =							// 読み込むモデルのソース先
{
	{ "data/TEXTURE/pause.pn" },		// ポーズ背景
	{ "data/TEXTURE/pause.png" },		// ポーズフレーム
	{ "data/TEXTURE/select.png" },		// 選択
	{ "data/TEXTURE/continue.png" },	// 続ける
	{ "data/TEXTURE/restart.png" },		// スタートからやり直す
	{ "data/TEXTURE/title_go.png" },	// タイトルにいく
};

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CPause::CPause()
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CPause::~CPause()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
void CPause::Init(void)
{
	// 初期化
	m_pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);	// 位置
	m_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);					// 色情報
	m_nSelect = PAUSETEXTURE_CONTINUE;								// 選択番号
	m_bPause = false;												// ポーズ中かどうか

	CRenderer *pRenderer = CManager::GetRenderer();					// レンダラーの情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスを取得する

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * PAUSETEXTURE_MAX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVertexBuff,
		NULL);

	// 背景のテクスチャ設定
	SetVertexPause(PAUSETEXTURE_BG, D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f), SCREEN_WIDTH, SCREEN_HEIGHT);

	// ポーズフレームのテクスチャ設定
	SetVertexPause(PAUSETEXTURE_FRAME, D3DXVECTOR3(SCREEN_WIDTH / 2, FREAME_POSY, 0), m_color, FREAME_SIZEX, FREAME_SIZEY);

	// 選択のテクスチャ設定
	SetVertexPause(PAUSETEXTURE_SELECT, D3DXVECTOR3(POS_X, SELECT_POSY, 0), m_color, SELECT_SIZEX, SELECT_SIZEY);

	// 列挙型のfor文
	for (auto nCnt : { PAUSETEXTURE_CONTINUE, PAUSETEXTURE_START, PAUSETEXTURE_TITLE })
	{
		// テクスチャ設定
		SetVertexPause(nCnt, D3DXVECTOR3(POS_X, POS_Y + SPACE_TEXTURE * (nCnt - 2), 0), m_color, SIZE_X, SIZE_Y);
	}
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CPause::Uninit(void)
{
	// テクスチャ情報廃棄
	Unload();

	// 頂点バッファがあるとき
	if (m_pVertexBuff != NULL)
	{
		m_pVertexBuff->Release();		// 頂点バッファの開放
		m_pVertexBuff = NULL;			// NULLにする
	}
}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CPause::Update(void)
{
	// キーボード取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// コントローラー取得
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	// ゲーム情報取得
	CGame *pGame = CRenderer::GetGame();

	// フェード取得
	CFade::FADE fade = CFade::GetFade();

	// キーボードの[↑]が押されたとき
	if (pInputKeyboard->GetKeyboardTrigger(DIK_UPARROW))
	{// 選択処理
		m_nSelect--;
	}
	else if (pInputKeyboard->GetKeyboardTrigger(DIK_DOWNARROW))
	{// キーボードの[↓]が押されたとき
		m_nSelect++;
	}

	// コントローラーの[十字キー上]が押されたとき
	if (pInputGamepad->GetTrigger(0, CInputGamepad::JOYPADKEY_UP))
	{//選択処理
		m_nSelect--;
	}
	else if (pInputGamepad->GetTrigger(0, CInputGamepad::JOYPADKEY_DOWN))
	{// コントローラーの[十字キー下]が押されたとき
		m_nSelect++;
	}

	// 選択番号が表示するテクスチャ番号を超えたら
	if (m_nSelect == PAUSETEXTURE_MAX)
	{// 選択肢の繰り返し処理
		// 0の戻す
		m_nSelect = PAUSETEXTURE_CONTINUE;
	}
	else if (m_nSelect < PAUSETEXTURE_CONTINUE)
	{// 選択番号がPAUSETEXTURE_CONTINUEより小さくなったら
		// 選択番号を最大にする
		m_nSelect = PAUSETEXTURE_TITLE;
	}

	// 選択時の表示処理
	if (m_nSelect == PAUSETEXTURE_CONTINUE)
	{// 選択番号が0のとき
		// 選択テクスチャの設定
		SetVertexPause(PAUSETEXTURE_SELECT, D3DXVECTOR3(POS_X, 250.0f, 0), m_color, SELECT_SIZEX, SIZE_Y);
	}
	else if (m_nSelect == PAUSETEXTURE_START)
	{// 選択番号が1のとき
		SetVertexPause(PAUSETEXTURE_SELECT, D3DXVECTOR3(POS_X, 400.0f, 0), m_color, SELECT_SIZEX, SIZE_Y);
	}
	else if (m_nSelect == PAUSETEXTURE_TITLE)
	{// 選択番号が2のとき
		SetVertexPause(PAUSETEXTURE_SELECT, D3DXVECTOR3(POS_X, 550.0f, 0), m_color, SELECT_SIZEX, SIZE_Y);
	}

	// キーボードの[エンター] 又は コントローラーの[B]が押されたとき
	if (pInputKeyboard->GetKeyboardPress(DIK_RETURN) || pInputGamepad->GetPress(0, CInputGamepad::JOYPADKEY_B))
	{
		// 選択番号がスタートからやり直す番号のとき
		if (m_nSelect == PAUSETEXTURE_START)
		{
			// ゲームの状態をスタートからやり直すにする
			pGame->SetGameState(CGame::GAMESTATE_START_OVER);
		}
		else if (m_nSelect == PAUSETEXTURE_TITLE)
		{// 選択番号がタイトルへいく番号のとき
			// ゲーム状態を中断にする
			pGame->SetGameState(CGame::GAMESTATE_BREAK);
		}
		else
		{
			// ポーズ状態false
			m_bPause = false;

			// ゲームの状態をNORMALにする
			pGame->SetGameState(CGame::GAMESTATE_NORMAL);
		}
	}
}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CPause::Draw(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();					// レンダラー情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();				// デバイスを取得する

	// 頂点バッファをデバイスのデータストリームにバインド
	pDevice->SetStreamSource(0, m_pVertexBuff, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// ポーズ状態のとき
	if (m_bPause == true)
	{
		// テクスチャの最大数までカウント
		for (int nCnt = 0; nCnt < PAUSETEXTURE_MAX; nCnt++)
		{
			// テクスチャの設定
			pDevice->SetTexture(0, m_pTexture[nCnt]);

			// ポリゴン描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCnt * 4, 2);

			// テクスチャの設定
			pDevice->SetTexture(0, NULL);
		}
	}
}

//==================================================================================================================
// オブジェクトの生成
//==================================================================================================================
CPause * CPause::Create(void)
{
	CPause *pPause = NULL;		// ポーズ変数をNULLにする

	pPause = new CPause;		// 動的に確保
	pPause->Init();				// 初期化処理

	return pPause;				// 値を返す
}

//==================================================================================================================
// テクスチャ情報ロード
//==================================================================================================================
HRESULT CPause::Load(void)
{
	CRenderer *pRenderer = CManager::GetRenderer();			// レンダラーの情報取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();		// デバイスを取得する

	//==============================テクスチャの読み込み==============================//
	// テクスチャの最大数までカウント
	for (int nCnt = 0; nCnt < PAUSETEXTURE_MAX; nCnt++)
	{
		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice, m_apFileName[nCnt], &m_pTexture[nCnt]);
	}

	return S_OK;		// 値を返す
}

//==================================================================================================================
// テクスチャ情報破棄
//==================================================================================================================
void CPause::Unload(void)
{
	// テクスチャの最大数までカウント
	for (int nCnt = 0; nCnt < PAUSETEXTURE_MAX; nCnt++)
	{
		// テクスチャがあるとき
		if (m_pTexture[nCnt] != NULL)
		{
			m_pTexture[nCnt]->Release();	// テクスチャの開放
			m_pTexture[nCnt] = NULL;		// NULLにする
		}
	}
}

//==================================================================================================================
// ポーズ状態設定処理
//==================================================================================================================
void CPause::SetPause(bool bPause)
{
	m_bPause = bPause;
}

//==================================================================================================================
// ポーズ状態取得処理
//==================================================================================================================
bool CPause::GetPause(void)
{
	return m_bPause;
}

//==================================================================================================================
// ポーズテクスチャ設定処理
//==================================================================================================================
void CPause::SetVertexPause(PAUSETEXTURE number, D3DXVECTOR3 pos, D3DXCOLOR col, float fWidth, float fHeight)
{
	VERTEX_2D *pVtx;					// 頂点情報へのポインタ

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	m_pVertexBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx += number * 4;

	// 頂点情報の設定
	// 頂点座標の設定(基準のx座標 + 間隔 * nCntScore (+ 幅), 基準のy座標)
	pVtx[0].pos = D3DXVECTOR3(pos.x - fWidth / 2, pos.y - fHeight / 2, 0);
	pVtx[1].pos = D3DXVECTOR3(pos.x + fWidth / 2, pos.y - fHeight / 2, 0);
	pVtx[2].pos = D3DXVECTOR3(pos.x - fWidth / 2, pos.y + fHeight / 2, 0);
	pVtx[3].pos = D3DXVECTOR3(pos.x + fWidth / 2, pos.y + fHeight / 2, 0);

	// 1.0で固定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// カラーチャートの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// テクスチャ描写の位置
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点データのアンロック
	m_pVertexBuff->Unlock();
}