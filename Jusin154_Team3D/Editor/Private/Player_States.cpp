#include "pch.h"
#include "Player.h"

#include "InfoInstance.h"

#include "GameInstance.h"
#include "CamPosition_Socket.h"
#include "Camera_Gaze.h"
#include "CamPosition_Arm.h"
#include "Wand.h"
#include "Character_Controller.h"
#include "CallBack_Playable_Behavior.h"
#include "CamPosition_Shoulder.h"
#include "CallBack_Playable_HitReport.h"
#include "Broom.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Dodge.h"
#include "State_Blink.h"
#include "State_Jump.h"
#include "State_Land.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_Hit.h"
#include "State_Broom_Ride.h"
#include "State_Broom_Ride_Move.h"
#include "State_Hover.h"
#include "State_Fly.h"
#include "State_Broom_TurboFly.h"
#include "State_Broom_Dismount.h"
#pragma endregion

#include "EffectPool.h"


#pragma region States

// UI 연동 추가
void CPlayer::Get_Spell(_int SkillIndex)
{
	if (SkillIndex == ENUM_CLASS(SKILL_TYPE::DIFFINDO))
		Index = SkillIndex;
}

void CPlayer::TestKeyInput(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F1))
	{
		m_eSpell = ENUM_CLASS(SKILL_TYPE::BOMBARDA);
	}
	if (m_pGameInstance->Key_Down(DIK_F2))
	{
		m_eSpell = ENUM_CLASS(SKILL_TYPE::DESCENDO);
	}
	if (m_pGameInstance->Key_Down(DIK_F3))
	{
		m_eSpell = ENUM_CLASS(SKILL_TYPE::LEVIOSO);
	}
	if (m_pGameInstance->Key_Down(DIK_F4))
	{
		m_eSpell = ENUM_CLASS(SKILL_TYPE::DIFFINDO);
	}
	if (m_pGameInstance->Key_Down(DIK_F5))
	{
		m_eSpell = ENUM_CLASS(SKILL_TYPE::DISILLUSIONMENT);
	}
	if (m_pGameInstance->Key_Down(DIK_F6))
	{
		m_eSpell = ENUM_CLASS(SKILL_TYPE::LUMOS);
	}
	if (m_pGameInstance->Key_Down(DIK_F7))
	{
		m_eSpell = ENUM_CLASS(SKILL_TYPE::ACCIO);
	}
}

HRESULT CPlayer::InputAction()
{
	if (
		m_pGameInstance->Key_Down(DIK_SPACE)
		|| m_pGameInstance->Key_Down(DIK_LCONTROL)
		|| m_pGameInstance->Key_Down(DIK_E)
		|| m_pGameInstance->Key_Down(DIK_R)
		|| m_pGameInstance->Key_Down(DIK_Q)
		|| m_pGameInstance->Mouse_Up(DIM_LBUTTON)
		|| m_pGameInstance->Key_Down(DIK_LSHIFT)
		|| m_pGameInstance->Key_Down(DIK_C)
		|| m_pGameInstance->Key_Down(DIK_V)
		|| m_pGameInstance->Key_Down(DIK_Z)
		|| m_pGameInstance->Key_Down(DIK_G)
		|| m_pGameInstance->Key_Down(DIK_B)
		|| m_pGameInstance->Key_Down(DIK_T)
		|| m_pGameInstance->Key_Down(DIK_TAB)
		)
	{

		return S_OK;
	}
	return E_FAIL;
}

HRESULT CPlayer::InputMove()
{
	if (m_pGameInstance->Key_Pressing(DIK_W)
		|| m_pGameInstance->Key_Pressing(DIK_A)
		|| m_pGameInstance->Key_Pressing(DIK_S)
		|| m_pGameInstance->Key_Pressing(DIK_D))
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CPlayer::InputKeyUpMove()
{
	if (m_pGameInstance->Key_Up(DIK_W)
		|| m_pGameInstance->Key_Up(DIK_A)
		|| m_pGameInstance->Key_Up(DIK_S)
		|| m_pGameInstance->Key_Up(DIK_D))
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CPlayer::InputSpell()
{
	if (
		m_pGameInstance->Key_Down(DIK_1)
		|| m_pGameInstance->Key_Down(DIK_2)
		|| m_pGameInstance->Key_Down(DIK_3)
		|| m_pGameInstance->Key_Down(DIK_4)
		)
	{


		return S_OK;
	}
	return E_FAIL;
}


HRESULT CPlayer::InputAim()
{
	if (m_pGameInstance->Mouse_Pressing(DIM_RBUTTON) ||
		m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		return S_OK;
	}
	return E_FAIL;
}

void CPlayer::Behavior_IdleEnter() {
	m_pFSM->Enable_State(FSMSTATE::IDLE);
	pair<_uint, _bool> pairAnimInfo;

	if (SUCCEEDED(InputAim()))
	{
		if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON)) {
			pairAnimInfo = m_Animation[STATEANIM::IDLE_AIM];
		}
		else
			pairAnimInfo = m_Animation[STATEANIM::IDLE];
	}
	else
	{
		pairAnimInfo = m_Animation[STATEANIM::IDLE];
	}

	m_bSprintToggle = false;
	m_bWalkToggle = false;
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

// S_OK -> 현 상태 유지
// E_FAIL -> 현 상태 탈출
HRESULT CPlayer::Behavior_IdleExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo;
	_vector xmvLook = XMVector4Normalize(XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
	_vector xmvRight = XMVector4Normalize(XMVectorSetY(m_pTransformCom->Get_State(STATE::RIGHT), 0.f));
	_uint iCurrentAnimIndex = m_pModelCom->Get_AnimIndex();
	if (SUCCEEDED(InputAction()) || SUCCEEDED(InputSpell())) {
		if (m_pGameInstance->Key_Down(DIK_SPACE)) {
			m_pFSM->Change_State(FSMSTATE::JUMP);
		}
		else if (m_pGameInstance->Key_Down(DIK_LCONTROL)) {
			m_pFSM->Change_State(FSMSTATE::DODGE);
		}
		else if (m_pGameInstance->Key_Down(DIK_R)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else if (m_pGameInstance->Key_Down(DIK_Q)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else if (SUCCEEDED(InputSpell())) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else if (m_pGameInstance->Mouse_Up(DIM_LBUTTON)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else if (m_pGameInstance->Key_Down(DIK_V)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else if (m_pGameInstance->Key_Down(DIK_G)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		return E_FAIL;
	}

	if (SUCCEEDED(InputMove()))
	{
		m_fInputTime += fTimeDelta;
		if (m_fInputTime > 0.2f)
		{
			m_fInputTime = 0.f;
			m_pFSM->Change_State(FSMSTATE::MOVE);
			return E_FAIL;
		}
	}

	if (SUCCEEDED(InputKeyUpMove()))
	{
		_vector xmvInputDir = XMVectorZero();

		_vector xmvCamLook = XMVector4Normalize(XMVectorSet(m_vCameraLookDir.x, 0.f, m_vCameraLookDir.z, 0.f));
		_vector xmvCamRight = XMVector4Normalize(XMVectorSet(m_vCameraRightDir.x, 0.f, m_vCameraRightDir.z, 0.f));

		if (m_pGameInstance->Key_Up(DIK_W))
			xmvInputDir += xmvCamLook;
		if (m_pGameInstance->Key_Up(DIK_S))
			xmvInputDir -= xmvCamLook;
		if (m_pGameInstance->Key_Up(DIK_A))
			xmvInputDir -= xmvCamRight;
		if (m_pGameInstance->Key_Up(DIK_D))
			xmvInputDir += xmvCamRight;

		if (XMVector3Equal(xmvInputDir, XMVectorZero()))
			return E_FAIL;

		xmvInputDir = XMVector3Normalize(xmvInputDir);

		_float2 vInputDir = { XMVectorGetX(xmvInputDir),XMVectorGetZ(xmvInputDir) };

		_vector xmvCurLook = XMVector4Normalize(
			XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
		_float2 vCurLook = { XMVectorGetX(xmvCurLook),XMVectorGetZ(xmvCurLook) };

		_float vDir = CMyTools::Get_Direction2D(vCurLook, vInputDir);
		_float absDir = fabsf(vDir);

		_float cross = vCurLook.x * vInputDir.y - vCurLook.y * vInputDir.x;

		if (m_pFSM->IsEnable(FSMSTATE::IDLE))
		{
			if (absDir < XMConvertToRadians(45.f))
			{
				m_pFSM->Change_State(FSMSTATE::MOVE);
				return E_FAIL;
			}
			else if (absDir < XMConvertToRadians(135.f))
			{
				if (cross > 0.f) {
					pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_L];
				}
				else {
					pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_R];
				}
			}
			else
			{
				pairAnimInfo = m_Animation[STATEANIM::IDLE_TURN_BWD];
			}

			m_pFSM->Enable_State(FSMSTATE::IDLE_TURN);
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, m_fAmount);
		}

		return E_FAIL;
	}

	if (!SUCCEEDED(InputAim()))
	{
		if (m_pModelCom->IsFinishedAnim())
		{
			pairAnimInfo = m_Animation[STATEANIM::IDLE];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
	}


	return S_OK;
}


void CPlayer::Behavior_BlinkEnter()
{
	m_pFSM->Enable_State(FSMSTATE::BLINK);


	Add_Event(m_Animation[STATEANIM::DODGE].first,
		[&]() {
			m_pEffectPool->Use_Skill(SKILL_TYPE::BLINK, this);
			m_bVisible = false;
		},
		0.1f);
}

HRESULT CPlayer::Behavior_BlinkExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo;
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
	m_fBlinkTime += fTimeDelta;

	if (m_fBlinkTime >= 1.f && iCurrAnimIndex != m_Animation[STATEANIM::DODGE_BLINK].first)
	{
		m_bVisible = true;
		pairAnimInfo = m_Animation[STATEANIM::DODGE_BLINK];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 0.8f, false, 1.5f);
		m_BroomScale = { 1.f,1.f,1.f };
	}
	else if (iCurrAnimIndex != m_Animation[STATEANIM::DODGE_BLINK].first) {


		_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
		vLook = XMVectorSetY(vLook, 0.f);
		vLook = XMVector3Normalize(vLook);
		_vector vPos = m_pCharacter_Controller->Get_Position();
		_vector vNextPos = vPos + vLook * 7.f * fTimeDelta;
		m_pCharacter_Controller->Set_Position(vNextPos);
	}

	if (iCurrAnimIndex == m_Animation[STATEANIM::DODGE_BLINK].first)
	{
		if (SUCCEEDED(InputMove()) && fRatio >= 0.2f) {
			m_pFSM->Change_State(FSMSTATE::MOVE);
			return E_FAIL;
		}

		if (fRatio >= 0.2f) {
			m_pFSM->Change_State(FSMSTATE::IDLE);
			return E_FAIL;
		}
	}

	return S_OK;
}

void CPlayer::Behavior_BlinkExit()
{
	m_pFSM->Disable_State(FSMSTATE::BLINK);
	m_fBlinkTime = 0.f;
}

void CPlayer::Behavior_IdleExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLE | FSMSTATE::IDLE_TURN);
}

void CPlayer::Behavior_MoveEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	_bool bFoward = m_pGameInstance->Key_Pressing(DIK_W);
	_bool bLeft = m_pGameInstance->Key_Pressing(DIK_A);
	_bool bRight = m_pGameInstance->Key_Pressing(DIK_D);
	_bool bBackward = m_pGameInstance->Key_Pressing(DIK_S);
	m_pFSM->Enable_State(FSMSTATE::MOVE);
	if (m_pFSM->IsEnable_Previous(FSMSTATE::IDLE | FSMSTATE::DODGE | FSMSTATE::BLINK)) {
		if (true == m_bSprintToggle) {
			m_pFSM->Enable_State(FSMSTATE::SPRINT);
			m_bWalkToggle = false;
			if (true == bFoward) {
				pairAnimInfo = m_Animation[STATEANIM::SPRINT];
			}
		}
		else if (true == m_bWalkToggle) {
			m_pFSM->Enable_State(FSMSTATE::WALK);
			m_bSprintToggle = false;
			if (true == bFoward) {
				pairAnimInfo = m_Animation[STATEANIM::WALK_FWD];
			}
		}
		else
		{
			m_pFSM->Enable_State(FSMSTATE::JOG);
			m_bSprintToggle = false;
			m_bWalkToggle = false;
			pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
			m_bRatio = true;
		}
	}
	else { // While Moving
		if (true == m_bSprintToggle) {
			m_pFSM->Enable_State(FSMSTATE::SPRINT);
			m_bWalkToggle = false;
			pairAnimInfo = m_Animation[STATEANIM::SPRINT];

		}
		else if (true == m_bWalkToggle) {
			m_pFSM->Enable_State(FSMSTATE::WALK);
			m_bSprintToggle = false;
			if (true == bFoward) {
				pairAnimInfo = m_Animation[STATEANIM::WALK_FWD];
			}
		}
		else {
			m_pFSM->Enable_State(FSMSTATE::JOG);
			m_bSprintToggle = false;
			m_bWalkToggle = false;
			pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
			m_bRatio = true;
		}
	}

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, m_fAmount, m_bRatio);
}

