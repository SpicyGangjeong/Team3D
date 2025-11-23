#include "pch.h"
#include "State_Dodge.h"
#include "Unit.h"


CState_Dodge::CState_Dodge()
	:CState_Root()
{
}

void CState_Dodge::Enter()
{
	__super::Enter();
}

HRESULT CState_Dodge::Update(_float fTimeDelta)
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

void CState_Dodge::Exit()
{
	__super::Exit();
}

HRESULT CState_Dodge::Initialize(STATE_DODGE_DESC* pDesc)
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

CState_Dodge* CState_Dodge::Create(STATE_DODGE_DESC* pDesc)
{
	CState_Dodge* pInstance = new CState_Dodge;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_Dodge::Free()
{
	__super::Free();
}

void CState_Dodge::Describe_Entity()
{
}
