#include "pch.h"
#include "Goblin_Assassin.h"
#include "InfoInstance.h"
#include "GameInstance.h"
#include "Player.h"
#include "Goblin_Assassin_Spector.h"
#include "EffectParts.h"

#include "EffectPool.h"
#include "Goblin_Sword.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_IdleBreak.h"
#include "State_Dodge.h"
#include "State_Jump.h"
#include "State_Land.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_Slash.h"
#include "State_Dash.h"
#include "State_Blink.h"
#include "State_Hit.h"
#include "State_Dead.h"
#include "State_Shuffle.h"
#include "State_Fear.h"
#pragma endregion


void CGoblin_Assassin::Behavior_IdleEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLE);

}

HRESULT CGoblin_Assassin::Behavior_IdleExitCheck()
{
	pair<_uint, _bool> pairAnimInfo;
	if (m_fTargetDistance <= 20.f && m_fTargetDistance > 10.f)
	{
		m_bLookAt = false;
		m_bDetection = true;

		pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::IDLE];

		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	}
	if ((m_fTargetDistance <= 10.f && m_fTargetDistance != 0.f) /*|| m_pDetection->Get_Step() == true*/)
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::MOVE);
		m_pEffectPool->Use_Skill(SKILL_TYPE::DUELIST_PROTEGO, this);
		return E_FAIL;
	}
	else if (m_fTargetDistance >= 25.f)
	{
		m_bDetection = false;

		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
		return E_FAIL;
	}

	return S_OK;
}

void CGoblin_Assassin::Behavior_IdleExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLE);
}

void CGoblin_Assassin::Behavior_IdleBreakEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLEBREAK);
	pair<_uint, _bool> pairAnimInfo;
	m_bLookAt = false;
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

HRESULT CGoblin_Assassin::Behavior_IdleBreakExitCheck()
{
	if (m_pModelCom->IsFinishedAnim()) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}

	return S_OK;
}

void CGoblin_Assassin::Behavior_IdleBreakExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLEBREAK);
}

void CGoblin_Assassin::Behavior_MoveEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::MOVE);
	m_bLookAt = true;
	m_bFirstMove = false;
}

HRESULT CGoblin_Assassin::Behavior_MoveExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (m_fTargetDistance >= 7.f)
	{
		pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	}
	else {
		if (!m_bFirstMove)
		{
			_uint Rand = m_pGameInstance->Real_Random_Int(0, 3);
			switch (Rand)
			{
			case  0:
				pairAnimInfo = m_Animation[STATEANIM::JOG_112_L];
				break;
			case  1:
				pairAnimInfo = m_Animation[STATEANIM::JOG_112_R];
				break;
			case  2:
				if (iCurrAnimIndex == m_Animation[STATEANIM::JOG_90_R].first) {
					pairAnimInfo = m_Animation[STATEANIM::JOG_112_R];
				}
				else {
					pairAnimInfo = m_Animation[STATEANIM::JOG_90_L];
				}
				break;
			case  3:
				if (iCurrAnimIndex == m_Animation[STATEANIM::JOG_90_L].first) {
					pairAnimInfo = m_Animation[STATEANIM::JOG_112_L];
				}
				else {
					pairAnimInfo = m_Animation[STATEANIM::JOG_90_R];
				}
				break;
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 0.8f, true);
			m_bFirstMove = true;
			m_fMoveTime = 0.f;
			return S_OK;
		}
		m_fMoveTime += fTimeDelta;

		if (m_fMoveTime >= 2.f)
		{
			m_bFirstMove = false;
			m_fMoveTime = 0.f;
		}
	}

	for (_uint i = 0; i < ENUM_CLASS(GOBLIN_ASSASSIN_SKILL::END); i++)
	{
		if (m_fSkillCoolTime[i] == 0.f)
		{
			m_pFSM->Change_State(FSMSTATE::COMBAT);
			return E_FAIL;
		}
	}

	return S_OK;
}


void CGoblin_Assassin::Behavior_MoveExit()
{
	m_pFSM->Disable_State(FSMSTATE::MOVE | FSMSTATE::SPRINT | FSMSTATE::JOG | FSMSTATE::WALK);
}

