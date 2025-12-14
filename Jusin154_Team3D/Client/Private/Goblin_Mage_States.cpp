#include "pch.h"
#include "Goblin_Mage.h"

#include "GameInstance.h"
#include "EffectPool.h"
#include "Player.h"
#include "EffectParts.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_IdleBreak.h"
#include "State_Dead.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_Spell.h"
#include "State_LightAttack.h"
#include "State_Sustain.h"
#include "State_Blink.h"
#include "State_Hit.h"
#pragma endregion


void CGoblin_Mage::Behavior_IdleEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLE);
	pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::IDLE];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CGoblin_Mage::Behavior_IdleExitCheck()
{
	m_bLookAt = false;
	if (m_fTargetDistance <= 20.f && m_fTargetDistance != 0.f)
	{
		m_vOriginPos = m_pTransformCom->Get_State(STATE::POSITION);
		m_pEffectPool->Use_Skill(SKILL_TYPE::GOBLIN_PROTEGO, this);
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::MOVE);
	}
	else if (m_fTargetDistance >= 25.f)
	{
		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
	}

	return E_FAIL;
}

void CGoblin_Mage::Behavior_IdleExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLE);
}

void CGoblin_Mage::Behavior_IdleBreakEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLEBREAK);
	pair<_uint, _bool> pairAnimInfo;
	m_bLookAt = true;
	_int RandIndex = m_pGameInstance->Real_Random_Int(0, 6);
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
	case 4:
		pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK5];
		break;
	case 5:
		pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK6];
		break;
	case 6:
		pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK7];
		break;
	}
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CGoblin_Mage::Behavior_IdleBreakExitCheck()
{
	if (m_pModelCom->IsFinishedAnim()) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
	}

	return E_FAIL;
}

void CGoblin_Mage::Behavior_IdleBreakExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLEBREAK);
}

void CGoblin_Mage::Behavior_MoveEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::MOVE);

	m_pFSM->Enable_State(FSMSTATE::JOG);
}

HRESULT CGoblin_Mage::Behavior_MoveExitCheck()
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (m_fTargetDistance <= 15.f && m_fTargetDistance != 0.f) {
		m_pFSM->Change_State(FSMSTATE::COMBAT);
	}
	else {
		pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	}

	return E_FAIL;
}

void CGoblin_Mage::Behavior_MoveExit()
{
	m_pFSM->Disable_State(FSMSTATE::MOVE | FSMSTATE::SPRINT | FSMSTATE::JOG | FSMSTATE::WALK);
}

void CGoblin_Mage::Behavior_CombatEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::COMBAT);

	m_bLookAt = true;

}

HRESULT CGoblin_Mage::Behavior_CombatExitCheck(_float fTimeDelta)
{
	m_bLookAt = true;

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


	if (m_fTargetDistance <= 7.f && m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::TP)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::BLINK);
	}
	else if (m_fTargetDistance <= 10.f && m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SPELL)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::SPELL);
	}
	else if (m_fTargetDistance <= 15.f && m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::LIGHTATTACK)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::LIGHT_ATTACK);
	}
	else {
		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
	}

	return E_FAIL;
}

void CGoblin_Mage::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT);
}

void CGoblin_Mage::Behavior_SpellEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pairAnimInfo = m_Animation[STATEANIM::SPELL_CHARGE];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 1.3f);
	m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SPELL)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SPELL)];

	Add_Event(pairAnimInfo.first, [this]() {

		m_pEffectPool->Use_Skill(SKILL_TYPE::GOBLIN_MAGE_SIDE, this);

		}, 0.1f);



}

HRESULT CGoblin_Mage::Behavior_SpellExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	if (iCurrAnimIndex == m_Animation[STATEANIM::SPELL_CHARGE].first)
	{
		if (m_pModelCom->IsFinishedAnim())
		{
			pairAnimInfo = m_Animation[STATEANIM::SPELL];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 1.3f);

			Add_Event(pairAnimInfo.first, [this]() {

				m_pEffectPool->Use_Skill(SKILL_TYPE::GOBLIN_MAGE_DOWN, this);

				}, 0.1f);
		}
		return S_OK;
	}

	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return E_FAIL;
}

void CGoblin_Mage::Behavior_SpellExit()
{
	m_pFSM->Disable_State(FSMSTATE::SPELL);
}

void CGoblin_Mage::Behavior_LightAttackEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,1.f,true,1.3f);
	m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::LIGHTATTACK)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::LIGHTATTACK)];



	Add_Event(pairAnimInfo.first, [this]() {

		m_pEffectPool->Use_Skill(SKILL_TYPE::GOBLIN_MAGE_SIDE, this);

		}, 0.1f);

	Add_Event(pairAnimInfo.first, [this]() {

		m_pEffectPool->Use_Skill(SKILL_TYPE::GOBLIN_MAGE_ATTACK, this);

		}, 0.4f);
}

HRESULT CGoblin_Mage::Behavior_LightAttackExitCheck(_float fTimeDelta)
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return E_FAIL;
}

