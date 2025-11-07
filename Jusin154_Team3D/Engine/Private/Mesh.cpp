#include "pch.h"
#include "Mesh.h"

#include "Bone.h"
#include "Model.h"
#include "Shader.h"


CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer( pDevice, pContext )
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer( Prototype )
{
}

HRESULT CMesh::Initialize_Prototype(MODEL eType, const class CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	strcpy_s(m_szName, pAIMesh->mName.data);

	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;

	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = 4;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	m_Indices.clear();
	m_Indices.reserve(pAIMesh->mNumFaces * 3);
	for (unsigned int f = 0; f < pAIMesh->mNumFaces; ++f)
	{
		const aiFace& face = pAIMesh->mFaces[f];
		if (face.mNumIndices == 3)
		{
			m_Indices.push_back(face.mIndices[0]);
			m_Indices.push_back(face.mIndices[1]);
			m_Indices.push_back(face.mIndices[2]);
		}
	}

	m_Vertices.clear();
	m_Vertices.reserve(pAIMesh->mNumVertices);

	XMMATRIX matPre = PreTransformMatrix;
	for (size_t i = 0; i < pAIMesh->mNumVertices; ++i)
	{
		_float3 pos{ pAIMesh->mVertices[i].x,
				pAIMesh->mVertices[i].y,
				pAIMesh->mVertices[i].z };
		XMVECTOR v = XMLoadFloat3(&pos);
		v = XMVector3TransformCoord(v, matPre);
		XMFLOAT3 tp;
		XMStoreFloat3(&tp, v);
		m_Vertices.push_back(tp);
	}

#pragma region VERTEX_BUFFER

	HRESULT hr = MODEL::NONANIM == eType ?
		Ready_VertexBuffer_For_NonAnim(pAIMesh, PreTransformMatrix) :
		Ready_VertexBuffer_For_Anim(pModel, pAIMesh);

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

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(const vector<CBone*>& Bones, CShader* pShader, const _char* pConstantName)
{
	if (m_iNumBones >= 512)
		return E_FAIL;


	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_pBoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	if (0 == m_iNumBones)
		return S_OK;

	return pShader->Bind_Matrices(pConstantName, m_pBoneMatrices, m_iNumBones);

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

		if (pAIMesh->HasNormals())
		{
			memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
			XMStoreFloat3(&pVertices[i].vNormal,
				XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix)));
		}
		else
		{
			pVertices[i].vNormal = _float3(0.f, 1.f, 0.f);
		}

		if (pAIMesh->HasTangentsAndBitangents())
		{
			memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
			XMStoreFloat3(&pVertices[i].vTangent,
				XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix)));
		}
		else
		{
			pVertices[i].vTangent = _float3(1.f, 0.f, 0.f);
		}

		if (pAIMesh->HasTextureCoords(0))
		{
			memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		}
		else
		{
			pVertices[i].vTexcoord = _float2(0.f, 0.f);
		}
	}

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_For_Anim(const CModel* pModel, const aiMesh* pAIMesh)
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
		if (pAIMesh->HasNormals())
		{
			memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		}
		else
		{
			pVertices[i].vNormal = _float3(0.f, 1.f, 0.f);
		}

		if (pAIMesh->HasTangentsAndBitangents())
		{
			memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		}
		else
		{
			pVertices[i].vTangent = _float3(1.f, 0.f, 0.f);
		}

		if (pAIMesh->HasTextureCoords(0))
		{
			memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		}
		else
		{
			pVertices[i].vTexcoord = _float2(0.f, 0.f);
		}
	}

	m_iNumBones = pAIMesh->mNumBones;

	m_OffsetMatrices.reserve(m_iNumBones);

	m_pBoneMatrices = new _float4x4[0 == m_iNumBones ? 1 : m_iNumBones];
	ZeroMemory(m_pBoneMatrices, sizeof(_float4x4) * m_iNumBones);

	_float4x4		OffsetMatrix;
	XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		_int		iBoneIndex = pModel->Get_BoneIndex(pAIBone->mName.data);

		if (-1 == iBoneIndex)
			return E_FAIL;

		m_OffsetMatrices.push_back(OffsetMatrix);
		m_BoneIndices.push_back(iBoneIndex);

		for (_uint j = 0; j < pAIBone->mNumWeights; j++)
		{
			aiVertexWeight	AIWeight = pAIBone->mWeights[j];

			if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.x)
			{
				pVertices[AIWeight.mVertexId].vBlendIndex.x = i;
				pVertices[AIWeight.mVertexId].vBlendWeight.x = AIWeight.mWeight;
			}

			else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.y)
			{
				pVertices[AIWeight.mVertexId].vBlendIndex.y = i;
				pVertices[AIWeight.mVertexId].vBlendWeight.y = AIWeight.mWeight;
			}

			else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.z)
			{
				pVertices[AIWeight.mVertexId].vBlendIndex.z = i;
				pVertices[AIWeight.mVertexId].vBlendWeight.z = AIWeight.mWeight;
			}

			else
			{
				pVertices[AIWeight.mVertexId].vBlendIndex.w = i;
				pVertices[AIWeight.mVertexId].vBlendWeight.w = AIWeight.mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		m_BoneIndices.push_back(pModel->Get_BoneIndex(m_szName));

		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const CModel* pModel, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModel, pAIMesh, PreTransformMatrix)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg, CGameObject* pOwner)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
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
