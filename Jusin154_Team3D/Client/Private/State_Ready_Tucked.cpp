#include "pch.h"
#include "State_Ready_Tucked.h"
#include "Unit.h"


CState_Ready_Tucked::CState_Ready_Tucked()
	:CState_Root()
{
}

void CState_Ready_Tucked::Enter()
{
	__super::Enter();
}

HRESULT CState_Ready_Tucked::Update(_float fTimeDelta)
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

void CState_Ready_Tucked::Exit()
{
	__super::Exit();
}

HRESULT CState_Ready_Tucked::Initialize(STATE_READY_TUCKED_DESC* pDesc)
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

CState_Ready_Tucked* CState_Ready_Tucked::Create(STATE_READY_TUCKED_DESC* pDesc)
{
	CState_Ready_Tucked* pInstance = new CState_Ready_Tucked;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_Ready_Tucked::Free()
{
	__super::Free();
}

void CState_Ready_Tucked::Describe_Entity()
{
}
