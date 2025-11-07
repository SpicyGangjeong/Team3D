#include "pch.h"
#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const CModel* pModel, const aiNodeAnim* pAIChannel)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	m_iBoneIndex = pModel->Get_BoneIndex(m_szName);
	if (-1 == m_iBoneIndex)
		return E_FAIL;

	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	_float3				vScale{};
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

void CChannel::Update_TransformationMatrix(_float fCurrentTrackPosition, vector<CBone*>& Bones, _uint* pCurrentKeyFrameIndex)
{
	if (0.f == fCurrentTrackPosition)
		*pCurrentKeyFrameIndex = 0;

	KEYFRAME		LastKeyFrame = m_KeyFrames.back();

	_vector			vScale{};
	_vector			vRotation{};
	_vector			vTranslation{};

	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}

	else
	{
		while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
			++*pCurrentKeyFrameIndex;

		_float3		vSourScale{}, vDestScale{};
		_float4		vSourRotation{}, vDestRotation{};
		_float3		vSourTranslation{}, vDestTranslation{};

		vSourScale = m_KeyFrames[*pCurrentKeyFrameIndex].vScale;
		vDestScale = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale;

		vSourRotation = m_KeyFrames[*pCurrentKeyFrameIndex].vRotation;
		vDestRotation = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation;

		vSourTranslation = m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation;
		vDestTranslation = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation;

		_float fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) /
			(m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&vSourTranslation), XMLoadFloat3(&vDestTranslation), fRatio), 1.f);

	}

	m_BoneTransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_Transformation(m_BoneTransformationMatrix);


}

CChannel* CChannel::Create(const CModel* pModel, const aiNodeAnim* pAIChannel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pModel, pAIChannel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	__super::Free();

	m_KeyFrames.clear();

}
