//==================================================================================================================
//
// プレイヤー処理[player.cpp]
// Author : Seiya Takahashi
//
//==================================================================================================================
#include "inputKeyboard.h"
#include "renderer.h"
#include "player.h"
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
#include "character.h"
#include "scene.h"

//==================================================================================================================
// 静的メンバ変数の初期化
//==================================================================================================================
CCamera *CPlayer::m_pCamera = NULL;				// カメラ情報

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define MOTION_FILE_NAME "data/TEXT/C++3DMotion.txt"// 読み込むテキストデータのソース先
#define TITLE_PLAYER_MOVE 0.15f		// タイトルのプレイヤーの移動量
#define PLAYER_MOVE 0.25f			// プレイヤーの移動量
#define PLAYER_INERTIA 3.0f			// 慣性スピード
#define PLAYER_SPEED_DOWN -0.2f		// スピードダウン速度
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
CPlayer::CPlayer(PRIORITY type = CScene::PRIORITY_PLAYER) : CCharacter(type)
{

}

//==================================================================================================================
// デストラクタ
//==================================================================================================================
CPlayer::~CPlayer()
{

}

//==================================================================================================================
// 初期化処理
//==================================================================================================================
void CPlayer::Init(void)
{
	m_pos = D3DXVECTOR3(0.0f, WhileY, 0.0f);				// 位置
	m_posOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				// 前回の位置
	m_rot = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);				// 回転
	m_difference = D3DXVECTOR3(0.0f, D3DX_PI, 0.0f);		// 回転の目標地点
	m_move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);					// 移動量
	m_size = D3DXVECTOR3(1.0f, 1.0f, 1.0f);					// 大きさ

	//モーションの初期化
	for (int nCntMotion = 0; nCntMotion < PLAYER_MOTION_MAX; nCntMotion++)
	{
		m_PlayerMotion[nCntMotion].nLoop = 0;										// ループ
		m_PlayerMotion[nCntMotion].nNumKey = 0;										// キー情報の数
		m_PlayerMotion[nCntMotion].Collision_nParts = 0;							// パーツ番号
		m_PlayerMotion[nCntMotion].CollisionOfset = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// オフセット
		m_PlayerMotion[nCntMotion].CollisionRadius = 0.0f;							// 円の半径
		m_PlayerMotion[nCntMotion].CollisionStartFram = 0;							// 開始フレーム
		m_PlayerMotion[nCntMotion].CollisionEndFram = 0;							// 終了フレーム
		m_PlayerMotion[nCntMotion].Collision_Damage = 0;							// ダメージ数

		// キーの総数をカウント
		for (int nCntKeySet = 0; nCntKeySet < 10; nCntKeySet++)
		{
			m_PlayerMotion[nCntMotion].key_info[nCntKeySet].nFram = 0;				// 各キーのフレーム

			for (int nCntKey = 0; nCntKey < 11; nCntKey++)
			{
				m_PlayerMotion[nCntMotion].key_info[nCntKeySet].key[nCntKey].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 各パーツの各キーの位置
				m_PlayerMotion[nCntMotion].key_info[nCntKeySet].key[nCntKey].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 各パーツの各キーの回転
			}
		}
	}

	//=================================モデルの設定=================================//
	//体 0
	m_MotionModel[0] = CMotionModel::CreateObject(CMotionModel::MODEL_PENGUIN_BODY);		// 体生成
	m_MotionModel[0]->SetPos(D3DXVECTOR3(0, -2.0f, 0));										// 位置設定
	m_MotionModel[0]->SetRot(D3DXVECTOR3(0.0f, -1.5f, 0.0f));								// 回転設定
	m_MotionModel[0]->SetParent(NULL);														// 親設定
	//頭 1
	m_MotionModel[1] = CMotionModel::CreateObject(CMotionModel::MODEL_PENGUIN_HEAD);		// 頭パーツ生成
	m_MotionModel[1]->SetPos(D3DXVECTOR3(-11.0f, 5.0f, -0.44f));							// 位置設定
	m_MotionModel[1]->SetParent(m_MotionModel[CMotionModel::MODEL_PENGUIN_BODY]);			// 親設定
	//左腕 2
	m_MotionModel[2] = CMotionModel::CreateObject(CMotionModel::MODEL_PENGUIN_LARM);		// 左腕生成
	m_MotionModel[2]->SetPos(D3DXVECTOR3(-5.0f, 3.0f, -4.0f));								// 位置設定
	m_MotionModel[2]->SetParent(m_MotionModel[CMotionModel::MODEL_PENGUIN_BODY]);			// 親設定
	//右腕 3
	m_MotionModel[3] = CMotionModel::CreateObject(CMotionModel::MODEL_PENGUIN_RARM);		// 右腕生成
	m_MotionModel[3]->SetPos(D3DXVECTOR3(-5.0f, 3.0f, 4.0f));								// 位置設定
	m_MotionModel[3]->SetParent(m_MotionModel[CMotionModel::MODEL_PENGUIN_BODY]);			// 親設定
	//左足 4
	m_MotionModel[4] = CMotionModel::CreateObject(CMotionModel::MODEL_PENGUIN_LLEG);		// 左足生成
	m_MotionModel[4]->SetPos(D3DXVECTOR3(6.0f, 0.0f, -3.0f));								// 位置設定
	m_MotionModel[4]->SetParent(m_MotionModel[CMotionModel::MODEL_PENGUIN_BODY]);			// 親設定
	//右足 5
	m_MotionModel[5] = CMotionModel::CreateObject(CMotionModel::MODEL_PENGUIN_RLEG);		// 右足生成
	m_MotionModel[5]->SetPos(D3DXVECTOR3(6.0f, 0.0f, 3.0f));								// 位置設定
	m_MotionModel[5]->SetParent(m_MotionModel[CMotionModel::MODEL_PENGUIN_BODY]);			// 親設定

	// モーションの読み込み
	LoadMotion();
}

