#include "pch.h"
#include "MapContainer.h"
#include "PartObject.h"

//#include "MapObject_LOD.h"

CMapContainer::CMapContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CContainerObject(pDevice, pContext)
{
}

CMapContainer::CMapContainer(const CMapContainer& rhs)
	:CContainerObject(rhs)
{
}

HRESULT CMapContainer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapContainer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapContainer::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

void CMapContainer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CMapContainer::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CMapContainer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CMapContainer::Render()
{
	return S_OK;
}

void CMapContainer::Free()
{
	__super::Free();
}