void CGoblin_Assassin::Behavior_CombatEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::COMBAT);
	m_bLookAt = true;
}

HRESULT CGoblin_Assassin::Behavior_CombatExitCheck(_float fTimeDelta)
{
	if (m_fTargetDistance > 25.f && m_fTargetDistance != 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}
	else if (m_fTargetDistance > 20.f && m_fTargetDistance != 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::MOVE);
		return E_FAIL;
	}

	if (m_fTargetDistance <= 7.f && m_fSkillCoolTime[ENUM_CLASS(GOBLIN_ASSASSIN_SKILL::DASH)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::DASH);
		return E_FAIL;
	}
	else if (m_fTargetDistance <= 15.f && m_fSkillCoolTime[ENUM_CLASS(GOBLIN_ASSASSIN_SKILL::TP)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::BLINK);
		return E_FAIL;
	}
	else {
		m_pFSM->Change_State(FSMSTATE::MOVE);
		return E_FAIL;
	}

	return S_OK;
}

void CGoblin_Assassin::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT);
}

void CGoblin_Assassin::Behavior_SlashEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::SLASH);

	_float fDisAmount = {1.f};

	if (m_fTargetDistance <= 4.f)
	{
		fDisAmount = 1.3f;
	}
	else if (m_fTargetDistance <= 7.f)
	{
		fDisAmount = 1.8f;
	}
	else if (m_fTargetDistance <= 10.f)
	{
		fDisAmount = 2.3f;
	}
	else if (m_fTargetDistance <= 13.f)
	{
		fDisAmount = 2.8f;
	}
	else if (m_fTargetDistance <= 16.f)
	{
		fDisAmount = 3.3f;
	}
	pairAnimInfo = m_Animation[STATEANIM::SLASH];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, fDisAmount,false,1.3f);

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pGoblinSpector->Set_Visible(true);
			m_pGoblinSpector->Spector_Trail_Visible(true);

			m_pEffectPool->Use_Skill(SKILL_TYPE::GOBLIN_ATTACK, m_pGoblinSpector->Get_PartObject<CGoblin_Sword>(), (void*)&m_pGoblinSpector->Get_PartObject<CGoblin_Sword>()->Get_SwordMatrix());
		},
		0.05f);

	Add_Event(pairAnimInfo.first,
		[this]() {
			Get_PartObject<CGoblin_Sword>("Goblin_Sword_L")->Set_CanTakeDamage(true);
			Get_PartObject<CGoblin_Sword>("Goblin_Sword_R")->Set_CanTakeDamage(true); },
		0.15f);

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pGoblinSpector->Set_Disolve(true); },
			0.28f);

	//Add_Event(pairAnimInfo.first,
	//	[this]() {
	//		m_bLookAt = true;
	//	},
	//	0.8f);

	Set_Easing(pairAnimInfo.first, 0.01f, 0.15f, 0.8f);

	Set_Easing(pairAnimInfo.first, 0.15f, 0.4f, 1.4f);
}

HRESULT CGoblin_Assassin::Behavior_SlashExitCheck(_float fTimeDelta)
{
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();



	if (fRatio >= 0.8f)
	{
		m_pGoblinSpector->Spector_Trail_Visible(false);
		Get_PartObject<CGoblin_Sword>("Goblin_Sword_L")->Set_CanTakeDamage(false);
		Get_PartObject<CGoblin_Sword>("Goblin_Sword_R")->Set_CanTakeDamage(false);
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return S_OK;
}

void CGoblin_Assassin::Behavior_SlashExit()
{
	m_pFSM->Disable_State(FSMSTATE::SLASH);
	m_bMotionTrail = false;
	m_bLookAt = true;
}

void CGoblin_Assassin::Behavior_DashEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::DASH);
	m_bMotionTrail = true;

	pairAnimInfo = m_Animation[STATEANIM::DASH];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,1.f,false,1.f,false);
	m_fSkillCoolTime[ENUM_CLASS(GOBLIN_ASSASSIN_SKILL::DASH)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_ASSASSIN_SKILL::DASH)];

	_float2 Dir8[8] =
	{
		{  0,  1 }, {  1,  1 }, {  1,  0 },
	    { -1,  0 }, { -1,  1 }, {  0, -1 },
		{  1, -1 }, { -1, -1 }
	};

	_int iIndex = m_pGameInstance->Real_Random_Int(0, 7);
	_float2 dir = Dir8[iIndex];

	_vector vRight = m_pTransformCom->Get_State(STATE::RIGHT);
	_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);

	_vector vDash = XMVector3Normalize(vRight * dir.x + vLook * dir.y);

	XMStoreFloat4(&m_vDashDir, vDash);
}

