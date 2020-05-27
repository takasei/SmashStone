//==================================================================================================================
//
// キャラクター処理[character.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================
#include "inputKeyboard.h"
#include "renderer.h"
#include "character.h"
#include "debugProc.h"
#include "camera.h"
#include "meshField.h"
#include "motionModel.h"
#include "time.h"
#include "title.h"
#include <initializer_list>
#include "tutorial.h"
#include "game.h"
#include "inputGamepad.h"
#include "player.h"

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
CCamera *CCharacter::m_pCamera = NULL;									// カメラ情報
CCharacter *CCharacter::m_pCharacter[MAX_CHARA][CHARACTER_MAX] = {};	// キャラクター情報
CPlayer *CCharacter::m_pPenguin = NULL;									// ペンギン情報
int CCharacter::m_nNumAll = 0;											// プレイヤーの人数

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define PLAYER_MOVE 0.25f			// プレイヤーの移動量
#define JUMP_MOVE 11.0f				// ジャンプするときの移動量
#define MAX_JAMP_SPEED 0.05f		// 最大ジャンプスピード
#define MAX_GRAVITY -6.0f			// 重力の最大値
#define GRAVITY -0.4f				// 重力
#define ROT_LIMIT 1.0f				// 回転制限
#define ROT_SPEED 0.4f				// 回転速度
#define ROT_AMOUNT 0.1f				// 回転の差を減らしていく量
#define REDUCE_SPEED 0.06f			// 減速スピード

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CCharacter::CCharacter() :CScene()
{

}

//==================================================================================================================
// コンストラクタ
//==================================================================================================================
CCharacter::CCharacter(PRIORITY type = CScene::PRIORITY_PLAYER) :CScene(type)
{
	// プレイヤーの人数加算
	m_nNumAll++;
}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CCharacter::~CCharacter()
{
	// プレイヤーの人数初期化
	m_nNumAll = 0;
}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
void CCharacter::Init(void)
{
	m_pos = D3DXVECTOR3(0.0f, WhileY, 0.0f);				// 位置
	m_posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 前回の位置
	m_rot = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);				// 回転
	m_difference = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);		// 回転の目標地点
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);					// 移動量
	m_size = D3DXVECTOR3(1.0f, 1.0f, 1.0f);					// 大きさ
	m_nCntGravity = 0;										// 重力用カウント
	m_bJump = false;										// ジャンプしたかどうか
	m_bWalk = false;										// 歩いているかどうか

	// キャラクターの種類がペンギンンとき
	if (m_Type == CHARACTER_PENGUIN)
	{
		m_pPenguin = CPlayer::Create();
	}
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CCharacter::Uninit(void)
{
	// キャラクターの種類がペンギンンとき
	if (m_Type == CHARACTER_PENGUIN)
	{
		// ペンギンの終了処理
		m_pPenguin->Uninit();
	}
}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CCharacter::Update(void)
{
	// キーボード取得
	CInputKeyboard *pInputKeyboard = CManager::GetInputKeyboard();

	// コントローラー取得
	CInputGamepad *pInputGamepad = CManager::GetInputGamepad();

	m_pos = GetPos();					// 位置取得
	m_rot = GetRot();					// 回転取得
	m_move = GetMove();					// 移動量取得
	m_posOld = m_pos;					// 今の位置を前回の位置にする

	// ジャンプスピードが規定値を超えたら
	if (m_move.y > MAX_JAMP_SPEED)
	{
		// ジャンプスピードを規定値にする
		m_move.y += MAX_JAMP_SPEED;
	}

	// ジャンプしていないとき
	if (m_bJump == false)
	{
		// キーボードの[SPACE] 又は コントローラーのBが押されたとき
		if (pInputKeyboard->GetKeyboardTrigger(DIK_SPACE) || pInputGamepad->GetTrigger(0, CInputGamepad::JOYPADKEY_B))
		{
			// 上に移動量を規定値増やす
			m_move.y += JUMP_MOVE;

			//// モーションさせる
			//m_MotionType = PLAYER_MOTION_JUMP;

			// ジャンプした状態にする
			m_bJump = true;
		}
	}

	// 位置更新
	m_pos.x += m_move.x;
	m_pos.y += m_move.y;
	m_pos.z += m_move.z;

	// 減速
	m_move.x += (0 - m_move.x) * REDUCE_SPEED;
	m_move.z += (0 - m_move.z) * REDUCE_SPEED;

	// プレイヤーの位置が0以下になったとき
	if (m_pos.y <= 0)
	{
		// プレイヤーの位置を0にする
		m_pos.y = 0;

		// ジャンプしていない状態にする
		m_bJump = false;
	}

	// 重力処理
	m_move.y += GRAVITY;

	// 落下スピードが規定値を超えたら
	if (m_move.y < MAX_GRAVITY)
	{
		// 落下スピードを規定値にする
		m_move.y = MAX_GRAVITY;
	}

	// チュートリアル 又は ゲームのとき
	if (CRenderer::GetMode() == CRenderer::MODE_TUTORIAL || CRenderer::GetMode() == CRenderer::MODE_GAME)
	{
		// 移動処理
		OperationMove(pInputKeyboard, pInputGamepad);
	}

	SetPos(m_pos);			// 位置設定
	SetRot(m_rot);			// 回転設定
	
#ifdef _DEBUG
	// デバッグ表示
	CDebugProc::Print("プレイヤーの位置 : %.2f, %.2f, %.2f\n", m_pos.x, m_pos.y, m_pos.z);
	CDebugProc::Print("プレイヤーの移動量 : %.2f, %.2f, %.2f\n", m_move.x, m_move.y, m_move.z);
	CDebugProc::Print("プレイヤーの回転 : %.2f, %.2f, %.2f\n", m_rot.x, m_rot.y, m_rot.z);
#endif // _DEBUG
}

