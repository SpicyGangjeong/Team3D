#include "pch.h"
#include "Layer.h"
#include "GameObject.h"
#include "GameInstance.h"

CLayer::CLayer():
	m_pGameInstance(CGameInstance::GetInstance())
{
	SAFE_ADDREF(m_pGameInstance);
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
		iter->Set_Dead();
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
		iter->Set_Dead();
		SAFE_RELEASE(iter);
	} m_ObjectList.clear();

	SAFE_RELEASE(m_pGameInstance);
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

#ifdef _DEBUG

void CLayer::Describe_Entity()
{
	_int iIndex = 0;

	for (CGameObject*& iter : m_ObjectList)
	{
		GUI::PushID(iter);
		if (GUI::SmallButton((to_string(iIndex) + " : " + typeid(*iter).name()).c_str())) {
			m_pGameInstance->Force_CamPosition(iter->Get_WorldPostion() - XMVectorSet(0.f, -1.f, 1.f, 0.f) * 10.f);
		} iIndex++;
		GUI::PopID();
	}
}

#endif // _DEBUG
