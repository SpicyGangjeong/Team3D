#include "pch.h"
#include "Troll.h"

#include "InfoInstance.h"
#include "GameInstance.h"
#include "Player.h"
#include "Troll_Rock.h"
#include "Layer.h"
#include "EffectPool.h"
#include "TrailObject.h"
#include "Troll_Weapon.h"
#include "Effect_Container.h"
#include "EffectParts.h"
#include "ReparoObject.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_IdleBreak.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "Troll_State_Stun.h"
#include "State_Rush.h"
#include "Troll_State_BackHand_Swing.h"
#include "State_Throw.h"
#include "State_Swing.h"
#include "State_Slam.h"
#include "State_Hit.h"
#include "State_Dead.h"
#pragma endregion


void CTroll::Behavior_IdleEnter()
{
	m_pFSM->Enable_State(FSMSTATE::IDLE);
	pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::IDLE];

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CTroll::Behavior_IdleExitCheck()
{
	if (m_fTargetDistance <= 18.f && m_fTargetDistance != 0)
		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);

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
	_int RandIndex = 0;
	if (m_pFSM->IsEnable_Previous(FSMSTATE::IDLE))
	{
		RandIndex = 3;
	}
	else {
		RandIndex = m_pGameInstance->Real_Random_Int(0, 3);
	}
	switch (RandIndex)
	{
	case 0: //땅 두번구르기 왼발 오른발
		pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK1];

		Add_Event(pairAnimInfo.first, [this]() {

			_matrix HandMat = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("IK_LeftFoot")) * m_pTransformCom->Get_XMWorldMatrix();

			_float4 vPosition = {};
			XMStoreFloat4(&vPosition, HandMat.r[3]);

			m_pEffectPool->Use_Skill(SKILL_TYPE::TROLL_NOMAL_SMOKE, this, &vPosition);
			}, 0.3f);

		Add_Event(pairAnimInfo.first, [this]() {

			_matrix HandMat = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("IK_RightFoot")) * m_pTransformCom->Get_XMWorldMatrix();

			_float4 vPosition = {};
			XMStoreFloat4(&vPosition, HandMat.r[3]);

			m_pEffectPool->Use_Skill(SKILL_TYPE::TROLL_NOMAL_SMOKE, this, &vPosition);
			}, 0.7f);

		break;
	case 1:// 방망이
		pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK2];

		Add_Event(pairAnimInfo.first, [this]() {

			_matrix WeaponMat = XMLoadFloat4x4(m_pWeapon_BoneMat) * Get_PartObject<CTroll_Weapon>()->Get_Component<CTransform>()->Get_XMWorldMatrix();;

			_float4 vPosition = {};
			XMStoreFloat4(&vPosition, WeaponMat.r[3]);

			m_pEffectPool->Use_Skill(SKILL_TYPE::TROLL_NOMAL_SMOKE, this, &vPosition);
			}, 0.7f);

		break;
	case 2: // 발구르기
		pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK3];

		Add_Event(pairAnimInfo.first, [this]() {

			_matrix HandMat = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("IK_RightFoot")) * m_pTransformCom->Get_XMWorldMatrix();

			_float4 vPosition = {};
			XMStoreFloat4(&vPosition, HandMat.r[3]);

			m_pEffectPool->Use_Skill(SKILL_TYPE::TROLL_NOMAL_SMOKE, this, &vPosition);
			}, 0.2f);

		break;
	case 3: //소리지르기
		pairAnimInfo = m_Animation[STATEANIM::IDLE_BREAK4];

		Add_Event(pairAnimInfo.first, [this]() {

			_matrix HandMat = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("mouth_bag")) * m_pTransformCom->Get_XMWorldMatrix();

			_float4 vPosition = {};

			XMStoreFloat4(&vPosition, HandMat.r[3]);


			m_pEffectPool->Use_Skill(SKILL_TYPE::TROLL_SHOUT, this, &vPosition);

			CameraShake(5.f, 1.f, 2.f, 0.5f);

			}, 0.2f);

		break;
	}
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CTroll::Behavior_IdleBreakExitCheck()
{
	if (m_fTargetDistance >= 6.f)
	{
		if (m_pModelCom->IsFinishedAnim()) {
			if (m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::RUSH)] == 0.f) {
				m_pFSM->Change_State(FSMSTATE::COMBAT);
				return E_FAIL;
			}
			else {
				m_pFSM->Change_State(FSMSTATE::MOVE);
				return E_FAIL;
			}
		}
	}
	else {
		if (m_pModelCom->IsFinishedAnim()) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
			return E_FAIL;
		}
	}

	return S_OK;
}

