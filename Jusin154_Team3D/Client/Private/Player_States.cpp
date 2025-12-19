#include "pch.h"
#include "Player.h"

#include "InfoInstance.h"

#include "GameInstance.h"
#include "CamPosition_Socket.h"
#include "Camera_Gaze.h"
#include "CamPosition_Arm.h"
#include "Wand.h"
#include "Item_Potion.h"
#include "Character_Controller.h"
#include "MapElement_Interactable.h"
#include "CallBack_Playable_Behavior.h"
#include "CamPosition_Shoulder.h"
#include "CallBack_Playable_HitReport.h"
#include "Broom.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Dodge.h"
#include "State_Jump.h"
#include "State_Land.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_LightAttack.h"
#include "State_Spell.h"
#include "State_AncientSpell.h"
#include "State_Shield.h"
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
	m_eSpell = SkillIndex;
}

void CPlayer::Get_UIState(_int UIState)
{
	m_eUIState = UIState;
}

HRESULT CPlayer::InputAction()
{
	if ((m_eUIState != ENUM_CLASS(UI_STATE::SPELL) && 
	m_eUIState !=ENUM_CLASS(UI_STATE::QUEST_CANVES))) {
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
			|| m_pGameInstance->Key_Down(DIK_X)
			|| m_pGameInstance->Key_Down(DIK_Z)
			|| m_pGameInstance->Key_Down(DIK_G)
			|| m_pGameInstance->Key_Down(DIK_B)
			|| m_pGameInstance->Key_Down(DIK_T)
			|| m_pGameInstance->Key_Down(DIK_TAB)
			)
		{
			if (m_pGameInstance->Key_Down(DIK_T)) m_pInfoInstance->Key_Input(ENUM_CLASS(KEYINPUT::INPUT_T));
			if (m_pGameInstance->Key_Down(DIK_TAB)) m_pInfoInstance->Key_Input(ENUM_CLASS(KEYINPUT::INPUT_TAB));
			if (m_pGameInstance->Key_Down(DIK_X)) m_pInfoInstance->Key_Input(ENUM_CLASS(KEYINPUT::INPUT_X));
			if (m_pGameInstance->Key_Down(DIK_G)) m_pInfoInstance->Key_Input(ENUM_CLASS(KEYINPUT::INPUT_G));
			if (m_pGameInstance->Key_Down(DIK_Z)) m_pInfoInstance->Key_Input(ENUM_CLASS(KEYINPUT::INPUT_Z));
			return S_OK;
		}
	}
	else {
		if (m_pGameInstance->Key_Down(DIK_T))
		{
			if (m_pGameInstance->Key_Down(DIK_T)) m_pInfoInstance->Key_Input(ENUM_CLASS(KEYINPUT::INPUT_T));
			return S_OK;
		}
		if (m_pGameInstance->Key_Down(DIK_TAB))
		{
			if (m_pGameInstance->Key_Down(DIK_TAB)) m_pInfoInstance->Key_Input(ENUM_CLASS(KEYINPUT::INPUT_TAB));
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CPlayer::InputMove()
{
	if ((m_eUIState != ENUM_CLASS(UI_STATE::SPELL) &&
		m_eUIState != ENUM_CLASS(UI_STATE::QUEST_CANVES))) {

		if (m_pGameInstance->Key_Pressing(DIK_W)
			|| m_pGameInstance->Key_Pressing(DIK_A)
			|| m_pGameInstance->Key_Pressing(DIK_S)
			|| m_pGameInstance->Key_Pressing(DIK_D))
		{

			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CPlayer::InputKeyUpMove()
{
	if ((m_eUIState != ENUM_CLASS(UI_STATE::SPELL) &&
		m_eUIState != ENUM_CLASS(UI_STATE::QUEST_CANVES))) {
		if (m_pGameInstance->Key_Up(DIK_W)
			|| m_pGameInstance->Key_Up(DIK_A)
			|| m_pGameInstance->Key_Up(DIK_S)
			|| m_pGameInstance->Key_Up(DIK_D))
		{
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT CPlayer::InputSpell()
{
	if ((m_eUIState != ENUM_CLASS(UI_STATE::SPELL) &&
		m_eUIState != ENUM_CLASS(UI_STATE::QUEST_CANVES))) {
		if (
			m_pGameInstance->Key_Down(DIK_1)
			|| m_pGameInstance->Key_Down(DIK_2)
			|| m_pGameInstance->Key_Down(DIK_3)
			|| m_pGameInstance->Key_Down(DIK_4)
			)
		{
			if (m_pGameInstance->Key_Down(DIK_1)) m_pInfoInstance->Key_Input(ENUM_CLASS(KEYINPUT::INPUT_1));
			if (m_pGameInstance->Key_Down(DIK_2)) m_pInfoInstance->Key_Input(ENUM_CLASS(KEYINPUT::INPUT_2));
			if (m_pGameInstance->Key_Down(DIK_3)) m_pInfoInstance->Key_Input(ENUM_CLASS(KEYINPUT::INPUT_3));
			if (m_pGameInstance->Key_Down(DIK_4)) m_pInfoInstance->Key_Input(ENUM_CLASS(KEYINPUT::INPUT_4));

			return S_OK;
		}
	}
	return E_FAIL;
}


HRESULT CPlayer::InputAim()
{
	if (m_pGameInstance->Mouse_Pressing(DIM_RBUTTON) ||
		m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		if (m_pGameInstance->Mouse_Pressing(DIM_RBUTTON)) {m_bAim  = true;}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CPlayer::InputBroom()
{
	if (m_pGameInstance->Key_Pressing(DIK_LCONTROL)
		|| m_pGameInstance->Key_Pressing(DIK_SPACE))
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
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 0.5f, true);
}

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
			m_pFSM->Change_State(FSMSTATE::SHIELD);
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
			Get_PartObject<CItem_Potion>()->Set_Visible(true);
			m_pModelCom->Set_Second_AnimationIndex(ENUM_CLASS(BLEND_BONE::SHOULDER_NECK_L), m_Animation[STATEANIM::POTION].first, m_Animation[STATEANIM::POTION].second);
		}
		else if (m_pGameInstance->Key_Down(DIK_B)) {
			m_pFSM->Change_State(FSMSTATE::BROOM_RIDE);
		}
		else if (m_pGameInstance->Key_Down(DIK_T)) {
			m_pInfoInstance->Change_Canvas();
		}
		else if (m_pGameInstance->Key_Down(DIK_X)) {
			m_pFSM->Change_State(FSMSTATE::ANCIENT_SPELL);
		}
		else if (m_pGameInstance->Key_Down(DIK_Z)) {
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
			return S_OK;

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

		return S_OK;
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
	if (m_pFSM->IsEnable_Previous(FSMSTATE::IDLE | FSMSTATE::DODGE)) {

		if (m_pFSM->IsEnable_Previous(FSMSTATE::IDLE))
		{
			if (true == m_bSprintToggle) {
				m_pFSM->Enable_State(FSMSTATE::SPRINT);
				m_bWalkToggle = false;
				pairAnimInfo = m_Animation[STATEANIM::SPRINT];
				m_bRatio = true;
			}
			else if (true == m_bWalkToggle) {
				m_pFSM->Enable_State(FSMSTATE::WALK);
				m_bSprintToggle = false;
				pairAnimInfo = m_Animation[STATEANIM::WALK_FWD];
				m_bRatio = true;
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
		else {
			if (true == m_bSprintToggle) {
				m_pFSM->Enable_State(FSMSTATE::SPRINT);
				m_bWalkToggle = false;
				pairAnimInfo = m_Animation[STATEANIM::SPRINT];
				m_bRatio = false;
			}
			else if (true == m_bWalkToggle) {
				m_pFSM->Enable_State(FSMSTATE::WALK);
				m_bSprintToggle = false;
				pairAnimInfo = m_Animation[STATEANIM::WALK_FWD];
				m_bRatio = false;
			}
			else
			{
				m_pFSM->Enable_State(FSMSTATE::JOG);
				m_bSprintToggle = false;
				m_bWalkToggle = false;
				pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
				m_bRatio = false;
			}
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

			pairAnimInfo = m_Animation[STATEANIM::WALK_FWD];
			
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
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

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
			m_pFSM->Change_State(FSMSTATE::SHIELD);
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
		else if (m_pGameInstance->Key_Down(DIK_X)) {
			m_pFSM->Change_State(FSMSTATE::ANCIENT_SPELL);
		}
		else if (m_pGameInstance->Key_Down(DIK_Z)) {
			m_pFSM->Change_State(FSMSTATE::COMBAT);
		}
		else if (m_pGameInstance->Key_Down(DIK_G)) {
			Get_PartObject<CItem_Potion>()->Set_Visible(true);
			m_pModelCom->Set_Second_AnimationIndex(ENUM_CLASS(BLEND_BONE::SHOULDER_NECK_L), m_Animation[STATEANIM::POTION].first, m_Animation[STATEANIM::POTION].second);
		}

		else if (m_pGameInstance->Key_Down(DIK_B)) {
			m_pFSM->Change_State(FSMSTATE::BROOM_RIDE);
		}
		else if (m_pGameInstance->Key_Down(DIK_LSHIFT) && !m_pFSM->IsEnable(FSMSTATE::STOP)) {
			m_bSprintToggle = !m_bSprintToggle;
			m_bWalkToggle = false;
			if (m_bSprintToggle)
			{
				m_pFSM->Enable_State(FSMSTATE::SPRINT);
				m_pFSM->Disable_State(FSMSTATE::WALK);
				pairAnimInfo = m_Animation[STATEANIM::SPRINT];
			}
			else
			{
				m_pFSM->Enable_State(FSMSTATE::JOG);
				m_pFSM->Disable_State(FSMSTATE::SPRINT);
				m_pFSM->Disable_State(FSMSTATE::WALK);
				pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			return S_OK;
		}
		else if (m_pGameInstance->Key_Down(DIK_C) && !m_pFSM->IsEnable(FSMSTATE::STOP)) {
			m_bWalkToggle = !m_bWalkToggle;
			m_bSprintToggle = false;
			if (m_bWalkToggle)
			{
				m_pFSM->Enable_State(FSMSTATE::WALK);
				m_pFSM->Disable_State(FSMSTATE::SPRINT);
				pairAnimInfo = m_Animation[STATEANIM::WALK_FWD];
			}
			else
			{
				m_pFSM->Enable_State(FSMSTATE::JOG);
				m_pFSM->Disable_State(FSMSTATE::WALK);
				m_pFSM->Disable_State(FSMSTATE::SPRINT);
				pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			return S_OK;
		}
		return E_FAIL;
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
			return S_OK;

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

		if (m_pFSM->IsEnable(FSMSTATE::JOG | FSMSTATE::WALK| FSMSTATE::SPRINT)) {
	/*		if (iCurrentAnimIndex != m_Animation[STATEANIM::JOG_FWD].first &&
				iCurrentAnimIndex != m_Animation[STATEANIM::WALK_FWD].first&&
				iCurrentAnimIndex != m_Animation[STATEANIM::SPRINT].first&&
				iCurrentAnimIndex != m_Animation[STATEANIM::JOG_AIM_LEFT].first&&
				iCurrentAnimIndex != m_Animation[STATEANIM::JOG_AIM_RIGHT].first&&
				iCurrentAnimIndex != m_Animation[STATEANIM::JOG_AIM_BWD].first) {
				bSkipAngleCheck = true;
*/
				if (m_pModelCom->IsFinishedAnim() || m_pFSM->IsEnable(FSMSTATE::STOP) || SUCCEEDED(InputAim())) {
					m_pFSM->Disable_State(FSMSTATE::STOP);
					bSkipAngleCheck = false;
				}
			//}
			if (!bSkipAngleCheck) {
				_float absDir = fabsf(vDir);
				if (absDir < XMConvertToRadians(80.f)) {
					if (m_bWalkToggle)
					{
						pairAnimInfo = m_Animation[STATEANIM::WALK_FWD];
					}
					else if (m_bSprintToggle)
					{
						pairAnimInfo = m_Animation[STATEANIM::SPRINT];
					}
					else
					{
						pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
					}
				}
				else if (absDir < XMConvertToRadians(160.f)) {
					if (cross > 0)
					{
						if (SUCCEEDED(InputAim()))
						{
							pairAnimInfo = m_Animation[STATEANIM::JOG_AIM_LEFT];
						}
						/*else if (m_pFSM->IsEnable(FSMSTATE::JOG) && !m_pFSM->IsEnable(FSMSTATE::SPRINT))
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
									Add_Event(pairAnimInfo.first,
										[&]() {
											pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
											m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, m_fAmount, m_bRatio, m_fAnimSpeed);
										}, 0.5f);
									m_fAnimSpeed = 1.2f;
								}
							}
						}*/
						else {
							pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
						}

						m_fAmount = 1.f;
						m_bRatio = true;
					}
					else {
						if (SUCCEEDED(InputAim()))
						{
							pairAnimInfo = m_Animation[STATEANIM::JOG_AIM_RIGHT];
						}
						/*else if (m_pFSM->IsEnable(FSMSTATE::JOG) && !m_pFSM->IsEnable(FSMSTATE::SPRINT))
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
									Add_Event(pairAnimInfo.first,
										[&]() {
											pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
											m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, m_fAmount, m_bRatio, m_fAnimSpeed);
										}, 0.5f);
									m_fAnimSpeed = 1.2f;
								}
							}
						}*/
						else {
							pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
						}

						m_fAmount = 1.f;
						m_bRatio = true;
					}
				}
				else {
					if (SUCCEEDED(InputAim()))
					{
						pairAnimInfo = m_Animation[STATEANIM::JOG_AIM_BWD];
					}
					/*else if (m_pFSM->IsEnable(FSMSTATE::JOG) && !m_pFSM->IsEnable(FSMSTATE::SPRINT))
					{
						if (m_pModelCom->IsFinishedAnim() && iCurrentAnimIndex == m_Animation[STATEANIM::JOG_BWD].first)
						{
							pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
						}
						else
						{
							pairAnimInfo = m_Animation[STATEANIM::JOG_BWD];
							m_fAnimSpeed = 1.2f;
						}

					}
					else if (m_pFSM->IsEnable(FSMSTATE::WALK))
					{
						pairAnimInfo = m_Animation[STATEANIM::WALK_BWD];
					*/
					else {
						pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
					}

					m_fAmount = 1.f;
					m_bRatio = true;
				}

				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, m_fAmount, true, m_fAnimSpeed);
				m_fAmount = 1.f;
				m_fAnimSpeed = 1.f;
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
				m_bWalkToggle = false;
				m_pFSM->Disable_State(FSMSTATE::WALK);
				pairAnimInfo = m_Animation[STATEANIM::WALK_STOP];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			}
			else if (m_pFSM->IsEnable(FSMSTATE::SPRINT)) {
				m_bSprintToggle = false;
				m_pFSM->Disable_State(FSMSTATE::SPRINT);
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
		else if (fRatio >= 0.2f)
		{
			m_pFSM->Disable_State(FSMSTATE::STOP);
			m_pFSM->Change_State(FSMSTATE::IDLE);
			return E_FAIL;
		}
	}

	return S_OK;
}

void CPlayer::Behavior_MoveExit()
{
	m_pFSM->Disable_State(FSMSTATE::MOVE | FSMSTATE::SPRINT | FSMSTATE::JOG | FSMSTATE::WALK | FSMSTATE::STOP);
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
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (m_pGameInstance->Key_Down(DIK_B))
	{
		m_pFSM->Change_State(FSMSTATE::BROOM_RIDE);
		return E_FAIL;
	}

	if (fRatio>=0.6f) {
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
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,1.f,false,1.3f);
}

HRESULT CPlayer::Behavior_DodgeExitCheck(_float fTimeDelta)
{
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
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

	if (m_pModelCom->Get_SecondAnimIndex() == m_Animation[STATEANIM::LUMOS].first)
	{
		m_pModelCom->Set_Second_AnimationIndex(ENUM_CLASS(BLEND_BONE::SHOULDER_R) ,-1) ;
		m_eSpell = ENUM_CLASS(SKILL_TYPE::END);
	}
	if (m_pGameInstance->Key_Down(DIK_R)) {
		m_pFSM->Enable_State(FSMSTATE::SKILL);
		pairAnimInfo = m_Animation[STATEANIM::SKILL];
		m_pEffectPool->Use_Skill(SKILL_TYPE::REVELIO, this);
	}
	else if (m_pGameInstance->Key_Down(DIK_Q)) {

		m_pFSM->Change_State(FSMSTATE::SHIELD);
		return;
	}
	else if (m_pGameInstance->Mouse_Up(DIM_LBUTTON)) {
		m_pFSM->Change_State(FSMSTATE::LIGHT_ATTACK);
		return;
	}
	else if (SUCCEEDED(InputSpell())) {
		m_pFSM->Change_State(FSMSTATE::SPELL);
		return;
	}
	else if (m_pGameInstance->Key_Down(DIK_V)) {
		m_pFSM->Enable_State(FSMSTATE::MAPHELP);
		pairAnimInfo = m_Animation[STATEANIM::MAPHELP];
	}
	else if (m_pGameInstance->Key_Down(DIK_X)) {
		m_pFSM->Change_State(FSMSTATE::ANCIENT_SPELL);
		return;
	}
	else if (m_pGameInstance->Key_Down(DIK_Z)) {
		if (nullptr != m_pGrapInteractive) {
			m_pFSM->Enable_State(FSMSTATE::ANCIENT_THROW);
			pairAnimInfo = m_Animation[STATEANIM::ANCIENT_THROW];
		}
		else if (nullptr != m_LockOnInfo.pInteractive) {
			m_pGrapInteractive = m_LockOnInfo.pInteractive;
			SAFE_ADDREF(m_pGrapInteractive);
			m_pGrapInteractive->Set_KinematicFlag(true);
			m_vGrapInteratableLerp.x = 0.f;
			return;
		}

		Add_Event(pairAnimInfo.first,
			[this]() { m_bLookAt = true; },
			0.12f);

		Add_Event(pairAnimInfo.first,
			[this]() { Throwing_Interactive(); },
			0.2f);

		Add_Event(pairAnimInfo.first,
			[this]() {
				m_pGameInstance->SlowMotion(0.1f, 0.35f);
			},
			0.1f);
	}
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_CombatExitCheck()
{
	pair<_uint, _bool> pairAnimInfo;
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (SUCCEEDED(InputAction()) || SUCCEEDED(InputSpell())) {
		if (m_pGameInstance->Key_Down(DIK_R))
		{
			m_pFSM->Enable_State(FSMSTATE::SKILL);
		}
		else if (m_pGameInstance->Key_Down(DIK_Q))
		{
			m_pFSM->Change_State(FSMSTATE::SHIELD);
			return E_FAIL;
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

			if (nullptr != m_pGrapInteractive) {
				m_pFSM->Enable_State(FSMSTATE::ANCIENT_THROW);
				pairAnimInfo = m_Animation[STATEANIM::ANCIENT_THROW];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
				Add_Event(pairAnimInfo.first,
					[this]() { m_bLookAt = true; },
					0.1f);
				Add_Event(pairAnimInfo.first,
					[this]() { 
						Throwing_Interactive(); 	
					},
					0.2f);

				Add_Event(pairAnimInfo.first,
					[this]() {
						m_pGameInstance->SlowMotion(0.1f, 0.35f);
					},
					0.1f);


			}
			else if (nullptr != m_LockOnInfo.pInteractive) {
				m_pGrapInteractive = m_LockOnInfo.pInteractive;
				SAFE_ADDREF(m_pGrapInteractive);
				m_pGrapInteractive->Set_KinematicFlag(true);
				m_vGrapInteratableLerp.x = 0.f;
			}
		}
	}

	if (m_pFSM->IsEnable(FSMSTATE::ANCIENT_THROW))
	{
		if (SUCCEEDED(InputMove()) && fRatio>=0.4f) {
			m_pFSM->Change_State(FSMSTATE::MOVE);
			m_bLookAt = false;
			return E_FAIL;
		}
	}
	else
	{
		if (SUCCEEDED(InputMove()) && fRatio >= 0.3f) {
			m_pFSM->Change_State(FSMSTATE::MOVE);
			m_bLookAt = false;
			return E_FAIL;
		}
	}

	if (m_pModelCom->IsFinishedAnim()) {
		m_bLookAt = false;
		if (SUCCEEDED(InputMove()))
		{
			m_pFSM->Change_State(FSMSTATE::MOVE);
			return E_FAIL;
		}
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT | FSMSTATE::SKILL |
		 FSMSTATE::MAPHELP | FSMSTATE::ANCIENT_THROW);
}

void CPlayer::Behavior_LightAttackEnter()
{
	pair<_uint, _bool> pairAnimInfo;
	m_pModelCom->Set_BlendDuration(0.1f);
	m_pFSM->Enable_State(FSMSTATE::LIGHT_ATTACK);
	pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK];
	if (m_LockOnInfo.pUnit)
	{
		_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
		_vector vTargetLook = m_LockOnInfo.pUnit->Get_Component<CTransform>()->Get_State(STATE::LOOK);

		vLook = XMVector3Normalize(vLook);
		vTargetLook = XMVector3Normalize(vTargetLook);

		_float fDot = XMVectorGetX(XMVector3Dot(vLook, vTargetLook));
		fDot = clamp(fDot, -1.0f, 1.0f);

		_float fAngleRad = acosf(fDot);
		_float Degree = XMConvertToDegrees(fAngleRad);
		_vector vCross = XMVector3Cross(vLook, vTargetLook);
		_float Cross = XMVectorGetY(vCross);

		if (Degree <= 30.f)	
		{
			pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK_180_R];
		}
		else if (Degree <= 120.f)
		{
			if (Cross > 0.f)
			{
				pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK_90_L];
			}
			else
			{
				pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK_90_R2];
			}
		} 
		else{
			Add_Event(pairAnimInfo.first,
				[this]() {m_bLookAt = true;
				},
				0.01f);
		}
	}

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,1.f,true);

	Add_Event(pairAnimInfo.first,
		[this]() {_uint iIndex = 0; m_pEffectPool->Use_Skill(SKILL_TYPE::JAP, Get_PartObject<CWand>(), &iIndex);  },
		0.07f);

	Add_Event(pairAnimInfo.first,
		[this]() { m_pEffectPool->Use_Skill(SKILL_TYPE::JAP_SIDE, Get_PartObject<CWand>());  },
		0.0f);
}

HRESULT CPlayer::Behavior_LightAttackExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo;
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();
	if (m_pGameInstance->Mouse_Up(DIM_LBUTTON))
	{
		_uint iCurr = m_pModelCom->Get_AnimIndex();
		_uint iStart = m_Animation[STATEANIM::LIGHT_ATTACK].first;

		if (iCurr == m_Animation[STATEANIM::LIGHT_ATTACK_180_R].first||
			iCurr == m_Animation[STATEANIM::LIGHT_ATTACK_90_L].first||
			iCurr == m_Animation[STATEANIM::LIGHT_ATTACK_90_R2].first) {
			iCurr = iStart;
		}

		if (iCurr >= iStart && iCurr < iStart + 3)
		{
			_uint iNext = iCurr + 1;
			pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK];
			pairAnimInfo.first = iNext;
			_float fAttackRatio;
			if (pairAnimInfo.first - m_Animation[STATEANIM::LIGHT_ATTACK].first == 3) {
				fAttackRatio = 0.12f;
			}
			else {
				fAttackRatio = 0.07f;
			}

			Add_Event(pairAnimInfo.first,
				[this]() {m_bLookAt = true;
				},
				0.01f);

			if (fRatio >= 0.1f)
			{
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

				Add_Event(iNext,
					[this]() {
						_uint comboIndex = m_pModelCom->Get_AnimIndex() - m_Animation[STATEANIM::LIGHT_ATTACK].first;
						m_pEffectPool->Use_Skill(SKILL_TYPE::JAP, Get_PartObject<CWand>(), &comboIndex);
					},
					fAttackRatio);

				Add_Event(iNext,
					[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::JAP_SIDE,Get_PartObject<CWand>());
					},
					0.0f);
			}
		}
	}
	else if (m_pGameInstance->Key_Down(DIK_X)) {
		m_pFSM->Change_State(FSMSTATE::ANCIENT_SPELL);
		return E_FAIL;
	}
	else if (m_pGameInstance->Key_Down(DIK_Z)) {
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	if (SUCCEEDED(InputMove()) && fRatio >= 0.3f) {
		m_pFSM->Change_State(FSMSTATE::MOVE);
		m_bLookAt = false;
		return E_FAIL;
	}

	if (m_pModelCom->IsFinishedAnim())
	{
		if (SUCCEEDED(InputMove()))
		{
			m_pFSM->Change_State(FSMSTATE::MOVE);
			m_bLookAt = false;
			return E_FAIL;
		}

		m_pFSM->Change_State(FSMSTATE::IDLE);
	}

	return S_OK;

}

void CPlayer::Behavior_LightAttackExit()
{
	m_pFSM->Disable_State(FSMSTATE::LIGHT_ATTACK);
	m_pModelCom->Set_BlendDuration(0.3f);
}

void CPlayer::Behavior_SpellEnter()
{
	pair<_uint, _bool> pairAnimInfo;

	m_pModelCom->Set_BlendDuration(0.1f);
	m_pFSM->Enable_State(FSMSTATE::SPELL);
	pairAnimInfo = m_Animation[STATEANIM::SPELL];
	_float fRatio = 0.f;
	if (m_LockOnInfo.pUnit)
	{
		_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
		_vector vTargetLook = m_LockOnInfo.pUnit->Get_Component<CTransform>()->Get_State(STATE::LOOK);

		vLook = XMVector3Normalize(vLook);
		vTargetLook = XMVector3Normalize(vTargetLook);

		_float fDot = XMVectorGetX(XMVector3Dot(vLook, vTargetLook));
		fDot = clamp(fDot, -1.0f, 1.0f);

		_float fAngleRad = acosf(fDot);
		_float Degree = XMConvertToDegrees(fAngleRad);
		_vector vCross = XMVector3Cross(vLook, vTargetLook);
		_float Cross = XMVectorGetY(vCross);

		if (Degree <= 30.f)
		{
			pairAnimInfo = m_Animation[STATEANIM::SPELL_180_R];
			fRatio = 0.15f;
		}
		else if (Degree <= 120.f)
		{
			if (Cross > 0.f)
			{
				pairAnimInfo = m_Animation[STATEANIM::SPELL_90_L];
			}
			else {
				Add_Event(pairAnimInfo.first,
					[this]() {m_bLookAt = true;
					},
					0.01f);
			}
			fRatio = 0.15f;
		}
		else {
			Add_Event(pairAnimInfo.first,
				[this]() {m_bLookAt = true;
				},
				0.01f);
			fRatio = 0.2f;
		}
	}
	else {
		fRatio = 0.2f;
	}

	if (m_eSpell != ENUM_CLASS(SKILL_TYPE::END) && m_eSpell != -1)
	{
		switch (m_eSpell)
		{
		case ENUM_CLASS(SKILL_TYPE::BOMBARDA):
			Add_Event(pairAnimInfo.first,
				[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::STUPEFY, this); }, /* 임시로 바꿔놓음 */
				fRatio);
			Add_Event(pairAnimInfo.first,
				[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::BOMBARDA_SIDE, Get_PartObject<CWand>()); },
				0.f);
			break;
		case ENUM_CLASS(SKILL_TYPE::DESCENDO):
			Add_Event(pairAnimInfo.first,
				[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::DESCENDO, Get_PartObject<CWand>()); },
				fRatio);
			Add_Event(pairAnimInfo.first,
				[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::DESCENDO_SIDE, Get_PartObject<CWand>()); },
				0.f);
			break;
		case ENUM_CLASS(SKILL_TYPE::LEVIOSO):
			Add_Event(pairAnimInfo.first,
				[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::LEVIOSO, this); },
				fRatio);
			Add_Event(pairAnimInfo.first,
				[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::LEVIOSO_SIDE, Get_PartObject<CWand>()); },
				0.f);
			break;
		case ENUM_CLASS(SKILL_TYPE::ACCIO):
			Add_Event(pairAnimInfo.first,
				[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::ACCIO, this); },
				fRatio);
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
				m_pModelCom->Set_Second_AnimationIndex(ENUM_CLASS(BLEND_BONE::SHOULDER_R),m_Animation[STATEANIM::LUMOS].first, m_Animation[STATEANIM::LUMOS].second);
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
				m_pModelCom->Set_Second_AnimationIndex(ENUM_CLASS(BLEND_BONE::SHOULDER_R),m_Animation[STATEANIM::LUMOS_STOP].first, m_Animation[STATEANIM::LUMOS_STOP].second);
			}
			break;
		default:
			pairAnimInfo = m_Animation[STATEANIM::SPELL_FAIL];
			Add_Event(pairAnimInfo.first,
				[this]() {	m_pEffectPool->Use_Skill(SKILL_TYPE::WAND_END, Get_PartObject<CWand>()); },
				0.2f);
			break;
		}
	}
	else
	{
		pairAnimInfo = m_Animation[STATEANIM::SPELL_FAIL];

		Add_Event(pairAnimInfo.first,
			[this]() {	m_pEffectPool->Use_Skill(SKILL_TYPE::WAND_END, Get_PartObject<CWand>()); },
			0.2f);
	}
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
}

