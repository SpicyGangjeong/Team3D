#include "pch.h"

#include "Bounding.h"

CBounding::CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
}

HRESULT CBounding::Initialize()
{
	return S_OK;
}

void CBounding::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
