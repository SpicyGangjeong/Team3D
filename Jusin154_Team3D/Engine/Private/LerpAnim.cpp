#include "pch.h"
#include "LerpAnim.h"
#include "Channel.h"
#include "Bone.h"

CLerpAnim::CLerpAnim()
{
}

_bool CLerpAnim::Update_TransformationMatrices(const vector<CBone*>& Bones, _float fTimeDelta)
{
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration) {
		m_fCurrentTrackPosition = m_fDuration;
		m_bLerping = false;
		return false;
	}

	_uint iIndex = {};
	for (auto& pChannel : m_Channels) {
		if (true == m_Movable[iIndex]) {
			pChannel->Update_TransformationMatirx(Bones, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iIndex]);
		}
		iIndex++;
	}
	return true;
}

HRESULT CLerpAnim::Initialize(_uint iNumBone, _float fTickPerSecond, const vector<CBone*>& Bones)
{
	m_iNumChannels = iNumBone;
	m_fTickPerSecond = fTickPerSecond;
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);
	m_Movable.resize(m_iNumChannels);
	m_InitialFrames.resize(m_iNumChannels);
	m_Channels.reserve(m_iNumChannels);

	for (_uint i = 0; i < m_iNumChannels; ++i) {
		CChannel* pChannel = CChannel::Create(Bones, i);
		if (nullptr == pChannel) {
			return E_FAIL;
		}
		m_Channels.push_back(pChannel);
		Bones[i]->Get_KeyFrame(m_InitialFrames[i], false);
		m_InitialFrames[i].fTrackPosition = m_fDuration;
		pChannel->Set_Frame(1, m_InitialFrames[i]);
	}
	return S_OK;
}
void CLerpAnim::Begin(const vector<class CBone*>& Bones, vector<LERPDESC>& StartFrames, vector<LERPDESC>& EndFrames, _float fDuration, _uint iRootBoneIndex)
{
	{ // 초기화

		//KEYFRAME kf = { // 항등행렬의 키프레임
		//	{ 1.f, 1.f, 1.f },
		//	{ 0.f, 0.f, 0.f, 1.f },
		//	{ 0.f, 0.f, 0.f},
		//	m_fDuration };
		m_bLerping = true;
		m_fDuration = fDuration;
		m_fCurrentTrackPosition = 0.f;
		for (_uint i = 0; i < m_iNumChannels; ++i) {
			m_Channels[i]->Set_Frame(1, m_InitialFrames[i]); // 끝프레임을 초기 값으로 채우기
		}
		fill(m_CurrentKeyFrameIndices.begin(), m_CurrentKeyFrameIndices.end(), 0);
		fill(m_Movable.begin(), m_Movable.end(), false);
	}
	{ // 실제 값 채우기
		for (LERPDESC& Desc : EndFrames) // 끝프레임을 실제 값으로 채우기
		{
			_int iIndex = Desc.iSlot;
			Desc.tagKeyFrame.fTrackPosition = m_fDuration;
			m_Channels[iIndex]->Set_Frame(1, Desc.tagKeyFrame);
		}
		for (_uint i = 0; i < StartFrames.size(); ++i) { // 시작 프레임을 현재 본 상태 키프레임으로 채우기
			_int iIndex = StartFrames[i].iSlot;
			m_Movable[iIndex] = true;
			m_Channels[iIndex]->Set_Frame(0, StartFrames[i].tagKeyFrame);
		}
		//m_Movable[iRootBoneIndex] = false;
	}
}
CLerpAnim* CLerpAnim::Create(_uint iNumBone, _float fTickPerSecond, const vector<CBone*>& Bones)
{
	CLerpAnim* pInstance = new CLerpAnim;
	if (FAILED(pInstance->Initialize(iNumBone, fTickPerSecond, Bones))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CLerpAnim::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels) {
		SAFE_RELEASE(pChannel);
	}
	m_Channels.clear();

	m_CurrentKeyFrameIndices.clear();
}
;