#include "pch.h"
#include "Model.h"
#include "Bone.h"
#include "Mesh.h"
#include "Material.h"
#include "Animation.h"
#include "GameObject.h"
#include "Transform.h"
#include "ComputeShader.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs),
	m_eType(rhs.m_eType),
	m_PreTransformMatrix(rhs.m_PreTransformMatrix),
	m_iRootBoneIndex(rhs.m_iRootBoneIndex),
	m_Meshes(rhs.m_Meshes),
	m_iNumMeshes(rhs.m_iNumMeshes),
	m_Materials(rhs.m_Materials),
	m_iNumMaterials(rhs.m_iNumMaterials),
	m_iNumAnimations(rhs.m_iNumAnimations),
	m_fRadius(rhs.m_fRadius),
	m_vRadiusOffset(rhs.m_vRadiusOffset)
{
	_uint iNumBones = (_uint)(rhs.m_Bones.size());
	m_Bones.reserve(iNumBones);
	for (_uint i = 0; i < iNumBones; ++i) {
		m_Bones.push_back(rhs.m_Bones[i]->Clone());
	}
	for (auto& pMesh : m_Meshes) {
		SAFE_ADDREF(pMesh);
	}
	for (auto& pMat : m_Materials) {
		SAFE_ADDREF(pMat);
	}
	m_Animations.reserve(m_iNumAnimations);
	for (_uint i = 0; i < m_iNumAnimations; ++i) {
		m_Animations.push_back(rhs.m_Animations[i]->Clone()); // 멤버 전부  얕은복사.
	}
}

_int CModel::Get_BoneIndex(const _char* pBoneName, vector<class CBone*> Bones)
{
	_int iBoneIndex = {};
	vector<CBone*>::const_iterator iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)->_bool {
		if (true == pBone->Compare_Name(pBoneName)) {
			return true;
		}
		++iBoneIndex;
		return false;
		});
	if (iter == Bones.end()) {

		return -1;
	}

	return iBoneIndex;
}

_int CModel::Get_BoneIndex(const _char* pBoneName) const
{
	_int	iBoneIndex = {};

	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (true == pBone->Compare_Name(pBoneName))
				return true;

			++iBoneIndex;

			return false;
		});

	if (iter == m_Bones.end())
		return -1;

	return iBoneIndex;
}

HRESULT CModel::Bind_Material(_uint iMeshIndex, CShader* pShader)
{
	if (iMeshIndex >= m_iNumMeshes) {
		return E_FAIL;
	}

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials) {
		return E_FAIL;
	}
	return m_Materials[iMaterialIndex]->Bind_SRV(pShader);
}

HRESULT CModel::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pConstantName)
{
	if (iMeshIndex >= m_iNumMeshes) {
		return E_FAIL;
	}
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(m_Bones, pShader, pConstantName);
}

_bool CModel::Play_Animation(_float fTimeDelta, CTransform* pTransform)
{
	if (!m_bPlayAnim){
		return false;
	}

	if (m_iCurrentAnimIndex < 0 || m_iCurrentAnimIndex >= (_int)m_iNumAnimations){
		return false;
	}

	if (m_iCurrSecondAnimIndex >= 0) {
		Play_Dual_Anim(fTimeDelta, pTransform);
	}
	else {
		Play_Anim(fTimeDelta, pTransform);
	}

	return m_bIsFinishedAnim;
}

_bool CModel::Play_Anim(_float fTimeDelta, CTransform* pTransform)
{
	/*if (m_bLoopRestarted)
	{
		m_vPrevRootPos = { 0,0,0 };
		m_bLoopRestarted = false;
	}*/

	//ComputeAnimation(m_iCurrentAnimIndex);

	if (m_iPreAnimIndex >= 0 && m_iPreAnimIndex != m_iCurrentAnimIndex)
	{
		m_fBlendTime += fTimeDelta;
		m_fRatio = (m_fBlendTime / m_fBlendDuration);
		if (m_fRatio > 1.f) {
			m_fRatio = 1.f;
		}

		CAnimation* pCurAnim = m_Animations[m_iCurrentAnimIndex];
		CAnimation* pPreAnim = m_Animations[m_iPreAnimIndex];

		m_bIsFinishedAnim = pCurAnim->Update_TransformationMatrices(m_Bones, m_pLocalPos, m_bIsLoop, fTimeDelta, true, m_iBoneMask, m_vector);

		pCurAnim->InterpAnim(pPreAnim, m_Bones, m_fRatio);

		if (m_fRatio >= 1.f)
		{
			m_iPreAnimIndex = m_iCurrentAnimIndex;
			m_fBlendTime = 0.f;
		}
	}
	else
	{
		m_bIsFinishedAnim = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_pLocalPos, m_bIsLoop, fTimeDelta, true, m_iBoneMask, m_vector);

		m_iPreAnimIndex = m_iCurrentAnimIndex;
	}
	
	if (m_bIsFinishedAnim)
	{
		if (m_bIsLoop)
		{
			m_bLoopRestarted = true;
			m_vPrevRootPos = { 0,0,0 };
		}
		else{
			XMStoreFloat3(&m_vPrevRootPos, m_vector[2]);
		}
	}

	if (m_bRatio) {
		Update_RootBone(m_fAmount * m_fRatio);
	}
	else
	{
		Update_RootBone(m_fAmount);
	}

	if (m_bIsFinishedAnim)
	{
		m_vPrevRootRot = { 0.f,0.f,0.f,0.f };
		m_bInitialRootRotSaved = false;
	}

	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}

	return m_bIsFinishedAnim;
}

_bool CModel::Play_Dual_Anim(_float fTimeDelta, CTransform* pTransform)
{
	if (m_bLoopRestarted)
	{
		m_vPrevRootPos = { 0,0,0 };
		m_bLoopRestarted = false;
	}

	//ComputeAnimation(m_iCurrentAnimIndex);
	//ComputeAnimation_Second(m_iCurrSecondAnimIndex);

	if (m_iPreAnimIndex >= 0 && m_iPreAnimIndex != m_iCurrentAnimIndex)
	{
		m_fBlendTime += fTimeDelta;
		m_fRatio = (m_fBlendTime / m_fBlendDuration);
		if (m_fRatio > 1.f) m_fRatio = 1.f;

		m_fSecondBlendTime += fTimeDelta;
		m_fSecondRatio = (m_fSecondBlendTime / m_fSecondBlendDuration);
		if (m_fSecondRatio > 1.f) m_fSecondRatio = 1.f;

		CAnimation* pCurAnim = m_Animations[m_iCurrentAnimIndex];
		CAnimation* pPreAnim = m_Animations[m_iPreAnimIndex];

		CAnimation* pSecondAnim = m_Animations[m_iCurrSecondAnimIndex];

		m_bIsFinishedAnim = pCurAnim->Update_TransformationMatrices(m_Bones, m_pLocalPos, m_bIsLoop, fTimeDelta, true, m_iBoneMask, m_vector);
		pCurAnim->InterpAnim(pPreAnim, m_Bones, m_fRatio);

		m_bIsSecondFinishedAnim = pSecondAnim->Update_TransformationMatrices(m_Bones, m_pLocalPos, m_bIsSecondLoop, fTimeDelta, false, m_iBoneMask);

		pSecondAnim->InterpSecondAnim(pCurAnim, m_iBoneMask, m_Bones, m_fSecondRatio);
		if (m_fRatio >= 1.f)
		{
			m_iPreAnimIndex = m_iCurrentAnimIndex;
			m_fBlendTime = 0.f;
		}

		if (m_bIsSecondFinishedAnim)
		{
			if (m_bIsSecondLoop)
			{
				m_fSecondRatio = 1.f;
			}
			else{
				m_fSecondBlendTime = 0.f;
			}
		}
	}
	else
	{
		m_fSecondBlendTime += fTimeDelta;
		m_fSecondRatio = (m_fSecondBlendTime / m_fSecondBlendDuration);
		if (m_fSecondRatio > 1.f) m_fSecondRatio = 1.f;

		m_bIsFinishedAnim = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_pLocalPos, m_bIsLoop, fTimeDelta, true, m_iBoneMask, m_vector);

		m_bIsSecondFinishedAnim = m_Animations[m_iCurrSecondAnimIndex]->Update_TransformationMatrices(m_Bones, m_pLocalPos, m_bIsSecondLoop, fTimeDelta, false, m_iBoneMask);
		m_Animations[m_iCurrSecondAnimIndex]->InterpSecondAnim(m_Animations[m_iCurrentAnimIndex], m_iBoneMask, m_Bones, m_fSecondRatio);
		m_iPreAnimIndex = m_iCurrentAnimIndex;

		if (m_bIsSecondFinishedAnim)
		{
			if (m_bIsSecondLoop)
			{
				m_fSecondRatio = 1.f;
			}
			else{
				m_fSecondBlendTime = 0.f;
			}
		}
	}

	if (m_bIsFinishedAnim)
	{
		if (m_bIsLoop)
		{
			m_bLoopRestarted = true;
			m_vPrevRootPos = { 0,0,0 };
		}
		else {
			XMStoreFloat3(&m_vPrevRootPos, m_vector[2]);
		}
	}

	if (m_bRatio) {
		Update_RootBone(m_fAmount * m_fRatio);
	}
	else
	{
		Update_RootBone(m_fAmount);
	}

	if (m_bIsFinishedAnim)
	{
		m_vPrevRootRot = { 0.f,0.f,0.f,0.f };
		m_bInitialRootRotSaved = false;
	}

	if (!m_bIsSecondLoop && m_bIsSecondFinishedAnim)
	{
		m_iCurrSecondAnimIndex = -1;
	}

	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}

	return m_bIsFinishedAnim;
}

