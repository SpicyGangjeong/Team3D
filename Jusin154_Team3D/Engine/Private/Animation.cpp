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
	, m_iBoneCount{rhs.m_iBoneCount}
	, m_fProgress{rhs.m_fProgress }
	, m_fDurationSeconds{ rhs.m_fDurationSeconds }
	, m_fTempTrack{rhs.m_fTempTrack }
{
	memcpy_s(m_TickPerSeconds, sizeof(_float) * 2, rhs.m_TickPerSeconds, sizeof(_float) * 2);
	
	for (auto& pChannel : m_Channels) {
		SAFE_ADDREF(pChannel);
	}
}
_bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, LOCALPOS_DESC** pLocalPosArray, _bool bIsLoop, _float fTimeDelta, _bool bIsSpine, vector<_uint> BoneMask,_vector vector[3])
{
	m_fTempTrack += fTimeDelta * m_fAnimSpeed;
	m_fProgress = m_fTickPerSecond * fTimeDelta * m_fAnimSpeed;
	m_fCurrentTrackPosition += m_fProgress;

	if (m_fCurrentTrackPosition >= m_fDuration) {
		if (false == bIsLoop) {
			m_fCurrentTrackPosition = 0.f;
			m_fTempTrack = 0.f;
			return true;
		}
		else {
			Depart_Animation();
			m_fCurrentTrackPosition = m_fProgress;
			m_fTempTrack = 0.f;
			ResetRootMotion();
			ProgressAnimation(Bones, pLocalPosArray, bIsSpine, BoneMask, vector);
			return true;
		}
	}

	ProgressAnimation(Bones, pLocalPosArray, bIsSpine, BoneMask, vector);

	return false;
}

void CAnimation::ResetRootMotion()
{
	for (auto& pChannel : m_Channels){
		pChannel->ResetRootMotion();
	}
}

