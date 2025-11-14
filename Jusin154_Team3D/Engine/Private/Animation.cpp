#include "pch.h"

#include "Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
	/*XMMatrixDecompose();*/
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_fCurrentTrackPosition{ rhs.m_fCurrentTrackPosition }
	, m_fDuration{ rhs.m_fDuration }
	, m_iNumChannels{ rhs.m_iNumChannels }
	, m_Channels{ rhs.m_Channels }
	, m_CurrentKeyFrameIndices{ rhs.m_CurrentKeyFrameIndices }
	, m_StartKeyFrames{ rhs.m_StartKeyFrames }
	, m_DestBones{ rhs.m_DestBones }
	, m_strName{ rhs.m_strName }
	, m_fTickPerSecond{ rhs.m_fTickPerSecond }
	, m_pSaveAnim{ rhs.m_pSaveAnim }
	, m_fAnimSpeed{rhs.m_fAnimSpeed }
{
	memcpy_s(m_TickPerSeconds, sizeof(_float) * 2, rhs.m_TickPerSeconds, sizeof(_float) * 2);

	strcpy_s(m_szName, sizeof(m_szName), rhs.m_szName);

	for (auto& pChannel : m_Channels) {
		SAFE_ADDREF(pChannel);
	}
}
_bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _bool bIsLoop, _float fTimeDelta)
{
	//m_fCurrentTrackPosition += m_TickPerSeconds[m_bPause] * fTimeDelta;
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta * m_fAnimSpeed;
	if (m_fCurrentTrackPosition >= m_fDuration) {
		m_fCurrentTrackPosition = 0.f; // 루프
		if (false == bIsLoop) {
			m_fCurrentTrackPosition = m_fDuration;
			return true;
		}
		else {
			Depart_Animation();
			return true;
		}
	}

	_uint iIndex = {};
	for (auto& pChannel : m_Channels) {
		pChannel->Update_TransformationMatirx(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iIndex++]);
	}

	return false;
}


void CAnimation::Depart_Animation()
{
	m_fCurrentTrackPosition = 0.f;
	fill(m_CurrentKeyFrameIndices.begin(), m_CurrentKeyFrameIndices.end(), 0);
}
_float CAnimation::Get_AnimProgressRatio()
{
	return (m_fCurrentTrackPosition / m_fDuration);
}
_float CAnimation::Get_AnimEstimatedDuration()
{
	return m_fDuration / m_TickPerSeconds[0];
}
_int CAnimation::Get_AnimProgressPostion(const _char* pAnimChannelName)
{
	for (_int iIndex = 0; iIndex < m_Channels.size(); ++iIndex) {
		if (true == m_Channels[iIndex]->Compare_Name(pAnimChannelName)) {
			return m_CurrentKeyFrameIndices[iIndex];
		}
	}
	return -1;
}
void CAnimation::Set_AnimProgressPostion(const _char* pChannelName, _uint iPosition)
{
	Depart_Animation();

	for (_int iIndex = 0; iIndex < m_Channels.size(); ++iIndex) {
		if (true == m_Channels[iIndex]->Compare_Name(pChannelName)) {
			m_fCurrentTrackPosition = m_Channels[iIndex]->Get_Frame(iPosition)->fTrackPosition;
			return;
		}
	}
}
vector<_int>* CAnimation::Capture_Bones()
{
	if (m_DestBones.empty()) {
		m_DestBones.reserve(m_iNumChannels);
		for (_uint i = 0; i < m_iNumChannels; ++i) {
			m_DestBones.push_back(m_Channels[i]->Get_BoneIndex());
		}
	}
	return &m_DestBones;
}
#ifdef EDITOR_PROJECT
HRESULT CAnimation::Initialize(const vector<CBone*>& Bones, const aiAnimation* pAIAnimation)
{
	m_strName = pAIAnimation->mName.data;
	strcpy_s(m_szName, pAIAnimation->mName.data);
	size_t pos = m_strName.find_last_of('|');
	if (0 > pos) {
		pos = 0;
	}
	else {
		pos++;
	}
	m_strName = m_strName.substr(pos);

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_TickPerSeconds[0] = (_float)pAIAnimation->mTicksPerSecond;
	m_iNumChannels = pAIAnimation->mNumChannels;
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (_uint i = 0; i < m_iNumChannels; ++i) {
		CChannel* pChannel = CChannel::Create(Bones, pAIAnimation->mChannels[i]);
		if (nullptr == pChannel) {
			return E_FAIL;
		}
		m_Channels.push_back(pChannel);
	}

	if (FAILED(Combined_Initialize())) {
		return E_FAIL;
	}
	return S_OK;
}
HRESULT CAnimation::SaveAsBinary(HANDLE hFile, DWORD& dwByte)
{
	size_t iNameLength = m_strName.length();
	WriteFile(hFile, &iNameLength, sizeof(size_t), &dwByte, nullptr);
	WriteFile(hFile, m_strName.data(), (DWORD)m_strName.length(), &dwByte, nullptr);
	WriteFile(hFile, &m_fDuration, sizeof(_float), &dwByte, nullptr);
	WriteFile(hFile, &m_TickPerSeconds, sizeof(_float) * 2, &dwByte, nullptr);
	WriteFile(hFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumChannels; ++i) {
		m_Channels[i]->SaveAsBinary(hFile, dwByte);
	}
	return S_OK;
}

