#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const class CModel* pModel, const aiAnimation* pAIAnimation);
	_bool Update_TransformationMatrices(_float fTimeDelta, _bool isLoop, vector<class CBone*>& Bones);
	void InterpAnim(CAnimation* pPreAnim, vector<CBone*>& Bones, float fRatio);

private:
	const aiAnimation* m_pAIAnimation = { nullptr };

	_float						m_fCurrentTrackPosition = {};
	_double						m_fDuration = {};
	_float						m_fTickPerSecond = {};

	vector<class CChannel*>		m_Channels;
	_uint						m_iNumChannels = {};
	vector<_uint>				m_CurrentKeyFrameIndices;
	vector<_matrix>				m_vBoneTransformationMatrix = {};

public:
	static CAnimation* Create(const class CModel* pModel, const aiAnimation* pAIAnimation);
	CAnimation* Clone();
	virtual void Free() override;
};

NS_END

