#include "pch.h"
#include "Model.h"
#include "Bone.h"
#include "Mesh.h"
#include "Material.h"
#include "Animation.h"
#include "LerpAnim.h"
#include "GameObject.h"

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
	m_pLerpAnim(rhs.m_pLerpAnim)
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

void CModel::Change_AnimationIndex(_int iAnimationIndex, _bool bIsLoop, _float fLerpDuration, _bool bIgnoreCurrentIndex)
{
	if (false == bIgnoreCurrentIndex && m_iCurrentAnimIndex == iAnimationIndex) { return; }
	m_iCurrentAnimIndex = iAnimationIndex;
	m_bIsLoop = bIsLoop;
	RefreshAnim();

	m_PrevAnimationMatrix = Get_BoneMatrix(m_iRootBoneIndex);


	m_Animations[m_iCurrentAnimIndex]->Depart_Animation(); // 애니메이션 종료 이벤트 ( 럴프애님은 자체적으로 불림 )
	vector<_int>* BeforeAnimIndex = m_Animations[m_iCurrentAnimIndex]->Capture_Bones(); // 영향을 받는 본 인덱스 추출 1
	vector<_int>* AfterAnimIndex = m_Animations[iAnimationIndex]->Capture_Bones(); // 영향을 받는 본 인덱스 추출 2
	unordered_set<_int> UnionAnimIndex = {};

	vector<LERPDESC> LerpStartFrames;
	for (_int i : *BeforeAnimIndex) {
		UnionAnimIndex.insert(i);
	}
	for (_int i : *AfterAnimIndex) {
		UnionAnimIndex.insert(i);
	}
	LerpStartFrames.reserve(UnionAnimIndex.size()); // 영향을 받는 모든 현재 본을 가져옴

	// 스타팅 키프레임 채우기 pair < 본 인덱스, 키프레임 >
	for (auto iter = UnionAnimIndex.begin(); iter != UnionAnimIndex.end(); ++iter) {
		LERPDESC desc = {};
		desc.iSlot = *iter;
		m_Bones[desc.iSlot]->Get_KeyFrame(desc.tagKeyFrame, true); // 스타팅 키프레임 채우기
		LerpStartFrames.emplace_back(desc);
	}

	// 엔딩 키프레임 채우기 pair < 본인덱스, 키프레임 >
	vector<LERPDESC> LerpEndFrames = m_Animations[iAnimationIndex]->Get_StartFrameInformations();

	// 럴프 애니메이션 시작
	m_pLerpAnim->Begin(m_Bones, LerpStartFrames, LerpEndFrames, fLerpDuration, m_iRootBoneIndex);

}
HRESULT CModel::Bind_Material(_uint iMeshIndex, CShader* pShader, const _char* pConstantName, _uint iType, _uint iTextureIndex)
{
	if (iMeshIndex >= m_iNumMeshes) {
		return E_FAIL;
	}

	_uint		iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials) {
		return E_FAIL;
	}
	return m_Materials[iMaterialIndex]->Bind_SRV(pShader, pConstantName, iType, iTextureIndex);
}

HRESULT CModel::Bind_BoneMatrices(_uint iMeshIndex, CShader* pShader, const _char* pConstantName)
{
	if (iMeshIndex >= m_iNumMeshes) {
		return E_FAIL;
	}
	return m_Meshes[iMeshIndex]->Bind_BoneMatrices(m_Bones, pShader, pConstantName);
}

