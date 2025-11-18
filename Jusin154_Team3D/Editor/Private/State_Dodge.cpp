#include "pch.h"
#include "State_Dodge.h"
#include "Unit.h"


CState_Dodge::CState_Dodge()
	:CState()
{
}

void CState_Dodge::Enter()
{
	auto anim = m_pOwner->Get_AnimInfo(STATEANIM::DODGE);
	m_pModel->Set_AnimationIndex(anim.first, anim.second);
}

void CState_Dodge::Update(_float fTimeDelta)
{
	if (CheckExitState())
		return;
}

void CState_Dodge::Exit()
{
}

_bool CState_Dodge::CheckExitState()
{
	if (m_pModel->IsFinishedAnim())
		m_pFSM->Change_State(FSMSTATE::IDLE);

	if (m_pOwner->Check(FSMSTATE::WALK) && m_pOwner->IsCurrentKeyFrame("Dodge"))
		m_pFSM->Change_State(FSMSTATE::WALK);

	return false;
}

void CState_Dodge::Free()
{
	__super::Free();
}

void CState_Dodge::Describe_Entity()
{
}
