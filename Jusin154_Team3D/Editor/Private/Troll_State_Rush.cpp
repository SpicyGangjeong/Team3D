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
	__super::Enter();
}

HRESULT CTroll_State_Rush::Update(_float fTimeDelta)
{
	if (nullptr != m_funcPriorityUpdate) {
		m_funcPriorityUpdate(fTimeDelta);
	}
	if (E_FAIL == (__super::Update(fTimeDelta))) {
		return E_FAIL;
	}
	if (nullptr != m_funcLateUpdate) {
		m_funcLateUpdate(fTimeDelta);
	}
	return S_OK;
}

void CTroll_State_Rush::Exit()
{
	__super::Exit();
}

HRESULT CTroll_State_Rush::Initialize(TROLL_STATE_RUSH_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc))) {
		return E_FAIL;
	}
	m_funcPriorityUpdate = pDesc->funcPriorityUpdate;
	m_funcLateUpdate = pDesc->funcLateUpdate;
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
