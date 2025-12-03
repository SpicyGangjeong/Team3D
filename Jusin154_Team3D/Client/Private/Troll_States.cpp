#include "pch.h"
#include "Troll.h"

#include "GameInstance.h"
#include "Player.h"
#include "Troll_Rock.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_Hit.h"
#include "Troll_State_Rush.h"
#include "State_Throw.h"
#pragma endregion


void CTroll::Behavior_IdleEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLE);
	pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::IDLE];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CTroll::Behavior_IdleExitCheck()
{
	if (m_fTargetDistance <= 20.f)
		m_pFSM->Change_State(FSMSTATE::MOVE);

	return E_FAIL;
}

void CTroll::Behavior_IdleExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLE);
}

void CTroll::Behavior_MoveEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::MOVE);

	if (m_pFSM->IsEnable_Previous(FSMSTATE::IDLE))
	{
		m_pFSM->Enable_State(FSMSTATE::JOG);
		pairAnimInfo = m_Animation[STATEANIM::JOG_START];
	}


	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CTroll::Behavior_MoveExitCheck()
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	if (iCurrAnimIndex == m_Animation[STATEANIM::JOG_START].first)
	{
		if (m_pModelCom->IsFinishedAnim())
		{
			pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
		}
	}

	if (m_fTargetDistance <= 18.f && m_fTargetDistance !=0.f)
		m_pFSM->Change_State(FSMSTATE::COMBAT);

	return E_FAIL;
}

void CTroll::Behavior_MoveExit()
{
	m_pFSM->Disable_State(FSMSTATE::MOVE | FSMSTATE::SPRINT | FSMSTATE::JOG | FSMSTATE::WALK);
}

void CTroll::Behavior_CombatEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::COMBAT);

	m_bLookAt = true;
	if (m_fTargetDistance <= 12.f && m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SLAM)] <= 0.f)
	{
		m_pFSM->Enable_State(FSMSTATE::SKILL);
		pairAnimInfo = m_Animation[STATEANIM::SKILL];
		m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SLAM)] = m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SLAM)];
	}
	else if (m_fTargetDistance <= 12.f && m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::BACKHAND_SWING)] <= 0.f)
	{
		m_pFSM->Enable_State(FSMSTATE::BACKHAND_SWING);
		pairAnimInfo = m_Animation[STATEANIM::BACKHAND_SWING_JOG];
		m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::BACKHAND_SWING)] = m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::BACKHAND_SWING)];
	}
	else if (m_fTargetDistance <= 12.f && m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SWING)] <= 0.f)
	{
		m_pFSM->Enable_State(FSMSTATE::SWING);
		pairAnimInfo = m_Animation[STATEANIM::SWING_FWD];
		m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SWING)] = m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SWING)];
	}
	else if (m_fTargetDistance <= 15.f && m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::THROWROCK)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::THROW_ROCK);
		return;
	}
	else  if (m_fTargetDistance <= 18.f && m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::RUSH)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::RUSH);
		return;
	}
	else
	{
		_int RandIndex = m_pGameInstance->Real_Random_Int(0,3);
		switch (RandIndex)
		{
		case 0:
			pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK1];
			break;
		case 1:
			pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK2];
			break;
		case 2:
			pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK3];
			break;
		case 3:
			pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK4];
			break;
		}
	}

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,1.f,true);
}

HRESULT CTroll::Behavior_CombatExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (m_pFSM->IsEnable(FSMSTATE::SKILL))
	{
		m_bLookAt = false;
		m_pFSM->Disable_State(FSMSTATE::SKILL);
		pairAnimInfo = m_Animation[STATEANIM::SKILL];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
		m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SLAM)] = m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SLAM)];
		return S_OK;
	}

	if (m_pFSM->IsEnable(FSMSTATE::SWING))
	{
		m_bLookAt = false;
		m_pFSM->Disable_State(FSMSTATE::SWING);
		pairAnimInfo = m_Animation[STATEANIM::SWING_FWD];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
		m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SWING)] = m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SWING)];
		return S_OK;
	}

	if (m_pFSM->IsEnable(FSMSTATE::BACKHAND_SWING))
	{
		m_bLookAt = false;
		m_pFSM->Disable_State(FSMSTATE::BACKHAND_SWING);
		pairAnimInfo = m_Animation[STATEANIM::BACKHAND_SWING_JOG];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
		m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::BACKHAND_SWING)] = m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::BACKHAND_SWING)];
		return S_OK;
	}

	
	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::IDLE);
	}

	return E_FAIL;
}