void CGoblin_Mage::Behavior_LightAttackExit()
{
	m_pFSM->Disable_State(FSMSTATE::LIGHT_ATTACK);
}

void CGoblin_Mage::Behavior_SustainEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pairAnimInfo = m_Animation[STATEANIM::SUSTAIN_START];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SUSTAIN)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SUSTAIN)];
}

HRESULT CGoblin_Mage::Behavior_SustainExitCheck(_float fTimeDelta)
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return E_FAIL;
}

void CGoblin_Mage::Behavior_SustainExit()
{
	m_pFSM->Disable_State(FSMSTATE::SUSTAIN);
}

void CGoblin_Mage::Behavior_BlinkEnter()
{

	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::BLINK);

	pairAnimInfo = m_Animation[STATEANIM::BLINK_START];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_bVisible = false;
			m_pGoblin_Orb->Set_Visible(false);
			m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::TP)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::TP)];
			_vector vPos = Get_WorldPostion();
			_vector vLook =m_pTransformCom->Get_State(STATE::LOOK);
			vLook = XMVectorSetY(vLook, 0.f);
			vLook = XMVector3Normalize(vLook);

			_float randAngleDeg = m_pGameInstance->Real_Random_Float(-60.f, 60.f);
			_float randAngleRad = XMConvertToRadians(randAngleDeg);

			_matrix rot = XMMatrixRotationY(randAngleRad);
			_vector offsetDir = XMVector3TransformNormal(vLook, rot);

			_float randDist = m_pGameInstance->Real_Random_Float(-5.f, -3.f);

			_vector vFinalPos = vPos + offsetDir * randDist;
			vFinalPos = XMVectorSetY(vFinalPos, XMVectorGetY(vPos) + 1.f);

			m_pCharacter_Controller->Set_Position(vFinalPos); }
	, 0.99f);

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pEffectPool->Use_Skill(SKILL_TYPE::GOBILN_TELEPORT, this);
		}, 0.1f);
}

HRESULT CGoblin_Mage::Behavior_BlinkExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	m_fTpTime += fTimeDelta;
	if (m_fTpTime >= 1.5f)
	{
		m_bVisible = true;
		m_pGoblin_Orb->Set_Visible(true);
		m_fTpTime = 0.f;
		pairAnimInfo = m_Animation[STATEANIM::BLINK];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
	}

	if (m_pModelCom->IsFinishedAnim() && iCurrAnimIndex == m_Animation[STATEANIM::BLINK].first)
	{
		m_bLookAt = true;
		m_fTpTime = 0.f;
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return E_FAIL;
}

void CGoblin_Mage::Behavior_BlinkExit()
{
	m_pFSM->Disable_State(FSMSTATE::BLINK);
}

void CGoblin_Mage::Behavior_HitEnter()
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
		pairAnimInfo = m_Animation[STATEANIM::STUMBLE_BWD_R];
		break;
	case ENUM_CLASS(SKILL_TYPE::JAP):
	{
		if (m_fHitDegree >= 90.f)
		{
			_int iRandIndex = m_pGameInstance->Real_Random_Int(0, 5);
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
		else{
			pairAnimInfo = m_Animation[STATEANIM::HIT_FWD];
		}
	}
	break;
	case ENUM_CLASS(SKILL_TYPE::LEVIOSO):
		pairAnimInfo = m_Animation[STATEANIM::HIT_LEVIOSO];
		break;
	}

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CGoblin_Mage::Behavior_HitExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (iCurrAnimIndex == m_Animation[STATEANIM::KNOCKDOWN_FWD].first)
	{
		if (m_pModelCom->Get_CurrentTrackProgressRatio() >= 0.5f)
		{
			pairAnimInfo = m_Animation[STATEANIM::KNOCKDOWN_FWD_SPLT];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f);
		}
	}

	if (iCurrAnimIndex == m_Animation[STATEANIM::HIT_LEVIOSO].first)
	{
		_vector vDir = m_pTransformCom->Get_State(STATE::UP);
		vDir = XMVector4Normalize(vDir);
		_vector vPos = m_pCharacter_Controller->Get_Position();
		m_fAirTime += fTimeDelta;
		_vector Force = vDir * fTimeDelta;
		if (m_fAirTime < 1.3f) {
			m_pCharacter_Controller->Set_Position(vPos + Force);
		}
		else {
			m_pCharacter_Controller->Set_Position(vPos);
		}
		m_pCharacter_Controller->SetGravity(false);

		if (m_pModelCom->Get_CurrentTrackProgressRatio() >= 0.31f)
		{
			m_fAirTime = 0.f;
			m_pCharacter_Controller->SetGravity(true);
			pairAnimInfo = m_Animation[STATEANIM::LAND];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f);
		}
	}


	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}
	return E_FAIL;
}

void CGoblin_Mage::Behavior_HitExit()
{
	m_pFSM->Disable_State(FSMSTATE::HIT);
}

void CGoblin_Mage::Behavior_DeadEnter()
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

HRESULT CGoblin_Mage::Behavior_DeadExitCheck(_float fTimeDelta)
{
	if (FLT_EPSILON > m_pModelCom->Get_CurrentTrackProgressRatio()) {
		return E_PENDING;
	}
	return S_OK;
}

