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
	if (m_fTargetDistance <= 20.f)
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
	pairAnimInfo = m_Animation[STATEANIM::JOG_START];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	
}

HRESULT CGoblin::Behavior_MoveExitCheck()
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

	if (iCurrAnimIndex == m_Animation[STATEANIM::JOG_FWD].first) {
		if (m_fTargetDistance <= 10.f && m_fTargetDistance != 0.f)
			m_pFSM->Change_State(FSMSTATE::COMBAT);
	}


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

	m_bLookAt = true;
	if (m_fTargetDistance <= 3.f && m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SLAM)] <= 0.f)
	{
		m_pFSM->Enable_State(FSMSTATE::SLAM);
		pairAnimInfo = m_Animation[STATEANIM::SLAM];
		m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SLAM)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SLAM)];
	}
	else if (m_fTargetDistance <= 5.f && m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SWING)] <= 0.f)
	{
		m_pFSM->Enable_State(FSMSTATE::SWING);
		pairAnimInfo = m_Animation[STATEANIM::SWING_FWD];
		m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SWING)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SWING)];
	}
	else if (m_fTargetDistance >= 5.f && m_fTargetDistance <= 10.f && m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::THROW)] <= 0.f)
	{
		m_pFSM->Enable_State(FSMSTATE::SKILL);
		pairAnimInfo = m_Animation[STATEANIM::SKILL];
		m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::THROW)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::THROW)];
	}
	else {
		pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
	}


	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CGoblin::Behavior_CombatExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (m_pFSM->IsEnable(FSMSTATE::SLAM))
	{
		m_pFSM->Disable_State(FSMSTATE::SLAM);
		pairAnimInfo = m_Animation[STATEANIM::SLAM];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
		m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SLAM)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SLAM)];
		Add_Event(pairAnimInfo.first,
			[this]() {m_bLookAt = false; },
			0.2f);
		return S_OK;
	}

	if (m_pFSM->IsEnable(FSMSTATE::SWING))
	{

		_float fDesiredRange = 1.5f;
		_float dist = m_fTargetDistance;

		if (dist > fDesiredRange)
		{
			_vector vPos = Get_WorldPostion();
			_vector vDistance = XMLoadFloat4(&m_vTargetPos) - vPos;
			vDistance = XMVector4Normalize(vDistance);
			_float step = min(dist - fDesiredRange, 1.5f);

			m_pTransformCom->AccumulateMomentum(vDistance * step * fTimeDelta);
		}

		m_pFSM->Disable_State(FSMSTATE::SWING);
		pairAnimInfo = m_Animation[STATEANIM::SWING_FWD];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
		m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SWING)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SWING)];
		Add_Event(pairAnimInfo.first,
			[this]() {m_bLookAt = false; },
			0.2f);
		return S_OK;
	}

	if (m_pFSM->IsEnable(FSMSTATE::SKILL))
	{
		m_pFSM->Disable_State(FSMSTATE::SKILL);
		pairAnimInfo = m_Animation[STATEANIM::SKILL];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
		m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::THROW)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::THROW)];
		Add_Event(pairAnimInfo.first,
			[this]() {m_bLookAt = false; },
			0.2f);
		return S_OK;
	}

	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}

	return E_FAIL;
}

void CGoblin::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT | FSMSTATE::SLAM | FSMSTATE::SKILL | FSMSTATE::SWING);
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
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::MOVE, CState_Move::Create(&Desc));
	}