void CModel::Set_AnimationIndex(_uint iIndex, _bool isLoop, _float fAmount, _bool bRatio, _float fAnimSpeed)
{
	if (m_iCurrentAnimIndex == iIndex)
		return;

	if (iIndex >= 0 && iIndex < m_iNumAnimations)
	{
		m_vPrevRootPos = { 0.f, 0.f, 0.f };
		m_vPrevRootRot = { 0.f,0.f,0.f,0.f };
		m_bInitialRootRotSaved = false;
		m_bInitialRootPos = false;

		if (m_iCurrentAnimIndex != -1)
		{
			m_iPreAnimIndex = m_iCurrentAnimIndex;

			m_Animations[m_iCurrentAnimIndex]->Depart_Animation();
			m_Animations[m_iCurrentAnimIndex]->Set_AnimSpeed(1.f);
		}

		m_fBlendTime = 0.f;
		m_fRatio = 0.f;

		m_iCurrentAnimIndex = iIndex;
		m_bIsLoop = isLoop;
		m_fAmount = fAmount;
		m_bRatio = bRatio;

		m_Animations[m_iCurrentAnimIndex]->Depart_Animation();
		m_Animations[m_iCurrentAnimIndex]->Set_AnimSpeed(fAnimSpeed);
	}
	else
	{
		m_iCurrentAnimIndex = -1;
	}
}


void CModel::Set_Second_AnimationIndex(_uint iIndex, _uint BoneIndex, _bool isLoop)
{
	if (m_iCurrSecondAnimIndex == iIndex) {
		return;
	}
	if (iIndex >= 0 && iIndex < m_iNumAnimations)
	{
		m_iCurrSecondAnimIndex = iIndex;
		m_bIsSecondLoop = isLoop;
		m_iBoneMask = m_BoneMask[BoneIndex];
		m_fSecondBlendTime = 0.f;
	}
	else {
		m_iCurrSecondAnimIndex = -1;
	}
}

void CModel::Update_RootBone(_float Amount)
{
	if (m_Bones[m_iRootBoneIndex]->Compare_Name("Reference")&& 
		m_pTransform != nullptr)
	{
		_float4x4 Root = m_Bones[m_iRootBoneIndex]->Get_TransformationMatrix();
		_matrix local = XMLoadFloat4x4(&Root);
		_float3 vCurRootPos;
		XMStoreFloat3(&vCurRootPos, m_vector[2]);

		_matrix pre = XMLoadFloat4x4(&m_PreTransformMatrix);

		_vector vDeltaLocal = XMLoadFloat3(&vCurRootPos) - XMLoadFloat3(&m_vPrevRootPos);
		_vector vDeltaAdjusted = XMVector3TransformNormal(vDeltaLocal, pre);

		_vector vRight = m_pTransform->Get_State(STATE::RIGHT);
		_vector vUp = m_pTransform->Get_State(STATE::UP);
		_vector vLook = m_pTransform->Get_State(STATE::LOOK);

		_float dx = XMVectorGetX(vDeltaAdjusted);
		_float dy = XMVectorGetY(vDeltaAdjusted);
		_float dz = XMVectorGetZ(vDeltaAdjusted);

		_vector vDeltaWorld = vRight * dx + (vUp * dz) + (-vLook * dy);

		vDeltaWorld *= 0.01f;
		vDeltaWorld *= Amount;

		vDeltaWorld = XMVectorSetW(vDeltaWorld, 1.f);
		m_pTransform->AccumulateMomentum(vDeltaWorld);

		m_vPrevRootPos = vCurRootPos;

		_float4 curRotF4;
		XMStoreFloat4(&curRotF4, m_vector[1]);
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
			m_pTransform->TurnAngle(XMLoadFloat4(&axis), angle);

		}
		XMStoreFloat4(&m_vPrevRootRot, qCur);

		_vector qInit = XMLoadFloat4(&m_vInitialRootRot);

		_vector scale, rot, trans;
		XMMatrixDecompose(&scale, &rot, &trans, local);

		rot = qInit;
		trans = XMVectorZero();

		local = XMMatrixAffineTransformation(scale, XMVectorSet(0.f, 0.f, 0.f, 1.f), rot, trans);

		m_Bones[m_iRootBoneIndex]->Set_TransformationMatrix(local);
	}
	else if (m_Bones[m_iRootBoneIndex]->Compare_Name("root") &&
		m_pTransform != nullptr)
	{
		_float4x4 Root = m_Bones[m_iRootBoneIndex]->Get_TransformationMatrix();
		_matrix local = XMLoadFloat4x4(&Root);
	
		_matrix pre = XMLoadFloat4x4(&m_PreTransformMatrix);

		_float4 curRotF4;
		XMStoreFloat4(&curRotF4, m_vector[1]);
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
			m_pTransform->TurnAngle(XMLoadFloat4(&axis), angle);

		}
		XMStoreFloat4(&m_vPrevRootRot, qCur);

		_vector qInit = XMLoadFloat4(&m_vInitialRootRot);

		_vector scale, rot, trans;
		XMMatrixDecompose(&scale, &rot, &trans, local);

		rot = qInit;
		trans = XMVectorZero();

		local = XMMatrixAffineTransformation(scale, XMVectorSet(0.f, 0.f, 0.f, 1.f), rot, trans);

		m_Bones[m_iRootBoneIndex]->Set_TransformationMatrix(local);
	}
}

void CModel::Initialize_RootBone()
{
	for (_uint i = 0; i < (_uint)m_Bones.size(); i++)
	{
		if (m_Bones[i]->Compare_Name("Reference") || m_Bones[i]->Compare_Name("root"))
		{
			m_iRootBoneIndex = i;
		}
	}
}

const _float4x4* CModel::Get_BoneMatrixPtr(const _char* pBoneName) const
{
	vector<CBone*>::const_iterator	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (true == pBone->Compare_Name(pBoneName))
				return true;

			return false;
		});


	return (*iter)->Get_CombinedTransformationMatrixPtr();
}

const _char* CModel::Get_MeshName(_uint iIndex)
{
	return m_Meshes[iIndex]->Get_Name();
}


_matrix CModel::Get_BoneMatrix(_uint iBoneIndex)
{
	return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
}

void CModel::Combined_BoneMatrix()
{
	for (auto& pBone : m_Bones)
	{
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}
}


void CModel::Set_CurrentTrackPosition(_float TrackPosition)
{
	m_Animations[m_iCurrentAnimIndex]->Set_CurrentTrackPosition(TrackPosition);
}

const _char* CModel::Get_AnimList(_uint iIndex)
{
	return m_Animations[iIndex]->Get_Name().c_str();
}

_float CModel::Get_CurrentTrackPosition()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_CurrentTrackPosition();
}

_float CModel::Get_CurrentTrackProgressRatio()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_CurrentTrackProgressRatio();
}

_float CModel::Get_TrackProgressRatio(_uint iIndex)
{
	return m_Animations[iIndex]->Get_CurrentTrackProgressRatio();
}

_float CModel::Get_AnimSpeed()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_AnimSpeed();
}

void CModel::Set_AnimSpeed(_float fSpeed)
{
	m_Animations[m_iCurrentAnimIndex]->Set_AnimSpeed(fSpeed);
}

HRESULT CModel::Anim_Event(_float fRatio, _uint AnimIndex, function<void()> Event)
{
	if (AnimIndex == m_iCurrentAnimIndex)
	{
		if (m_Animations[m_iCurrentAnimIndex]->Get_CurrentTrackProgressRatio() >= fRatio)
		{
			Event();
		}
	}
	return E_FAIL;
}


HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Resources();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Render_Indexed(_uint iMeshIndex, _uint IndexCount, _uint StartIndexLocation, _uint BaseVertexLocation)
{
	m_Meshes[iMeshIndex]->Bind_Resources();
	m_Meshes[iMeshIndex]->Render_Indexed(IndexCount, StartIndexLocation, BaseVertexLocation);

	return S_OK;
}
HRESULT CModel::Ready_PhysXMeshes(_fmatrix& PreTransformMatrix)
{
	m_iNumPhysXMeshes = m_iNumMeshes;

	m_TriMeshes.reserve(m_iNumMeshes);

	m_pGameInstance->ConvertToTriMeshes(m_Meshes, m_TriMeshes, PreTransformMatrix);

	for (_uint i = 0; i < m_iNumMeshes; ++i) {
		m_pGameInstance->RegistTriMesh((m_Meshes[i]->Get_Name() + to_string(i)).c_str(), m_TriMeshes[i]);
	}
	return S_OK;
}
#ifdef EDITOR_PROJECT

HRESULT CModel::Ready_Meshes(MODEL eType, const aiScene* pAIScene, _fmatrix& PreTransformMatrix)
{
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	m_iNumMeshes = pAIScene->mNumMeshes;

	m_Meshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eType, m_Bones, pAIScene->mMeshes[i], PreTransformMatrix);
		if (nullptr == pMesh) {
			return E_FAIL;
		}

		m_Meshes.push_back(pMesh);
	}
	m_Meshes.shrink_to_fit();
	return S_OK;
}

HRESULT CModel::Save_PhysXTriMeshes(const _char* pModelFilePath)
{
	return m_pGameInstance->SaveTriMeshes(pModelFilePath, m_TriMeshes);
}

HRESULT CModel::Ready_Materials(const aiScene* pAIScene, const _char* pModelFilePath)
{
	m_iNumMaterials = pAIScene->mNumMaterials;
	m_Materials.reserve(m_iNumMaterials);
	for (size_t i = 0; i < m_iNumMaterials; ++i) {
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, pModelFilePath, pAIScene->mMaterials[i]);
		if (nullptr == pMaterial) {
			return E_FAIL;
		}
		m_Materials.push_back(pMaterial);
	}
	m_Materials.shrink_to_fit();
	return S_OK;
}

HRESULT CModel::Ready_Materials_FromFile(const aiScene* pAIScene, const _char* pModelFilePath)
{
	m_iNumMaterials = pAIScene->mNumMaterials;
	m_Materials.reserve(m_iNumMaterials);

	_char szDrive[MAX_PATH] = {};
	_char szDir[MAX_PATH] = {};
	_char szFileName[MAX_PATH] = {};
	_char szMeshFilePath[MAX_PATH] = {};
	vector<_string> MaterialFilePathes;
	string strFolderPath = pModelFilePath;

	size_t iFolderPos = strFolderPath.find("MeshTable");

	strFolderPath = strFolderPath.substr(0, iFolderPos) + "MeshTable/";

	_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

	strcpy_s(szMeshFilePath, szDrive);
	strcat_s(szMeshFilePath, szDir);
	strcat_s(szMeshFilePath, szFileName);
	strcat_s(szMeshFilePath, ".props.txt");

	ifstream file(szMeshFilePath);

	if (!file.is_open())
	{
		//MSG_BOX("Failed to Open Mesh File");
		///* LOD */
		return S_OK;
	}

	string strText = {};
	_uint iNumParameter = {};

	getline(file, strText);
	getline(file, strText);

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		getline(file, strText);

		_int iBeginIndex = (_int)strText.find_first_of("/");
		_int iEndIndex = (_int)strText.find('.');
		if (-1 == iBeginIndex || -1 == iEndIndex) {
			MSG_BOX("Fail Path _ NotExist"); // Not Exsist
			return E_FAIL;
		}
		if ((_int)strText.size() < iEndIndex - iBeginIndex)
		{
			MSG_BOX("Fail Path");
			return E_FAIL;
		}

		string	strPath = strFolderPath + strText.substr(iBeginIndex + 1, iEndIndex - iBeginIndex - 1);
		MaterialFilePathes.push_back(strPath);
	}

	for (size_t i = 0; i < m_iNumMaterials; ++i) {
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, MaterialFilePathes[i].c_str(), strFolderPath.c_str());
		if (nullptr == pMaterial) {
			return E_FAIL;
		}
		m_Materials.push_back(pMaterial);
	}
	m_Materials.shrink_to_fit();

	while (true)
	{
		getline(file, strText);
		if (string::npos != strText.find("Origin"))
		{
			size_t iXPos = strText.find_first_of("X");
			size_t iYPos = strText.find_first_of("Y");
			size_t iZPos = strText.find_first_of("Z");
			size_t iEndPos = strText.find_first_of("}");

			m_vRadiusOffset.x = stof(strText.substr(iXPos + 2, iYPos - 2)) * 0.01f;
			m_vRadiusOffset.z = stof(strText.substr(iYPos + 2, iZPos - 2)) * -0.01f;
			m_vRadiusOffset.y = stof(strText.substr(iZPos + 2, iEndPos)) * 0.01f;
		}
		if (string::npos != strText.find("SphereRadius"))
		{
			size_t iBeginIndex = strText.find_first_of("=");
			m_fRadius = stof(strText.substr(iBeginIndex + 1)) / 100.f;
			break;
		}
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials_FromFile_Anim(const aiScene* pAIScene, const _char* pModelFilePath)
{
	m_iNumMaterials = pAIScene->mNumMaterials;
	m_Materials.reserve(m_iNumMaterials);

	_char szDrive[MAX_PATH] = {};
	_char szDir[MAX_PATH] = {};
	_char szFileName[MAX_PATH] = {};
	_char szMeshFilePath[MAX_PATH] = {};
	vector<_string> MaterialFilePathes;
	string strFolderPath = pModelFilePath;

	size_t iFolderPos = strFolderPath.find("MeshTable");

	strFolderPath = strFolderPath.substr(0, iFolderPos) + "MeshTable/";

	_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

	strcpy_s(szMeshFilePath, szDrive);
	strcat_s(szMeshFilePath, szDir);
	strcat_s(szMeshFilePath, szFileName);
	strcat_s(szMeshFilePath, ".props.txt");

	ifstream file(szMeshFilePath);

	if (!file.is_open())
	{
		return S_OK;
	}

	string strText = {};
	_uint iNumParameter = {};

	getline(file, strText);
	getline(file, strText);

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		getline(file, strText);

		_int iBeginIndex = (_int)strText.find_first_of("/");
		_int iEndIndex = (_int)strText.find('.');
		if (-1 == iBeginIndex || -1 == iEndIndex) {

			MSG_BOX("Fail Path _ NotExist");
			return E_FAIL;
		}
		if ((_int)strText.size() < iEndIndex - iBeginIndex)
		{
			MSG_BOX("Fail Path");
			return E_FAIL;
		}

		string	strPath = strFolderPath + strText.substr(iBeginIndex + 1, iEndIndex - iBeginIndex - 1);
		MaterialFilePathes.push_back(strPath);
	}

	for (size_t i = 0; i < m_iNumMaterials; ++i) {
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, MaterialFilePathes[i].c_str(), strFolderPath.c_str());
		if (nullptr == pMaterial) {
			return E_FAIL;
		}
		m_Materials.push_back(pMaterial);
	}
	m_Materials.shrink_to_fit();

	return S_OK;
}

HRESULT CModel::Ready_Materials_Independent(MODEL eType, const aiScene* pAIScene, const _char* pModelFilePath)
{
	m_iNumMaterials = pAIScene->mNumMaterials;
	m_Materials.reserve(m_iNumMaterials);
	for (size_t i = 0; i < m_iNumMaterials; ++i) {
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, eType, pModelFilePath, pAIScene->mMaterials[i]);
		if (nullptr == pMaterial) {
			return E_FAIL;
		}
		m_Materials.push_back(pMaterial);
	}
	m_Materials.shrink_to_fit();
	return S_OK;
}

