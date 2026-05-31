#include "pch.h"
#include "Goblin.h"
#include "InfoInstance.h"
#include "GameInstance.h"
#include "Player.h"
#include "Goblin_Dagger.h"
#include "EditEffect.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Dodge.h"
#include "State_Jump.h"
#include "State_Land.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_Swing.h"
#include "State_Throw.h"
#include "State_Blink.h"
#include "State_Hit.h"
#include "State_Dead.h"
#include "State_Shuffle.h"
#pragma endregion


void CGoblin::Behavior_IdleEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLE);
	pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::IDLE];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CGoblin::Behavior_IdleExitCheck()
{
	if (m_fTargetDistance <= 20.f && m_fTargetDistance != 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::MOVE);
	}

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
	m_bLookAt = true;
	m_pFSM->Enable_State(FSMSTATE::JOG);
}

HRESULT CGoblin::Behavior_MoveExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();


	if (m_fTargetDistance >= 6.f)
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
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 0.5f, true);

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

	if (m_fTargetDistance <= 15.f && m_fTargetDistance >= 3.f && m_fTargetDistance != 0.f)
		m_pFSM->Change_State(FSMSTATE::COMBAT);

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
}

HRESULT CGoblin::Behavior_CombatExitCheck(_float fTimeDelta)
{
	m_bLookAt = true;

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

	if (m_fTargetDistance <= 6.f && m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SWING)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::SWING);
	}
	else if (m_fTargetDistance >= 7.f && m_fTargetDistance <= 12.f && m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::THROW)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::SKILL);
	}
	else if (m_fTargetDistance <= 15.f && m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::TP)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::BLINK);
	}
	else {
		m_pFSM->Change_State(FSMSTATE::MOVE);
	}

	return E_FAIL;
}

void CGoblin::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT);
}

void CGoblin::Behavior_SwingEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::SWING);

	pairAnimInfo = m_Animation[STATEANIM::SWING_FWD];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
	m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SWING)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::SWING)];

	Add_Event(pairAnimInfo.first,
		[this]() {m_bStep = false; },
		0.2f);

	Add_Event(pairAnimInfo.first,
		[this]() {m_bSwing = true; },
		0.05f);

	Add_Event(pairAnimInfo.first,
		[&]() {	m_bLookAt = true;
	m_bStep = true;
	m_bSwing = false;
	m_pFSM->Change_State(FSMSTATE::SHUFFLE); },
		0.45f);

	
}

HRESULT CGoblin::Behavior_SwingExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	if (m_pFSM->IsEnable(FSMSTATE::SWING) && m_bStep)
	{
		_float fDesiredRange = 2.f;
		_float dist = m_fTargetDistance;

		if (dist > fDesiredRange)
		{
			_vector vPos = Get_WorldPostion();
			_vector vDistance = XMLoadFloat4(&m_vTargetPos) - vPos;
			vDistance = XMVector4Normalize(vDistance);
			_float step = dist - fDesiredRange;

			m_pTransformCom->AccumulateVelocity(vDistance * step * fTimeDelta * 1.5f);
		}
	}

	return E_FAIL;
}

void CGoblin::Behavior_SwingExit()
{
	m_pFSM->Disable_State(FSMSTATE::SWING);
}

void CGoblin::Behavior_ThrowEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::SKILL);

	pairAnimInfo = m_Animation[STATEANIM::SKILL];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
	m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::THROW)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::THROW)];

	Add_Event(pairAnimInfo.first,
		[this]() {
			Get_PartObject<CGoblin_Dagger>()->Set_Visible(true);
			Get_PartObject<CGoblin_Dagger>()->Set_Attach(false); },
		0.3f);
}

HRESULT CGoblin::Behavior_ThrowExitCheck(_float fTimeDelta)
{
	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_bStep = true;
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return E_FAIL;
}