void CTroll::Behavior_IdleBreakExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLEBREAK);
}

void CTroll::Behavior_MoveEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::MOVE);

	m_pFSM->Enable_State(FSMSTATE::JOG);
	pairAnimInfo = m_Animation[STATEANIM::JOG_START];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,1.f,false,1.2f);

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
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 1.2f);
		}
	}

	if (m_fTargetDistance <= 6.f) {

		for (_uint i = 0; i < ENUM_CLASS(TROLL_SKILL::END); i++)
		{
			if (m_fSkillCoolTime[i] == 0.f)
			{
				m_pFSM->Change_State(FSMSTATE::COMBAT);
				return E_FAIL;
			}
		}
	}


	if (bTurnPlaying)
	{
		m_bLookAt = false;

		if (m_pModelCom->IsFinishedAnim())
		{
			m_bLookAt = true;
			pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 1.2f);
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
			if (fRatio >= 0.9f) {
				pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 1.2f);
			}
		}
		else if (iCurrAnimIndex != m_Animation[STATEANIM::JOG_FWD].first)
		{
			pairAnimInfo = m_Animation[STATEANIM::JOG_START];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 1.2f);
		}

		return S_OK;
	}


	if (m_fTargetDistance <= 6.f && m_fTargetDistance != 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return S_OK;
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
	{
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}
	else if (m_fTargetDistance > 25.f && m_fTargetDistance != 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::MOVE);
		return E_FAIL;
	}

	if (m_fTargetDistance <= 6.f && (m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SLAM)] <= 0.f))
	{
		m_pFSM->Change_State(FSMSTATE::SLAM);
		return E_FAIL;
	}
	else if (m_fTargetDistance <= 6.f && (m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::BACKHAND_SWING)] <= 0.f))
	{
		m_pFSM->Change_State(FSMSTATE::BACKHAND_SWING);
		return E_FAIL;
	}
	else if (m_fTargetDistance <= 6.f && (m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::SWING)] <= 0.f))
	{
		m_pFSM->Change_State(FSMSTATE::SWING);
		return E_FAIL;
	}
	else if (m_fTargetDistance <= 15.f &&m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::THROWROCK)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::THROW_ROCK);
		return E_FAIL;
	}
	else  if (m_fTargetDistance <= 18.f && m_fSkillCoolTime[ENUM_CLASS(TROLL_SKILL::RUSH)] <= 0.f)
	{
		m_pFSM->Change_State(FSMSTATE::RUSH);
		return E_FAIL;
	}
	else
	{
		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
		return E_FAIL;
	}

	return S_OK;
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

	Add_Event(pairAnimInfo.first, [this]() {

		_matrix HandMat = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("IK_LeftFoot")) * m_pTransformCom->Get_XMWorldMatrix();

		_float4 vPosition = {};
		XMStoreFloat4(&vPosition, HandMat.r[3]);

		m_pEffectPool->Use_Skill(SKILL_TYPE::TROLL_NOMAL_SMOKE, this, &vPosition);
		}, 0.45f);

	Add_Event(m_Animation[STATEANIM::RUSH_LOOP].first, [this]() {

		m_pEffectPool->Use_Skill(SKILL_TYPE::TROLL_RUSH, this, nullptr, &m_pRushEffect);

		}, 0.f);
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


			if (m_fRushTime >= 1.f && m_pRushEffect != nullptr)
			{
				m_pRushEffect->Get_PartObject<CEffectParts>()->Get_Component<CInstance_Model>()->Set_Loop(false);
				SAFE_RELEASE(m_pRushEffect);
			}

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
		if (m_fDegree >= 90.f)
		{
			if (m_fCross > 0){
				pairAnimInfo = m_Animation[STATEANIM::IDLE_COMBAT_TURN_BWD_L];
			}
			else{
				pairAnimInfo = m_Animation[STATEANIM::IDLE_COMBAT_TURN_BWD_R];
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

			Add_Event(pairAnimInfo.first,
				[this]() {m_bLookAt = true; },
				0.3f);
		}
	}

	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
		return E_FAIL;
	}
	return S_OK;
}

void CTroll::Behavior_RushExit()
{
	m_pFSM->Disable_State(FSMSTATE::RUSH);
	m_fRushTime = 0.f;
}

