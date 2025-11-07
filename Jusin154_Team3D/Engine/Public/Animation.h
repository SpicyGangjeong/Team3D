#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CAnimation : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;
public:
	_bool			Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool bIsLoop, _float fTimeDelta);
	void			Depart_Animation();
	void			Set_AnimPause(_bool bValue) { m_bPause = bValue; }
	_float			Get_AnimProgressRatio();
	_float			Get_AnimEstimatedDuration();
	_int			Get_AnimProgressPostion(const _char* pAnimChannelName);
	void			Set_AnimProgressPostion(const _char* pChannelName, _uint iPosition);
	vector<_int>* Capture_Bones();
	vector<LERPDESC> Get_StartFrameInformations() { return m_StartKeyFrames; }
	const _string& Get_Name() const { return m_strName; }


#ifdef EDITOR_PROJECT
private:
	HRESULT Initialize(const vector<class CBone*>& Bones, const aiAnimation* pAIAnimation);
public:
	HRESULT SaveAsBinary(HANDLE hFile, DWORD& dwByte);
	static CAnimation* Create(const vector<class CBone*>& Bones, const aiAnimation* pAnimation);
#endif // EDITOR_PROJECT

private:
	HRESULT Initialize(HANDLE hFile, DWORD& dwByte);
	HRESULT Combined_Initialize();
	_uint Get_BoneIndex(const char* pChannelName);

private:
	_bool					m_bPause = { FALSE };							// 애님퍼즈 인덱스
	_string					m_strName = {};									// 애님 이름
	_float					m_fCurrentTrackPosition = {};					// 현재 트랙 위치
	_float					m_fDuration = {};								// 트랙의 전체 길이
	_float					m_TickPerSeconds[2] = { 0.f, 0.f };				// 초당 트랙 이동 거리
	_uint					m_iNumChannels = {};							// 영향을 받는 본의 수

	vector<class CChannel*> m_Channels;						// 이 애니메이션이 영향을 준 사전 계산된 본의 배열
	vector<_uint>			m_CurrentKeyFrameIndices;		// 각 본이 현재 애니메이션에서 취해야할 키프레임 왼쪽 인덱스

	vector<_int>			m_DestBones;					// 이 애니메이션에서 영향을 받는 본들의 집합
	vector<LERPDESC>		m_StartKeyFrames;	// 이 애니메이션의 영향을 받는 본들의 초기값

public:
	static CAnimation* Create(HANDLE hFile, DWORD& dwByte);
	CAnimation* Clone();
	virtual void Free() override;
	void Describe_Entity();
};

NS_END