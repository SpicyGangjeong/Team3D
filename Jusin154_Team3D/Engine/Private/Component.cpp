#include "pch.h"
#include "Component.h"
#include "GameInstance.h"

CComponent::CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
	, m_bCloned{ false }
{
	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
}

CComponent::CComponent(const CComponent& rhs)
	: m_pDevice{ rhs.m_pDevice }
	, m_pContext{ rhs.m_pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
	, m_bCloned{ true }
{
	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void* pArg)
{
	return S_OK;
}

void CComponent::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