HRESULT CPlayer::Behavior_MoveExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo;
	_vector xmvLook = XMVector4Normalize(XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
	_vector xmvRight = XMVector4Normalize(XMVectorSetY(m_pTransformCom->Get_State(STATE::RIGHT), 0.f));
	_uint iCurrentAnimIndex = m_pModelCom->Get_AnimIndex();

	if (SUCCEEDED(InputAction()) || SUCCEEDED(InputSpell())) {
		if (m_pGameInstance->Key_Down(DIK_SPACE)) {
			m_pFSM->Change_State(FSMSTATE::JUMP);
		}
		else if (m_pGameInstance->Key_Down(DIK_LCONTROL)) {
			m_pFSM->Change_State(FSMSTATE::DODGE);
		}
		else if (m_pGameInstance->Key_Down(DIK_R)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else if (m_pGameInstance->Key_Down(DIK_Q)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else if (m_pGameInstance->Mouse_Up(DIM_LBUTTON)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else if (SUCCEEDED(InputSpell())) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else if (m_pGameInstance->Key_Down(DIK_V)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else if (m_pGameInstance->Key_Down(DIK_Z)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else if (m_pGameInstance->Key_Down(DIK_G)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else if (m_pGameInstance->Key_Down(DIK_B)) {
			m_pFSM->Change_State(FSMSTATE::BROOM_RIDE);
		}
		else if (m_pGameInstance->Key_Down(DIK_LSHIFT)) {
			m_bSprintToggle = !m_bSprintToggle;
			m_bWalkToggle = false;
			if (m_bSprintToggle)
			{
				m_pFSM->Enable_State(FSMSTATE::SPRINT);
				m_pFSM->Disable_State(FSMSTATE::JOG);
				pairAnimInfo = m_Animation[STATEANIM::SPRINT];
			}
			else
			{
				m_pFSM->Enable_State(FSMSTATE::JOG);
				m_pFSM->Disable_State(FSMSTATE::SPRINT);
				pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
		else if (m_pGameInstance->Key_Down(DIK_C)) {
			m_bWalkToggle = !m_bWalkToggle;
			m_bSprintToggle = false;
			if (m_bWalkToggle)
			{
				m_pFSM->Enable_State(FSMSTATE::WALK);
				m_pFSM->Disable_State(FSMSTATE::JOG);
				pairAnimInfo = m_Animation[STATEANIM::WALK_FWD];
			}
			else
			{
				m_pFSM->Enable_State(FSMSTATE::JOG);
				m_pFSM->Disable_State(FSMSTATE::WALK);
				pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
		return S_OK;
	}

	if (SUCCEEDED(InputMove())) {
		_vector xmvInputDir = XMVectorZero();

		_vector xmvCamLook = XMVector4Normalize(XMVectorSet(m_vCameraLookDir.x, 0.f, m_vCameraLookDir.z, 0.f));
		_vector xmvCamRight = XMVector4Normalize(XMVectorSet(m_vCameraRightDir.x, 0.f, m_vCameraRightDir.z, 0.f));

		if (m_pGameInstance->Key_Pressing(DIK_W))
			xmvInputDir += xmvCamLook;
		if (m_pGameInstance->Key_Pressing(DIK_S))
			xmvInputDir -= xmvCamLook;
		if (m_pGameInstance->Key_Pressing(DIK_A))
			xmvInputDir -= xmvCamRight;
		if (m_pGameInstance->Key_Pressing(DIK_D))
			xmvInputDir += xmvCamRight;


		if (XMVector3Equal(xmvInputDir, XMVectorZero()))
			return E_FAIL;

		xmvInputDir = XMVector3Normalize(xmvInputDir);

		_float2 vInputDir = { XMVectorGetX(xmvInputDir),XMVectorGetZ(xmvInputDir) };
		_vector xmvCurLook = XMVector4Normalize(
			XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
		_float2 vCurLook = { XMVectorGetX(xmvCurLook),XMVectorGetZ(xmvCurLook) };

		_float vDir = CMyTools::Get_Direction2D(vCurLook, vInputDir);
		_float absDir = fabsf(vDir);
		_float cross = vCurLook.x * vInputDir.y - vCurLook.y * vInputDir.x;

		_float2 fCamLook = { XMVectorGetX(xmvCamLook), XMVectorGetZ(xmvCamLook) };

		_float angle = CMyTools::Get_Direction2D(vCurLook, fCamLook);

		_float degree = XMConvertToDegrees(angle);
		_bool bSkipAngleCheck = { false };

		if (m_pFSM->IsEnable(FSMSTATE::JOG | FSMSTATE::WALK)) {
			if (iCurrentAnimIndex != m_Animation[STATEANIM::JOG_FWD].first &&
				iCurrentAnimIndex != m_Animation[STATEANIM::WALK_FWD].first) {
				bSkipAngleCheck = true;

				if (m_pModelCom->IsFinishedAnim() || m_pFSM->IsEnable(FSMSTATE::STOP) || SUCCEEDED(InputAim())) {
					m_pFSM->Disable_State(FSMSTATE::STOP);
					bSkipAngleCheck = false;
				}
			}
			if (!bSkipAngleCheck) {
				_float absDir = fabsf(vDir);
				if (absDir < XMConvertToRadians(80.f)) {
					if (m_pFSM->IsEnable(FSMSTATE::JOG))
					{
						pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
					}
					if (m_pFSM->IsEnable(FSMSTATE::WALK))
					{
						pairAnimInfo = m_Animation[STATEANIM::WALK_FWD];
					}
				}
				else if (absDir < XMConvertToRadians(160.f)) {
					if (cross > 0)
					{
						if (SUCCEEDED(InputAim()))
						{
							pairAnimInfo = m_Animation[STATEANIM::JOG_AIM_LEFT];
						}
						else if (m_pFSM->IsEnable(FSMSTATE::JOG))
						{
							if (degree > 35.f && degree < 55.f)
							{
								pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
							}
							else if (degree < -125.f && degree > -145.f)
							{
								pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
							}
							else {
								if (m_pModelCom->IsFinishedAnim() && iCurrentAnimIndex == m_Animation[STATEANIM::JOG_LEFT].first)
								{
									pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
								}
								else {
									pairAnimInfo = m_Animation[STATEANIM::JOG_LEFT];
								}

							}
						}

						m_fAmount = 0.5f;
						m_bRatio = true;
					}
					else {
						if (SUCCEEDED(InputAim()))
						{
							pairAnimInfo = m_Animation[STATEANIM::JOG_AIM_RIGHT];
						}
						else if (m_pFSM->IsEnable(FSMSTATE::JOG))
						{
							if (degree < -35.f && degree > -55.f)
							{
								pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
							}
							else if (degree > 125.f && degree < 145.f)
							{
								pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
							}
							else {
								if (m_pModelCom->IsFinishedAnim() && iCurrentAnimIndex == m_Animation[STATEANIM::JOG_RIGHT].first)
								{
									pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
								}
								else {
									pairAnimInfo = m_Animation[STATEANIM::JOG_RIGHT];
								}

							}
						}

						m_fAmount = 0.5f;
						m_bRatio = true;
					}
				}
				else {
					if (SUCCEEDED(InputAim()))
					{
						pairAnimInfo = m_Animation[STATEANIM::JOG_AIM_BWD];
					}
					else if (m_pFSM->IsEnable(FSMSTATE::JOG))
					{
						if (m_pModelCom->IsFinishedAnim() && iCurrentAnimIndex == m_Animation[STATEANIM::JOG_BWD].first)
						{
							pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
						}
						else
						{
							pairAnimInfo = m_Animation[STATEANIM::JOG_BWD];
						}

					}
					else if (m_pFSM->IsEnable(FSMSTATE::WALK))
					{
						pairAnimInfo = m_Animation[STATEANIM::WALK_BWD];
					}
					m_fAmount = 0.5f;
					m_bRatio = true;
				}

				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, m_fAmount, m_bRatio);
				m_fAmount = 1.f;
			}
		}

		Player_InterpTurn(fTimeDelta);

		return S_OK;
	}

	if (m_pFSM->IsEnable(FSMSTATE::JOG | FSMSTATE::WALK | FSMSTATE::SPRINT) ||
		!SUCCEEDED(InputMove())) {
		if (!m_pFSM->IsEnable(FSMSTATE::STOP))
		{
			m_pFSM->Enable_State(FSMSTATE::STOP);
			if (m_pFSM->IsEnable(FSMSTATE::WALK)) {
				pairAnimInfo = m_Animation[STATEANIM::WALK_STOP];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			}
			else if (m_pFSM->IsEnable(FSMSTATE::SPRINT)) {
				pairAnimInfo = m_Animation[STATEANIM::JOG_STOP];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			}
			else if (m_pFSM->IsEnable(FSMSTATE::JOG)) {
				if (iCurrentAnimIndex == m_Animation[STATEANIM::JOG_AIM_LEFT].first)
				{
					pairAnimInfo = m_Animation[STATEANIM::JOG_AIM_STOP_L];
				}
				else if (iCurrentAnimIndex == m_Animation[STATEANIM::JOG_AIM_RIGHT].first)
				{
					pairAnimInfo = m_Animation[STATEANIM::JOG_AIM_STOP_R];
				}
				else if (iCurrentAnimIndex == m_Animation[STATEANIM::JOG_AIM_BWD].first)
				{
					pairAnimInfo = m_Animation[STATEANIM::JOG_AIM_STOP_BWD];
				}
				else {
					pairAnimInfo = m_Animation[STATEANIM::JOG_STOP];
				}

				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			}
		}
		else if (IsCurrentKeyFrame("TurnStop"))
		{
			m_pFSM->Change_State(FSMSTATE::IDLE);
			m_pFSM->Disable_State(FSMSTATE::STOP);
		}

		return S_OK;
	}

	return E_FAIL;
}

void CPlayer::Behavior_MoveExit()
{
	m_pFSM->Disable_State(FSMSTATE::MOVE | FSMSTATE::SPRINT | FSMSTATE::JOG | FSMSTATE::WALK);
}

void CPlayer::Behavior_JumpEnter()
{
	pair<_uint, _bool> pairAnimInfo;
	m_pFSM->Enable_State(FSMSTATE::JUMP);
	if (m_pFSM->IsEnable_Previous(FSMSTATE::IDLE | FSMSTATE::WALK | FSMSTATE::JOG | FSMSTATE::SPRINT)) {
		if (m_pFSM->IsEnable_Previous(FSMSTATE::IDLE)) {
			pairAnimInfo = m_Animation[STATEANIM::JUMP];
		}
		else if (m_pFSM->IsEnable_Previous(FSMSTATE::SPRINT)) {
			pairAnimInfo = m_Animation[STATEANIM::JUMP_SPRINT];
		}
		else if (m_pFSM->IsEnable_Previous(FSMSTATE::WALK)) {
			pairAnimInfo = m_Animation[STATEANIM::JUMP];
		}
		else if (m_pFSM->IsEnable_Previous(FSMSTATE::JOG)) {
			pairAnimInfo = m_Animation[STATEANIM::JUMP_JOG];
		}
	}
	else {
		// Drop?
	}
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_JumpExitCheck()
{
	if (IsCurrentKeyFrame("Jump")) {
		m_pFSM->Change_State(FSMSTATE::LAND);
		return E_FAIL;
	}
	return S_OK;
}

void CPlayer::Behavior_JumpExit()
{
	m_pFSM->Disable_State(FSMSTATE::JUMP);
}

void CPlayer::Behavior_LandEnter()
{
	m_pFSM->Enable_State(FSMSTATE::LAND);
	pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::LAND];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_LandExitCheck()
{
	if (m_pModelCom->IsFinishedAnim()) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	} // 혹시 Land to (Jog, Sprint, Dodge) 같은 애니메이션 있으면 여기에 분기 조건 넣으면 됨

	if (SUCCEEDED(InputMove())) {
		m_pFSM->Change_State(FSMSTATE::MOVE);
		return E_FAIL;
	}
	return S_OK;
}

void CPlayer::Behavior_LandExit()
{
	m_pFSM->Disable_State(FSMSTATE::LAND);
}

void CPlayer::Behavior_DodgeEnter()
{
	// 혹시 @@ to Dodge 있으면 여기에
	m_pFSM->Enable_State(FSMSTATE::DODGE);
	pair<_uint, _bool> pairAnimInfo = m_Animation[STATEANIM::DODGE];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 1.3f);
}

HRESULT CPlayer::Behavior_DodgeExitCheck(_float fTimeDelta)
{
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (fRatio >= 0.08f)
	{
		if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
		{
			m_pFSM->Change_State(FSMSTATE::BLINK);
			return E_FAIL;
		}
	}

	if (m_pModelCom->IsFinishedAnim()) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}

	if (SUCCEEDED(InputMove()) && fRatio >= 0.3f) {
		m_pFSM->Change_State(FSMSTATE::MOVE);
		return E_FAIL;
	}
	return S_OK;
}

void CPlayer::Behavior_DodgeExit()
{
	m_pFSM->Disable_State(FSMSTATE::DODGE);
}

void CPlayer::Behavior_CombatEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::COMBAT);

	// UI 연동 추가
	if (Index == ENUM_CLASS(SKILL_TYPE::DIFFINDO))
	{
		Add_Event(pairAnimInfo.first, [this]() { m_pEffectPool->Use_Skill(SKILL_TYPE::DIFFINDO, Get_PartObject<CWand>());  }, 0.1f);
		Index = -1;
	}

	if (m_pModelCom->Get_SecondAnimIndex() == m_Animation[STATEANIM::LUMOS].first)
	{
		m_pModelCom->Set_Second_AnimationIndex(-1, ENUM_CLASS(BLEND_BONE::SHOULDER_R));
		m_eSpell = ENUM_CLASS(SKILL_TYPE::END);
	}
	if (m_pGameInstance->Key_Down(DIK_R)) {
		m_pFSM->Enable_State(FSMSTATE::SKILL);
		pairAnimInfo = m_Animation[STATEANIM::SKILL];
		m_pEffectPool->Use_Skill(SKILL_TYPE::REVELIO, this);
	}
	else if (m_pGameInstance->Key_Down(DIK_Q)) {
		m_pFSM->Enable_State(FSMSTATE::SKILL2);
		pairAnimInfo = m_Animation[STATEANIM::SKILL2];
		Add_Event(pairAnimInfo.first,
			[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::PROTEGO, this); },
			0.4f);
	}
	else if (m_pGameInstance->Mouse_Up(DIM_LBUTTON)) {
		m_pFSM->Enable_State(FSMSTATE::LIGHT_ATTACK);
		pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK];

		Add_Event(pairAnimInfo.first, [this]() {_uint iIndex = 0; m_pEffectPool->Use_Skill(SKILL_TYPE::JAP, Get_PartObject<CWand>(), &iIndex);  }, 0.1f);

		Add_Event(pairAnimInfo.first, [this]() { m_pEffectPool->Use_Skill(SKILL_TYPE::JAP_SIDE, Get_PartObject<CWand>());  }, 0.0f);
	}
	else if (SUCCEEDED(InputSpell())) {

		m_pFSM->Enable_State(FSMSTATE::SPELL);
		_float fEventRatio = 0.2f;
		pairAnimInfo = m_Animation[STATEANIM::SPELL];

		if (m_eSpell != ENUM_CLASS(SKILL_TYPE::END))
		{
			switch (m_eSpell)
			{
			case ENUM_CLASS(SKILL_TYPE::BOMBARDA):
				Add_Event(pairAnimInfo.first,
					[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::LEVIOSO_SIDE, Get_PartObject<CWand>()); },
					0.01f);


				Add_Event(pairAnimInfo.first,
					[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::LEVIOSO, this);  },
					0.2f);

				break;
			case ENUM_CLASS(SKILL_TYPE::DESCENDO):
				Add_Event(pairAnimInfo.first,
					[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::DESCENDO, Get_PartObject<CWand>()); },
					0.2f);
				Add_Event(pairAnimInfo.first,
					[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::DESCENDO_SIDE, Get_PartObject<CWand>()); },
					0.f);
				break;
			case ENUM_CLASS(SKILL_TYPE::LEVIOSO):
				Add_Event(pairAnimInfo.first,
					[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::LEVIOSO, this); },
					0.2f);
				Add_Event(pairAnimInfo.first,
					[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::LEVIOSO_SIDE, Get_PartObject<CWand>()); },
					0.f);
				break;
			case ENUM_CLASS(SKILL_TYPE::DIFFINDO):
				pairAnimInfo = m_Animation[STATEANIM::DIFFINDO];
				break;
			case ENUM_CLASS(SKILL_TYPE::DISILLUSIONMENT):
				pairAnimInfo = m_Animation[STATEANIM::DISILLUSION_ENTER];
				break;
			case ENUM_CLASS(SKILL_TYPE::LUMOS):
				if (m_pModelCom->Get_SecondAnimIndex() != m_Animation[STATEANIM::LUMOS].first)
				{
					if (SUCCEEDED(InputMove()))
					{
						if (m_bSprintToggle) {
							pairAnimInfo = m_Animation[STATEANIM::SPRINT];
						}
						else if (m_bWalkToggle) {
							pairAnimInfo = m_Animation[STATEANIM::WALK_FWD];
						}
						else {
							pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
						}
					}
					else {
						pairAnimInfo = m_Animation[STATEANIM::IDLE];
					}
					Add_Event(pairAnimInfo.first,
						[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::LUMOS, Get_PartObject<CWand>()); },
						0.f);
					m_pModelCom->Set_Second_AnimationIndex(m_Animation[STATEANIM::LUMOS].first, ENUM_CLASS(BLEND_BONE::SHOULDER_R), true);
				}
				else
				{
					if (SUCCEEDED(InputMove()))
					{
						if (m_bSprintToggle) {
							pairAnimInfo = m_Animation[STATEANIM::SPRINT];
						}
						else if (m_bWalkToggle) {
							pairAnimInfo = m_Animation[STATEANIM::WALK_FWD];
						}
						else {
							pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
						}
					}
					else {
						pairAnimInfo = m_Animation[STATEANIM::IDLE];
					}
					m_pModelCom->Set_Second_AnimationIndex(m_Animation[STATEANIM::LUMOS_STOP].first, ENUM_CLASS(BLEND_BONE::SHOULDER_R));
					m_eSpell = STATEANIM::END;
				}
				break;
			default:
				pairAnimInfo = m_Animation[STATEANIM::SPELL];
				break;
			}
		}
		else
		{
			pairAnimInfo = m_Animation[STATEANIM::SPELL];
		}
	}
	else if (m_pGameInstance->Key_Down(DIK_V)) {
		m_pFSM->Enable_State(FSMSTATE::MAPHELP);
		pairAnimInfo = m_Animation[STATEANIM::MAPHELP];
	}
	else if (m_pGameInstance->Key_Down(DIK_Z)) {
		m_pFSM->Enable_State(FSMSTATE::ANCIENT_THROW);
		pairAnimInfo = m_Animation[STATEANIM::ANCIENT_THROW];
	}
	else if (m_pGameInstance->Key_Down(DIK_G)) {

	}

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_CombatExitCheck()
{
	pair<_uint, _bool> pairAnimInfo;

	if (SUCCEEDED(InputAction()) || SUCCEEDED(InputSpell())) {
		if (m_pGameInstance->Key_Down(DIK_R))
		{
			m_pFSM->Enable_State(FSMSTATE::SKILL);
		}
		else if (m_pGameInstance->Key_Down(DIK_Q))
		{
			m_pFSM->Enable_State(FSMSTATE::SKILL2);
			pairAnimInfo = m_Animation[STATEANIM::SKILL2];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
		else if (m_pGameInstance->Mouse_Up(DIM_LBUTTON))
		{
			m_pFSM->Enable_State(FSMSTATE::LIGHT_ATTACK);
			_uint iIndex = m_pModelCom->Get_AnimIndex();
			if (m_Animation[STATEANIM::LIGHT_ATTACK].first <= iIndex && m_Animation[STATEANIM::LIGHT_ATTACK].first + 3 > iIndex) {
				_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
				if (fRatio >= 0.1f) {
					pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK];
					pairAnimInfo.first = iIndex + 1;
					m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

					Add_Event(pairAnimInfo.first,
						[this]() {
							_uint iIndex = m_pModelCom->Get_AnimIndex() - m_Animation[STATEANIM::LIGHT_ATTACK].first;
							m_pEffectPool->Use_Skill(SKILL_TYPE::JAP, Get_PartObject<CWand>(), &iIndex); },
						0.1f);

					Add_Event(pairAnimInfo.first, [this]() { m_pEffectPool->Use_Skill(SKILL_TYPE::JAP_SIDE, Get_PartObject<CWand>());  }, 0.0f);
				}
			}
		}
		else if (SUCCEEDED(InputSpell()))
		{
			m_pFSM->Enable_State(FSMSTATE::SPELL);
			_uint iIndex = m_pModelCom->Get_AnimIndex();
			if (m_Animation[STATEANIM::SPELL].first <= iIndex && m_Animation[STATEANIM::SPELL].first + 3 > iIndex) {
				_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
				if (fRatio >= 0.15f) {
					pairAnimInfo = m_Animation[STATEANIM::SPELL];
					pairAnimInfo.first = iIndex + 1;
					m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
				}
			}
		}
		else if (m_pGameInstance->Key_Down(DIK_V))
		{
			m_pFSM->Enable_State(FSMSTATE::MAPHELP);
			pairAnimInfo = m_Animation[STATEANIM::MAPHELP];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
		else if (m_pGameInstance->Key_Down(DIK_Z)) {
			m_pFSM->Enable_State(FSMSTATE::ANCIENT_THROW);
			pairAnimInfo = m_Animation[STATEANIM::ANCIENT_THROW];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
		else if (m_pGameInstance->Key_Down(DIK_G)) {
		
		}
	}

	if (m_pFSM->IsEnable(FSMSTATE::SPELL) && IsCurrentKeyFrame("Combat"))
	{
		if (m_eSpell != ENUM_CLASS(SKILL_TYPE::END))
		{
			switch (m_eSpell)
			{
			case ENUM_CLASS(SKILL_TYPE::ACCIO):
				pairAnimInfo = m_Animation[STATEANIM::ACCIO];
				break;
			case ENUM_CLASS(SKILL_TYPE::DESCENDO):
				pairAnimInfo = m_Animation[STATEANIM::DESCENDO];
				break;
			default:
				m_eSpell = ENUM_CLASS(SKILL_TYPE::END);
				return S_OK;
				break;
			}

			m_eSpell = ENUM_CLASS(SKILL_TYPE::END);
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
	}

	if (m_pFSM->IsEnable(FSMSTATE::ANCIENT_THROW))
	{
		if (SUCCEEDED(InputMove()) && IsCurrentKeyFrame("Throw")) {
			m_pFSM->Change_State(FSMSTATE::MOVE);
			return E_FAIL;
		}
	}
	else
	{
		if (SUCCEEDED(InputMove()) && IsCurrentKeyFrame("Combat")) {
			m_pFSM->Change_State(FSMSTATE::MOVE);
			return E_FAIL;
		}
	}

	if (m_pModelCom->IsFinishedAnim()) {
		if (SUCCEEDED(InputMove()))
		{
			m_pFSM->Change_State(FSMSTATE::MOVE);
			return E_FAIL;
		}
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}

	return E_FAIL;
}

void CPlayer::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT | FSMSTATE::LIGHT_ATTACK | FSMSTATE::SPELL | FSMSTATE::SKILL |
		FSMSTATE::MAPHELP | FSMSTATE::ANCIENT_THROW);
}

void CPlayer::Behavior_HitEnter()
{
	m_pFSM->Enable_State(FSMSTATE::HIT);
	pair<_uint, _bool> pairAnimInfo;
	_int RandIndex = m_pGameInstance->Real_Random_Int(0, 1);
	switch (RandIndex)
	{
	case 0:
		pairAnimInfo = m_Animation[STATEANIM::HIT_R];
		break;
	case 1:
		pairAnimInfo = m_Animation[STATEANIM::HIT_L];
		break;
	default:
		break;
	}

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_HitExitCheck()
{
	if (m_pModelCom->IsFinishedAnim()) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}
	return E_FAIL;
}

void CPlayer::Behavior_HitExit()
{
	m_pFSM->Disable_State(FSMSTATE::HIT);
}

void CPlayer::Behavior_Broom_RideEnter()
{
	m_pFSM->Enable_State(FSMSTATE::BROOM_RIDE);
	m_pBroom->Set_Ride(true);
	pair<_uint, _bool> pairAnimInfo;
	pairAnimInfo = m_Animation[STATEANIM::BROOM_MOUNT];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_Broom_RideExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

	if (iCurrAnimIndex == m_Animation[STATEANIM::BROOM_MOUNT].first)
	{
		if (m_pModelCom->IsFinishedAnim()) {
			pairAnimInfo = m_Animation[STATEANIM::BROOM_MOUNT_END];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, true);
		}
		return S_OK;
	}

	if (m_pModelCom->IsFinishedAnim())
	{
		m_pFSM->Change_State(FSMSTATE::BROOM_RIDE_MOVE);
		return E_FAIL;
	}

	if (m_pGameInstance->Key_Pressing(DIK_N))
	{
		auto flags = m_pCharacter_Controller->Get_CollisionFlags();
		if (flags & PSX::PxControllerCollisionFlag::eCOLLISION_DOWN)
		{
			m_pFSM->Change_State(FSMSTATE::BROOM_DISMOUNT);
			return E_FAIL;
		}
	}

	return S_OK;
}

