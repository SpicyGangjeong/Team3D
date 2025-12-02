#include "pch.h"
#include "Bone.h"

CBone::CBone()
{
}

CBone::CBone(const CBone& other):
	CBase()
{
	m_strName = other.m_strName;
	m_TransformationMatrix = other.m_TransformationMatrix;
	m_CombinedTransformationMatrix = other.m_CombinedTransformationMatrix;
	m_iParentBoneIndex = other.m_iParentBoneIndex;
	m_matInitial = other.m_matInitial;
}

void CBone::Get_KeyFrame(KEYFRAME& kf, _bool bIsCurrentFrame)
{
	_vector vScale = {};
	_vector vRotation = {};
	_vector vTranspose = {};
	if (bIsCurrentFrame) {
		XMMatrixDecompose(&vScale, &vRotation, &vTranspose, XMLoadFloat4x4(&m_TransformationMatrix));
	}
	else {
		XMMatrixDecompose(&vScale, &vRotation, &vTranspose, XMLoadFloat4x4(&m_matInitial));
	}
	XMStoreFloat3(&kf.vScale, vScale);
	XMStoreFloat4(&kf.vRotation, vRotation);
	XMStoreFloat3(&kf.vTranslation, vTranspose);
	kf.fTrackPosition = 0.f;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentBoneIndex) {
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);
	}
	else {
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());
	}
}
void CBone::Set_TransformationMatrix(_fmatrix TransformationMatrix)
{
	XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
}
_float3 CBone::Get_LocalPosition()
{
	_float3 vPos = { m_TransformationMatrix.m[3][0], m_TransformationMatrix.m[3][1], m_TransformationMatrix.m[3][2] };
	return vPos;
}
#ifdef EDITOR_PROJECT
HRESULT CBone::SaveAsBinary(HANDLE hFile, DWORD& dwByte)
{
	m_strName.shrink_to_fit();
	DWORD iLength = (DWORD)m_strName.length();
	WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, m_strName.data(), iLength, &dwByte, nullptr);
	WriteFile(hFile, &m_iParentBoneIndex, sizeof(_int), &dwByte, nullptr);
	WriteFile(hFile, &m_TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);
	WriteFile(hFile, &m_matInitial, sizeof(_float4x4), &dwByte, nullptr);

	return S_OK;
}
CBone* CBone::Create(const aiNode* pAINode, _int iParentIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, iParentIndex)))
	{
		MSG_BOX("Failed to Created : CBone");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


HRESULT CBone::Initialize(const aiNode* pAINode, _int iParentIndex)
{
	m_strName = pAINode->mName.data;
	m_strName.shrink_to_fit();
	m_iParentBoneIndex = iParentIndex;

	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	m_matInitial = m_TransformationMatrix;
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}
#endif

CBone* CBone::Create(const SaveNode& _SaveNode, _int iParentIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(_SaveNode, iParentIndex)))
	{
		MSG_BOX("Failed to Created : CBone");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

HRESULT CBone::Initialize(HANDLE hFile, DWORD& dwByte)
{
	_uint iLength = { 0 };
	if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr)) {
		return E_FAIL;
	}
	m_strName.resize(iLength);
	if (!ReadFile(hFile, m_strName.data(), iLength, &dwByte, nullptr)) {
		return E_FAIL;
	}
	if (!ReadFile(hFile, &m_iParentBoneIndex, sizeof(_int), &dwByte, nullptr)) {
		return E_FAIL;
	}
	if (!ReadFile(hFile, &m_TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr)) {
		return E_FAIL;
	}
	if (!ReadFile(hFile, &m_matInitial, sizeof(_float4x4), &dwByte, nullptr)) {
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CBone::Initialize(const SaveNode& _SaveNode, _int iParentIndex)
{
	m_strName = _SaveNode.NodeName;
	m_iParentBoneIndex = iParentIndex;

	memcpy(&m_TransformationMatrix, &_SaveNode.Transformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

CBone* CBone::Create(HANDLE hFile, DWORD& dwByte)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(hFile, dwByte)))
	{
		MSG_BOX("Failed to Created : CBone");
		SAFE_RELEASE(pInstance);
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
