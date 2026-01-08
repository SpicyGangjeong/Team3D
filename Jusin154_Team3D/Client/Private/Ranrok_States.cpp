#include "pch.h"
#include "Ranrok.h"
#include "InfoInstance.h"
#include "GameInstance.h"
#include "Player.h"
#include "Effect_Container.h"
#include "EffectPool.h"
#include "EffectParts.h"
#include "TrailObject.h"
#include "Ranrok_Breath.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_IdleBreak.h"
#include "State_Land.h"
#include "State_Move.h"
#include "State_Ground.h"
#include "State_Combat.h"
#include "State_Hover.h"
#include "State_Fly.h"
#include "State_FireBreath.h"
#include "State_FireSweep.h"
#include "State_FireBall.h"
#include "State_Swipe.h"
#include "State_Skill.h"
#include "State_Rush.h"
#include "State_Pulse.h"
#include "State_Tucked.h"
#include "State_Hit.h"
#include "State_Dead.h"
#pragma endregion


void CRanrok::Behavior_IdleEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLE);
	pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::HOVER_LOOP];
	m_bLookAt = true;
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrok::Behavior_IdleExitCheck()
{
	if (m_fTargetDistance <= 100.f && m_fTargetDistance != 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return S_OK;
}

void CRanrok::Behavior_IdleExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLE);
	m_bLookAt = false;
}

void CRanrok::Behavior_IdleBreakEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLEBREAK);
	pair<_uint, _bool> pairAnimInfo;
	if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_AIR))
	{
		pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK2];
	}
	else if(m_ePhase== ENUM_CLASS(RANROK_PHASE::PHASE_GROUND)) {
		if (m_fDegree < 30.f)
		{
			_int iRand = m_pGameInstance->Real_Random_Int(0, 1);
			if (iRand == 0)
			{
				pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK3];
			}
			else {
				pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK4];
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
		else if (m_fDegree < 60.f)
		{
			if (m_fCross < 0.f)
			{
				pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_45_L];
			}
			else {
				pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_45_R];
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 1.5f);
		}
		else if (m_fDegree < 150.f)
		{
			if (m_fCross < 0.f)
			{
				pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_90_L];
			}
			else {
				pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_90_R];
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 1.5f);
		}
		else {
			if (m_fCross < 0.f)
			{
				pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_180_L];
			}
			else {
				pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_180_R];
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,1.f,false,1.5f);
		}
	}
}

HRESULT CRanrok::Behavior_IdleBreakExitCheck(_float fTimeDelta)
{
	if (m_pModelCom->IsFinishedAnim()) {
		m_pFSM->Change_State(FSMSTATE::COMBAT);
	}

	return S_OK;
}

void CRanrok::Behavior_IdleBreakExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLEBREAK);
	m_bLookAt = false;
}

void CRanrok::Behavior_MoveEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::MOVE);
	m_bLookAt = true;
	m_pFSM->Enable_State(FSMSTATE::JOG);
}

HRESULT CRanrok::Behavior_MoveExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (m_fTargetDistance <= 100.f && m_fTargetDistance != 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return S_OK;
}

void CRanrok::Behavior_MoveExit()
{
	m_pFSM->Disable_State(FSMSTATE::MOVE | FSMSTATE::SPRINT | FSMSTATE::JOG | FSMSTATE::WALK);
}

void CRanrok::Behavior_CombatEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::COMBAT);
	m_bLookAt = true;
}

HRESULT CRanrok::Behavior_CombatExitCheck(_float fTimeDelta)
{
	if (m_fTargetDistance > 45.f && m_fTargetDistance != 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}

	if (m_fTargetDistance <= 100.f && m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::FIREBREATH)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::FIREBREATH);
		return E_FAIL;
	}
	else if (m_fTargetDistance <= 100.f && m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::FIREBALL)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::FIREBALL);
		return E_FAIL;
	}
	else if (m_fTargetDistance <= 100.f && m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::FIRESWEEP)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::FIRESWEEP);
		return E_FAIL;
	}
	else if (m_fTargetDistance <= 100.f && m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::RUSH)] <= 0.f && m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_GROUND))
	{
		m_pFSM->Change_State(FSMSTATE::RUSH);
		return E_FAIL;
	}
	else if (m_fTargetDistance <= 15.f && m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::SWIPE)] <= 0.f && m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_GROUND))
	{
		m_pFSM->Change_State(FSMSTATE::SWIPE);
		return E_FAIL;
	}

	else if (m_fTargetDistance <= 100.f && m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::PULSE)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::PULSE);
		return E_FAIL;
	}
	else {
		if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_AIR)) {
			m_pFSM->Change_State(FSMSTATE::IDLE);
			return E_FAIL;
		}
		else if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_GROUND)) {
			m_pFSM->Change_State(FSMSTATE::GROUND);
			return E_FAIL;
		}
	}

	return S_OK;
}