void CTroll::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT | FSMSTATE::SKILL | FSMSTATE::SKILL2 |FSMSTATE::SWING |FSMSTATE::BACKHAND_SWING);
}

void CTroll::Behavior_RushEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::RUSH);
	pairAnimInfo = m_Animation[STATEANIM::RUSH_START];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
}

HRESULT CTroll::Behavior_RushExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	if (m_pFSM->IsEnable(FSMSTATE::RUSH))
	{
		if (iCurrAnimIndex == m_Animation[STATEANIM::RUSH_START].first)
		{
			if (m_pModelCom->IsFinishedAnim())
			{
				m_bLookAt = false;
				pairAnimInfo = m_Animation[STATEANIM::RUSH_LOOP];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			}
		}
		else {
			m_fRushTime += fTimeDelta;

			if (m_fRushTime >= 1.5f)
			{
				m_fRushTime = 0.f;
				m_pFSM->Disable_State(FSMSTATE::RUSH);
				pairAnimInfo = m_Animation[STATEANIM::RUSH_END];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
				m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::RUSH)] = m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::RUSH)];
			}
		}
		return S_OK;
	}
	if (iCurrAnimIndex == m_Animation[STATEANIM::RUSH_END].first)
	{
		m_bLookAt = true;
		pairAnimInfo = m_Animation[STATEANIM::IDLE_COMBAT_TURN_BWD];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
	}

	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::IDLE);
	}
	return E_FAIL;
}

void CTroll::Behavior_RushExit()
{
	m_pFSM->Disable_State(FSMSTATE::RUSH);
}

void CTroll::Behavior_ThrowEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::THROW_ROCK);
	pairAnimInfo = m_Animation[STATEANIM::THROW_ROCK_START];

	Add_Event(pairAnimInfo.first,
		[this](){Get_PartObject<CTroll_Rock>()->Set_Visible(true);},
		0.9f);
	
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
}

HRESULT CTroll::Behavior_ThrowExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	if (m_pFSM->IsEnable(FSMSTATE::THROW_ROCK))
	{
		if (iCurrAnimIndex == m_Animation[STATEANIM::THROW_ROCK_START].first)
		{
			if (m_pModelCom->IsFinishedAnim())
			{
				m_pFSM->Disable_State(FSMSTATE::THROW_ROCK);
				pairAnimInfo = m_Animation[STATEANIM::THROW_ROCK];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
				m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::THROWROCK)] = m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::THROWROCK)];
				Add_Event(pairAnimInfo.first,
					[this]() {Get_PartObject<CTroll_Rock>()->Set_Attach(false); },
					0.25f);
			}
		}
		return S_OK;
	}
	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::IDLE);
	}
	return E_FAIL;
}

void CTroll::Behavior_ThrowExit()
{
	m_pFSM->Disable_State(FSMSTATE::THROW_ROCK);
}

