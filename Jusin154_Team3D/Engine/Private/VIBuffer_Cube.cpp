#include "pch.h"
#include "VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Cube::Bind_Resources()
{
	return __super::Bind_Resources();
}

HRESULT CVIBuffer_Cube::Bind_Resources(_uint iStartSlot, _uint iNumBuffer, vector<ID3D11Buffer*>& pBuffer, vector<_uint>& strides, vector<_uint>& offsets)
{
	vector<ID3D11Buffer*> vBuffers;
	vBuffers.reserve(1 + iNumBuffer);
	vBuffers.push_back(m_pVB);
	for (ID3D11Buffer* iter : pBuffer) {
		vBuffers.push_back(iter);
	}
	vector<_uint> vStrides;
	vStrides.reserve(1 + iNumBuffer);
	vStrides.push_back(m_iVertexStride);
	for (_uint iter : strides) {
		vStrides.push_back(iter);
	}
	vector<_uint> vOffsets;
	vOffsets.reserve(1 + iNumBuffer);
	vOffsets.push_back(0);
	for (_uint iter : offsets) {
		vOffsets.push_back(iter);
	}



	m_pContext->IASetVertexBuffers(iStartSlot, 1 + iNumBuffer, vBuffers.data(), vStrides.data(), vOffsets.data());
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitive);
	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = 24;
	m_iVertexStride = sizeof(VTXBLOCK);

	m_iIndexStride = sizeof(_uint);
	m_iNumIndices = 36;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VTX_BUFFER
	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	m_pVertexPositions = new _float3[m_iNumVertices];
	VTXBLOCK* pVertices = new VTXBLOCK[m_iNumVertices];
	memset(pVertices, 0, sizeof(VTXBLOCK) * m_iNumVertices);

	m_pVertexPositions[0] = pVertices[0].vTexcoord = pVertices[0].vPosition = { -0.5f, 0.5f, -0.5f };
	m_pVertexPositions[1] = pVertices[1].vTexcoord = pVertices[1].vPosition = { 0.5f, 0.5f, -0.5f };
	m_pVertexPositions[2] = pVertices[2].vTexcoord = pVertices[2].vPosition = { 0.5f, 0.5f, 0.5f };
	m_pVertexPositions[3] = pVertices[3].vTexcoord = pVertices[3].vPosition = { -0.5f, 0.5f, 0.5f };
	XMStoreFloat3(&pVertices[0].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[0].vTexcoord)));
	XMStoreFloat3(&pVertices[1].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[1].vTexcoord)));
	XMStoreFloat3(&pVertices[2].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[2].vTexcoord)));
	XMStoreFloat3(&pVertices[3].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[3].vTexcoord)));
	for (int i = 0; i < 4; ++i) {
		pVertices[i].vNormal = { 0,1,0 };
	}


	m_pVertexPositions[4] = pVertices[4].vTexcoord = pVertices[4].vPosition = { -0.5f, -0.5f, 0.5f };
	m_pVertexPositions[5] = pVertices[5].vTexcoord = pVertices[5].vPosition = { 0.5f, -0.5f, 0.5f };
	m_pVertexPositions[6] = pVertices[6].vTexcoord = pVertices[6].vPosition = { 0.5f, 0.5f, 0.5f };
	m_pVertexPositions[7] = pVertices[7].vTexcoord = pVertices[7].vPosition = { -0.5f, 0.5f, 0.5f };
	XMStoreFloat3(&pVertices[4].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[4].vTexcoord)));
	XMStoreFloat3(&pVertices[5].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[5].vTexcoord)));
	XMStoreFloat3(&pVertices[6].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[6].vTexcoord)));
	XMStoreFloat3(&pVertices[7].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[7].vTexcoord)));

	for (int i = 4; i < 8; ++i) {
		pVertices[i].vNormal = { 0,0,1 };
	}


	m_pVertexPositions[8] = pVertices[8].vTexcoord = pVertices[8].vPosition = { 0.5f, -0.5f, 0.5f };
	m_pVertexPositions[9] = pVertices[9].vTexcoord = pVertices[9].vPosition = { 0.5f, 0.5f, 0.5f };
	m_pVertexPositions[10] = pVertices[10].vTexcoord = pVertices[10].vPosition = { 0.5f, 0.5f, -0.5f };
	m_pVertexPositions[11] = pVertices[11].vTexcoord = pVertices[11].vPosition = { 0.5f, -0.5f, -0.5f };
	XMStoreFloat3(&pVertices[8].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[8].vTexcoord)));
	XMStoreFloat3(&pVertices[9].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[9].vTexcoord)));
	XMStoreFloat3(&pVertices[10].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[10].vTexcoord)));
	XMStoreFloat3(&pVertices[11].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[11].vTexcoord)));

	for (int i = 8; i < 12; ++i) {
		pVertices[i].vNormal = { 1,0,0 };
	}


	m_pVertexPositions[12] = pVertices[12].vTexcoord = pVertices[12].vPosition = { 0.5f, -0.5f, -0.5f };
	m_pVertexPositions[13] = pVertices[13].vTexcoord = pVertices[13].vPosition = { -0.5f, -0.5f, -0.5f };
	m_pVertexPositions[14] = pVertices[14].vTexcoord = pVertices[14].vPosition = { -0.5f, -0.5f, 0.5f };
	m_pVertexPositions[15] = pVertices[15].vTexcoord = pVertices[15].vPosition = { 0.5f, -0.5f, 0.5f };
	XMStoreFloat3(&pVertices[12].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[12].vTexcoord)));
	XMStoreFloat3(&pVertices[13].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[13].vTexcoord)));
	XMStoreFloat3(&pVertices[14].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[14].vTexcoord)));
	XMStoreFloat3(&pVertices[15].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[15].vTexcoord)));

	for (int i = 12; i < 16; ++i) {
		pVertices[i].vNormal = { 0,-1,0 };
	}


	m_pVertexPositions[16] = pVertices[16].vTexcoord = pVertices[16].vPosition = { -0.5f, -0.5f, -0.5f };
	m_pVertexPositions[17] = pVertices[17].vTexcoord = pVertices[17].vPosition = { -0.5f, 0.5f, -0.5f };
	m_pVertexPositions[18] = pVertices[18].vTexcoord = pVertices[18].vPosition = { -0.5f, 0.5f, 0.5f };
	m_pVertexPositions[19] = pVertices[19].vTexcoord = pVertices[19].vPosition = { -0.5f, -0.5f, 0.5f };
	XMStoreFloat3(&pVertices[16].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[16].vTexcoord)));
	XMStoreFloat3(&pVertices[17].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[17].vTexcoord)));
	XMStoreFloat3(&pVertices[18].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[18].vTexcoord)));
	XMStoreFloat3(&pVertices[19].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[19].vTexcoord)));

	for (int i = 16; i < 20; ++i) {
		pVertices[i].vNormal = { -1,0,0 };
	}

	m_pVertexPositions[20] = pVertices[20].vTexcoord = pVertices[20].vPosition = { 0.5f, -0.5f, -0.5f };
	m_pVertexPositions[21] = pVertices[21].vTexcoord = pVertices[21].vPosition = { 0.5f, 0.5f, -0.5f };
	m_pVertexPositions[22] = pVertices[22].vTexcoord = pVertices[22].vPosition = { -0.5f, 0.5f, -0.5f };
	m_pVertexPositions[23] = pVertices[23].vTexcoord = pVertices[23].vPosition = { -0.5f, -0.5f, -0.5f };
	XMStoreFloat3(&pVertices[20].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[20].vTexcoord)));
	XMStoreFloat3(&pVertices[21].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[21].vTexcoord)));
	XMStoreFloat3(&pVertices[22].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[22].vTexcoord)));
	XMStoreFloat3(&pVertices[23].vTexcoord, XMVector3Normalize(XMLoadFloat3(&pVertices[23].vTexcoord)));

	for (int i = 20; i < 24; ++i) {
		pVertices[i].vNormal = { 0,0,-1 };
	}

	D3D11_SUBRESOURCE_DATA VBRes{};
	VBRes.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBRes, &m_pVB))) {
		return E_FAIL;
	}
	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region IDX_BUFFER
	D3D11_BUFFER_DESC IBDesc{};

	IBDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;


	_uint* pIndices = new _uint[m_iNumIndices]{};
	m_pIndices = new _uint[m_iNumIndices];

	m_pIndices[0] = pIndices[0] = 1;
	m_pIndices[1] = pIndices[1] = 0;
	m_pIndices[2] = pIndices[2] = 3;

	m_pIndices[3] = pIndices[3] = 2;
	m_pIndices[4] = pIndices[4] = 1;
	m_pIndices[5] = pIndices[5] = 3;

	m_pIndices[6] = pIndices[6] = 6;
	m_pIndices[7] = pIndices[7] = 7;
	m_pIndices[8] = pIndices[8] = 4;

	m_pIndices[9] = pIndices[9] = 4;
	m_pIndices[10] = pIndices[10] = 5;
	m_pIndices[11] = pIndices[11] = 6;

	m_pIndices[12] = pIndices[12] = 10;
	m_pIndices[13] = pIndices[13] = 9;
	m_pIndices[14] = pIndices[14] = 8;

	m_pIndices[15] = pIndices[15] = 8;
	m_pIndices[16] = pIndices[16] = 11;
	m_pIndices[17] = pIndices[17] = 10;


	m_pIndices[18] = pIndices[18] = 13;
	m_pIndices[19] = pIndices[19] = 12;
	m_pIndices[20] = pIndices[20] = 15;

	m_pIndices[21] = pIndices[21] = 15;
	m_pIndices[22] = pIndices[22] = 14;
	m_pIndices[23] = pIndices[23] = 13;

	m_pIndices[24] = pIndices[24] = 18;
	m_pIndices[25] = pIndices[25] = 17;
	m_pIndices[26] = pIndices[26] = 16;

	m_pIndices[27] = pIndices[27] = 16;
	m_pIndices[28] = pIndices[28] = 19;
	m_pIndices[29] = pIndices[29] = 18;

	m_pIndices[30] = pIndices[30] = 22;
	m_pIndices[31] = pIndices[31] = 21;
	m_pIndices[32] = pIndices[32] = 20;

	m_pIndices[33] = pIndices[33] = 20;
	m_pIndices[34] = pIndices[34] = 23;
	m_pIndices[35] = pIndices[35] = 22;

	D3D11_SUBRESOURCE_DATA IBRes{};
	IBRes.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBRes, &m_pIB))) {
		return E_FAIL;
	}
	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Cube* CVIBuffer_Cube::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cube");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CVIBuffer_Cube* CVIBuffer_Cube::Clone(void* pArg, CGameObject* pOwner)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Cube");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();


}

void CVIBuffer_Cube::Describe_Entity()
{
}