HRESULT CPlayer::Behavior_SpellExitCheck()
{
	pair<_uint, _bool> pairAnimInfo;
	_uint iIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (SUCCEEDED(InputSpell()))
	{
		_uint iCurr = m_pModelCom->Get_AnimIndex();
		_uint iStart = m_Animation[STATEANIM::SPELL].first;

		if (iCurr == m_Animation[STATEANIM::SPELL_180_R].first||
			iCurr == m_Animation[STATEANIM::SPELL_90_L].first) {
			iCurr = iStart;
		}

		if (iCurr >= iStart && iCurr < iStart + 3)
		{
			if (fRatio >= 0.15f)
			{
				_uint iNext = iCurr + 1;
				pairAnimInfo = m_Animation[STATEANIM::SPELL];
				pairAnimInfo.first = iNext;

				Add_Event(pairAnimInfo.first,
					[this]() {m_bLookAt = true;
					},
					0.01f);

				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

				if (m_eSpell != ENUM_CLASS(SKILL_TYPE::END) && m_eSpell != -1)
				{
					_float fRatio = 0.2f;
					if (iNext - iStart == 2) {
						fRatio = 0.17f; 
					}
					else if (iNext - iStart == 3) {
						fRatio = 0.14f;
					}
					else {
						fRatio = 0.2f;
					}
					switch (m_eSpell)
					{
					case ENUM_CLASS(SKILL_TYPE::BOMBARDA):
						Add_Event(pairAnimInfo.first,
							[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::BOMBARDA, this); },
							fRatio);
						Add_Event(pairAnimInfo.first,
							[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::BOMBARDA_SIDE, Get_PartObject<CWand>()); },
							0.f);
						break;
					case ENUM_CLASS(SKILL_TYPE::DESCENDO):
						Add_Event(pairAnimInfo.first,
							[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::DESCENDO, Get_PartObject<CWand>()); },
							fRatio);
						Add_Event(pairAnimInfo.first,
							[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::DESCENDO_SIDE, Get_PartObject<CWand>()); },
							0.f);
						break;
					case ENUM_CLASS(SKILL_TYPE::LEVIOSO):
						Add_Event(pairAnimInfo.first,
							[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::LEVIOSO, this); },
							fRatio);
						Add_Event(pairAnimInfo.first,
							[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::LEVIOSO_SIDE, Get_PartObject<CWand>()); },
							0.f);
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
							m_pModelCom->Set_Second_AnimationIndex(ENUM_CLASS(BLEND_BONE::SHOULDER_R), m_Animation[STATEANIM::LUMOS].first, m_Animation[STATEANIM::LUMOS].second);
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
							m_pModelCom->Set_Second_AnimationIndex(ENUM_CLASS(BLEND_BONE::SHOULDER_R), m_Animation[STATEANIM::LUMOS_STOP].first, m_Animation[STATEANIM::LUMOS_STOP].second);
						}
						break;
					}
				}
				else {
					pairAnimInfo = m_Animation[STATEANIM::SPELL_FAIL];
					m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,1.f,true);
				}
			}
		}
	}
	else if (m_pGameInstance->Key_Down(DIK_X)) {
		m_pFSM->Change_State(FSMSTATE::ANCIENT_SPELL);
		return E_FAIL;
	}
	else if (m_pGameInstance->Key_Down(DIK_Z)) {
		m_pFSM->Change_State(FSMSTATE::COMBAT);
		return E_FAIL;
	}

	if (m_bSpellHit)
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

			m_bSpellHit = false;
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
	}


	if (SUCCEEDED(InputMove()) && fRatio >= 0.3f) {
		m_pFSM->Change_State(FSMSTATE::MOVE);
		m_bLookAt = false;
		return E_FAIL;
	}

	if (m_pModelCom->IsFinishedAnim()) {
		m_bLookAt = false;
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

	return S_OK;
}