void CTroll::Behavior_ThrowEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::THROW_ROCK);
	pairAnimInfo = m_Animation[STATEANIM::THROW_ROCK_START];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

	Add_Event(pairAnimInfo.first,
		[this]() {
			Get_PartObject<CTroll_Rock>()->Set_Visible(true); 

		},
		0.95f);

	Set_Easing(m_Animation[STATEANIM::THROW_ROCK].first, 0.1f, 0.47f, 1.5f);
	Set_Easing(m_Animation[STATEANIM::THROW_ROCK].first, 0.47f, 0.5f, 0.5f);
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

				Add_Event(pairAnimInfo.first,
					[this]() {Troll_Trail_Visible(false); },
					0.6f);

				Add_Event(pairAnimInfo.first, [this]() {


					Troll_Trail_Visible(true);

					_matrix HandMat = XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("SKT_LeftHand")) * m_pTransformCom->Get_XMWorldMatrix();

					_float4 vPosition = {};
					XMStoreFloat4(&vPosition, HandMat.r[3]);

					m_pEffectPool->Use_Skill(SKILL_TYPE::TROLL_NOMAL_SMOKE, this, &vPosition);
					}, 0.1f);
			}
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

	Troll_Trail_Visible(true);
	m_pWeaponTrail->Set_Visible(true);
	m_pWeaponTrail->Get_Component<CTrail>()->Reset_Trail();

	Add_Event(pairAnimInfo.first,
		[this]() {
			Troll_Trail_Visible(false);
			m_pWeaponTrail->Set_Visible(false);
		},
		0.6f);


	Set_Easing(pairAnimInfo.first, 0.1f, 0.47f, 1.8f);
	Set_Easing(pairAnimInfo.first, 0.47f, 0.5f, 0.5f);
}

HRESULT CTroll::Behavior_SwingExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	return S_OK;
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


	Add_Event(pairAnimInfo.first,
		[this]() {
			Troll_Trail_Visible(true);
			m_pWeaponTrail->Set_Visible(true);
			m_pWeaponTrail->Get_Component<CTrail>()->Reset_Trail(); },
		0.15f);

	Add_Event(m_Animation[STATEANIM::SLAM].first,
		[this]() { m_bLookAt = false; },
		0.2f);

	Add_Event(m_Animation[STATEANIM::SLAM].first, [this]() {
		m_pEffectPool->Use_Skill(SKILL_TYPE::TROLL_ATTACK, this);
		Troll_Trail_Visible(false);
		m_pWeaponTrail->Set_Visible(false);
		}, 0.3f);

	Set_Easing(pairAnimInfo.first, 0.2f, 0.42f, 1.5f);

	Set_Easing(pairAnimInfo.first, 0.42f, 0.45f, 0.5f);

}

HRESULT CTroll::Behavior_SlamExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();


	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
		return E_FAIL;
	}

	return S_OK;
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


	Add_Event(pairAnimInfo.first,
		[this]() {
			Troll_Trail_Visible(true);
			m_pWeaponTrail->Set_Visible(true);
			m_pWeaponTrail->Get_Component<CTrail>()->Reset_Trail(); },
		0.4f);

	Add_Event(pairAnimInfo.first,
		[this]() {
			Troll_Trail_Visible(false);
			m_pWeaponTrail->Set_Visible(false);
		},
		0.66f);

	Set_Easing(pairAnimInfo.first, 0.1f, 0.45f, 1.8f);
	Set_Easing(pairAnimInfo.first, 0.65f, 0.7f, 0.5f);
}

HRESULT CTroll::Behavior_BackHandSwingExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
	if (m_pModelCom->IsFinishedAnim())
	{
		m_bLookAt = true;
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}
	return S_OK;
}

void CTroll::Behavior_BackHandSwingExit()
{
	m_pFSM->Disable_State(FSMSTATE::BACKHAND_SWING);
}