void CRanrok::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT);
}

void CRanrok::Behavior_GroundEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::GROUND);
	m_bLookAt = true;
	pairAnimInfo = m_Animation[STATEANIM::IDLE_G];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrok::Behavior_GroundExitCheck(_float fTimeDelta)
{
	if (m_fTargetDistance <= 100.f && m_fTargetDistance != 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return S_OK;
}

void CRanrok::Behavior_GroundExit()
{
	m_pFSM->Disable_State(FSMSTATE::GROUND);
}

void CRanrok::Behavior_HoverEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::HOVER);
	_int iRand = m_pGameInstance->Real_Random_Int(0, 1);
	switch (iRand)
	{
	case 0:
		pairAnimInfo = m_Animation[STATEANIM::HOVER_DASH_LEFT];
		break;
	case 1:
		pairAnimInfo = m_Animation[STATEANIM::HOVER_DASH_RIGHT];
		break;
	}

	Add_Event(pairAnimInfo.first,
		[this]() {m_bHoverDash = true; },
		0.3f);

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrok::Behavior_HoverExitCheck(_float fTimeDelta)
{
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (m_bHoverDash)
	{
		if (iCurrAnimIndex == m_Animation[STATEANIM::HOVER_DASH_LEFT].first)
		{
			if (fRatio <= 0.7f)
			{
				_vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
				vRight = XMVectorSetY(vRight, 0.f);
				vRight = XMVector3Normalize(vRight);
				_vector vPos = m_pCharacter_Controller->Get_Position();
				_vector vNextPos = vPos - vRight * 5.f * fTimeDelta;
				m_pCharacter_Controller->Set_Position(vNextPos);
			}
		}
		else if (iCurrAnimIndex == m_Animation[STATEANIM::HOVER_DASH_RIGHT].first)
		{
			if (fRatio <= 0.7f)
			{
				_vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
				vRight = XMVectorSetY(vRight, 0.f);
				vRight = XMVector3Normalize(vRight);
				_vector vPos = m_pCharacter_Controller->Get_Position();
				_vector vNextPos = vPos + vRight * 5.f * fTimeDelta;
				m_pCharacter_Controller->Set_Position(vNextPos);
			}
		}
	}

	if (fRatio >= 0.95f)
	{
		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
		return E_FAIL;
	}
	return S_OK;
}

void CRanrok::Behavior_HoverExit()
{
	m_pFSM->Disable_State(FSMSTATE::HOVER);
	m_bHoverDash = false;
}

void CRanrok::Behavior_FlyEnter()
{
}

HRESULT CRanrok::Behavior_FlyExitCheck(_float fTimeDelta)
{
	return E_NOTIMPL;
}

void CRanrok::Behavior_FlyExit()
{
	m_pFSM->Disable_State(FSMSTATE::FLY);
}