//==================================================================================================================
// 終了処理
//==================================================================================================================
void CPlayer::Uninit(void)
{
	// プレイヤーモデルの最大パーツまでカウント
	for (int nCnt = 0; nCnt < PLAYER_MAX_MODEL; nCnt++)
	{
		// モデルの終了処理
		m_MotionModel[nCnt]->Uninit();

		// メモリ削除
		delete m_MotionModel[nCnt];

		// ポインタ用NULL
		m_MotionModel[nCnt] = nullptr;
	}
}

//==================================================================================================================
// 更新処理
//==================================================================================================================
void CPlayer::Update(void)
{
	m_pos = GetPos();			// 位置取得
	m_rot = GetRot();			// 回転取得
	m_size = GetSize();			// 大きさ取得

	// 前のモーションの代入
	m_MotionOld = m_MotionType;

	// モーション
	Moation();

	SetPos(m_pos);				// 位置設定
	SetRot(m_rot);				// 回転設定
	SetSize(m_size);			// 大きさ設定

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
void CPlayer::Draw(void)
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

	// モデルの描画
	for (int nCnt = 0; nCnt < PLAYER_MAX_MODEL; nCnt++)
	{
		// 中心のマトリックス設定
		m_MotionModel[nCnt]->SetMtxCenter(m_mtxWorld);

		// 描画処理
		m_MotionModel[nCnt]->Draw();
	}
}

//==================================================================================================================
// 生成処理
//==================================================================================================================
CPlayer *CPlayer::Create(void)
{
	// シーン動的に確保
	CPlayer *pPlayer = new CPlayer(CScene::PRIORITY_PLAYER);

	pPlayer->Init();			// プレイヤーの初期化

	// 値を返す
	return pPlayer;
}

////==================================================================================================================
//// 位置設定
////==================================================================================================================
//void CPlayer::SetPos(D3DXVECTOR3 pos)
//{
//	m_pos = pos;
//}
//
////==================================================================================================================
//// 回転設定
////==================================================================================================================
//void CPlayer::SetRot(D3DXVECTOR3 rot)
//{
//	m_rot = rot;
//}
//
////==================================================================================================================
//// 大きさ設定
////==================================================================================================================
//void CPlayer::SetSize(D3DXVECTOR3 size)
//{
//	m_size = size;
//}

