#include "pch.h"
#include "Goblin_Mage.h"

#include "GameInstance.h"
#include "EffectPool.h"
#include "Player.h"
#include "EffectParts.h"
#include "InfoInstance.h"

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
#include "State_Fear.h"
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
		m_pEffectPool->Use_Skill(SKILL_TYPE::GOBLIN_PROTEGO, this);
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::MOVE);
		return E_FAIL;
	}
	else if (m_fTargetDistance >= 25.f)
	{
		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
		return E_FAIL;
	}

	return S_OK;
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
		return E_FAIL;
	}

	return S_OK;
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
		return E_FAIL;
	}
	else {
		pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	}

	return S_OK;
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
		return E_FAIL;
	}
	else if (m_fTargetDistance <= 10.f && m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SPELL)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::SPELL);
		return E_FAIL;
	}
	else if (m_fTargetDistance <= 15.f && m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::LIGHTATTACK)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::LIGHT_ATTACK);
		return E_FAIL;
	}
	else {
		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
		return E_FAIL;
	}

	return S_OK;
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

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pGameInstance->Sound_Play(SOUND::SD_KIND::GOBLIN_84, SD_CHANNEL_GROUP::ENEMY, false, 0.5f); },
			0.5f);


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

			Add_Event(pairAnimInfo.first,
				[this]() {
					m_pGameInstance->Sound_Play(SOUND::SD_KIND::GOBLIN_25, SD_CHANNEL_GROUP::ENEMY, false, 0.8f); },
					0.1f);
		}
		return S_OK;
	}

	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return S_OK;
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

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pGameInstance->Sound_Play(SOUND::SD_KIND::GOBLIN_52, SD_CHANNEL_GROUP::ENEMY, false, 0.8f); },
			0.14f);
	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pGameInstance->Sound_Play(SOUND::SD_KIND::GOBLIN_1, SD_CHANNEL_GROUP::ENEMY, false, 0.8f); },
			0.4f);

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pGameInstance->Sound_Play(SOUND::SD_KIND::VOICE_GOBLIN_11, SD_CHANNEL_GROUP::ENEMY, false, 0.8f); },
			0.4f);
}

HRESULT CGoblin_Mage::Behavior_LightAttackExitCheck(_float fTimeDelta)
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return S_OK;
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
	return S_OK;
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
	m_bDisolve = true;

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pEffectPool->Use_Skill(SKILL_TYPE::GOBILN_TELEPORT, this);
		}, 0.1f);

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pGameInstance->Sound_Play(SOUND::SD_KIND::GOBLIN_132, SD_CHANNEL_GROUP::ENEMY, false, 0.3f); },
			0.6f);
}

HRESULT CGoblin_Mage::Behavior_BlinkExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	if (m_fDisolveTime >= 0.8f && iCurrAnimIndex != m_Animation[STATEANIM::BLINK].first)
	{
		m_bDisolve = false;
		m_fDisolveTime = 0.f;
		m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::TP)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::TP)];
		_vector vPlayerPos = XMLoadFloat4(&m_vTargetPos);
		_vector vPlayerLook = m_pTarget->Get_Component<CTransform>()->Get_State(STATE::LOOK);
		vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);
		vPlayerLook = XMVector3Normalize(vPlayerLook);

		_float randAngleDeg = m_pGameInstance->Real_Random_Float(-60.f, 60.f);
		_float randAngleRad = XMConvertToRadians(randAngleDeg);

		_matrix rot = XMMatrixRotationY(randAngleRad);
		_vector offsetDir = XMVector3TransformNormal(vPlayerLook, rot);

		_float randDist = m_pGameInstance->Real_Random_Float(-5.f, -3.f);

		_vector vFinalPos = vPlayerPos + offsetDir * randDist;
		vFinalPos = XMVectorSetY(vFinalPos, XMVectorGetY(vPlayerPos) + 1.5f);

		m_pCharacter_Controller->Set_Position(vFinalPos);

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

	return S_OK;
}

void CGoblin_Mage::Behavior_BlinkExit()
{
	m_pFSM->Disable_State(FSMSTATE::BLINK);
}
void CGoblin_Mage::Behavior_FearEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::FEAR);

	_uint iRand = m_pGameInstance->Real_Random_Int(0, 1);
	switch (iRand)
	{
	case  0:
		pairAnimInfo = m_Animation[STATEANIM::FEAR_L];
		break;
	case  1:
		pairAnimInfo = m_Animation[STATEANIM::FEAR_R];
		break;
	}
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
}

HRESULT CGoblin_Mage::Behavior_FearExitCheck(_float fTimeDelta)
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return S_OK;
}

