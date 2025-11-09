#include "pch.h"
#include "VIBuffer_Terrain.h"

#include "GameInstance.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(CVIBuffer_Terrain& rhs)
	:CVIBuffer(rhs),
	m_iNumVerticesX{ rhs.m_iNumVerticesX },
	m_iNumVerticesZ{ rhs.m_iNumVerticesZ }
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _char* pFilePath, _uint iSizeX, _uint iSizeZ)
{
	m_bChange = { false };

	m_iNumVerticesX = iSizeX;
	m_iNumVerticesZ = iSizeZ;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	//if (FAILED(Load_HeightFile(pFilePath)))
	//	return E_FAIL;

	m_iNumVertexBuffers = 1;
	m_iVertexStride = sizeof(VTXNORTEX);

	_uint* pPixels = new _uint[m_iNumVertices];
	ZeroMemory(pPixels, sizeof(_uint) * m_iNumVertices);

	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3;
	m_iIndexStride = 4;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VETEX_BUFFER
	D3D11_BUFFER_DESC VBDesc = {};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.StructureByteStride = m_iVertexStride;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIndex = j + i * m_iNumVerticesX;

			m_pVertexPositions[iIndex] = pVertices[iIndex].vPosition = _float3((_float)j, 0.f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
		}
	}
	D3D11_SUBRESOURCE_DATA InitVBData = {};
	InitVBData.pSysMem = pVertices;
#pragma endregion

#pragma region INDEX_BUFFER
	D3D11_BUFFER_DESC IBDesc = {};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.StructureByteStride = m_iIndexStride;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint iNumIndex = {};
	_uint iCountZ = m_iNumVerticesZ - 1;
	_uint iCountX = m_iNumVerticesX - 1;
	for (_uint i = 0; i < iCountZ; ++i)
	{
		for (_uint j = 0; j < iCountX; ++j)
		{
			_uint iIndex = j + i * m_iNumVerticesX;

			_uint Indices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex,
			};

			pIndices[iNumIndex++] = Indices[0];
			pIndices[iNumIndex++] = Indices[1];
			pIndices[iNumIndex++] = Indices[2];

			_vector vSrc, vDst, vNormal;

			vSrc = XMLoadFloat3(&pVertices[Indices[1]].vPosition) - XMLoadFloat3(&pVertices[Indices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[Indices[2]].vPosition) - XMLoadFloat3(&pVertices[Indices[0]].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[Indices[0]].vNormal, XMLoadFloat3(&pVertices[Indices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[Indices[1]].vNormal, XMLoadFloat3(&pVertices[Indices[1]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[Indices[2]].vNormal, XMLoadFloat3(&pVertices[Indices[2]].vNormal) + vNormal);

			pIndices[iNumIndex++] = Indices[0];
			pIndices[iNumIndex++] = Indices[2];
			pIndices[iNumIndex++] = Indices[3];

			vSrc = XMLoadFloat3(&pVertices[Indices[2]].vPosition) - XMLoadFloat3(&pVertices[Indices[0]].vPosition);
			vDst = XMLoadFloat3(&pVertices[Indices[3]].vPosition) - XMLoadFloat3(&pVertices[Indices[2]].vPosition);

			vNormal = XMVector3Normalize(XMVector3Cross(vSrc, vDst));

			XMStoreFloat3(&pVertices[Indices[0]].vNormal, XMLoadFloat3(&pVertices[Indices[0]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[Indices[2]].vNormal, XMLoadFloat3(&pVertices[Indices[2]].vNormal) + vNormal);
			XMStoreFloat3(&pVertices[Indices[3]].vNormal, XMLoadFloat3(&pVertices[Indices[3]].vNormal) + vNormal);
		}
	}

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
	}

	D3D11_SUBRESOURCE_DATA InitIBData = {};
	InitIBData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitVBData, &m_pVB)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitIBData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	Safe_Delete_Array(pPixels);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

_bool CVIBuffer_Terrain::Picking(CTransform* pTransform, _float3& pOut)
{
	_matrix WorldMatrixInv = pTransform->Get_WorldMatrixInv();

	m_pGameInstance->Ray_WorldToLocal(&WorldMatrixInv);

	_uint iNumIndices = {};

	for (_uint i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint	iIndex = i * m_iNumVerticesX + j;

			_uint	iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			if (true == m_pGameInstance->MousePicking_InLocalSpace(m_pVertexPositions[iIndices[0]], m_pVertexPositions[iIndices[1]], m_pVertexPositions[iIndices[2]], pOut))
			{
				XMStoreFloat3(&pOut, XMVector3TransformCoord(XMLoadFloat3(&pOut), XMLoadFloat4x4(pTransform->Get_WorldMatrixPtr())));
				return true;
			}

			if (true == m_pGameInstance->MousePicking_InWorldSpace(m_pVertexPositions[iIndices[0]], m_pVertexPositions[iIndices[2]], m_pVertexPositions[iIndices[3]], pOut))
			{
				XMStoreFloat3(&pOut, XMVector3TransformCoord(XMLoadFloat3(&pOut), XMLoadFloat4x4(pTransform->Get_WorldMatrixPtr())));
				return true;
			}
		}
	}

	return false;
}
/

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _char * pFilePath, _uint iSizeX, _uint iSizeZ)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pFilePath, iSizeX, iSizeZ)))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Terrain");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Clone(void* pArg, CGameObject* pOwner)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}

void CVIBuffer_Terrain::Describe_Entity()
{
}