#include "pch.h"
#include "OcclusionQuery.h"

COcclusionQuery::COcclusionQuery(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

COcclusionQuery::COcclusionQuery(const COcclusionQuery& rhs)
	:CComponent(rhs), m_iCheckFrame{0}
{
}

void COcclusionQuery::Begin_Query()
{
	m_pContext->Begin(m_pQuery[m_iCheckFrame]);
}

_bool COcclusionQuery::isDraw()
{
	if (false == m_bCurFrameDraw)
	{
		m_bPreFrameDraw = m_bCurFrameDraw = true;
		return true;
	}

	bool bVisible = m_bPreFrameDraw;

	_uint iIndex = (m_iCheckFrame + 3) % 5;
	
	UINT64 iSampleCount = 0;
	HRESULT hr = m_pContext->GetData(
		m_pQuery[iIndex],
		&iSampleCount,
		sizeof(UINT64),
		D3D11_ASYNC_GETDATA_DONOTFLUSH
	);

	if (hr == S_OK)
	{
		bVisible = (iSampleCount > 0);
			
	}
	else
		return m_bPreFrameDraw;

	m_bPreFrameDraw = bVisible;

	return bVisible;
}

void COcclusionQuery::End_Query()
{
	m_pContext->End(m_pQuery[m_iCheckFrame++]);

	if (5 <= m_iCheckFrame)
		m_iCheckFrame = 0;
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

	for (_uint i = 0; i < 5; i++)
	{
		if (FAILED(m_pDevice->CreateQuery(&queryDesc, &m_pQuery[i])))
		{
			MSG_BOX("Failed to Created : ID3D11Query");
			return E_FAIL;
		}
	}
	m_bCurFrameDraw = false;
	m_bPreFrameDraw = true;
	m_iNumCullFrame = 0;

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

	for(auto& pQuery : m_pQuery)
		SAFE_RELEASE(pQuery);
	
}

#ifdef _DEBUG
void COcclusionQuery::Describe_Entity()
{
}
#endif
