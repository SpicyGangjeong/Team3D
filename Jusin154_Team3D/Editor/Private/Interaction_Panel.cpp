#include "pch.h"
#include "Interaction_Panel.h"

CInteraction_Panel::CInteraction_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CInteraction_Panel::CInteraction_Panel(const CInteraction_Panel& rhs)
	:CPanelObject(rhs)
{
}

void CInteraction_Panel::Priority_Update(_float fTimeDelta)
{
}

void CInteraction_Panel::Update(_float fTimeDelta)
{
}

void CInteraction_Panel::Late_Update(_float fTimeDelta)
{
}

HRESULT CInteraction_Panel::Render()
{
	return E_NOTIMPL;
}

_vector CInteraction_Panel::Get_WorldPostion()
{
	return _vector();
}

HRESULT CInteraction_Panel::Bind_ShaderResources()
{
	return E_NOTIMPL;
}

HRESULT CInteraction_Panel::Ready_Components(void* pArg)
{
	return E_NOTIMPL;
}

HRESULT CInteraction_Panel::Ready_Element(void* pArg)
{
	return E_NOTIMPL;
}

HRESULT CInteraction_Panel::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CInteraction_Panel::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

CInteraction_Panel* CInteraction_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CInteraction_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	return nullptr;
}

void CInteraction_Panel::Free()
{
}

void CInteraction_Panel::Describe_Entity()
{
}