void CRanrok::Behavior_FireBreathEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::FIREBREATH);
	if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_AIR))
	{
		pairAnimInfo = m_Animation[STATEANIM::FIREBREATH_WINDUP_A];

		Add_Event(pairAnimInfo.first,
			[&]() {
				pairAnimInfo = m_Animation[STATEANIM::FIREBREATH_A];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
				m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::FIREBREATH)] = m_fMaxSkillCoolTime[ENUM_CLASS(RANROK_SKILL::FIREBREATH)];
				m_bLookAt = false;
			}, 0.95f);

		Add_Event(m_Animation[STATEANIM::FIREBREATH_A].first,
			[&]() {
				pairAnimInfo = m_Animation[STATEANIM::FIREBREATH_COOLDOWN_A];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
				m_bLookAt = true;
			}, 0.95f);


		Add_Event(m_Animation[STATEANIM::FIREBREATH_A].first,
			[this]() {


				CRanrok_Breath::BREATH_INFO BreathDesc;

				BreathDesc.fTime = 1.f;
				BreathDesc.iPase = ENUM_CLASS(m_ePhase);

				m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_BREATH, this, &BreathDesc);

			}, 0.1f);

		/*Add_Event(m_Animation[STATEANIM::FIREBREATH_A].first,
			[this]() {
				m_pModelCom->Play_HeadBone(true);
			}, 0.2f);*/
	}
	else if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_GROUND))
	{
		pairAnimInfo = m_Animation[STATEANIM::FIREBREATH_G];
		m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::FIREBREATH)] = m_fMaxSkillCoolTime[ENUM_CLASS(RANROK_SKILL::FIREBREATH)];

		Add_Event(pairAnimInfo.first,
			[this]() {
				CRanrok_Breath::BREATH_INFO BreathDesc;

				BreathDesc.fTime = 2.f;
				BreathDesc.iPase = ENUM_CLASS(m_ePhase);

				m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_BREATH, this, &BreathDesc);

				m_bLookAt = false;
			}, 0.2f);

		Add_Event(pairAnimInfo.first,
			[this]() {
				m_pModelCom->Play_HeadBone(true);
			}, 0.3f);
	}
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrok::Behavior_FireBreathExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
	if (m_pModelCom->Get_PlayHeadBone())
	{
		m_fHeadAimWeight += fTimeDelta *0.3f;
		if (m_fHeadAimWeight >= 0.7f)
		{
			m_fHeadAimWeight = 0.7f;
		}
		m_pModelCom->Set_HeadAimWeight(m_fHeadAimWeight);
	}

	if (iCurrAnimIndex == m_Animation[STATEANIM::FIREBREATH_G].first && fRatio >= 0.8f)
	{
	
		m_fHeadAimWeight -= fTimeDelta*1.5f;
		if (m_fHeadAimWeight >= 0.f) {
			m_pModelCom->Set_HeadAimWeight(m_fHeadAimWeight);
		}
		else {
			m_pModelCom->Set_HeadAimWeight(0.f);
		}
	}

	if (m_pModelCom->IsFinishedAnim())
	{
		if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_AIR)) {
			m_pFSM->Change_State(FSMSTATE::HOVER);
		}
		else {
			m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
		}
		return E_FAIL;
	}

	return S_OK;
}

void CRanrok::Behavior_FireBreathExit()
{
	m_pFSM->Disable_State(FSMSTATE::FIREBREATH);
	m_pModelCom->Set_HeadAimWeight(0.f);
	//m_pModelCom->Play_HeadBone(false);
	m_fHeadAimWeight = 0.f;
}

void CRanrok::Behavior_FireSweepEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_bLookAt = true;
	m_pFSM->Enable_State(FSMSTATE::FIRESWEEP);
	if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_AIR))
	{
		pairAnimInfo = m_Animation[STATEANIM::FIRESWEEP_WINDUP_A];

		Add_Event(pairAnimInfo.first,
			[&]() { pairAnimInfo = m_Animation[STATEANIM::FIRESWEEP_A];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::FIRESWEEP)] = m_fMaxSkillCoolTime[ENUM_CLASS(RANROK_SKILL::FIRESWEEP)];
		m_bLookAt = false; },
			0.95f);

		Add_Event(m_Animation[STATEANIM::FIRESWEEP_A].first,
			[&]() { pairAnimInfo = m_Animation[STATEANIM::FIRESWEEP_COOLDOWN_A];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			},
			0.95f);

		Add_Event(m_Animation[STATEANIM::FIRESWEEP_A].first,
			[this]() {


				CRanrok_Breath::BREATH_INFO BreathDesc;

				BreathDesc.fTime = 2.5f;
				BreathDesc.iPase = ENUM_CLASS(m_ePhase);

				m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_BREATH, this, &BreathDesc);

			}, 0.08f);

		Add_Event(m_Animation[STATEANIM::FIRESWEEP_A].first,
			[this]() {
				m_pModelCom->Play_HeadBone(true);
			},
			0.2f);
	}
	else if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_GROUND))
	{
		pairAnimInfo = m_Animation[STATEANIM::FIRESWEEP_G];
		m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::FIRESWEEP)] = m_fMaxSkillCoolTime[ENUM_CLASS(RANROK_SKILL::FIRESWEEP)];

		Add_Event(m_Animation[STATEANIM::FIRESWEEP_G].first,
			[this]() {

				CRanrok_Breath::BREATH_INFO BreathDesc;

				BreathDesc.fTime = 3.f;
				BreathDesc.iPase = ENUM_CLASS(m_ePhase);

				m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_BREATH, this, &BreathDesc);
				m_bLookAt = false;
			}, 0.2f);


		Add_Event(m_Animation[STATEANIM::FIRESWEEP_G].first,
			[this]() {
				m_pModelCom->Play_HeadBone(true);
			}, 0.3f);
	}

	m_iBreathRand = (m_pGameInstance->Real_Random_Int(0, 1) == 0) ? -1 : 1;
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrok::Behavior_FireSweepExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (m_pModelCom->Get_PlayHeadBone())
	{
		m_fHeadAimWeight += fTimeDelta * 0.3f;
		if (m_fHeadAimWeight >= 0.7f)
		{
			m_fHeadAimWeight = 0.7f;
		}
		m_pModelCom->Set_HeadAimWeight(m_fHeadAimWeight);
	}

	if (iCurrAnimIndex == m_Animation[STATEANIM::FIRESWEEP_COOLDOWN_A].first || 
		iCurrAnimIndex == m_Animation[STATEANIM::FIRESWEEP_G].first && fRatio >= 0.8f)
	{
		m_fHeadAimWeight -= fTimeDelta * 1.5f;
		if (m_fHeadAimWeight >= 0.f) {
			m_pModelCom->Set_HeadAimWeight(m_fHeadAimWeight);
		}
		else {
			m_pModelCom->Set_HeadAimWeight(0.f);
		}
	}

	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
		return E_FAIL;
	}

	return S_OK;
}

