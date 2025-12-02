#include "pch.h"
#include "MapPartObject.h"

#include "GameInstance.h"
#include "Camera_Debug.h"
#include "Layer.h"
#include "VIBuffer_Terrain.h"

CMapPartObject::CMapPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CMapPartObject::CMapPartObject(const CMapPartObject& Prototype)
	: CPartObject(Prototype)
{
}

void CMapPartObject::Priority_Update(_float fTimeDelta)
{
}

void CMapPartObject::Update(_float fTimeDelta)
{

}

void CMapPartObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CMapPartObject::Render()
{
	return S_OK;
}

HRESULT CMapPartObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapPartObject::Initialize(void* pArg)
{
	MAPOBJECT_DESC* pDesc = static_cast<MAPOBJECT_DESC*>(pArg);

	PARTOBJECT_DESC Desc = {};
	Desc.pParentTransform = pDesc->pParentTransform;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapPartObject::Ready_Components()
{
	__super::Ready_Components(nullptr);

	return S_OK;
}

HRESULT CMapPartObject::Bind_ShaderResources()
{
	return S_OK;
}

void CMapPartObject::Free()
{
	__super::Free();
}

#ifdef _DEBUG
void CMapPartObject::Describe_Entity()
{
}
#endif // _DEBUG