HRESULT CModel::Ready_Animations(const aiScene* pAIScene)
{
	m_iNumAnimations = pAIScene->mNumAnimations;
	m_Animations.reserve(m_iNumAnimations);
	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_Bones, pAIScene->mAnimations[i]);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone) {
		return E_FAIL;
	}

	m_Bones.push_back(pBone);

	_int	iParent = (_int)(m_Bones.size() - 1);

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
	{
		Ready_Bones(pAINode->mChildren[i], iParent);
	}

	return S_OK;
}
_bool CModel::SaveAssimpModel(const _char* filename)
{
	if (!m_pAIScene) {
		return false;
	}

	_char szModel[MAX_PATH] = {};
	_char szExt[MAX_PATH] = {};
	_splitpath_s(filename, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	strcat_s(szModel, sizeof(_char) * MAX_PATH, szExt);

	_string savePath = filename;

	SaveModel modelData;
	modelData.MeshCount = m_pAIScene->mNumMeshes;
	modelData.MaterialCount = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_pAIScene->mNumMeshes; i++)
	{
		aiMesh* mesh = m_pAIScene->mMeshes[i];
		SaveMesh saveMesh{};

		saveMesh.MeshName = mesh->mName.C_Str();
		saveMesh.MeshNameSize = (_int)saveMesh.MeshName.size() + 1;
		saveMesh.VertexCount = mesh->mNumVertices;
		saveMesh.IndexCount = mesh->mNumFaces * 3;
		saveMesh.MaterialIndex = mesh->mMaterialIndex;

		saveMesh.Vertices.resize(saveMesh.VertexCount);
		for (_uint i = 0; i < mesh->mNumVertices; i++)
		{
			saveMesh.Vertices[i].Pos = XMFLOAT3{ mesh->mVertices[i].x , mesh->mVertices[i].y,mesh->mVertices[i].z };

			if (mesh->HasNormals())
			{
				saveMesh.Vertices[i].Normal = XMFLOAT3{
					mesh->mNormals[i].x,
					mesh->mNormals[i].y,
					mesh->mNormals[i].z
				};
				saveMesh.Vertices[i].bHasNormal = true;
			}
			else
			{
				saveMesh.Vertices[i].Normal = XMFLOAT3(0.f, 1.f, 0.f);
				saveMesh.Vertices[i].bHasNormal = false;
			}

			if (mesh->HasTangentsAndBitangents())
			{
				saveMesh.Vertices[i].Tan = XMFLOAT3{
					mesh->mTangents[i].x,
					mesh->mTangents[i].y,
					mesh->mTangents[i].z
				};
				saveMesh.Vertices[i].bHasTan = true;
			}
			else
			{
				saveMesh.Vertices[i].Tan = XMFLOAT3(1.f, 0.f, 0.f);
				saveMesh.Vertices[i].bHasTan = false;
			}

			saveMesh.Vertices[i].BiNoraml = XMFLOAT3{
				mesh->mBitangents[i].x,
				mesh->mBitangents[i].y,
				mesh->mBitangents[i].z
			};

			if (mesh->HasTextureCoords(0))
			{
				saveMesh.Vertices[i].UV = XMFLOAT2{
					mesh->mTextureCoords[0][i].x,
					mesh->mTextureCoords[0][i].y
				};
				saveMesh.Vertices[i].bHasUV = true;
			}
			else
			{
				saveMesh.Vertices[i].UV = XMFLOAT2(0.f, 0.f);
				saveMesh.Vertices[i].bHasUV = false;
			}
			saveMesh.Vertices[i].BlendIndex = XMUINT4(0, 0, 0, 0);
			saveMesh.Vertices[i].BlendWeight = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
		}
		saveMesh.BoneCount = mesh->mNumBones;
		saveMesh.Bones.resize(mesh->mNumBones);
		for (size_t j = 0; j < mesh->mNumBones; j++)
		{
			saveMesh.Bones[j].BoneName = mesh->mBones[j]->mName.C_Str();
			saveMesh.Bones[j].BoneNameSize = (_int)saveMesh.Bones[j].BoneName.size();

			memcpy(&saveMesh.Bones[j].OffsetMatrix, &mesh->mBones[j]->mOffsetMatrix, sizeof(_float4x4));
			XMStoreFloat4x4(&saveMesh.Bones[j].OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&saveMesh.Bones[j].OffsetMatrix)));
			saveMesh.Bones[j].WeightsCount = mesh->mBones[j]->mNumWeights;
			saveMesh.Bones[j].Weights.resize(mesh->mBones[j]->mNumWeights);
			for (size_t k = 0; k < saveMesh.Bones[j].WeightsCount; k++)
			{
				saveMesh.Bones[j].Weights[k].VertexId = mesh->mBones[j]->mWeights[k].mVertexId;
				saveMesh.Bones[j].Weights[k].Weight = mesh->mBones[j]->mWeights[k].mWeight;
			}
		}

		saveMesh.Indices.reserve(saveMesh.IndexCount);
		for (_uint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace& face = mesh->mFaces[i];
			for (_uint idx = 0; idx < face.mNumIndices; idx++)
				saveMesh.Indices.push_back(face.mIndices[idx]);
		}

		modelData.Meshes.push_back(saveMesh);
	}

	for (size_t i = 0; i < m_Materials.size(); i++)
	{
		modelData.Materials.emplace_back(m_Materials[i]->Get_SaveMaterial());
	}

	std::vector<SaveNode> allNodes;
	SaveNodeRecursive(m_pAIScene->mRootNode, allNodes, -1);

	modelData.AnimationCount = m_pAIScene->mNumAnimations;
	for (_uint i = 0; i < modelData.AnimationCount; i++)
	{
		aiAnimation* Anim = m_pAIScene->mAnimations[i];
		SaveAnimation saveAnim{};
		saveAnim.AnimName = Anim->mName.C_Str();
		saveAnim.AnimNameSize = (_int)saveAnim.AnimName.size() + 1;
		saveAnim.mDuration = (_float)Anim->mDuration;
		saveAnim.mTicksPerSecond = (_float)Anim->mTicksPerSecond;
		saveAnim.ChannelCount = Anim->mNumChannels;

		saveAnim.Channels.resize(Anim->mNumChannels);
		for (size_t j = 0; j < saveAnim.ChannelCount; j++)
		{
			saveAnim.Channels[j].ChannelName = Anim->mChannels[j]->mNodeName.C_Str();
			saveAnim.Channels[j].ChannelNameSize = (_int)saveAnim.Channels[j].ChannelName.size();

			saveAnim.Channels[j].ScalingKeyCount = Anim->mChannels[j]->mNumScalingKeys;
			saveAnim.Channels[j].RotationKeyCount = Anim->mChannels[j]->mNumRotationKeys;
			saveAnim.Channels[j].PositionKeyCount = Anim->mChannels[j]->mNumPositionKeys;
			for (size_t k = 0; k < saveAnim.Channels[j].ScalingKeyCount; k++)
			{
				SaveKeyFrameVec Key;
				Key.Time = (_float)Anim->mChannels[j]->mScalingKeys[k].mTime;
				Key.Value = XMFLOAT3(
					Anim->mChannels[j]->mScalingKeys[k].mValue.x,
					Anim->mChannels[j]->mScalingKeys[k].mValue.y,
					Anim->mChannels[j]->mScalingKeys[k].mValue.z);
				saveAnim.Channels[j].ScalingKeys.push_back(Key);
			}
			for (size_t k = 0; k < saveAnim.Channels[j].RotationKeyCount; k++)
			{
				SaveKeyFrameRotation Key;
				Key.Time = (_float)Anim->mChannels[j]->mRotationKeys[k].mTime;
				Key.Value = XMFLOAT4(
					Anim->mChannels[j]->mRotationKeys[k].mValue.x,
					Anim->mChannels[j]->mRotationKeys[k].mValue.y,
					Anim->mChannels[j]->mRotationKeys[k].mValue.z,
					Anim->mChannels[j]->mRotationKeys[k].mValue.w);
				saveAnim.Channels[j].RotationKeys.push_back(Key);
			}
			for (size_t k = 0; k < saveAnim.Channels[j].PositionKeyCount; k++)
			{
				SaveKeyFrameVec Key;
				Key.Time = (_float)Anim->mChannels[j]->mPositionKeys[k].mTime;
				Key.Value = XMFLOAT3(
					Anim->mChannels[j]->mPositionKeys[k].mValue.x,
					Anim->mChannels[j]->mPositionKeys[k].mValue.y,
					Anim->mChannels[j]->mPositionKeys[k].mValue.z);
				saveAnim.Channels[j].PositionKeys.push_back(Key);
			}
		}
		modelData.Animations.push_back(saveAnim);
	}

	FILE* fp = nullptr;
	fopen_s(&fp, savePath.c_str(), "wb");
	if (!fp) {
		return false;
	}

	fwrite(&modelData.MeshCount, sizeof(_uint), 1, fp);
	fwrite(&modelData.MaterialCount, sizeof(_uint), 1, fp);
	fwrite(&modelData.AnimationCount, sizeof(_uint), 1, fp);
	modelData.NodeCount = (_uint)allNodes.size();
	fwrite(&modelData.NodeCount, sizeof(_uint), 1, fp);

	for (auto& mesh : modelData.Meshes)
	{
		fwrite(&mesh.MeshNameSize, sizeof(_uint), 1, fp);
		fwrite(mesh.MeshName.data(), 1, mesh.MeshNameSize, fp);
		fwrite(&mesh.VertexCount, sizeof(_uint), 1, fp);
		fwrite(&mesh.IndexCount, sizeof(_uint), 1, fp);
		fwrite(&mesh.MaterialIndex, sizeof(_uint), 1, fp);
		fwrite(&mesh.BoneCount, sizeof(_uint), 1, fp);

		fwrite(mesh.Vertices.data(), sizeof(SaveVertex), mesh.VertexCount, fp);
		fwrite(mesh.Indices.data(), sizeof(_uint), mesh.IndexCount, fp);
		for (auto& bone : mesh.Bones)
		{
			fwrite(&bone.BoneNameSize, sizeof(_uint), 1, fp);
			fwrite(bone.BoneName.data(), 1, bone.BoneNameSize, fp);
			fwrite(&bone.OffsetMatrix, sizeof(XMFLOAT4X4), 1, fp);
			fwrite(&bone.WeightsCount, sizeof(_uint), 1, fp);
			for (auto& Weights : bone.Weights)
			{
				fwrite(&Weights.VertexId, sizeof(_uint), 1, fp);
				fwrite(&Weights.Weight, sizeof(_float), 1, fp);
			}
		}
	}


	for (auto& node : allNodes)
	{
		fwrite(&node.NodeNameSize, sizeof(_uint), 1, fp);
		fwrite(node.NodeName.data(), 1, node.NodeNameSize, fp);
		fwrite(&node.ParentIndex, sizeof(int), 1, fp);
		fwrite(&node.Transformation, sizeof(XMFLOAT4X4), 1, fp);
		fwrite(&node.ChildrenCount, sizeof(_uint), 1, fp);

		for (auto child : node.ChildrenIndices)
		{
			fwrite(&child, sizeof(_int), 1, fp);
		}
	}

	for (auto& Anim : modelData.Animations)
	{
		fwrite(&Anim.AnimNameSize, sizeof(_uint), 1, fp);
		fwrite(Anim.AnimName.data(), 1, Anim.AnimNameSize, fp);
		fwrite(&Anim.mDuration, sizeof(_float), 1, fp);
		fwrite(&Anim.mTicksPerSecond, sizeof(_float), 1, fp);
		fwrite(&Anim.ChannelCount, sizeof(_uint), 1, fp);
		for (size_t i = 0; i < Anim.ChannelCount; i++)
		{
			fwrite(&Anim.Channels[i].ChannelNameSize, sizeof(_uint), 1, fp);
			fwrite(Anim.Channels[i].ChannelName.data(), 1, Anim.Channels[i].ChannelNameSize, fp);

			fwrite(&Anim.Channels[i].ScalingKeyCount, sizeof(_uint), 1, fp);
			fwrite(Anim.Channels[i].ScalingKeys.data(), sizeof(SaveKeyFrameVec), Anim.Channels[i].ScalingKeyCount, fp);

			fwrite(&Anim.Channels[i].RotationKeyCount, sizeof(_uint), 1, fp);
			fwrite(Anim.Channels[i].RotationKeys.data(), sizeof(SaveKeyFrameRotation), Anim.Channels[i].RotationKeyCount, fp);

			fwrite(&Anim.Channels[i].PositionKeyCount, sizeof(_uint), 1, fp);
			fwrite(Anim.Channels[i].PositionKeys.data(), sizeof(SaveKeyFrameVec), Anim.Channels[i].PositionKeyCount, fp);
		}
	}

	for (auto& mat : modelData.Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
		{
			_uint MaterialCount = static_cast<_uint>(mat.Path[i].size());
			fwrite(&MaterialCount, sizeof(_uint), 1, fp);
			for (const auto& path : mat.Path[i])
			{
				_uint len = static_cast<_uint>(path.size());
				fwrite(&len, sizeof(_uint), 1, fp);
				fwrite(path.c_str(), sizeof(_char), len, fp);
			}
		}
	}

	if (MODEL::ENVIROMENT == m_eType)
	{
		fwrite(&m_fRadius, sizeof(_float), 1, fp);
		fwrite(&m_vRadiusOffset, sizeof(_float3), 1, fp);
	}

	fclose(fp);

	return true;
}
_int CModel::SaveNodeRecursive(const aiNode* pAINode, std::vector<SaveNode>& outNodes, _int parentIndex)
{
	SaveNode node{};
	node.NodeName = pAINode->mName.C_Str();
	node.NodeNameSize = (_uint)node.NodeName.size() + 1;
	node.ParentIndex = parentIndex;

	const aiMatrix4x4& m = pAINode->mTransformation;
	node.Transformation = XMFLOAT4X4(
		m.a1, m.a2, m.a3, m.a4,
		m.b1, m.b2, m.b3, m.b4,
		m.c1, m.c2, m.c3, m.c4,
		m.d1, m.d2, m.d3, m.d4
	);

	XMStoreFloat4x4(&node.Transformation, XMMatrixTranspose(XMLoadFloat4x4(&node.Transformation)));

	node.ChildrenCount = pAINode->mNumChildren;
	node.ChildrenIndices.resize(node.ChildrenCount);

	_int currentIndex = static_cast<_int>(outNodes.size());
	outNodes.push_back(node);

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
	{
		_int childIndex = SaveNodeRecursive(pAINode->mChildren[i], outNodes, currentIndex);
		outNodes[currentIndex].ChildrenIndices[i] = childIndex;
	}

	return currentIndex;
}
HRESULT CModel::Assimp_Model_Load(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex)
{
	_uint			iFlag = {};
	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (MODEL::NONANIM == eType || MODEL::PBR_NONANIM == eType || MODEL::ENVIROMENT == eType || MODEL::NONANIM_LOCAL == eType) {
		iFlag |= aiProcess_PreTransformVertices;
	}
	m_iRootBoneIndex = iRootBoneIndex;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene){
		return E_FAIL;
	}


