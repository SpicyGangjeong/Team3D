#include "pch.h"
#include "VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXPOSTEX);

	m_iNumIndices = 6;
	m_iIndexStride = 4;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VTX_BUFFER
	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices]{};
	m_pVertexPositions = new _float3[m_iNumIndices]{};

	m_pVertexPositions[0] = pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.0f, 0.0f);

	m_pVertexPositions[1] = pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.0f, 0.0f);

	m_pVertexPositions[2] = pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.0f, 1.0f);

	m_pVertexPositions[3] = pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.0f, 1.0f);

	D3D11_SUBRESOURCE_DATA InitialVBDate{};
	InitialVBDate.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBDate, &m_pVB))) {
		return E_FAIL;
	}
	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region IDX_BUFFER
	D3D11_BUFFER_DESC IBDesc{};
	IBDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;


	_uint* pIndices = new _uint[m_iNumIndices]{};

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	D3D11_SUBRESOURCE_DATA InitialIBDate{};
	InitialIBDate.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBDate, &m_pIB))) {
		return E_FAIL;
	}
	Safe_Delete_Array(pIndices);
#pragma endregion



	return S_OK;
}

HRESULT CVIBuffer_Rect::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Rect* CVIBuffer_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CVIBuffer_Rect* CVIBuffer_Rect::Clone(void* pArg, CGameObject* pOwner)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	__super::Free();


}

void CVIBuffer_Rect::Describe_Entity()
{
}
