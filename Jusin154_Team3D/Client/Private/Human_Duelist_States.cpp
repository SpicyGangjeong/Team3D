#include "pch.h"
#include "Human_Duelist.h"

#include "InfoInstance.h"

#include "GameInstance.h"
#include "Wand.h"
#include "Character_Controller.h"
#include "CallBack_Playable_Behavior.h"
#include "CallBack_Playable_HitReport.h"
#include "Effect_Container.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_IdleBreak.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_LightAttack.h"
#include "State_Spell.h"
#include "State_Shield.h"
#include "State_Hit.h"
#include "State_CutScene.h"
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
	if (false == m_pFSM->IsEnable_Previous(FSMSTATE::COMBAT))
	{
		if (m_bBattle) {
			m_pFSM->Change_State(FSMSTATE::CUTSCENE);
			return E_FAIL;
		}
	}


	if (m_fTargetDistance <= 20.f) {
		for (_uint i = 0; i < ENUM_CLASS(SKILL::END); i++)
		{
			if (m_fSkillCoolTime[i] == 0.f) {
				m_pFSM->Change_State(FSMSTATE::COMBAT);
				return E_FAIL;
			}
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
	m_pFSM->Enable_State(FSMSTATE::IDLEBREAK);
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
	if (m_fTargetDistance <= 20.f) {
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
	_float fRatio = 0.f;
	_int iRand = m_pGameInstance->Real_Random_Int(0, 3);
	switch (iRand)
	{
	case 0:
		pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK];
		fRatio = 0.12f;
		break;
	case 1:
		pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK2];
		fRatio = 0.12f;
		break;
	case 2:
		pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK3];
		fRatio = 0.18f;
		break;
	case 3:
		pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK4];
		fRatio = 0.11f;
		break;
	}
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

	Add_Event(pairAnimInfo.first,
		[this]() {_uint iIndex = 0; m_pEffectPool->Use_Skill(SKILL_TYPE::DUELIST_JAP, Get_PartObject<CWand>(), &iIndex);},
		fRatio);

	Add_Event(pairAnimInfo.first,
		[this, fRatio]() {
			_float OffsetRatio = 0.1f;
			m_pGameInstance->SlowMotion(0.2f, 0.3f); },
		0.f);

	Add_Event(pairAnimInfo.first,
		[this]() { m_pEffectPool->Use_Skill(SKILL_TYPE::DUELIST_JAPSIDE, Get_PartObject<CWand>()); 
	 },
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
	pair<_uint, _bool> pairAnimInfo;
	m_bLookAt = false;
	m_bCameraShake = true;

	switch (m_eHitSpell)
	{
	case ENUM_CLASS(SKILL_TYPE::JAP):
		if (m_eHitState != ENUM_CLASS(HIT_STATE::END)) {
			return;
		}
		pairAnimInfo = m_Animation[STATEANIM::HIT_BWD];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 0.1f);
		break;
	case ENUM_CLASS(SKILL_TYPE::LEVIOSO):
		pairAnimInfo = m_Animation[STATEANIM::HIT_LEVIOSO];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		break;
	case ENUM_CLASS(SKILL_TYPE::STUPEFY):
		pairAnimInfo = m_Animation[STATEANIM::DAZED_START];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 0.1f);
		Add_Event(pairAnimInfo.first,
			[this]() {
				pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::DAZED_LOOP];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
				m_eHitState = ENUM_CLASS(HIT_STATE::STUN); },
			0.95f);

		Add_Event(m_Animation[STATEANIM::DAZED_LOOP].first,
			[this]() {
				_string strBoneName = "HeadEnd";
				m_pEffectPool->Use_Skill(SKILL_TYPE::STUN, this, &strBoneName); },
			0.1f);

		Add_Event(m_Animation[STATEANIM::DAZED_LOOP].first,
			[this]() {
				pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::DAZED_END1];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second); },
			0.95f);
		break;
	}

	{
		Add_Event(m_Animation[STATEANIM::TUMBLE_FWD].first,
			[this]() {
				pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::KNOCKDOWN_FWD_SPLT];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
				m_pCharacter_Controller->SetGravity(true);
				m_pCharacter_Controller->Set_GravityAmount(1.4f);
				m_eHitState = ENUM_CLASS(HIT_STATE::END); },
			0.75f);

		Add_Event(m_Animation[STATEANIM::KNOCKDOWN_FWD_SPLT].first,
			[this]() {
				pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::GETUP_FWD];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			},
			0.95f);

		Add_Event(m_Animation[STATEANIM::TUMBLE].first,
			[this]() {
				pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
				m_pCharacter_Controller->SetGravity(true);
				m_pCharacter_Controller->Set_GravityAmount(1.4f);
				m_eHitState = ENUM_CLASS(HIT_STATE::END); },
			0.75f);

		Add_Event(m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT].first,
			[this]() {
				pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::GETUP_BWD];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			},
			0.95f);
	}

}