void CPlayer::Behavior_Broom_RideExit()
{
	m_pFSM->Disable_State(FSMSTATE::BROOM_RIDE);
	Reset_Event();
}

void CPlayer::Behavior_Broom_Ride_MoveEnter()
{
	m_pFSM->Enable_State(FSMSTATE::BROOM_RIDE_MOVE);
}

HRESULT CPlayer::Behavior_Broom_Ride_MoveExitCheck(_float fTimeDelta)
{
	if (m_pBroom->Get_Hover())
	{
		m_pFSM->Change_State(FSMSTATE::HOVER);
		return E_FAIL;
	}
	else if (m_pBroom->Get_Turbo()) {
		m_pFSM->Change_State(FSMSTATE::TURBOFLY);
		return E_FAIL;
	}
	else {
		m_pFSM->Change_State(FSMSTATE::FLY);
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Behavior_Broom_Ride_MoveExit()
{
	m_pFSM->Disable_State(FSMSTATE::BROOM_RIDE_MOVE);
	Reset_Event();
}

void CPlayer::Behavior_Broom_HoverEnter()
{
	m_pFSM->Enable_State(FSMSTATE::HOVER);
}

HRESULT CPlayer::Behavior_Broom_HoverExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

	if (!m_pBroom->Get_Ride()) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}

	_bool bFwd = m_pGameInstance->Key_Pressing(DIK_W);
	_bool bLft = m_pGameInstance->Key_Pressing(DIK_A);
	_bool bRht = m_pGameInstance->Key_Pressing(DIK_D);
	_bool bDown = m_pGameInstance->Key_Pressing(DIK_LCONTROL);
	_bool bUp = m_pGameInstance->Key_Pressing(DIK_SPACE);

	if (m_pBroom->Get_Hover() && iCurrAnimIndex != m_Animation[STATEANIM::BROOM_HOVER_REVELIO].first)
	{
		if (SUCCEEDED(InputBroom()) || SUCCEEDED(InputMove()))
		{
			_float3 vInput = { 0.f, 0.f, 0.f };

			if (bFwd)  vInput.z += 1.f;
			if (bLft)  vInput.x -= 1.f;
			if (bRht)  vInput.x += 1.f;
			if (bUp)   vInput.y += 1.f;
			if (bDown) vInput.y -= 1.f;

			_bool bHasInput = !(vInput.x == 0 && vInput.y == 0 && vInput.z == 0);
			_vector vDir = XMVector3Normalize(XMLoadFloat3(&vInput));

			if (bUp || bDown)
			{
				if (bUp)
				{
					pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE];
				}
				else
				{
					pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE];
				}
			}

			_float ax = fabsf(vInput.x);
			_float ay = fabsf(vInput.y);
			_float az = fabsf(vInput.z);

			if (ay > ax && ay > az)
			{
				if (vInput.y > 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE];
				else
					pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE];
			}
			else if (ax >= az)
			{
				if (ay >= ax)
				{
					if (vInput.y > 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE];
					else
						pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE];
				}
				else {
					if (vInput.x < 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_LEFT];
					else if (vInput.x > 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_RIGHT];
					else {
						pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_FWD];
					}
				}
			}
			else
			{
				if (vInput.y > 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE];
				else if (vInput.y < 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE];
				else {
					pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_FWD];
				}
			}
		}
		else {
			pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE];
		}

		if (m_pGameInstance->Key_Down(DIK_R))
		{
			pairAnimInfo = Get_AnimInfo(STATEANIM::BROOM_HOVER_REVELIO);
			Add_Event(pairAnimInfo.first,
				[this]() {
					m_pEffectPool->Use_Skill(SKILL_TYPE::REVELIO, this);
				}, 0.2f);
		}

		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	}



	if (iCurrAnimIndex == m_Animation[STATEANIM::BROOM_HOVER_REVELIO].first)
	{
		if (m_pModelCom->IsFinishedAnim())
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_FWD];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
	}

	if (m_pGameInstance->Key_Pressing(DIK_N))
	{
		auto flags = m_pCharacter_Controller->Get_CollisionFlags();
		if (flags & PSX::PxControllerCollisionFlag::eCOLLISION_DOWN)
		{
			m_pFSM->Change_State(FSMSTATE::BROOM_DISMOUNT);
			return E_FAIL;
		}
	}

	if (!m_pBroom->Get_Hover())
	{
		m_pFSM->Change_State(FSMSTATE::BROOM_RIDE_MOVE);
		return E_FAIL;
	}

	if (m_pBroom->Get_Turbo())
	{
		m_pFSM->Change_State(FSMSTATE::TURBOFLY);
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Behavior_Broom_HoverExit()
{
	m_pFSM->Disable_State(FSMSTATE::HOVER);
}

void CPlayer::Behavior_Broom_FlyEnter()
{
	m_pFSM->Enable_State(FSMSTATE::FLY);
	Reset_Event();
}

HRESULT CPlayer::Behavior_Broom_FlyExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

	if (!m_pBroom->Get_Ride()) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}

	_bool bFwd = m_pGameInstance->Key_Pressing(DIK_W);
	_bool bLft = m_pGameInstance->Key_Pressing(DIK_A);
	_bool bRht = m_pGameInstance->Key_Pressing(DIK_D);
	_bool bDown = m_pGameInstance->Key_Pressing(DIK_LCONTROL);
	_bool bUp = m_pGameInstance->Key_Pressing(DIK_SPACE);

	if (!m_pBroom->Get_Hover() && iCurrAnimIndex != m_Animation[STATEANIM::BROOM_REVELIO].first)
	{
		if (SUCCEEDED(InputBroom()) || SUCCEEDED(InputMove()))
		{
			_float3 vInput = { 0.f, 0.f, 0.f };

			if (bFwd)  vInput.z += 1.f;
			if (bLft)  vInput.x -= 1.f;
			if (bRht)  vInput.x += 1.f;
			if (bUp)   vInput.y += 1.f;
			if (bDown) vInput.y -= 1.f;

			_bool bHasInput = !(vInput.x == 0 && vInput.y == 0 && vInput.z == 0);
			_vector vDir = XMVector3Normalize(XMLoadFloat3(&vInput));

			if (bUp || bDown)
			{
				if (bUp)
				{
					pairAnimInfo = m_Animation[STATEANIM::BROOM_UP];
				}
				else
				{
					pairAnimInfo = m_Animation[STATEANIM::BROOM_DOWN];
				}
			}

			_float ax = fabsf(vInput.x);
			_float ay = fabsf(vInput.y);
			_float az = fabsf(vInput.z);

			if (ay > ax && ay > az)
			{
				if (vInput.y > 0) {
					pairAnimInfo = m_Animation[STATEANIM::BROOM_UP];

				}
				else if (vInput.y < 0) {
					pairAnimInfo = m_Animation[STATEANIM::BROOM_DOWN];
				}
			}
			else if (ax >= az)
			{
				if (ay >= ax)
				{
					if (vInput.y > 0) {
						pairAnimInfo = m_Animation[STATEANIM::BROOM_UP];

					}
					else if (vInput.y < 0) {
						pairAnimInfo = m_Animation[STATEANIM::BROOM_DOWN];

					}
					else {
						pairAnimInfo = m_Animation[STATEANIM::BROOM_IDLE];
					}
				}
				else {
					if (vInput.x < 0) {
						pairAnimInfo = m_Animation[STATEANIM::BROOM_LEFT];

					}
					else if (vInput.x > 0) {
						pairAnimInfo = m_Animation[STATEANIM::BROOM_RIGHT];

					}
					else {
						pairAnimInfo = m_Animation[STATEANIM::BROOM_IDLE];
					}
				}
			}
			else
			{
				if (vInput.y > 0) {
					pairAnimInfo = m_Animation[STATEANIM::BROOM_UP];

				}
				else if (vInput.y < 0) {
					pairAnimInfo = m_Animation[STATEANIM::BROOM_DOWN];

				}
				else {
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FWD];
				}
			}

			if (iCurrAnimIndex != pairAnimInfo.first)
			{
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false, true);
			}
		}
		else {

			pairAnimInfo = Get_AnimInfo(STATEANIM::BROOM_IDLE);

			if (iCurrAnimIndex != pairAnimInfo.first)
			{
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false, true);
			}
		}

		if (m_pGameInstance->Key_Down(DIK_R))
		{
			pairAnimInfo = Get_AnimInfo(STATEANIM::BROOM_REVELIO);
			Add_Event(pairAnimInfo.first,
				[this]() {
					m_pEffectPool->Use_Skill(SKILL_TYPE::REVELIO, this);
				}, 0.2f);
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
	}

	if (iCurrAnimIndex == m_Animation[STATEANIM::BROOM_REVELIO].first)
	{
		if (m_pModelCom->IsFinishedAnim())
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_FWD];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
	}

	if (m_pGameInstance->Key_Pressing(DIK_N))
	{
		auto flags = m_pCharacter_Controller->Get_CollisionFlags();
		if (flags & PSX::PxControllerCollisionFlag::eCOLLISION_DOWN)
		{
			m_pFSM->Change_State(FSMSTATE::BROOM_DISMOUNT);
			return E_FAIL;
		}
	}

	if (m_pBroom->Get_Hover())
	{
		m_pFSM->Change_State(FSMSTATE::BROOM_RIDE_MOVE);
		return E_FAIL;
	}

	if (m_pBroom->Get_Turbo())
	{
		m_pFSM->Change_State(FSMSTATE::TURBOFLY);
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Behavior_Broom_FlyExit()
{
	m_pFSM->Disable_State(FSMSTATE::FLY);
	Reset_Event();
}

void CPlayer::Behavior_Broom_TurboFlyEnter()
{
	m_pFSM->Enable_State(FSMSTATE::TURBOFLY);
}

HRESULT CPlayer::Behavior_Broom_TurboFlyExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

	if (!m_pBroom->Get_Ride()) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}

	_bool bFwd = m_pGameInstance->Key_Pressing(DIK_W);
	_bool bLft = m_pGameInstance->Key_Pressing(DIK_A);
	_bool bRht = m_pGameInstance->Key_Pressing(DIK_D);
	_bool bDown = m_pGameInstance->Key_Pressing(DIK_LCONTROL);
	_bool bUp = m_pGameInstance->Key_Pressing(DIK_SPACE);

	if (!m_pBroom->Get_Hover() && m_pBroom->Get_Turbo())
	{
		if (SUCCEEDED(InputBroom()) || SUCCEEDED(InputMove()))
		{
			_float3 vInput = { 0.f, 0.f, 0.f };

			if (bFwd)  vInput.z += 1.f;
			if (bLft)  vInput.x -= 1.f;
			if (bRht)  vInput.x += 1.f;
			if (bUp)   vInput.y += 1.f;
			if (bDown) vInput.y -= 1.f;

			_bool bHasInput = !(vInput.x == 0 && vInput.y == 0 && vInput.z == 0);
			_vector vDir = XMVector3Normalize(XMLoadFloat3(&vInput));

			if (bUp || bDown)
			{
				if (bUp)
				{
					pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_UP];
				}
				else
				{
					pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_DOWN];
				}
			}

			_float ax = fabsf(vInput.x);
			_float ay = fabsf(vInput.y);
			_float az = fabsf(vInput.z);

			if (ay > ax && ay > az)
			{
				if (vInput.y > 0.f)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_UP];
				else if (vInput.y < 0.f)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_DOWN];
			}
			else if (ax >= az)
			{
				if (ay >= ax)
				{
					if (vInput.y > 0.f)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_UP];
					else if (vInput.y < 0.f)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_DOWN];
					else {
						pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_FWD];
					}
				}
				else {
					if (vInput.x < 0.f)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_LEFT];
					else if (vInput.x > 0.f)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_RIGHT];
					else {
						pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_FWD];
					}
				}
			}
			else
			{
				if (vInput.y > 0.f)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_UP];
				else if (vInput.y < 0.f)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_DOWN];
				else {
					pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_FWD];
				}
			}

			if (iCurrAnimIndex != pairAnimInfo.first)
			{
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false, true);
			}
		}
		else {

			pairAnimInfo = Get_AnimInfo(STATEANIM::BROOM_TURBO_FWD);

			if (iCurrAnimIndex != pairAnimInfo.first)
			{
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false, true);
			}
		}
	}

	if (m_pGameInstance->Key_Pressing(DIK_N))
	{
		auto flags = m_pCharacter_Controller->Get_CollisionFlags();
		if (flags & PSX::PxControllerCollisionFlag::eCOLLISION_DOWN)
		{
			m_pFSM->Change_State(FSMSTATE::BROOM_DISMOUNT);
			return E_FAIL;
		}
	}

	if (m_pBroom->Get_Hover())
	{
		m_pFSM->Change_State(FSMSTATE::BROOM_RIDE_MOVE);
		return E_FAIL;
	}

	if (!m_pBroom->Get_Turbo())
	{
		m_pFSM->Change_State(FSMSTATE::FLY);
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Behavior_Broom_TurboFlyExit()
{
	m_pFSM->Disable_State(FSMSTATE::TURBOFLY);
	Reset_Event();
}

void CPlayer::Behavior_Broom_DismountEnter()
{
	pair<_uint, _bool> pairAnimInfo;
	m_bOnce = false;
	//m_pInfoInstance->Event_CallBack(TEXT("BroomRide"), &m_bOnce);
	pairAnimInfo = Get_AnimInfo(STATEANIM::BROOM_DISMOUNT);
	m_pFSM->Enable_State(FSMSTATE::BROOM_DISMOUNT);
	m_pBroom->Set_Ride(false);
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, false, 1.6f);
}