void CGoblin_Mage::Behavior_FearExit()
{
	m_pFSM->Disable_State(FSMSTATE::FEAR);
}

void CGoblin_Mage::Behavior_HitEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	m_pCharacter_Controller->Reset_GravityAmount();
	m_fGravityAmount = 0.f;
	m_fAirTime = 0.f;
	m_bLookAt = false;
	m_bCameraShake = true;
	m_bAir = false;

	_float fAnimSpeed = 1.f;
	switch (m_eHitSpell)
	{
	case ENUM_CLASS(SKILL_TYPE::DESCENDO):
	{
		Descendo_Event();
	}
	break;
	case ENUM_CLASS(SKILL_TYPE::BOMBARDA):
		if (m_eHitState != ENUM_CLASS(HIT_STATE::END)) {
			return;
		}
		pairAnimInfo = m_Animation[STATEANIM::STUMBLE_BWD_L];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, fAnimSpeed);
		break;
	case ENUM_CLASS(SKILL_TYPE::JAP):
	{
		Hit_Jap();
	}
	break;
	case ENUM_CLASS(SKILL_TYPE::LEVIOSO):
		if (m_eHitState != ENUM_CLASS(HIT_STATE::AIR_LEVIOSO)) {
			return;
		}
		pairAnimInfo = m_Animation[STATEANIM::HIT_LEVIOSO];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, fAnimSpeed);
		break;
	case ENUM_CLASS(SKILL_TYPE::ACCIO):
		if (m_eHitState != ENUM_CLASS(HIT_STATE::AIR_LEVIOSO)) {
			return;
		}
		pairAnimInfo = m_Animation[STATEANIM::INCARCEROUS];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, fAnimSpeed);
		break;
	default:
		_int iRand = m_pGameInstance->Real_Random_Int(0, 1);
		if (iRand == 0)
			pairAnimInfo = m_Animation[STATEANIM::STUMBLE_BWD_L];
		else
			pairAnimInfo = m_Animation[STATEANIM::STUMBLE_BWD_R];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, fAnimSpeed);
		break;
	}

	_uint iAnimIndex = m_pModelCom->Get_AnimIndex();

	if (0 == m_pGameInstance->Real_Random_Int(0, 1))
	{
		Add_Event(iAnimIndex,
			[this]() {
				m_pGameInstance->Sound_Play(SOUND::SD_KIND::VOICE_GOBLIN_10, SD_CHANNEL_GROUP::ENEMY, false, 0.6f);
			}, 0.1f);
	}
	else
	{
		Add_Event(iAnimIndex,
			[this]() {
				m_pGameInstance->Sound_Play(SOUND::SD_KIND::VOICE_GOBLIN_7, SD_CHANNEL_GROUP::ENEMY, false, 0.6f);
			}, 0.1f);
	}
}

HRESULT CGoblin_Mage::Behavior_HitExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (m_bAir)
	{
		_vector vDir = m_pTransformCom->Get_State(STATE::UP);
		vDir = XMVector4Normalize(vDir);
		_vector vPos = m_pCharacter_Controller->Get_Position();
		_vector Force = vDir * fTimeDelta * 0.6f;
		m_pCharacter_Controller->Set_Position(vPos + Force);
	}

	if (!m_bAir && iCurrAnimIndex == m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT].first)
	{
		m_fGravityAmount += fTimeDelta * fRatio * 50.f;
		m_pCharacter_Controller->Set_GravityAmount(m_fGravityAmount);
	}

	Hit_Accio(fTimeDelta);

	Hit_Levioso(fTimeDelta);

	HitState_Behavior(fTimeDelta);

	TumbleAnim(fTimeDelta);

	if (iCurrAnimIndex == m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT].first)
	{
		if (true == m_pCharacter_Controller->IsOnGround() && m_bCameraShake) {
			_int iRand = m_pGameInstance->Real_Random_Int(0, 2);
			switch (iRand)
			{
			case 0:
				m_pGameInstance->Sound_Play(SOUND::SD_KIND::TROLL_14, SD_CHANNEL_GROUP::EFFECT, false, 0.7f);
				break;
			case 1:
				m_pGameInstance->Sound_Play(SOUND::SD_KIND::TROLL_15, SD_CHANNEL_GROUP::EFFECT, false, 0.7f);
				break;
			case 2:
				m_pGameInstance->Sound_Play(SOUND::SD_KIND::TROLL_20, SD_CHANNEL_GROUP::EFFECT, false, 0.7f);
				break;
			}
			CameraShake(10.f, 1.f, 2.f, 0.3f);
			m_bCameraShake = false;
		}
	}

	if (m_pModelCom->IsFinishedAnim() && iCurrAnimIndex != m_Animation[STATEANIM::TUMBLE].first)
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return S_OK;
}