void CRanrok::Behavior_FireSweepExit()
{
	m_pFSM->Disable_State(FSMSTATE::FIRESWEEP);
	m_pModelCom->Set_HeadAimWeight(0.f);
	//m_pModelCom->Play_HeadBone(false);
	m_fHeadAimWeight = 0.f;
}

void CRanrok::Behavior_FireBallEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::FIREBALL);
	if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_AIR))
	{
		_int iRand = m_pGameInstance->Real_Random_Int(0, 1);

		switch (iRand)
		{
		case 0:
			pairAnimInfo = m_Animation[STATEANIM::FIREBALL1_A];

			Add_Event(pairAnimInfo.first,
				[this]() {
					m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_FIREBALL, this);
				}, 0.38f);

			break;
		case 1:
			pairAnimInfo = m_Animation[STATEANIM::FIREBALL2_A];

			Add_Event(pairAnimInfo.first,
				[this]() {
					m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_FIREBALL, this);
				}, 0.5f);

			break;
		}

	}
	else if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_GROUND))
	{
		_int iRand = m_pGameInstance->Real_Random_Int(0, 1);

		switch (iRand)
		{
		case 0:
			pairAnimInfo = m_Animation[STATEANIM::FIREBALL1_G];

			Add_Event(pairAnimInfo.first,
				[this]() {
					m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_FIREBALL, this);
				}, 0.4f);

			break;
		case 1:
			pairAnimInfo = m_Animation[STATEANIM::FIREBALL2_G];

			Add_Event(pairAnimInfo.first,
				[this]() {
					m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_FIREBALL, this);
				}, 0.45f);
			break;
		}
	}

	m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::FIREBALL)] = m_fMaxSkillCoolTime[ENUM_CLASS(RANROK_SKILL::FIREBALL)];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrok::Behavior_FireBallExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (m_pModelCom->IsFinishedAnim())
	{
		if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_AIR))
		{
			m_pFSM->Change_State(FSMSTATE::COMBAT);
			return E_FAIL;
		}
		else if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_GROUND)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
			return E_FAIL;
		}
	}
	return S_OK;
}

void CRanrok::Behavior_FireBallExit()
{
	m_pFSM->Disable_State(FSMSTATE::FIREBALL);
}

void CRanrok::Behavior_SwipeEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::SWIPE);

	if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_GROUND))
	{
		if (m_fCross > 0.f)
		{
			pairAnimInfo = m_Animation[STATEANIM::GROUND_SWIPE_L];
		}
		else {
			pairAnimInfo = m_Animation[STATEANIM::GROUND_SWIPE_R];
		}
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,1.8f);

	}
	Add_Event(pairAnimInfo.first,
		[this]() {
			m_bMotionTrail = true; 
			m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_SWIPE , this);
		},
		0.3f);

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_bMotionTrail = false;
		},
		0.6f);

	m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::SWIPE)] = m_fMaxSkillCoolTime[ENUM_CLASS(RANROK_SKILL::SWIPE)];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrok::Behavior_SwipeExitCheck(_float fTimeDelta)
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

