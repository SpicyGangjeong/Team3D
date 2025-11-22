#include "pch.h"
#include "State_Player_Idle.h"
#include "Unit.h"

CState_Idle::CState_Idle()
	:CState_Root()
{
}

void CState_Idle::Enter()
{
	__super::Enter();
}

HRESULT CState_Idle::Update(_float fTimeDelta)
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

void CState_Idle::Exit()
{
	__super::Exit();
}

HRESULT CState_Idle::Initialize(STATE_IDLE_DESC* pDesc)
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

CState_Idle* CState_Idle::Create(STATE_IDLE_DESC* pDesc)
{
	CState_Idle* pInstance = new CState_Idle;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_Idle::Free()
{
	__super::Free();
}

void CState_Idle::Describe_Entity()
{
}