void CGoblin::Behavior_ThrowExit()
{
	m_pFSM->Disable_State(FSMSTATE::SKILL);
	Get_PartObject<CGoblin_Dagger>()->Set_Attach(true);
	Get_PartObject<CGoblin_Dagger>()->Set_Visible(false);
}

void CGoblin::Behavior_BlinkEnter()
{

	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::BLINK);

	pairAnimInfo = m_Animation[STATEANIM::BLINK_START];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_bVisible = false;
			m_fSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::TP)] = m_fMaxSkillCoolTime[ENUM_CLASS(GOBLIN_SKILL::TP)];
			_vector vPlayerPos = XMLoadFloat4(&m_vTargetPos);
			_vector vPlayerLook = m_pTarget->Get_Component<CTransform>()->Get_State(STATE::LOOK);
			vPlayerLook = XMVectorSetY(vPlayerLook, 1.f);
			vPlayerLook = XMVector3Normalize(vPlayerLook);

			_float randAngleDeg = m_pGameInstance->Real_Random_Float(-60.f, 60.f);
			_float randAngleRad = XMConvertToRadians(randAngleDeg);

			_matrix rot = XMMatrixRotationY(randAngleRad);
			_vector offsetDir = XMVector3TransformNormal(vPlayerLook, rot);

			_float randDist = m_pGameInstance->Real_Random_Float(5.f, 5.5f);

			_vector vFinalPos = vPlayerPos + offsetDir * randDist;
			vFinalPos = XMVectorSetY(vFinalPos, XMVectorGetY(vPlayerPos) + 1.f);

			m_pCharacter_Controller->Set_Position(vFinalPos); }
	, 0.99f);
}

HRESULT CGoblin::Behavior_BlinkExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	m_fTpTime += fTimeDelta;
	if (m_fTpTime >= 1.5f)
	{
		m_bVisible = true;
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

void CGoblin::Behavior_BlinkExit()
{
	m_pFSM->Disable_State(FSMSTATE::BLINK);
}

void CGoblin::Behavior_ShuffleEnter()
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

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pFSM->Change_State(FSMSTATE::COMBAT); },
			0.3f);
}

HRESULT CGoblin::Behavior_ShuffleExitCheck(_float fTimeDelta)
{
	return E_FAIL;
}

void CGoblin::Behavior_ShuffleExit()
{
	m_pFSM->Disable_State(FSMSTATE::SHUFFLE);
}

void CGoblin::Behavior_HitEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::HIT);

	m_bLookAt = false;
	switch (m_eHitSpell)
	{
	case STATEANIM::KNOCKDOWN_FWD:
		pairAnimInfo = m_Animation[STATEANIM::KNOCKDOWN_FWD];
		break;
	case STATEANIM::TUMBLE2:
		pairAnimInfo = m_Animation[STATEANIM::TUMBLE2];
		break;
	case STATEANIM::HIT_LEVIOSO:
	{
		pairAnimInfo = m_Animation[STATEANIM::HIT_LEVIOSO];
		break;
	}
	default:
		pairAnimInfo = m_Animation[STATEANIM::KNOCKDOWN_FWD];
		break;
	}

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CGoblin::Behavior_HitExitCheck(_float fTimeDelta)
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

void CGoblin::Behavior_HitExit()
{
	m_pFSM->Disable_State(FSMSTATE::HIT);
}

void CGoblin::Behavior_DeadEnter()
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

	Get_PartObject<CEditEffect>("Goblin_Particle")->Set_Visible(false);
	Get_PartObject<CEditEffect>("Goblin_Particle2")->Set_Visible(false);
	Get_PartObject<CEditEffect>("Goblin_Smoke")->Set_Visible(false);

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CGoblin::Behavior_DeadExitCheck(_float fTimeDelta)
{
	if (FLT_EPSILON > m_pModelCom->Get_CurrentTrackProgressRatio()) {
		return E_PENDING;
	}
	return S_OK;
}

