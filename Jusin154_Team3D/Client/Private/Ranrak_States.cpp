#include "pch.h"
#include "Ranrak.h"
#include "InfoInstance.h"
#include "GameInstance.h"
#include "Player.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_IdleBreak.h"
#include "State_Land.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_Hover.h"
#include "State_Fly.h"
#include "State_FireBreath.h"
#include "State_FireBall.h"
#include "State_Pulse.h"
#include "State_Tucked.h"
#include "State_Hit.h"
#include "State_Dead.h"
#pragma endregion


void CRanrak::Behavior_IdleEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLE);
	pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::HOVER_LOOP];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrak::Behavior_IdleExitCheck()
{
	if (m_fTargetDistance <= 20.f && m_fTargetDistance !=0.f)
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return S_OK;
}

void CRanrak::Behavior_IdleExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLE);
}

void CRanrak::Behavior_IdleBreakEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLEBREAK);
	pair<_uint, _bool> pairAnimInfo;

}

HRESULT CRanrak::Behavior_IdleBreakExitCheck()
{
	if (m_pModelCom->IsFinishedAnim()) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
	}

	return S_OK;
}

void CRanrak::Behavior_IdleBreakExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLEBREAK);
}

void CRanrak::Behavior_MoveEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::MOVE);
	m_bLookAt = true;
	m_pFSM->Enable_State(FSMSTATE::JOG);
}

HRESULT CRanrak::Behavior_MoveExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	return S_OK;
}

void CRanrak::Behavior_MoveExit()
{
	m_pFSM->Disable_State(FSMSTATE::MOVE | FSMSTATE::SPRINT | FSMSTATE::JOG | FSMSTATE::WALK);
}

void CRanrak::Behavior_CombatEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::COMBAT);
	m_bLookAt = true;

	pairAnimInfo = m_Animation[STATEANIM::HOVER_LOOP];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrak::Behavior_CombatExitCheck(_float fTimeDelta)
{
	if (m_fTargetDistance > 30.f && m_fTargetDistance != 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}
	else if (m_fTargetDistance > 25.f && m_fTargetDistance != 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::MOVE);
		return E_FAIL;
	}


	if (m_fTargetDistance <= 20.f && m_fSkillCoolTime[ENUM_CLASS(RANRAK_SKILL::FIREBREATH)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::FIREBREATH);
		return E_FAIL;
	}
	else if (m_fTargetDistance <= 20.f && m_fSkillCoolTime[ENUM_CLASS(RANRAK_SKILL::FIREBALL)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::FIREBALL);
		return E_FAIL;
	}
	else if (m_fTargetDistance <= 20.f && m_fSkillCoolTime[ENUM_CLASS(RANRAK_SKILL::PULSE)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::PULSE);
		return E_FAIL;
	}
	else {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}

	return S_OK;
}

void CRanrak::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT);
}

void CRanrak::Behavior_HoverEnter()
{
}

HRESULT CRanrak::Behavior_HoverExitCheck(_float fTimeDelta)
{
	return E_NOTIMPL;
}

void CRanrak::Behavior_HoverExit()
{
	m_pFSM->Disable_State(FSMSTATE::HOVER);
}

void CRanrak::Behavior_FlyEnter()
{
}

HRESULT CRanrak::Behavior_FlyExitCheck(_float fTimeDelta)
{
	return E_NOTIMPL;
}

void CRanrak::Behavior_FlyExit()
{
	m_pFSM->Disable_State(FSMSTATE::FLY);
}

void CRanrak::Behavior_FireBreathEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::FIREBREATH);

	pairAnimInfo = m_Animation[STATEANIM::FIREBREATH_WINDUP];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

	Add_Event(pairAnimInfo.first,
		[&]() { pairAnimInfo = m_Animation[STATEANIM::FIREBREATH];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
				m_fSkillCoolTime[ENUM_CLASS(RANRAK_SKILL::FIREBREATH)] = m_fMaxSkillCoolTime[ENUM_CLASS(RANRAK_SKILL::FIREBREATH)]; },
		0.95f);

	Add_Event(m_Animation[STATEANIM::FIREBREATH].first,
		[&]() { pairAnimInfo = m_Animation[STATEANIM::FIREBREATH_COOLDOWN];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second); },
		0.95f);
}

HRESULT CRanrak::Behavior_FireBreathExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (m_pModelCom->IsFinishedAnim() && iCurrAnimIndex == m_Animation[STATEANIM::FIREBREATH_COOLDOWN].first)
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return S_OK;
}

