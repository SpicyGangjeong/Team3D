#include "pch.h"
#include "Cell.h"

#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
}

_vector CCell::Get_Point(NAVI_POINT ePoint)
{
	return XMLoadFloat3(&m_vPoints[ENUM_CLASS(ePoint)]);
}

const _float3* CCell::Get_ConstPoint(NAVI_POINT ePoint)
{
	return &m_vPoints[ENUM_CLASS(ePoint)];
}

const _float3* CCell::Get_Normarl(NAVI_LINE eLine)
{
	return &m_vNormals[ENUM_CLASS(eLine)];
}

void CCell::Set_Neighbor(NAVI_LINE eLine, CCell* pNeighbor) 
{
	m_NeighborIndices[ENUM_CLASS(eLine)] = pNeighbor->m_iIndex;
}

_int CCell::Get_Neighbor(NAVI_LINE eLine)
{
	return m_NeighborIndices[ENUM_CLASS(eLine)];
}

_ubyte CCell::Get_CellAttribute()
{
	return m_ubyteCellAttribute;
}

HRESULT CCell::Initialize(const _float3* pPoints, _uint iIndex)
{
	if (nullptr == pPoints) {
		return S_OK; // 로딩 전용
	}
	memcpy(m_vPoints, pPoints, sizeof(_float3) * ENUM_CLASS(NAVI_POINT::END));

	XMStoreFloat3(&m_vLines[ENUM_CLASS(NAVI_LINE::AB)], XMVector3Normalize(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::B)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::A)])));
	XMStoreFloat3(&m_vLines[ENUM_CLASS(NAVI_LINE::BC)], XMVector3Normalize(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::C)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::B)])));
	XMStoreFloat3(&m_vLines[ENUM_CLASS(NAVI_LINE::CA)], XMVector3Normalize(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::A)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::C)])));

	m_vNormals[ENUM_CLASS(NAVI_LINE::AB)] = _float3(m_vLines[ENUM_CLASS(NAVI_LINE::AB)].z * -1.f, 0.f, m_vLines[ENUM_CLASS(NAVI_LINE::AB)].x);
	m_vNormals[ENUM_CLASS(NAVI_LINE::BC)] = _float3(m_vLines[ENUM_CLASS(NAVI_LINE::BC)].z * -1.f, 0.f, m_vLines[ENUM_CLASS(NAVI_LINE::BC)].x);
	m_vNormals[ENUM_CLASS(NAVI_LINE::CA)] = _float3(m_vLines[ENUM_CLASS(NAVI_LINE::CA)].z * -1.f, 0.f, m_vLines[ENUM_CLASS(NAVI_LINE::CA)].x);

	m_iIndex = iIndex;

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, pPoints);
	if (nullptr == m_pVIBuffer) {
		return E_FAIL;
	}
#endif


	XMStoreFloat4(&m_vPlane, XMPlaneFromPoints( XMLoadFloat3(&m_vPoints[0]), XMLoadFloat3(&m_vPoints[1]), XMLoadFloat3(&m_vPoints[2])));

	return S_OK;
}

HRESULT CCell::isIn(_fvector vLocalPos, _float* fOuts, _uint& iCrossCnt)
{
	HRESULT inside = S_OK; iCrossCnt = 0;
	for (int i = 0; i < 3; ++i)
	{
		_vector vToPosition = XMVectorSetY(vLocalPos - XMLoadFloat3(&m_vPoints[i]), 0.f);
		_vector vLine = XMVectorSetY(XMLoadFloat3(&m_vLines[i]), 0.f);

		fOuts[i] = XMVectorGetY(XMVector3Cross(vLine, vToPosition));
		if (fOuts[i] < 0.f)
		{
			iCrossCnt++;
			inside = E_FAIL;
		}
	}
	return inside; 
}

_bool CCell::Compare(_fvector vSourPoint, _fvector vDestPoint)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::A)]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::B)]), vDestPoint))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::C)]), vDestPoint))
			return true;
	}
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::B)]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::C)]), vDestPoint))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::A)]), vDestPoint))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::C)]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::A)]), vDestPoint))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(NAVI_POINT::B)]), vDestPoint))
			return true;
	}
	return false;
}

_float CCell::Compute_Height(_fvector vPoint)
{
	return (-m_vPlane.x * XMVectorGetX(vPoint) 
		- m_vPlane.z * XMVectorGetZ(vPoint) - m_vPlane.w) / m_vPlane.y;
}
HRESULT CCell::LoadAsBinary(HANDLE hFile, DWORD& dwByte, _uint iCellIndex)
{
	m_iIndex = iCellIndex;
	if (!ReadFile(hFile, m_vPoints, sizeof(_float3) * ENUM_CLASS(NAVI_POINT::END), &dwByte, nullptr)) {
		return E_FAIL;
	}
	if (!ReadFile(hFile, m_vLines, sizeof(_float3) * ENUM_CLASS(NAVI_LINE::END), &dwByte, nullptr)) {
		return E_FAIL;
	}
	if (!ReadFile(hFile, m_vNormals, sizeof(_float3) * ENUM_CLASS(NAVI_LINE::END), &dwByte, nullptr)) {
		return E_FAIL;
	}
	if (!ReadFile(hFile, &m_vPlane, sizeof(_float4), &dwByte, nullptr)) {
		return E_FAIL;
	}
	if (!ReadFile(hFile, &m_ubyteCellAttribute, sizeof(_ubyte), &dwByte, nullptr)) {
		return E_FAIL;
	}

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
#endif // _DEBUG
	return S_OK;
}
#ifdef _DEBUG

HRESULT CCell::SaveAsBinary(HANDLE hFile, DWORD& dwByte)
{
	if (UINT_MAX == m_iIndex) {
		assert(false); // 초기화 되지 않은 정점 저장 시도
		return E_FAIL;
	}
	WriteFile(hFile, m_vPoints, sizeof(_float3) * ENUM_CLASS(NAVI_POINT::END), &dwByte, nullptr);
	WriteFile(hFile, m_vLines, sizeof(_float3) * ENUM_CLASS(NAVI_LINE::END), &dwByte, nullptr);
	WriteFile(hFile, m_vNormals, sizeof(_float3) * ENUM_CLASS(NAVI_LINE::END), &dwByte, nullptr);
	WriteFile(hFile, &m_vPlane, sizeof(_float4), &dwByte, nullptr);
	WriteFile(hFile, &m_ubyteCellAttribute, sizeof(_ubyte), &dwByte, nullptr);
	return S_OK;
}


void CCell::ReallocatedCellIndex(_uint iIndex)
{
	m_iIndex = iIndex;
}

void CCell::Change_PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY eType) {
	m_pVIBuffer->Change_PrimitiveTopology(eType);
}
HRESULT CCell::Render()
{
	if (nullptr != m_pVIBuffer) {
		m_pVIBuffer->Bind_Resources();

		m_pVIBuffer->Render();
	}

	return S_OK;
}
#endif // _DEBUG


CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* vPoints, _uint iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(vPoints, iIndex)))
	{
		MSG_BOX("Failed to Created : CCell");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
	__super::Free();



#ifdef _DEBUG
	SAFE_RELEASE(m_pVIBuffer);
#endif

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
