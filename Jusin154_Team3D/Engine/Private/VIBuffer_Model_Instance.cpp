#include "pch.h"
#include "VIBuffer_Model_Instance.h"
#include "GameInstance.h"

#include "Mesh.h"
#include "Bone.h"
#include "Model.h"
#include "Transform.h"
#include "Shader.h"
#include "ComputeShader.h"

CVIBuffer_Model_Instance::CVIBuffer_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance(pDevice, pContext)
	, m_iNumBuffer{ 2 }
{
}

CVIBuffer_Model_Instance::CVIBuffer_Model_Instance(const CVIBuffer_Model_Instance& rhs)
	: CVIBuffer_Instance(rhs)
	, m_iNumBuffer(rhs.m_iNumBuffer)
	, m_iCurrentNumInstance( rhs.m_iCurrentNumInstance )
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iMaxNumInstance(rhs.m_iMaxNumInstance)
	, m_Meshes(rhs.m_Meshes)
	, m_InstanceDesc(rhs.m_InstanceDesc)
	, m_MeshMaterialsSRV(rhs.m_MeshMaterialsSRV)
	, m_SurfaceMaterialFlag(rhs.m_SurfaceMaterialFlag)
#ifdef EDITOR_PROJECT
	, m_pAIScene(rhs.m_pAIScene)
	, m_MeshMaterialPathes(rhs.m_MeshMaterialPathes)
#endif // EDITOR_PROJECT
{
	for (auto& pMesh : m_Meshes)
		SAFE_ADDREF(pMesh);

	for (auto& SRVs : m_MeshMaterialsSRV)
	{
		for (auto& pSRV : SRVs)
			SAFE_ADDREF(pSRV);
	}
}

const _char* CVIBuffer_Model_Instance::Get_MeshName(_uint iMeshIndex)
{
	if (m_iNumMeshes <= iMeshIndex)
		return nullptr;

	return m_Meshes[iMeshIndex]->Get_Name();
}

