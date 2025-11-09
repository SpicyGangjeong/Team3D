#include "pch.h"
#include "Mesh.h"
#include "Model.h"
#include "Bone.h"
#include "Shader.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CMesh::Bind_BoneMatrices(const vector<class CBone*>& Bones, CShader* pShader, const _char* pConstantName)
{
	if (m_iNumBones >= 512) {
		return E_FAIL;
	}

	for (size_t i = 0; i < m_iNumBones; ++i) {
		XMStoreFloat4x4(&m_pBoneMatrices[i],
			XMLoadFloat4x4(&m_offsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}
	if (0 == m_iNumBones) {
		return S_OK;
	}

	return pShader->Bind_Matrices(pConstantName, m_pBoneMatrices, m_iNumBones);
}

HRESULT CMesh::Bind_BoneMatrices(_float4x4* pCombinedMatrices, CShader* pShader, const _char* pConstantName)
{
	if (m_iNumBones >= 512) {
		return E_FAIL;
	}

	for (size_t i = 0; i < m_iNumBones; ++i) {
		XMStoreFloat4x4(&m_pBoneMatrices[i],
			XMLoadFloat4x4(&m_offsetMatrices[i]) * XMLoadFloat4x4(&pCombinedMatrices[m_BoneIndices[i]]));
	}
	if (0 == m_iNumBones) {
		return S_OK;
	}

	return pShader->Bind_Matrices(pConstantName, m_pBoneMatrices, m_iNumBones);
}

HRESULT CMesh::Render_Indexed(_uint IndexCount, _uint StartIndexLocation, _uint BaseVertexLocation)
{
	m_pContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
	return S_OK;
}
#ifdef EDITOR_PROJECT
HRESULT CMesh::Initialize_Prototype(MODEL eType, vector<class CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix& PreTransformMatrix)
{
	strcpy_s(m_szName, pAIMesh->mName.data);

	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;

	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = 4;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT hr = (eType == MODEL::NONANIM) ?
		Ready_VertexBuffer_For_NonAnim(pAIMesh, PreTransformMatrix) :
		Ready_VertexBuffer_For_Anim(Bones, pAIMesh);

#pragma endregion

#pragma region INDEX_BUFFER
	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.StructureByteStride = m_iIndexStride;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;


	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint	iNumIndices = {};

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{

		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
	}


	D3D11_SUBRESOURCE_DATA	InitialIBData{};
	InitialIBData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion



	return S_OK;
}
#endif

HRESULT CMesh::Bind_Resources_Instance(ID3D11Buffer* pVBInstance, _uint iInstanceStride, _uint iBufferCount)
{
	ID3D11Buffer* VertexBuffers[] = {
	   m_pVB,
	   pVBInstance,

	};

	_uint		VertexStrides[] = {
		m_iVertexStride,
		iInstanceStride,
	};

	_uint		Offsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, iBufferCount, VertexBuffers, VertexStrides, Offsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitive);

	return S_OK;
}

HRESULT CMesh::Render_Instance(_uint iNumInstance)
{
	m_pContext->DrawIndexedInstanced(m_iNumIndices, iNumInstance, 0, 0, 0);

	return S_OK;
}


HRESULT CMesh::Initialize_Prototype(HANDLE hFile, DWORD& dwByte)
{
	{ // 멤버변수
		if (!ReadFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr)) {
			return E_FAIL;
		}
		if (!ReadFile(hFile, &m_iNumVertexBuffers, sizeof(_uint), &dwByte, nullptr)) {
			return E_FAIL;
		}
		if (!ReadFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr)) {
			return E_FAIL;
		}
		if (!ReadFile(hFile, &m_iVertexStride, sizeof(_uint), &dwByte, nullptr)) {
			return E_FAIL;
		}
		if (!ReadFile(hFile, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr)) {
			return E_FAIL;
		}
		if (!ReadFile(hFile, &m_iIndexStride, sizeof(_uint), &dwByte, nullptr)) {
			return E_FAIL;
		}
		if (!ReadFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr)) {
			return E_FAIL;
		}
		if (!ReadFile(hFile, &m_eIndexFormat, sizeof(DXGI_FORMAT), &dwByte, nullptr)) {
			return E_FAIL;
		}
		if (!ReadFile(hFile, &m_ePrimitive, sizeof(D3D_PRIMITIVE_TOPOLOGY), &dwByte, nullptr)) {
			return E_FAIL;
		}
		DWORD iSize = {};
		if (!ReadFile(hFile, &iSize, sizeof(DWORD), &dwByte, nullptr)) {
			return E_FAIL;
		}
		m_BoneIndices.resize(iSize / sizeof(_int));
		if (!ReadFile(hFile, m_BoneIndices.data(), iSize, &dwByte, nullptr)) {
			return E_FAIL;
		}
		if (!ReadFile(hFile, &iSize, sizeof(DWORD), &dwByte, nullptr)) {
			return E_FAIL;
		}
		m_offsetMatrices.resize(iSize / sizeof(_float4x4));
		if (!ReadFile(hFile, m_offsetMatrices.data(), iSize, &dwByte, nullptr)) {
			DWORD err = GetLastError();
			return E_FAIL;
		}

		m_pBoneMatrices = new _float4x4[0 == m_iNumBones ? 1 : m_iNumBones];
		ZeroMemory(m_pBoneMatrices, sizeof(_float4x4) * m_iNumBones);
	}
	{ // Vertex Buffer
		HRESULT hr = (m_iVertexStride == sizeof(VTXMESH)) ?
			Ready_VertexBuffer_For_NonAnim(hFile, dwByte) :
			Ready_VertexBuffer_For_Anim(hFile, dwByte);
	}
	{ // Index Buffer
		D3D11_BUFFER_DESC IBDesc{};
		if (!ReadFile(hFile, &IBDesc, sizeof(D3D11_BUFFER_DESC), &dwByte, nullptr)) {
			return E_FAIL;
		}

		_uint* pIndices = new _uint[m_iNumIndices];
		ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

		if (!ReadFile(hFile, pIndices, IBDesc.ByteWidth, &dwByte, nullptr)) {
			return E_FAIL;
		}

		D3D11_SUBRESOURCE_DATA	InitialIBData{};
		InitialIBData.pSysMem = pIndices;

		if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB))) {
			return E_FAIL;
		}

		Safe_Delete_Array(pIndices);
	}
	return S_OK;
}