CAnimation* CAnimation::Create(const vector<CBone*>& Bones, const aiAnimation* pAnimation)
{
	CAnimation* pInstance = new CAnimation;
	if (FAILED(pInstance->Initialize(Bones, pAnimation))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}
#endif


HRESULT CAnimation::Initialize(HANDLE hFile, DWORD& dwByte)
{
	size_t iNameLength = {};
	if (!ReadFile(hFile, &iNameLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}
	m_strName.resize(iNameLength);
	if (!ReadFile(hFile, (void*)m_strName.data(), (DWORD)iNameLength, &dwByte, nullptr)) {
		return E_FAIL;
	}
	if (!ReadFile(hFile, &m_fDuration, sizeof(_float), &dwByte, nullptr)) {
		return E_FAIL;
	}
	if (!ReadFile(hFile, &m_TickPerSeconds, sizeof(_float) * 2, &dwByte, nullptr)) {
		return E_FAIL;
	}
	if (!ReadFile(hFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr)) {
		return E_FAIL;
	}
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);
	m_Channels.reserve(m_iNumChannels);
	for (_uint i = 0; i < m_iNumChannels; ++i) {
		m_Channels.push_back(CChannel::Create(hFile, dwByte));
	}
	if (FAILED(Combined_Initialize())) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CAnimation::Initialize(const CModel* pModel, SaveAnimation* pSaveAnimation)
{
	m_pSaveAnim = pSaveAnimation;

	strcpy_s(m_szName, pSaveAnimation->AnimName.c_str());
	m_fDuration = pSaveAnimation->mDuration;
	m_fTickPerSecond = pSaveAnimation->mTicksPerSecond;

	m_iNumChannels = pSaveAnimation->ChannelCount;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pModel, &pSaveAnimation->Channels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Combined_Initialize()
{
	m_StartKeyFrames.reserve(m_iNumChannels);
	for (auto& pChannel : m_Channels) {
		LERPDESC desc = { pChannel->Get_BoneIndex(), *pChannel->Get_Frame(0) };
		m_StartKeyFrames.push_back(desc);
	}
	return S_OK;
}

_uint CAnimation::Get_BoneIndex(const char* pChannelName)
{
	return _uint();
}

CAnimation* CAnimation::Create(HANDLE hFile, DWORD& dwByte)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(hFile, dwByte)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Create(const CModel* pModel, SaveAnimation* pSaveAnimation)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pModel, pSaveAnimation)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels) {
		SAFE_RELEASE(pChannel);
	}
	m_Channels.clear();

	m_CurrentKeyFrameIndices.clear();
}
;
void CAnimation::Describe_Entity()
{
	//if (GUI::CollapsingHeader(m_strName.c_str())) {
	//	
	//}
}