_bool CModel::Play_Animation(_float fTimeDelta)
{
	if (-1 == m_iCurrentAnimIndex  // 정지 혹은 시작을 안시켜준 애님
		|| m_iCurrentAnimIndex >= (_int)m_iNumAnimations)  // 애님 인덱스 초과됨
	{
		return false; // 잘못된 초기화
	}

	if (true == m_pLerpAnim->IsLerping()) { // 럴프중이면 자동으로 럴프 Update를 부름
		m_bIsFinishedLerp = m_pLerpAnim->Update_TransformationMatrices(m_Bones, fTimeDelta);
	}
	else { // 럴프중이지 않다면 실제 애니메이션 재생을 시작함
		m_bIsFinishedAnim = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_bIsLoop, fTimeDelta);
	}

	for (int i = 0; i < m_Bones.size(); ++i)
	{
		m_Bones[i]->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));

		if (i == m_iRootBoneIndex) { // 루트본의 매트릭스를 갱신했다면 애니메이션에 맞게 이동량을 루트본에서 트랜스폼으로 보정시킴
			if (true == m_pLerpAnim->IsLerping()) { // 럴프중이었으면 이동량 반영 x
				_matrix currAnimMatrix = Get_BoneMatrix(m_iRootBoneIndex);
				currAnimMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
				m_Bones[i]->Set_CombinedTransformationMatrixPtr(currAnimMatrix);
			}
			else {
				_matrix currAnimMatrix = Get_BoneMatrix(m_iRootBoneIndex); // 현재 루트본의 컴바인드 트랜스폼을 꺼내오고
				_matrix deltaAnimMatrix = XMMatrixMultiply(XMMatrixInverse(nullptr, m_PrevAnimationMatrix), currAnimMatrix); // 이전 트랜스폼이랑 비교해서 델타매트릭스를 꺼냄
				_vector momentum = XMMatrixMultiply(deltaAnimMatrix, XMMatrixRotationQuaternion(m_pTransform->Get_QuarternionVector())).r[3];
				deltaAnimMatrix.r[3] = XMVectorSetW(momentum, 0.f);

				m_pTransform->AccumulateMomentum(deltaAnimMatrix.r[3]); // 모체 트랜스폼에 이동량 반영
				//m_pTransform->Set_State(STATE::POSITION, m_pTransform->Get_State(STATE::POSITION) + deltaAnimMatrix.r[3]);

				m_PrevAnimationMatrix = currAnimMatrix;
				currAnimMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
				m_Bones[i]->Set_CombinedTransformationMatrixPtr(currAnimMatrix); // 루트본의 이동량 소거
			}
		}
	}

	return m_bIsFinishedAnim;
}

void CModel::Stop_Animation()
{
	if (-1 == m_iCurrentAnimIndex  // 정지 혹은 시작을 안시켜준 애님
		|| m_iCurrentAnimIndex >= (_int)m_iNumAnimations)  // 애님 인덱스 초과됨
	{
		return; // 잘못된 초기화
	}

	m_Animations[m_iCurrentAnimIndex]->Depart_Animation(); // 애님이 퇴근할 때 불리는 함수를 부르면서 초기상태로 되돌림
}

void CModel::RefreshAnim()
{
	m_bIsFinishedAnim = false;
	m_bIsFinishedLerp = false;
}
_float CModel::Get_AnimProgressRatio()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_AnimProgressRatio();
}
_int CModel::Get_AnimProgressPostion(const char* pAnimChannelName)
{
	return m_Animations[m_iCurrentAnimIndex]->Get_AnimProgressPostion(pAnimChannelName);
}
_float CModel::Get_AnimEstimatedDuration()
{
	if (MODEL::ANIM == m_eType) {
		return m_Animations[m_iCurrentAnimIndex]->Get_AnimEstimatedDuration();
	}
	else {
		return 0.f;
	}

}
void CModel::Set_AnimProgressPostion(const _char* pChannelName, _uint iPosition)
{
	return m_Animations[m_iCurrentAnimIndex]->Set_AnimProgressPostion(pChannelName, iPosition);
}
void CModel::Set_AnimPauseState(_bool bValue) {
	m_Animations[m_iCurrentAnimIndex]->Set_AnimPause(bValue);
}

