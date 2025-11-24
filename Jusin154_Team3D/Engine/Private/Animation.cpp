#include "pch.h"

#include "Animation.h"
#include "Channel.h"
#include "Model.h"
#include "Transform.h"
#include "Bone.h"

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
	,m_iBoneCount{rhs.m_iBoneCount}
{
	memcpy_s(m_TickPerSeconds, sizeof(_float) * 2, rhs.m_TickPerSeconds, sizeof(_float) * 2);

	strcpy_s(m_szName, sizeof(m_szName), rhs.m_szName);

	for (auto& pChannel : m_Channels) {
		SAFE_ADDREF(pChannel);
	}
}
_bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, const LOCALPOS_DESC* pLocalPosArray, _bool bIsLoop, _float fTimeDelta,CTransform*pTransform)
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
	if (m_vBoneTransformationMatrix.size() > 0)
		m_vBoneTransformationMatrix.clear();
	for (auto& pChannel : m_Channels) {
		pChannel->Update_TransformationMatirx(Bones, pLocalPosArray, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iIndex++],pTransform);
		m_vBoneTransformationMatrix.push_back(pChannel->Get_BoneTransformationMatrix());
	}

	return false;
}

void CAnimation::ResetRootMotion()
{
	for (auto& pChannel : m_Channels)
		pChannel->ResetRootMotion();
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


void CAnimation::InterpAnim(CAnimation* pPreAnim, vector<CBone*>& Bones, float fRatio)
{
	if (!pPreAnim) return;
	if (m_Channels.empty() || pPreAnim->m_Channels.empty()) return;

	size_t CurCount = m_vBoneTransformationMatrix.size();
	size_t PreCount = pPreAnim->m_vBoneTransformationMatrix.size();

	if (CurCount == 0 || PreCount == 0)
		return;

	if (CurCount > PreCount)
	{
		pPreAnim->m_vBoneTransformationMatrix.resize(CurCount, XMMatrixIdentity());
	}
	else if (CurCount < PreCount)
	{
		m_vBoneTransformationMatrix.resize(PreCount, XMMatrixIdentity());
	}

	size_t Count = m_vBoneTransformationMatrix.size();

	unordered_map<_string, int> PreBoneMap;
	PreBoneMap.reserve(pPreAnim->m_Channels.size());

	for (size_t j = 0; j < pPreAnim->m_Channels.size(); j++)
	{
		PreBoneMap[pPreAnim->m_pSaveAnim->Channels[j].ChannelName] = (int)j;
	}

	for (size_t i = 0; i < m_Channels.size(); i++)
	{
		const _string& BoneName = m_pSaveAnim->Channels[i].ChannelName;

		auto iter = PreBoneMap.find(BoneName);
		if (iter == PreBoneMap.end())
		{
			_int BoneIndex = m_Channels[i]->Get_BoneIndex();
			if (BoneIndex < Bones.size())
				Bones[BoneIndex]->Set_TransformationMatrix(m_vBoneTransformationMatrix[i]);
			continue;
		}

		int PreIndex = iter->second;

		_matrix CurMat = m_vBoneTransformationMatrix[i];
		_matrix PreMat = pPreAnim->m_vBoneTransformationMatrix[PreIndex];

		_vector CurScale, CurRot, CurPos;
		_vector PreScale, PreRot, PrePos;
		XMMatrixDecompose(&CurScale, &CurRot, &CurPos, CurMat);
		XMMatrixDecompose(&PreScale, &PreRot, &PrePos, PreMat);

		_vector FinalScale = XMVectorLerp(PreScale, CurScale, fRatio);
		_vector FinalRot = XMQuaternionSlerp(PreRot, CurRot, fRatio);
		_vector FinalPos = XMVectorLerp(PrePos, CurPos, fRatio);

		if (BoneName == "Reference")
		{
			//FinalPos = XMVectorZero();
		}

		_matrix FinalMat = XMMatrixAffineTransformation(FinalScale, XMVectorZero(), FinalRot, FinalPos);

		int BoneIndex = m_Channels[i]->Get_BoneIndex();
		if (BoneIndex < Bones.size())
		{
			Bones[BoneIndex]->Set_TransformationMatrix(FinalMat);
		}

		m_vBoneTransformationMatrix[i] = FinalMat;
	}
}

void CAnimation::CreateGPUData(ID3D11Device* pDevice)
{
	vector<CHANNEL_DESC> channels;
	vector<KEYFRAME_DESC> keyframes;

	channels.resize(m_iBoneCount);

	for (auto& pChannel : m_Channels)
	{
		CHANNEL_DESC desc = pChannel->Fill_GPU_ChannelDesc();
		desc.StartIndex = (_uint)keyframes.size();

		pChannel->Fill_GPU_Keyframes(keyframes);

		channels[desc.BoneIndex] = desc;
	}

	m_iChannelCount = (_uint)channels.size();
	m_iKeyframeCount = (_uint)keyframes.size();

	CreateKeyFrameBuffer(pDevice, keyframes);
	CreateChannelBuffer(pDevice, channels);

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

CAnimation* CAnimation::Create(const vector<CBone*>& Bones, const aiAnimation* pAnimation)
{
	CAnimation* pInstance = new CAnimation;
	if (FAILED(pInstance->Initialize(Bones, pAnimation))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}
#endif


HRESULT CAnimation::Initialize(const vector<CBone*>& Bones, const CModel* pModel, SaveAnimation* pSaveAnimation)
{
	m_pSaveAnim = pSaveAnimation;

	strcpy_s(m_szName, pSaveAnimation->AnimName.c_str());
	m_fDuration = pSaveAnimation->mDuration;
	m_fTickPerSecond = pSaveAnimation->mTicksPerSecond;

	m_iNumChannels = pSaveAnimation->ChannelCount;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	m_iBoneCount = (_uint)Bones.size();

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

HRESULT CAnimation::CreateKeyFrameBuffer(ID3D11Device* pDevice,vector<KEYFRAME_DESC> keyframe)
{
	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = (_int)sizeof(KEYFRAME_DESC) * m_iKeyframeCount;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.StructureByteStride = (_int)sizeof(KEYFRAME_DESC);
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA init{};
	init.pSysMem = keyframe.data();

	pDevice->CreateBuffer(&desc, &init, &m_pKeyFrameBuffer);



	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = m_iKeyframeCount;

	if (FAILED(pDevice->CreateShaderResourceView(m_pKeyFrameBuffer, &srvDesc, &m_pKeyFrameSrv)))
		return E_FAIL;


	return S_OK;
}

HRESULT CAnimation::CreateChannelBuffer(ID3D11Device* pDevice, vector<CHANNEL_DESC> channel)
{
	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = (_int)sizeof(CHANNEL_DESC) * m_iChannelCount;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.StructureByteStride = (_int)sizeof(CHANNEL_DESC);
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA init{};
	init.pSysMem = channel.data();

	pDevice->CreateBuffer(&desc, &init, &m_pChannelBuffer);


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = m_iChannelCount;

	if (FAILED(pDevice->CreateShaderResourceView(m_pChannelBuffer, &srvDesc, &m_pChannelSrv)))
		return E_FAIL;

	return S_OK;
}

CAnimation* CAnimation::Create(const vector<CBone*>& Bones, const CModel* pModel, SaveAnimation* pSaveAnimation)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(Bones,pModel, pSaveAnimation)))
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

	m_KeyFrameDesc.clear();
	m_ChannelDesc.clear();

	if (m_pKeyFrameSrv&& m_pChannelSrv)
	{
		SAFE_RELEASE(m_pKeyFrameBuffer);
		SAFE_RELEASE(m_pChannelBuffer);
		SAFE_RELEASE(m_pKeyFrameSrv);
		SAFE_RELEASE(m_pChannelSrv);
	}

}
#ifdef _DEBUG

void CAnimation::Describe_Entity()
{
	//if (GUI::CollapsingHeader(m_strName.c_str())) {
	//	
	//}
}

#endif // _DEBUG