HRESULT CVIBuffer_Model_Instance::Ready_PhysXMeshes(_uint iLevel)
{
	m_iNumPhysXMeshes = m_iNumMeshes;

	m_TriMeshes.reserve(m_iNumMeshes);

	m_pGameInstance->ConvertToTriMeshes(m_Meshes, m_TriMeshes);

	for (_uint i = 0; i < m_iNumMeshes; ++i) {
		m_pGameInstance->RegistTriMesh((m_Meshes[i]->Get_Name() + to_string(i)).c_str(), m_TriMeshes[i], iLevel);
	}
	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Initialize_Prototype(const _char* pModelFilePath, const _char* pMatrialPath)
{
	if (FAILED(Load_ModelData(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Load_Material(pModelFilePath, pMatrialPath)))
		return E_FAIL;

	if (FAILED(Ready_Materials()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Initialize(void* pArg)
{
	m_fTimeAcc = 0.f;

	if (FAILED(Create_ConstantBuffer()))
		return E_FAIL;

	if (FAILED(Create_Instance_Value_Buffer()))
		return E_FAIL;

	if (FAILED(Reay_Instance_Buffer()))
		return E_FAIL;

	if (FAILED(Create_ComputeShader()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Resources_Instance(m_pVBInstance, m_iInstanceStride, m_iNumBuffer);

	m_Meshes[iMeshIndex]->Render_Instance(m_iNumInstance);

	return S_OK;
}

void CVIBuffer_Model_Instance::Shake(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (3600.0f < m_fTimeAcc)
		m_fTimeAcc = 0.f;

	D3D11_MAPPED_SUBRESOURCE ConstantSubResource = {};

	if (SUCCEEDED(m_pContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantSubResource)))
	{
		CS_MODEL_INSTANCE_CONSTANT_DESC* pDesc = static_cast<CS_MODEL_INSTANCE_CONSTANT_DESC*>(ConstantSubResource.pData);

		pDesc->fTimeDelta = m_fTimeAcc;
		pDesc->fPading1 = 0.f;
		pDesc->fPading2 = 0.f;
		pDesc->fPading3 = 0.f;
		m_pContext->Unmap(m_pConstantBuffer, 0);
	}

	if (m_pComputeShader == nullptr)
		return;

	_uint iGroupCountX = (m_iNumInstance + 255) / 256;

	ID3D11Buffer* CSBuffers[] = {
		 m_pVBInstance,
		 m_pParticleValueBuffer
	};

	m_pComputeShader->Dispatch(0, 0, _float3((_float)iGroupCountX, 1.f, 1.f), CSBuffers, m_pConstantBuffer);

	return;
}

HRESULT CVIBuffer_Model_Instance::Set_Shake_Value(_float2 vRadius, _float2 vSpeed)
{
	D3D11_MAPPED_SUBRESOURCE InstanceValueResource = {};

	if (SUCCEEDED(m_pContext->Map(m_pParticleValueBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &InstanceValueResource)))
	{
		CS_MODEL_INSTANCE_DESC* pDesc = static_cast<CS_MODEL_INSTANCE_DESC*>(InstanceValueResource.pData);

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			pDesc[i].fMaxRadian = m_pGameInstance->Real_Random_Float(vRadius.x, vRadius.y);
			pDesc[i].fSpeed = m_pGameInstance->Real_Random_Float(vSpeed.x, vSpeed.y);
		}

		m_pContext->Unmap(m_pParticleValueBuffer, 0);
	}

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Bind_OutPut_SRV_VS(_uint Index, _uint iBufferIndex)
{
	m_pComputeShader->Bind_OutPut_SRV_VS(Index, iBufferIndex);

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Bind_Matrial(CShader* pShader, _uint iMeshIndex)
{
	if(FAILED(pShader->Bind_SRV("g_DiffuseTexture", m_MeshMaterialsSRV[iMeshIndex][0])))
		return E_FAIL;

	if(FAILED(pShader->Bind_SRV("g_NormalTexture", m_MeshMaterialsSRV[iMeshIndex][1])))
		return E_FAIL;

	if(FAILED(pShader->Bind_SRV("g_SurfaceParamsTexture", m_MeshMaterialsSRV[iMeshIndex][2])))
		return E_FAIL;

	_float fUsingSurfaceParams = ((_float)m_SurfaceMaterialFlag[iMeshIndex] / (_float)AI_TEXTURE_TYPE_MAX);

	if (FAILED(pShader->Bind_RawValue("g_fUsingSurfaceParams", &fUsingSurfaceParams, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Load_WorldData(vector<_float4x4>& WorldMatrices)
{
	m_iCurrentNumInstance = (_uint)WorldMatrices.size();

	if (m_iMaxNumInstance <= m_iCurrentNumInstance)
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
		return E_FAIL;

	VTX_INSTANCE_MODEL* pData = static_cast<VTX_INSTANCE_MODEL*>(SubResource.pData);

	for (_uint i = 0; i < m_iCurrentNumInstance; i++)
	{
		memcpy(&pData[i].vRight, &WorldMatrices[i].m[0], sizeof(_float4));
		memcpy(&pData[i].vUp, &WorldMatrices[i].m[1], sizeof(_float4));
		memcpy(&pData[i].vLook, &WorldMatrices[i].m[2], sizeof(_float4));
		memcpy(&pData[i].vTranslation, &WorldMatrices[i].m[3], sizeof(_float4));
	}
	m_iNumInstance = m_iCurrentNumInstance;

	m_pContext->Unmap(m_pVBInstance, 0);

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Reay_Instance_Buffer()
{
	m_iInstanceStride = sizeof(VTX_INSTANCE_MODEL);

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

HRESULT CVIBuffer_Model_Instance::Load_InstanceData(ifstream& in)
{
	in.read(reinterpret_cast<_char*>(&m_iNumInstance), sizeof(_uint));

	if (FAILED(Reay_Instance_Buffer()))
		return E_FAIL;

	/* Input Data */
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
	{
		in.close();
		return E_FAIL;
	}

	VTX_INSTANCE_MODEL* pData = static_cast<VTX_INSTANCE_MODEL*>(SubResource.pData);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		in.read(reinterpret_cast<_char*>(&pData[i]), sizeof(VTX_INSTANCE_MODEL));
	}

	m_pContext->Unmap(m_pVBInstance, 0);


	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Load_ModelData(const _char* pModelFilePath)
{
	ifstream In(pModelFilePath, ios::binary);

	if (!In.is_open())
	{
		MSG_BOX("Failed to Load : Model Data");
		return E_FAIL;
	}

	In.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof(_uint));

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		SaveMesh mesh{};

		In.read(reinterpret_cast<_char*>(&mesh.MeshNameSize), sizeof(_uint));
		mesh.MeshName.resize(mesh.MeshNameSize);
		In.read(reinterpret_cast<_char*>(mesh.MeshName.data()), mesh.MeshNameSize);


		In.read(reinterpret_cast<_char*>(&mesh.VertexCount), sizeof(_uint));
		In.read(reinterpret_cast<_char*>(&mesh.IndexCount), sizeof(_uint));
		In.read(reinterpret_cast<_char*>(&mesh.MaterialIndex), sizeof(_uint));
		In.read(reinterpret_cast<_char*>(&mesh.BoneCount), sizeof(_uint));

		mesh.Vertices.resize(mesh.VertexCount);
		In.read(reinterpret_cast<_char*>(mesh.Vertices.data()), sizeof(SaveVertex) * mesh.VertexCount);

		mesh.Indices.resize(mesh.IndexCount);
		In.read(reinterpret_cast<_char*>(mesh.Indices.data()), sizeof(_uint) * mesh.IndexCount);

		CModel* pModel = { nullptr };

		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, MODEL::NONANIM, pModel, &mesh, XMMatrixIdentity());

		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}
	In.read(reinterpret_cast<_char*>(&m_iNumInstance), sizeof(_uint));

	m_iMaxNumInstance = m_iNumInstance;

	In.close();

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Load_Material(const _char* pModelFilePath, const _char* pMatrialPath)
{
	tinyxml2::XMLDocument xmlDoc;

	if ((tinyxml2::XML_SUCCESS != xmlDoc.LoadFile(pMatrialPath)))
		return E_FAIL;

	tinyxml2::XMLElement* root = xmlDoc.FirstChildElement("Instance");

	_char szName[MAX_PATH] = {};

	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, szName, MAX_PATH, nullptr, 0);

	tinyxml2::XMLElement* Model = root->FirstChildElement(szName);

	/* Check Mesh Num */
	_uint iNumMesh = {};
	Model->QueryUnsignedAttribute("mesh", &iNumMesh);

	if (iNumMesh != m_iNumMeshes)
		return E_FAIL;

	for (auto* Mesh = Model->FirstChildElement("Mesh"); Mesh; Mesh = Mesh->NextSiblingElement("Mesh"))
	{
		vector<string> MaterialPathes = {};

		for (auto* Material = Mesh->FirstChildElement("Material"); Material; Material = Material->NextSiblingElement("Material"))
		{
			MaterialPathes.push_back("../Bin/Resources/Models/InstanceProp/Textures/" + string(Material->GetText()) + ".dds");
		}

		m_MeshMaterialPathes.push_back(MaterialPathes);

		auto* Surface = Mesh->FirstChildElement("Surface");
		string strSurface = Surface->GetText();

		if (string::npos != strSurface.find("SRO"))
			m_SurfaceMaterialFlag.push_back(aiTextureType_SPECULAR);
		else
			m_SurfaceMaterialFlag.push_back(aiTextureType_METALNESS);
	}

	return S_OK;
}


HRESULT CVIBuffer_Model_Instance::Ready_Materials()
{
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		vector<ID3D11ShaderResourceView*> SRVs = {};
		for (_uint j = 0; j < 3; ++j)
		{
			ID3D11ShaderResourceView* pSRV = { nullptr };

			pSRV = m_pGameInstance->Add_Resource(m_MeshMaterialPathes[i][j].c_str());

			if (nullptr == pSRV){
				return E_FAIL;
			}

			SRVs.push_back(pSRV);
		}
		m_MeshMaterialsSRV.push_back(SRVs);
	}

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Create_ComputeShader()
{
	_uint		CS_InputStrides[] = {
	sizeof(VTX_INSTANCE_MODEL),
	sizeof(CS_MODEL_INSTANCE_DESC),
	};

	_uint		CS_OutputStrides[] = {
	sizeof(VTX_INSTANCE_MODEL),
	};

	CComputeShader::CS_INFO CS_Desc = {};

	CS_Desc.iNumElement = m_iMaxNumInstance;
	CS_Desc.iNumInputBuffer = 2;
	CS_Desc.iNumOutputBuffer = 1;

	CS_Desc.iInputStructStride = CS_InputStrides;
	CS_Desc.iOutputStructStride = CS_OutputStrides;

	m_pComputeShader = (CComputeShader*)m_pGameInstance->Clone_Asset_Prototype(g_iStaticLevel, CS_INSTANCE_MODEL, &CS_Desc, nullptr);

	if (nullptr == m_pComputeShader) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Create_ConstantBuffer()
{
	//컨스턴트 버퍼 생성
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.ByteWidth = sizeof(CS_MODEL_INSTANCE_CONSTANT_DESC);

	D3D11_SUBRESOURCE_DATA	CBInitialDesc = {};

	if (FAILED(m_pDevice->CreateBuffer(&cbDesc, nullptr, &m_pConstantBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Create_Instance_Value_Buffer()
{
	// 벨류 버퍼 생성하기
	D3D11_BUFFER_DESC Particle_Buffer_Desc = {};
	Particle_Buffer_Desc.ByteWidth = sizeof(CS_MODEL_INSTANCE_DESC) * m_iNumInstance;
	Particle_Buffer_Desc.Usage = D3D11_USAGE_DYNAMIC;
	Particle_Buffer_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Particle_Buffer_Desc.StructureByteStride = sizeof(CS_MODEL_INSTANCE_DESC);
	Particle_Buffer_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Particle_Buffer_Desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA	InitialDesc = {};
	InitialDesc.pSysMem = nullptr;

	if (FAILED(m_pDevice->CreateBuffer(&Particle_Buffer_Desc, nullptr, &m_pParticleValueBuffer)))
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE InstanceValueResource = {};

	if (SUCCEEDED(m_pContext->Map(m_pParticleValueBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &InstanceValueResource)))
	{
		CS_MODEL_INSTANCE_DESC* pDesc = static_cast<CS_MODEL_INSTANCE_DESC*>(InstanceValueResource.pData);

		for (_uint i = 0; i < m_iNumInstance; ++i)
		{
			pDesc[i].fMaxRadian = 0.f;
			pDesc[i].fSpeed = 0.f;
		}

		m_pContext->Unmap(m_pParticleValueBuffer, 0);
	}

	return S_OK;
}

#ifdef EDITOR_PROJECT

HRESULT CVIBuffer_Model_Instance::SaveAssimpModel(const _char* pModelFilePath)
{
	if (!m_pAIScene) {
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pAIScene->mNumMeshes; i++)
	{
		SaveMesh saveMesh{};

		aiMesh* mesh = m_pAIScene->mMeshes[i];

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

		saveMesh.Indices.reserve(saveMesh.IndexCount);
		for (_uint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace& face = mesh->mFaces[i];
			for (_uint idx = 0; idx < face.mNumIndices; idx++)
				saveMesh.Indices.push_back(face.mIndices[idx]);
		}

		m_SaveMeshDesc.push_back(saveMesh);
	}

#pragma region SAVE_MESH
	filesystem::path ModelPath = pModelFilePath;

	string pFileName = ModelPath.parent_path().string() + "/" + ModelPath.stem().string() + ".bin";

	ofstream out(pFileName, ios::binary);

	if (!out.is_open())
	{
		MSG_BOX("Failed to Save : Instance Data");
		return E_FAIL;
	}
	out.write(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof(_uint));


	for (auto& mesh : m_SaveMeshDesc)
	{
		out.write(reinterpret_cast<_char*>(&mesh.MeshNameSize), sizeof(_uint));
		out.write(reinterpret_cast<_char*>(mesh.MeshName.data()), mesh.MeshNameSize);
		out.write(reinterpret_cast<_char*>(&mesh.VertexCount), sizeof(_uint));
		out.write(reinterpret_cast<_char*>(&mesh.IndexCount), sizeof(_uint));
		out.write(reinterpret_cast<_char*>(&mesh.MaterialIndex), sizeof(_uint));
		out.write(reinterpret_cast<_char*>(&mesh.BoneCount), sizeof(_uint));

		out.write(reinterpret_cast<_char*>(mesh.Vertices.data()), sizeof(SaveVertex) * mesh.VertexCount);
		out.write(reinterpret_cast<_char*>(mesh.Indices.data()), sizeof(_uint) * mesh.IndexCount);
	}

	out.write(reinterpret_cast<_char*>(&m_iMaxNumInstance), sizeof(_uint));
#pragma endregion

	out.close();


	return S_OK;
}

HRESULT CVIBuffer_Model_Instance::Initialize_Prototype(const INSTANCE_DESC* pDesc, const _char* pModelFilePath, const _char* pMatrialPath)
{
	m_iNumBuffer = 2;
	m_iMaxNumInstance = m_iNumInstance = pDesc->iNum;

	if (FAILED(Assimp_Model_Load(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Load_Material(pModelFilePath, pMatrialPath)))
		return E_FAIL;

	if(FAILED(Ready_Materials()))
		return E_FAIL;

	

	return S_OK;
}

void CVIBuffer_Model_Instance::Update_Instance()
{
	m_iNumInstance = m_iCurrentNumInstance;

}

void CVIBuffer_Model_Instance::Add_Instance(_fmatrix WorldMatrix)
{
	++m_iCurrentNumInstance;

	if (m_iMaxNumInstance <= m_iCurrentNumInstance)
		return;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource)))
		return;

	VTX_INSTANCE_MODEL* pData = static_cast<VTX_INSTANCE_MODEL*>(SubResource.pData);

	memcpy(&pData[m_iCurrentNumInstance - 1].vRight, &WorldMatrix.r[0], sizeof(_float4));
	memcpy(&pData[m_iCurrentNumInstance - 1].vUp, &WorldMatrix.r[1], sizeof(_float4));
	memcpy(&pData[m_iCurrentNumInstance - 1].vLook, &WorldMatrix.r[2], sizeof(_float4));
	memcpy(&pData[m_iCurrentNumInstance - 1].vTranslation, &WorldMatrix.r[3], sizeof(_float4));
	
	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Model_Instance::Delete_Instance()
{
	if (0 != m_iCurrentNumInstance)
		m_iCurrentNumInstance--;

	m_iNumInstance = m_iCurrentNumInstance;

	return;
}

void CVIBuffer_Model_Instance::Fix_Instance(_fmatrix WorldMatrix)
{
	if (0 == m_iCurrentNumInstance)
		return;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	if (FAILED(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource)))
		return;

	VTX_INSTANCE_MODEL* pData = static_cast<VTX_INSTANCE_MODEL*>(SubResource.pData);

	memcpy(&pData[m_iCurrentNumInstance - 1].vRight, &WorldMatrix.r[0], sizeof(_float4));
	memcpy(&pData[m_iCurrentNumInstance - 1].vUp, &WorldMatrix.r[1], sizeof(_float4));
	memcpy(&pData[m_iCurrentNumInstance - 1].vLook, &WorldMatrix.r[2], sizeof(_float4));
	memcpy(&pData[m_iCurrentNumInstance - 1].vTranslation, &WorldMatrix.r[3], sizeof(_float4));

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Model_Instance::Assimp_Model_Load(const _char* pModelFilePath)
{
	_uint			iFlag = {};
	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast | aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene) {
		return E_FAIL;
	}

	m_iNumMeshes = m_pAIScene->mNumMeshes;

	vector<class CBone*> bones;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, MODEL::NONANIM, bones, m_pAIScene->mMeshes[i], XMMatrixIdentity());

		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	if (FAILED(SaveAssimpModel(pModelFilePath)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Model_Instance* CVIBuffer_Model_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc, const _char* pModelFilePath, const _char* pMatrialPath)
{
	CVIBuffer_Model_Instance* pInstance = new CVIBuffer_Model_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pInstanceDesc, pModelFilePath, pMatrialPath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Model_Instance");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


#endif

CVIBuffer_Model_Instance* CVIBuffer_Model_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const _char* pMatrialPath)
{
	CVIBuffer_Model_Instance* pInstance = new CVIBuffer_Model_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, pMatrialPath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Model_Instance");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Model_Instance::Clone(void* pArg, CGameObject* pOwner)
{
	CVIBuffer_Model_Instance* pInstance = new CVIBuffer_Model_Instance(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Model_Instance");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Model_Instance::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pComputeShader);

	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pParticleValueBuffer);

	for (auto& pMesh : m_Meshes)
		SAFE_RELEASE(pMesh);
	m_Meshes.clear();

	for (auto& Materials : m_MeshMaterialsSRV)
	{
		for(auto& pSRV : Materials)
			SAFE_RELEASE(pSRV);
		Materials.clear();
	}
	m_MeshMaterialsSRV.clear();

	for (auto& Pathes : m_MeshMaterialPathes)
		Pathes.clear();
	m_MeshMaterialPathes.clear();

	m_SurfaceMaterialFlag.clear();
}
#ifdef _DEBUG
void CVIBuffer_Model_Instance::Describe_Entity()
{
	GUI::Text("Instance Count : %d / %d", m_iCurrentNumInstance, m_iMaxNumInstance);
}
#endif
