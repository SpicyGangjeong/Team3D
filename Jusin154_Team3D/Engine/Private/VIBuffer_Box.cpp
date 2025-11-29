#include "pch.h"
#include "VIBuffer_Box.h"

CVIBuffer_Box::CVIBuffer_Box(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Box::CVIBuffer_Box(const CVIBuffer_Box& rhs)
	: CVIBuffer(rhs)
{
}
#ifdef EDITOR_PROJECT
HRESULT CVIBuffer_Box::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 8;
	m_iVertexStride = sizeof(VTXPOS);

	m_iIndexStride = 2;
	m_iNumIndices = 36;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region IB_BUFFER
	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.StructureByteStride = m_iIndexStride;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;


	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	/* + x */
	pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

	/* - x */
	pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;

	/* + y */
	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;

	/* - y */
	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;

	/* + z */
	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;

	/* - z */
	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;


	D3D11_SUBRESOURCE_DATA IBRes{};
	IBRes.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBRes, &m_pIB))) {
		return E_FAIL;
	}
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Box::Initialize(void* pArg)
{
#pragma region VTX_BUFFER
	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	memset(pVertices, 0, sizeof(VTXPOS) * m_iNumVertices);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);

	D3D11_SUBRESOURCE_DATA VBRes{};
	VBRes.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBRes, &m_pVB))) {
		return E_FAIL;
	}
	Safe_Delete_Array(pVertices);

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Box::Update(_float3* pVertexDatas)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	VTXPOS* pVertices = static_cast<VTXPOS*>(SubResource.pData);
	
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pVertexDatas[i], sizeof(_float3));
	}

	m_pContext->Unmap(m_pVB, 0);

	return S_OK;
}

#else
HRESULT CVIBuffer_Box::Initialize(void* pArg)
{
	_float3* pCorners = static_cast<_float3*>(pArg);

#pragma region VTX_BUFFER
	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	memset(pVertices, 0, sizeof(VTXPOS) * m_iNumVertices);

	pVertices[0].vPosition = pCorners[0];
	pVertices[1].vPosition = pCorners[1];
	pVertices[2].vPosition = pCorners[2];
	pVertices[3].vPosition = pCorners[3];
	pVertices[4].vPosition = pCorners[4];
	pVertices[5].vPosition = pCorners[5];
	pVertices[6].vPosition = pCorners[6];
	pVertices[7].vPosition = pCorners[7];

	D3D11_SUBRESOURCE_DATA VBRes{};
	VBRes.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBRes, &m_pVB))) {
		return E_FAIL;
	}
	Safe_Delete_Array(pVertices);

#pragma endregion

	return S_OK;
}


#endif

CVIBuffer_Box* CVIBuffer_Box::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Box* pInstance = new CVIBuffer_Box(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CVIBuffer_Box* CVIBuffer_Box::Clone(void* pArg, CGameObject* pOwner)
{
	CVIBuffer_Box* pInstance = new CVIBuffer_Box(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Box");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Box::Free()
{
	__super::Free();
}
#ifdef _DEBUG

void CVIBuffer_Box::Describe_Entity()
{
}

#endif // _DEBUG
