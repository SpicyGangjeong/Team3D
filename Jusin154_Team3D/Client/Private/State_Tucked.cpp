#include "pch.h"
#include "State_Tucked.h"
#include "GameInstance.h"
#include "Unit.h"

CState_Tucked::CState_Tucked()
	:CState_Root()
{
}

void CState_Tucked::Enter()
{
	__super::Enter();
}

HRESULT CState_Tucked::Update(_float fTimeDelta)
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

void CState_Tucked::Exit()
{
	__super::Exit();
}

HRESULT CState_Tucked::Initialize(STATE_TUCKED_DESC* pDesc)
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

CState_Tucked* CState_Tucked::Create(STATE_TUCKED_DESC* pDesc)
{
	CState_Tucked* pInstance = new CState_Tucked;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_Tucked::Free()
{
	__super::Free();
}

void CState_Tucked::Describe_Entity()
{
}