HRESULT CPlayer::Behavior_Broom_DismountExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
	if (iCurrAnimIndex == m_Animation[STATEANIM::BROOM_DISMOUNT].first)
	{
		m_BroomScale.x -= (m_TargetScale.x - m_BroomScale.x) * fTimeDelta * m_fScaleSmoothSpeed;
		m_BroomScale.y -= (m_TargetScale.y - m_BroomScale.y) * fTimeDelta * m_fScaleSmoothSpeed;
		m_BroomScale.z -= (m_TargetScale.z - m_BroomScale.z) * fTimeDelta * m_fScaleSmoothSpeed;
		if (m_BroomScale.x >= 0.1f)
		{
			m_pBroomTransform->Set_Scale(m_BroomScale);
		}
		if (fRatio >= 0.5f) {
			m_BroomScale = { 0.f,0.f,0.f };
			m_pBroomTransform->Set_State(STATE::POSITION, XMVectorSet(XMVectorGetX(m_pTransformCom->Get_State(STATE::POSITION)),
				200.f,
				XMVectorGetZ(m_pTransformCom->Get_State(STATE::POSITION)), 1.f));
			if (SUCCEEDED(InputMove()))
			{
				m_pFSM->Change_State(FSMSTATE::MOVE);
				return E_FAIL;
			}
			else {
				m_pFSM->Change_State(FSMSTATE::IDLE);
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

void CPlayer::Behavior_Broom_DismountExit()
{
	m_pFSM->Disable_State(FSMSTATE::BROOM_DISMOUNT);
	Reset_Event();
	m_bOnce = false;
}


void CPlayer::Attach_Broom()
{
	if (m_pBroom->Get_Ride()) {
		_matrix BroomWorld = XMLoadFloat4x4(m_pBroomTransform->Get_WorldMatrixPtr());
		_matrix BoneLocal = XMLoadFloat4x4(m_pBroomModel->Get_BoneMatrixPtr("broomSocket"));

		_vector Scale, Rot, Trans;

		XMMatrixDecompose(&Scale, &Rot, &Trans, BoneLocal);

		_matrix BoneNoScale = XMMatrixRotationQuaternion(Rot) * XMMatrixTranslationFromVector(Trans);

		m_OffsetPos = { 0.f,1.f,0.f };

		_matrix Offset = XMMatrixTranslation(m_OffsetPos.x,
			m_OffsetPos.y, m_OffsetPos.z);

		_vector BS, BR, BT;
		XMMatrixDecompose(&BS, &BR, &BT, BroomWorld);
		_matrix BroomWorld_NoScale =
			XMMatrixRotationQuaternion(BR) *
			XMMatrixTranslationFromVector(BT);


		_matrix SocketWorld = BoneNoScale * Offset * BroomWorld_NoScale;

		_matrix FixRot = XMMatrixRotationY(XMConvertToRadians(180.f));

		_matrix FinalWorld = FixRot * SocketWorld;

		m_pTransformCom->Set_WorldMatrix(FinalWorld);
		m_pCharacter_Controller->Set_Position(FinalWorld.r[3]);
	}
}


void CPlayer::Player_InterpTurn(_float fTimeDelta)
{
	_uint iCurrIndex = m_pModelCom->Get_AnimIndex();
	if (iCurrIndex != m_Animation[STATEANIM::JOG_FWD].first &&
		iCurrIndex != m_Animation[STATEANIM::WALK_FWD].first &&
		iCurrIndex != m_Animation[STATEANIM::SPRINT].first)
		return;

	_vector xmvCurLook = XMVector4Normalize(
		XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
	_float2 vCurLook = { XMVectorGetX(xmvCurLook),XMVectorGetZ(xmvCurLook) };

	_vector vCameraLook = XMVectorSet(m_vCameraLookDir.x, 0.f, m_vCameraLookDir.z, 0.f);

	_vector vCameraRight = XMVectorSet(m_vCameraRightDir.x, 0.f, m_vCameraRightDir.z, 0.f);

	_float2 fCamLook = { XMVectorGetX(vCameraLook), XMVectorGetZ(vCameraLook) };

	vCameraRight = XMVector3Normalize(vCameraRight);

	_float angle = CMyTools::Get_Direction2D(vCurLook, fCamLook);

	_float degree = XMConvertToDegrees(angle);

	_vector xmvInput = XMVectorZero();

	if (m_pGameInstance->Key_Pressing(DIK_W))
		xmvInput += vCameraLook;

	if (m_pGameInstance->Key_Pressing(DIK_S))
		xmvInput -= vCameraLook;

	if (m_pGameInstance->Key_Pressing(DIK_D))
		xmvInput -= vCameraRight;

	if (m_pGameInstance->Key_Pressing(DIK_A))
		xmvInput += vCameraRight;

	xmvInput = XMVector3Normalize(xmvInput);

	_float2 fInput2D = { XMVectorGetX(xmvInput), XMVectorGetZ(xmvInput) };

	_float targetAngle = XMConvertToDegrees(CMyTools::Get_Direction2D(fCamLook, fInput2D));

	_float angleDiff = degree - targetAngle;

	if (angleDiff > 180.f) {
		angleDiff -= 360.f;
	}
	if (angleDiff < -180.f) {
		angleDiff += 360.f;
	}

	_float Offset = 2.f;

	if (angleDiff > Offset)
	{
		m_pTransformCom->Turn(-m_pTransformCom->Get_State(STATE::UP), fTimeDelta);
	}
	else if (angleDiff < -Offset)
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta);
	}
}

void CPlayer::Add_FSM()
{
#pragma region Behavior_Movement_NotFocus
	{
		CState_Idle::STATE_IDLE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_IdleEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_IdleExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_IdleExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			{
				if (SUCCEEDED(InputAim()))
				{
					if (m_pGameInstance->Mouse_Pressing(DIM_RBUTTON))
					{
						_float3	fMove = m_pGameInstance->Get_MouseMove();
						m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * fMove.x * 0.03f);
					}

				}
			}
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::IDLE, CState_Idle::Create(&Desc));
	}
	{
		CState_Move::STATE_MOVE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_MoveEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_MoveExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_MoveExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			{
				/*if (!m_pFSM->IsEnable(FSMSTATE::STOP))
				{
					_float3	fMove = m_pGameInstance->Get_MouseMove();
					m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * fMove.x * 0.05f);
				}*/
			}
			};

		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::MOVE, CState_Move::Create(&Desc));
	}
