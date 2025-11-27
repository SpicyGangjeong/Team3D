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
	CTransform* pTransform,_float m_fAmount)
{
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
		while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition){
			++*pCurrentKeyFrameIndex;
		}

		_float3		vSourScale{}, vDestScale{};
		_float4		vSourRotation{}, vDestRotation{};
		_float3		vSourTranslation{}, vDestTranslation{};

		vSourScale = m_KeyFrames[*pCurrentKeyFrameIndex].vScale;
		vDestScale = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale;

		vSourRotation = m_KeyFrames[*pCurrentKeyFrameIndex].vRotation;
		vDestRotation = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation;

		vSourTranslation = m_KeyFrames[*pCurrentKeyFrameIndex].vTranslation;
		vDestTranslation = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vTranslation;

		_float		fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) /
			(m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

		vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
		vTranslation = XMVectorSetW(XMVectorLerp(XMLoadFloat3(&vSourTranslation), XMLoadFloat3(&vDestTranslation), fRatio), 1.f);
	}


	//const LOCALPOS_DESC& LocalPos = pLocalPosArray[m_iBoneIndex];

	//_vector vScale = XMLoadFloat3(&LocalPos.Scale);
	//_vector vRotation = XMLoadFloat4(&LocalPos.Rotation);
	//_vector vTranslation = XMVectorSet(
	//	LocalPos.Translation.x,
	//	LocalPos.Translation.y,
	//	LocalPos.Translation.z,
	//	1.f);

	if (Bones[m_iBoneIndex]->Compare_Name("Reference") && pTransform != nullptr)
	{
	
		_float3 vCurRootPos;
		XMStoreFloat3(&vCurRootPos, vTranslation);

		_matrix pre = XMLoadFloat4x4(&m_PreTransformMatrix);
		
			_vector vDeltaLocal = XMLoadFloat3(&vCurRootPos) - XMLoadFloat3(&m_vPrevRootPos);
			_vector vDeltaAdjusted = XMVector3TransformNormal(vDeltaLocal, pre);

			_vector vRight = pTransform->Get_State(STATE::RIGHT);
			_vector vUp = pTransform->Get_State(STATE::UP);
			_vector vLook = pTransform->Get_State(STATE::LOOK);

			_float dx = XMVectorGetX(vDeltaAdjusted);
			_float dy = XMVectorGetY(vDeltaAdjusted);
			_float dz = XMVectorGetZ(vDeltaAdjusted);

			_vector vDeltaWorld = vRight * dx + (vUp * dz) + (-vLook * dy);
			vDeltaWorld *= 0.01f;
			vDeltaWorld *= m_fAmount;

			

			vDeltaWorld = XMVectorSetW(vDeltaWorld, 1.f);
			//pTransform->Set_State(STATE::POSITION,vDeltaWorld);
			pTransform->AccumulateMomentum(vDeltaWorld);

			m_vPrevRootPos = vCurRootPos;
			vTranslation = XMVectorZero();

			_float4 curRotF4;
		XMStoreFloat4(&curRotF4, vRotation);
		_vector qCur = XMLoadFloat4(&curRotF4);

		if (!m_bInitialRootRotSaved)
		{
			m_vInitialRootRot = curRotF4;
			m_vPrevRootRot = curRotF4;
			m_bInitialRootRotSaved = true;
		}
		else
		{
			_vector qPrev = XMLoadFloat4(&m_vPrevRootRot);
			_vector qInvPrev = XMQuaternionInverse(qPrev);
			_vector qDelta = XMQuaternionMultiply(qInvPrev, qCur);

			_vector axisLocal;
			_float angle = 0.f;
			XMQuaternionToAxisAngle(&axisLocal, &angle, qDelta);

			_vector axisWorld = XMVector3TransformNormal(axisLocal, pre);
			axisWorld = XMVector3Normalize(axisWorld);

			_float4 axis;
			XMStoreFloat4(&axis, axisWorld);

			swap(axis.z, axis.y);
			pTransform->TurnAngle(XMLoadFloat4(&axis), angle);
		}

		XMStoreFloat4(&m_vPrevRootRot, qCur);
		vRotation = XMLoadFloat4(&m_vInitialRootRot);
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