void CGoblin_Mage::Behavior_HitExit()
{
	m_bPos = false;
	m_bLookAt = true;
}

void CGoblin_Mage::Behavior_DeadEnter()
{
	m_bLookAt = false;
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::DEAD);
	m_pCharacter_Controller->SetGravity(true);
	m_pInfoInstance->Deregist_ActiveMonster(this);

	_bool bStrongerKnockDown = { false };

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

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pGameInstance->Sound_Play(SOUND::SD_KIND::VOICE_GOBLIN_12, SD_CHANNEL_GROUP::ENEMY, false, 0.4f);
		}, 0.1f);
}

HRESULT CGoblin_Mage::Behavior_DeadExitCheck(_float fTimeDelta)
{
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (fRatio >= 0.8f)
	{
		m_fDeadRatio += fTimeDelta / 3.f;
		m_fDeadRatio = min(1.f, m_fDeadRatio);
	}
	else
	{
		m_fDeadRatio = 0.f;
	}
	return S_OK;
}

void CGoblin_Mage::Behavior_DeadExit()
{
	m_bDead = true;
}

void CGoblin_Mage::Hit_Accio(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	CTransform* pTransform = m_pTarget->Get_Component<CTransform>();
	if (m_eHitSpell == ENUM_CLASS(SKILL_TYPE::ACCIO))
	{
		if (!m_bPos)
		{
			m_pCharacter_Controller->SetGravity(false);
			_vector vPlayerPos = pTransform->Get_State(STATE::POSITION);
			vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + 2.5f);
			_vector vMonsterPos = m_pCharacter_Controller->Get_Position();

			_vector vDir = XMVector3Normalize(vPlayerPos - vMonsterPos);
			_float fLength = XMVectorGetX(XMVector3Length(vPlayerPos - vMonsterPos));

			m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fAccel;

			if (fLength >= 4.f)
			{
				m_pCharacter_Controller->Set_Position(
					vMonsterPos + vDir * m_fSpeed * fTimeDelta
				);
			}
			else {
				m_bPos = true;
				pairAnimInfo = m_Animation[STATEANIM::HIT_LEVIOSO];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f);
			}
		}
	}

}

void CGoblin_Mage::Hit_Levioso(_float fTimeDelta)
{
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	if (m_eHitSpell == ENUM_CLASS(SKILL_TYPE::LEVIOSO))
	{
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
		}
	}
}

void CGoblin_Mage::Hit_Jap()
{
	pair<_uint, _bool> pairAnimInfo = {};
	if (m_eHitState != ENUM_CLASS(HIT_STATE::END)) {
		return;
	}
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
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false);
}

void CGoblin_Mage::Descendo_Event()
{
	pair<_uint, _bool> pairAnimInfo = {};

	m_eHitSpell = ENUM_CLASS(SKILL_TYPE::END);
	m_pCharacter_Controller->SetGravity(true);
	m_pCharacter_Controller->Set_GravityAmount(2.f);
	m_fAirTime = 0.f;

	pairAnimInfo = m_Animation[STATEANIM::KNOCKDOWN_BWD];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 3.f);

	Add_Event(m_Animation[STATEANIM::KNOCKDOWN_BWD].first,
		[this]() {			pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 2.f);
		},
		0.15f);

	Add_Event(m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT].first,
		[this]() {CameraShake(10.f, 1.f, 2.f, 0.3f);
	m_bCameraShake = false; }
	, 0.27f);

	Add_Event(m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT].first,
		[this]() {			pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::TUMBLE_FWD];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 0.5f);
	m_pCharacter_Controller->SetGravity(false);
	m_bAir = true;
		},
		0.3f);

	Add_Event(m_Animation[STATEANIM::TUMBLE_FWD].first,
		[this]() {m_pModelCom->Set_AnimSpeed(1.5f); },
		0.65f);

	Add_Event(m_Animation[STATEANIM::TUMBLE_FWD].first,
		[this]() {			pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	m_bAir = false;
	m_pCharacter_Controller->SetGravity(true);
	m_eHitState = ENUM_CLASS(HIT_STATE::END);
	m_pCharacter_Controller->Reset_GravityAmount();
	m_fGravityAmount = 0.f;
	m_bCameraShake = true;
		},
		0.85f);

	Add_Event(m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT].first,
		[this]() {			pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::GETUP_BWD];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f);
		},
		0.95f);

	Add_Event(m_Animation[STATEANIM::GETUP_BWD].first,
		[this]() {m_bLookAt = true; },
		0.7f);

}