#pragma endregion


	{
		CState_Blink::STATE_BLINK_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_BlinkEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_BlinkExitCheck(fTimeDelta); };
		Desc.funcExitEvent = [this]() { Behavior_BlinkExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {

			if (SUCCEEDED(InputMove()))
			{
				_vector xmvInputDir = XMVectorZero();

				_vector xmvCamLook = XMVector4Normalize(XMVectorSet(m_vCameraLookDir.x, 0.f, m_vCameraLookDir.z, 0.f));
				_vector xmvCamRight = XMVector4Normalize(XMVectorSet(m_vCameraRightDir.x, 0.f, m_vCameraRightDir.z, 0.f));

				if (m_pGameInstance->Key_Pressing(DIK_W))
					xmvInputDir += xmvCamLook;
				if (m_pGameInstance->Key_Pressing(DIK_S))
					xmvInputDir -= xmvCamLook;
				if (m_pGameInstance->Key_Pressing(DIK_A))
					xmvInputDir -= xmvCamRight;
				if (m_pGameInstance->Key_Pressing(DIK_D))
					xmvInputDir += xmvCamRight;

				xmvInputDir = XMVector3Normalize(xmvInputDir);

				_float2 vInputDir = { XMVectorGetX(xmvInputDir),XMVectorGetZ(xmvInputDir) };
				_vector xmvCurLook = XMVector4Normalize(
					XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
				_float2 vCurLook = { XMVectorGetX(xmvCurLook),XMVectorGetZ(xmvCurLook) };

				_float vDir = CMyTools::Get_Direction2D(vCurLook, vInputDir);
				_float absDir = fabsf(vDir);
				_float cross = vCurLook.x * vInputDir.y - vCurLook.y * vInputDir.x;
				if (cross > 0.f)
				{
					m_pTransformCom->Turn(-m_pTransformCom->Get_State(STATE::UP), fTimeDelta);
				}
				else {
					m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta);
				}
			}
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::BLINK, CState_Blink::Create(&Desc));
	}

