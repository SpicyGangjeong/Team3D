#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const class CModel* pModel, const aiNodeAnim* pAIChannel);
	void	Update_TransformationMatrix(_float fCurrentTrackPosition, vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex);
	_matrix Get_BoneTransformationMatrix() { return m_BoneTransformationMatrix; }
	_int	Get_BoneIndex() { return m_iBoneIndex; }
private:
	_char					m_szName[MAX_PATH] = {};
	_int					m_iBoneIndex = { -1 };

	_uint					m_iNumKeyFrames = {};
	vector<KEYFRAME>		m_KeyFrames;

	_uint					m_iCurrentKeyFrameIndex = { 0 };
	_matrix					m_BoneTransformationMatrix = {};

public:
	static CChannel* Create(const class CModel* pModel, const aiNodeAnim* pAIChannel);
	virtual void Free() override;
};

NS_END