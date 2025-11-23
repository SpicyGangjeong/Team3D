#include "pch.h"
#include "QuadTree.h"
#include "GameInstance.h"

CQuadTree::CQuadTree()
{
}

void CQuadTree::Set_CullingRadius(_float fRadius)
{
	m_fCullingRadius = fRadius;

	if (nullptr == m_pChildren[CORNER_LT])
		return;

	for (_uint i = 0; i < CORNER_END; i++)
	{
		m_pChildren[i]->Set_CullingRadius(fRadius);
	}
}

void CQuadTree::Culling(CGameInstance* pGameInstance, const _float3* pVertexPositions, _uint* pIndices, _uint* pNumIndices)
{
	/* 해당 노드에서 그릴건지 확인 */
	if (nullptr == m_pChildren[CORNER_LT] ||
		true == isDraw(pGameInstance, pVertexPositions))
	{
		_bool		isDraw[NEIGHBOR_END] = { true, true, true, true };

		/* 이웃노드가 그려지는지 확인 */
		for (size_t i = 0; i < NEIGHBOR_END; i++)
		{
			if (nullptr != m_pNeighbors[i])
				isDraw[i] = m_pNeighbors[i]->isDraw(pGameInstance, pVertexPositions);

		}

		/* 점들이 절두체 안에 있는지 확인 */
		_bool		isIn[4] = {
			pGameInstance->isIn_LocalFrustum(XMLoadFloat3(&pVertexPositions[m_iCorners[0]]), 0.f),
			pGameInstance->isIn_LocalFrustum(XMLoadFloat3(&pVertexPositions[m_iCorners[1]]), 0.f),
			pGameInstance->isIn_LocalFrustum(XMLoadFloat3(&pVertexPositions[m_iCorners[2]]), 0.f),
			pGameInstance->isIn_LocalFrustum(XMLoadFloat3(&pVertexPositions[m_iCorners[3]]), 0.f),
		};

		/* 이웃하는 노드가 모두 그려진다면 */
		if (true == isDraw[NEIGHBOR_LEFT] &&
			true == isDraw[NEIGHBOR_TOP] &&
			true == isDraw[NEIGHBOR_RIGHT] &&
			true == isDraw[NEIGHBOR_BOTTOM])
		{
			/* 삼각형의 한점이라도 절두체 안에 있다면 그린다(인덱스를 넣어주고 NumIndices++)*/

			if (true == isIn[0] ||
				true == isIn[1] ||
				true == isIn[2])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[0];
				pIndices[(*pNumIndices)++] = m_iCorners[1];
				pIndices[(*pNumIndices)++] = m_iCorners[2];
			}

			if (true == isIn[0] ||
				true == isIn[2] ||
				true == isIn[3])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[0];
				pIndices[(*pNumIndices)++] = m_iCorners[2];
				pIndices[(*pNumIndices)++] = m_iCorners[3];
			}

			return;
		}

		/* 이웃 노드와 크기 차이가 있을 때 */
		_uint		iLC, iTC, iRC, iBC;

		iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
		iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
		iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
		iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;


		/* 왼쪽 아래 확인 */
		if (true == isIn[0] ||
			true == isIn[2] ||
			true == isIn[3])
		{
			/* 왼쪽 이웃 확인 (같은 크기로 그려지는지)*/
			if (false == isDraw[NEIGHBOR_LEFT])
			{
				/* 아닐때 삼각형 분할 */
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iLC;

				pIndices[(*pNumIndices)++] = iLC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}
			else
			{
				/* 같은 크기면 인접한 큰 삼각형으로(사각형의 1/4 )*/
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
			}

			if (false == isDraw[NEIGHBOR_BOTTOM])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iBC;

				pIndices[(*pNumIndices)++] = iBC;
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LB];
				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
		}

		/* 오른쪽 위 확인*/
		if (true == isIn[0] ||
			true == isIn[1] ||
			true == isIn[2])
		{
			if (false == isDraw[NEIGHBOR_TOP])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iTC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_LT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}

			if (false == isDraw[NEIGHBOR_RIGHT])
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCenter;

				pIndices[(*pNumIndices)++] = m_iCenter;
				pIndices[(*pNumIndices)++] = iRC;
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
			}
			else
			{
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RT];
				pIndices[(*pNumIndices)++] = m_iCorners[CORNER_RB];
				pIndices[(*pNumIndices)++] = m_iCenter;
			}
		}

		return;
	}

	/* 현재 사각형이 절두체 안에 있다면 자식 호출 */
	/* Range 는 사각형의 중심에서 꼭짓점까지의 거리 */
	_float		fRange = XMVector3Length(XMLoadFloat3(&pVertexPositions[m_iCorners[CORNER_LT]]) - XMLoadFloat3(&pVertexPositions[m_iCenter])).m128_f32[0];

	if (true == pGameInstance->isIn_LocalFrustum(XMLoadFloat3(&pVertexPositions[m_iCenter]), fRange * 1.5f))
	{
		for (size_t i = 0; i < CORNER_END; i++)
		{
			if (nullptr != m_pChildren[i])
				m_pChildren[i]->Culling(pGameInstance, pVertexPositions, pIndices, pNumIndices);
		}
	}

}