void CPlayer::Behavior_SpellExit()
{
	m_pFSM->Disable_State(FSMSTATE::SPELL);

	m_pModelCom->Set_BlendDuration(0.3f);
}

void CPlayer::Behavior_AncientSpellEnter()
{
	pair<_uint, _bool> pairAnimInfo;
	m_pFSM->Enable_State(FSMSTATE::ANCIENT_SPELL);
	pairAnimInfo = m_Animation[STATEANIM::ANCIENT_LIGHTNING_CHARGE];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

	Add_Event(pairAnimInfo.first,
		[&]() {	pairAnimInfo = m_Animation[STATEANIM::ANCIENT_LIGHTNING];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second); },
		0.5f);
}

HRESULT CPlayer::Behavior_AncientSpellExitCheck()
{
	pair<_uint, _bool> pairAnimInfo;
	_int iCurrAnim = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (SUCCEEDED(InputMove()) && fRatio >= 0.3f) {
		m_pFSM->Change_State(FSMSTATE::MOVE);
		m_bLookAt = false;
		return E_FAIL;
	}

	if (m_pModelCom->IsFinishedAnim()) {
		m_bLookAt = false;
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
	return S_OK;
}

void CPlayer::Behavior_AncientSpellExit()
{
	m_pFSM->Disable_State(FSMSTATE::ANCIENT_SPELL);
}

void CPlayer::Behavior_ShieldEnter()
{
	pair<_uint, _bool> pairAnimInfo;

	if (!m_bShield)
	{
		m_pFSM->Enable_State(FSMSTATE::SHIELD);
		pairAnimInfo = m_Animation[STATEANIM::SKILL2];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f);

		Add_Event(pairAnimInfo.first,
			[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::PROTEGO, this); },
			0.1f);

	}
}

