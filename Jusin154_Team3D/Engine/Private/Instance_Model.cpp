#include "pch.h"

#include "Instance_Model.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "ComputeShader.h"
#include "Bone.h"

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
{
	for (auto& pMesh : m_Meshes) {
		SAFE_ADDREF(pMesh);
	}

	Safe_AddRef(m_pVBInstance);
}
#ifdef EDITOR_PROJECT
HRESULT CInstance_Model::Initialize_Prototype(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumBuffer = 2;

	if (FAILED(Assimp_Model_Load(pModelFilePath, eType, PreTransformMatrix, iRootBoneIndex)))
		return E_FAIL;

	return S_OK;
}
HRESULT CInstance_Model::Assimp_Model_Load(const _char* pModelFilePath, MODEL eType, _fmatrix& PreTransformMatrix, _uint iRootBoneIndex)
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

#pragma region Mesh
	if (FAILED(Ready_Meshes(eType, pAIScene, PreTransformMatrix))) {
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

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
HRESULT CInstance_Model::Save_InstanceModel(HANDLE hFile)
{
	DWORD dwByte = {};

	if (!WriteFile(hFile, &m_InstanceDesc, sizeof(INSTANCE_DESC), &dwByte, nullptr)) {
		return E_FAIL;
	}
	return S_OK;
}

#endif

HRESULT CInstance_Model::Load_InstanceModel(HANDLE hFile)
{
	DWORD dwByte = {};

	if (!ReadFile(hFile, &m_InstanceDesc, sizeof(INSTANCE_DESC), &dwByte, nullptr)) {
		return E_FAIL;
	}

	Change_NumInstance();

	return S_OK;
}

HRESULT CInstance_Model::Change_NumInstance()
{
	Safe_Release(m_pVBInstance);
	Safe_Release(m_pComputeShader);
	Safe_Release(m_pConstantBuffer);
	Safe_Release(m_pParticleValueBuffer);

	//모든 버퍼를 지우고 재 생성 해야함

	if (FAILED(Create_Instance_Buffer()))
		return E_FAIL;

	if (FAILED(Create_SubResource_Buffer()))
		return E_FAIL;

	Instane_Buffer_ReStruct();

	if (FAILED(Create_CS()))
		return E_FAIL;

	//시작 시에 인스턴트 버퍼를 구성 해줌





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

	m_pComputeShader = CComputeShader::Create(m_pDevice, m_pContext,
		L"../Bin/Resources/ShaderFiles/Shader_Particle_Compute.hlsl", "CS_MAIN", m_iNumInstance, 2, 2, CS_OutputStrides, CS_InputStrides);

	if (m_pComputeShader == nullptr)
		return E_FAIL;

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

	if(FAILED(Create_CS()))
		return E_FAIL;

	return S_OK;
}

void CInstance_Model::Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE ConstantSubResource = {};

	if (SUCCEEDED(m_pContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantSubResource)))
	{
		CS_PARTICLE_DESC* pDesc = static_cast<CS_PARTICLE_DESC*>(ConstantSubResource.pData);

		pDesc->fTimeDelta = fTimeDelta;
		pDesc->isLoop = m_InstanceDesc.isLoop;
		pDesc->isBillboard = m_InstanceDesc.isBillboard;
		pDesc->CamViewInvMatrix = *m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW_INV);

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

	OutSubResources = m_pComputeShader->Dispatch(0, 0, _float3((_float)iGroupCountX, 1.f, 1.f), CSBuffers, m_pConstantBuffer);

	if (SUCCEEDED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &VBInstanceResource)))
	{

		memcpy(VBInstanceResource.pData, OutSubResources[0].pData, m_iInstanceStride * m_InstanceDesc.iNumInstance); // 아웃풋 버퍼에 들어온 값들을 전부 복사한다.

		m_pContext->Unmap(m_pVBInstance, 0);
	}

	D3D11_MAPPED_SUBRESOURCE ParticleValueResource = {};

	if (SUCCEEDED(m_pContext->Map(m_pParticleValueBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ParticleValueResource)))
	{
		CS_PARTICLE_VALUE_DESC* pValueDesc = static_cast<CS_PARTICLE_VALUE_DESC*>(OutSubResources[1].pData);

		memcpy(ParticleValueResource.pData, OutSubResources[1].pData, sizeof(CS_PARTICLE_VALUE_DESC) * m_InstanceDesc.iNumInstance); // 아웃풋 버퍼에 들어온 값들을 전부 복사한다.

		m_pContext->Unmap(m_pParticleValueBuffer, 0);
	}


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
					XMConvertToDegrees(vRoataion.x),
					XMConvertToDegrees(vRoataion.y),
					XMConvertToDegrees(vRoataion.z)
				);


				//위치 설정
				_float4x4 SRMatrix = {};

				XMStoreFloat4x4(&SRMatrix, ScaleMatrix * RotationMatrix);

				memcpy(&pVertices[i].vRight, SRMatrix.m[0], sizeof(_float4));
				memcpy(&pVertices[i].vUp, SRMatrix.m[1], sizeof(_float4));
				memcpy(&pVertices[i].vLook, SRMatrix.m[2], sizeof(_float4));


				pVertices[i].vTranslation = _float4(
					m_pGameInstance->Random_Float(m_InstanceDesc.vCenter.x - m_InstanceDesc.vRange.x * 0.5f, m_InstanceDesc.vCenter.x + m_InstanceDesc.vRange.x * 0.5f),
					m_pGameInstance->Random_Float(m_InstanceDesc.vCenter.y - m_InstanceDesc.vRange.y * 0.5f, m_InstanceDesc.vCenter.y + m_InstanceDesc.vRange.y * 0.5f),
					m_pGameInstance->Random_Float(m_InstanceDesc.vCenter.z - m_InstanceDesc.vRange.z * 0.5f, m_InstanceDesc.vCenter.z + m_InstanceDesc.vRange.z * 0.5f),
					1.f);

				//라이프 타임 설정
				pVertices[i].vLifeTime = _float2(0.0f, m_pGameInstance->Random_Float(m_InstanceDesc.vLifeTime.x, m_InstanceDesc.vLifeTime.y));

				pParticleValues[i].vAniTime = _float2(0.0f, m_pGameInstance->Random_Float(m_InstanceDesc.vAniTime.x, m_InstanceDesc.vAniTime.y));
				pParticleValues[i].vMaskingUVMoveTime = _float2(0.0f, m_pGameInstance->Random_Float(m_InstanceDesc.vMaskingUVMoveTime.x, m_InstanceDesc.vMaskingUVMoveTime.y));
				pParticleValues[i].vDiffuseUVMoveTime = _float2(0.0f, m_pGameInstance->Random_Float(m_InstanceDesc.vDiffuseUVMoveTime.x, m_InstanceDesc.vDiffuseUVMoveTime.y));
				pParticleValues[i].vNoiseUVMoveTime = _float2(0.0f, m_pGameInstance->Random_Float(m_InstanceDesc.vNoiseUVMoveTime.x, m_InstanceDesc.vNoiseUVMoveTime.y));
				pParticleValues[i].fSpeed = m_pGameInstance->Random_Float(m_InstanceDesc.vSpeed.x, m_InstanceDesc.vSpeed.y);
				pParticleValues[i].fRotaionSpeed = m_pGameInstance->Random_Float(m_InstanceDesc.vRotationSpeed.x, m_InstanceDesc.vRotationSpeed.y);
				pParticleValues[i].vAniIndex = _float2(0.f, m_InstanceDesc.vAniIndex.y);

				memcpy(&pParticleValues[i].vOriginRight, SRMatrix.m[0], sizeof(_float4));
				memcpy(&pParticleValues[i].vOriginUp, SRMatrix.m[1], sizeof(_float4));
				memcpy(&pParticleValues[i].vOriginLook, SRMatrix.m[2], sizeof(_float4));
				memcpy(&pParticleValues[i].vOriginTranslation, SRMatrix.m[3], sizeof(_float4));
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


