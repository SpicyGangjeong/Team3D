#include "pch.h"
#include "Animation.h"
#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
	: m_fCurrentTrackPosition{ Prototype.m_fCurrentTrackPosition }
	, m_fDuration{ Prototype.m_fDuration }
	, m_fTickPerSecond{ Prototype.m_fTickPerSecond }
	, m_iNumChannels{ Prototype.m_iNumChannels }
	, m_Channels{ Prototype.m_Channels }
	, m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }
	, m_pAIAnimation{ Prototype.m_pAIAnimation }
{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);

}


HRESULT CAnimation::Initialize(const CModel* pModel, const aiAnimation* pAIAnimation)
{
	m_pAIAnimation = pAIAnimation;
	m_fDuration = pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	m_iNumChannels = pAIAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(pModel, pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(_float fTimeDelta, _bool isLoop, vector<CBone*>& Bones)
{
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		if (isLoop)
			m_fCurrentTrackPosition = 0.f;
		else
			return true;
	}

	_uint		iIndex = {};
	m_vBoneTransformationMatrix.clear();

	for (auto& pChannel : m_Channels)
	{
		pChannel->Update_TransformationMatrix(m_fCurrentTrackPosition, Bones, &m_CurrentKeyFrameIndices[iIndex++]);
		m_vBoneTransformationMatrix.push_back(pChannel->Get_BoneTransformationMatrix());
	}
	return false;
}

void CAnimation::InterpAnim(CAnimation* pPreAnim, vector<CBone*>& Bones, float fRatio)
{

	for (size_t i = 0; i < m_Channels.size(); i++)
	{
		if (m_vBoneTransformationMatrix.size() > pPreAnim->m_vBoneTransformationMatrix.size())
		{
			for (size_t j = 0; j < pPreAnim->m_Channels.size(); j++)
			{
				if (strcmp(m_pAIAnimation->mChannels[i]->mNodeName.C_Str(), pPreAnim->m_pAIAnimation->mChannels[j]->mNodeName.C_Str()) == 0)
				{
					break;
				}
				else
				{
					if (j == pPreAnim->m_Channels.size() - 1)
					{
						pPreAnim->m_vBoneTransformationMatrix.push_back(m_vBoneTransformationMatrix[i]);
						break;
					}
				}
			}
		}
	}
	for (size_t i = 0; i < m_Channels.size(); i++)
	{
		for (size_t j = 0; j < pPreAnim->m_pAIAnimation->mNumChannels; j++)
		{
			if (strcmp(m_pAIAnimation->mChannels[i]->mNodeName.C_Str(), pPreAnim->m_pAIAnimation->mChannels[j]->mNodeName.C_Str()) == 0)
			{
				_matrix CurMat = m_vBoneTransformationMatrix[i];
				_matrix PreMat = pPreAnim->m_vBoneTransformationMatrix[j];

				_vector CurScale, CurRot, CurPos;
				_vector PreScale, PreRot, PrePos;
				XMMatrixDecompose(&CurScale, &CurRot, &CurPos, CurMat);
				XMMatrixDecompose(&PreScale, &PreRot, &PrePos, PreMat);

				_vector FinalScale = XMVectorLerp(PreScale, CurScale, fRatio);
				_vector FinalRot = XMQuaternionSlerp(PreRot, CurRot, fRatio);
				_vector FinalPos = XMVectorLerp(PrePos, CurPos, fRatio);

				m_vBoneTransformationMatrix[i] = XMMatrixAffineTransformation(FinalScale, XMVectorZero(), FinalRot, FinalPos);
				_int BoneIndex = m_Channels[i]->Get_BoneIndex();
				if (BoneIndex < Bones.size())
				{
					Bones[BoneIndex]->Set_Transformation(m_vBoneTransformationMatrix[i]);
				}
			}
		}
	}
}

CAnimation* CAnimation::Create(const CModel* pModel, const aiAnimation* pAIAnimation)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pModel, pAIAnimation)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
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

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();

	m_CurrentKeyFrameIndices.clear();
}
