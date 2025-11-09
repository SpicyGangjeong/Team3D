#include "pch.h"
#include "UIComponent.h"

CUIComponent ::CUIComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CUIComponent::CUIComponent(const CUIComponent& rhs)
	: CComponent(rhs)
{
}

HRESULT CUIComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIComponent::Initialize(void* pArg)
{
	return S_OK;
}

CUIComponent* CUIComponent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIComponent* pInstance = new CUIComponent(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIComponent");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CUIComponent::Clone(void* pArg, class CGameObject* pOwner)
{
	CUIComponent* pInstance = new CUIComponent(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIComponent");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CUIComponent::Free()
{
	__super::Free();
}

void CUIComponent::Describe_Entity()
{
}
