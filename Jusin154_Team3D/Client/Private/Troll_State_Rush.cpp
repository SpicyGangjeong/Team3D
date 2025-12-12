#include "pch.h"
#include "Troll_State_Rush.h"
#include "GameInstance.h"
#include "Unit.h"

CTroll_State_Rush::CTroll_State_Rush()
	:CState_Root()
{
}

void CTroll_State_Rush::Enter()
{
	*m_pRushPlayerHit = false;
	__super::Enter();
}

HRESULT CTroll_State_Rush::Update(_float fTimeDelta)
{
	if (E_FAIL == (__super::Update(fTimeDelta))) {
		return E_FAIL;
	}
	return S_OK;
}

void CTroll_State_Rush::Exit()
{
	*m_pRushPlayerHit = false;
	__super::Exit();
}

HRESULT CTroll_State_Rush::Initialize(TROLL_STATE_RUSH_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc))) {
		return E_FAIL;
	}
	m_pRushPlayerHit = pDesc->pCollisionPlayer;
	m_pModel = m_pOwner->Get_Component<CModel>();
	m_pFSM = m_pOwner->Get_Component<CFSM>();

	return S_OK;
}

CTroll_State_Rush* CTroll_State_Rush::Create(TROLL_STATE_RUSH_DESC* pDesc)
{
	CTroll_State_Rush* pInstance = new CTroll_State_Rush;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CTroll_State_Rush::Free()
{
	__super::Free();
}

void CTroll_State_Rush::Describe_Entity()
{
}