HRESULT CPlayer::Behavior_ShieldExitCheck()
{
	pair<_uint, _bool> pairAnimInfo;
	_int iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	if (m_bShield)
	{

		if (m_pGameInstance->Key_Pressing(DIK_Q))
		{
			m_pFSM->Enable_State(FSMSTATE::PARRY);
			if (m_LockOnInfo.pUnit)
			{
				_vector vLook = m_pTransformCom->Get_State(STATE::LOOK);
				_vector vTargetLook = m_LockOnInfo.pUnit->Get_Component<CTransform>()->Get_State(STATE::LOOK);

				vLook = XMVector3Normalize(vLook);
				vTargetLook = XMVector3Normalize(vTargetLook);

				_float fDot = XMVectorGetX(XMVector3Dot(vLook, vTargetLook));
				fDot = clamp(fDot, -1.0f, 1.0f);

				_float fAngleRad = acosf(fDot);
				_float Degree = XMConvertToDegrees(fAngleRad);
				_vector vCross = XMVector3Cross(vLook, vTargetLook);
				_float Cross = XMVectorGetY(vCross);

				if (Degree <= 45.f)
				{
					pairAnimInfo = m_Animation[STATEANIM::PARRY_180];
				}
				else if (Degree <= 120.f)
				{
					if (Cross > 0.f)
					{
						pairAnimInfo = m_Animation[STATEANIM::PARRY2];

					}
					else
					{
						pairAnimInfo = m_Animation[STATEANIM::PARRY3];
					}
				}
				else {
					pairAnimInfo = m_Animation[STATEANIM::PARRY4];
				}
				Add_Event(pairAnimInfo.first,
					[this]() {m_bLookAt = true;
					},
					0.01f);
			}

			Start_CameraShake(0.3f, 2.f);
			m_pGameInstance->SlowMotion(0.8f, 0.3f);
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

			Add_Event(pairAnimInfo.first,
				[this]() {m_pEffectPool->Use_Skill(SKILL_TYPE::STUPEFY, this);  },
				0.2f);

			m_bShield = false;
			return S_OK;

		}
	}
	
	if (m_pModelCom->IsFinishedAnim()) {
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

	if (m_pFSM->IsEnable(FSMSTATE::PARRY))
	{
		if (fRatio >= 0.35f)
		{
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

void CPlayer::Behavior_ShieldExit()
{
	m_pFSM->Disable_State(FSMSTATE::SHIELD);
	m_pFSM->Disable_State(FSMSTATE::PARRY);
	m_bShield = false;
	m_bLookAt = false;
}

void CPlayer::Behavior_HitEnter()
{
	m_pBroom->Set_Ride(false);
	m_pFSM->Enable_State(FSMSTATE::HIT);
	pair<_uint, _bool> pairAnimInfo;
	_int RandIndex = m_pGameInstance->Real_Random_Int(0, 2);
	switch (RandIndex)
	{
	case 0:
		pairAnimInfo = m_Animation[STATEANIM::HIT_R];
		break;
	case 1:
		pairAnimInfo = m_Animation[STATEANIM::HIT_L];
		break;
	case 2:
		pairAnimInfo = m_Animation[STATEANIM::KNOCKBACK];
		break;
	default:
		break;
	}

	if (m_fHitDegree >= 90.f)
	{
		pairAnimInfo = m_Animation[STATEANIM::FLINCH_BWD];
	}
	else if (m_fHitDegree >= 45.f) {

		if (m_fHitCross > 0.f) {
			pairAnimInfo = m_Animation[STATEANIM::FLINCH_LEFT];
		}
		else {
			pairAnimInfo = m_Animation[STATEANIM::FLINCH_RIGHT];
		}
	}
	else  if(m_fHitDegree>=0.f){
		pairAnimInfo = m_Animation[STATEANIM::FLINCH_FWD];
	}

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_HitExitCheck()
{
	if (m_pModelCom->IsFinishedAnim()) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}
	return S_OK;
}

void CPlayer::Behavior_HitExit()
{
	m_pFSM->Disable_State(FSMSTATE::HIT);
	m_bLookAt = false;
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
	else if(m_pBroom->Get_Turbo()) {
		m_pFSM->Change_State(FSMSTATE::TURBOFLY);
		return E_FAIL;
	}
	else{
		m_pFSM->Change_State(FSMSTATE::FLY);
		return E_FAIL;
	}
	
	return S_OK;
}

void CPlayer::Behavior_Broom_Ride_MoveExit()
{
	m_pFSM->Disable_State(FSMSTATE::BROOM_RIDE_MOVE);
}

void CPlayer::Behavior_Broom_HoverEnter()
{
	m_pFSM->Enable_State(FSMSTATE::HOVER);
}

HRESULT CPlayer::Behavior_Broom_HoverExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

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
}

HRESULT CPlayer::Behavior_Broom_FlyExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

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
				if (vInput.y > 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_UP];
				else if (vInput.y < 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_DOWN];
			}
			else if (ax >= az)
			{
				if (ay >= ax)
				{
					if (vInput.y > 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_UP];
					else if (vInput.y < 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_DOWN];
				}
				else {
					if (vInput.x < 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_LEFT];
					else if (vInput.x > 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_RIGHT];
					else {
						pairAnimInfo = m_Animation[STATEANIM::BROOM_FWD];
					}
				}
			}
			else
			{
				if (vInput.y > 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_UP];
				else if (vInput.y < 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_DOWN];
				else {
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FWD];
				}
			}
		}
		else {
			pairAnimInfo = Get_AnimInfo(STATEANIM::BROOM_IDLE);
		}

		if (m_pGameInstance->Key_Down(DIK_R))
		{
			pairAnimInfo = Get_AnimInfo(STATEANIM::BROOM_REVELIO);
			Add_Event(pairAnimInfo.first,
				[this]() {
					m_pEffectPool->Use_Skill(SKILL_TYPE::REVELIO, this);
				}, 0.2f);
		}

		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
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
}