void CGoblin_Mage::Behavior_DeadExit()
{
	m_bDead = true;
}

void CGoblin_Mage::Add_FSM()
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

	{
		CState_Spell::STATE_SPELL_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_SpellEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_SpellExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_SpellExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::SPELL, CState_Spell::Create(&Desc));
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
		CState_Sustain::STATE_SUSTAIN_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_SustainEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_SustainExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_SustainExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::SUSTAIN, CState_Sustain::Create(&Desc));
	}

	{
		CState_Blink::STATE_BLINK_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_BlinkEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_BlinkExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_BlinkExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::BLINK, CState_Blink::Create(&Desc));
	}

#pragma endregion
#pragma region Behavior_Combat_Focus
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
}

void CGoblin_Mage::Set_Anim()
{
	m_Animation[STATEANIM::IDLE] = { 22,true };
	m_Animation[STATEANIM::IDLE_TURN_BWD] = { 24, false };
	m_Animation[STATEANIM::IDLE_TURN_L] = { 26, false };
	m_Animation[STATEANIM::IDLE_TURN_R] = { 30, false };

	m_Animation[STATEANIM::IDLE_BREAK1] = { 137, false };
	m_Animation[STATEANIM::IDLE_BREAK2] = { 138, false };
	m_Animation[STATEANIM::IDLE_BREAK3] = { 139, false };
	m_Animation[STATEANIM::IDLE_BREAK4] = { 37, false };
	m_Animation[STATEANIM::IDLE_BREAK5] = { 38, false };
	m_Animation[STATEANIM::IDLE_BREAK6] = { 39, false };
	m_Animation[STATEANIM::IDLE_BREAK7] = { 41, false };

	m_Animation[STATEANIM::WALK_FWD] = { 45,true };
	m_Animation[STATEANIM::WALK_START] = { 57,false };
	m_Animation[STATEANIM::WALK_STOP] = { 58,false };
	m_Animation[STATEANIM::WALK_BWD] = { 252, true };

	m_Animation[STATEANIM::JOG_FWD] = { 147,true }; // 147
	m_Animation[STATEANIM::JOG_START] = { 157,false };
	m_Animation[STATEANIM::JOG_BWD] = { 146,false };

	m_Animation[STATEANIM::SHUFFLE1] = { 203,false };
	m_Animation[STATEANIM::SHUFFLE2] = { 204,false };
	m_Animation[STATEANIM::SHUFFLE3] = { 205,false };
	m_Animation[STATEANIM::SHUFFLE4] = { 206,false };

	m_Animation[STATEANIM::DODGE_LEFT] = { 125, false };
	m_Animation[STATEANIM::DODGE_RIGHT] = { 126, false };
	m_Animation[STATEANIM::SPRINT] = { 90, true };

	m_Animation[STATEANIM::BLINK_START] = { 428,false };
	m_Animation[STATEANIM::BLINK] = { 426,false }; // 텔레포트


	m_Animation[STATEANIM::SPELL_CHARGE] = { 437,false };
	m_Animation[STATEANIM::SPELL] = { 435,false };
	m_Animation[STATEANIM::LIGHT_ATTACK] = { 456,false }; // 평타 번개쏘기

	m_Animation[STATEANIM::SUSTAIN_START] = { 453,false };
	m_Animation[STATEANIM::SUSTAIN_LOOP] = { 454,true };
	m_Animation[STATEANIM::SUSTAIN_END] = { 455,false };

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
	m_Animation[STATEANIM::TUMBLE2] = { 380, true };// 원래 true 였음
	m_Animation[STATEANIM::PETRIFICUSED_START] = { 383, false };

	m_Animation[STATEANIM::LAND] = { 376, false };
	m_Animation[STATEANIM::HIT_LEVIOSO] = { 381, false };

	m_Animation[STATEANIM::STUMBLE_BWD_L] = { 394, false };
	m_Animation[STATEANIM::STUMBLE_BWD_R] = { 395, false };
	m_Animation[STATEANIM::STUMBLE_FWD] = { 396, false };

	m_Animation[STATEANIM::DEAD_BWD] = { 317, false };
	m_Animation[STATEANIM::DEAD_BWD2] = { 318, false };
	m_Animation[STATEANIM::DEAD_FWD] = { 319, false };
	m_Animation[STATEANIM::DEAD_FWD2] = { 320, false };
	m_Animation[STATEANIM::DEAD_L] = { 321, false };
	m_Animation[STATEANIM::DEAD_L2] = { 322, false };
	m_Animation[STATEANIM::DEAD_R] = { 323, false };
	m_Animation[STATEANIM::DEAD_R2] = { 324, false };

	// Range 올려치기 어택 443
	// 내려찍기 공격 시작모션 437
	// 내려찍기 공격 435

	// 쉴드 블럭

	// 겁먹기 327 328




	// idle break 137


	// 453 레이저빔 End
	// 454 레이저빔 Loop
	// 455 레이저빔 start

	//137 139

}
