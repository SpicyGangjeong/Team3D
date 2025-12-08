#include "pch.h"
#include "MapContainer.h"
#include "PartObject.h"
#include "MapObject_Collision.h"
#include "MapObject_Render.h"

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
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

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

HRESULT CMapContainer::Render_Shadow()
{
	unordered_map<_string, CPartObject*>::iterator iter =	m_PartObjects.begin();
	for (; iter != m_PartObjects.end(); ++iter) {
		CPartObject* pObject = (*iter).second;
		if (FAILED(pObject->Render_Shadow())) {
			return E_FAIL;
		}
	}
	return S_OK;
}

void CMapContainer::ReadyForPhysX()
{
	if (true == m_bHasCollisionMesh) {
		vector<class CPartObject*>::iterator iter = m_ColiisonPartObjects.begin();
		for (; iter != m_ColiisonPartObjects.end(); ++iter) {
			CMapObject_Collision* pObject = static_cast<CMapObject_Collision*>(*iter);
			pObject->ReadyForPhysX();
		}
	}
	else {
		unordered_map<_string, CPartObject*>::iterator iter = m_PartObjects.begin();
		for (; iter != m_PartObjects.end(); ++iter) {
			CMapObject_Render* pObject = static_cast<CMapObject_Render*>((*iter).second);
			pObject->ReadyForPhysX();
		}
	}
}

void CMapContainer::ConvertToPhysX()
{
	if (true == m_bHasCollisionMesh) {
		vector<class CPartObject*>::iterator iter = m_ColiisonPartObjects.begin();
		for (; iter != m_ColiisonPartObjects.end(); ++iter) {
			CMapObject_Collision* pObject = static_cast<CMapObject_Collision*>(*iter);
			pObject->ConvertToPhysX();
		}
	}
	else {
		unordered_map<_string, CPartObject*>::iterator iter = m_PartObjects.begin();
		for (; iter != m_PartObjects.end(); ++iter) {
			CMapObject_Render* pObject = static_cast<CMapObject_Render*>((*iter).second);
			pObject->ConvertToPhysX();
		}
	}
}

void CMapContainer::Free()
{
	__super::Free();
}

#ifdef _DEBUG

void CMapContainer::Describe_Entity()
{
}

#endif // _DEBUG