void CPlayer::Behavior_Broom_TurboFlyEnter()
{
	m_pFSM->Enable_State(FSMSTATE::TURBOFLY);
}

HRESULT CPlayer::Behavior_Broom_TurboFlyExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;

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
				else if(vInput.y<0.f)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_DOWN];
			}
			else if (ax >= az)
			{
				if (ay >= ax)
				{
					if (vInput.y > 0.f)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_UP];
					else if(vInput.y<0.f)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_TURBO_DOWN];
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
		}
		else {
			pairAnimInfo = Get_AnimInfo(STATEANIM::BROOM_TURBO_FWD);
		}

		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,1.f,true,1.f,false);
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
}

void CPlayer::Behavior_Broom_DismountEnter()
{
	pair<_uint, _bool> pairAnimInfo;
	m_bOnce = false;
	pairAnimInfo = Get_AnimInfo(STATEANIM::BROOM_DISMOUNT);
	m_pFSM->Enable_State(FSMSTATE::BROOM_DISMOUNT);
	m_pBroom->Set_Ride(false);
	_vector look = XMVector3Normalize(m_pTransformCom->Get_State(STATE::LOOK));

	_vector worldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector right = XMVector3Normalize(XMVector3Cross(worldUp, look));
	_vector up = XMVector3Normalize(XMVector3Cross(look, right));

	m_pTransformCom->Set_State(STATE::RIGHT, right);
	m_pTransformCom->Set_State(STATE::UP, up);
	m_pTransformCom->Set_State(STATE::LOOK, look);
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,1.f,false,1.6f);
}

