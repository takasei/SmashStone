//==================================================================================================================
//
// メッシュフィールド[meshField.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _MESHFIELD_H_
#define _MESHFIELD_H_

//==================================================================================================================
// インクルードファイル
//==================================================================================================================
#include "main.h"
#include "scene.h"

//==================================================================================================================
// マクロ定義
//==================================================================================================================
#define Depth 570									// 縦のポリゴン数
#define Width 20									// 横のブロック数
#define WhileX 10.0f								// イチマスの長さ横
#define WhileY 6.0f									// イチマスの長さ高さ
#define WhileZ 10.0f								// イチマスの長さ縦
#define FIELD_TEXTUE "data/TEXTURE/water4.png"		// 読み込むテクスチャのソース先

//==================================================================================================================
// 前方宣言
//==================================================================================================================
class CPlayer;

//==================================================================================================================
//
// メッシュフィールドクラスの定義
//
//==================================================================================================================
class CMeshField : public CScene
{
public:
	CMeshField(PRIORITY type);								// コンストラクタ
	~CMeshField();											// デストラクタ
	void Init(void);										// 初期化処理
	void Uninit(void);										// 終了処理
	void Update(void);										// 更新処理
	void Draw(void);										// 描画処理

	static CMeshField *Create(void);						// 生成処理
	static CMeshField *GetMeshField(void);					// メッシュフィールド情報取得
	static HRESULT Load(void);								// テクスチャ情報ロード
	static void Unload(void);								// テクスチャ情報アンロード

	void SetPos(D3DXVECTOR3 pos);							// 位置設定処理
	void SetMove(D3DXVECTOR3 move);							// 移動量設定処理

	D3DXVECTOR3 GetPos(void);								// 位置取得処理
	D3DXVECTOR3 GetMove(void);								// 移動量取得処理

	float GetHeight(D3DXVECTOR3 pos);						// 高さ算出処理

protected:

private:
	static LPDIRECT3DTEXTURE9 m_pTexture;					// テクスチャへのポインタ
	static CMeshField *m_pMeshField;						// メッシュフィールドのポインタ
	static CPlayer *m_pPlayer;								// プレイヤーの情報ポインタ

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;						// バッファ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;						// インデックスバッファへのポインタ
	VERTEX_3D *m_pVtx;										// 頂点格納
	D3DXMATRIX m_mtxWorld;									// ワールドマトリックス

	D3DXVECTOR3 m_pos;										// 位置
	D3DXVECTOR3 m_rot;										// 回転
	D3DXVECTOR3 m_move;										// 移動
	D3DXVECTOR3 m_aVecA[Depth * Width];						// 法線ベクトルを面の数分一時的に格納
	D3DXVECTOR3 m_aVecB[Depth * Width];						// 法線ベクトルを面の数分一時的に格納
	D3DXVECTOR3 m_vectorA;									// Aベクトル
	D3DXVECTOR3 m_vectorB;									// Bベクトル
	D3DXVECTOR3 m_vectorC;									// Cベクトル

	int m_nDepth;											// 縦のブロック数
	int m_nWidth;											// 横のブロック数
	int m_nNumVertex;										// 総頂点数
	int m_nNumIndex;										// 総インデックス数
	int m_nNumPolygon;										// 総ポリゴン数
	int m_nNumber;											// 三角形の配列の番号
	int m_nStartBox;										// 始まる箱
	int m_nEndBox;											// 引かれる箱

	float fDivide;											// sinの中身を割る変数
};
#endif