#pragma region Bone
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1))) {
		return E_FAIL;
	}
#pragma endregion
#pragma region Mesh
	if (FAILED(Ready_Meshes(eType, m_pAIScene, PreTransformMatrix))) {
		return E_FAIL;
	}
#pragma endregion
#pragma region Material
	if (MODEL::ENVIROMENT == eType)
	{
		if (FAILED(Ready_Materials_FromFile(m_pAIScene, pModelFilePath))) {
			return E_FAIL;
		}
	}
	else if (MODEL::ANIM_LOCAL == eType || MODEL::NONANIM_LOCAL == eType)
	{
		if (FAILED(Ready_Materials_FromFile_Anim(m_pAIScene, pModelFilePath))) {
			return E_FAIL;
		}
	}
	else if (MODEL::PBR_NONANIM == eType || MODEL::PBR_ANIM == eType) {
		if (FAILED(Ready_Materials_Independent(eType, m_pAIScene, pModelFilePath))) {
			return E_FAIL;
		}
	} else {
		if (FAILED(Ready_Materials(m_pAIScene, pModelFilePath))) {
			return E_FAIL;
		}
	}
#pragma endregion
#pragma region Animation
	if (FAILED(Ready_Animations(m_pAIScene))) {
		return E_FAIL;
	}
#pragma endregion

	m_pGameInstance->Save_ModelFilePath(pModelFilePath);

	m_pGameInstance->Add_ModelToMap(pModelFilePath, this);

	return S_OK;
}

#endif // EDITOR_PROJECT

HRESULT CModel::Create_CS()
{
	_uint CS1_InputStrides[] = {
		sizeof(KEYFRAME_DESC),
		sizeof(CHANNEL_DESC),
	};

	_uint CS1_OutputStrides[] = {
		sizeof(LOCALPOS_DESC),
		sizeof(LOCALPOS_DESC),
	};


	m_pComputeShader = CComputeShader::Create(m_pDevice, m_pContext, L"../Bin/Resources/ShaderFiles/Shader_Mesh_Compute.hlsl",
		"CS_LocalSRT", (_uint)m_Bones.size(), 0, 2, nullptr, CS1_OutputStrides
	);

	if (!m_pComputeShader)
		return E_FAIL;

	return S_OK;
}

