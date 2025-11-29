#include "pch.h"
#include "OcclusionQuery.h"

COcclusionQuery::COcclusionQuery(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

COcclusionQuery::COcclusionQuery(const COcclusionQuery& rhs)
	:CComponent(rhs)
{
}

void COcclusionQuery::Begin_Query()
{
	m_pContext->Begin(m_pQuery);
}

_bool COcclusionQuery::isDraw()
{
	UINT64 iNumSample;
	
	if (m_pContext->GetData(m_pQuery, &iNumSample, sizeof(UINT64), 0) != S_OK)
		return m_bPreFrameDraw;

	0 < iNumSample ? m_bThisFrameDraw = true : m_bThisFrameDraw = false;

	_bool bDraw = { false };
	
	if(true == m_bThisFrameDraw)
		m_NumCullFrame = 0;
	else
	{
		if(m_NumCullFrame < 5)
			++m_NumCullFrame;
	}

	m_bPreFrameDraw = m_bThisFrameDraw;

	return m_NumCullFrame < 5;
}

void COcclusionQuery::End_Query()
{
	m_pContext->End(m_pQuery);
}

HRESULT COcclusionQuery::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COcclusionQuery::Initialize(void* pArg)
{
	D3D11_QUERY_DESC queryDesc = {};

	queryDesc.Query = D3D11_QUERY_OCCLUSION;
	queryDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateQuery(&queryDesc, &m_pQuery)))
	{
		MSG_BOX("Failed to Created : ID3D11Query");
		return E_FAIL;
	}

	m_bPreFrameDraw = m_bThisFrameDraw = true;
	m_NumCullFrame = 0;

	return S_OK;
}

COcclusionQuery* COcclusionQuery::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	COcclusionQuery* pInstance = new COcclusionQuery(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : COcclusionQuery");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* COcclusionQuery::Clone(void* pArg, CGameObject* pOwner)
{
	COcclusionQuery* pInstance = new COcclusionQuery(*this);
	pInstance->m_pOwner = pOwner;

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : COcclusionQuery");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void COcclusionQuery::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pQuery);
}

#ifdef _DEBUG
void COcclusionQuery::Describe_Entity()
{
}
#endif
