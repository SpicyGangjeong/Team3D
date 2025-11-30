#include "pch.h"

#include "Channel.h"
#include "Model.h"
#include "Bone.h"
#include "Transform.h"

CChannel::CChannel()
{
}
#ifdef EDITOR_PROJECT
HRESULT CChannel::Initialize(const vector<CBone*>& Bones, const aiNodeAnim* pAIChannel)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	m_iBoneIndex = CModel::Get_BoneIndex(m_szName, Bones);
	if (-1 == m_iBoneIndex) {
		return E_FAIL;
	}

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, (_uint)pAIChannel->mNumPositionKeys);

	_float3				vScale = { };
	_float4				vRotation{};
	_float3				vTranslation{};
	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME		KeyFrame{};

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

			KeyFrame.fTrackPosition = (_float)pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = (_float)pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

CChannel* CChannel::Create(const vector<CBone*>& Bones, const aiNodeAnim* pAIChannel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(Bones, pAIChannel))) {
		MSG_BOX("Failed to Created : CChannel");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

#endif

void CChannel::Fill_GPU_Keyframes(vector<KEYFRAME_DESC>& outKeyframes)
{
	for (_uint i = 0; i < m_iNumKeyFrames; i++)
	{
		const KEYFRAME& src = m_KeyFrames[i];

		KEYFRAME_DESC dst{};
		dst.vScale = src.vScale;
		dst.vRotation = src.vRotation;
		dst.vTranslation = src.vTranslation;
		dst.fTrackPosition = src.fTrackPosition;

		outKeyframes.push_back(dst);
	}
}

CHANNEL_DESC CChannel::Fill_GPU_ChannelDesc()
{
	CHANNEL_DESC desc{};
	desc.StartIndex = 0;
	desc.KeyCount = m_iNumKeyFrames;
	desc.BoneIndex = m_iBoneIndex;

	return desc;
}

HRESULT CChannel::Initialize(const vector<CBone*>& Bones, _uint iIndex)
{
	m_iBoneIndex = iIndex;
	memcpy_s(m_szName, sizeof(_char) * MAX_PATH, Bones[iIndex]->Get_Name(), sizeof(_char) * MAX_PATH);

	m_iNumKeyFrames = 2;
	m_KeyFrames.resize(m_iNumKeyFrames);

	return S_OK;
}


void CChannel::Update_TransformationMatirx(
	const vector<CBone*>& Bones,
	const LOCALPOS_DESC* pLocalPosArray,
	_float fCurrentTrackPosition,
	_uint* pCurrentKeyFrameIndex,
	_vector vector[3])
{
	const LOCALPOS_DESC& LocalPos = pLocalPosArray[m_iBoneIndex];

	_vector vScale = XMLoadFloat3(&LocalPos.Scale);
	_vector vRotation = XMLoadFloat4(&LocalPos.Rotation);
	_vector vTranslation = XMVectorSet(
		LocalPos.Translation.x,
		LocalPos.Translation.y,
		LocalPos.Translation.z,
		1.f);

	if (Bones[m_iBoneIndex]->Compare_Name("Reference"))
	{
		vector[0] = vScale;
		vector[1] = vRotation;
		vector[2] = vTranslation;
	}
	
	m_BoneTransformationMatrix =
		XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(m_BoneTransformationMatrix);
}

void CChannel::ResetRootMotion()
{
	m_vPrevRootPos = { 0.f, 0.f, 0.f };
	m_vPrevRootRot = { 0.f,0.f,0.f,0.f };
	m_bInitialRootRotSaved = false;
	m_bInitialRootPos = false;
}


KEYFRAME* CChannel::Get_Frame(_uint iIndex)
{
	return &m_KeyFrames[iIndex];
}

void CChannel::Set_Frame(_uint iIndex, KEYFRAME& kf)
{
	m_KeyFrames[iIndex] = kf;
}

HRESULT CChannel::Initialize(const CModel* pModel, SaveChannel* pSaveChannel)
{
	strcpy_s(m_szName, pSaveChannel->ChannelName.c_str());

	m_iBoneIndex = pModel->Get_BoneIndex(m_szName);
	m_PreTransformMatrix = pModel->Get_PreTransformMatrix();

	m_iNumKeyFrames = max(pSaveChannel->ScalingKeyCount, pSaveChannel->RotationKeyCount);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pSaveChannel->PositionKeyCount);

	_float3				vScale{};
	_float4				vRotation{};
	_float3				vTranslation{};

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME		KeyFrame{};

		if (i < pSaveChannel->ScalingKeyCount)
		{
			memcpy(&vScale, &pSaveChannel->ScalingKeys[i].Value, sizeof(_float3));
			KeyFrame.fTrackPosition = pSaveChannel->ScalingKeys[i].Time;
		}

		if (i < pSaveChannel->RotationKeyCount)
		{
			vRotation.x = pSaveChannel->RotationKeys[i].Value.x;
			vRotation.y = pSaveChannel->RotationKeys[i].Value.y;
			vRotation.z = pSaveChannel->RotationKeys[i].Value.z;
			vRotation.w = pSaveChannel->RotationKeys[i].Value.w;

			KeyFrame.fTrackPosition = pSaveChannel->RotationKeys[i].Time;
		}

		if (i < pSaveChannel->PositionKeyCount)
		{
			memcpy(&vTranslation, &pSaveChannel->PositionKeys[i].Value, sizeof(_float3));
			KeyFrame.fTrackPosition = pSaveChannel->PositionKeys[i].Time;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;


		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

CChannel* CChannel::Create(const CModel* pModel, SaveChannel* pSaveChannel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pModel, pSaveChannel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	__super::Free();

	m_KeyFrames.clear();

	m_KeyFrameDesc.clear();
}
