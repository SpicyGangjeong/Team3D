#include "pch.h"
#include "State_Walk.h"
#include "Unit.h"

CState_Walk::CState_Walk()
	:CState()
{
}

void CState_Walk::Enter()
{
	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		auto anim = m_pOwner->Get_AnimInfo(FSMSTATE::WALK_FWD);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);

	}

	if (m_pGameInstance->Key_Down(DIK_DOWN))
	{
		auto anim = m_pOwner->Get_AnimInfo(FSMSTATE::WALK_BWD);
		m_pModel->Set_AnimationIndex(anim.first, anim.second);
	}
}

void CState_Walk::Update(_float fTimeDelta)
{
	if (CheckExitState())
		return;

	auto Transform = m_pOwner->Get_Component<CTransform>();

	if (m_pGameInstance->Key_Pressing(DIK_LEFT))
		Transform->Turn(-Transform->Get_State(STATE::UP), fTimeDelta);

	if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
		Transform->Turn(Transform->Get_State(STATE::UP), fTimeDelta);
}

void CState_Walk::Exit()
{
}

_bool CState_Walk::CheckExitState()
{
	if (m_pOwner->IsSprint())
		m_pFSM->Change_State(FSMSTATE::SPRINT);

	else if (m_pOwner->IsDodge())
		m_pFSM->Change_State(FSMSTATE::DODGE);

	else if (!m_pOwner->IsWalking())
		m_pFSM->Change_State(FSMSTATE::IDLE);

	return false;
}

void CState_Walk::Free()
{
	__super::Free();
}

void CState_Walk::Describe_Entity()
{
}