HRESULT CHuman_Duelist::Behavior_HitExitCheck(_float fTimeDelta)
{
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (iCurrAnimIndex == m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT].first || 
		iCurrAnimIndex == m_Animation[STATEANIM::KNOCKDOWN_FWD_SPLT].first)
	{
		if (true == m_pCharacter_Controller->IsOnGround() && m_bCameraShake) {
			CameraShake(10.f, 1.f, 2.f, 0.3f);
			m_bCameraShake = false;
		}
	}
	
	Hit_Levioso(fTimeDelta);

	HitState_Behavior(fTimeDelta);

	if (iCurrAnimIndex != m_Animation[STATEANIM::HIT_LEVIOSO].first) {
		if (m_pModelCom->IsFinishedAnim())
		{
			m_pFSM->Change_State(FSMSTATE::COMBAT);
			return E_FAIL;
		}
	}
	return S_OK;
}

void CHuman_Duelist::Behavior_HitExit()
{
	m_pCharacter_Controller->Reset_GravityAmount();
	m_bLookAt = true;
	m_fAirTime = 0.f;
	
}

void CHuman_Duelist::Behavior_CutSceneEnter()
{
	m_pFSM->Enable_State(FSMSTATE::CUTSCENE);
	pair<_uint, _bool> pairAnimInfo;
	pairAnimInfo = m_Animation[STATEANIM::CUTSCENE_DUEL];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CHuman_Duelist::Behavior_CutSceneExitCheck(_float fTimeDelta)
{
	if (m_pModelCom->IsFinishedAnim()) {
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return S_OK;
}

void CHuman_Duelist::Behavior_CutSceneExit()
{
	m_pFSM->Disable_State(FSMSTATE::CUTSCENE);
	//m_bBattle = false;
}

void CHuman_Duelist::Hit_Levioso(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo;
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	if (iCurrAnimIndex == m_Animation[STATEANIM::HIT_LEVIOSO].first)
	{
		_vector vDir = m_pTransformCom->Get_State(STATE::UP);
		vDir = XMVector4Normalize(vDir);
		_vector vPos = m_pCharacter_Controller->Get_Position();
		m_fAirTime += fTimeDelta;
		_vector Force = vDir * fTimeDelta;
		if (m_fAirTime < 1.3f) {
			m_pCharacter_Controller->Set_Position(vPos + Force);
			m_pCharacter_Controller->SetGravity(false);
		}
		else {
			m_pCharacter_Controller->Set_Position(vPos);
		}
		if (m_fAirTime > 2.3f)
		{
			m_eHitState = ENUM_CLASS(HIT_STATE::END);
			pairAnimInfo = m_Animation[STATEANIM::LAND];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			m_fAirTime = 0.f;
			m_pCharacter_Controller->SetGravity(true);
		}
	}
}


void CHuman_Duelist::HitState_Behavior(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	CTransform* pTransform = m_pTarget->Get_Component<CTransform>();
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (m_eHitState == ENUM_CLASS(HIT_STATE::AIR_LEVIOSO))
	{
		switch (m_eHitSpell)
		{
		case ENUM_CLASS(SKILL_TYPE::JAP):
		{
			if (iCurrAnimIndex != m_Animation[STATEANIM::TUMBLE_FWD].first) {
				pairAnimInfo = m_Animation[STATEANIM::TUMBLE_FWD];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 0.7f, false, 1.5f);
			}
			else {
				pairAnimInfo = m_Animation[STATEANIM::TUMBLE];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 0.7f, false, 1.5f);
				m_bHitJap = true;
			}
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::END);
		}
		break;
		}
	}

	if (m_eHitState == ENUM_CLASS(HIT_STATE::STUN))
	{
		switch (m_eHitSpell)
		{
		case ENUM_CLASS(SKILL_TYPE::JAP):
		{
			pairAnimInfo = m_Animation[STATEANIM::HIT_BWD];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
		break;
		case ENUM_CLASS(SKILL_TYPE::LEVIOSO):
		{
			pairAnimInfo = m_Animation[STATEANIM::HIT_LEVIOSO];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			m_fAirTime = 0.f;
		}
		break;
		}
		m_eHitSpell = ENUM_CLASS(SKILL_TYPE::END);
	}

	if (m_bHitJap) {
		m_fHitTimer += fTimeDelta;
		if (m_fHitTimer <= 0.4f)
		{
			_vector vPlayerLook = pTransform->Get_State(STATE::LOOK);
			vPlayerLook = XMVector4Normalize(vPlayerLook);
			_vector vPos = m_pCharacter_Controller->Get_Position();

			_float t = clamp(m_fHitTimer, 0.f, 1.f);
			_float ease = 1.f - (t * t);
			_vector Force = vPlayerLook * fTimeDelta * 3.f * ease;

			m_pCharacter_Controller->Set_Position(vPos + Force);
		}
		else {
			m_fHitTimer = 0.f;
			m_bHitJap = false;
		}
	}
	

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
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_HitExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_HitExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::HIT, CState_Hit::Create(&Desc));
	}

	{
		CState_CutScene::STATE_CUTSCENE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_CutSceneEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_CutSceneExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_CutSceneExit(); };
		m_States.emplace(FSMSTATE::CUTSCENE, CState_CutScene::Create(&Desc));
	}

#pragma endregion

}