HRESULT CMesh::Initialize_Prototype(MODEL eType, const CModel* pModel, SaveMesh* _SaveMesh, _fmatrix PreTransformMatrix)
{
	strcpy_s(m_szName, _SaveMesh->Name);

	m_iMaterialIndex = _SaveMesh->MaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = _SaveMesh->VertexCount;

	m_iNumIndices = _SaveMesh->IndexCount;
	m_iIndexStride = 4;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_Indices = _SaveMesh->Indices;

	m_Vertices.clear();
	m_Vertices.reserve(_SaveMesh->Vertices.size());

	XMMATRIX matPre = PreTransformMatrix;
	for (size_t i = 0; i < _SaveMesh->Vertices.size(); ++i)
	{
		_float3 pos = _SaveMesh->Vertices[i].Pos;
		XMVECTOR v = XMLoadFloat3(&pos);
		v = XMVector3TransformCoord(v, matPre);
		XMFLOAT3 tp;
		XMStoreFloat3(&tp, v);
		m_Vertices.push_back(tp);
	}

#pragma region VERTEX_BUFFER

	HRESULT hr = MODEL::NONANIM == eType ?
		Ready_VertexBuffer_For_NonAnim(_SaveMesh, PreTransformMatrix) :
		Ready_VertexBuffer_For_Anim(pModel, _SaveMesh);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER
	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.StructureByteStride = m_iIndexStride;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;


	D3D11_SUBRESOURCE_DATA	InitialIBData{};
	InitialIBData.pSysMem = _SaveMesh->Indices.data();

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB)))
		return E_FAIL;