//==================================================================================================================
// 描画処理
//==================================================================================================================
void CCharacter::Draw(void)
{
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;			// 計算用格納変数

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScale, m_size.x, m_size.y, m_size.z);
	// 2つの行列の積
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxScale);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	// 2つの行列の積
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	// 2つの行列の積
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// キャラクターの種類がペンギンだったとき
	if (m_Type == CHARACTER_PENGUIN)
	{
		// ペンギンの描画処理
		m_pPenguin->Draw();
	}
}

//==================================================================================================================
// 生成処理
//==================================================================================================================
CCharacter *CCharacter::Create(CCharacter::CHARACTER type)
{
	// シーン動的に確保
	m_pCharacter[m_nNumAll][type] = new CCharacter(CScene::PRIORITY_PLAYER);

	m_pCharacter[m_nNumAll][type]->m_Type = type;	// キャラクターの種類
	m_pCharacter[m_nNumAll][type]->Init();			// キャラクターの初期化

	// 値を返す
	return m_pCharacter[m_nNumAll][type];
}

//==================================================================================================================
// 位置設定
//==================================================================================================================
void CCharacter::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}

//==================================================================================================================
// 回転設定
//==================================================================================================================
void CCharacter::SetRot(D3DXVECTOR3 rot)
{
	m_rot = rot;
}

//==================================================================================================================
// ワールドマトリックス設定
//==================================================================================================================
void CCharacter::SetMtx(D3DMATRIX mtxWorld)
{
	m_mtxWorld = mtxWorld;
}