void CTroll::Behavior_StunEnter()
{
	m_pFSM->Enable_State(FSMSTATE::STUN);
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

	m_bLookAt = false;
	if (iCurrAnimIndex == m_Animation[STATEANIM::RUSH_LOOP].first)
	{
		pairAnimInfo = m_Animation[STATEANIM::STUN];

		_string strBoneName = "HeadEnd";

		m_pEffectPool->Use_Skill(SKILL_TYPE::TROLL_RUSH_HIT, this);
		m_pEffectPool->Use_Skill(SKILL_TYPE::STUN, this, &strBoneName);

	}

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

	Add_Event(pairAnimInfo.first,
		[this]() { m_bLookAt = true;
		},
		0.6f);

	if (m_pRushEffect != nullptr)
	{
		m_pRushEffect->Get_PartObject<CEffectParts>()->Get_Component<CInstance_Model>()->Set_Loop(false);
		SAFE_RELEASE(m_pRushEffect);
	}

}

HRESULT CTroll::Behavior_StunExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo;
	if (true == m_pModelCom->IsFinishedAnim()) {
		
		m_pFSM->Change_State(FSMSTATE::IDLEBREAK);
		return E_FAIL;
	}
	return S_OK;
}

void CTroll::Behavior_StunExit()
{
	m_pFSM->Disable_State(FSMSTATE::STUN);
}

void CTroll::Behavior_HitEnter()
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
	pair<_uint, _bool> pairAnimInfo;

	m_bLookAt = false;
	if (iCurrAnimIndex == m_Animation[STATEANIM::SLAM].first)
	{
		if (fRatio <= 0.75f) {
			pairAnimInfo = m_Animation[STATEANIM::HIT_FACE];
			Add_Event(pairAnimInfo.first,
				[this]() {
					CameraShake(10.f, 2.f, 3.f, 0.3f);	
					m_pEffectPool->Use_Skill(SKILL_TYPE::TROLL_SELF_HIT, this);
				},0.1f);

			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
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
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	}

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
}

void CTroll::Behavior_DeadEnter()
{
	m_bLookAt = false;
	m_pFSM->Enable_State(FSMSTATE::DEAD);
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

	pairAnimInfo = m_Animation[STATEANIM::KNOCKDOWN_BWD];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

	PSX::PxExtendedVec3 pxControlllerPos = m_pCharacter_Controller->Get_Controller()->getPosition();
	PSX::PxTransform pxTransform((_float)pxControlllerPos.x, (_float)pxControlllerPos.y + 100.f, (_float)pxControlllerPos.z);
	m_pCharacter_Controller->Set_Position(XMLoadFloat3((_float3*)&pxTransform.p));
	m_pRigidBody->SetActive(false);
	m_pCharacter_Controller->SetActive(false);

	m_pTroll_Particle->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -9999.f, 0.f, 1.f));
	m_pTroll_Particle2->Get_Component<CTransform>()->Set_State(STATE::POSITION, XMVectorSet(0.f, -9999.f, 0.f, 1.f));

	m_pLeft_Smoke->Set_Visible(false);
	m_pRight_Smoke->Set_Visible(false);

	Add_Event(pairAnimInfo.first,
		[this]() {
			m_pDead_Smoke->Set_Visible(true);
			m_pDead_Smoke->Get_Component<CTransform>()->Set_State(STATE::POSITION, m_pTransformCom->Get_State(STATE::POSITION));
		}, 0.55f);

}

HRESULT CTroll::Behavior_DeadExitCheck(_float fTimeDelta)
{
	if (FLT_EPSILON > m_pModelCom->Get_CurrentTrackProgressRatio()) {

		return E_PENDING;
	}
	return S_OK;
}

void CTroll::Behavior_DeadExit()
{
	m_bDead = true;
	m_pDead_Smoke->Set_Visible(false);
}

_bool CTroll::IsHitSpellDisabled()
{
	if (m_eHitSpell == ENUM_CLASS(SKILL_TYPE::ANCIENT_MAGIC) ||
		m_eHitSpell == ENUM_CLASS(SKILL_TYPE::ANCIENT_MAGIC_THROW) ||
		m_eHitSpell == ENUM_CLASS(SKILL_TYPE::AVADAKEDAVRA))
	{
		return true;
	}
	return false;
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
		CState_Rush::STATE_RUSH_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_RushEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_RushExitCheck(fTimeDelta); };
		Desc.funcExitEvent = [this]() { Behavior_RushExit(); };
		Desc.pCollisionPlayer = &m_bCollisionPlayer;
		m_States.emplace(FSMSTATE::RUSH, CState_Rush::Create(&Desc));
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
		Desc.funcLateUpdate = [this](_float fTimeDelta, _bool& bHit) {  SwingHit(bHit); };
		m_States.emplace(FSMSTATE::SWING, CState_Swing::Create(&Desc));
	}

	{
		CState_Slam::STATE_SLAM_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_SlamEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_SlamExitCheck(fTimeDelta); };
		Desc.funcExitEvent = [this]() { Behavior_SlamExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = [this](_float fTimeDelta, _bool& bHit) {  SlamHit(bHit); };
		m_States.emplace(FSMSTATE::SLAM, CState_Slam::Create(&Desc));
	}

	{
		CTroll_State_BackHand_Swing::TROLL_STATE_BACK_HAND_SWING_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_BackHandSwingEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_BackHandSwingExitCheck(fTimeDelta); };
		Desc.funcExitEvent = [this]() { Behavior_BackHandSwingExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = [this](_float fTimeDelta, _bool& bHit) {  SwingHit(bHit); };
		m_States.emplace(FSMSTATE::BACKHAND_SWING, CTroll_State_BackHand_Swing::Create(&Desc));
	}
