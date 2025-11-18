#include "pch.h"
#include "State_Sprint.h"
#include "Unit.h"

CState_Sprint::CState_Sprint()
	:CState()
{
}

void CState_Sprint::Enter()
{
	auto anim = m_pOwner->Get_AnimInfo(STATEANIM::SPRINT);
	m_pModel->Set_AnimationIndex(anim.first, anim.second);
}

void CState_Sprint::Update(_float fTimeDelta)
{
	if (CheckExitState())
		return;

}

void CState_Sprint::Exit()
{
}

_bool CState_Sprint::CheckExitState()
{
	if (!m_pOwner->Check(FSMSTATE::SPRINT))
	{
		m_pOwner->Reset_Sprint();
		m_pFSM->Change_State(FSMSTATE::WALK);
	}

	else if (m_pOwner->Check(FSMSTATE::DODGE))
		m_pFSM->Change_State(FSMSTATE::DODGE);

	if (m_pOwner->Check(FSMSTATE::JUMP))
		m_pFSM->Change_State(FSMSTATE::JUMP);

	return false;
}

void CState_Sprint::Free()
{
	__super::Free();
}

void CState_Sprint::Describe_Entity()
{
}
