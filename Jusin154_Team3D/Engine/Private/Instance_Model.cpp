#include "pch.h"

#include "Instance_Model.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "ComputeShader.h"
#include "Bone.h"
#include "GameObject.h"

CInstance_Model::CInstance_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent{ pDevice  , pContext }
{
}

CInstance_Model::CInstance_Model(const CInstance_Model& rhs)
	: CComponent(rhs)
	, m_iInstanceStride{ rhs.m_iInstanceStride }
	, m_iNumInstance{ rhs.m_iNumInstance }
	, m_iNumBuffer{ rhs.m_iNumBuffer }
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes{ rhs.m_Meshes }
	, m_PreTransformMatrix{ rhs.m_PreTransformMatrix }
{
	for (auto& pMesh : m_Meshes) {
		SAFE_ADDREF(pMesh);
	}

	SAFE_ADDREF(m_pVBInstance);
}

#ifdef EDITOR_PROJECT

HRESULT CInstance_Model::Initialize_Prototype(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumBuffer = 2;

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

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

		SaveAssimpModel(Temp, m_pAIScene);
		return S_OK;
	}
#pragma region Mesh
	if (FAILED(Ready_Meshes())) {
		return E_FAIL;
	}

#pragma endregion

	return S_OK;
}
#endif

#ifndef EDITOR_PROJECT

HRESULT CInstance_Model::Initialize_Prototype(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumBuffer = 2;

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

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
	else {
		return E_FAIL;
	}
#pragma region Mesh
	if (FAILED(Ready_Meshes())) {
		return E_FAIL;
	}

#pragma endregion

	return S_OK;
}

#endif

#ifdef EDITOR_PROJECT

HRESULT CInstance_Model::Assimp_Model_Load(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex)
{
	_uint			iFlag = {};
	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

	if (MODEL::NONANIM == eType || MODEL::PBR_NONANIM == eType || MODEL::ENVIRONMENT == eType) {
		iFlag |= aiProcess_PreTransformVertices;
	}
	m_iRootBoneIndex = iRootBoneIndex;

	m_pAIScene = { nullptr };


	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;



#pragma region Mesh
	if (FAILED(Ready_Meshes(eType, m_pAIScene, PreTransformMatrix))) {
		return E_FAIL;
	}

	return S_OK;
#pragma endregion	
}
#endif

#ifdef EDITOR_PROJECT
HRESULT CInstance_Model::Ready_Meshes(MODEL eType, const aiScene* pAIScene, _fmatrix& PreTransformMatrix)
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
#endif