void CRanrak::Behavior_FireBreathExit()
{
	m_pFSM->Disable_State(FSMSTATE::FIREBREATH);
}

void CRanrak::Behavior_FireBallEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::FIREBALL);

	_int iRand = m_pGameInstance->Real_Random_Int(0, 1);

	switch (iRand)
	{
	case 0 :
		pairAnimInfo = m_Animation[STATEANIM::FIREBALL1];
		break;
	case 1:
		pairAnimInfo = m_Animation[STATEANIM::FIREBALL2];
		break;
	}
	m_fSkillCoolTime[ENUM_CLASS(RANRAK_SKILL::FIREBALL)] = m_fMaxSkillCoolTime[ENUM_CLASS(RANRAK_SKILL::FIREBALL)];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrak::Behavior_FireBallExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return S_OK;
}

void CRanrak::Behavior_FireBallExit()
{
	m_pFSM->Disable_State(FSMSTATE::FIREBALL);
}

void CRanrak::Behavior_PulseEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::PULSE);

	pairAnimInfo = m_Animation[STATEANIM::PULSE];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

	m_fSkillCoolTime[ENUM_CLASS(RANRAK_SKILL::PULSE)] = m_fMaxSkillCoolTime[ENUM_CLASS(RANRAK_SKILL::PULSE)];
}

HRESULT CRanrak::Behavior_PulseExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return S_OK;
}

void CRanrak::Behavior_PulseExit()
{
	m_pFSM->Disable_State(FSMSTATE::PULSE);
}

void CRanrak::Behavior_TuckedEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::TUCKED);

	pairAnimInfo = m_Animation[STATEANIM::TUCKED];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrak::Behavior_TuckedExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return S_OK;
}

void CRanrak::Behavior_TuckedExit()
{
	m_pFSM->Disable_State(FSMSTATE::TUCKED);
}

void CRanrak::Behavior_HitEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::HIT);

	m_bLookAt = false;

	switch (m_eHitSpell)
	{
	case ENUM_CLASS(SKILL_TYPE::DESCENDO):
		pairAnimInfo = m_Animation[STATEANIM::KNOCKDOWN_FWD];
		break;
	case ENUM_CLASS(SKILL_TYPE::BOMBARDA):
		pairAnimInfo = m_Animation[STATEANIM::STUMBLE_BWD_L];
		break;
	case ENUM_CLASS(SKILL_TYPE::JAP):
	{
		if (m_fHitDegree >= 90.f)
		{
			_int iRandIndex = m_pGameInstance->Real_Random_Int(0, 3);
			switch (iRandIndex)
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
			case 3:
				pairAnimInfo = m_Animation[STATEANIM::HIT_BWD4];
				break;
			default:
				break;
			}
		}
		else if (m_fHitDegree >= 45.f) {

			if (m_fHitCross > 0.f) {
				pairAnimInfo = m_Animation[STATEANIM::HIT_L];
			}
			else {
				pairAnimInfo = m_Animation[STATEANIM::HIT_R];
			}
		}
		else {
			pairAnimInfo = m_Animation[STATEANIM::HIT_FWD];
		}
	}
	break;
	case ENUM_CLASS(SKILL_TYPE::LEVIOSO):
		pairAnimInfo = m_Animation[STATEANIM::HIT_LEVIOSO];
		break;
	case ENUM_CLASS(SKILL_TYPE::ACCIO):
		pairAnimInfo = m_Animation[STATEANIM::INCARCEROUS];
		break;
	}

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrak::Behavior_HitExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}
	return E_FAIL;
}

void CRanrak::Behavior_HitExit()
{
	m_pFSM->Disable_State(FSMSTATE::HIT);
	m_bLookAt = true;
}

