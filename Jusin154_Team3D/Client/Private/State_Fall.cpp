#include "pch.h"
#include "State_Fall.h"
#include "Unit.h"


CState_Fall::CState_Fall()
	:CState_Root()
{
}

void CState_Fall::Enter()
{
	__super::Enter();
}

HRESULT CState_Fall::Update(_float fTimeDelta)
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

void CState_Fall::Exit()
{
	__super::Exit();
}

HRESULT CState_Fall::Initialize(STATE_FALL_DESC* pDesc)
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

CState_Fall* CState_Fall::Create(STATE_FALL_DESC* pDesc)
{
	CState_Fall* pInstance = new CState_Fall;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_Fall::Free()
{
	__super::Free();
}

void CState_Fall::Describe_Entity()
{
}