HRESULT CPlayer::Behavior_Broom_DismountExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;
	if (iCurrAnimIndex == m_Animation[STATEANIM::BROOM_DISMOUNT].first)
	{
		m_BroomScale.x -= (m_TargetScale.x - m_BroomScale.x) * fTimeDelta * m_fScaleSmoothSpeed;
		m_BroomScale.y -= (m_TargetScale.y - m_BroomScale.y) * fTimeDelta * m_fScaleSmoothSpeed;
		m_BroomScale.z -= (m_TargetScale.z - m_BroomScale.z) * fTimeDelta * m_fScaleSmoothSpeed;
		if (m_BroomScale.x >= 0.1f)
		{
			m_pBroomTransform->Set_Scale(m_BroomScale);
		}
		if (m_pModelCom->IsFinishedAnim()) {
			m_BroomScale = { 0.f,0.f,0.f };
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

void CPlayer::Throwing_Interactive()
{
	if (nullptr == m_pGrapInteractive) {
		return;
	} _vector vDir = {}; _float vDistance = 45.f;
	CRigidBody_Dynamic* pBody = m_pGrapInteractive->Get_Component<CRigidBody_Dynamic>();
	if (nullptr != m_LockOnInfo.pUnit) {
		vDir = XMVector3Normalize(m_LockOnInfo.pUnit->Get_LockOnPos() - m_pGrapInteractive->Get_LockOnPos());
	}
	else {
		vDir = m_pTransformCom->Get_State(STATE::LOOK);
	}
	m_pGrapInteractive->Set_KinematicFlag(false);
	
	pBody->Add_Force(pBody->Get_Mass() * vDir * (vDistance / 1.5f), PSX::PxForceMode::eIMPULSE);
	SAFE_RELEASE(m_pGrapInteractive);
}

void CPlayer::Attach_Broom()
{
	_matrix BroomWorld = XMLoadFloat4x4(m_pBroomTransform->Get_WorldMatrixPtr());
	_matrix BoneLocal = XMLoadFloat4x4(m_pBroomModel->Get_BoneMatrixPtr("broomSocket"));

	_vector Scale, Rot, Trans;

	XMMatrixDecompose(&Scale, &Rot, &Trans, BoneLocal);

	_matrix BoneNoScale = XMMatrixRotationQuaternion(Rot) * XMMatrixTranslationFromVector(Trans);

	m_OffsetPos = { 0.f,0.9f,0.f };

	_matrix Offset = XMMatrixTranslation(m_OffsetPos.x,
		m_OffsetPos.y, m_OffsetPos.z);

	_matrix SocketWorld = BoneNoScale * Offset * BroomWorld;

	_matrix FixRot = XMMatrixRotationY(XMConvertToRadians(180.f));

	_matrix FinalWorld = FixRot * SocketWorld;

	m_pTransformCom->Set_WorldMatrix(FinalWorld);
	m_pCharacter_Controller->Set_Position(FinalWorld.r[3]);
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
				if ((m_eUIState != ENUM_CLASS(UI_STATE::SPELL))) {
					if (!m_pFSM->IsEnable(FSMSTATE::STOP))
					{
						_float3	fMove = m_pGameInstance->Get_MouseMove();
						m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * fMove.x * 0.05f);
					}
				}
			}
			};

		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::MOVE, CState_Move::Create(&Desc));
	}