void CModel::Create_Temp()
{
	for (auto& anim : m_Animations)
	{
		anim->CreateGPUData(m_pDevice);
	}
}

void CModel::Create_LocalPosVB()
{
	_uint iCount = (_uint)m_Bones.size();
	_uint iStride = sizeof(LOCALPOS_DESC);

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = iStride * iCount;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.StructureByteStride = iStride;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	HRESULT hr = m_pDevice->CreateBuffer(&desc, nullptr, &m_pLocalPosBuffer);

	return;
}


void CModel::Create_Con()
{
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(ANIMSTATE_DESC);

	if (FAILED(m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pConstantBuffer)))
		return;
}


void CModel::UpdateAnimationCS(_uint AnimIndex)
{
	D3D11_MAPPED_SUBRESOURCE sub{};
	if (SUCCEEDED(m_pContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub)))
	{
		ANIMSTATE_DESC* pDesc = (ANIMSTATE_DESC*)sub.pData;

		CAnimation* pAnim = m_Animations[AnimIndex];
		pDesc->CurrentTime = pAnim->Get_CurrentTrackPosition();
		pDesc->Duration = pAnim->Get_Duration();
		pDesc->Speed = pAnim->Get_AnimSpeed();
		pDesc->BoneCount = (_uint)m_Bones.size();
		pDesc->PreTransformMatrix = m_PreTransformMatrix;
		m_pContext->Unmap(m_pConstantBuffer, 0);
	}
}

void CModel::ComputeAnimation(_uint AnimIndex)
{
	UpdateAnimationCS(AnimIndex);

	_uint iGroupCountX = ((_uint)m_Bones.size() + 255) / 256;

	CAnimation* pAnim = m_Animations[AnimIndex];
	ID3D11ShaderResourceView* srvs[2] = {
		pAnim->Get_KeyFrameSrv(),
		pAnim->Get_ChannelSrv(),
	};

	m_pContext->CSSetShader(m_pComputeShader->Get_Compute(), nullptr, 0);
	m_pContext->CSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pContext->CSSetShaderResources(0, 2, srvs);

	ID3D11UnorderedAccessView* uavs[1] = { m_pComputeShader->GetOutputUAV(0) };
	m_pContext->CSSetUnorderedAccessViews(0, 1, uavs, nullptr);

	m_pContext->Dispatch(iGroupCountX, 1, 1);

	auto outSubs = m_pComputeShader->ReadBackOutputs();
	m_pLocalPos[0] = static_cast<LOCALPOS_DESC*>(outSubs[0].pData);
}

void CModel::ComputeAnimation_Second(_uint AnimIndex)
{
	UpdateAnimationCS(AnimIndex);

	_uint iGroupCountX = ((_uint)m_Bones.size() + 255) / 256;

	CAnimation* pAnim = m_Animations[AnimIndex];
	ID3D11ShaderResourceView* srvs[2] = {
		pAnim->Get_KeyFrameSrv(),
		pAnim->Get_ChannelSrv(),
	};

	m_pContext->CSSetShader(m_pComputeShader->Get_Compute(), nullptr, 0);
	m_pContext->CSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pContext->CSSetShaderResources(0, 2, srvs);

	ID3D11UnorderedAccessView* uavs[1] = { m_pComputeShader->GetOutputUAV(1) };
	m_pContext->CSSetUnorderedAccessViews(0, 1, uavs, nullptr);

	m_pContext->Dispatch(iGroupCountX, 1, 1);

	auto outSubs = m_pComputeShader->ReadBackOutputs();
	m_pLocalPos[1] = static_cast<LOCALPOS_DESC*>(outSubs[1].pData);
}

void CModel::InItialize_BoneIndex()
{
	for (_uint i = 0; i < (_uint)m_Bones.size(); i++)
	{
		if (m_Bones[i]->Compare_Name("Spine"))
		{
			m_iBoneIndex[ENUM_CLASS(BLEND_BONE::SPINE)] = i;
		}
		if (m_Bones[i]->Compare_Name("LeftShoulder"))
		{
			m_iBoneIndex[ENUM_CLASS(BLEND_BONE::SHOULDER_L)] = i;
		}
		if (m_Bones[i]->Compare_Name("RightShoulder"))
		{
			m_iBoneIndex[ENUM_CLASS(BLEND_BONE::SHOULDER_R)] = i;
		}
		if (m_Bones[i]->Compare_Name("Neck"))
		{
			m_iBoneIndex[ENUM_CLASS(BLEND_BONE::NECK)] = i;
		}
	}
}

void CModel::Initialize_BoneMasks()
{
	m_BoneMask.resize((_int)BLEND_BONE::END);
	for (auto& mask : m_BoneMask)
		mask.resize(m_Bones.size(), false);

	auto BuildMask = [&](_int rootIndex, vector<_uint>& outMask)
		{
			if (rootIndex == -1)
				return;
			for (_uint i = 0; i < (_uint)m_Bones.size(); i++)
			{
				_int cur = i;
				while (cur != -1)
				{
					if (cur == rootIndex)
					{
						outMask[i] = true;
						break;
					}
					cur = m_Bones[cur]->Get_ParentBoneIndex();
				}
			}
		};

	BuildMask(m_iBoneIndex[ENUM_CLASS(BLEND_BONE::SPINE)], m_BoneMask[ENUM_CLASS(BLEND_BONE::SPINE)]);
	BuildMask(m_iBoneIndex[ENUM_CLASS(BLEND_BONE::SHOULDER_L)], m_BoneMask[ENUM_CLASS(BLEND_BONE::SHOULDER_L)]);
	BuildMask(m_iBoneIndex[ENUM_CLASS(BLEND_BONE::SHOULDER_R)], m_BoneMask[ENUM_CLASS(BLEND_BONE::SHOULDER_R)]);
	BuildMask(m_iBoneIndex[ENUM_CLASS(BLEND_BONE::NECK)], m_BoneMask[ENUM_CLASS(BLEND_BONE::NECK)]);

	for (_uint i = 0; i < m_Bones.size(); i++)
	{
		m_BoneMask[ENUM_CLASS(BLEND_BONE::SHOULDER_NECK_L)][i] =
			m_BoneMask[ENUM_CLASS(BLEND_BONE::SHOULDER_L)][i] | m_BoneMask[ENUM_CLASS(BLEND_BONE::NECK)][i];

		m_BoneMask[ENUM_CLASS(BLEND_BONE::SHOULDER_NECK_R)][i] =
			m_BoneMask[ENUM_CLASS(BLEND_BONE::SHOULDER_R)][i] | m_BoneMask[ENUM_CLASS(BLEND_BONE::NECK)][i];
	}
}

HRESULT CModel::Create_ParentVB()
{
	D3D11_BUFFER_DESC desc{};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = (_int)sizeof(PARENT_DESC) * (_uint)m_Bones.size();
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.StructureByteStride = (_int)sizeof(PARENT_DESC);
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA init{};
	init.pSysMem = m_Parent.data();

	m_pDevice->CreateBuffer(&desc, &init, &m_pParentBuffer);


	return S_OK;
}
#ifdef _DEBUG
HRESULT CModel::Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	m_eType = eType;
	_char		szTextureFileName[MAX_PATH] = {};
	_char		szDir[MAX_PATH] = {};
	_char		szName[MAX_PATH] = {};
	_char		szEXT[MAX_PATH] = {};

	_splitpath_s(pModelFilePath, nullptr, 0, szDir, MAX_PATH, szName, MAX_PATH, szEXT, MAX_PATH);


	if (strcmp(".bin", szEXT) == 0)
	{
		LoadData(pModelFilePath);
	}
	else if (strcmp(".fbx", szEXT) == 0)
	{
		_char Temp[256];
		strcpy_s(Temp, szDir);
		strcat_s(Temp, szName);
		strcat_s(Temp, ".bin");

		if (FAILED(Assimp_Model_Load(pModelFilePath, eType, PreTransformMatrix, 0))) {
			return E_FAIL;
		}

		SaveAssimpModel(Temp);

		return S_OK;
	}

	m_pSaveModel = m_pGameInstance->Load_SaveModel(pModelFilePath);
	if (nullptr == m_pSaveModel) {
		return S_OK;
	}

	LoadAdditionalAnimations(pModelFilePath);

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	Ready_Bones(m_pSaveModel->Nodes, 0, -1);

	if (FAILED(Ready_Meshes())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Materials(pModelFilePath))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Animations(m_Bones))) {
		return E_FAIL;
	}

	return S_OK;
}

#endif // _DEBUG