void CAnimation::Depart_Animation()
{
	m_fCurrentTrackPosition = 0.f;
	m_fTempTrack = 0.f;
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

void CAnimation::InterpAnim(CAnimation* pPreAnim, vector<CBone*>& Bones, _float fRatio)
{
	if (!pPreAnim)
		return;

	if (m_Channels.empty() || pPreAnim->m_Channels.empty())
		return;

	if (Bones.empty())
		return;

	if (fRatio <= 0.f)
		fRatio = 0.f;
	else if (fRatio >= 1.f)
		fRatio = 1.f;

	_uint boneCount = (_uint)Bones.size();

	vector<_matrix> preBoneMats(boneCount);
	vector<_bool>    preHasBone(boneCount, false);

	_uint preChannelCount =(_uint)pPreAnim->m_Channels.size();
	_uint preMatCount =(_uint)pPreAnim->m_vBoneTransformationMatrix.size();

	for (_uint i = 0; i < preChannelCount; ++i)
	{
		_uint boneIndex = pPreAnim->m_Channels[i]->Get_BoneIndex();
		if (boneIndex < 0 || boneIndex >= boneCount)
			continue;

		if (i >= preMatCount)
			continue;

		preBoneMats[boneIndex] = pPreAnim->m_vBoneTransformationMatrix[i];
		preHasBone[boneIndex] = true;
	}

	if (m_vBoneTransformationMatrix.size() < m_Channels.size())
		m_vBoneTransformationMatrix.resize(m_Channels.size(), XMMatrixIdentity());

	_bool hasSaveAnim = (m_pSaveAnim != nullptr);

	_uint curChannelCount =(_uint)m_Channels.size();
	_uint curMatCount = (_uint)m_vBoneTransformationMatrix.size();

	for (_uint i = 0; i < curChannelCount; ++i)
	{
		_uint boneIndex = m_Channels[i]->Get_BoneIndex();
		if (boneIndex < 0 || boneIndex >= (boneCount))
			continue;

		if (i >= curMatCount)
			continue;

		_matrix curMat = m_vBoneTransformationMatrix[i];

		if (!preHasBone[boneIndex] || fRatio <= 0.f)
		{
			Bones[boneIndex]->Set_TransformationMatrix(curMat);
			continue;
		}

		if (fRatio >= 1.f)
		{
			Bones[boneIndex]->Set_TransformationMatrix(curMat);
			continue;
		}

		_matrix preMat = preBoneMats[boneIndex];

		_vector curS, curR, curT;
		_vector preS, preR, preT;

		XMMatrixDecompose(&curS, &curR, &curT, curMat);
		XMMatrixDecompose(&preS, &preR, &preT, preMat);

		_vector finalS = XMVectorLerp(preS, curS, fRatio);
		_vector finalR = XMQuaternionSlerp(preR, curR, fRatio);
		_vector finalT = XMVectorLerp(preT, curT, fRatio);

		if (hasSaveAnim)
		{
			_string& boneName = m_pSaveAnim->Channels[i].ChannelName;
			if (boneName == "Reference")
			{
				finalT = XMVectorZero();

			}
		}

		_matrix finalMat = XMMatrixAffineTransformation(finalS, XMVectorZero(), finalR, finalT);

		Bones[boneIndex]->Set_TransformationMatrix(finalMat);

		m_vBoneTransformationMatrix[i] = finalMat;
	}

}


void CAnimation::InterpSecondAnim(CAnimation* pPreAnim, vector<_uint> BoneMask,vector<CBone*>& Bones, _float fRatio)
{
	if (!pPreAnim)
		return;

	if (m_Channels.empty() || pPreAnim->m_Channels.empty())
		return;

	if (Bones.empty())
		return;

	if (fRatio <= 0.f)
		fRatio = 0.f;
	else if (fRatio >= 1.f)
		fRatio = 1.f;

	_uint boneCount = (_uint)Bones.size();

	vector<_matrix> preBoneMats(boneCount);
	vector<_bool>    preHasBone(boneCount, false);

	_uint preChannelCount = (_uint)pPreAnim->m_Channels.size();
	_uint preMatCount = (_uint)pPreAnim->m_vBoneTransformationMatrix.size();

	for (_uint i = 0; i < preChannelCount; ++i)
	{
		_uint boneIndex = pPreAnim->m_Channels[i]->Get_BoneIndex();
		if (boneIndex < 0 || boneIndex >= boneCount)
			continue;

		if (i >= preMatCount)
			continue;

		preBoneMats[boneIndex] = pPreAnim->m_vBoneTransformationMatrix[i];
		preHasBone[boneIndex] = true;
	}

	if (m_vBoneTransformationMatrix.size() < m_Channels.size())
		m_vBoneTransformationMatrix.resize(m_Channels.size(), XMMatrixIdentity());

	_bool hasSaveAnim = (m_pSaveAnim != nullptr);

	_uint curChannelCount = (_uint)m_Channels.size();
	_uint curMatCount = (_uint)m_vBoneTransformationMatrix.size();

	for (_uint i = 0; i < curChannelCount; ++i)
	{
		_uint boneIndex = m_Channels[i]->Get_BoneIndex();
		if (boneIndex < 0 || boneIndex >= (boneCount))
			continue;

		if (i >= curMatCount)
			continue;

		_matrix curMat = m_vBoneTransformationMatrix[i];

		_bool isUpperBone = BoneMask[boneIndex];

		if (!isUpperBone)
		{
			if (preHasBone[boneIndex])
				Bones[boneIndex]->Set_TransformationMatrix(preBoneMats[boneIndex]);
			else
				Bones[boneIndex]->Set_TransformationMatrix(curMat);

			continue;
		}


		if (!preHasBone[boneIndex] || fRatio <= 0.f)
		{
			Bones[boneIndex]->Set_TransformationMatrix(curMat);
			continue;
		}

		if (fRatio >= 1.f)
		{
			Bones[boneIndex]->Set_TransformationMatrix(curMat);
			continue;
		}

		_matrix preMat = preBoneMats[boneIndex];

		_vector curS, curR, curT;
		_vector preS, preR, preT;

		XMMatrixDecompose(&curS, &curR, &curT, curMat);
		XMMatrixDecompose(&preS, &preR, &preT, preMat);

		_vector finalS = XMVectorLerp(preS, curS, fRatio);
		_vector finalR = XMQuaternionSlerp(preR, curR, fRatio);
		_vector finalT = XMVectorLerp(preT, curT, fRatio);

		if (hasSaveAnim)
		{
			_string& boneName = m_pSaveAnim->Channels[i].ChannelName;
			if (boneName == "Reference")
			{
				finalT = XMVectorZero();
			}
		}

		_matrix finalMat = XMMatrixAffineTransformation(finalS, XMVectorZero(), finalR, finalT);

		Bones[boneIndex]->Set_TransformationMatrix(finalMat);

		m_vBoneTransformationMatrix[i] = finalMat;
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
	size_t pos = m_strName.find_last_of('|');
	if (0 > pos) {
		pos = 0;
	}
	else {
		pos++;
	}
	m_strName = m_strName.substr(pos);
	m_strName.shrink_to_fit();

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_TickPerSeconds[0] = (_float)pAIAnimation->mTicksPerSecond;
	m_iNumChannels = pAIAnimation->mNumChannels;
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	m_fDurationSeconds = m_fDuration / m_fTickPerSecond;

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
	m_strName = pSaveAnimation->AnimName;
	m_strName.shrink_to_fit();
	m_fDuration = pSaveAnimation->mDuration;
	m_fTickPerSecond = pSaveAnimation->mTicksPerSecond;

	m_iNumChannels = pSaveAnimation->ChannelCount;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	m_iBoneCount = (_uint)Bones.size();

	m_fDurationSeconds = m_fDuration / m_fTickPerSecond;

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pModel, &pSaveAnimation->Channels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::ProgressAnimation(const vector<CBone*>& Bones, LOCALPOS_DESC** pLocalPosArray, _bool bIsSpine,vector<_uint>BoneMask,_vector vector[3])
{
	_uint iIndex = {};
	if (m_vBoneTransformationMatrix.size() > 0) {
		m_vBoneTransformationMatrix.clear();
	}
	for (auto& pChannel : m_Channels) {
		pChannel->Update_TransformationMatirx(Bones, pLocalPosArray, m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[iIndex++], bIsSpine, BoneMask, vector);
		m_vBoneTransformationMatrix.push_back(pChannel->Get_BoneTransformationMatrix());
	}
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