void CRanrok::Behavior_SwipeExit()
{
	m_pFSM->Disable_State(FSMSTATE::SWIPE);
	m_bMotionTrail = false;
}
// FireBurst 란록은 평상시에 맞지않는데 특정 구체를 파괴해서 란록한테 피해를 입히면 버스트상태가 해제되고 이제 마법에 맞는다. 그리고 특정시간이 지나면 이 파이어버스트 상태가 되면서 다시 맞지않는다.
void CRanrok::Behavior_SkillEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::SKILL);
	pairAnimInfo = m_Animation[STATEANIM::SKILL];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrok::Behavior_SkillExitCheck(_float fTimeDelta)
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::HOVER);
		return E_FAIL;
	}
	return S_OK;
}

void CRanrok::Behavior_SkillExit()
{
	m_pFSM->Disable_State(FSMSTATE::SKILL);
}
// 깨물면서 돌진
void CRanrok::Behavior_RushEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::RUSH);
	pairAnimInfo = m_Animation[STATEANIM::RUSH_LOOP];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,1.f,false,1.15f);
	m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::RUSH)] = m_fMaxSkillCoolTime[ENUM_CLASS(RANROK_SKILL::RUSH)];
	m_bLookAt = false;

	Add_Event(m_Animation[STATEANIM::RUSH_LOOP].first,
		[this]() {

			_matrix LeftBoneMat = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("wrist_left_target")) * m_pTransformCom->Get_XMWorldMatrix();

			_float4 vLeftPos;
			XMStoreFloat4(&vLeftPos, LeftBoneMat.r[3]);

			m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_RUSH_BOTTOM, this, &vLeftPos);

		}, 0.5f, true);

	Add_Event(m_Animation[STATEANIM::RUSH_LOOP].first,
		[this]() {

			_matrix RightBoneMat = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("wrist_right_target")) * m_pTransformCom->Get_XMWorldMatrix();

			_float4 vRightPos;
			XMStoreFloat4(&vRightPos, RightBoneMat.r[3]);

			m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_RUSH_BOTTOM, this, &vRightPos);

		}, 0.8f, true);

}

HRESULT CRanrok::Behavior_RushExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	m_fRushTime += fTimeDelta;
	if (m_fRushTime >= 2.5f && iCurrAnimIndex == m_Animation[STATEANIM::RUSH_LOOP].first)
	{
		if (m_fDegree < 30.f)
		{
			m_pFSM->Change_State(FSMSTATE::GROUND);
			return E_FAIL;
		}
		else if (m_fDegree < 80.f)
		{
			if (m_fCross < 0.f)
			{
				pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_45_L];
			}
			else {
				pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_45_R];
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 1.5f);
		}
		else if (m_fDegree < 150.f)
		{
			if (m_fCross < 0.f)
			{
				pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_90_L];
			}
			else {
				pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_90_R];
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 1.5f);
		}
		else {
			if (m_fCross < 0.f)
			{
				pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_180_L];
			}
			else {
				pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_180_R];
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 1.5f);
		}

	}
	else if (iCurrAnimIndex == m_Animation[STATEANIM::RUSH_LOOP].first)
	{
		_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
		vLook = XMVectorSetY(vLook, 0.f);
		vLook = XMVector3Normalize(vLook);
		_vector vPos = m_pCharacter_Controller->Get_Position();
		_vector vNextPos = vPos + vLook * 10.f * fTimeDelta;
		m_pCharacter_Controller->Set_Position(vNextPos);
		return S_OK;
	}

	if (m_pModelCom->IsFinishedAnim() && iCurrAnimIndex != m_Animation[STATEANIM::RUSH_LOOP].first)
	{
		m_pFSM->Change_State(FSMSTATE::GROUND);
		return E_FAIL;
	}



	return S_OK;
}

void CRanrok::Behavior_RushExit()
{
	m_pFSM->Disable_State(FSMSTATE::RUSH);
	m_fRushTime = 0.f;
	m_bLookAt = true;
}

