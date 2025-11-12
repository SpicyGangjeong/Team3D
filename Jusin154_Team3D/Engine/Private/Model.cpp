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
	if (!m_bPlayAnim)
		return false;
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

void CModel::Set_AnimationIndex(_uint iIndex, _bool isLoop)
{
	if (m_iCurrentAnimIndex == iIndex)
		return;
	if (iIndex >= 0 && iIndex < m_iNumAnimations)
	{
		m_iCurrentAnimIndex = iIndex;
		m_bIsLoop = isLoop;
	}
	else
		m_iCurrentAnimIndex = -1;
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

void CModel::Set_CurrentTrackPosition(_float TrackPosition)
{
	m_Animations[m_iCurrentAnimIndex]->Set_CurrentTrackPosition(TrackPosition);
}

const _char* CModel::Get_AnimList(_uint iIndex)
{
	return m_Animations[iIndex]->Get_SZName();
}

_float CModel::Get_CurrentTrackPosition()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_CurrentTrackPosition();
}

_float CModel::Get_AnimSpeed()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_AnimSpeed();
}

void CModel::Set_AnimSpeed(_float fSpeed)
{
	m_Animations[m_iCurrentAnimIndex]->Set_AnimSpeed(fSpeed);
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

	size_t iFolderPos = strFolderPath.find("Environment");

	strFolderPath = strFolderPath.substr(0, iFolderPos);

	_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);

	string FileName = szFileName;

	auto iPos = FileName.find_last_of('.');

	FileName = FileName.substr(0, iPos);

	strcpy_s(szMeshFilePath, szDrive);
	strcat_s(szMeshFilePath, szDir);
	strcat_s(szMeshFilePath, FileName.c_str());
	strcat_s(szMeshFilePath, ".props.txt");

	ifstream file(szMeshFilePath);

	if (!file.is_open())
	{
		//MSG_BOX("Failed to Open Mesh File");
		/* LOD */
		return S_OK;
	}

	string strText = {};
	_uint iNumParameter = {};

	getline(file, strText);
	getline(file, strText);

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		getline(file, strText);

		// value
		_uint iBeginIndex = (_uint)strText.find("Environment");
		_uint iEndIndex = (_uint)strText.find('.');

		if ((_uint)strText.size() < iEndIndex - iBeginIndex)
		{
			MSG_BOX("Fail Path");
			return E_FAIL;
		}

		string	strPath = strFolderPath + strText.substr(iBeginIndex, iEndIndex - iBeginIndex);
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
bool CModel::SaveAssimpModel(const _char* filename)
{
	if (!m_pAIScene) return false;

	_char szModel[MAX_PATH] = {};
	_char szExt[MAX_PATH] = {};
	_splitpath_s(filename, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	strcat_s(szModel, sizeof(_char) * MAX_PATH, szExt);

	_string savePath = filename; 

	SaveModel modelData;
	modelData.MeshCount = m_pAIScene->mNumMeshes;
	modelData.MaterialCount = m_pAIScene->mNumMaterials;

	for (unsigned int i = 0; i < m_pAIScene->mNumMeshes; i++)
	{
		aiMesh* mesh = m_pAIScene->mMeshes[i];
		SaveMesh saveMesh{};
		
		saveMesh.MeshName =  mesh->mName.C_Str();
		saveMesh.MeshNameSize = (_int)saveMesh.MeshName.size() + 1;
		saveMesh.VertexCount = mesh->mNumVertices;
		saveMesh.IndexCount = mesh->mNumFaces * 3;
		saveMesh.MaterialIndex = mesh->mMaterialIndex;

		saveMesh.Vertices.resize(saveMesh.VertexCount);
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
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
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace& face = mesh->mFaces[i];
			for (unsigned int idx = 0; idx < face.mNumIndices; idx++)
				saveMesh.Indices.push_back(face.mIndices[idx]);
		}

		modelData.Meshes.push_back(saveMesh);
	}

	for (size_t i = 0; i < m_Materials.size(); i++)
	{
		modelData.Materials.push_back(m_Materials[i]->Get_SaveMaterial());
	}

	std::vector<SaveNode> allNodes;
	SaveNodeRecursive(m_pAIScene->mRootNode, allNodes, -1);

	modelData.AnimationCount = m_pAIScene->mNumAnimations;
	for (unsigned int i = 0; i < modelData.AnimationCount; i++)
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
	if (!fp) return false;

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
				fwrite(&Weights.Weight, sizeof(float), 1, fp);
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
			fwrite(&child, sizeof(int), 1, fp);
		}
	}

	for (auto& Anim : modelData.Animations)
	{
		fwrite(&Anim.AnimNameSize, sizeof(_uint), 1, fp);
		fwrite(Anim.AnimName.data(), 1,Anim.AnimNameSize, fp);
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
				fwrite(path.c_str(), sizeof(char), len, fp);
			}
		}
	}

	fclose(fp);

	return true;
}
_int CModel::SaveNodeRecursive(const aiNode* pAINode, std::vector<SaveNode>& outNodes, _int parentIndex)
{
	SaveNode node{};
	node.NodeName = pAINode->mName.C_Str();
	node.NodeNameSize =(_uint)node.NodeName.size() + 1;
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

	int currentIndex = static_cast<int>(outNodes.size());
	outNodes.push_back(node);

	for (size_t i = 0; i < pAINode->mNumChildren; i++)
	{
		int childIndex = SaveNodeRecursive(pAINode->mChildren[i], outNodes, currentIndex);
		outNodes[currentIndex].ChildrenIndices[i] = childIndex;
	}

	return currentIndex;
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

	if (MODEL::NONANIM == eType || MODEL::ENVIROMENT == eType) {
		iFlag |= aiProcess_PreTransformVertices;
	}
	m_iRootBoneIndex = iRootBoneIndex;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;


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
	else
	{
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

HRESULT CModel::Initialize_Prototype(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex)
{
	return Assimp_Model_Load(pModelFilePath, eType, PreTransformMatrix, iRootBoneIndex);
}
#endif // EDITOR_PROJECT

HRESULT CModel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CModel::Initialize_Prototype(MODEL eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
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
		Assimp_Model_Load(pModelFilePath, eType, PreTransformMatrix, 0);
		SaveAssimpModel(Temp);

		return S_OK;
	}
	m_pSaveModel = m_pGameInstance->Load_SaveModel(pModelFilePath);

	m_eType = eType;
	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	Ready_Bones(m_pSaveModel->Nodes, 0, -1);

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;



	return S_OK;
}



bool CModel::LoadData(const _char* filename)
{
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	if (!fp) return false;

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
			for (size_t j = 0;j < mesh.Bones[i].WeightsCount; j++)
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
			fread(&NewModel.Nodes[i].ChildrenIndices[j], sizeof(int), 1, fp);
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
		SaveMaterial mat;

		for (_uint k = 0; k < AI_TEXTURE_TYPE_MAX; k++)
		{
			_uint MaterialCount = 0;
			fread(&MaterialCount, sizeof(_uint), 1, fp);
			mat.Path[k].resize(MaterialCount);
			for (_uint j = 0; j < MaterialCount; j++)
			{
				_uint len = 0;
				fread(&len, sizeof(_uint), 1, fp);
				std::string temp;
				temp.resize(len);
				fread(&temp[0], sizeof(char), len, fp);
				mat.Path[k][j] = temp;
			}
		}
		
		NewModel.Materials.push_back(mat);
	}


	fclose(fp);

	m_SaveModel.push_back(NewModel);

	m_pGameInstance->Add_SaveModel(filename, m_SaveModel.back());

	return true;
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

	if (!pBone) return E_FAIL;

	m_Bones.push_back(pBone);

	_int myIndex = _int(m_Bones.size() - 1);

	for (size_t i = 0; i < saveNode.ChildrenIndices.size(); i++)
	{
		Ready_Bones(allNodes, saveNode.ChildrenIndices[i], myIndex);
	}
	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pSaveModel->AnimationCount;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(this, &m_pSaveModel->Animations[i]);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

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

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath,_fmatrix PreTransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
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

	m_SaveModel.clear();
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
