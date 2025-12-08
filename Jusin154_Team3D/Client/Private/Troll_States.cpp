#include "pch.h"
#include "Troll.h"

#include "GameInstance.h"
#include "Player.h"
#include "Troll_Rock.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_IdleBreak.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "Troll_State_Rush.h"
#include "Troll_State_BackHand_Swing.h"
#include "State_Throw.h"
#include "State_Swing.h"
#include "State_Slam.h"
#include "State_Hit.h"
#pragma endregion


void CTroll::Behavior_IdleEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLE);
	pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::IDLE];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CTroll::Behavior_IdleExitCheck()
{
	if (m_fTargetDistance <= 18.f && m_fTargetDistance !=0)
		m_pFSM->Change_State(FSMSTATE::MOVE);

	return E_FAIL;
}

void CTroll::Behavior_IdleExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLE);
}

void CTroll::Behavior_IdleBreakEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLEBREAK);
	pair<_uint, _bool> pairAnimInfo;
	m_bLookAt = true;
	_int RandIndex = m_pGameInstance->Real_Random_Int(0, 3);
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
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CTroll::Behavior_IdleBreakExitCheck()
{
	if (m_pModelCom->IsFinishedAnim())
		m_pFSM->Change_State(FSMSTATE::MOVE);

	return E_FAIL;
}

void CTroll::Behavior_IdleBreakExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLEBREAK);
}

void CTroll::Behavior_MoveEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::MOVE);

	if (m_pFSM->IsEnable_Previous(FSMSTATE::IDLE))
	{
		m_pFSM->Enable_State(FSMSTATE::JOG);
		pairAnimInfo = m_Animation[STATEANIM::JOG_START];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	}
}

HRESULT CTroll::Behavior_MoveExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_bool bTurnPlaying =
		(iCurrAnimIndex == m_Animation[STATEANIM::JOG_LEFT_TURN].first) ||
		(iCurrAnimIndex == m_Animation[STATEANIM::JOG_RIGHT_TURN].first);

	if (iCurrAnimIndex == m_Animation[STATEANIM::JOG_START].first)
	{
		_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
		if (fRatio >= 0.9f)
		{
			pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
	}

	if (m_fTargetDistance <= 18.f && m_fTargetDistance >= 14.f) {
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	
	if (bTurnPlaying)
	{
		m_bLookAt = false;

		if (m_pModelCom->IsFinishedAnim())
		{
			m_bLookAt = true;
			pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}

		return S_OK;
	}

	if (m_fDegree >= 90.f)
	{
		m_bLookAt = true;

		if (m_fCross > 0)
			pairAnimInfo = m_Animation[STATEANIM::JOG_LEFT_TURN];
		else
			pairAnimInfo = m_Animation[STATEANIM::JOG_RIGHT_TURN];

		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		return S_OK;
	}

	if (m_fTargetDistance > 8.f)
	{
		m_bLookAt = true;

		if (iCurrAnimIndex == m_Animation[STATEANIM::JOG_START].first)
		{
			_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
			if (fRatio >= 0.9f){
				pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			}
		}
		else if (iCurrAnimIndex != m_Animation[STATEANIM::JOG_FWD].first)
		{
			pairAnimInfo = m_Animation[STATEANIM::JOG_START];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}

		return S_OK;
	}


	if (m_fTargetDistance <= 6.f && m_fTargetDistance != 0.f)
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
}

HRESULT CTroll::Behavior_CombatExitCheck(_float fTimeDelta)
{
	m_bLookAt = true;
	if (m_fTargetDistance > 30.f && m_fTargetDistance != 0.f)
		m_pFSM->Change_State(FSMSTATE::IDLE);
	else if (m_fTargetDistance > 25.f && m_fTargetDistance != 0.f)
		m_pFSM->Change_State(FSMSTATE::MOVE);

	return E_FAIL;
}

void CTroll::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT);
}

void CTroll::Behavior_RushEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::RUSH);
	pairAnimInfo = m_Animation[STATEANIM::RUSH_START];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
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
				m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::RUSH)] = m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::RUSH)];
			}
		}
		else {
			m_fRushTime += fTimeDelta;

			if (m_fRushTime >= 1.5f)
			{
				m_fRushTime = 0.f;
				m_pFSM->Disable_State(FSMSTATE::RUSH);
				pairAnimInfo = m_Animation[STATEANIM::RUSH_END];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			}
		}
		return S_OK;
	}
	if (iCurrAnimIndex == m_Animation[STATEANIM::RUSH_END].first)
	{
		if (m_fCross > 0)
			pairAnimInfo = m_Animation[STATEANIM::IDLE_COMBAT_TURN_BWD_L];
		else
			pairAnimInfo = m_Animation[STATEANIM::IDLE_COMBAT_TURN_BWD_R];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

		Add_Event(pairAnimInfo.first,
			[this]() {m_bLookAt = true; },
			0.3f);
	}

	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
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
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

	Add_Event(pairAnimInfo.first,
		[this](){Get_PartObject<CTroll_Rock>()->Set_Visible(true);},
		0.95f);
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
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
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
		m_pFSM->Change_State(FSMSTATE::COMBAT);
	}
	return E_FAIL;
}