void CRanrok::Behavior_PulseEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::PULSE);
	m_bLookAt = false;

	pairAnimInfo = m_Animation[STATEANIM::PULSE];
	m_fSkillCoolTime[ENUM_CLASS(RANROK_SKILL::PULSE)] = m_fMaxSkillCoolTime[ENUM_CLASS(RANROK_SKILL::PULSE)];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_CHARGE, this);
			m_pLeftSmoke->Set_Visible(false);
			m_pRightSmoke->Set_Visible(false);
			m_pBottomSmoke->Set_Visible(false);

		}, 0.01f);

	if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_AIR))
	{
		Add_Event(pairAnimInfo.first,
			[this]() {
				CameraShake(10.f, 2.f, 5.f, 1.2f);
				m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_PURSE, this);
			}, 0.39f);
	}
	else
	{
		Add_Event(pairAnimInfo.first,
			[this]() {
				CameraShake(10.f, 2.f, 5.f, 1.2f);
				m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_GROUNDPULSE, this);
			}, 0.39f);
	}

}

HRESULT CRanrok::Behavior_PulseExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (fRatio >= 0.95f)
	{
		if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_AIR)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else {
			m_pFSM->Change_State(FSMSTATE::LAND);
		}

		m_pLeftSmoke->Set_Visible(true);
		m_pRightSmoke->Set_Visible(true);
		m_pBottomSmoke->Set_Visible(true);
		return E_FAIL;
	}
	return S_OK;
}

void CRanrok::Behavior_PulseExit()
{
	m_pFSM->Disable_State(FSMSTATE::PULSE);
	m_bLookAt = true;
}

void CRanrok::Behavior_TuckedEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::TUCKED);
	m_bDisolve = true;
	pairAnimInfo = m_Animation[STATEANIM::TUCKED];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_POINT, this, nullptr, &m_pRanrok_Point);

			m_pLeftEye_Trail->Set_Visible(false);
			m_pRightEye_Trail->Set_Visible(false);

		}, 0.05f);

}

HRESULT CRanrok::Behavior_TuckedExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	if (m_fDisolveTime >= 0.25f) {
		MoveTo(fTimeDelta);
	}

	if (m_bTucked && iCurrAnimIndex != m_Animation[STATEANIM::FLY_TO_HOVER].first)
	{
		m_bDisolveReverse = true;
		m_iCurrentFlow++;
		if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_GROUND)) {
			m_pFSM->Change_State(FSMSTATE::LAND);
			return E_FAIL;
		}
		else {
			pairAnimInfo = m_Animation[STATEANIM::FLY_TO_HOVER];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
	}

	if (m_pModelCom->IsFinishedAnim() && iCurrAnimIndex == m_Animation[STATEANIM::FLY_TO_HOVER].first)
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return S_OK;
}

void CRanrok::Behavior_TuckedExit()
{
	/* 이펙트를 서서히 사라지게 하기위해 도착 이후에 나오는 이펙트를 아래로 떨굼 */
	if (m_bTucked == true)
	{
		m_pRanrok_Point->Setting_Pos(XMVectorSet(0.f, -500.f, 0.f, 1.f));
		SAFE_RELEASE(m_pRanrok_Point);
	}

	m_pLeftEye_Trail->Set_Visible(true);
	m_pRightEye_Trail->Set_Visible(true);

	m_pFSM->Disable_State(FSMSTATE::TUCKED);
	m_bTucked = false;

}

void CRanrok::Behavior_LandEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::LAND);
	_float fAnimSpeed = 1.f;
	m_pCharacter_Controller->SetGravity(true);
	if (m_pFSM->IsEnable_Previous(FSMSTATE::TUCKED)) {
		fAnimSpeed = 2.f;
	}

	pairAnimInfo = m_Animation[STATEANIM::LAND];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,1.f,false,fAnimSpeed);


	Add_Event(pairAnimInfo.first,
		[this]() {	
			m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_LAND, this);
			CameraShake(30.f, 5.f, 10.f, 1.2f); },
		0.53f);
}

HRESULT CRanrok::Behavior_LandExitCheck(_float fTimeDelta)
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::GROUND);
		return E_FAIL;
	}
	return S_OK;
}

void CRanrok::Behavior_LandExit()
{
	m_pFSM->Disable_State(FSMSTATE::LAND);
}