#pragma endregion



	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(HANDLE hFile, DWORD& dwByte)
{
	D3D11_BUFFER_DESC VBDesc{};
	if (!ReadFile(hFile, &VBDesc, sizeof(D3D11_BUFFER_DESC), &dwByte, nullptr)) {
		return E_FAIL;
	}
	VTXMESH* pVertices = new VTXMESH[m_iNumVertices]{};
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);
	if (!ReadFile(hFile, pVertices, VBDesc.ByteWidth, &dwByte, nullptr)) {
		return E_FAIL;
	}
	for (_uint i = 0; i < m_iNumVertices; ++i) {
		m_pVertexPositions[i] = pVertices[i].vPosition;
	}
	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB))) {
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(HANDLE hFile, DWORD& dwByte)
{
	D3D11_BUFFER_DESC VBDesc{};
	if (!ReadFile(hFile, &VBDesc, sizeof(D3D11_BUFFER_DESC), &dwByte, nullptr)) {
		return E_FAIL;
	}
	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices]{};
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);
	if (!ReadFile(hFile, pVertices, VBDesc.ByteWidth, &dwByte, nullptr)) {
		return E_FAIL;
	}
	for (_uint i = 0; i < m_iNumVertices; ++i) {
		m_pVertexPositions[i] = pVertices[i].vPosition;
	}
	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB))) {
		return E_FAIL;
	}

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(SaveMesh* _SaveMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.StructureByteStride = m_iVertexStride;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &_SaveMesh->Vertices[i].Pos, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition,
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &_SaveMesh->Vertices[i].Normal, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal,
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix)));

		memcpy(&pVertices[i].vTangent, &_SaveMesh->Vertices[i].Tan, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent,
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix)));

		memcpy(&pVertices[i].vBinormal, &_SaveMesh->Vertices[i].BiNoraml, sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vBinormal, 
			XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vBinormal), PreTransformMatrix)));

		pVertices[i].vTexcoord = _SaveMesh->Vertices[i].UV;
	}

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(const CModel* pModel, SaveMesh* _SaveMesh)
{
	m_iVertexStride = sizeof(VTXANIMMESH);
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.StructureByteStride = m_iVertexStride;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &_SaveMesh->Vertices[i].Pos, sizeof(_float3));
		if (_SaveMesh->Vertices[i].bHasNormal)
			memcpy(&pVertices[i].vNormal, &_SaveMesh->Vertices[i].Normal, sizeof(_float3));
		else
			pVertices[i].vNormal = _float3(0.f, 1.f, 0.f);
		if (_SaveMesh->Vertices[i].bHasTan)
			memcpy(&pVertices[i].vTangent, &_SaveMesh->Vertices[i].Tan, sizeof(_float3));
		else
			pVertices[i].vTangent = _float3(1.f, 0.f, 0.f);

		memcpy(&pVertices[i].vBinormal, &_SaveMesh->Vertices[i].BiNoraml, sizeof(_float3));

		if (_SaveMesh->Vertices[i].bHasUV)
			memcpy(&pVertices[i].vTexcoord, &_SaveMesh->Vertices[i].UV, sizeof(_float2));
		else
			pVertices[i].vTexcoord = _float2(0.f, 0.f);
	}

	m_iNumBones = (_uint)_SaveMesh->Bones.size();

	m_offsetMatrices.reserve(m_iNumBones);

	m_pBoneMatrices = new _float4x4[0 == m_iNumBones ? 1 : m_iNumBones];
	ZeroMemory(m_pBoneMatrices, sizeof(_float4x4) * m_iNumBones);

	_float4x4		OffsetMatrix;
	XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		SaveBone _SaveBone = _SaveMesh->Bones[i];
		strcpy_s(m_szName, _SaveBone.Name);

		memcpy(&OffsetMatrix, &_SaveBone.OffsetMatrix, sizeof(_float4x4));

		_int		iBoneIndex = pModel->Get_BoneIndex(_SaveBone.Name);

		if (-1 == iBoneIndex)
			return E_FAIL;

		m_BoneIndices.push_back(iBoneIndex);
		m_offsetMatrices.push_back(OffsetMatrix);


		for (_uint j = 0; j < _SaveBone.Weights.size(); j++)
		{
			SaveBoneWeight _SaveBoneWeight = _SaveBone.Weights[j];

			if (0.f == pVertices[_SaveBoneWeight.VertexId].vBlendWeight.x)
			{
				pVertices[_SaveBoneWeight.VertexId].vBlendIndex.x = i;
				pVertices[_SaveBoneWeight.VertexId].vBlendWeight.x = _SaveBoneWeight.Weight;
			}
			else if (0.f == pVertices[_SaveBoneWeight.VertexId].vBlendWeight.y)
			{
				pVertices[_SaveBoneWeight.VertexId].vBlendIndex.y = i;
				pVertices[_SaveBoneWeight.VertexId].vBlendWeight.y = _SaveBoneWeight.Weight;
			}
			else if (0.f == pVertices[_SaveBoneWeight.VertexId].vBlendWeight.z)
			{
				pVertices[_SaveBoneWeight.VertexId].vBlendIndex.z = i;
				pVertices[_SaveBoneWeight.VertexId].vBlendWeight.z = _SaveBoneWeight.Weight;
			}
			else
			{
				pVertices[_SaveBoneWeight.VertexId].vBlendIndex.w = i;
				pVertices[_SaveBoneWeight.VertexId].vBlendWeight.w = _SaveBoneWeight.Weight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		m_BoneIndices.push_back(pModel->Get_BoneIndex(m_szName));

		m_offsetMatrices.push_back(OffsetMatrix);
	}

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

#ifdef EDITOR_PROJECT
CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, vector<class CBone*>& Bones, const aiMesh* pAIMesh, _fmatrix& PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, Bones, pAIMesh, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

HRESULT CMesh::SaveAsBinary(HANDLE hFile, DWORD& dwByte)
{
	{ // 멤버변수
		WriteFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &m_iNumVertexBuffers, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &m_iVertexStride, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &m_iIndexStride, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
		WriteFile(hFile, &m_eIndexFormat, sizeof(DXGI_FORMAT), &dwByte, nullptr);
		WriteFile(hFile, &m_ePrimitive, sizeof(D3D_PRIMITIVE_TOPOLOGY), &dwByte, nullptr);

		DWORD iSize = (DWORD)(sizeof(_int) * m_BoneIndices.size());
		WriteFile(hFile, &iSize, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, m_BoneIndices.data(), iSize, &dwByte, nullptr);

		iSize = (DWORD)(sizeof(_float4x4) * m_offsetMatrices.size());
		WriteFile(hFile, &iSize, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, m_offsetMatrices.data(), iSize, &dwByte, nullptr);
	}
	{ // Vertex Buffer
		D3D11_BUFFER_DESC VBDesc{};
		m_pVB->GetDesc(&VBDesc); // 버퍼 디스크립션 저장
		WriteFile(hFile, &VBDesc, sizeof(D3D11_BUFFER_DESC), &dwByte, nullptr);

		D3D11_BUFFER_DESC StagingDesc{};
		StagingDesc.Usage = D3D11_USAGE_STAGING;
		StagingDesc.ByteWidth = VBDesc.ByteWidth;
		StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		StagingDesc.BindFlags = 0;
		StagingDesc.MiscFlags = 0;

		ID3D11Buffer* pStaging = nullptr;
		if (FAILED(m_pDevice->CreateBuffer(&StagingDesc, nullptr, &pStaging)) || nullptr == pStaging) {
			return E_FAIL;
		}
		m_pContext->CopyResource(pStaging, m_pVB); // 버퍼 복사

		D3D11_MAPPED_SUBRESOURCE mapped{};
		m_pContext->Map(pStaging, 0, D3D11_MAP_READ, 0, &mapped);
		WriteFile(hFile, mapped.pData, VBDesc.ByteWidth, &dwByte, nullptr);

		m_pContext->Unmap(pStaging, 0);
		pStaging->Release();
	}
	{ // Index Buffer
		D3D11_BUFFER_DESC IBDesc{};
		m_pIB->GetDesc(&IBDesc);
		WriteFile(hFile, &IBDesc, sizeof(D3D11_BUFFER_DESC), &dwByte, nullptr);

		D3D11_BUFFER_DESC StagingDesc{};
		StagingDesc.Usage = D3D11_USAGE_STAGING;
		StagingDesc.ByteWidth = IBDesc.ByteWidth;
		StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		StagingDesc.BindFlags = 0;
		StagingDesc.MiscFlags = 0;

		ID3D11Buffer* pStaging = nullptr;
		if (FAILED(m_pDevice->CreateBuffer(&StagingDesc, nullptr, &pStaging)) || nullptr == pStaging) {
			return E_FAIL;
		}
		m_pContext->CopyResource(pStaging, m_pIB);

		D3D11_MAPPED_SUBRESOURCE mapped{};
		m_pContext->Map(pStaging, 0, D3D11_MAP_READ, 0, &mapped);
		WriteFile(hFile, mapped.pData, IBDesc.ByteWidth, &dwByte, nullptr);

		m_pContext->Unmap(pStaging, 0);
		pStaging->Release();
	}
	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.StructureByteStride = m_iVertexStride;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix)));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix)));
		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vBinormal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vBinormal), PreTransformMatrix)));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
	}

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(vector<class CBone*>& Bones, const aiMesh* pAIMesh)
{
	m_iVertexStride = sizeof(VTXANIMMESH);
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.StructureByteStride = m_iVertexStride;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
	}

	m_iNumBones = pAIMesh->mNumBones;

	m_offsetMatrices.reserve(m_iNumBones);

	m_pBoneMatrices = new _float4x4[0 == m_iNumBones ? 1 : m_iNumBones];
	ZeroMemory(m_pBoneMatrices, sizeof(_float4x4) * m_iNumBones);

	_float4x4 OffSetMatrix;
	XMStoreFloat4x4(&OffSetMatrix, XMMatrixIdentity());

	for (_uint i = 0; i < m_iNumBones; ++i) {
		aiBone* pAIBone = pAIMesh->mBones[i];
		_uint iNumWeight = pAIBone->mNumWeights;

		memcpy(&OffSetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffSetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffSetMatrix)));

		_int	iBoneIndex = CModel::Get_BoneIndex(pAIBone->mName.data, Bones);

		if (-1 == iBoneIndex) {
			return E_FAIL;
		}

		m_offsetMatrices.push_back(OffSetMatrix);
		m_BoneIndices.push_back(iBoneIndex);

		for (_uint j = 0; j < iNumWeight; ++j) {
			aiVertexWeight AIWeight = pAIBone->mWeights[j];
			if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.x) {
				pVertices[AIWeight.mVertexId].vBlendIndex.x = i;
				pVertices[AIWeight.mVertexId].vBlendWeight.x = AIWeight.mWeight;
			}
			else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.y) {
				pVertices[AIWeight.mVertexId].vBlendIndex.y = i;
				pVertices[AIWeight.mVertexId].vBlendWeight.y = AIWeight.mWeight;
			}
			else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.z) {
				pVertices[AIWeight.mVertexId].vBlendIndex.z = i;
				pVertices[AIWeight.mVertexId].vBlendWeight.z = AIWeight.mWeight;
			}
			else {
				pVertices[AIWeight.mVertexId].vBlendIndex.w = i;
				pVertices[AIWeight.mVertexId].vBlendWeight.w = AIWeight.mWeight;
			}
		}
	}
	if (0 == m_iNumBones) {
		m_iNumBones = 1;
		m_BoneIndices.push_back(CModel::Get_BoneIndex(m_szName, Bones));
		m_offsetMatrices.push_back(OffSetMatrix);
	}

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}
#endif

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HANDLE hFile, DWORD& dwByte)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(hFile, dwByte)))
	{
		MSG_BOX("Failed to Created : CMesh");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const CModel* pModel, SaveMesh* _SaveMesh, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModel, _SaveMesh, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Clone(void* pArg, class CGameObject* pOwner)
{
	CMesh* pInstance = new CMesh(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
void CMesh::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pBoneMatrices);
}

void CMesh::Describe_Entity()
{
}
