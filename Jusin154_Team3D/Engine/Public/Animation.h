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
	_bool			Update_TransformationMatrices(const vector<class CBone*>& Bones, const LOCALPOS_DESC* pLocalPosArray, _bool bIsLoop, _float fTimeDelta, class CTransform* pTransform = nullptr, _float m_fAmount = 1.f);
	void Reset();
	void			ResetRootMotion();
	void			Depart_Animation();
	void			Set_AnimPause(_bool bValue) { m_bPause = bValue; }
	_float			Get_AnimProgressRatio();
	_float			Get_AnimEstimatedDuration();
	_int			Get_AnimProgressPostion(const _char* pAnimChannelName);
	void			Set_AnimProgressPostion(const _char* pChannelName, _uint iPosition);
	vector<_int>* Capture_Bones();
	void InterpAnim(CAnimation* pPreAnim, vector<CBone*>& Bones, float fRatio);
	void CreateGPUData(ID3D11Device* pDevice);
	vector<LERPDESC> Get_StartFrameInformations() { return m_StartKeyFrames; }
	const _string& Get_Name() const { return m_strName; }

	const _char* Get_SZName() const { return m_szName; }
	void Set_CurrentTrackPosition(_float TrackPosition) { m_fCurrentTrackPosition = TrackPosition; }
	_float Get_CurrentTrackPosition() { return m_fCurrentTrackPosition; }
	_float Get_CurrentTrackProgressRatio() { return m_fCurrentTrackPosition / m_fDuration; }
	void Set_AnimSpeed(_float fSpeed) { m_fAnimSpeed = fSpeed; }
	_float Get_AnimSpeed() { return m_fAnimSpeed; }

	_uint Get_ChannelCount() { return m_iNumChannels; }
	_float Get_Duration() { return m_fDuration; }

	ID3D11Buffer* Get_KeyFrameBuffer() { return m_pKeyFrameBuffer; }
	ID3D11Buffer* Get_ChannelBuffer() { return m_pChannelBuffer; }
	ID3D11ShaderResourceView* Get_KeyFrameSrv() { return m_pKeyFrameSrv; }
	ID3D11ShaderResourceView* Get_ChannelSrv() { return m_pChannelSrv; }
#ifdef EDITOR_PROJECT
private:
	HRESULT Initialize(const vector<class CBone*>& Bones, const aiAnimation* pAIAnimation);
public:
	static CAnimation* Create(const vector<class CBone*>& Bones, const aiAnimation* pAnimation);
#endif // EDITOR_PROJECT

private:
	// 바이너리
	HRESULT Initialize(const vector<CBone*>& Bones, const class CModel* pModel,SaveAnimation* pSaveAnimation);
	//
	HRESULT Combined_Initialize();
	_uint Get_BoneIndex(const char* pChannelName);

	HRESULT CreateKeyFrameBuffer(ID3D11Device* pDevice, vector<KEYFRAME_DESC> keyframe);
	HRESULT CreateChannelBuffer(ID3D11Device* pDevice, vector<CHANNEL_DESC> channel);



private:
	SaveAnimation*			m_pSaveAnim = { nullptr };
	_char					m_szName[MAX_PATH] = {};
	_float					m_fTickPerSecond = {};
	_float					m_fAnimSpeed = {1.f};

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
	vector<_matrix>			m_vBoneTransformationMatrix = {};

	_uint m_iChannelCount;
	_uint m_iKeyframeCount;
	_uint m_iBoneCount;
	vector<KEYFRAME_DESC>	m_KeyFrameDesc;
	vector<CHANNEL_DESC>    m_ChannelDesc;

	ID3D11Buffer* m_pKeyFrameBuffer;
	ID3D11Buffer* m_pChannelBuffer;
	ID3D11ShaderResourceView* m_pKeyFrameSrv = {};
	ID3D11ShaderResourceView* m_pChannelSrv = {};
public:
	// 바이너리
	static CAnimation* Create(const vector<CBone*>& Bones, const class CModel* pModel, SaveAnimation* pSaveAnimation);
	//
	CAnimation* Clone();
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity();
#endif // _DEBUG

};

NS_END
