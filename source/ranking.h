//==================================================================================================================
//
// ランキング[ranking.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _RANKING_H_
#define _RANKING_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"

//==================================================================================================================
//　マクロ定義
//==================================================================================================================
#define	RANKING_BG_X (640.0f)				// 背景のXの位置
#define	RANKING_BG_Y (360.0f)				// 背景のYの位置
#define	TIME_COUNT (600)					// 自動フェードするカウンタ

//==================================================================================================================
//　前方宣言
//==================================================================================================================
class CLogo;
class CMeshSphere;

//==================================================================================================================
//
// ランキングクラスの定義
//
//==================================================================================================================
class CRanking
{
public:
	CRanking();								// コンストラクタ
	~CRanking();							// デストラクタ
	void Init(void);						// 初期化処理
	void Uninit(void);						// 終了処理
	void Update(void);						// 更新処理
	void Draw(void);						// 描画処理

	static CRanking *Create(void);			// 生成処理

protected:

private:
	static CLogo *m_pLogo;					// ロゴの情報ポインタ
	static CMeshSphere *m_pMeshSphere;		// メッシュ球の情報ポインタ
	static int m_nTime;						// タイム変数

	int m_nCntTitle;						// タイトルへのカウンタ
};
#endif