#pragma region Behavior_Movement_Focus
	{
		CState_Jump::STATE_JUMP_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_JumpEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_JumpExitCheck(); };
		Desc.funcExitEvent = [this]() { Behavior_JumpExit(); };
		m_States.emplace(FSMSTATE::JUMP, CState_Jump::Create(&Desc));
	}
	{
		CState_Land::STATE_LAND_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_LandEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_LandExitCheck(); };
		Desc.funcExitEvent = [this]() { Behavior_LandExit(); };
		m_States.emplace(FSMSTATE::LAND, CState_Land::Create(&Desc));
	}
	{
		CState_Dodge::STATE_DODGE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_DodgeEnter(); };
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_DodgeExitCheck(fTimeDelta); };
		Desc.funcExitEvent = [this]() { Behavior_DodgeExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			if (m_pGameInstance->Key_Pressing(DIK_A)) { m_pTransformCom->Turn(-m_pTransformCom->Get_State(STATE::UP), fTimeDelta); }
			if (m_pGameInstance->Key_Pressing(DIK_D)) { m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta); };
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::DODGE, CState_Dodge::Create(&Desc));
	}
#pragma endregion
#pragma region Behavior_Combat_NotFocus
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
#pragma endregion
#pragma region Behavior_Combat_Focus