void CRanrok::Behavior_HitEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::HIT);

	m_bLookAt = false;

	if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_AIR))
	{
		pairAnimInfo = m_Animation[STATEANIM::HIT_BWD]; // 스몰
	}
	else if (m_ePhase == ENUM_CLASS(RANROK_PHASE::PHASE_GROUND))
	{
		pairAnimInfo = m_Animation[STATEANIM::HIT_BWD4]; // 스몰
	}

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CRanrok::Behavior_HitExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return E_FAIL;
}

void CRanrok::Behavior_HitExit()
{
	m_pFSM->Disable_State(FSMSTATE::HIT);
	m_bLookAt = true;
}

void CRanrok::Behavior_DeadEnter()
{
	m_bLookAt = false;
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::DEAD);



	pairAnimInfo = m_Animation[STATEANIM::DEAD];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);


	m_pBottomSmoke->Set_Visible(false);
	m_pLeftSmoke->Set_Visible(false);
	m_pRightSmoke->Set_Visible(false);
	m_pLeftPt->Set_Visible(false);
	m_pRightPt->Set_Visible(false);

	Add_Event(pairAnimInfo.first,
		[&]() {

			_vector vPos = m_pCharacter_Controller->Get_Position();
			_float4 vPosFloat4 = {};
			XMStoreFloat4(&vPosFloat4, vPos);

			m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_IMPACT, this, &vPosFloat4);

		}, 0.f);

	Add_Event(pairAnimInfo.first,
		[&]() {

			_matrix BoneMat = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("pinky_06_right")) * m_pTransformCom->Get_XMWorldMatrix();

			_vector vPos = BoneMat.r[3];
			_float4 vPosFloat4 = {};
			XMStoreFloat4(&vPosFloat4, vPos);
			m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_DEAD_SPLASH, this, &vPosFloat4);

		}, 0.2f);

	Add_Event(pairAnimInfo.first,
		[&]() {

			_matrix BoneMat = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("elbow_left")) * m_pTransformCom->Get_XMWorldMatrix();

			_vector vPos = BoneMat.r[3];
			_float4 vPosFloat4 = {};
			XMStoreFloat4(&vPosFloat4, vPos);

			m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_DEAD_SPLASH, this, &vPosFloat4);

		}, 0.34f);

	Add_Event(pairAnimInfo.first,
		[&]() {

			_matrix BoneMat = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("chest_Main")) * m_pTransformCom->Get_XMWorldMatrix();

			_vector vPos = BoneMat.r[3];
			_float4 vPosFloat4 = {};
			XMStoreFloat4(&vPosFloat4, vPos);

			m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_DEAD_SPLASH, this, &vPosFloat4);

		}, 0.44f);

	Add_Event(pairAnimInfo.first,
		[&]() {

			_matrix BoneMat = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("chest_Main")) * m_pTransformCom->Get_XMWorldMatrix();

			_vector vPos = BoneMat.r[3];
			_float4 vPosFloat4 = {};
			XMStoreFloat4(&vPosFloat4, vPos);

			m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_DEAD_SPLASH, this, &vPosFloat4);

		}, 0.6f);

	Add_Event(pairAnimInfo.first,
		[&]() {

			m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_DEAD_IMPACT, this);

		}, 0.8f);

}

HRESULT CRanrok::Behavior_DeadExitCheck(_float fTimeDelta)
{
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
	m_fDeadRatio = CMyTools::Saturate((fRatio - 0.8f) / 0.2f);
	if (FLT_EPSILON > fRatio) {
		return E_PENDING;
	}
	return S_OK;
}

void CRanrok::Behavior_DeadExit()
{
	m_bDead = true;
}

_bool CRanrok::IsHitStateDisabled()
{
	return 	m_pFSM->IsEnable(FSMSTATE::FIREBREATH | FSMSTATE::FIRESWEEP | FSMSTATE::FIREBALL | FSMSTATE::PULSE | FSMSTATE::SWIPE | FSMSTATE::RUSH |FSMSTATE::SKILL);
}

_bool CRanrok::IsHitSpellDisabled()
{
	if (m_eHitSpell == ENUM_CLASS(SKILL_TYPE::ANCIENT_MAGIC) ||
		m_eHitSpell == ENUM_CLASS(SKILL_TYPE::ANCIENT_MAGIC_THROW) ||
		m_eHitSpell == ENUM_CLASS(SKILL_TYPE::AVADAKEDAVRA))
	{
		return true;
	}
	return false;
}