#ifndef _DEBUG

HRESULT CModel::Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	m_eType = eType;

	LoadData(pModelFilePath);

	m_pSaveModel = m_pGameInstance->Load_SaveModel(pModelFilePath);

	LoadAdditionalAnimations(pModelFilePath);

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	Ready_Bones(m_pSaveModel->Nodes, 0, -1);

	if (FAILED(Ready_Meshes())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Materials(pModelFilePath))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Animations(m_Bones))) {
		return E_FAIL;
	}
	return S_OK;
}
#endif 

void CModel::LoadAdditionalAnimations(const char* ModelFilePath)
{
	filesystem::path fullPath(ModelFilePath);
	filesystem::path folder = fullPath.parent_path();
	_string baseName = fullPath.stem().string();

	_string defaultModel = fullPath.filename().string();

	for (auto& entry : filesystem::directory_iterator(folder))
	{
		if (!entry.is_regular_file())
			continue;

		auto file = entry.path().filename().string();
		auto stem = entry.path().stem().string();
		auto ext = entry.path().extension().string();

		if (ext != ".bin")
			continue;

		if (file == defaultModel)
			continue;

		if (stem.rfind(baseName, 0) == 0)
		{
			LoadAnim(entry.path().string().c_str());
		}
	}
}


_bool CModel::LoadData(const _char* filename)
{
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	if (!fp) {
		return false;
	}

	SaveModel NewModel = {};

	fread(&NewModel.MeshCount, sizeof(_uint), 1, fp);
	fread(&NewModel.MaterialCount, sizeof(_uint), 1, fp);
	fread(&NewModel.AnimationCount, sizeof(_uint), 1, fp);
	fread(&NewModel.NodeCount, sizeof(_uint), 1, fp);

	for (_uint i = 0; i < NewModel.MeshCount; i++)
	{
		SaveMesh mesh{};

		fread(&mesh.MeshNameSize, sizeof(_uint), 1, fp);
		mesh.MeshName.resize(mesh.MeshNameSize);
		fread(mesh.MeshName.data(), 1, mesh.MeshNameSize, fp);

		fread(&mesh.VertexCount, sizeof(_uint), 1, fp);
		fread(&mesh.IndexCount, sizeof(_uint), 1, fp);
		fread(&mesh.MaterialIndex, sizeof(_uint), 1, fp);
		fread(&mesh.BoneCount, sizeof(_uint), 1, fp);

		mesh.Vertices.resize(mesh.VertexCount);
		fread(mesh.Vertices.data(), sizeof(SaveVertex), mesh.VertexCount, fp);

		mesh.Indices.resize(mesh.IndexCount);
		fread(mesh.Indices.data(), sizeof(_uint), mesh.IndexCount, fp);

		mesh.Bones.resize(mesh.BoneCount);
		for (size_t i = 0; i < mesh.BoneCount; i++)
		{
			fread(&mesh.Bones[i].BoneNameSize, sizeof(_uint), 1, fp);
			mesh.Bones[i].BoneName.resize(mesh.Bones[i].BoneNameSize);
			fread(mesh.Bones[i].BoneName.data(), 1, mesh.Bones[i].BoneNameSize, fp);
			fread(&mesh.Bones[i].OffsetMatrix, sizeof(XMFLOAT4X4), 1, fp);
			fread(&mesh.Bones[i].WeightsCount, sizeof(_uint), 1, fp);
			mesh.Bones[i].Weights.resize(mesh.Bones[i].WeightsCount);
			for (size_t j = 0; j < mesh.Bones[i].WeightsCount; j++)
			{
				fread(&mesh.Bones[i].Weights[j].VertexId, sizeof(_uint), 1, fp);
				fread(&mesh.Bones[i].Weights[j].Weight, sizeof(_float), 1, fp);
			}
		}
		NewModel.Meshes.push_back(mesh);
	}

	NewModel.Nodes.resize(NewModel.NodeCount);
	for (size_t i = 0; i < NewModel.NodeCount; i++)
	{
		fread(&NewModel.Nodes[i].NodeNameSize, sizeof(_uint), 1, fp);
		NewModel.Nodes[i].NodeName.resize(NewModel.Nodes[i].NodeNameSize);
		fread(NewModel.Nodes[i].NodeName.data(), 1, NewModel.Nodes[i].NodeNameSize, fp);
		fread(&NewModel.Nodes[i].ParentIndex, sizeof(int), 1, fp);
		fread(&NewModel.Nodes[i].Transformation, sizeof(XMFLOAT4X4), 1, fp);
		fread(&NewModel.Nodes[i].ChildrenCount, sizeof(_uint), 1, fp);
		NewModel.Nodes[i].ChildrenIndices.resize(NewModel.Nodes[i].ChildrenCount);
		for (size_t j = 0; j < NewModel.Nodes[i].ChildrenCount; j++)
		{
			fread(&NewModel.Nodes[i].ChildrenIndices[j], sizeof(_int), 1, fp);
		}
	}

	for (size_t i = 0; i < NewModel.AnimationCount; i++)
	{
		SaveAnimation saveAnim = {};
		fread(&saveAnim.AnimNameSize, sizeof(_uint), 1, fp);
		saveAnim.AnimName.resize(saveAnim.AnimNameSize);
		fread(saveAnim.AnimName.data(), 1, saveAnim.AnimNameSize, fp);
		fread(&saveAnim.mDuration, sizeof(_float), 1, fp);
		fread(&saveAnim.mTicksPerSecond, sizeof(_float), 1, fp);
		fread(&saveAnim.ChannelCount, sizeof(_uint), 1, fp);
		saveAnim.Channels.resize(saveAnim.ChannelCount);
		for (size_t j = 0; j < saveAnim.ChannelCount; j++)
		{
			fread(&saveAnim.Channels[j].ChannelNameSize, sizeof(_uint), 1, fp);

			saveAnim.Channels[j].ChannelName.resize(saveAnim.Channels[j].ChannelNameSize);
			fread(saveAnim.Channels[j].ChannelName.data(), 1, saveAnim.Channels[j].ChannelNameSize, fp);

			fread(&saveAnim.Channels[j].ScalingKeyCount, sizeof(_uint), 1, fp);
			saveAnim.Channels[j].ScalingKeys.resize(saveAnim.Channels[j].ScalingKeyCount);
			fread(saveAnim.Channels[j].ScalingKeys.data(), sizeof(SaveKeyFrameVec), saveAnim.Channels[j].ScalingKeyCount, fp);

			fread(&saveAnim.Channels[j].RotationKeyCount, sizeof(_uint), 1, fp);
			saveAnim.Channels[j].RotationKeys.resize(saveAnim.Channels[j].RotationKeyCount);
			fread(saveAnim.Channels[j].RotationKeys.data(), sizeof(SaveKeyFrameRotation), saveAnim.Channels[j].RotationKeyCount, fp);

			fread(&saveAnim.Channels[j].PositionKeyCount, sizeof(_uint), 1, fp);
			saveAnim.Channels[j].PositionKeys.resize(saveAnim.Channels[j].PositionKeyCount);
			fread(saveAnim.Channels[j].PositionKeys.data(), sizeof(SaveKeyFrameVec), saveAnim.Channels[j].PositionKeyCount, fp);
		}
		NewModel.Animations.push_back(saveAnim);
	}

	NewModel.Materials.clear();
	for (_uint i = 0; i < NewModel.MaterialCount; i++)
	{
		SaveMaterial pMAt;

		for (_uint k = 0; k < AI_TEXTURE_TYPE_MAX; k++)
		{
			_uint MaterialCount = 0;
			fread(&MaterialCount, sizeof(_uint), 1, fp);
			pMAt.Path[k].resize(MaterialCount);
			for (_uint j = 0; j < MaterialCount; j++)
			{
				_uint len = 0;
				fread(&len, sizeof(_uint), 1, fp);
				std::string temp;
				temp.resize(len);
				fread(&temp[0], sizeof(_char), len, fp);
				pMAt.Path[k][j] = temp;
			}
		}

		NewModel.Materials.push_back(pMAt);
	}

	if (MODEL::ENVIROMENT == m_eType)
	{
		fread(&m_fRadius, sizeof(_float), 1, fp);
		fread(&m_vRadiusOffset, sizeof(_float3), 1, fp);
	}

	fclose(fp);

	m_SaveModel.push_back(NewModel);

	m_pGameInstance->Add_SaveModel(filename, m_SaveModel.back());

	return true;
}

