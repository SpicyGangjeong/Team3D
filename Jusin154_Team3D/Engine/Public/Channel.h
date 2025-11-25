#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CChannel : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

#ifdef EDITOR_PROJECT
private:
	HRESULT Initialize(const vector<class CBone*>& Bones, const aiNodeAnim* pAIChannel);

public:
	static CChannel* Create(const vector<class CBone*>& Bones, const aiNodeAnim* pAIChannel);

#endif // EDITOR_PROJECT
public:
	void Fill_GPU_Keyframes(vector<KEYFRAME_DESC>& outKeyframes);
	CHANNEL_DESC Fill_GPU_ChannelDesc();

public:
	void Update_TransformationMatirx(const vector<class CBone*>& Bones, const LOCALPOS_DESC* pLocalPosArray, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex,class CTransform* pTransform=nullptr,_float m_fAmount = 1.f);
	_int Get_BoneIndex() { return { m_iBoneIndex }; }
	void Set_BoneIndex(_int iBoneIndex) { m_iBoneIndex = iBoneIndex; }
	void ResetRootMotion();
	_matrix Get_BoneTransformationMatrix() { return m_BoneTransformationMatrix; }

	KEYFRAME* Get_Frame(_uint iIndex);
	void Set_Frame(_uint iIndex, KEYFRAME& kf);
	_bool Compare_Name(const _char* pChannelname) const {
		return !strcmp(pChannelname, m_szName);
	}
	_char* Get_Name() { return m_szName; }
private:
	HRESULT Initialize(HANDLE hFile, DWORD& dwByte);
	// 바이너리
	HRESULT Initialize(const class CModel* pModel, SaveChannel* pSaveChannel);
	//
	HRESULT Initialize(const vector<class CBone*>& Bones, _uint iIndex);

private:
	_char					m_szName[MAX_PATH] = {};	// 영향을 받을 본의 이름
	_int					m_iBoneIndex = { -1 };		// 본 벡터에서 본의 인덱스
	_uint					m_iNumKeyFrames = {};		// Animation의 m_fDuration동안 이 본이 취해야할 키프레임의 갯수
	_float4x4				m_PreTransformMatrix = {};
	vector<KEYFRAME>		m_KeyFrames;	// 키프레임 벡터
	_float3					m_vPrevRootPos = { 0.f, 0.f, 0.f };
	_float4					m_vPrevRootRot = { 0.f, 0.f, 0.f,0.f };
	_matrix					m_BoneTransformationMatrix = {};
	_bool					m_bInitialRootPos = { false };
	_bool					m_bInitialRootRotSaved = {false};
	_float4					m_vInitialRootRot = {};

	vector<KEYFRAME_DESC>	m_KeyFrameDesc;
public:
	// 바이너리
	static CChannel* Create(const class CModel* pModel, SaveChannel* pSaveChannel);
	//
	virtual void Free() override;
};

NS_END
