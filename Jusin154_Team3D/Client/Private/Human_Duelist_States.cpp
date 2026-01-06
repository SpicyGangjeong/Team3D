#include "pch.h"
#include "Human_Duelist.h"

#include "InfoInstance.h"

#include "GameInstance.h"
#include "Wand.h"
#include "Character_Controller.h"
#include "CallBack_Playable_Behavior.h"
#include "CallBack_Playable_HitReport.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_LightAttack.h"
#include "State_Spell.h"
#include "State_Shield.h"
#include "State_Hit.h"
#pragma endregion

#include "EffectPool.h"

void CHuman_Duelist::Behavior_IdleEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLE);
}

HRESULT CHuman_Duelist::Behavior_IdleExitCheck(_float fTimeDelta)
{
	return S_OK;
}

void CHuman_Duelist::Behavior_IdleExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLE);
}

void CHuman_Duelist::Behavior_CombatEnter()
{
	m_pFSM->Enable_State(FSMSTATE::COMBAT);
}

HRESULT CHuman_Duelist::Behavior_CombatExitCheck()
{
	return S_OK;
}

void CHuman_Duelist::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT);
}

void CHuman_Duelist::Behavior_LightAttackEnter()
{
	m_pFSM->Enable_State(FSMSTATE::LIGHT_ATTACK);
}

HRESULT CHuman_Duelist::Behavior_LightAttackExitCheck(_float fTimeDelta)
{
	return S_OK;
}

void CHuman_Duelist::Behavior_LightAttackExit()
{
	m_pFSM->Disable_State(FSMSTATE::LIGHT_ATTACK);
}

void CHuman_Duelist::Behavior_SpellEnter()
{
	m_pFSM->Enable_State(FSMSTATE::SPELL);
}

HRESULT CHuman_Duelist::Behavior_SpellExitCheck()
{
	return S_OK;
}

void CHuman_Duelist::Behavior_SpellExit()
{
	m_pFSM->Disable_State(FSMSTATE::SPELL);
}

void CHuman_Duelist::Behavior_ShieldEnter()
{
	m_pFSM->Enable_State(FSMSTATE::SHIELD);
}

HRESULT CHuman_Duelist::Behavior_ShieldExitCheck()
{
	return S_OK;
}

void CHuman_Duelist::Behavior_ShieldExit()
{
	m_pFSM->Disable_State(FSMSTATE::SHIELD);
}

void CHuman_Duelist::Behavior_HitEnter()
{
	m_pFSM->Enable_State(FSMSTATE::HIT);
}

HRESULT CHuman_Duelist::Behavior_HitExitCheck()
{
	return S_OK;
}

void CHuman_Duelist::Behavior_HitExit()
{
	m_pFSM->Disable_State(FSMSTATE::HIT);
}


void CHuman_Duelist::Add_FSM()
{
#pragma region Behavior_Movement_NotFocus
	{
		CState_Idle::STATE_IDLE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_IdleEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_IdleExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_IdleExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::IDLE, CState_Idle::Create(&Desc));
	}
#pragma endregion
#pragma region Behavior_Combat
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

	{
		CState_LightAttack::STATE_LIGHTATTACK_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_LightAttackEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_LightAttackExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_LightAttackExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::LIGHT_ATTACK, CState_LightAttack::Create(&Desc));
	}

	{
		CState_Spell::STATE_SPELL_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_SpellEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_SpellExitCheck(); };
		Desc.funcExitEvent = [this]() { Behavior_SpellExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::SPELL, CState_Spell::Create(&Desc));
	}

	{
		CState_Shield::STATE_SHIELD_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_ShieldEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_ShieldExitCheck(); };
		Desc.funcExitEvent = [this]() { Behavior_ShieldExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::SHIELD, CState_Shield::Create(&Desc));
	}
#pragma endregion

#pragma region Behavior_Hit
	{
		CState_Hit::STATE_HIT_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_HitEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_HitExitCheck(); };
		Desc.funcExitEvent = [this]() { Behavior_HitExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::HIT, CState_Hit::Create(&Desc));
	}

#pragma endregion

}