void CQuadTree::SetUp_Neighbors()
{
	if (nullptr == m_pChildren[CORNER_LT]->m_pChildren[CORNER_LT])
		return;

	/* 해당 위치의 child의 neighbor에 포인터를 저장*/
	m_pChildren[CORNER_LT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChildren[CORNER_RT];
	m_pChildren[CORNER_LT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChildren[CORNER_LB];

	m_pChildren[CORNER_RT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChildren[CORNER_LT];
	m_pChildren[CORNER_RT]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pChildren[CORNER_RB];

	m_pChildren[CORNER_RB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pChildren[CORNER_LB];
	m_pChildren[CORNER_RB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChildren[CORNER_RT];

	m_pChildren[CORNER_LB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pChildren[CORNER_RB];
	m_pChildren[CORNER_LB]->m_pNeighbors[NEIGHBOR_TOP] = m_pChildren[CORNER_LT];

	/* 다른 부모의 자식인 인접한 이웃을 설정 */
	if (nullptr != m_pNeighbors[NEIGHBOR_LEFT])
	{
		m_pChildren[CORNER_LT]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChildren[CORNER_RT];
		m_pChildren[CORNER_LB]->m_pNeighbors[NEIGHBOR_LEFT] = m_pNeighbors[NEIGHBOR_LEFT]->m_pChildren[CORNER_RB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_TOP])
	{
		m_pChildren[CORNER_LT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChildren[CORNER_LB];
		m_pChildren[CORNER_RT]->m_pNeighbors[NEIGHBOR_TOP] = m_pNeighbors[NEIGHBOR_TOP]->m_pChildren[CORNER_RB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_RIGHT])
	{
		m_pChildren[CORNER_RT]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChildren[CORNER_LT];
		m_pChildren[CORNER_RB]->m_pNeighbors[NEIGHBOR_RIGHT] = m_pNeighbors[NEIGHBOR_RIGHT]->m_pChildren[CORNER_LB];
	}

	if (nullptr != m_pNeighbors[NEIGHBOR_BOTTOM])
	{
		m_pChildren[CORNER_LB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChildren[CORNER_LT];
		m_pChildren[CORNER_RB]->m_pNeighbors[NEIGHBOR_BOTTOM] = m_pNeighbors[NEIGHBOR_BOTTOM]->m_pChildren[CORNER_RT];
	}

	for (size_t i = 0; i < CORNER_END; i++)
	{
		m_pChildren[i]->SetUp_Neighbors();
	}
}

_bool CQuadTree::Picking(CGameInstance* pGameInstance, const _float3* pVertexPositions, _float3& Out, _fmatrix WorldMatrix)
{
	_bool isIn = { false };

	if (true == pGameInstance->MousePicking_InLocalSpace(pVertexPositions[m_iCorners[CORNER_LT]],pVertexPositions[m_iCorners[CORNER_RT]],pVertexPositions[m_iCorners[CORNER_RB]], Out))
	{
		if (nullptr == m_pChildren[CORNER_LT])
		{
			XMStoreFloat3(&Out, XMVector3TransformCoord(XMLoadFloat3(&Out), WorldMatrix));
			return true;
		}
		isIn = true;
	}

	if (true == pGameInstance->MousePicking_InLocalSpace(pVertexPositions[m_iCorners[CORNER_LT]], pVertexPositions[m_iCorners[CORNER_RB]], pVertexPositions[m_iCorners[CORNER_LB]], Out))
	{
		if (nullptr == m_pChildren[CORNER_LT])
		{
			XMStoreFloat3(&Out, XMVector3TransformCoord(XMLoadFloat3(&Out), WorldMatrix));
			return true;
		}
		isIn = true;
	}

	if (isIn)
	{
		for (_uint i = 0; i < CORNER_END; ++i)
		{
			if (m_pChildren[i]->Picking(pGameInstance, pVertexPositions, Out, WorldMatrix))
				return true;
		}
	}

	return false;
}

_bool CQuadTree::Set_Y(CGameInstance* pGameInstance, const _float3* pVertexPositions, VTXNORTEX* pVertices, _fmatrix WorldMatrix, _float fY)
{
	_bool isIn = { false };
	_float3 Out = {};
	if (true == pGameInstance->MousePicking_InLocalSpace(pVertexPositions[m_iCorners[CORNER_LT]], pVertexPositions[m_iCorners[CORNER_RT]], pVertexPositions[m_iCorners[CORNER_RB]], Out))
	{
		if (nullptr == m_pChildren[CORNER_LT])
		{
			pVertices[m_iCorners[CORNER_LT]].vPosition.y += fY;
			pVertices[m_iCorners[CORNER_RT]].vPosition.y += fY;
			pVertices[m_iCorners[CORNER_RB]].vPosition.y += fY;

			XMStoreFloat3(&Out, XMVector3TransformCoord(XMLoadFloat3(&Out), WorldMatrix));
			return true;
		}
		isIn = true;
	}

	if (true == pGameInstance->MousePicking_InLocalSpace(pVertexPositions[m_iCorners[CORNER_LT]], pVertexPositions[m_iCorners[CORNER_RB]], pVertexPositions[m_iCorners[CORNER_LB]], Out))
	{
		if (nullptr == m_pChildren[CORNER_LT])
		{
			pVertices[m_iCorners[CORNER_LT]].vPosition.y += fY;
			pVertices[m_iCorners[CORNER_RB]].vPosition.y += fY;
			pVertices[m_iCorners[CORNER_LB]].vPosition.y += fY;

			XMStoreFloat3(&Out, XMVector3TransformCoord(XMLoadFloat3(&Out), WorldMatrix));
			return true;
		}
		isIn = true;
	}

	if (isIn)
	{
		for (_uint i = 0; i < CORNER_END; ++i)
		{
			if (m_pChildren[i]->Set_Y(pGameInstance, pVertexPositions, pVertices, WorldMatrix, fY))
				return true;
		}
	}

	return false;
}

_bool CQuadTree::isDraw(CGameInstance* pGameInstance, const _float3* pVertexPositions)
{
	_vector		vCamPos = XMLoadFloat4(pGameInstance->Get_CamPosition());
	_vector		vCenter = XMLoadFloat3(&pVertexPositions[m_iCenter]);

	if (m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT] < XMVectorGetX(XMVector3Length(vCamPos - vCenter)) * 0.04f)
		return true;

	return false;
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorners[CORNER_LT] = iLT;
	m_iCorners[CORNER_RT] = iRT;
	m_iCorners[CORNER_RB] = iRB;
	m_iCorners[CORNER_LB] = iLB;

	/* 길이가 1일때 트리 최하위 */
	if (1 == m_iCorners[CORNER_RT] - m_iCorners[CORNER_LT])
		return S_OK;

	/* 중간 점들 인덱스 구하기 */
	m_iCenter = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RB]) >> 1;

	_uint		iLC, iTC, iRC, iBC;

	iLC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_LB]) >> 1;
	iTC = (m_iCorners[CORNER_LT] + m_iCorners[CORNER_RT]) >> 1;
	iRC = (m_iCorners[CORNER_RT] + m_iCorners[CORNER_RB]) >> 1;
	iBC = (m_iCorners[CORNER_LB] + m_iCorners[CORNER_RB]) >> 1;

	/* 점들을 이용해서 하위 트리를 만든다 */
	m_pChildren[CORNER_LT] = CQuadTree::Create(m_iCorners[CORNER_LT], iTC, m_iCenter, iLC);
	m_pChildren[CORNER_RT] = CQuadTree::Create(iTC, m_iCorners[CORNER_RT], iRC, m_iCenter);
	m_pChildren[CORNER_RB] = CQuadTree::Create(m_iCenter, iRC, m_iCorners[CORNER_RB], iBC);
	m_pChildren[CORNER_LB] = CQuadTree::Create(iLC, m_iCenter, iBC, m_iCorners[CORNER_LB]);

	return S_OK;
}

CQuadTree* CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree* pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX("Failed to Created : CQuadTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuadTree::Free()
{
	__super::Free();

	for (auto& pChild : m_pChildren)
		Safe_Release(pChild);

}