#pragma endregion
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
		CState_AncientSpell::STATE_ANCIENTSPELL_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_AncientSpellEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_AncientSpellExitCheck(); };
		Desc.funcExitEvent = [this]() { Behavior_AncientSpellExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::ANCIENT_SPELL, CState_AncientSpell::Create(&Desc));
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
#pragma region Behavior_Combat_Focus

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
				m_pBroomTransform->Set_WorldMatrix(m_pTransformCom->Get_XMWorldMatrix());
				m_pBroomTransform->Set_State(STATE::POSITION, m_pBroomTransform->Get_State(STATE::POSITION) + XMVectorSet(0.f,2.f,0.f,0.f));
				_float3 vScale = { 0.3f,0.3f,0.3f };
				m_pBroomTransform->Set_Scale(vScale);
				m_bOnce = true;
			}

			m_BroomScale.x += (m_TargetScale.x - m_BroomScale.x) * fTimeDelta * m_fScaleSmoothSpeed;
			m_BroomScale.y += (m_TargetScale.y - m_BroomScale.y) * fTimeDelta * m_fScaleSmoothSpeed;
			m_BroomScale.z += (m_TargetScale.z - m_BroomScale.z) * fTimeDelta * m_fScaleSmoothSpeed;
			if (m_BroomScale.x <= 1.f)
			{
				m_pBroomTransform->Set_Scale(m_BroomScale);
			}
			_matrix BroomWorld = XMLoadFloat4x4(m_pBroomTransform->Get_WorldMatrixPtr());
			_matrix BoneLocal = XMLoadFloat4x4(m_pBroomModel->Get_BoneMatrixPtr("broomSocket"));

			_vector Scale, Rot, Trans;

			XMMatrixDecompose(&Scale, &Rot, &Trans, BoneLocal);

			_matrix BoneNoScale = XMMatrixRotationQuaternion(Rot) * XMMatrixTranslationFromVector(Trans);

			m_OffsetPos = { 0.f,0.9f,0.f };

			_matrix Offset = XMMatrixTranslation(m_OffsetPos.x,
				m_OffsetPos.y, m_OffsetPos.z);

			_matrix SocketWorld = BoneNoScale * Offset * BroomWorld;

			_matrix FixRot = XMMatrixRotationY(XMConvertToRadians(180.f));

			_matrix FinalWorld = FixRot * SocketWorld;

			m_pTransformCom->Set_WorldMatrix(FinalWorld);
			m_pCharacter_Controller->Set_Position(FinalWorld.r[3]);

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
	m_Animation[STATEANIM::IDLE] = { 893,true }; //266
	m_Animation[STATEANIM::IDLE_AIM] = { 5,true };
	m_Animation[STATEANIM::IDLE_TURN_L] = { 270,false };
	m_Animation[STATEANIM::IDLE_TURN_R] = { 430,false };
	m_Animation[STATEANIM::IDLE_TURN_BWD] = { 268,false };

	m_Animation[STATEANIM::WALK_FWD] = { 625,true };
	m_Animation[STATEANIM::WALK_LEFT] = { 337,false };
	m_Animation[STATEANIM::WALK_RIGHT] = { 504,false };
	m_Animation[STATEANIM::WALK_BWD] = { 352,false };

	m_Animation[STATEANIM::WALK_STOP] = { 344,false };

	m_Animation[STATEANIM::JOG_FWD] = { 167,true };
	m_Animation[STATEANIM::JOG_LEFT] = { 303,false }; // Turn 164
	m_Animation[STATEANIM::JOG_RIGHT] = { 307,false };// Turn 185
	m_Animation[STATEANIM::JOG_BWD] = { 154,false }; // Turn
	m_Animation[STATEANIM::JOG_STOP] = { 446,false };

	m_Animation[STATEANIM::JOG_AIM_LEFT] = { 171,true };
	m_Animation[STATEANIM::JOG_AIM_RIGHT] = { 175,true };
	m_Animation[STATEANIM::JOG_AIM_BWD] = { 166,true };

	m_Animation[STATEANIM::JUMP] = { 205,false };
	m_Animation[STATEANIM::JUMP_JOG] = { 203,false };
	m_Animation[STATEANIM::JUMP_SPRINT] = { 207,false };

	m_Animation[STATEANIM::SPRINT] = { 599,true };

	m_Animation[STATEANIM::LAND] = { 259,false };

	m_Animation[STATEANIM::DODGE] = { 878,false };
	m_Animation[STATEANIM::DODGE_BLINK] = { 876,true };

	m_Animation[STATEANIM::SKILL] = { 593,false };
	m_Animation[STATEANIM::SKILL2] = { 991,false };

	m_Animation[STATEANIM::LIGHT_ATTACK] = { 413,false };
	m_Animation[STATEANIM::LIGHT_ATTACK_180_R] = { 809,false };

	m_Animation[STATEANIM::LIGHT_ATTACK_90_L] = { 801,false };
	m_Animation[STATEANIM::LIGHT_ATTACK_90_R1] = { 849,false };
	m_Animation[STATEANIM::LIGHT_ATTACK_90_R2] = { 853,false };
	m_Animation[STATEANIM::LIGHT_ATTACK_90_R3] = { 854,false };

	m_Animation[STATEANIM::SPELL] = { 772,false };
	m_Animation[STATEANIM::SPELL_180_R] = { 837,false }; // 837 805
	m_Animation[STATEANIM::SPELL_90_L] = { 826,false };

	m_Animation[STATEANIM::MAPHELP] = { 122,false };
	m_Animation[STATEANIM::POTION] = { 115,false }; // 114 포션 // 909 루모스
	m_Animation[STATEANIM::ACCIO] = { 759,false };
	m_Animation[STATEANIM::DESCENDO] = { 857,false };
	m_Animation[STATEANIM::DEPULSO] = { 858,false };
	m_Animation[STATEANIM::DIFFINDO] = { 862,false };
	m_Animation[STATEANIM::LUMOS] = { 910,true };
	m_Animation[STATEANIM::LUMOS_STOP] = { 912,false };
	m_Animation[STATEANIM::DISILLUSION_ENTER] = { 585,false };
	m_Animation[STATEANIM::DISILLUSION_EXIT] = { 586,false };
	m_Animation[STATEANIM::ANCIENT_THROW] = { 919,false };

	m_Animation[STATEANIM::ANCIENT_LIGHTNING_CHARGE] = { 755,false };
	m_Animation[STATEANIM::ANCIENT_LIGHTNING] = { 859,false }; // 859

	m_Animation[STATEANIM::SPELL_FAIL] = { 906,false };

	m_Animation[STATEANIM::PARRY_180] = { 947,false };
	m_Animation[STATEANIM::PARRY2] = { 913,false };
	m_Animation[STATEANIM::PARRY3] = { 915,false };
	m_Animation[STATEANIM::PARRY4] = { 921,false };


	m_Animation[STATEANIM::HIT_L] = { 1200,false };
	m_Animation[STATEANIM::HIT_R] = { 1201,false };
	m_Animation[STATEANIM::FLINCH_BWD] = { 1060,false };
	m_Animation[STATEANIM::FLINCH_FWD] = { 1061,false };
	m_Animation[STATEANIM::FLINCH_LEFT] = { 1062,false };
	m_Animation[STATEANIM::FLINCH_RIGHT] = { 1063,false };
	m_Animation[STATEANIM::KNOCKBACK] = { 1078,false };

	m_Animation[STATEANIM::BROOM_IDLE] = { 710,true };
	m_Animation[STATEANIM::BROOM_FWD] = { 711,true };
	m_Animation[STATEANIM::BROOM_LEFT] = { 713,true };
	m_Animation[STATEANIM::BROOM_RIGHT] = { 714,true };
	m_Animation[STATEANIM::BROOM_DOWN] = { 712,true };
	m_Animation[STATEANIM::BROOM_UP] = { 715,true };
	m_Animation[STATEANIM::BROOM_REVELIO] = { 678,false };

	m_Animation[STATEANIM::BROOM_TURBO_DOWN] = { 716,true };
	m_Animation[STATEANIM::BROOM_TURBO_FWD] = { 717,true };
	m_Animation[STATEANIM::BROOM_TURBO_LEFT] = { 718,true };
	m_Animation[STATEANIM::BROOM_TURBO_RIGHT] = { 719,true };
	m_Animation[STATEANIM::BROOM_TURBO_UP] = { 720,true };

	m_Animation[STATEANIM::BROOM_MOUNT] = { 734,false };
	m_Animation[STATEANIM::BROOM_MOUNT_END] = { 737,false };
	m_Animation[STATEANIM::BROOM_HOVER_START] = { 699,false };
	m_Animation[STATEANIM::BROOM_DISMOUNT] = { 738,false };


	m_Animation[STATEANIM::BROOM_HOVER_IDLE] = { 703,true };
	m_Animation[STATEANIM::BROOM_HOVER_FWD] = { 700,true };
	m_Animation[STATEANIM::BROOM_HOVER_LEFT] = { 701,true };
	m_Animation[STATEANIM::BROOM_HOVER_RIGHT] = { 702,true };
	m_Animation[STATEANIM::BROOM_HOVER_REVELIO] = { 721,false };

	m_Animation[STATEANIM::JOG_AIM_STOP_L] = { 295,false };
	m_Animation[STATEANIM::JOG_AIM_STOP_R] = { 456,false };
	m_Animation[STATEANIM::JOG_AIM_STOP_BWD] = { 444,false };

	//루모스 스탑 912

	//713 왼쪽 슬로우
	//714 오른쪽
	//712 아래
	//715 위



	// 호버 앞 700
	// 호버 좌 701
	// 호버 우 702

	//738 착지

	//906 마법 실패



	// 821 90도 L
	// 849 90도 R
	// 853 90도 R
	// 854 90도 R

	// 915 

	// 716 아래 앞 왼 오 업
	// 947

	// 755 기모으기 859
}


#pragma endregion State

