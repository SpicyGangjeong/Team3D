#include "pch.h"
#include "Navigation.h"

#include "Cell.h"
#include "Shader.h"
#include "GameInstance.h"

_float4x4 CNavigation::s_WorldMatrix = { };

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
{
}

HRESULT CNavigation::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr == pArg) {
		return S_OK;
	}

#ifdef _DEBUG
	m_pShader = (CShader*)m_pGameInstance->Clone_Asset_Prototype(g_iStaticLevel, TEXT("FX_CELL"), nullptr, nullptr);
	if (nullptr == m_pShader) {
		return E_FAIL;
	}
#endif

	NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

	m_iCurrentCellIndex = pDesc->iCurrentCellIndex;
	m_pCells = pDesc->pCells;

	for (auto& pCell : *m_pCells) {
		SAFE_ADDREF(pCell);
	}

	return S_OK;
}

void CNavigation::Update(_fmatrix WorldMatrix) {
	XMStoreFloat4x4(&s_WorldMatrix, WorldMatrix);
}

HRESULT CNavigation::isMove(_fvector vCurrentPos, _vector vCurrentDir, _vector& vCorrectOut)
{
	_matrix		matLocal = XMMatrixInverse(nullptr, XMLoadFloat4x4(&s_WorldMatrix));
	_vector		vLocalCurrentPos = XMVector3TransformCoord(vCurrentPos, matLocal);
	_vector		vCurrentDirection = XMVector3TransformNormal(vCurrentDir, matLocal);
	_vector		vEstimatedDestination = vLocalCurrentPos + vCurrentDirection;

	_float  fOuts[3] = {  };
	_uint	iCrossCount = { 0 };
	CCell* pCell = (*m_pCells)[m_iCurrentCellIndex];

	if (SUCCEEDED(pCell->isIn(vEstimatedDestination, fOuts, iCrossCount))) {
		vCorrectOut = XMVector3TransformNormal(vCurrentDirection, XMLoadFloat4x4(&s_WorldMatrix));
		return S_OK;
	}

	// iCrossCount > 0 이므로
	// 슬라이딩 혹은 이웃찾기 혹은 블록
	_uint iCrossedNeighborCount = 0;
	_int iBlockedLine = -1;
	_int iCrossedNeighbor = -1;
	{
		_int iTempNeighbor = -1;
		for (int i = 0; i < 3; ++i) {
			if (fOuts[i] < 0.f) { // 선분에 가로막혔는데
				iTempNeighbor = (*m_pCells)[m_iCurrentCellIndex]->Get_Neighbor((NAVI_LINE)i);
				if (-1 != iTempNeighbor) { // 이웃으로 향하는 길 이면
					iCrossedNeighborCount++;
					iCrossedNeighbor = iTempNeighbor;
				}
				else {
					iBlockedLine = i; // 진짜 막힌 길이면  저장
				}
			}
		}
	}
	// 10 11
	// 20 21 22
	if (iCrossCount == iCrossedNeighborCount) // 가로막혔던 길이 모두 이웃이 있는 길이었으면
	{
		m_iCurrentCellIndex = iCrossedNeighbor; // 아무 방향으로나
		vCorrectOut = XMVector3TransformNormal(vCurrentDirection, XMLoadFloat4x4(&s_WorldMatrix));
		return S_OK; // 이동
	}

	// 10
	// 20 21
	if (iCrossCount > iCrossedNeighborCount) // 가로막혔던 길 중 이웃으로 향하는 길이 더 적다면
	{
		if (iCrossCount == 2 && iCrossedNeighborCount == 0) {// 가로막혔던 길이 둘 다 막힌 길이었으면 이동불가능
			return E_FAIL;
		}

		// 10
		// 21
		// 막힌 길로 슬라이딩 타면 됨

		// 슬라이딩 벡터
		_vector vNeighborNormal = -XMLoadFloat3(pCell->Get_Normarl((NAVI_LINE)iBlockedLine));
		vCurrentDirection = vCurrentDirection - XMVector3Dot(vCurrentDirection, vNeighborNormal) * vNeighborNormal;
		if (iCrossedNeighborCount == 0) {
			// 1 0
			vCorrectOut = XMVector3TransformNormal(vCurrentDirection, XMLoadFloat4x4(&s_WorldMatrix));
		}
		else {
			// 2 1
			vEstimatedDestination = vLocalCurrentPos + vCurrentDirection;
			memset(fOuts, 0, sizeof(_float) * 3);
			iCrossCount = { 0 };
			if (SUCCEEDED((*m_pCells)[iCrossedNeighbor]->isIn(vEstimatedDestination, fOuts, iCrossCount))) {
				m_iCurrentCellIndex = iCrossedNeighbor;
			}
			else { // K 자 네비도트가 있을 때 아래 삼각형에서 바로 윗삼각형으로 갔을 경우임
				int iFoundCell = Find_Cell_ByPosition(vEstimatedDestination, m_iCurrentCellIndex, 5);

				if (iFoundCell >= 0)
				{
					m_iCurrentCellIndex = iFoundCell;
					vCorrectOut = XMVector3TransformNormal(vCurrentDirection, XMLoadFloat4x4(&s_WorldMatrix));
					return S_OK;
				}
				else
				{
					return E_FAIL; // 완전히 네비 밖
				}
			}
		}
		return S_OK;
	}
	else {
		assert(false);
		// 불가능, 
		// 경우의 수가 없음
	}
	return S_OK;
}
HRESULT CNavigation::Get_NearCell(_fvector vCurrentPos, _int& iDestCell)
{
	_matrix		matLocal = XMMatrixInverse(nullptr, XMLoadFloat4x4(&s_WorldMatrix));
	_vector		vEstimatedDestination = XMVector3TransformCoord(vCurrentPos, matLocal);

	_float  fOuts[3] = {  };
	_uint	iCrossCount = { 0 };
	CCell* pCell = (*m_pCells)[m_iCurrentCellIndex];

	if (SUCCEEDED(pCell->isIn(vEstimatedDestination, fOuts, iCrossCount))) {
		iDestCell = m_iCurrentCellIndex;
		return S_OK;
	}

	// iCrossCount > 0 이므로
	// 슬라이딩 혹은 이웃찾기 혹은 블록
	_uint iCrossedNeighborCount = 0;
	_int iBlockedLine = -1;
	_int iCrossedNeighbor = -1;
	{
		_int iTempNeighbor = -1;
		for (int i = 0; i < 3; ++i) {
			if (fOuts[i] < 0.f) { // 선분에 가로막혔는데
				iTempNeighbor = (*m_pCells)[m_iCurrentCellIndex]->Get_Neighbor((NAVI_LINE)i);
				if (-1 != iTempNeighbor) { // 이웃으로 향하는 길 이면
					iCrossedNeighborCount++;
					iCrossedNeighbor = iTempNeighbor;
				}
				else {
					iBlockedLine = i; // 진짜 막힌 길이면  저장
				}
			}
		}
	}
	// 10 11
	// 20 21 22
	if (iCrossCount == iCrossedNeighborCount) // 가로막혔던 길이 모두 이웃이 있는 길이었으면
	{
		iDestCell = iCrossedNeighbor;
		return S_OK; // 이동
	}
	return E_FAIL;
}
_ubyte CNavigation::Get_CellAttribute()
{
	return (*m_pCells)[m_iCurrentCellIndex]->Get_CellAttribute();
}

