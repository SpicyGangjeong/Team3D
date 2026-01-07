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
#include "State_IdleBreak.h"
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
	pair<_uint, _bool> pairAnimInfo;
	pairAnimInfo = m_Animation[STATEANIM::IDLE];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CHuman_Duelist::Behavior_IdleExitCheck(_float fTimeDelta)
{
	for (_uint i = 0; i < ENUM_CLASS(SKILL::END); i++)
	{
		if (m_fSkillCoolTime[i] == 0.f) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
			return E_FAIL;
		}
	}
	return S_OK;
}

void CHuman_Duelist::Behavior_IdleExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLE);
}

void CHuman_Duelist::Behavior_IdleBreakEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLE);
	pair<_uint, _bool> pairAnimInfo;
	_int iRand = m_pGameInstance->Real_Random_Int(0, 1);
	if (iRand == 0) {
		pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK1];
	}
	else if (iRand == 1) {
		pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK2];
	}


	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CHuman_Duelist::Behavior_IdleBreakExitCheck(_float fTimeDelta)
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return S_OK;
}

void CHuman_Duelist::Behavior_IdleBreakExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLEBREAK);
}

void CHuman_Duelist::Behavior_CombatEnter()
{
	m_pFSM->Enable_State(FSMSTATE::COMBAT);
}

HRESULT CHuman_Duelist::Behavior_CombatExitCheck()
{
	if (m_fSkillCoolTime[ENUM_CLASS(SKILL::LEVIOSO)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::SPELL);
		return E_FAIL;
	}
	else if (m_fSkillCoolTime[ENUM_CLASS(SKILL::PROTEGO)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::SHIELD);
		return E_FAIL;
	}
	else if (m_fSkillCoolTime[ENUM_CLASS(SKILL::LIGHT_ATTACK)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::LIGHT_ATTACK);
		return E_FAIL;
	}
	else {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}

	return S_OK;
}

void CHuman_Duelist::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT);
}

void CHuman_Duelist::Behavior_LightAttackEnter()
{
	pair<_uint, _bool> pairAnimInfo;
	m_pFSM->Enable_State(FSMSTATE::LIGHT_ATTACK);
	_int iRand = m_pGameInstance->Real_Random_Int(0, 3);
	switch (iRand)
	{
	case 0:
		pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK];
		break;
	case 1:
		pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK2];
		break;
	case 2:
		pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK3];
		break;
	case 3:
		pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK4];
		break;
	}
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

	Add_Event(pairAnimInfo.first,
		[this]() {_uint iIndex = 0; m_pEffectPool->Use_Skill(SKILL_TYPE::DUELIST_JAP, Get_PartObject<CWand>(), &iIndex);  },
		0.2f);

	Add_Event(pairAnimInfo.first,
		[this]() { m_pEffectPool->Use_Skill(SKILL_TYPE::JAP_SIDE, Get_PartObject<CWand>());  },
		0.0f);

	m_fSkillCoolTime[ENUM_CLASS(SKILL::LIGHT_ATTACK)] = m_fMaxSkillCoolTime[ENUM_CLASS(SKILL::LIGHT_ATTACK)];
}

HRESULT CHuman_Duelist::Behavior_LightAttackExitCheck(_float fTimeDelta)
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return S_OK;
}

void CHuman_Duelist::Behavior_LightAttackExit()
{
	m_pFSM->Disable_State(FSMSTATE::LIGHT_ATTACK);
}

void CHuman_Duelist::Behavior_SpellEnter()
{
	m_pFSM->Enable_State(FSMSTATE::SPELL);
	pair<_uint, _bool> pairAnimInfo;
	pairAnimInfo = m_Animation[STATEANIM::SPELL];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

	Add_Event(pairAnimInfo.first,
		[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::DUELIST_LEVIOSO, this); },
		0.2f);
	Add_Event(pairAnimInfo.first,
		[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::LEVIOSO_SIDE, Get_PartObject<CWand>()); },
		0.f);

	m_fSkillCoolTime[ENUM_CLASS(SKILL::LEVIOSO)] = m_fMaxSkillCoolTime[ENUM_CLASS(SKILL::LEVIOSO)];
}

HRESULT CHuman_Duelist::Behavior_SpellExitCheck()
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return S_OK;
}

void CHuman_Duelist::Behavior_SpellExit()
{
	m_pFSM->Disable_State(FSMSTATE::SPELL);
}

void CHuman_Duelist::Behavior_ShieldEnter()
{
	m_pFSM->Enable_State(FSMSTATE::SHIELD);

	pair<_uint, _bool> pairAnimInfo;
	pairAnimInfo = m_Animation[STATEANIM::SKILL];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

	Add_Event(pairAnimInfo.first,
		[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::DUELIST_PROTEGO, this); },
		0.1f);

	m_fSkillCoolTime[ENUM_CLASS(SKILL::PROTEGO)] = m_fMaxSkillCoolTime[ENUM_CLASS(SKILL::PROTEGO)];
}

HRESULT CHuman_Duelist::Behavior_ShieldExitCheck()
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
		return E_FAIL;
	}
	return S_OK;
}

void CHuman_Duelist::Behavior_ShieldExit()
{
	m_pFSM->Disable_State(FSMSTATE::SHIELD);
}

void CHuman_Duelist::Behavior_HitEnter()
{
	m_pFSM->Enable_State(FSMSTATE::HIT);
	pair<_uint, _bool> pairAnimInfo;

	switch (m_eHitSpell)
	{
	case ENUM_CLASS(SKILL_TYPE::JAP):
		break;
	}

}

HRESULT CHuman_Duelist::Behavior_HitExitCheck()
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
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

	{
		CState_IdleBreak::STATE_IDLEBREAK_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_IdleBreakEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_IdleBreakExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_IdleBreakExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::IDLEBREAK, CState_IdleBreak::Create(&Desc));
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