void CModel::ReallocateResources(CTransform* pTransform)
{
	SAFE_RELEASE(m_pTransform);
	m_pTransform = pTransform;
	SAFE_ADDREF(m_pTransform);
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


_matrix CModel::Get_BoneMatrix(_uint iBoneIndex)
{
	return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
}

void CModel::Get_BoneMatrices(_float4x4* pOut)
{
	if (0 == m_Bones.size()) {
		return;
	}
	for (_uint i = 0; i < m_Bones.size(); ++i) {
		pOut[i] = *m_Bones[i]->Get_CombinedTransformationMatrixPtr();
	}
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
#endif // EDITOR_PROJECT

HRESULT CModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CModel::Initialize_Prototype(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex)
{
	return Assimp_Model_Load(pModelFilePath, eType, PreTransformMatrix, iRootBoneIndex);
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, eType, PreTransformMatrix, iRootBoneIndex)))
	{
		MSG_BOX("Failed to Created : CModel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

HRESULT CModel::Assimp_Model_Load(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex)
{
	_uint			iFlag = {};
	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (MODEL::NONANIM == eType) {
		iFlag |= aiProcess_PreTransformVertices;
	}
	m_iRootBoneIndex = iRootBoneIndex;

	Assimp::Importer Importer = {};
	const aiScene* pAIScene = { nullptr };

	pAIScene = Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == pAIScene) {
		return E_FAIL;
	}

#pragma region Bone
	if (FAILED(Ready_Bones(pAIScene->mRootNode, -1))) {
		return E_FAIL;
	}
#pragma endregion
#pragma region Mesh
	if (FAILED(Ready_Meshes(eType, pAIScene, PreTransformMatrix))) {
		return E_FAIL;
	}
#pragma endregion
#pragma region Material
	if (FAILED(Ready_Materials(pAIScene, pModelFilePath))) {
		return E_FAIL;
	}
#pragma endregion
#pragma region Animation
	if (FAILED(Ready_Animations(pAIScene))) {
		return E_FAIL;
	}
#pragma endregion
	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	m_pLerpAnim = CLerpAnim::Create((_uint)m_Bones.size(), 1.f, m_Bones);
	if (nullptr == m_pLerpAnim) {
		return E_FAIL;
	}
	m_pTransform = m_pOwner->Get_Component<CTransform>();
	SAFE_ADDREF(m_pTransform);

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
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
	SAFE_RELEASE(m_pLerpAnim);
}

void CModel::Describe_Entity()
{
	//GUI::Begin("Model_Desc");
	//GUI::Text("Lerping : %d\nAnim : %d", m_pLerpAnim->IsLerping(), m_bIsFinishedAnim);
	//for (_uint i = 0; i < m_iNumAnimations; ++i) {
	//	if (GUI::Button(m_Animations[i]->Get_Name().c_str())) {
	//		Change_AnimationIndex(i, false, 0.25f);
	//	}
	//}
	//if (GUI::Button("SaveAnimIndexWithName")) {
	//	_wstring wstrPath = TEXT("../Bin/AnimIndex.txt");
	//	HANDLE	hFile = CreateFile(wstrPath.c_str(),
	//		GENERIC_WRITE, NULL,
	//		NULL, CREATE_ALWAYS,
	//		FILE_ATTRIBUTE_NORMAL, NULL
	//	);

	//	if (hFile == INVALID_HANDLE_VALUE)
	//	{
	//		GUI::End();
	//		CloseHandle(hFile);
	//		return;
	//	}

	//	_string name = {};
	//	for (_uint i = 0; i < m_iNumAnimations; ++i) {
	//		
	//		name.clear();
	//		name.append(to_string(i) + '\t' + m_Animations[i]->Get_Name() + "\n");
	//		name.shrink_to_fit();
	//		WriteFile(hFile, name.data(),(DWORD) name.length(), nullptr, nullptr);
	//	}

	//	CloseHandle(hFile);

	//}
	//GUI::End();
	//
	//GUI::Begin("Bone_Desc");
	//_uint iNumBones = (_uint)m_Bones.size();
	//for (_uint i = 0; i < iNumBones; ++i){
	//	_float3 vPos = m_Bones[i]->Get_LocalPosition();
	//	GUI::PushID(i);
	//	GUI::DragFloat3(m_Bones[i]->Get_Name(), (_float*)&vPos, 1.f, 0.f, 0.f, "%.3f", ImGuiSliderFlags_NoInput);
	//	GUI::PopID();
	//}
	//GUI::End();
}