void CRanrok::Update_BehaviorByHPRatio(ON_COLLISION_INFO* CollisionInfo)
{
	_float curr = Get_HpRatio();
	pair<_uint, _bool> pairAnimInfo = {};
	if (m_fPrevHpRatio > 0.85f && curr <= 0.85f)
	{
		pairAnimInfo = m_Animation[STATEANIM::HIT_BWD2];
		m_fPrevHpRatio = curr;

		m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_IMPACT, this, &CollisionInfo->vWorldPos);

		Add_Event(pairAnimInfo.first,
			[&]() {m_bDisolve = true; },
			0.25f);
		Add_Event(pairAnimInfo.first,
			[&]() {m_pFSM->Change_State(FSMSTATE::TUCKED); },
			0.55f);
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		return;
	}
	else if (m_fPrevHpRatio > 0.7f && curr <= 0.7f)
	{
		pairAnimInfo = m_Animation[STATEANIM::HIT_BWD2];
		m_fPrevHpRatio = curr;

		m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_IMPACT, this, &CollisionInfo->vWorldPos);

		Add_Event(pairAnimInfo.first,
			[&]() {m_bDisolve = true; },
			0.25f);
		Add_Event(pairAnimInfo.first,
			[&]() {m_pFSM->Change_State(FSMSTATE::TUCKED); },
			0.55f);
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		return;
	}
	else if (m_fPrevHpRatio > 0.5f && curr <= 0.5f)
	{
		pairAnimInfo = m_Animation[STATEANIM::HIT_BWD2];
		m_ePhase = ENUM_CLASS(RANROK_PHASE::PHASE_GROUND);
		m_fPrevHpRatio = curr;

		m_pEffectPool->Use_Skill(SKILL_TYPE::RANROK_IMPACT, this, &CollisionInfo->vWorldPos);

		Add_Event(pairAnimInfo.first,
			[&]() {m_bDisolve = true; },
			0.25f);
		Add_Event(pairAnimInfo.first,
			[&]() {m_pFSM->Change_State(FSMSTATE::TUCKED); },
			0.55f);
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		return;
	}
	m_fPrevHpRatio = curr;

}


void CRanrok::Add_FSM()
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
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_IdleBreakExitCheck(fTimedelta); };
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

	{
		CState_Ground::STATE_GROUND_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_GroundEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_GroundExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_GroundExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::GROUND, CState_Ground::Create(&Desc));
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
		CState_FireSweep::STATE_FIRESWEEP_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_FireSweepEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_FireSweepExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_FireSweepExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::FIRESWEEP, CState_FireSweep::Create(&Desc));
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
		CState_Swipe::STATE_SWIPE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_SwipeEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_SwipeExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_SwipeExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::SWIPE, CState_Swipe::Create(&Desc));
	}


	{
		CState_Skill::STATE_SKILL_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_SkillEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_SkillExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_SkillExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::SKILL, CState_Skill::Create(&Desc));
	}

	{
		CState_Rush::STATE_RUSH_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_RushEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_RushExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_RushExit(); };
		Desc.pCollisionPlayer = &m_bCollisionPlayer;
		m_States.emplace(FSMSTATE::RUSH, CState_Rush::Create(&Desc));
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

#pragma region Land
	{
		CState_Land::STATE_LAND_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_LandEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_LandExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_LandExit(); };
		m_States.emplace(FSMSTATE::LAND, CState_Land::Create(&Desc));
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
			// m_fDeadRatio = fDeadRatio;
			if (m_fDeadRatio >= 1.f) {

				PSX::PxExtendedVec3 pxControlllerPos = m_pCharacter_Controller->Get_Controller()->getPosition();
				PSX::PxTransform pxTransform((_float)pxControlllerPos.x, (_float)pxControlllerPos.y + 100.f, (_float)pxControlllerPos.z);
				m_pCharacter_Controller->Set_Position(XMLoadFloat3((_float3*)&pxTransform.p));
				m_pRigidBody->SetActive(false);
				m_pCharacter_Controller->SetActive(false);

				m_bDead = true;
			}
			};
		Desc.vDeadTimer.x = FLT_EPSILON5;
		Desc.vDeadTimer.y = 2.f;
		m_States.emplace(FSMSTATE::DEAD, CState_Dead::Create(&Desc));
	}
#pragma endregion

}
