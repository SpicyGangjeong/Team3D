#include "pch.h"
#include "State_Jump.h"
#include "Unit.h"


CState_Jump::CState_Jump()
	:CState()
{
}

void CState_Jump::Enter()
{
	auto anim = m_pOwner->Get_AnimInfo(FSMSTATE::JUMP);
	m_pModel->Set_AnimationIndex(anim.first, anim.second);
}

void CState_Jump::Update(_float fTimeDelta)
{
	if (CheckExitState())
		return;
}

void CState_Jump::Exit()
{
}

_bool CState_Jump::CheckExitState()
{
	if (m_pModel->IsFinishedAnim())
		m_pFSM->Change_State(FSMSTATE::IDLE);

	return false;
}

void CState_Jump::Free()
{
	__super::Free();
}

void CState_Jump::Describe_Entity()
{
}
