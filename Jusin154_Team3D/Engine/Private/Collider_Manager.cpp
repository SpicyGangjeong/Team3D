#include "pch.h"
#include "Collider_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Collider.h"
#include "Transform.h"


CCollider_Manager::CCollider_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
	SAFE_ADDREF(m_pGameInstance);
}

HRESULT CCollider_Manager::Initialize(_uint iMaxColliderGroup)
{
	m_iMaxColliderGroup = iMaxColliderGroup;
	m_pColliderObjects = new list<CCollider*>[m_iMaxColliderGroup];
	return S_OK;
}

void CCollider_Manager::Refresh_Collider_Manager()
{
	for (_uint i = 0; i < m_iMaxColliderGroup; ++i) {
		for (auto& pBoundingObject : m_pColliderObjects[i]) {
			SAFE_RELEASE(pBoundingObject);
		}
		m_pColliderObjects[i].clear();
	}
}

HRESULT CCollider_Manager::Add_ColliderGroup(_uint iRenderGroup, CCollider* pBounding)
{
	m_pColliderObjects[iRenderGroup].push_back(pBounding);
	SAFE_ADDREF(pBounding);
	return S_OK;
}

void CCollider_Manager::Collide(_uint iCollideGroupA, _uint iCollideGroupB)
{
	for (auto& pColliderA : m_pColliderObjects[iCollideGroupA]) {
		for (auto& pColliderB : m_pColliderObjects[iCollideGroupB]) {
			COLLIDER eTypeB = pColliderB->Get_ColliderType();
			if (pColliderA->Intersect(eTypeB, pColliderB)) {
				pColliderA->OnCollision(pColliderB->Get_Caster());
				pColliderB->OnCollision(pColliderA->Get_Caster());
			}
		}
	}
}

CCollider_Manager* CCollider_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iMaxColliderGroup)
{
	CCollider_Manager* pInstance = new CCollider_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iMaxColliderGroup)))
	{
		MSG_BOX("Failed to Created : CCollider_Manager");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CCollider_Manager::Free()
{
	__super::Free();

	Refresh_Collider_Manager();
	Safe_Delete_Array(m_pColliderObjects);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pGameInstance);
}