_int CNavigation::Find_Cell_ByPosition(_fvector vLocalPos, _uint iStartCell, _uint iMaxDepth)
{
	const _uint iCellCount = (_uint)(*m_pCells).size();
	vector<_bool> visited(iCellCount, false);

	struct Node { _int idx; _uint depth; };
	deque<Node> q;

	q.push_back({ (_int)iStartCell, 0 });
	visited[iStartCell] = true;

	while (!q.empty())
	{
		Node cur = q.front();
		q.pop_front();

		_float fOuts[3] = {};
		_uint iCrossCnt = 0;
		if (SUCCEEDED((*m_pCells)[cur.idx]->isIn(vLocalPos, fOuts, iCrossCnt)))
		{
			return cur.idx;
		}

		if (cur.depth < iMaxDepth)
		{
			for (int i = 0; i < 3; ++i)
			{
				_int iNeighbor = (*m_pCells)[cur.idx]->Get_Neighbor((NAVI_LINE)i);
				if (iNeighbor >= 0 && !visited[iNeighbor])
				{
					visited[iNeighbor] = true;
					q.push_back({ iNeighbor, cur.depth + 1 });
				}
			}
		}
	}

	return -1;
}

_bool CNavigation::Compute_Height(CTransform* pTransform, _float fTimeDelta, _bool bIsHover)
{
	return false;
}

const vector<CCell*>* CNavigation::Get_CellPtr()
{
	return m_pCells;
}

HRESULT CNavigation::isIn(_fvector vLocalPos)
{
	_float  fOuts[3] = {  };
	_uint	iCrossCount = { 0 };
	_matrix		matLocal = XMMatrixInverse(nullptr, XMLoadFloat4x4(&s_WorldMatrix));
	_vector		vLocalCurrentPos = XMVector3TransformCoord(vLocalPos, matLocal); 
	return (*m_pCells)[m_iCurrentCellIndex]->isIn(vLocalCurrentPos, fOuts, iCrossCount);
}

#ifdef _DEBUG

void CNavigation::Change_PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY eType)
{
	if (-1 != m_iCurrentCellIndex)
	{
		return;
	}

	for (auto& pCell : *m_pCells) {
		pCell->Change_PrimitiveTopology(eType);
	}
}

HRESULT CNavigation::Render()
{
	if ((*m_pCells).empty()) {
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}

	_float4		vColor = { };
	_float4		vRed = { 1.f, 0.f, 0.f, 1.f };
	_float4x4	WorldMatrix = s_WorldMatrix;
	{
		if (-1 != m_iCurrentCellIndex) {
			WorldMatrix._42 += 0.25f;
			vColor = vRed;
		}
		else
		{
			WorldMatrix._42 += 0.15f;
			vColor = _float4(0.f, 1.f, 1.f, 1.f);
		}
	}

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix))) {
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4)))) {
		return E_FAIL;
	}

	m_pShader->Begin(ENUM_CLASS(SHADER_PASS_CELL::DEBUG));

	if (-1 != m_iCurrentCellIndex)
	{
		return (*m_pCells)[m_iCurrentCellIndex]->Render();
	}

	for (auto& pCell : *m_pCells) {
		pCell->Render();
	}

	return S_OK;
}
#endif

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNavigation");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


CComponent* CNavigation::Clone(void* pArg, class CGameObject* pOwner)
{
	CNavigation* pInstance = new CNavigation(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


void CNavigation::Free()
{
	__super::Free();

#ifdef _DEBUG
	SAFE_RELEASE(m_pShader);
#endif
	if (nullptr != m_pCells) {
		for (CCell*& pCell : (*m_pCells)) {
			SAFE_RELEASE(pCell);
		}
	}
}
#ifdef _DEBUG

void CNavigation::Describe_Entity()
{
}
#endif // _DEBUG
