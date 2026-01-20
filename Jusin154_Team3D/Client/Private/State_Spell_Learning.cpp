#include "pch.h"
#include "State_Spell_Learning.h"
#include "Unit.h"


CState_Spell_Learning::CState_Spell_Learning()
	:CState_Root()
{
}

void CState_Spell_Learning::Enter()
{
	__super::Enter();
}

HRESULT CState_Spell_Learning::Update(_float fTimeDelta)
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

void CState_Spell_Learning::Exit()
{
	__super::Exit();
}

HRESULT CState_Spell_Learning::Initialize(STATE_SPELL_LEARNING_DESC* pDesc)
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

CState_Spell_Learning* CState_Spell_Learning::Create(STATE_SPELL_LEARNING_DESC* pDesc)
{
	CState_Spell_Learning* pInstance = new CState_Spell_Learning;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_Spell_Learning::Free()
{
	__super::Free();
}

void CState_Spell_Learning::Describe_Entity()
{
}
