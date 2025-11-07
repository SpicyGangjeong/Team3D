#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLerpAnim : public CBase
{
private:
	CLerpAnim();
	virtual ~CLerpAnim() = default;
public:
	_bool		IsLerping() { return m_bLerping; }
	_bool		Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta);
	HRESULT		Initialize(_uint iNumBone, _float fTickPerSecond, const vector<class CBone*>& Bones);
	void		Begin(const vector<class CBone*>& Bones, vector<LERPDESC>& StartFrames, vector<LERPDESC>& EndFrames, _float fDuration, _uint iRootBoneIndex);

private:
	_float					m_fCurrentTrackPosition = {};	// 현재 트랙 위치
	_float					m_fTickPerSecond = {};			// 초당 트랙 이동 거리
	_uint					m_iNumChannels = {};			// 영향을 받는 본의 수

	vector<class CChannel*> m_Channels;						// 이 애니메이션이 영향을 준 사전 계산된 본의 배열
	vector<KEYFRAME>		m_InitialFrames;				// 초기 프레임
	vector<_uint>			m_CurrentKeyFrameIndices;		// 각 본이 현재 애니메이션에서 취해야할 키프레임 왼쪽 인덱스
	vector<_bool>			m_Movable;						// 각 본이 럴프 도중 업데이트를 받아야할지 말아야할지 스위치
	_bool					m_bLerping = { false };			// 럴프 확인 부울
	_float					m_fDuration = { 0.25f };		// 럴프 주기

public:
	static CLerpAnim* Create(_uint iNumBone, _float fTickPerSecond, const vector<class CBone*>& Bones);
	virtual void Free() override;

};

NS_END