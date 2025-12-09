#include "pch.h"
#include "State_Shuffle.h"
#include "GameInstance.h"
#include "Unit.h"

CState_Shuffle::CState_Shuffle()
	:CState_Root()
{
}

void CState_Shuffle::Enter()
{
	__super::Enter();
}

HRESULT CState_Shuffle::Update(_float fTimeDelta)
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

void CState_Shuffle::Exit()
{
	__super::Exit();
}

HRESULT CState_Shuffle::Initialize(STATE_SHUFFLE_DESC* pDesc)
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

CState_Shuffle* CState_Shuffle::Create(STATE_SHUFFLE_DESC* pDesc)
{
	CState_Shuffle* pInstance = new CState_Shuffle;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_Shuffle::Free()
{
	__super::Free();
}

void CState_Shuffle::Describe_Entity()
{
}