HRESULT CGoblin_Assassin::Behavior_DashExitCheck(_float fTimeDelta)
{
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
	_float4 RootMomentum = m_pModelCom->Get_RootBoneMomentum();
	_vector RootDelta = XMLoadFloat4(&RootMomentum);
	_float dashDist = XMVectorGetX(XMVector3Length(RootDelta));
	m_pTransformCom->AccumulateMomentum(XMLoadFloat4(&m_vDashDir) * dashDist * 1.5f);

	if (fRatio >= 0.7f)
	{
		m_pFSM->Change_State(FSMSTATE::SLASH);
		return E_FAIL;
	}

	return S_OK;
}

void CGoblin_Assassin::Behavior_DashExit()
{
	m_pFSM->Disable_State(FSMSTATE::DASH);
}

void CGoblin_Assassin::Behavior_BlinkEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::BLINK);

	pairAnimInfo = m_Animation[STATEANIM::BLINK_START];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
	m_bDisolve = true;


	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pEffectPool->Use_Skill(SKILL_TYPE::GOBILN_TELEPORT, this);
			Get_PartObject<CGoblin_Sword>("Goblin_Sword_L")->Set_Visible(false);
			Get_PartObject<CGoblin_Sword>("Goblin_Sword_R")->Set_Visible(false);
		}, 0.1f);
}

HRESULT CGoblin_Assassin::Behavior_BlinkExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
	if (m_fDisolveTime >= 0.8f && iCurrAnimIndex != m_Animation[STATEANIM::BLINK].first)
	{

		Get_PartObject<CGoblin_Sword>("Goblin_Sword_L")->Set_Visible(true);
		Get_PartObject<CGoblin_Sword>("Goblin_Sword_R")->Set_Visible(true);
		m_bDisolve = false;
		m_fDisolveTime = 0.f;
		m_fSkillCoolTime[ENUM_CLASS(GOBLIN_ASSASSIN_SKILL::TP)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_ASSASSIN_SKILL::TP)];
		_vector vPlayerPos = XMLoadFloat4(&m_vTargetPos);
		_vector vPlayerLook = m_pTarget->Get_Component<CTransform>()->Get_State(STATE::LOOK);
		vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);
		vPlayerLook = XMVector3Normalize(vPlayerLook);

		_float randAngleDeg = m_pGameInstance->Real_Random_Float(-60.f, 60.f);
		_float randAngleRad = XMConvertToRadians(randAngleDeg);

		_matrix rot = XMMatrixRotationY(randAngleRad);
		_vector offsetDir = XMVector3TransformNormal(vPlayerLook, rot);

		_float randDist = m_pGameInstance->Real_Random_Float(3.f, 3.5f);

		_vector vFinalPos = vPlayerPos + offsetDir * randDist;
		vFinalPos = XMVectorSetY(vFinalPos, XMVectorGetY(vPlayerPos) + 1.5f);

		m_pCharacter_Controller->Set_Position(vFinalPos);

		pairAnimInfo = m_Animation[STATEANIM::BLINK];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
	}

	if (fRatio >= 0.8f && iCurrAnimIndex == m_Animation[STATEANIM::BLINK].first)
	{
		m_bLookAt = true;
		m_fTpTime = 0.f;
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return S_OK;
}

void CGoblin_Assassin::Behavior_BlinkExit()
{
	m_pFSM->Disable_State(FSMSTATE::BLINK);
}

