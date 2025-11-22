#include "pch.h"
#include "State_Player_Idle.h"
#include "Unit.h"

CState_Player_Idle::CState_Player_Idle()
	:CState_Player()
{
}

void CState_Player_Idle::Enter()
{
	__super::Enter();
}

HRESULT CState_Player_Idle::Update(_float fTimeDelta)
{
	if (E_FAIL == (__super::Update(fTimeDelta))) {
		return E_FAIL;
	}
	return S_OK;
}

void CState_Player_Idle::Exit()
{
	__super::Exit();
}

HRESULT CState_Player_Idle::Initialize(STATE_PLAYER_IDLE_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc))) {
		return E_FAIL;
	}
	m_pModel = m_pOwner->Get_Component<CModel>();
	m_pFSM = m_pOwner->Get_Component<CFSM>();

	return S_OK;
}

CState_Player_Idle* CState_Player_Idle::Create(STATE_PLAYER_IDLE_DESC* pDesc)
{
	CState_Player_Idle* pInstance = new CState_Player_Idle;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_Player_Idle::Free()
{
	__super::Free();
}

void CState_Player_Idle::Describe_Entity()
{
}