#ifdef EDITOR_PROJECT

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
#endif

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

	Safe_Release(m_pVBInstance);

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	Safe_Release(m_pComputeShader);
	Safe_Release(m_pConstantBuffer);
	Safe_Release(m_pParticleValueBuffer);
}

void CInstance_Model::Describe_Entity()
{
	if (ImGui::TreeNode("Model Option"))
	{
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

		if (GUI::Checkbox("BillBoard", &m_InstanceDesc.isBillboard))
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


		if (ImGui::DragFloat3("Center", reinterpret_cast<_float*>(&m_InstanceDesc.vCenter)))
		{
			Instane_Buffer_ReStruct();
		}

		if (ImGui::DragFloat3("Range", reinterpret_cast<_float*>(&m_InstanceDesc.vRange)))
		{
			Instane_Buffer_ReStruct();
		}

		if (ImGui::DragFloat2("Speed", reinterpret_cast<_float*>(&m_InstanceDesc.vSpeed)))
		{
			Instane_Buffer_ReStruct();
		}

		if (ImGui::DragFloat2("RotationSpeed", reinterpret_cast<_float*>(&m_InstanceDesc.vRotationSpeed)))
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

		if (ImGui::DragFloat2("LifeTime", reinterpret_cast<_float*>(&m_InstanceDesc.vLifeTime)))
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

		if (ImGui::DragFloat2("NoiseUVMoveTime", reinterpret_cast<_float*>(&m_InstanceDesc.vNoiseUVMoveTime)))
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

		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	ImGui::Separator();
}