#pragma endregion
#pragma region Behavior_Combat_NotFocus
	{
		CState_Combat::STATE_COMBAT_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_CombatEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_CombatExitCheck(fTimedelta); };
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
	m_Animation[STATEANIM::IDLE_TURN_BWD] = { 24, false };
	m_Animation[STATEANIM::IDLE_TURN_L] = { 26, false };
	m_Animation[STATEANIM::IDLE_TURN_R] = { 30, false };

	m_Animation[STATEANIM::WALK_FWD] = { 50,true };
	m_Animation[STATEANIM::WALK_START] = { 57,false };
	m_Animation[STATEANIM::WALK_STOP] = { 58,false };
	m_Animation[STATEANIM::WALK_BWD] = { 49, true };

	m_Animation[STATEANIM::JOG_FWD] = { 147,true };
	m_Animation[STATEANIM::JOG_START] = { 157,false };
	m_Animation[STATEANIM::JOG_BWD] = { 154,false };

	m_Animation[STATEANIM::DODGE_LEFT] = { 125, false };
	m_Animation[STATEANIM::DODGE_RIGHT] = { 126, false };
	m_Animation[STATEANIM::SPRINT] = { 90, true };

	m_Animation[STATEANIM::SLAM] = { 439,false }; // 도끼 찍기
	m_Animation[STATEANIM::SKILL] = { 85,false }; // 나이프 던지기
	m_Animation[STATEANIM::SWING_FWD] = { 444,false }; // 도끼 스윙

	m_Animation[STATEANIM::HIT_FWD] = { 353, false };
	m_Animation[STATEANIM::HIT_BWD] = { 352, false };
	m_Animation[STATEANIM::HIT_BWD2] = { 349, false };
	m_Animation[STATEANIM::HIT_BWD3] = { 350, false };
	m_Animation[STATEANIM::HIT_BWD4] = { 351, false };
	m_Animation[STATEANIM::HIT_L] = { 354, false };
	m_Animation[STATEANIM::HIT_R] = { 355, false };

	m_Animation[STATEANIM::KNOCKDOWN_BWD] = { 374, false };
	m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT] = { 375, false };
	m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT_HOLD] = { 370, false };
	m_Animation[STATEANIM::KNOCKDOWN_FWD] = { 371, false };
	m_Animation[STATEANIM::KNOCKDOWN_FWD_SPLT] = { 372, false };
	m_Animation[STATEANIM::KNOCKDOWN_FWD_SPLT_HOLD] = { 373, false };

	m_Animation[STATEANIM::GETUP_BWD] = { 330, false };
	m_Animation[STATEANIM::GETUP_FWD] = { 332, false };
	m_Animation[STATEANIM::GETUP_L] = { 334, false };
	m_Animation[STATEANIM::GETUP_L_FD] = { 336, false };
	m_Animation[STATEANIM::GETUP_L_FU] = { 338, false };
	m_Animation[STATEANIM::GETUP_R] = { 340, false };
	m_Animation[STATEANIM::GETUP_R_FD] = { 342, false };
	m_Animation[STATEANIM::GETUP_R_FU] = { 344, false };
	m_Animation[STATEANIM::GETUP_SLOUCH] = { 346, false };

	m_Animation[STATEANIM::KNOCKBACK] = { 377, true };
	m_Animation[STATEANIM::KNOCKBACK2] = { 378, true };
	m_Animation[STATEANIM::TUMBLE] = { 379, true };
	m_Animation[STATEANIM::TUMBLE2] = { 380, true };
	m_Animation[STATEANIM::PETRIFICUSED_START] = { 383, false };

	m_Animation[STATEANIM::DEAD_BWD] = { 317, false };
	m_Animation[STATEANIM::DEAD_BWD2] = { 318, false };
	m_Animation[STATEANIM::DEAD_FWD] = { 319, false };
	m_Animation[STATEANIM::DEAD_FWD2] = { 320, false };
	m_Animation[STATEANIM::DEAD_L] = { 321, false };
	m_Animation[STATEANIM::DEAD_L2] = { 322, false };
	m_Animation[STATEANIM::DEAD_R] = { 323, false };
	m_Animation[STATEANIM::DEAD_R2] = { 324, false };









	// 걷기 시작 57
	// 걷기 루프 50
	// 뛰기 시작 157
	// 뛰기 루프 45
	// 나이프 던지기 85
	// 도끼 공격 444
	// 도끼 공격 후 76
	// 도끼 찍기 439
	// 

}