//==================================================================================================================
// モーション読み込み
//==================================================================================================================
void CPlayer::LoadMotion(void)
{
	FILE *pFile;			// ファイルポイント
	int nCntMotion = 0;		// モーションの数
	int nCntModel = 0;		// モデルのカウント
	int nCntKeySet = 0;		// フレームの分割数
	int nCntKey = 0;		// パーツの番号
	char cReadText[128];	// 文字として読み取り用
	char cHeadText[128];	// 比較する用
	char cDie[128];			// 不要な文字

	// ファイルを開く
	pFile = fopen(MOTION_FILE_NAME, "r");

	// ファイルがあるとき
	if (pFile != NULL)
	{
		// 読み込んだ文字が[SCRIPT]まで繰り返す
		while (strcmp(cHeadText, "SCRIPT") != 0)
		{
			fgets(cReadText, sizeof(cReadText), pFile);						// 一文を読み込む
			sscanf(cReadText, "%s", &cHeadText);							// 比較用テキストに文字を代入
		}

		// 読み込んだ文字が[SCRIPT]のとき
		if (strcmp(cHeadText, "SCRIPT") == 0)
		{
			// エンドスクリプトが来るまでループ
			while (strcmp(cHeadText, "END_SCRIPT") != 0)
			{
				fgets(cReadText, sizeof(cReadText), pFile);					// 一文を読み込む
				sscanf(cReadText, "%s", &cHeadText);						// 比較用テキストに文字を代入

				// モーションセットが来たら
				if (strcmp(cHeadText, "MOTIONSET") == 0)
				{
					// フレーム分割数の初期化
					nCntKeySet = 0;

					// エンドモーションセットが来るまでループ
					while (strcmp(cHeadText, "END_MOTIONSET") != 0)
					{
						fgets(cReadText, sizeof(cReadText), pFile);			// 一文を読み込む
						sscanf(cReadText, "%s", &cHeadText);				// 比較用テキストに文字を代入

						// 読み込んだ文字が[LOOP]のとき
						if (strcmp(cHeadText, "LOOP") == 0)
						{
							sscanf(cReadText, "%s %s %d", &cDie, &cDie, &m_PlayerMotion[nCntMotion].nLoop);			// ループ変数に値を代入
						}
						// 読み込んだ文字が[NUM_KEY]のとき
						else if (strcmp(cHeadText, "NUM_KEY") == 0)
						{
							sscanf(cReadText, "%s %s %d", &cDie, &cDie, &m_PlayerMotion[nCntMotion].nNumKey);		// キー情報の数に値を代入
						}
						// 読み込んだ文字が[KEYSET]のとき
						else if (strcmp(cHeadText, "KEYSET") == 0)
						{
							nCntKey = 0;									// キー情報初期化

							// 読み込んだ文字が[END_KEYSET]まで繰り返す
							while (strcmp(cHeadText, "END_KEYSET") != 0)
							{
								fgets(cReadText, sizeof(cReadText), pFile);							// 一文を読み込む
								sscanf(cReadText, "%s", &cHeadText);								// 比較用テキストに文字を代入

								// 読み込んだ文字が[FRAME]のとき
								if (strcmp(cHeadText, "FRAME") == 0)
								{
									sscanf(cReadText, "%s %s %d", &cDie, &cDie,
										&m_PlayerMotion[nCntMotion].key_info[nCntKeySet].nFram);	// フレームに値を代入
								}
								// 読み込んだ文字が[KEY]のとき
								else if (strcmp(cHeadText, "KEY") == 0)
								{
									// 読み込んだ文字が[END_KEY]まで繰り返す
									while (strcmp(cHeadText, "END_KEY") != 0)
									{
										fgets(cReadText, sizeof(cReadText), pFile);				// 一文を読み込む
										sscanf(cReadText, "%s", &cHeadText);					// 比較用テキストに文字を代入

										// 読み込んだ文字が[POS]のとき
										if (strcmp(cHeadText, "POS") == 0)
										{
											sscanf(cReadText, "%s %s %f %f %f",					// 位置に値を代入
												&cDie, &cDie,
												&m_PlayerMotion[nCntMotion].key_info[nCntKeySet].key[nCntKey].pos.x,
												&m_PlayerMotion[nCntMotion].key_info[nCntKeySet].key[nCntKey].pos.y,
												&m_PlayerMotion[nCntMotion].key_info[nCntKeySet].key[nCntKey].pos.z);

											//m_PlayerMotion[nCntMotion].key_info[nCntKeySet].key[nCntKey].pos += m_Model[nCntKey]->GetPosition();
										}
										// 読み込んだ文字が[ROT]のとき
										else if (strcmp(cHeadText, "ROT") == 0)
										{
											sscanf(cReadText, "%s %s %f %f %f",					// 回転に値を代入
												&cDie, &cDie,
												&m_PlayerMotion[nCntMotion].key_info[nCntKeySet].key[nCntKey].rot.x,
												&m_PlayerMotion[nCntMotion].key_info[nCntKeySet].key[nCntKey].rot.y,
												&m_PlayerMotion[nCntMotion].key_info[nCntKeySet].key[nCntKey].rot.z);
										}
									}

									// パーツ番号加算
									nCntKey++;
								}
							}

							// フレームの分割数加算
							nCntKeySet++;
						}
					}
					// モーションの数加算
					nCntMotion++;
				}
			}
		}
		// ファイルを閉じる
		fclose(pFile);
	}
	// ファイルが無いとき
	else
	{
		// メッセージで知らせる
		MessageBox(NULL, "モーションデータの読み込み失敗", "警告", MB_ICONWARNING);
	}
}