#pragma endregion
#pragma region Behavior_Combat_Focus
	{
		CTroll_State_Stun::TROLL_STATE_STUN Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_StunEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_StunExitCheck(fTimeDelta); };
		Desc.funcExitEvent = [this]() { Behavior_StunExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::STUN, CTroll_State_Stun::Create(&Desc));
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

	{
		CState_Dead::STATE_DEAD_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_DeadEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_DeadExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_DeadExit(); };
		Desc.funcLateUpdate = [this](_float fDeadRatio) {
			m_fDeadRatio = fDeadRatio;
			if (m_fDeadRatio > 1.f) {
				CLayer* pLayer = m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_REPARO);

				if (nullptr != pLayer)
				{
					const list<class CGameObject*>* pReparoObjects = pLayer->Get_Objects();

					for (auto& pObject : *pReparoObjects)
					{
						CReparoObject* pReparoObject = dynamic_cast<CReparoObject*>(pObject);
						if (nullptr != pReparoObject)
							pReparoObject->IsRepairable(true);
					}
				}
				m_bDead = true;
			}
			};
		Desc.vDeadTimer.x = FLT_EPSILON5;
		Desc.vDeadTimer.y = 2.f;
		m_States.emplace(FSMSTATE::DEAD, CState_Dead::Create(&Desc));
	}

#pragma endregion

}

void CTroll::SwingHit(_bool& bPlayerHit)
{
	vector<PSX::PxSweepHit> pxHits;
	_uint iHitCount = 0;
	CheckHammerHits(iHitCount, pxHits);
	{
		for (_uint i = 0; i < pxHits.size(); ++i) {
			PSX::PxActor* pxHitActor = pxHits[i].actor;
			if (nullptr != pxHitActor && nullptr != pxHitActor->userData) {
				PHYSX_USERDATA* pUserData = (PHYSX_USERDATA*)pxHitActor->userData;
				ON_COLLISION_INFO tagCollInfo = {};

				tagCollInfo.vWorldPos.w = 1.f;

				memcpy_s(&tagCollInfo.vWorldPos, sizeof(tagCollInfo.vWorldPos), &pxHits[i].position, sizeof(pxHits[i].position));

				memcpy_s(&tagCollInfo.vWorldNomal, sizeof(tagCollInfo.vWorldNomal), &pxHits[i].normal, sizeof(pxHits[i].normal));
				_vector vHitDir = pUserData->pOwner->Get_WorldPostion() - this->Get_WorldPostion();
				vHitDir = XMVector3Normalize(vHitDir);
				XMStoreFloat4(&tagCollInfo.vHitDir, vHitDir);
				tagCollInfo.fLength = pxHits[i].distance;
				tagCollInfo.eHitType = ENUM_CLASS(HIT_TYPE::HIT_HEAVY);
				tagCollInfo.fDamage = 15.f;
				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::PLAYER:
				{
					if (true == bPlayerHit) {
						continue;
					}
					bPlayerHit = true;
					pUserData->pOwner->OnCollision(this,&tagCollInfo);
				} break;
				case PXOBJECT::ALLY_HITBOX:
					break;
				case PXOBJECT::ENVIRIONMENT:
					break;
				case PXOBJECT::TERRAIN:
					break;
				case PXOBJECT::BOX:
					break;
				case PXOBJECT::NPC:
					break;
				default:
					break;
				}
			}
		}
	}
}

