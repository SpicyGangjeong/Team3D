#include "pch.h"
#include "State_IdleBreak.h"
#include "GameInstance.h"
#include "Unit.h"

CState_IdleBreak::CState_IdleBreak()
	:CState_Root()
{
}

void CState_IdleBreak::Enter()
{
	__super::Enter();
}

HRESULT CState_IdleBreak::Update(_float fTimeDelta)
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

void CState_IdleBreak::Exit()
{
	__super::Exit();
}

HRESULT CState_IdleBreak::Initialize(STATE_IDLEBREAK_DESC* pDesc)
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

CState_IdleBreak* CState_IdleBreak::Create(STATE_IDLEBREAK_DESC* pDesc)
{
	CState_IdleBreak* pInstance = new CState_IdleBreak;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_IdleBreak::Free()
{
	__super::Free();
}

void CState_IdleBreak::Describe_Entity()
{
}
