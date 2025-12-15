#include "pch.h"
#include "Troll_State_BackHand_Swing.h"
#include "GameInstance.h"
#include "Unit.h"

CTroll_State_BackHand_Swing::CTroll_State_BackHand_Swing()
	:CState_Root()
{
}

void CTroll_State_BackHand_Swing::Enter()
{
	m_bPlayerHit = false;
	__super::Enter();
}

HRESULT CTroll_State_BackHand_Swing::Update(_float fTimeDelta)
{
	if (nullptr != m_funcPriorityUpdate) {
		m_funcPriorityUpdate(fTimeDelta);
	}
	if (E_FAIL == (__super::Update(fTimeDelta))) {
		return E_FAIL;
	}
	if (nullptr != m_funcLateUpdate) {
		m_funcLateUpdate(fTimeDelta, m_bPlayerHit);
	}
	return S_OK;
}

void CTroll_State_BackHand_Swing::Exit()
{
	m_bPlayerHit = false;
	__super::Exit();
}

HRESULT CTroll_State_BackHand_Swing::Initialize(TROLL_STATE_BACK_HAND_SWING_DESC* pDesc)
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

CTroll_State_BackHand_Swing* CTroll_State_BackHand_Swing::Create(TROLL_STATE_BACK_HAND_SWING_DESC* pDesc)
{
	CTroll_State_BackHand_Swing* pInstance = new CTroll_State_BackHand_Swing;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CTroll_State_BackHand_Swing::Free()
{
	__super::Free();
}

void CTroll_State_BackHand_Swing::Describe_Entity()
{
}