//==================================================================================================================
// モーションさせる
//==================================================================================================================
void CPlayer::Moation(void)
{
	// 前のモーションと今のモーションが違うとき
	if (m_MotionType != m_MotionOld)
	{
		m_Fram = 0;					// フレームを初期値に戻す
		m_nCntKeySet = 0;			// モーション情報を初期値に戻す
	}

	// プレイヤーのモデルの最大パーツ数までカウント
	for (int nCnt = 0; nCnt < PLAYER_MAX_MODEL; nCnt++)
	{
		// フレームが0のとき
		if (m_Fram == 0)
		{
			// モーション情報のフレームが0じゃないとき
			if (m_PlayerMotion[m_MotionType].key_info[m_nCntKeySet].nFram != 0)
			{
				// 移動量ROTの計算
				rotBET[nCnt] = (m_PlayerMotion[m_MotionType].key_info[m_nCntKeySet].key[nCnt].rot - m_MotionModel[nCnt]->GetRot()) /
					(float)m_PlayerMotion[m_MotionType].key_info[m_nCntKeySet].nFram;
			}
			else
			{
				//m_MotionModel[nCnt]->GetPosition() = m_PlayerMotion[m_MotionType].key_info[mCntKeySet].key[nCnt].pos;
				m_MotionModel[nCnt]->GetRot() = m_PlayerMotion[m_MotionType].key_info[m_nCntKeySet].key[nCnt].rot;
				posBET[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				rotBET[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			}
		}

		// フレーム移動
		if (m_Fram <= m_PlayerMotion[m_MotionType].key_info[m_nCntKeySet].nFram)
		{
			m_MotionModel[nCnt]->SetPos(m_MotionModel[nCnt]->GetPos() + posBET[nCnt]);
			m_MotionModel[nCnt]->SetRot(m_MotionModel[nCnt]->GetRot() + rotBET[nCnt]);
		}
	}

	// フレームがモーション情報のフレームと同じであるとき
	if (m_Fram == m_PlayerMotion[m_MotionType].key_info[m_nCntKeySet].nFram)
	{
		m_nCntKeySet++;
		m_Fram = 0;

		// キーセット数が規定値と同じになったら
		if (m_nCntKeySet == m_PlayerMotion[m_MotionType].nNumKey)
		{
			// ループしないとき
			if (m_PlayerMotion[m_MotionType].nLoop == 0)
			{
				m_nCntKeySet = 0;
				m_MotionType = 0;
				m_Fram = m_PlayerMotion[m_MotionType].key_info[m_nCntKeySet].nFram;
				//g_Player.bAttack = false;
			}
			// ループするとき
			else if (m_PlayerMotion[m_MotionType].nLoop == 1)
			{
				m_nCntKeySet = 0;
				m_MotionType = 0;
			}
		}
	}
	// フレーム数が規定値と同じではないとき
	else
	{
		// フレーム加算
		m_Fram++;
	}
}