void CTroll::Behavior_ThrowExit()
{
	m_pFSM->Disable_State(FSMSTATE::THROW_ROCK);
	Get_PartObject<CTroll_Rock>()->Set_Attach(true);
	Get_PartObject<CTroll_Rock>()->Set_Visible(false);
}

void CTroll::Behavior_SwingEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::SWING);

	pairAnimInfo = m_Animation[STATEANIM::SWING_FWD];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SWING)] =
		m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SWING)];

	Add_Event(pairAnimInfo.first,
		[this]() {m_bLookAt = false; },
		0.2f);
}

HRESULT CTroll::Behavior_SwingExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return E_FAIL;
}

void CTroll::Behavior_SwingExit()
{
	m_pFSM->Disable_State(FSMSTATE::SWING);
}

void CTroll::Behavior_SlamEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::SLAM);

	pairAnimInfo = m_Animation[STATEANIM::SLAM];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SLAM)] =
		m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SLAM)];

	Add_Event(m_Animation[STATEANIM::SLAM].first,
		[this]() { m_bLookAt = false; },
		0.2f);
}

HRESULT CTroll::Behavior_SlamExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return E_FAIL;
}

void CTroll::Behavior_SlamExit()
{
	m_pFSM->Disable_State(FSMSTATE::SLAM);
}

void CTroll::Behavior_BackHandSwingEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::BACKHAND_SWING);
	pairAnimInfo = m_Animation[STATEANIM::BACKHAND_SWING_JOG];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::BACKHAND_SWING)] =
		m_fMaxSkillCoolTime[ENUM_CLASS(TROLL_SKILL::BACKHAND_SWING)];

	Add_Event(m_Animation[STATEANIM::BACKHAND_SWING_JOG].first,
		[this]() { m_bLookAt = false; },
		0.2f);
}

HRESULT CTroll::Behavior_BackHandSwingExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return E_FAIL;
}

void CTroll::Behavior_BackHandSwingExit()
{
	m_pFSM->Disable_State(FSMSTATE::BACKHAND_SWING);
}

void CTroll::Behavior_HitEnter()
{
	m_pFSM->Enable_State(FSMSTATE::HIT);
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

	m_bLookAt = false;
	if (iCurrAnimIndex == m_Animation[STATEANIM::SLAM].first)
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
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		m_bLookAt = true;
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
		CState_IdleBreak::STATE_IDLEBREAK_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_IdleBreakEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_IdleBreakExitCheck(); };
		Desc.funcExitEvent = [this]() { Behavior_IdleBreakExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::IDLEBREAK, CState_IdleBreak::Create(&Desc));
	}
	{
		CState_Move::STATE_MOVE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_MoveEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_MoveExitCheck(fTimedelta); };
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

	{
		CState_Swing::STATE_SWING_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_SwingEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_SwingExitCheck(fTimeDelta); };
		Desc.funcExitEvent = [this]() { Behavior_SwingExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::SWING, CState_Swing::Create(&Desc));
	}

	{
		CState_Slam::STATE_SLAM_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_SlamEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_SlamExitCheck(fTimeDelta); };
		Desc.funcExitEvent = [this]() { Behavior_SlamExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::SLAM, CState_Slam::Create(&Desc));
	}

	{
		CTroll_State_BackHand_Swing::TROLL_STATE_BACK_HAND_SWING_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_BackHandSwingEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_BackHandSwingExitCheck(fTimeDelta); };
		Desc.funcExitEvent = [this]() { Behavior_BackHandSwingExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::BACKHAND_SWING, CTroll_State_BackHand_Swing::Create(&Desc));
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
	m_Animation[STATEANIM::IDLE_COMBAT_TURN_BWD_R] = { 7,false };
	m_Animation[STATEANIM::IDLE_COMBAT_TURN_BWD_L] = { 8,false };
	m_Animation[STATEANIM::IDLE_BREAK1] = { 11,false };
	m_Animation[STATEANIM::IDLE_BREAK2] = { 141,false };
	m_Animation[STATEANIM::IDLE_BREAK3] = { 14,false };
	m_Animation[STATEANIM::IDLE_BREAK4] = { 143,false };

	m_Animation[STATEANIM::WALK_FWD] = { 22,true };

	m_Animation[STATEANIM::JOG_START] = { 104,false };
	m_Animation[STATEANIM::JOG_LEFT_TURN] = { 112,false };
	m_Animation[STATEANIM::JOG_RIGHT_TURN] = { 108,false };
	m_Animation[STATEANIM::JOG_FWD] = { 103,true };

	m_Animation[STATEANIM::DODGE_LEFT] = { 125, false };
	m_Animation[STATEANIM::DODGE_RIGHT] = { 126, false };
	m_Animation[STATEANIM::SPRINT] = { 90, true };

	m_Animation[STATEANIM::SLAM] = { 66,false }; // 도끼 찍기
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


	//왼턴 108
	//오른턴 112
	// 106 우회전
	// 110 좌회전

}
