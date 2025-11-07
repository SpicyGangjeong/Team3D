#include "pch.h"
#include "Layer.h"
#include "GameObject.h"
#include "GameInstance.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

void CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& iter : m_ObjectList)
	{
		if (iter == nullptr)
			return;

		iter->Priority_Update(fTimeDelta);
	}
}

void CLayer::Update(_float fTimeDelta)
{
	for (auto& iter : m_ObjectList)
	{
		if (iter == nullptr)
			return;
		iter->Update(fTimeDelta);
	}
}

void CLayer::Late_Update(_float fTimeDelta)
{
	for (auto& iter : m_ObjectList)
	{
		if (iter == nullptr)
			return;
		iter->Late_Update(fTimeDelta);
	}
}

HRESULT CLayer::Add_GameObject(CGameObject* _pGameObject)
{
	if (nullptr == _pGameObject)
		return E_FAIL;

	m_ObjectList.push_back(_pGameObject);

	return S_OK;
}
void CLayer::Clear_DeadObj()
{
	auto	iter_end = m_ObjectList.end();
	for (auto iter = m_ObjectList.begin(); iter != iter_end;)
	{
		if (true == (*iter)->isDead())
		{
			SAFE_RELEASE(*iter);
			iter = m_ObjectList.erase(iter);
		}
		else {
			++iter;
		}
	}
}
HRESULT CLayer::Clear_Layer()
{
	for (auto& iter : m_ObjectList)
	{
		SAFE_RELEASE(iter);
	}

	m_ObjectList.clear();

	return S_OK;
}
void CLayer::Free()
{
	__super::Free();

	for (auto& iter : m_ObjectList)
	{
		SAFE_RELEASE(iter);
	}

	m_ObjectList.clear();
}

CLayer* CLayer::Create()
{
	CLayer* pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLayer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