void CRanrak::Behavior_DeadEnter()
{
	m_bLookAt = false;
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::DEAD);
	PSX::PxExtendedVec3 pxControlllerPos = m_pCharacter_Controller->Get_Controller()->getPosition();
	PSX::PxTransform pxTransform((_float)pxControlllerPos.x, (_float)pxControlllerPos.y + 100.f, (_float)pxControlllerPos.z);
	m_pCharacter_Controller->Set_Position(XMLoadFloat3((_float3*)&pxTransform.p));
	m_pRigidBody->SetActive(false);
	m_pCharacter_Controller->SetActive(false);

	_bool bStrongerKnockDown = { false };

	switch (m_eHitSpell)
	{
	case STATEANIM::KNOCKDOWN_FWD:
		bStrongerKnockDown = true;
		break;
	case STATEANIM::TUMBLE2:
		bStrongerKnockDown = true;
		break;
	default:
		break;
	}

	_float fabsRadius = fabsf(m_fHitRadius);
	_uint iState = { UINT_MAX };

	if (fabsRadius > 135.f) {
		iState = STATEANIM::DEAD_FWD;
	}
	else if (fabsRadius < 45.f) {
		iState = STATEANIM::DEAD_BWD;
	}
	else {
		if (m_fHitRadius < 0.f) {
			iState = STATEANIM::DEAD_L;
		}
		else {
			iState = STATEANIM::DEAD_R;
		}
	}
	pairAnimInfo = m_Animation[iState + bStrongerKnockDown];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrak::Behavior_DeadExitCheck(_float fTimeDelta)
{
	if (FLT_EPSILON > m_pModelCom->Get_CurrentTrackProgressRatio()) {
		return E_PENDING;
	}
	return S_OK;
}

void CRanrak::Behavior_DeadExit()
{
	m_bDead = true;
}


void CRanrak::Add_FSM()
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

#pragma region Hover

	{
		CState_Hover::STATE_HOVER_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_HoverEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_HoverExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_HoverExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::HOVER, CState_Hover::Create(&Desc));
	}

#pragma endregion

#pragma region Fly

	{
		CState_Fly::STATE_FLY_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_FlyEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_FlyExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_FlyExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::FLY, CState_Fly::Create(&Desc));
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


	{
		CState_FireBreath::STATE_FIREBREATH_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_FireBreathEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_FireBreathExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_FireBreathExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::FIREBREATH, CState_FireBreath::Create(&Desc));
	}

	{
		CState_FireBall::STATE_FIREBALL_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_FireBallEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_FireBallExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_FireBallExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::FIREBALL, CState_FireBall::Create(&Desc));
	}

	{
		CState_Pulse::STATE_PULSE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_PulseEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_PulseExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_PulseExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::PULSE, CState_Pulse::Create(&Desc));
	}

	{
		CState_Tucked::STATE_TUCKED_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_TuckedEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_TuckedExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_TuckedExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::TUCKED, CState_Tucked::Create(&Desc));
	}
#pragma endregion


#pragma region Hit
	{
		CState_Hit::STATE_HIT_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_HitEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_HitExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_HitExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::HIT, CState_Hit::Create(&Desc));
	}

#pragma endregion

#pragma region Dead
	{
		CState_Dead::STATE_DEAD_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_DeadEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_DeadExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_DeadExit(); };
		Desc.funcLateUpdate = [this](_float fDeadRatio) {
			m_fDeadRatio = fDeadRatio;
			if (m_fDeadRatio > 1.f) {
				m_bDead = true;
			}
			};
		Desc.vDeadTimer.x = FLT_EPSILON5;
		Desc.vDeadTimer.y = 2.f;
		m_States.emplace(FSMSTATE::DEAD, CState_Dead::Create(&Desc));
	}
#pragma endregion



}

void CRanrak::Set_Anim()
{
	m_Animation[STATEANIM::IDLE_BREAK1] = { 27,false };
	m_Animation[STATEANIM::IDLE_BREAK2] = { 28,false };

	m_Animation[STATEANIM::HOVER_LOOP] = { 29,true };
	m_Animation[STATEANIM::HOVER_DASH_BWD] = { 23,false };
	m_Animation[STATEANIM::HOVER_DASH_FWD] = { 24,false };
	m_Animation[STATEANIM::HOVER_DASH_LEFT] = { 25,false };
	m_Animation[STATEANIM::HOVER_DASH_RIGHT] = { 26,false };

	m_Animation[STATEANIM::FIREBREATH] = { 9,false };
	m_Animation[STATEANIM::FIREBREATH_COOLDOWN] = { 10,false };
	m_Animation[STATEANIM::FIREBREATH_WINDUP] = { 11,false };

	m_Animation[STATEANIM::FIREBALL1] = { 0,false };
	m_Animation[STATEANIM::FIREBALL2] = { 1,false };
	m_Animation[STATEANIM::PULSE] = { 33,false };

	m_Animation[STATEANIM::TUCKED] = { 21,true };

}