//==================================================================================================================
// 操作処理
//==================================================================================================================
void CCharacter::OperationMove(CInputKeyboard *pInputKeyboard, CInputGamepad *pInputGamepad)
{
	D3DXVECTOR3 Diff;	// 計算用格納変数

	// コントローラー
	float fValueH = 0.0f;		// 縦
	float fValueV = 0.0f;		// 横
	float fMove = 0.0f;			// コントローラースティック移動量

	// 移動処理 左スティック
	pInputGamepad->GetStickLeft(0, &fValueV, &fValueH);

	// キーボードの移動キーが押されていないとき
	if (!pInputKeyboard->GetKeyboardPress(DIK_D) && !pInputKeyboard->GetKeyboardPress(DIK_A))
	{
		// 横スティック 又は 縦スティックが0じゃないとき
		if (fValueH != 0 || fValueV != 0)
		{
			// 左にスティックが倒れたとき
			if (fValueV >= -JOY_MAX_RANGE && fValueV < 0)
			{
				// 移動量計算
				fMove = fValueV * -PLAYER_MOVE / JOY_MAX_RANGE;

				// スティックの傾き度で移動させる
				m_move.x += sinf(D3DX_PI + m_rot.y) * PLAYER_MOVE;
				m_move.z += cosf(D3DX_PI + m_rot.y) * PLAYER_MOVE;

				// 回転の目標地点を決める
				m_difference.z = -ROT_LIMIT;

				// 回転の目標地点を代入
				m_difference.y = m_rot.y - ROT_SPEED;

				//// モーションさせる
				//m_MotionType = PLAYER_MOTION_WALK;

				// 歩く状態にする
				m_bWalk = true;
			}
			else if (fValueV <= JOY_MAX_RANGE && fValueV > 0)
			{// 右にスティックが倒れたとき
			 // 移動量計算
				fMove = fValueV * PLAYER_MOVE / JOY_MAX_RANGE;

				// スティックの傾き度で移動させる
				m_move.x += sinf(D3DX_PI + m_rot.y) * PLAYER_MOVE;
				m_move.z += cosf(D3DX_PI + m_rot.y) * PLAYER_MOVE;

				// 回転の目標地点を決める
				m_difference.z = ROT_LIMIT;

				// 回転の目標地点を代入
				m_difference.y = m_rot.y + ROT_SPEED;

				//// モーションさせる
				//m_MotionType = PLAYER_MOTION_WALK;

				// 歩く状態にする
				m_bWalk = true;
			}
		}
	}

	// コントローラーの左スティックが倒されていないとき
	if (fValueV == 0)
	{
		// キーボードの[A]が押されたとき
		if (pInputKeyboard->GetKeyboardPress(DIK_A))
		{
			// キーボードの[W]が押されたとき
			if (pInputKeyboard->GetKeyboardPress(DIK_W))
			{// 左上
				m_move.x += sinf(-D3DX_PI * 0.25f) * PLAYER_MOVE;
				m_move.z += cosf(-D3DX_PI * 0.25f) * PLAYER_MOVE;

				// 回転の目標地点を代入
				m_difference.y = D3DX_PI * 0.75f;
			}
			else if (pInputKeyboard->GetKeyboardPress(DIK_S))
			{// キーボードの[S]が押されたとき
			 // 左下
				m_move.x += sinf(-D3DX_PI * 0.75f) * PLAYER_MOVE;
				m_move.z += cosf(-D3DX_PI * 0.75f) * PLAYER_MOVE;

				// 回転の目標地点を代入
				m_difference.y = D3DX_PI * 0.25f;
			}
			else
			{// 左
				m_move.x += sinf(-D3DX_PI * 0.5f) * PLAYER_MOVE;
				m_move.z += cosf(-D3DX_PI * 0.5f) * PLAYER_MOVE;

				// 回転の目標地点を代入
				m_difference.y = D3DX_PI * 0.5f;
			}

			//// モーションさせる
			//m_MotionType = PLAYER_MOTION_WALK;

			// 歩く状態にする
			m_bWalk = true;
		}
		else if (pInputKeyboard->GetKeyboardPress(DIK_D))
		{// キーボードの[D]が押されたとき
		 // キーボードの[W]が押されたとき
			if (pInputKeyboard->GetKeyboardPress(DIK_W))
			{// 右上
				m_move.x += sinf(D3DX_PI * 0.25f) * PLAYER_MOVE;
				m_move.z += cosf(D3DX_PI * 0.25f) * PLAYER_MOVE;

				// 回転の目標地点を代入
				m_difference.y = -D3DX_PI * 0.75f;
			}
			else if (pInputKeyboard->GetKeyboardPress(DIK_S))
			{// キーボードの[S]が押されたとき
			 // 右下
				m_move.x += sinf(D3DX_PI * 0.75f) * PLAYER_MOVE;
				m_move.z += cosf(D3DX_PI * 0.75f) * PLAYER_MOVE;

				// 回転の目標地点を代入
				m_difference.y = -D3DX_PI * 0.25f;
			}
			else
			{// 右
				m_move.x += sinf(D3DX_PI * 0.5f) * PLAYER_MOVE;
				m_move.z += cosf(D3DX_PI * 0.5f) * PLAYER_MOVE;

				// 回転の目標地点を代入
				m_difference.y = -D3DX_PI * 0.5f;
			}

			//// モーションさせる
			//m_MotionType = PLAYER_MOTION_WALK;

			// 歩く状態にする
			m_bWalk = true;
		}
		else if (pInputKeyboard->GetKeyboardPress(DIK_W))
		{// キーボードの[W]が押されたとき
		 // 上
			m_move.x += sinf(-D3DX_PI * 0.0f) * PLAYER_MOVE;
			m_move.z += cosf(-D3DX_PI * 0.0f) * PLAYER_MOVE;

			// 回転の目標地点を代入
			m_difference.y = D3DX_PI * 1.0f;
		}
		else if (pInputKeyboard->GetKeyboardPress(DIK_S))
		{// キーボードの[S]が押されたとき
		 // 下
			m_move.x += sinf(-D3DX_PI * 1.0f) * PLAYER_MOVE;
			m_move.z += cosf(-D3DX_PI * 1.0f) * PLAYER_MOVE;

			// 回転の目標地点を代入
			m_difference.y = D3DX_PI * 0.0f;
		}
	}

	// プレイヤーの回転と目標地点の差を格納
	Diff.y = m_rot.y - m_difference.y;

	// D3DX_PIより大きいとき
	if (Diff.y > D3DX_PI)
	{
		Diff.y -= D3DX_PI * 2;
	}
	else if (Diff.y < -D3DX_PI)
	{// D3DX_PIより小さいとき
		Diff.y += D3DX_PI * 2;
	}

	// プレイヤーを徐々に回転させていく
	m_rot.y -= Diff.y * ROT_AMOUNT;

	// プレイヤーの回転がD3DX_PIより大きい場合
	if (m_rot.y > D3DX_PI)
	{
		m_rot.y -= D3DX_PI * 2;
	}
	else if (m_rot.y < -D3DX_PI)
	{// プレイヤーの回転が-D3DX_PIより小さい場合
		m_rot.y += D3DX_PI * 2;
	}

	// 絶対値がD3DX_PIより大きい場合
	if (fabs(m_rot.y) > D3DX_PI)
	{
		m_rot.y *= -1;
	}
}