#ifdef EDITOR_PROJECT
_bool CInstance_Model::SaveAssimpModel(const _char* filename, const aiScene* pAIScene)
{
	if (!pAIScene) {
		return false;
	}

	_char szModel[MAX_PATH] = {};
	_char szExt[MAX_PATH] = {};
	_splitpath_s(filename, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	strcat_s(szModel, sizeof(_char) * MAX_PATH, szExt);

	_string savePath = filename;

	SaveModel modelData;
	modelData.MeshCount = pAIScene->mNumMeshes;

	for (_uint i = 0; i < pAIScene->mNumMeshes; i++)
	{
		aiMesh* mesh = pAIScene->mMeshes[i];
		SaveMesh saveMesh{};

		saveMesh.MeshName = mesh->mName.C_Str();
		saveMesh.MeshNameSize = (_int)saveMesh.MeshName.size() + 1;
		saveMesh.VertexCount = mesh->mNumVertices;
		saveMesh.IndexCount = mesh->mNumFaces * 3;
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

		saveMesh.Indices.reserve(saveMesh.IndexCount);
		for (_uint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace& face = mesh->mFaces[i];
			for (_uint idx = 0; idx < face.mNumIndices; idx++)
				saveMesh.Indices.push_back(face.mIndices[idx]);
		}

		modelData.Meshes.push_back(saveMesh);
	}


	FILE* fp = nullptr;
	fopen_s(&fp, savePath.c_str(), "wb");
	if (!fp) {
		return false;
	}

	fwrite(&modelData.MeshCount, sizeof(_uint), 1, fp);

	for (auto& mesh : modelData.Meshes)
	{
		fwrite(&mesh.MeshNameSize, sizeof(_uint), 1, fp);
		fwrite(mesh.MeshName.data(), 1, mesh.MeshNameSize, fp);
		fwrite(&mesh.VertexCount, sizeof(_uint), 1, fp);
		fwrite(&mesh.IndexCount, sizeof(_uint), 1, fp);

		fwrite(mesh.Vertices.data(), sizeof(SaveVertex), mesh.VertexCount, fp);
		fwrite(mesh.Indices.data(), sizeof(_uint), mesh.IndexCount, fp);
	}


	fclose(fp);

	return true;
}

#endif
HRESULT CInstance_Model::Ready_Meshes()
{
	m_iNumMeshes = m_SaveModel.MeshCount;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, nullptr, &m_SaveModel.Meshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

#ifdef EDITOR_PROJECT

HRESULT CInstance_Model::PreLoad(HANDLE hFile)
{
	DWORD dwByte = {};

	if (!ReadFile(hFile, &m_InstanceDesc, sizeof(PRE_INSTANCE_DESC), &dwByte, nullptr)) {
		return E_FAIL;
	}

	Change_NumInstance();

	return S_OK;

}
HRESULT CInstance_Model::Save_InstanceModel(HANDLE hFile)
{
	DWORD dwByte = {};

	if (!WriteFile(hFile, &m_InstanceDesc, sizeof(INSTANCE_DESC), &dwByte, nullptr)) {
		return E_FAIL;
	}
	return S_OK;
}

#endif

HRESULT CInstance_Model::Load_InstanceModel(INSTANCE_DESC InstanceDesc)
{

	m_InstanceDesc = InstanceDesc;

	if (m_InstanceDesc.fTimeMult <= 0)
		m_InstanceDesc.fTimeMult = 1.f;


	Change_NumInstance();

	return S_OK;
}

HRESULT CInstance_Model::Load_InstanceModel(HANDLE hFile)
{
	DWORD dwByte = {};

	if (!ReadFile(hFile, &m_InstanceDesc, sizeof(INSTANCE_DESC), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (m_InstanceDesc.fTimeMult <= 0)
		m_InstanceDesc.fTimeMult = 1.f;


	Change_NumInstance();

	return S_OK;
}

bool CInstance_Model::LoadData(const _char* filename)
{
	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");
	if (!fp) {
		return false;
	}

	SaveModel NewModel = {};

	fread(&NewModel.MeshCount, sizeof(_uint), 1, fp);

	for (_uint i = 0; i < NewModel.MeshCount; i++)
	{
		SaveMesh mesh{};

		fread(&mesh.MeshNameSize, sizeof(_uint), 1, fp);
		mesh.MeshName.resize(mesh.MeshNameSize);
		fread(mesh.MeshName.data(), 1, mesh.MeshNameSize, fp);

		fread(&mesh.VertexCount, sizeof(_uint), 1, fp);
		fread(&mesh.IndexCount, sizeof(_uint), 1, fp);

		mesh.Vertices.resize(mesh.VertexCount);
		fread(mesh.Vertices.data(), sizeof(SaveVertex), mesh.VertexCount, fp);

		mesh.Indices.resize(mesh.IndexCount);
		fread(mesh.Indices.data(), sizeof(_uint), mesh.IndexCount, fp);

		NewModel.Meshes.push_back(mesh);
	}

	fclose(fp);

	m_SaveModel = NewModel;

	return true;
}

HRESULT CInstance_Model::Change_NumInstance()
{
	SAFE_RELEASE(m_pVBInstance);
	SAFE_RELEASE(m_pComputeShader);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pParticleValueBuffer);

	//모든 버퍼를 지우고 재 생성 해야함

	if (FAILED(Create_Instance_Buffer()))
		return E_FAIL;

	if (FAILED(Create_SubResource_Buffer()))
		return E_FAIL;


	//시작 시에 인스턴트 버퍼를 구성 해줌
	Instane_Buffer_ReStruct();

	if (FAILED(Create_CS()))
		return E_FAIL;


	return S_OK;
}

HRESULT CInstance_Model::Create_Instance_Buffer()
{


	m_iNumInstance = m_InstanceDesc.iNumInstance;

	m_iInstanceStride = sizeof(VTX_INSTANCE_PARTICLE);

	D3D11_BUFFER_DESC		InstanceBufferDesc = {};

	InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	InstanceBufferDesc.MiscFlags = 0;


	if (FAILED(m_pDevice->CreateBuffer(&InstanceBufferDesc, nullptr, &m_pVBInstance)))
		return E_FAIL;



	return S_OK;
}

HRESULT CInstance_Model::Create_SubResource_Buffer()
{

	// 벨류 버퍼 생성하기
	D3D11_BUFFER_DESC Particle_Buffer_Desc = {};
	Particle_Buffer_Desc.ByteWidth = sizeof(CS_PARTICLE_VALUE_DESC) * m_iNumInstance;
	Particle_Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Particle_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Particle_Buffer_Desc.StructureByteStride = sizeof(CS_PARTICLE_VALUE_DESC);
	Particle_Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Particle_Buffer_Desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA	InitialDesc = {};
	InitialDesc.pSysMem = nullptr;


	if (FAILED(m_pDevice->CreateBuffer(&Particle_Buffer_Desc, nullptr, &m_pParticleValueBuffer)))
		return E_FAIL;


	//컨스턴트 버퍼 생성하기 

	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(CS_PARTICLE_DESC);

	D3D11_SUBRESOURCE_DATA	CBInitialDesc = {};

	if (FAILED(m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pConstantBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstance_Model::Create_CS()
{
	_uint		CS_InputStrides[] = {
	sizeof(VTX_INSTANCE_PARTICLE),
	sizeof(CS_PARTICLE_VALUE_DESC),
	};

	_uint		CS_OutputStrides[] = {
	sizeof(VTX_INSTANCE_PARTICLE),
	sizeof(CS_PARTICLE_VALUE_DESC),
	};

	//, 2, 2, CS_InputStrides, CS_OutputStrides

	CComputeShader::CS_INFO CS_Desc = {};

	CS_Desc.iNumElement = m_iNumInstance;
	CS_Desc.iNumInputBuffer = 2;
	CS_Desc.iNumOutputBuffer = 2;

	CS_Desc.iInputStructStride = CS_InputStrides;
	CS_Desc.iOutputStructStride = CS_OutputStrides;

	m_pComputeShader = (CComputeShader*)m_pGameInstance->Clone_Asset_Prototype(g_iStaticLevel, CS_EFFECT, &CS_Desc, nullptr);

	if (nullptr == m_pComputeShader) {
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CInstance_Model::Initialize(void* pArg)
{

	INSTANCE_DESC* pInstanceDesc = static_cast<INSTANCE_DESC*>(pArg);

	if (pInstanceDesc != nullptr)
		m_InstanceDesc = *pInstanceDesc;
	else
		return S_OK;

	if (FAILED(Create_Instance_Buffer()))
		return E_FAIL;

	if (FAILED(Create_SubResource_Buffer()))
		return E_FAIL;

	//시작 시에 인스턴트 버퍼를 구성 해줌
	Instane_Buffer_ReStruct();

	if (FAILED(Create_CS()))
		return E_FAIL;

	return S_OK;
}

void CInstance_Model::Compute_CS(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE ConstantSubResource = {};

	if (SUCCEEDED(m_pContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantSubResource)))
	{
		CS_PARTICLE_DESC* pDesc = static_cast<CS_PARTICLE_DESC*>(ConstantSubResource.pData);

		pDesc->fTimeDelta = fTimeDelta * m_InstanceDesc.fTimeMult;

		pDesc->isLoop = m_InstanceDesc.isLoop;
		pDesc->isDrop = m_InstanceDesc.isDrop;
		pDesc->isTurn = m_InstanceDesc.isTurn;
		pDesc->isAxisTurn = m_InstanceDesc.isAxisTurn;
		pDesc->isMoveForward = m_InstanceDesc.isMoveForward;
		pDesc->isSinWave = m_InstanceDesc.isSinWave;
		pDesc->isPivotMove = m_InstanceDesc.isPivotMove;
		pDesc->isSizeLerp = m_InstanceDesc.isSizeLerp;
		pDesc->isNoWorld = m_InstanceDesc.isNoWorld;
		pDesc->isMoveUp = m_InstanceDesc.isMoveUp;
		pDesc->isMoveRight = m_InstanceDesc.isMoveRight;
		pDesc->isDetphCompareStop = m_InstanceDesc.isDetphCompareStop;
		pDesc->isRandomAniIndex = m_InstanceDesc.isRandomAniIndex;
		pDesc->isExcludePos = m_InstanceDesc.isExcludePos;
		pDesc->isStop_Move_For_Depth_Compare = m_InstanceDesc.isStop_Move_For_Depth_Compare;
		pDesc->isNoPos = m_InstanceDesc.isNoPos;
		pDesc->isNoResetTime = m_InstanceDesc.isNoResetTime;
		pDesc->isLocal_Located_Not_TakeDelay = m_InstanceDesc.isLocal_Located_Not_TakeDelay;
		pDesc->isCompute_LocalInverse = m_InstanceDesc.isCompute_LocalInverse;
		pDesc->isRoundLengthLerp = m_InstanceDesc.isRoundLengthLerp;
		
		
		
		pDesc->WorldMatrix = *m_pOwner->Get_Component<CTransform>()->Get_WorldMatrixPtr();
		pDesc->fSizeLerpOption = m_InstanceDesc.fSizeLerpOption;
		pDesc->fMoveLerpOption = m_InstanceDesc.fMoveLerpOption;

		pDesc->ViewMatrix = *m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW);
		pDesc->ProjMatrix = *m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ);
		pDesc->fFar = *m_pGameInstance->Get_CurrentCameraFar();
		pDesc->vScreenSize = m_pGameInstance->Get_ViewPortSize();



		m_pContext->Unmap(m_pConstantBuffer, 0);
	}

	if (m_pComputeShader == nullptr)
		return;

	_uint iGroupCountX = (m_iNumInstance + 255) / 256;

	ID3D11Buffer* CSBuffers[] = {
		 m_pVBInstance,
		 m_pParticleValueBuffer

	};

	vector<D3D11_MAPPED_SUBRESOURCE> OutSubResources = {};
	D3D11_MAPPED_SUBRESOURCE VBInstanceResource = {};

	m_pGameInstance->Bind_CS_RenderTarget(2, L"Target_Depth");

	OutSubResources = m_pComputeShader->Dispatch(0, 0, _float3((_float)iGroupCountX, 1.f, 1.f), CSBuffers, m_pConstantBuffer);

}


HRESULT CInstance_Model::Render(_uint iMeshIndx)
{

	if (FAILED(m_Meshes[iMeshIndx]->Bind_Resources_Instance(m_pVBInstance, m_iInstanceStride, m_iNumBuffer)))
		return E_FAIL;

	if (FAILED(m_Meshes[iMeshIndx]->Render_Instance(m_iNumInstance)))
		return E_FAIL;


	return S_OK;
}

void CInstance_Model::Instane_Buffer_ReStruct()
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};
	D3D11_MAPPED_SUBRESOURCE		ParticleValue_SubResource{};

	_float4x4 IdentityMat = {};
	XMStoreFloat4x4(&IdentityMat, XMMatrixIdentity());

	if (SUCCEEDED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
	{
		VTX_INSTANCE_PARTICLE* pVertices = static_cast<VTX_INSTANCE_PARTICLE*>(SubResource.pData);

		if (SUCCEEDED(m_pContext->Map(m_pParticleValueBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ParticleValue_SubResource)))
		{

			CS_PARTICLE_VALUE_DESC* pParticleValues = static_cast<CS_PARTICLE_VALUE_DESC*>(ParticleValue_SubResource.pData);

			for (size_t i = 0; i < m_iNumInstance; i++)
			{

				//스케일 설정 
				_float3			vScale = _float3(
					m_pGameInstance->Random_Float(m_InstanceDesc.vSizeMin.x, m_InstanceDesc.vSizeMax.x),
					m_pGameInstance->Random_Float(m_InstanceDesc.vSizeMin.y, m_InstanceDesc.vSizeMax.y),
					m_pGameInstance->Random_Float(m_InstanceDesc.vSizeMin.z, m_InstanceDesc.vSizeMax.z)
				);

				_matrix ScaleMatrix = XMMatrixScaling(vScale.x, vScale.y, vScale.z);

				//로테이션 설정
				_float3			vRoataion = _float3(
					m_pGameInstance->Random_Float(m_InstanceDesc.vRotationAngleMin.x, m_InstanceDesc.vRotationAngleMax.x),
					m_pGameInstance->Random_Float(m_InstanceDesc.vRotationAngleMin.y, m_InstanceDesc.vRotationAngleMax.y),
					m_pGameInstance->Random_Float(m_InstanceDesc.vRotationAngleMin.z, m_InstanceDesc.vRotationAngleMax.z)
				);

				_matrix RotationMatrix = XMMatrixRotationRollPitchYaw(
					XMConvertToRadians(vRoataion.x),
					XMConvertToRadians(vRoataion.y),
					XMConvertToRadians(vRoataion.z)
				);


				//위치 설정
				_float4x4 SRMatrix = {};

				XMStoreFloat4x4(&SRMatrix, ScaleMatrix * RotationMatrix);

				memcpy(&pVertices[i].vRight, SRMatrix.m[0], sizeof(_float4));
				memcpy(&pVertices[i].vUp, SRMatrix.m[1], sizeof(_float4));
				memcpy(&pVertices[i].vLook, SRMatrix.m[2], sizeof(_float4));

				/* 구위의 한점으로 위치를 정하기 위한 반지름 */
				_float fRoundRange = m_pGameInstance->Random_Float(m_InstanceDesc.vRoundRangeLength.x, m_InstanceDesc.vRoundRangeLength.y);
				_float fAzimuthAngle = m_pGameInstance->Random_Float(m_InstanceDesc.vAzimuthAngle.x, m_InstanceDesc.vAzimuthAngle.y);
				_float fPolarAngle = m_pGameInstance->Random_Float(m_InstanceDesc.vPolarAngle.x, m_InstanceDesc.vPolarAngle.y);

				pVertices[i].vTranslation = _float4(
					m_pGameInstance->Random_Float(m_InstanceDesc.vCenter.x - m_InstanceDesc.vRange.x * 0.5f, m_InstanceDesc.vCenter.x + m_InstanceDesc.vRange.x * 0.5f)
					 + fRoundRange * cosf(XMConvertToRadians(fAzimuthAngle)) * sinf(XMConvertToRadians(fPolarAngle)) ,
					m_pGameInstance->Random_Float(m_InstanceDesc.vCenter.y - m_InstanceDesc.vRange.y * 0.5f, m_InstanceDesc.vCenter.y + m_InstanceDesc.vRange.y * 0.5f)
					+ fRoundRange * sinf(XMConvertToRadians(fAzimuthAngle)) * sinf(XMConvertToRadians(fPolarAngle)),
					m_pGameInstance->Random_Float(m_InstanceDesc.vCenter.z - m_InstanceDesc.vRange.z * 0.5f, m_InstanceDesc.vCenter.z + m_InstanceDesc.vRange.z * 0.5f)
					+ fRoundRange * cosf(XMConvertToRadians(fPolarAngle)),
					1.f);

				
				pParticleValues[i].fRoundRangeLength = fRoundRange;
				pParticleValues[i].fAzimuthAngle = XMConvertToRadians(fAzimuthAngle);
				pParticleValues[i].fPolarAngle = XMConvertToRadians(fPolarAngle);
				pParticleValues[i].fRoundLengthLerpSpeed = m_InstanceDesc.fRoundLengthLerpSpeed;


				_float3			vSinAmount = _float3(
					m_pGameInstance->Random_Float(m_InstanceDesc.vSinMinAmount.x, m_InstanceDesc.vSinMaxAmount.x),
					m_pGameInstance->Random_Float(m_InstanceDesc.vSinMinAmount.y, m_InstanceDesc.vSinMaxAmount.y),
					m_pGameInstance->Random_Float(m_InstanceDesc.vSinMinAmount.z, m_InstanceDesc.vSinMaxAmount.z)
				);

				pParticleValues[i].vSinAmount = vSinAmount;

				_float3			vDeltaAngle = _float3(
					m_pGameInstance->Random_Float(m_InstanceDesc.vDeltaAngleMin.x, m_InstanceDesc.vDeltaAngleMax.x),
					m_pGameInstance->Random_Float(m_InstanceDesc.vDeltaAngleMin.y, m_InstanceDesc.vDeltaAngleMax.y),
					m_pGameInstance->Random_Float(m_InstanceDesc.vDeltaAngleMin.z, m_InstanceDesc.vDeltaAngleMax.z)
				);

				pParticleValues[i].vDeltaAngle = vDeltaAngle;

				_float3			vDeltaAxisAngle = _float3(
					m_pGameInstance->Random_Float(m_InstanceDesc.vDeltaAxisAngleMin.x, m_InstanceDesc.vDeltaAxisAngleMax.x),
					m_pGameInstance->Random_Float(m_InstanceDesc.vDeltaAxisAngleMin.y, m_InstanceDesc.vDeltaAxisAngleMax.y),
					m_pGameInstance->Random_Float(m_InstanceDesc.vDeltaAxisAngleMin.z, m_InstanceDesc.vDeltaAxisAngleMax.z)
				);

				pParticleValues[i].vDeltaAxisAngle = vDeltaAxisAngle;

				_float3			vPivot = _float3(
					m_pGameInstance->Random_Float(m_InstanceDesc.vPivotMin.x, m_InstanceDesc.vPivotMax.x),
					m_pGameInstance->Random_Float(m_InstanceDesc.vPivotMin.y, m_InstanceDesc.vPivotMax.y),
					m_pGameInstance->Random_Float(m_InstanceDesc.vPivotMin.z, m_InstanceDesc.vPivotMax.z)
				);

				pParticleValues[i].vPivot = vPivot;

				_float3			vVelocity = _float3(
					m_pGameInstance->Random_Float(m_InstanceDesc.vVelocityMin.x, m_InstanceDesc.vVelocityMax.x),
					m_pGameInstance->Random_Float(m_InstanceDesc.vVelocityMin.y, m_InstanceDesc.vVelocityMax.y),
					m_pGameInstance->Random_Float(m_InstanceDesc.vVelocityMin.z, m_InstanceDesc.vVelocityMax.z)
				);

				pParticleValues[i].vVelocity = vVelocity;

				_float3			vSizeLerp = _float3(
					m_InstanceDesc.vDeltaSize.x,
					m_InstanceDesc.vDeltaSize.y,
					m_InstanceDesc.vDeltaSize.z
				);

				pParticleValues[i].vDeltaSize = vSizeLerp;


				_float3			vWolrdOffset = _float3(
					m_pGameInstance->Random_Float(m_InstanceDesc.vWolrdOffsetMin.x, m_InstanceDesc.vWolrdOffsetMax.x),
					m_pGameInstance->Random_Float(m_InstanceDesc.vWolrdOffsetMin.y, m_InstanceDesc.vWolrdOffsetMax.y),
					m_pGameInstance->Random_Float(m_InstanceDesc.vWolrdOffsetMin.z, m_InstanceDesc.vWolrdOffsetMax.z)
				);


				pParticleValues[i].vWolrdOffset = vWolrdOffset;

				//라이프 타임 설정
				pVertices[i].vLifeTime = _float2(0.0f, m_pGameInstance->Random_Float(m_InstanceDesc.vLifeTime.x, m_InstanceDesc.vLifeTime.y));

				pParticleValues[i].vAniTime = _float2(0.0f, m_pGameInstance->Random_Float(m_InstanceDesc.vAniTime.x, m_InstanceDesc.vAniTime.y));
				pParticleValues[i].vMaskingUVMoveTime = _float2(0.0f, m_pGameInstance->Random_Float(m_InstanceDesc.vMaskingUVMoveTime.x, m_InstanceDesc.vMaskingUVMoveTime.y));
				pParticleValues[i].vDiffuseUVMoveTime = _float2(0.0f, m_pGameInstance->Random_Float(m_InstanceDesc.vDiffuseUVMoveTime.x, m_InstanceDesc.vDiffuseUVMoveTime.y));
				pParticleValues[i].vDistortionUVMoveTime = _float2(0.0f, m_pGameInstance->Random_Float(m_InstanceDesc.vDistortionUVMoveTime.x, m_InstanceDesc.vDistortionUVMoveTime.y));
				pParticleValues[i].vNoiseUVMoveTime = _float2(0.0f, m_pGameInstance->Random_Float(m_InstanceDesc.vNoiseUVMoveTime.x, m_InstanceDesc.vNoiseUVMoveTime.y));
				pParticleValues[i].vDissolveUVMoveTime = _float2(0.0f, m_pGameInstance->Random_Float(m_InstanceDesc.vDissolveUVMoveTime.x, m_InstanceDesc.vDissolveUVMoveTime.y));
				pParticleValues[i].fSpeed = m_pGameInstance->Random_Float(m_InstanceDesc.vSpeed.x, m_InstanceDesc.vSpeed.y);
				pParticleValues[i].fRotaionSpeed = m_pGameInstance->Random_Float(m_InstanceDesc.vRotationSpeed.x, m_InstanceDesc.vRotationSpeed.y);
				pParticleValues[i].vAniIndex = _float2(0.f, m_InstanceDesc.vAniIndex.y);
				pParticleValues[i].fGravity = m_pGameInstance->Random_Float(m_InstanceDesc.vGravity.x, m_InstanceDesc.vGravity.y);
				pParticleValues[i].fDrag = m_pGameInstance->Random_Float(m_InstanceDesc.vDrag.x, m_InstanceDesc.vDrag.y);
				pParticleValues[i].fSizeDrag = m_pGameInstance->Random_Float(m_InstanceDesc.vSizeDrag.x, m_InstanceDesc.vSizeDrag.y);
				pParticleValues[i].vDelay = _float2(0.0f, m_pGameInstance->Random_Float(m_InstanceDesc.vDelay.x, m_InstanceDesc.vDelay.y));
				pParticleValues[i].fDropAttenuation = m_pGameInstance->Random_Float(m_InstanceDesc.vDropAttenuation.x, m_InstanceDesc.vDropAttenuation.y);
				pParticleValues[i].fAcceleration = _float(m_pGameInstance->Random_Float(m_InstanceDesc.vAcceleration.x, m_InstanceDesc.vAcceleration.y));
				pParticleValues[i].fRotateAttenuation = _float(m_pGameInstance->Random_Float(m_InstanceDesc.vRotateAttenuation.x, m_InstanceDesc.vRotateAttenuation.y));
				pParticleValues[i].fRotateAttDelay = _float(m_pGameInstance->Random_Float(m_InstanceDesc.vRotateAttDelay.x, m_InstanceDesc.vRotateAttDelay.y));
				
				pParticleValues[i].fCollisionTime = 0.f;
				pParticleValues[i].isCompareStop = false;
				pParticleValues[i].isStop = false;
				pParticleValues[i].fLoopCount = 0.f;
				

				memcpy(&pParticleValues[i].vOriginRight, SRMatrix.m[0], sizeof(_float4));
				memcpy(&pParticleValues[i].vOriginUp, SRMatrix.m[1], sizeof(_float4));
				memcpy(&pParticleValues[i].vOriginLook, SRMatrix.m[2], sizeof(_float4));
				memcpy(&pParticleValues[i].vOriginTranslation, &pVertices[i].vTranslation, sizeof(_float4));

				memcpy(&pParticleValues[i].PreWorldMatrix, &IdentityMat, sizeof(_float4x4));
				memcpy(&pParticleValues[i].LocalMatrixInv, &IdentityMat, sizeof(_float4x4));
				

				if (m_InstanceDesc.isRandomAniIndex == true)
				{
					pParticleValues[i].vAniIndex = _float2((_float)m_pGameInstance->Random_Int(0, (_int)m_InstanceDesc.vAniIndex.y) , m_InstanceDesc.vAniIndex.y);
				}


			}

			m_pContext->Unmap(m_pParticleValueBuffer, 0);

		}
		m_pContext->Unmap(m_pVBInstance, 0);
	}


}

HRESULT CInstance_Model::Bind_CS_Output(_uint Index, _uint iBufferIndex)
{
	if (m_pComputeShader == nullptr)
		return E_FAIL;

	m_pComputeShader->Bind_OutPut_SRV(Index, iBufferIndex);

	return S_OK;
}

HRESULT CInstance_Model::Bind_OutPut_SRV_VS(_uint Index, _uint iBufferIndex)
{
	if (m_pComputeShader == nullptr)
		return E_FAIL;

	m_pComputeShader->Bind_OutPut_SRV_VS(Index, iBufferIndex);

	return S_OK;

}

CInstance_Model* CInstance_Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex)
{
	CInstance_Model* pInstance = new CInstance_Model(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, eType, PreTransformMatrix, iRootBoneIndex)))
	{
		MSG_BOX("Failed to Created : CInstance_Model");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CInstance_Model::Clone(void* pArg, CGameObject* pOwner)
{
	CInstance_Model* pInstance = new CInstance_Model(*this);
	pInstance->m_pOwner = pOwner;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInstance_Model");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


void CInstance_Model::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVBInstance);

	for (auto& pMesh : m_Meshes)
		SAFE_RELEASE(pMesh);

	SAFE_RELEASE(m_pComputeShader);
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pParticleValueBuffer);
	;
}
#ifdef _DEBUG

void CInstance_Model::Describe_Entity()
{
	if (ImGui::TreeNode("Model Option"))
	{
		const char* pLerp[] = { "Linear" , "EaseInQuad", "EaseOutQuad", "EaseInCubic" , "EaseOutCubic" , "EaseInOutSin" , "EaseInBack" , "Expo" , "Circle" };

		ImGui::Separator(); ImGui::Spacing();
		ImGui::PushItemWidth(120);

		if (ImGui::InputInt("NumInstance", &m_InstanceDesc.iNumInstance))
		{
			Change_NumInstance();
		}

		if (GUI::Checkbox("Loop", &m_InstanceDesc.isLoop))
		{
			Instane_Buffer_ReStruct();
		}

		if (GUI::Checkbox("NoWorld", &m_InstanceDesc.isNoWorld))
		{
			Instane_Buffer_ReStruct();
		}

		if (GUI::Checkbox("NoPos", &m_InstanceDesc.isNoPos))
		{
			Instane_Buffer_ReStruct();
		}
		
		if (GUI::Checkbox("Local_Located_Not_TakeDelay", &m_InstanceDesc.isLocal_Located_Not_TakeDelay))
		{
			Instane_Buffer_ReStruct();

		}

		if (GUI::Checkbox("Compute_LocalInverse", &m_InstanceDesc.isCompute_LocalInverse))
		{
			Instane_Buffer_ReStruct();

		}
		

		if (GUI::Checkbox("RandomIndex", &m_InstanceDesc.isRandomAniIndex))
		{
			Instane_Buffer_ReStruct();
		}


		if (GUI::TreeNode("Transform"))
		{

			if (ImGui::DragFloat3("Center", reinterpret_cast<_float*>(&m_InstanceDesc.vCenter)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat3("Range", reinterpret_cast<_float*>(&m_InstanceDesc.vRange)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat3("SizeMin", reinterpret_cast<_float*>(&m_InstanceDesc.vSizeMin)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat3("SizeMax", reinterpret_cast<_float*>(&m_InstanceDesc.vSizeMax)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat3("RotationMin", reinterpret_cast<_float*>(&m_InstanceDesc.vRotationAngleMin)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat3("RotationMax", reinterpret_cast<_float*>(&m_InstanceDesc.vRotationAngleMax)))
			{
				Instane_Buffer_ReStruct();
			}


			if (ImGui::DragFloat3("WolrdOffsetMin", reinterpret_cast<_float*>(&m_InstanceDesc.vWolrdOffsetMin)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat3("WolrdOffsetMax", reinterpret_cast<_float*>(&m_InstanceDesc.vWolrdOffsetMax)))
			{
				Instane_Buffer_ReStruct();
			}


			if (GUI::TreeNode("Round Range"))
			{
	

				if (ImGui::DragFloat2("RoundRangeLength", reinterpret_cast<_float*>(&m_InstanceDesc.vRoundRangeLength)))
				{
					Instane_Buffer_ReStruct();
				}

				if (ImGui::DragFloat2("AzimuthAngle", reinterpret_cast<_float*>(&m_InstanceDesc.vAzimuthAngle)))
				{
					Instane_Buffer_ReStruct();
				}

				if (ImGui::DragFloat2("PolarAngle", reinterpret_cast<_float*>(&m_InstanceDesc.vPolarAngle)))
				{
					Instane_Buffer_ReStruct();
				}

				if (GUI::Checkbox("RoundLengthLerp", &m_InstanceDesc.isRoundLengthLerp))
				{
					Instane_Buffer_ReStruct();
				}

				if (ImGui::DragFloat("fRoundLengthLerpSpeed", reinterpret_cast<_float*>(&m_InstanceDesc.fRoundLengthLerpSpeed)))
				{
					Instane_Buffer_ReStruct();
				}

				GUI::TreePop();
			}

			GUI::TreePop();
		}


		if (GUI::TreeNode("Move"))
		{
			_int iMoveLerpOption = (_int)m_InstanceDesc.fMoveLerpOption;

			if (ImGui::Combo("Move Lerp Option", &iMoveLerpOption, pLerp, 9))
			{
				Instane_Buffer_ReStruct();

				m_InstanceDesc.fMoveLerpOption = (_float)iMoveLerpOption;
			}

			if (ImGui::DragFloat2("Speed", reinterpret_cast<_float*>(&m_InstanceDesc.vSpeed)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat2("Drag", reinterpret_cast<_float*>(&m_InstanceDesc.vDrag)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat2("Acceleration", reinterpret_cast<_float*>(&m_InstanceDesc.vAcceleration)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat3("VelocityMin", reinterpret_cast<_float*>(&m_InstanceDesc.vVelocityMin)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat3("VelocityMax", reinterpret_cast<_float*>(&m_InstanceDesc.vVelocityMax)))
			{
				Instane_Buffer_ReStruct();
			}


			if (GUI::TreeNode("Dir Move"))
			{


				if (GUI::Checkbox("Forward Move", &m_InstanceDesc.isMoveForward))
				{
					Instane_Buffer_ReStruct();
				}

				if (GUI::Checkbox("Up Move", &m_InstanceDesc.isMoveUp))
				{
					Instane_Buffer_ReStruct();
				}

				if (GUI::Checkbox("Right Move", &m_InstanceDesc.isMoveRight))
				{
					Instane_Buffer_ReStruct();
				}

				GUI::TreePop();
			}

			if (GUI::TreeNode("Pivot Move"))
			{

				if (GUI::Checkbox("ExcludePos From PibotMove", &m_InstanceDesc.isExcludePos))
				{
					Instane_Buffer_ReStruct();
				}

				if (GUI::Checkbox("PivotMove", &m_InstanceDesc.isPivotMove))
				{
					Instane_Buffer_ReStruct();
				}

				if (ImGui::DragFloat3("PivotMin", reinterpret_cast<_float*>(&m_InstanceDesc.vPivotMin)))
				{
					Instane_Buffer_ReStruct();
				}

				if (ImGui::DragFloat3("PivotMax", reinterpret_cast<_float*>(&m_InstanceDesc.vPivotMax)))
				{
					Instane_Buffer_ReStruct();
				}


				GUI::TreePop();
			}

			if (GUI::TreeNode("Drop Move"))
			{
				if (GUI::Checkbox("Drop", &m_InstanceDesc.isDrop))
				{
					Instane_Buffer_ReStruct();
				}

				if (ImGui::DragFloat2("Gravity", reinterpret_cast<_float*>(&m_InstanceDesc.vGravity)))
				{
					Instane_Buffer_ReStruct();
				}

				if (ImGui::DragFloat2("DropAttenuation", reinterpret_cast<_float*>(&m_InstanceDesc.vDropAttenuation)))
				{
					Instane_Buffer_ReStruct();
				}

				GUI::TreePop();
			}
			GUI::TreePop();
		}


		if (GUI::TreeNode("Rotate"))
		{

			if (ImGui::DragFloat2("RotationSpeed", reinterpret_cast<_float*>(&m_InstanceDesc.vRotationSpeed)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat2("RotateAttenuation", reinterpret_cast<_float*>(&m_InstanceDesc.vRotateAttenuation)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat2("RotateAttDelay", reinterpret_cast<_float*>(&m_InstanceDesc.vRotateAttDelay)))
			{
				Instane_Buffer_ReStruct();
			}
			
			

			if (GUI::TreeNode("Nomal Turn"))
			{
				if (GUI::Checkbox("Turn", &m_InstanceDesc.isTurn))
				{
					Instane_Buffer_ReStruct();
				}

				if (ImGui::DragFloat3("DeltaAngleMin", reinterpret_cast<_float*>(&m_InstanceDesc.vDeltaAngleMin)))
				{
					Instane_Buffer_ReStruct();
				}

				if (ImGui::DragFloat3("DeltaAngleMax", reinterpret_cast<_float*>(&m_InstanceDesc.vDeltaAngleMax)))
				{
					Instane_Buffer_ReStruct();
				}

				GUI::TreePop();
			}


			if (GUI::TreeNode("Axis Turn"))
			{

				if (GUI::Checkbox("AxisTurn", &m_InstanceDesc.isAxisTurn))
				{
					Instane_Buffer_ReStruct();
				}




				if (ImGui::DragFloat3("DeltaAxisAngleMin", reinterpret_cast<_float*>(&m_InstanceDesc.vDeltaAxisAngleMin)))
				{
					Instane_Buffer_ReStruct();
				}

				if (ImGui::DragFloat3("DeltaAxisAngleMax", reinterpret_cast<_float*>(&m_InstanceDesc.vDeltaAxisAngleMax)))
				{
					Instane_Buffer_ReStruct();
				}


				GUI::TreePop();
			}

			if (GUI::TreeNode("Sin Turn"))
			{


				if (GUI::Checkbox("Sin Wave", &m_InstanceDesc.isSinWave))
				{
					Instane_Buffer_ReStruct();
				}

				if (ImGui::DragFloat3("SinWaveMin", reinterpret_cast<_float*>(&m_InstanceDesc.vSinMinAmount)))
				{
					Instane_Buffer_ReStruct();
				}

				if (ImGui::DragFloat3("SinWaveMax", reinterpret_cast<_float*>(&m_InstanceDesc.vSinMaxAmount)))
				{
					Instane_Buffer_ReStruct();
				}


				GUI::TreePop();
			}

			GUI::TreePop();
		}


		if (GUI::TreeNode("Size"))
		{

			if (GUI::Checkbox("SizeLerp", &m_InstanceDesc.isSizeLerp))
			{
				Instane_Buffer_ReStruct();
			}

			_int iSizeLerpOption = (_int)m_InstanceDesc.fSizeLerpOption;

			if (ImGui::Combo("Size Lerp Option", &iSizeLerpOption, pLerp, 9))
			{
				Instane_Buffer_ReStruct();

				m_InstanceDesc.fSizeLerpOption = (_float)iSizeLerpOption;
			}

			if (ImGui::DragFloat3("LerpSizeAmount", reinterpret_cast<_float*>(&m_InstanceDesc.vDeltaSize)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat2("Size Drag", reinterpret_cast<_float*>(&m_InstanceDesc.vSizeDrag)))
			{
				Instane_Buffer_ReStruct();
			}


			GUI::TreePop();
		}


		if (GUI::TreeNode("Time"))
		{

			if (GUI::Checkbox("NoResetTime", &m_InstanceDesc.isNoResetTime))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat("TimeMult", reinterpret_cast<_float*>(&m_InstanceDesc.fTimeMult)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat2("LifeTime", reinterpret_cast<_float*>(&m_InstanceDesc.vLifeTime)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat2("Delay", reinterpret_cast<_float*>(&m_InstanceDesc.vDelay)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat2("DiffuseUVMoveTime", reinterpret_cast<_float*>(&m_InstanceDesc.vDiffuseUVMoveTime)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat2("MaskingUVMoveTime", reinterpret_cast<_float*>(&m_InstanceDesc.vMaskingUVMoveTime)))
			{
				Instane_Buffer_ReStruct();
			}


			if (ImGui::DragFloat2("DistortionUVMoveTime", reinterpret_cast<_float*>(&m_InstanceDesc.vDistortionUVMoveTime)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat2("NoiseUVMoveTime", reinterpret_cast<_float*>(&m_InstanceDesc.vNoiseUVMoveTime)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat2("DissolveUVMoveTime", reinterpret_cast<_float*>(&m_InstanceDesc.vDissolveUVMoveTime)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat2("AniTime", reinterpret_cast<_float*>(&m_InstanceDesc.vAniTime)))
			{
				Instane_Buffer_ReStruct();
			}

			if (ImGui::DragFloat2("AniIndex", reinterpret_cast<_float*>(&m_InstanceDesc.vAniIndex)))
			{
				Instane_Buffer_ReStruct();
			}

			GUI::TreePop();
		}

		if (GUI::TreeNode("Depth Compare"))
		{
			if (GUI::Checkbox("DetphCompareStop", &m_InstanceDesc.isDetphCompareStop))
			{
				Instane_Buffer_ReStruct();
			}


			if (GUI::Checkbox("Stop_Move_For_Depth_Compare", &m_InstanceDesc.isStop_Move_For_Depth_Compare))
			{
				Instane_Buffer_ReStruct();
			}

			GUI::TreePop();
		}
		

	



		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	ImGui::Separator();
}

#endif // _DEBUG