void CGoblin::Behavior_DeadExit()
{
	m_bDead = true;
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
		CState_Swing::STATE_SWING_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_SwingEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_SwingExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_SwingExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::SWING, CState_Swing::Create(&Desc));
	}
	{
		CState_Throw::STATE_THROW_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_ThrowEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_ThrowExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_ThrowExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::SKILL, CState_Throw::Create(&Desc));
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

void CGoblin::Set_Anim()
{
	m_Animation[STATEANIM::IDLE] = { 22,true };
	m_Animation[STATEANIM::IDLE_TURN_BWD] = { 24, false };
	m_Animation[STATEANIM::IDLE_TURN_L] = { 26, false };
	m_Animation[STATEANIM::IDLE_TURN_R] = { 30, false };

	m_Animation[STATEANIM::WALK_FWD] = { 45,true };
	m_Animation[STATEANIM::WALK_START] = { 57,false };
	m_Animation[STATEANIM::WALK_STOP] = { 58,false };
	m_Animation[STATEANIM::WALK_BWD] = { 252, true };

	m_Animation[STATEANIM::JOG_FWD] = { 147,true }; // 147
	m_Animation[STATEANIM::JOG_START] = { 157,false };
	m_Animation[STATEANIM::JOG_BWD] = { 146,false };
	m_Animation[STATEANIM::JOG_112_L] = { 148,true };
	m_Animation[STATEANIM::JOG_112_R] = { 152,true };
	m_Animation[STATEANIM::JOG_90_L] = { 151,true };
	m_Animation[STATEANIM::JOG_90_R] = { 155,true };

	m_Animation[STATEANIM::SHUFFLE1] = { 203,false };
	m_Animation[STATEANIM::SHUFFLE2] = { 204,false };
	m_Animation[STATEANIM::SHUFFLE3] = { 205,false };
	m_Animation[STATEANIM::SHUFFLE4] = { 206,false };

	m_Animation[STATEANIM::DODGE_LEFT] = { 125, false };
	m_Animation[STATEANIM::DODGE_RIGHT] = { 126, false };
	m_Animation[STATEANIM::SPRINT] = { 90, true };

	m_Animation[STATEANIM::SKILL] = { 85,false }; // 나이프 던지기
	m_Animation[STATEANIM::BLINK_START] = { 428,false }; // 텔레포트
	m_Animation[STATEANIM::BLINK] = { 426,false }; // 텔레포트
	m_Animation[STATEANIM::SWING_FWD] = { 100,false }; // 도끼 스윙 444
	m_Animation[STATEANIM::ATTACK_END] = { 76,false };

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

	m_Animation[STATEANIM::DEAD_BWD] = { 317, false };
	m_Animation[STATEANIM::DEAD_BWD2] = { 318, false };
	m_Animation[STATEANIM::DEAD_FWD] = { 319, false };
	m_Animation[STATEANIM::DEAD_FWD2] = { 320, false };
	m_Animation[STATEANIM::DEAD_L] = { 321, false };
	m_Animation[STATEANIM::DEAD_L2] = { 322, false };
	m_Animation[STATEANIM::DEAD_R] = { 323, false };
	m_Animation[STATEANIM::DEAD_R2] = { 324, false };

	//Tp 426
	// Tp Start 428
	// 76 공격끝








	// 걷기 시작 57
	// 걷기 루프 50
	// 뛰기 시작 157
	// 뛰기 루프 45
	// 나이프 던지기 85
	// 도끼 공격 444
	// 도끼 공격 후 76
	// 도끼 찍기 439
	// 

	// 텀블링 히트 380
	// 디센도 찍기전 띄우기 371
	// 디센도 찍기 372

	// 레비오소 피격 381
	// 착지 376
	//

	// Shuffle1 211
	// Shuffle2 215
	// Shuffle3 219
	// Shuffle4 223	

	//Jog 112도 L 148
	//Jog 112도 R 152
	// Jog 90 L 151
	// Jog 90 R 155



}