void CTroll::Behavior_HitEnter()
{
	m_pFSM->Enable_State(FSMSTATE::HIT);
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

	if (iCurrAnimIndex == m_Animation[STATEANIM::SKILL].first)
	{
		pairAnimInfo = m_Animation[STATEANIM::HIT_FACE];
	}
	else {
		_int RandIndex = m_pGameInstance->Real_Random_Int(0, 2);
		switch (RandIndex)
		{
		case 0:
			pairAnimInfo = m_Animation[STATEANIM::HIT_BWD];
			break;
		case 1:
			pairAnimInfo = m_Animation[STATEANIM::HIT_BWD2];
			break;
		case 2:
			pairAnimInfo = m_Animation[STATEANIM::HIT_BWD3];
			break;
		}
		}
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CTroll::Behavior_HitExitCheck()
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

	if (iCurrAnimIndex == m_Animation[STATEANIM::HIT_FACE].first)
	{
		if (m_pModelCom->IsFinishedAnim()) {
			pairAnimInfo = m_Animation[STATEANIM::HIT_FACE_END];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			return S_OK;
		}
	}
	
	if (m_pModelCom->IsFinishedAnim()) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}
	return E_FAIL;
}

void CTroll::Behavior_HitExit()
{
	m_pFSM->Disable_State(FSMSTATE::HIT);
}


void CTroll::Add_FSM()
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
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_CombatExitCheck(fTimeDelta); };
		Desc.funcExitEvent = [this]() { Behavior_CombatExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::COMBAT, CState_Combat::Create(&Desc));
	}

	{
		CTroll_State_Rush::TROLL_STATE_RUSH_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_RushEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_RushExitCheck(fTimeDelta); };
		Desc.funcExitEvent = [this]() { Behavior_RushExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::RUSH, CTroll_State_Rush::Create(&Desc));
	}

	{
		CState_Throw::STATE_THROW_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_ThrowEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_ThrowExitCheck(fTimeDelta); };
		Desc.funcExitEvent = [this]() { Behavior_ThrowExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::THROW_ROCK, CState_Throw::Create(&Desc));
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

void CTroll::Set_Anim()
{
	m_Animation[STATEANIM::IDLE] = { 16,true };
	m_Animation[STATEANIM::IDLE_COMBAT_TURN_BWD] = { 8,false };
	m_Animation[STATEANIM::IDLE_BREAK1] = { 11,false };
	m_Animation[STATEANIM::IDLE_BREAK2] = { 141,false };
	m_Animation[STATEANIM::IDLE_BREAK3] = { 14,false };
	m_Animation[STATEANIM::IDLE_BREAK4] = { 143,false };

	m_Animation[STATEANIM::WALK_FWD] = { 22,true };

	m_Animation[STATEANIM::JOG_START] = { 104,false };
	m_Animation[STATEANIM::JOG_FWD] = { 103,true };

	m_Animation[STATEANIM::DODGE_LEFT] = { 125, false };
	m_Animation[STATEANIM::DODGE_RIGHT] = { 126, false };
	m_Animation[STATEANIM::SPRINT] = { 90, true };

	m_Animation[STATEANIM::SKILL] = { 66,false }; // 도끼 찍기
	m_Animation[STATEANIM::SKILL2] = { 70,false }; 

	m_Animation[STATEANIM::THROW_ROCK_START] = { 77,false };
	m_Animation[STATEANIM::THROW_ROCK] = { 74,false };

	m_Animation[STATEANIM::RUSH_START] = { 55,false }; 
	m_Animation[STATEANIM::RUSH_LOOP] = { 54,true };
	m_Animation[STATEANIM::RUSH_END] = { 56,false };

	m_Animation[STATEANIM::BACKHAND_SWING_JOG] = { 89,false };

	m_Animation[STATEANIM::SWING_FWD] = { 87,false };

	m_Animation[STATEANIM::HIT_BWD] = { 167, false }; // 
	m_Animation[STATEANIM::HIT_BWD2] = { 168, false }; //
	m_Animation[STATEANIM::HIT_BWD3] = { 169, false }; // 

	m_Animation[STATEANIM::HIT_FACE] = { 156, false }; // 
	m_Animation[STATEANIM::HIT_FACE_END] = { 157, false }; // 

	m_Animation[STATEANIM::STUN] = { 60, false }; // 
	

	m_Animation[STATEANIM::KNOCKDOWN_BWD] = { 165, false }; // 


}