#pragma endregion

#pragma region Behavior_Hit
	

#pragma endregion

#pragma region Behavior_Broom_Ride
	{
		CState_Broom_Ride::STATE_BROOM_RIDE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_RideEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_RideExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_RideExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
			if (!m_bOnce) {
				m_pBroomTransform->Set_State(
					STATE::POSITION,
					m_pTransformCom->Get_State(STATE::POSITION) + XMVectorSet(0.f, 2.f, 0.f, 0.f)
				);

				m_pBroomTransform->Set_State(STATE::LOOK, m_pTransformCom->Get_State(STATE::LOOK));
				m_pBroomTransform->Set_State(STATE::UP, m_pTransformCom->Get_State(STATE::UP));
				m_pBroomTransform->Set_State(STATE::RIGHT, m_pTransformCom->Get_State(STATE::RIGHT));

				_float3 vScale = { 0.3f,0.3f,0.3f };
				m_pBroomTransform->Set_Scale(vScale);
				m_bOnce = true;
				//m_pInfoInstance->Event_CallBack(TEXT("BroomRide"), &m_bOnce);
			}
			m_BroomScale.x += (m_TargetScale.x - m_BroomScale.x) * fTimeDelta * m_fScaleSmoothSpeed;
			m_BroomScale.y += (m_TargetScale.y - m_BroomScale.y) * fTimeDelta * m_fScaleSmoothSpeed;
			m_BroomScale.z += (m_TargetScale.z - m_BroomScale.z) * fTimeDelta * m_fScaleSmoothSpeed;
			if (m_BroomScale.x <= 1.f)
			{
				m_pBroomTransform->Set_Scale(m_BroomScale);
			}


			Attach_Broom();
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::BROOM_RIDE, CState_Broom_Ride::Create(&Desc));
	}

	{
		CState_Broom_Ride_Move::STATE_BROOM_RIDE_MOVE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_Ride_MoveEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_Ride_MoveExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_Ride_MoveExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
			Attach_Broom();
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::BROOM_RIDE_MOVE, CState_Broom_Ride_Move::Create(&Desc));
	}

	{
		CState_Hover::STATE_HOVER_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_HoverEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_HoverExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_HoverExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
			Attach_Broom();
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::HOVER, CState_Hover::Create(&Desc));
	}

	{
		CState_Fly::STATE_FLY_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_FlyEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_FlyExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_FlyExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
			Attach_Broom();
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::FLY, CState_Fly::Create(&Desc));
	}

	{
		CState_Broom_TurboFly::STATE_BROOM_TURBOFLY_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_TurboFlyEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_TurboFlyExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_TurboFlyExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
			Attach_Broom();
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::TURBOFLY, CState_Broom_TurboFly::Create(&Desc));
	}

	{
		CState_Broom_Dismount::STATE_BROOM_DISMOUNT_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_DismountEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_DismountExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_DismountExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::BROOM_DISMOUNT, CState_Broom_Dismount::Create(&Desc));
	}