void CGoblin_Assassin::Behavior_ShuffleEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::SHUFFLE);
	_uint iRand = m_pGameInstance->Real_Random_Int(0, 3);
	switch (iRand)
	{
	case  0:
		pairAnimInfo = m_Animation[STATEANIM::SHUFFLE1];
		break;
	case  1:
		pairAnimInfo = m_Animation[STATEANIM::SHUFFLE2];
		break;
	case  2:
		pairAnimInfo = m_Animation[STATEANIM::SHUFFLE3];
		break;
	case  3:
		pairAnimInfo = m_Animation[STATEANIM::SHUFFLE4];
		break;
	}
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
}

HRESULT CGoblin_Assassin::Behavior_ShuffleExitCheck(_float fTimeDelta)
{
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (fRatio >= 0.3f)
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return S_OK;
}

void CGoblin_Assassin::Behavior_ShuffleExit()
{
	m_pFSM->Disable_State(FSMSTATE::SHUFFLE);
}

void CGoblin_Assassin::Behavior_FearEnter()
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

HRESULT CGoblin_Assassin::Behavior_FearExitCheck(_float fTimeDelta)
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return S_OK;
}

void CGoblin_Assassin::Behavior_FearExit()
{
	m_pFSM->Disable_State(FSMSTATE::FEAR);
}

void CGoblin_Assassin::Behavior_HitEnter()
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


}

HRESULT CGoblin_Assassin::Behavior_HitExitCheck(_float fTimeDelta)
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

void CGoblin_Assassin::Behavior_HitExit()
{
	m_bPos = false;
	m_bLookAt = true;
}

void CGoblin_Assassin::Behavior_DeadEnter()
{
	m_bLookAt = false;
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::DEAD);
	m_pCharacter_Controller->SetGravity(true);

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

	Get_PartObject<CGoblin_Sword>("Goblin_Sword_L")->Set_Disolve(true);
	Get_PartObject<CGoblin_Sword>("Goblin_Sword_R")->Set_Disolve(true);
}

HRESULT CGoblin_Assassin::Behavior_DeadExitCheck(_float fTimeDelta)
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

void CGoblin_Assassin::Behavior_DeadExit()
{
	m_bDead = true;
}

void CGoblin_Assassin::Hit_Accio(_float fTimeDelta)
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

void CGoblin_Assassin::Hit_Levioso(_float fTimeDelta)
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

void CGoblin_Assassin::Hit_Jap()
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

void CGoblin_Assassin::Descendo_Event()
{
	pair<_uint, _bool> pairAnimInfo = {};

	m_eHitSpell = ENUM_CLASS(SKILL_TYPE::END);
	m_pCharacter_Controller->SetGravity(true);
	m_pCharacter_Controller->Set_GravityAmount(2.f);
	m_fAirTime = 0.f;

	pairAnimInfo = m_Animation[STATEANIM::KNOCKDOWN_BWD];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 5.f);

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

void CGoblin_Assassin::TumbleAnim(_float fTimeDelta)
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

void CGoblin_Assassin::HitState_Behavior(_float fTimeDelta)
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


void CGoblin_Assassin::Add_FSM()
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
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_CombatExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_CombatExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::COMBAT, CState_Combat::Create(&Desc));
	}
	{
		CState_Slash::STATE_SLASH_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_SlashEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_SlashExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_SlashExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::SLASH, CState_Slash::Create(&Desc));
	}
	{
		CState_Dash::STATE_DASH_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_DashEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_DashExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_DashExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::DASH, CState_Dash::Create(&Desc));
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

	{
		CState_Shuffle::STATE_SHUFFLE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_ShuffleEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_ShuffleExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_ShuffleExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::SHUFFLE, CState_Shuffle::Create(&Desc));
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
				m_pGoblinSpector->Set_Dead();
				m_bDead = true;
			}
			};
		Desc.vDeadTimer.x = FLT_EPSILON5;
		Desc.vDeadTimer.y = 2.f;
		m_States.emplace(FSMSTATE::DEAD, CState_Dead::Create(&Desc));
	}
#pragma endregion


}