void CModel::LoadAnim(const _char* fileName)
{
	FILE* fp = nullptr;
	fopen_s(&fp, fileName, "rb");
	if (!fp) {
		return;
	}

	_uint MeshCount, MaterialCount, AnimationCount, NodeCount;

	fread(&MeshCount, sizeof(_uint), 1, fp);
	fread(&MaterialCount, sizeof(_uint), 1, fp);
	fread(&AnimationCount, sizeof(_uint), 1, fp);
	fread(&NodeCount, sizeof(_uint), 1, fp);

	for (_uint i = 0; i < MeshCount; i++)
	{
		_uint nameSize, vtxCount, idxCount, matIndex, boneCount;

		fread(&nameSize, sizeof(_uint), 1, fp);
		fseek(fp, nameSize, SEEK_CUR);

		fread(&vtxCount, sizeof(_uint), 1, fp);
		fread(&idxCount, sizeof(_uint), 1, fp);
		fread(&matIndex, sizeof(_uint), 1, fp);
		fread(&boneCount, sizeof(_uint), 1, fp);

		fseek(fp, sizeof(SaveVertex) * vtxCount, SEEK_CUR);

		fseek(fp, sizeof(_uint) * idxCount, SEEK_CUR);

		for (_uint b = 0; b < boneCount; b++)
		{
			_uint boneNameSize = 0;
			fread(&boneNameSize, sizeof(_uint), 1, fp);
			fseek(fp, boneNameSize, SEEK_CUR);

			fseek(fp, sizeof(_float4x4), SEEK_CUR);

			_uint weightCount = 0;
			fread(&weightCount, sizeof(_uint), 1, fp);
			fseek(fp, (sizeof(_uint) + sizeof(_float)) * weightCount, SEEK_CUR);
		}
	}

	for (_uint i = 0; i < NodeCount; i++)
	{
		_uint nodeNameSize;
		fread(&nodeNameSize, sizeof(_uint), 1, fp);
		fseek(fp, nodeNameSize, SEEK_CUR);

		fseek(fp, sizeof(int), SEEK_CUR);
		fseek(fp, sizeof(XMFLOAT4X4), SEEK_CUR);

		_uint childrenCount;
		fread(&childrenCount, sizeof(_uint), 1, fp);
		fseek(fp, sizeof(_int) * childrenCount, SEEK_CUR);
	}

	for (_uint i = 0; i < AnimationCount; i++)
	{
		SaveAnimation saveAnim = {};

		fread(&saveAnim.AnimNameSize, sizeof(_uint), 1, fp);
		saveAnim.AnimName.resize(saveAnim.AnimNameSize);
		fread(saveAnim.AnimName.data(), 1, saveAnim.AnimNameSize, fp);

		fread(&saveAnim.mDuration, sizeof(_float), 1, fp);
		fread(&saveAnim.mTicksPerSecond, sizeof(_float), 1, fp);

		fread(&saveAnim.ChannelCount, sizeof(_uint), 1, fp);
		saveAnim.Channels.resize(saveAnim.ChannelCount);

		for (size_t j = 0; j < saveAnim.ChannelCount; j++)
		{
			fread(&saveAnim.Channels[j].ChannelNameSize, sizeof(_uint), 1, fp);

			saveAnim.Channels[j].ChannelName.resize(saveAnim.Channels[j].ChannelNameSize);
			fread(saveAnim.Channels[j].ChannelName.data(), 1, saveAnim.Channels[j].ChannelNameSize, fp);

			fread(&saveAnim.Channels[j].ScalingKeyCount, sizeof(_uint), 1, fp);
			saveAnim.Channels[j].ScalingKeys.resize(saveAnim.Channels[j].ScalingKeyCount);
			fread(saveAnim.Channels[j].ScalingKeys.data(),
				sizeof(SaveKeyFrameVec), saveAnim.Channels[j].ScalingKeyCount, fp);

			fread(&saveAnim.Channels[j].RotationKeyCount, sizeof(_uint), 1, fp);
			saveAnim.Channels[j].RotationKeys.resize(saveAnim.Channels[j].RotationKeyCount);
			fread(saveAnim.Channels[j].RotationKeys.data(),
				sizeof(SaveKeyFrameRotation), saveAnim.Channels[j].RotationKeyCount, fp);

			fread(&saveAnim.Channels[j].PositionKeyCount, sizeof(_uint), 1, fp);
			saveAnim.Channels[j].PositionKeys.resize(saveAnim.Channels[j].PositionKeyCount);
			fread(saveAnim.Channels[j].PositionKeys.data(),
				sizeof(SaveKeyFrameVec), saveAnim.Channels[j].PositionKeyCount, fp);
		}

		m_pSaveModel->Animations.push_back(saveAnim);
		m_pSaveModel->AnimationCount++;
	}

	fclose(fp);
}


HRESULT CModel::Initialize(void* pArg)
{
	m_pTransform = m_pOwner->Get_Component<CTransform>();
	SAFE_ADDREF(m_pTransform);


	if (m_eType == MODEL::ANIM || m_eType == MODEL::PBR_ANIM || m_eType == MODEL::ANIM_LOCAL)
	{
		Initialize_RootBone();
		InItialize_BoneIndex();
		Initialize_BoneMasks();

		/*Create_Temp();
		m_Parent.resize(m_Bones.size());
		for (size_t i = 0; i < (_uint)m_Bones.size(); i++)
		{
			m_Parent[i].ParentIndex = m_Bones[i]->Get_ParentBoneIndex();
		}

		if (FAILED(Create_CS()))
			return E_FAIL;

		Create_Con();
		Create_LocalPosVB();
		Create_ParentVB();*/

	}


	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pSaveModel->MeshCount;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, this, &m_pSaveModel->Meshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pSaveModel->MaterialCount;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, pModelFilePath, m_pSaveModel->Materials[i]);
		if (nullptr == pMaterial)
			return E_FAIL;

		m_Materials.push_back(pMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const std::vector<SaveNode>& allNodes, _int currentIndex, _int parentIndex)
{
	const SaveNode& saveNode = allNodes[currentIndex];

	CBone* pBone = CBone::Create(saveNode, parentIndex);

	if (!pBone) {
		return E_FAIL;
	}
	m_Bones.push_back(pBone);

	_int myIndex = _int(m_Bones.size() - 1);

	for (size_t i = 0; i < saveNode.ChildrenIndices.size(); i++)
	{
		Ready_Bones(allNodes, saveNode.ChildrenIndices[i], myIndex);
	}
	return S_OK;
}

HRESULT CModel::Ready_Animations(const vector<CBone*>& Bones)
{
	m_iNumAnimations = m_pSaveModel->AnimationCount;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(Bones, this, &m_pSaveModel->Animations[i]);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg, class CGameObject* pOwner)
{
	CModel* pInstance = new CModel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


void CModel::Free()
{
	__super::Free();


	SAFE_RELEASE(m_pComputeShader);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pParentBuffer);
	SAFE_RELEASE(m_pLocalPosBuffer);
	
	for (auto& pAnimation : m_Animations) {
		SAFE_RELEASE(pAnimation);
	}
	m_Animations.clear();

	for (auto& pMesh : m_Meshes) {
		SAFE_RELEASE(pMesh);
	}
	m_Meshes.clear();
	for (auto& pMaterial : m_Materials) {
		SAFE_RELEASE(pMaterial);
	}
	m_Materials.clear();
	for (auto& pBone : m_Bones) {
		SAFE_RELEASE(pBone);
	}
	m_Bones.clear();

	SAFE_RELEASE(m_pTransform);

	m_SaveModel.clear();

	m_Parent.clear();
	m_AnimRanges.clear();

}
#ifdef _DEBUG
void CModel::Describe_Entity()
{
	GUI::Begin("Model_Desc");
	if (GUI::Button("SaveAnimIndexWithName")) {
		_wstring wstrPath = TEXT("../Bin/AnimIndex.txt");
		HANDLE	hFile = CreateFile(wstrPath.c_str(),
			GENERIC_WRITE, NULL,
			NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL
		);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			GUI::End();
			CloseHandle(hFile);
			return;
		}

		_string name = {};
		for (_uint i = 0; i < m_iNumAnimations; ++i) {

			name.clear();
			name.append(m_Animations[i]->Get_Name().c_str());

			size_t pos = name.find_last_of('|');
			if (0 > pos) { pos = 0; }
			else { pos++; }
			name = name.substr(pos);
			name = to_string(i) + '\t' + name + '\n';
			name.shrink_to_fit();
			WriteFile(hFile, name.data(), (DWORD)name.length(), nullptr, nullptr);
		}

		CloseHandle(hFile);

	}
	GUI::End();
}

#endif // _DEBUG

