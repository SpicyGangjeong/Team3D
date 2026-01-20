#include "pch.h"
#include "State_Fear.h"
#include "Unit.h"


CState_Fear::CState_Fear()
	:CState_Root()
{
}

void CState_Fear::Enter()
{
	__super::Enter();
}

HRESULT CState_Fear::Update(_float fTimeDelta)
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

void CState_Fear::Exit()
{
	__super::Exit();
}

HRESULT CState_Fear::Initialize(STATE_FEAR_DESC* pDesc)
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

CState_Fear* CState_Fear::Create(STATE_FEAR_DESC* pDesc)
{
	CState_Fear* pInstance = new CState_Fear;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_Fear::Free()
{
	__super::Free();
}

void CState_Fear::Describe_Entity()
{
}
