#include "pch.h"
#include "MapElement.h"

#include "GameInstance.h"

CMapElement::CMapElement(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMapElement::CMapElement(const CMapElement& rhs)
	: CGameObject(rhs)
{
}

void CMapElement::Priority_Update(_float fTimeDelta)
{
}

void CMapElement::Update(_float fTimeDelta)
{
}

void CMapElement::Late_Update(_float fTimeDelta)
{
}

HRESULT CMapElement::Render()
{
	return S_OK;
}

HRESULT CMapElement::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapElement::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapElement::Bind_ShaderResources()
{
	return S_OK;
}

void CMapElement::Free()
{
	__super::Free();
}

void CMapElement::Describe_Entity()
{
}