void CTroll::SlamHit(_bool& bPlayerHit)
{
	vector<PSX::PxSweepHit> pxHits;
	_uint iHitCount = 0;
	CheckHammerHits(iHitCount, pxHits);
	{
		for (_uint i = 0; i < pxHits.size(); ++i) {
			PSX::PxActor* pxHitActor = pxHits[i].actor;
			if (nullptr != pxHitActor && nullptr != pxHitActor->userData) {
				PHYSX_USERDATA* pUserData = (PHYSX_USERDATA*)pxHitActor->userData;

				ON_COLLISION_INFO tagCollInfo = {};

				tagCollInfo.vWorldPos.w = 1.f;

				memcpy_s(&tagCollInfo.vWorldPos, sizeof(tagCollInfo.vWorldPos), &pxHits[i].position, sizeof(pxHits[i].position));

				memcpy_s(&tagCollInfo.vWorldNomal, sizeof(tagCollInfo.vWorldNomal), &pxHits[i].normal, sizeof(pxHits[i].normal));
				_vector vHitDir = pUserData->pOwner->Get_WorldPostion() - this->Get_WorldPostion();
				vHitDir = XMVector3Normalize(vHitDir);
				XMStoreFloat4(&tagCollInfo.vHitDir, vHitDir);
				tagCollInfo.fLength = pxHits[i].distance;
				tagCollInfo.eHitType = ENUM_CLASS(HIT_TYPE::HIT_HEAVY);
				tagCollInfo.fDamage = 20.f;
				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::PLAYER:
				{
					if (true == bPlayerHit) {
						continue;
					}
					bPlayerHit = true;
					pUserData->pOwner->OnCollision(this,&tagCollInfo);
				} break;
				case PXOBJECT::ALLY_HITBOX:
					break;
				case PXOBJECT::ENVIRIONMENT:
					break;
				case PXOBJECT::TERRAIN:
					break;
				case PXOBJECT::BOX:
					break;
				case PXOBJECT::NPC:
					break;
				default:
					break;
				}
			}
		}
	}
}

void CTroll::CheckHammerHits(_uint& iHitCount, vector<PSX::PxSweepHit>& pxHits)
{
	{
		_vector vStartPos = XMLoadFloat4(&m_vStartHammerPos);
		_vector vEndPos = XMLoadFloat4(m_pHammerPos);
		_vector vDir = vEndPos - vStartPos;
		_float fDistance = XMVectorGetX(XMVector4Length(vDir));
		vDir = XMVector4Normalize(vDir);

		_bool bCollision = false;
		m_SweepBufferHammer = {};
		bCollision = m_pGameInstance->SphereCast(1.5f, vStartPos, vDir, fDistance, PSX::PxHitFlag::eDEFAULT, PSX::PxQueryFlag::eDYNAMIC, m_SweepBufferHammer);
		if (true == bCollision) {
			iHitCount += m_SweepBufferHammer.getNbTouches() + m_SweepBufferHammer.hasBlock;
			auto touches = m_SweepBufferHammer.getTouches();
			for (_uint i = 0; i < m_SweepBufferHammer.nbTouches; ++i) {
				pxHits.push_back(m_SweepBufferHammer.touches[i]);
			}
			if (true == m_SweepBufferHammer.hasBlock) {
				pxHits.push_back(m_SweepBufferHammer.block);
			}
		}
	}
	{
		_vector vStartPos = XMLoadFloat4(&m_vStartGripPos);
		_vector vEndPos = XMLoadFloat4(m_pHammerGripPos);
		_vector vDir = vEndPos - vStartPos;
		_float fDistance = XMVectorGetX(XMVector4Length(vDir));
		vDir = XMVector4Normalize(vDir);

		_bool bCollision = false;
		m_SweepBufferGrip = {};
		bCollision = m_pGameInstance->SphereCast(1.5f, vStartPos, vDir, fDistance, PSX::PxHitFlag::eDEFAULT, PSX::PxQueryFlag::eDYNAMIC, m_SweepBufferGrip);
		if (true == bCollision) {
			iHitCount += m_SweepBufferGrip.getNbTouches() + m_SweepBufferGrip.hasBlock;
			auto touches = m_SweepBufferGrip.getTouches();
			for (_uint i = 0; i < m_SweepBufferGrip.nbTouches; ++i) {
				pxHits.push_back(m_SweepBufferGrip.touches[i]);
			}
			if (true == m_SweepBufferGrip.hasBlock) {
				pxHits.push_back(m_SweepBufferGrip.block);
			}
		}
	}
}
