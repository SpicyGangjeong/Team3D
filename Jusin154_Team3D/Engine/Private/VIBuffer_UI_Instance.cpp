#include "pch.h"
#include "VIBuffer_UI_Instance.h"
#include "GameInstance.h"

CVIBuffer_UI_Instance::CVIBuffer_UI_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_UI_Instance::CVIBuffer_UI_Instance(const CVIBuffer_UI_Instance& Prototype)
	: CVIBuffer_Instance{ Prototype }
	, m_pInstanceVertices{ Prototype.m_pInstanceVertices }
	, m_fPosition{ Prototype.m_fPosition }
{
}

HRESULT CVIBuffer_UI_Instance::Initialize_Prototype(const INSTANCE_DESC* pInstanceDesc)
{
	m_iNumVertexBuffers = 2;
	m_iNumVertices = 4;
	m_iVertexStride = sizeof(VTXPOSTEX);

	m_iNumIndices = 6;
	m_iIndexStride = 2;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.StructureByteStride = m_iVertexStride;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	m_pVertexPositions[0] = pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexcoord = _float2(0.0f, 0.0f);

	m_pVertexPositions[1] = pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.0f, 0.0f);

	m_pVertexPositions[2] = pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.0f, 1.0f);

	m_pVertexPositions[3] = pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.0f, 1.0f);

	D3D11_SUBRESOURCE_DATA	InitialVBData{};
	InitialVBData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &InitialVBData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER
	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.StructureByteStride = m_iIndexStride;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;


	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0;
	pIndices[4] = 2;
	pIndices[5] = 3;

	D3D11_SUBRESOURCE_DATA	InitialIBData{};
	InitialIBData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER
	const UI_INSTANCE_DESC* pDesc = static_cast<const UI_INSTANCE_DESC*>(pInstanceDesc);

	m_iNumInstance = pDesc->iNum;
	m_iInstanceStride = sizeof(VTX_INSTANCE_UI);
	m_iNumIndexPerInstance = 6;

	m_InstanceBufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceStride;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTX_INSTANCE_UI[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTX_INSTANCE_UI) * m_iNumInstance);

	m_fPosition = new _float2[m_iNumInstance];
	ZeroMemory(m_fPosition, sizeof(_float2) * m_iNumInstance);


	m_InstanceInitialDesc.pSysMem = m_pInstanceVertices;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_UI_Instance::Initialize(void* pArg)
{
	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &m_InstanceInitialDesc, &m_pVBInstance)))
		return E_FAIL;


	return S_OK;
}

void CVIBuffer_UI_Instance::Set_Index_Position(_uint iIndex, _float fX, _float fY)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTX_INSTANCE_UI* pVertices = static_cast<VTX_INSTANCE_UI*>(SubResource.pData);

	pVertices[iIndex].fPos.x = fX;
	pVertices[iIndex].fPos.y = fY;

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_UI_Instance::Set_Index_Size(_uint iIndex, _float fSizeX, _float fSizeY)
{
	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTX_INSTANCE_UI* pVertices = static_cast<VTX_INSTANCE_UI*>(SubResource.pData);

	pVertices[iIndex].fSize.x = fSizeX;
	pVertices[iIndex].fSize.y = fSizeY;

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_UI_Instance::Set_Pos(_float fX, _float fY, _float OffSetX, _float OffSetY, _uint iCols)
{

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTX_INSTANCE_UI* pVertices = static_cast<VTX_INSTANCE_UI*>(SubResource.pData);

	for (_uint i = 0; i < m_iNumInstance; i++)
	{
		_uint col = i % iCols;      // 가로 인덱스
		_uint row = i / iCols;      // 세로 인덱스

		pVertices[i].fPos.x = fX + col * OffSetX;
		pVertices[i].fPos.y = fY - row * OffSetY;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_UI_Instance::Set_Size(_float fSizeX, _float fSizeY)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTX_INSTANCE_UI* pVertices = static_cast<VTX_INSTANCE_UI*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].fSize.x = fSizeX;
		pVertices[i].fSize.y = fSizeY;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_UI_Instance::Set_SizeX(_float fSizeX)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTX_INSTANCE_UI* pVertices = static_cast<VTX_INSTANCE_UI*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].fSize.x = fSizeX;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_UI_Instance::Set_SizeY(_float fSizeY)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTX_INSTANCE_UI* pVertices = static_cast<VTX_INSTANCE_UI*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].fSize.y = fSizeY;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_UI_Instance::Set_ImageUV(UI_ATLAS_DESC* AtlasUV)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTX_INSTANCE_UI* pVertices = static_cast<VTX_INSTANCE_UI*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		if (AtlasUV == nullptr)
		{
			pVertices[i].fUV = _float4(0.f, 0.f,1.f,1.f);
		}
		else
		{
			pVertices[i].fUV = AtlasUV[i].fUV;
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_UI_Instance* CVIBuffer_UI_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pInstanceDesc)
{
	CVIBuffer_UI_Instance* pInstance = new CVIBuffer_UI_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pInstanceDesc)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_UI_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_UI_Instance::Clone(void* pArg, CGameObject* pOwner)
{
	CVIBuffer_UI_Instance* pInstance = new CVIBuffer_UI_Instance(*this);
	pInstance->m_pOwner = pOwner;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CVIBuffer_UI_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pInstanceVertices);
		Safe_Delete_Array(m_fPosition);
	}
}
#ifdef _DEBUG

void CVIBuffer_UI_Instance::Describe_Entity()
{
}

#endif // _DEBUG