void CGoblin_Mage::TumbleAnim(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (iCurrAnimIndex == m_Animation[STATEANIM::TUMBLE].first)
	{
		m_fTumbleTimer += fTimeDelta;

		if (m_fTumbleTimer >= 1.5f)
		{
			if (fRatio >= 0.85f)
			{
				m_fTumbleTimer = 0.f;
				pairAnimInfo = m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
				m_pCharacter_Controller->SetGravity(true);
				m_eHitSpell = ENUM_CLASS(SKILL_TYPE::END);
				m_eHitState = ENUM_CLASS(HIT_STATE::END);


				Add_Event(m_Animation[STATEANIM::KNOCKDOWN_BWD_SPLT].first,
					[this]() {			pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::GETUP_BWD];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f);
				m_fAirTime = 0.f;
				m_bAir = false;
					},
					0.95f);
			}
		}
	}
}

void CGoblin_Mage::HitState_Behavior(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	CTransform* pTransform = m_pTarget->Get_Component<CTransform>();
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (m_eHitState == ENUM_CLASS(HIT_STATE::AIR_LEVIOSO))
	{
		switch (m_eHitSpell)
		{
		case  ENUM_CLASS(SKILL_TYPE::DESCENDO):
		{
			Descendo_Event();
		}
		break;
		case  ENUM_CLASS(SKILL_TYPE::BOMBARDA):
		{
			m_eHitSpell = ENUM_CLASS(SKILL_TYPE::END);
			m_pCharacter_Controller->ConvertToDO(*m_pRigidBody);
			_vector vCameraLook = m_pGameInstance->Get_CameraLook();
			vCameraLook = XMVector3Normalize(vCameraLook);
			m_pRigidBody->Add_Force(vCameraLook * 60.f, PSX::PxForceMode::eIMPULSE);
			m_fAirTime = 0.f;
		}
		break;
		case ENUM_CLASS(SKILL_TYPE::JAP):
		{
			m_fHitTimer += fTimeDelta;
			if (m_fHitTimer <= 0.4f)
			{
				_vector vPlayerLook = pTransform->Get_State(STATE::LOOK);
				vPlayerLook = XMVector4Normalize(vPlayerLook);
				_vector vPos = m_pCharacter_Controller->Get_Position();

				_float t = clamp(m_fHitTimer, 0.f, 1.f);
				_float ease = 1.f - (t * t);
				_vector Force = vPlayerLook * fTimeDelta * 6.f * ease;

				m_pCharacter_Controller->Set_Position(vPos + Force);
				pairAnimInfo = m_Animation[STATEANIM::TUMBLE];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 0.6f, false, 0.8f);
			}
		}
		break;
		}
		if (fRatio >= 0.31f && iCurrAnimIndex == m_Animation[STATEANIM::HIT_LEVIOSO].first)
		{
			m_fAirTime = 0.f;
			m_pCharacter_Controller->SetGravity(true);
			pairAnimInfo = m_Animation[STATEANIM::LAND];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f);
			m_eHitState = ENUM_CLASS(HIT_STATE::END);
		}
	}
	if (m_eHitState == ENUM_CLASS(HIT_STATE::DESCENDO))
	{
		switch (m_eHitSpell)
		{
		case ENUM_CLASS(SKILL_TYPE::JAP):
		{
			m_fHitTimer += fTimeDelta;
			if (m_fHitTimer <= 0.4f)
			{
				_vector vPlayerLook = pTransform->Get_State(STATE::LOOK);
				vPlayerLook = XMVector4Normalize(vPlayerLook);
				_vector vPos = m_pCharacter_Controller->Get_Position();

				_float t = clamp(m_fHitTimer, 0.f, 1.f);
				_float ease = 1.f - (t * t);
				_vector Force = vPlayerLook * fTimeDelta * 6.f * ease;

				m_pCharacter_Controller->Set_Position(vPos + Force);
				pairAnimInfo = m_Animation[STATEANIM::TUMBLE];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 0.8f);
			}
		}
		break;
		}
	}
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

	{
		CState_Fear::STATE_FEAR_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_FearEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_FearExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_FearExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::FEAR, CState_Fear::Create(&Desc));
	}


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

#pragma region Behavior_Combat_Focus
	{
		CState_Dead::STATE_DEAD_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_DeadEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_DeadExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_DeadExit(); };
		Desc.funcLateUpdate = [this](_float fDeadRatio) {
			//m_fDeadRatio = fDeadRatio;
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

