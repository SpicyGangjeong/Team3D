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

HRESULT CChannel::SaveAsBinary(HANDLE hFile, DWORD& dwByte)
{
	string strName = m_szName;
	strName.shrink_to_fit();
	DWORD iLength = (DWORD)strName.length();
	WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, strName.data(), iLength, &dwByte, nullptr);
	WriteFile(hFile, &m_iBoneIndex, sizeof(_int), &dwByte, nullptr);
	WriteFile(hFile, &m_iNumKeyFrames, sizeof(_uint), &dwByte, nullptr);
	for (auto& keyFrame : m_KeyFrames) {
		WriteFile(hFile, &keyFrame, sizeof(KEYFRAME), &dwByte, nullptr);
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

HRESULT CChannel::Initialize(const vector<CBone*>& Bones, _uint iIndex)
{
	m_iBoneIndex = iIndex;
	memcpy_s(m_szName, sizeof(_char) * MAX_PATH, Bones[iIndex]->Get_Name(), sizeof(_char) * MAX_PATH);

	m_iNumKeyFrames = 2;
	m_KeyFrames.resize(m_iNumKeyFrames);

	return S_OK;
}


void CChannel::Update_TransformationMatirx(const vector<CBone*>& Bones, _float fCurrentTrackPosition, _uint* pCurrentKeyFrameIndex, CTransform* pTransform)

{
	if (0.f == fCurrentTrackPosition) {
		*pCurrentKeyFrameIndex = 0;
	}
	KEYFRAME LastKeyFrame = m_KeyFrames.back();

	_vector vScale = {};
	_vector vRotation = {};
	_vector vTranslation = {};

	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vTranslation = XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f);
	}
	else {
		while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition) {
			++*pCurrentKeyFrameIndex;
		}

		_float3 vSrcScale = {}, vDstScale = {};
		_float4 vSrcRotation = {}, vDstRotation = {};
		_float3 vSrcTranslation = {}, vDstTranslation = {};

		vSrcScale = m_KeyFrames[*pCurrentKeyFrameIndex].vScale;
		vDstScale = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale;

		vSrcRotation = m_KeyFrames[*pCurrentKeyFrameIndex].vRotation;
		vDstRotation = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation;

		vSrcTranslation = m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation;
		vDstTranslation = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation;

		_float fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) /
			(m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&vSrcScale), XMLoadFloat3(&vDstScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSrcRotation), XMLoadFloat4(&vDstRotation), fRatio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&vSrcTranslation), XMLoadFloat3(&vDstTranslation), fRatio), 1.f);
	}
	
	if (Bones[m_iBoneIndex]->Compare_Name("Reference") && pTransform != nullptr)
	{
		_float3 vCurRootPos;
		XMStoreFloat3(&vCurRootPos, vTranslation);

		_vector vDeltaLocal = XMLoadFloat3(&vCurRootPos) - XMLoadFloat3(&m_vPrevRootPos);

		_vector vDeltaAdjusted = {};

		XMMATRIX pre = XMLoadFloat4x4(&m_PreTransformMatrix);
		vDeltaAdjusted = XMVector3TransformNormal(vDeltaLocal, pre);

		_vector vRight = pTransform->Get_State(STATE::RIGHT);
		_vector vUp = pTransform->Get_State(STATE::UP);
		_vector vLook = pTransform->Get_State(STATE::LOOK);

		_float dx = XMVectorGetX(vDeltaAdjusted);
		_float dy = XMVectorGetY(vDeltaAdjusted);
		_float dz = XMVectorGetZ(vDeltaAdjusted);

		_vector vDeltaWorld = vRight * dx + vUp * dz + vLook * dy;

		_vector vCurrentPos = pTransform->Get_State(STATE::POSITION);
		_vector vTargetPos = vCurrentPos + vDeltaWorld;

		pTransform->Set_State(STATE::POSITION, vTargetPos);

		m_vPrevRootPos = vCurRootPos;
		vTranslation = XMVectorZero();
	}

	_matrix BoneTransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation);

	Bones[m_iBoneIndex]->Set_TransformationMatrix(BoneTransformationMatrix);
}


KEYFRAME* CChannel::Get_Frame(_uint iIndex)
{
	return &m_KeyFrames[iIndex];
}

void CChannel::Set_Frame(_uint iIndex, KEYFRAME& kf)
{
	m_KeyFrames[iIndex] = kf;
}


HRESULT CChannel::Initialize(HANDLE hFile, DWORD& dwByte)
{
	_uint iLength = { 0 };
	if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr)) {
		return E_FAIL;
	}
	if (!ReadFile(hFile, m_szName, iLength, &dwByte, nullptr)) {
		return E_FAIL;
	}
	if (!ReadFile(hFile, &m_iBoneIndex, sizeof(_int), &dwByte, nullptr)) {
		return E_FAIL;
	}
	if (!ReadFile(hFile, &m_iNumKeyFrames, sizeof(_uint), &dwByte, nullptr)) {
		return E_FAIL;
	}
	m_KeyFrames.resize(m_iNumKeyFrames);
	for (_uint i = 0; i < m_iNumKeyFrames; ++i) {
		if (!ReadFile(hFile, &m_KeyFrames[i], sizeof(KEYFRAME), &dwByte, nullptr)) {
			return E_FAIL;
		}
	}
	return S_OK;
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

CChannel* CChannel::Create(const vector<CBone*>& Bones, _uint iIndex)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(Bones, iIndex)))
	{
		MSG_BOX("Failed to Created : CChannel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CChannel* CChannel::Create(HANDLE hFile, DWORD& dwByte)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(hFile, dwByte)))
	{
		MSG_BOX("Failed to Created : CChannel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CChannel* CChannel::Create(const CModel* pModel, SaveChannel* pSaveChannel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pModel, pSaveChannel)))
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
