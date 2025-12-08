#include "pch.h"
#include "State_Hit.h"
#include "Unit.h"


CState_Hit::CState_Hit()
	:CState_Root()
{
}

void CState_Hit::Enter()
{
	__super::Enter();
}

HRESULT CState_Hit::Update(_float fTimeDelta)
{
	if (nullptr != m_funcPriorityUpdate) {
		m_funcPriorityUpdate(fTimeDelta);
	}
	if (FAILED(__super::Update(fTimeDelta))) {
		return E_FAIL;
	}
	if (nullptr != m_funcLateUpdate) {
		m_funcLateUpdate(fTimeDelta);
	}
	return S_OK;
}

void CState_Hit::Exit()
{
	__super::Exit();
}

HRESULT CState_Hit::Initialize(STATE_HIT_DESC* pDesc)
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

CState_Hit* CState_Hit::Create(STATE_HIT_DESC* pDesc)
{
	CState_Hit* pInstance = new CState_Hit;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_Hit::Free()
{
	__super::Free();
}

void CState_Hit::Describe_Entity()
{
}