#pragma endregion


}

void CPlayer::Set_Anim()
{
	m_Animation[STATEANIM::IDLE] = { 894,true }; //266
	m_Animation[STATEANIM::IDLE_AIM] = { 6,true };
	m_Animation[STATEANIM::IDLE_TURN_L] = { 271,false };
	m_Animation[STATEANIM::IDLE_TURN_R] = { 431,false };
	m_Animation[STATEANIM::IDLE_TURN_BWD] = { 269,false };

	m_Animation[STATEANIM::WALK_FWD] = { 626,true };
	m_Animation[STATEANIM::WALK_LEFT] = { 338,false };
	m_Animation[STATEANIM::WALK_RIGHT] = { 505,false };
	m_Animation[STATEANIM::WALK_BWD] = { 353,false };

	m_Animation[STATEANIM::WALK_STOP] = { 345,false };

	m_Animation[STATEANIM::JOG_FWD] = { 168,true };
	m_Animation[STATEANIM::JOG_LEFT] = { 304,false }; // Turn 164
	m_Animation[STATEANIM::JOG_RIGHT] = { 308,false };// Turn 185
	m_Animation[STATEANIM::JOG_BWD] = { 155,false }; // Turn
	m_Animation[STATEANIM::JOG_STOP] = { 447,false };

	m_Animation[STATEANIM::JOG_AIM_LEFT] = { 172,true };
	m_Animation[STATEANIM::JOG_AIM_RIGHT] = { 176,true };
	m_Animation[STATEANIM::JOG_AIM_BWD] = { 167,true };

	m_Animation[STATEANIM::JUMP] = { 206,false };
	m_Animation[STATEANIM::JUMP_JOG] = { 204,false };
	m_Animation[STATEANIM::JUMP_SPRINT] = { 208,false };
	m_Animation[STATEANIM::FALL_LOOP] = { 30,true };

	m_Animation[STATEANIM::SPRINT] = { 600,true };

	m_Animation[STATEANIM::LAND] = { 260,false };
	m_Animation[STATEANIM::LAND_TO_JOG] = { 248,false };
	m_Animation[STATEANIM::LAND_TO_SPRINT] = { 250,false };
	m_Animation[STATEANIM::LAND_HARD] = { 256,false };
	m_Animation[STATEANIM::LAND_MID] = { 258,false };
	m_Animation[STATEANIM::LAND_SOFT] = { 260,false };

	m_Animation[STATEANIM::SLIDE_STOP_R] = { 569,false };
	m_Animation[STATEANIM::SLIDE_START_R] = { 567,false };
	m_Animation[STATEANIM::SLIDE_LOOP_R] = { 565,true };

	m_Animation[STATEANIM::DODGE] = { 879,false };
	m_Animation[STATEANIM::DODGE_BLINK] = { 875,false };

	m_Animation[STATEANIM::SKILL] = { 594,false };
	m_Animation[STATEANIM::SKILL2] = { 992,false };

	m_Animation[STATEANIM::LIGHT_ATTACK] = { 414,false };
	m_Animation[STATEANIM::LIGHT_ATTACK_180_R] = { 810,false };

	m_Animation[STATEANIM::LIGHT_ATTACK_90_L] = { 802,false };
	m_Animation[STATEANIM::LIGHT_ATTACK_90_R1] = { 850,false };
	m_Animation[STATEANIM::LIGHT_ATTACK_90_R2] = { 854,false };
	m_Animation[STATEANIM::LIGHT_ATTACK_90_R3] = { 855,false };

	m_Animation[STATEANIM::SPELL] = { 773,false };
	m_Animation[STATEANIM::SPELL_180_R] = { 838,false };
	m_Animation[STATEANIM::SPELL_90_L] = { 827,false };

	m_Animation[STATEANIM::MAPHELP] = { 123,false };
	m_Animation[STATEANIM::POTION] = { 116,false };
	m_Animation[STATEANIM::ACCIO] = { 760,false };
	m_Animation[STATEANIM::DESCENDO] = { 858,false };
	m_Animation[STATEANIM::DEPULSO] = { 859,false };
	m_Animation[STATEANIM::DIFFINDO] = { 863,false };
	m_Animation[STATEANIM::LUMOS] = { 911,true };
	m_Animation[STATEANIM::LUMOS_STOP] = { 913,false };
	m_Animation[STATEANIM::DISILLUSION_ENTER] = { 586,false };
	m_Animation[STATEANIM::DISILLUSION_EXIT] = { 587,false };
	m_Animation[STATEANIM::ANCIENT_THROW] = { 920,false };
	m_Animation[STATEANIM::AVADA_KEDAVRA] = { 0,false };

	m_Animation[STATEANIM::ANCIENT_LIGHTNING] = { 1002,false };

	m_Animation[STATEANIM::SPELL_FAIL] = { 907,false };

	m_Animation[STATEANIM::PARRY_180] = { 954,false };
	m_Animation[STATEANIM::PARRY2] = { 914,false };
	m_Animation[STATEANIM::PARRY3] = { 916,false };
	m_Animation[STATEANIM::PARRY4] = { 922,false };

	m_Animation[STATEANIM::HIT_L] = { 1210,false };
	m_Animation[STATEANIM::HIT_R] = { 1211,false };
	m_Animation[STATEANIM::FLINCH_BWD] = { 1070,false };
	m_Animation[STATEANIM::FLINCH_FWD] = { 1071,false };
	m_Animation[STATEANIM::FLINCH_LEFT] = { 1072,false };
	m_Animation[STATEANIM::FLINCH_RIGHT] = { 1073,false };
	m_Animation[STATEANIM::KNOCKBACK] = { 1088,false };

	m_Animation[STATEANIM::BROOM_IDLE] = { 711,true };
	m_Animation[STATEANIM::BROOM_FWD] = { 712,true };
	m_Animation[STATEANIM::BROOM_LEFT] = { 714,true };
	m_Animation[STATEANIM::BROOM_RIGHT] = { 715,true };
	m_Animation[STATEANIM::BROOM_DOWN] = { 713,true };
	m_Animation[STATEANIM::BROOM_UP] = { 716,true };
	m_Animation[STATEANIM::BROOM_REVELIO] = { 679,false };

	m_Animation[STATEANIM::BROOM_TURBO_DOWN] = { 717,true };
	m_Animation[STATEANIM::BROOM_TURBO_FWD] = { 718,true };
	m_Animation[STATEANIM::BROOM_TURBO_LEFT] = { 719,true };
	m_Animation[STATEANIM::BROOM_TURBO_RIGHT] = { 720,true };
	m_Animation[STATEANIM::BROOM_TURBO_UP] = { 721,true };

	m_Animation[STATEANIM::BROOM_MOUNT] = { 735,false };
	m_Animation[STATEANIM::BROOM_MOUNT_END] = { 738,false };
	m_Animation[STATEANIM::BROOM_HOVER_START] = { 700,false };
	m_Animation[STATEANIM::BROOM_DISMOUNT] = { 739,false };

	m_Animation[STATEANIM::BROOM_HOVER_IDLE] = { 704,true };
	m_Animation[STATEANIM::BROOM_HOVER_FWD] = { 701,true };
	m_Animation[STATEANIM::BROOM_HOVER_LEFT] = { 702,true };
	m_Animation[STATEANIM::BROOM_HOVER_RIGHT] = { 703,true };
	m_Animation[STATEANIM::BROOM_HOVER_REVELIO] = { 722,false };

	m_Animation[STATEANIM::JOG_AIM_STOP_L] = { 296,false };
	m_Animation[STATEANIM::JOG_AIM_STOP_R] = { 457,false };
	m_Animation[STATEANIM::JOG_AIM_STOP_BWD] = { 445,false };

	m_Animation[STATEANIM::SPELL_LEARNING_FAIL] = { 1286,false };
	m_Animation[STATEANIM::SPELL_LEARNING_LOOP] = { 1287,true };
	m_Animation[STATEANIM::SPELL_LEARNING_START] = { 1288,false };
	m_Animation[STATEANIM::SPELL_LEARNING_SUCCESS] = { 1289,false };
	m_Animation[STATEANIM::SPELL_LEARNING_WANDWAVE] = { 1290,false };

	//루모스 스탑 912

	//713 왼쪽 슬로우
	//714 오른쪽
	//712 아래
	//715 위



	// 호버 앞 700
	// 호버 좌 701
	// 호버 우 702

	//738 착지

}


#pragma endregion State

