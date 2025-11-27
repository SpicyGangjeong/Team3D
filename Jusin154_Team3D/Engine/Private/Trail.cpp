#include "pch.h"
#include "Trail.h"

CTrail::CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CTrail::CTrail(const CTrail& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrail::Initialize(void* pArg)
{
	TRAIL_DESC* pDesc = static_cast<TRAIL_DESC*>(pArg);

	if (pDesc == nullptr)
		return E_FAIL;

	m_TrailDesc = *pDesc;

	m_iNumVertexBuffers = 1;
	m_iNumVertices = 256;
	m_iVertexStride = sizeof(VTXPOSTEX);

	m_iNumIndices = (m_iNumVertices / 2 - 1) * 6;
	m_iIndexStride = 4;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pVertices = new VTXPOSTEX[m_iNumVertices];

#pragma region VTX_BUFFER
	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices]{};

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
	IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;


	_uint* pIndices = new _uint[m_iNumIndices]{};

	_uint iIndex = { 0 };

	for (_uint i = 0; i < m_iNumVertices / 2 - 1; i++)
	{
		_uint iStartIndex = i * 2;

		_uint Indicies[4] = {
			iStartIndex ,
			iStartIndex + 1,
			iStartIndex + 2,
			iStartIndex + 3
		};

		pIndices[iIndex++] = Indicies[0];
		pIndices[iIndex++] = Indicies[1];
		pIndices[iIndex++] = Indicies[2];

		pIndices[iIndex++] = Indicies[1];
		pIndices[iIndex++] = Indicies[3];
		pIndices[iIndex++] = Indicies[2];
	}

	D3D11_SUBRESOURCE_DATA InitialIBDate{};
	InitialIBDate.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &InitialIBDate, &m_pIB))) {
		return E_FAIL;
	}

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

void CTrail::Trail_Update(_float fDeltaTime, _fmatrix WorldMatrix)
{
	
	_vector vLow = XMVector3TransformCoord(XMLoadFloat3(&m_TrailDesc.vLow), WorldMatrix);
	_vector vHigh = XMVector3TransformCoord(XMLoadFloat3(&m_TrailDesc.vHigh), WorldMatrix);


	//한칸씩 밀기
	memmove(m_PreLow, m_PreLow + 1, sizeof(VTXPOSTEX));
	memmove(m_PreHigh, m_PreHigh + 1, sizeof(VTXPOSTEX));

	m_PreLow[1] = vLow;
	m_PreHigh[1] = vHigh;

	if (m_iNumCount < 4)
	{
		XMStoreFloat3(&m_pVertices[m_iNumCount++].vPosition, vLow);
		XMStoreFloat3(&m_pVertices[m_iNumCount++].vPosition, vHigh); 

		//카운트가 적으면 시도하지 않음
		return;
	}

	//시작점 끝점 내점 두개를 이용하여 보간할 것임

	for (size_t i = 0; i < 4; i++)
	{
		_float iRatio =  (_float)i / 3;
		_vector vLerpLow = XMVectorCatmullRom(XMLoadFloat3(&m_pVertices[m_iNumCount - 2].vPosition), m_PreLow[0], m_PreLow[1], m_PreLow[1] + (m_PreLow[1] - m_PreLow[0]), iRatio); // LoW
		_vector vLerpHigh = XMVectorCatmullRom(XMLoadFloat3(&m_pVertices[m_iNumCount - 1].vPosition), m_PreHigh[0], m_PreHigh[1], m_PreHigh[1] + (m_PreHigh[1] - m_PreHigh[0]), iRatio);

		XMStoreFloat3(&m_pVertices[m_iNumCount++].vPosition, vLerpLow);
		XMStoreFloat3(&m_pVertices[m_iNumCount++].vPosition, vLerpHigh);

		if (m_iNumCount >= m_iNumVertices)
		{
			memmove(m_pVertices, m_pVertices + 2, sizeof(VTXPOSTEX) * (m_iNumCount - 2));

			m_iNumCount -= 2;
		}

	}

	//XMStoreFloat3(&m_pVertices[0].vPosition, vLow);
	//XMStoreFloat3(&m_pVertices[1].vPosition, vHigh);

	for (_uint i = 0; i < m_iNumCount; i += 2)
	{

		_float u = ((_float)i / (m_iNumCount - 2));

		if (m_iNumCount - 2 <= 0)
			u = 0;

		m_pVertices[i].vTexcoord = _float2(u, 0); // Low

		m_pVertices[i + 1].vTexcoord = _float2(u, 1); // High
	}


	D3D11_MAPPED_SUBRESOURCE		VBResource{};

	if (SUCCEEDED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &VBResource)))
	{

		memcpy(VBResource.pData, m_pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);
		
		m_pContext->Unmap(m_pVB, 0);
	}

}

void CTrail::Reset_Trail()
{
	ZeroMemory(m_pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);
	ZeroMemory(&m_PreHigh, sizeof(_vector) * 2);
	ZeroMemory(&m_PreLow, sizeof(_vector) * 2);
	m_iNumCount = 0;
	m_fAccTime = 0.f;
 }

#ifdef _DEBUG
HRESULT CTrail::Save_Trail(HANDLE hFile)
{
	DWORD	dwByte(0);

	if (!WriteFile(hFile, &m_TrailDesc, sizeof(TRAIL_DESC), &dwByte, nullptr)) {
		return E_FAIL;
	}

	return S_OK;
}
#endif

HRESULT CTrail::Load_Trail(HANDLE hFile)
{
	DWORD	dwByte(0);

	if (!ReadFile(hFile, &m_TrailDesc, sizeof(TRAIL_DESC), &dwByte, nullptr)) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTrail::Render()
{
	if (m_iNumCount < 4)
		return S_OK;

	m_pContext->DrawIndexed( ((m_iNumCount / 2) - 1) * 6, 0, 0);

	return S_OK;
}

CTrail* CTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTrail* pInstance = new CTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrail");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CTrail* CTrail::Clone(void* pArg, CGameObject* pOwner)
{
	CTrail* pInstance = new CTrail(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrail");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTrail::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pVertices);
}
#ifdef _DEBUG

void CTrail::Describe_Entity()
{

	GUI::InputFloat3("Low", (_float*)&m_TrailDesc.vLow);
	GUI::InputFloat3("High", (_float*)&m_TrailDesc.vHigh);
	
}

#endif // _DEBUG
