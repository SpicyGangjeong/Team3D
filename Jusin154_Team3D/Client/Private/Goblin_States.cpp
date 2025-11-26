#include "pch.h"
#include "Goblin.h"

#include "GameInstance.h"
#include "Player.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Dodge.h"
#include "State_Jump.h"
#include "State_Land.h"
#include "State_Move.h"
#include "State_Combat.h"
#pragma endregion


void CGoblin::Behavior_IdleEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLE);
	pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::IDLE];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CGoblin::Behavior_IdleExitCheck()
{
	if (m_fTargetDistance <= 15.f)
		m_pFSM->Change_State(FSMSTATE::MOVE);

	return E_FAIL;
}

void CGoblin::Behavior_IdleExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLE);
}

void CGoblin::Behavior_MoveEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};

	m_pFSM->Enable_State(FSMSTATE::MOVE);
	m_pFSM->Enable_State(FSMSTATE::JOG);

	pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CGoblin::Behavior_MoveExitCheck()
{
	if (m_fTargetDistance <= 7.f &&m_fTargetDistance !=0.f)
		m_pFSM->Change_State(FSMSTATE::COMBAT);

	return E_FAIL;
}

void CGoblin::Behavior_MoveExit()
{
	m_pFSM->Disable_State(FSMSTATE::MOVE | FSMSTATE::SPRINT | FSMSTATE::JOG | FSMSTATE::WALK);
}

void CGoblin::Behavior_CombatEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};

	m_pFSM->Enable_State(FSMSTATE::COMBAT);

	m_pFSM->Enable_State(FSMSTATE::SKILL2);

	pairAnimInfo = m_Animation[STATEANIM::SKILL2];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CGoblin::Behavior_CombatExitCheck()
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::IDLE);
	}
	return E_FAIL;
}

void CGoblin::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT | FSMSTATE::SKILL | FSMSTATE::SKILL2);
}


void CGoblin::Add_FSM()
{
#pragma region Behavior_Movement_NotFocus
	{
		CState_Idle::STATE_IDLE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_IdleEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_IdleExitCheck(); };
		Desc.funcExitEvent = [this]() { Behavior_IdleExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::IDLE, CState_Idle::Create(&Desc));
	}
	{
		CState_Move::STATE_MOVE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_MoveEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_MoveExitCheck(); };
		Desc.funcExitEvent = [this]() { Behavior_MoveExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			if (m_pGameInstance->Key_Pressing(DIK_A)) { m_pTransformCom->Turn(-m_pTransformCom->Get_State(STATE::UP), fTimeDelta); }
			if (m_pGameInstance->Key_Pressing(DIK_D)) { m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta); };
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::MOVE, CState_Move::Create(&Desc));
	}
#pragma endregion

#pragma region Behavior_Combat_NotFocus
	{
		CState_Combat::STATE_COMBAT_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_CombatEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_CombatExitCheck(); };
		Desc.funcExitEvent = [this]() { Behavior_CombatExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::COMBAT, CState_Combat::Create(&Desc));
	}
#pragma endregion
#pragma region Behavior_Combat_Focus

#pragma endregion

}

void CGoblin::Set_Anim()
{

	m_Animation[STATEANIM::IDLE] = { 22,true };

	m_Animation[STATEANIM::WALK_FWD] = { 50,true };
	m_Animation[STATEANIM::WALK_STOP] = { 58,false };

	m_Animation[STATEANIM::JOG_FWD] = { 46,true };
	m_Animation[STATEANIM::JOG_BWD] = { 154,false };

	m_Animation[STATEANIM::SKILL] = { 70,false }; // 도끼 스윙
	m_Animation[STATEANIM::SKILL2] = { 85,false }; // 나이프 던지기
}
