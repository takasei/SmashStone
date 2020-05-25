//==================================================================================================================
//
// �����L���O[ranking.h]
// Author : Seiya Takahashi
//
//==================================================================================================================
#ifndef _RANKING_H_
#define _RANKING_H_

//==================================================================================================================
// �C���N���[�h�t�@�C��
//==================================================================================================================
#include "main.h"

//==================================================================================================================
//�@�}�N����`
//==================================================================================================================
#define	RANKING_BG_X (640.0f)				// �w�i��X�̈ʒu
#define	RANKING_BG_Y (360.0f)				// �w�i��Y�̈ʒu
#define	TIME_COUNT (600)					// �����t�F�[�h����J�E���^

//==================================================================================================================
//�@�O���錾
//==================================================================================================================
class CLogo;
class CMeshSphere;

//==================================================================================================================
//
// �����L���O�N���X�̒�`
//
//==================================================================================================================
class CRanking
{
public:
	CRanking();								// �R���X�g���N�^
	~CRanking();							// �f�X�g���N�^
	void Init(void);						// ����������
	void Uninit(void);						// �I������
	void Update(void);						// �X�V����
	void Draw(void);						// �`�揈��

	static CRanking *Create(void);			// ��������

protected:

private:
	static CLogo *m_pLogo;					// ���S�̏��|�C���^
	static CMeshSphere *m_pMeshSphere;		// ���b�V�����̏��|�C���^
	static int m_nTime;						// �^�C���ϐ�

	int m_nCntTitle;						// �^�C�g���ւ̃J�E���^
};
#endif