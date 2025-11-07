#include "pch.h"
#include "Bone.h"

CBone::CBone()
{

}

HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentIndex)
{
	strcpy_s(m_szName, pAINode->mName.data);

	m_iParentBoneIndex = iParentIndex;

	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);

	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());
}

void CBone::Set_Transformation(_fmatrix BoneTransformationMatrix)
{
	XMStoreFloat4x4(&m_TransformationMatrix, BoneTransformationMatrix);
}

CBone* CBone::Create(const aiNode* pAINode, _int iParentIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, iParentIndex)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();
}
