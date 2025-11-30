#include "pch.h"
#include "Player.h"

#include "GameInstance.h"
#include "CamPosition_Socket.h"
#include "Camera_Gaze.h"
#include "CamPosition_Arm.h"
#include "Wand.h"
#include "Character_Controller.h"
#include "CallBack_Playable_Behavior.h"
#include "CamPosition_Shoulder.h"
#include "CallBack_Playable_HitReport.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Dodge.h"
#include "State_Jump.h"
#include "State_Land.h"
#include "State_Move.h"
#include "State_Combat.h"
#include "State_Broom_Ride.h"
#pragma endregion

#include "EffectPool.h"

#pragma region States

void CPlayer::TestKeyInput(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_F1))
	{
		m_eSpell = STATEANIM::ACCIO;
	}
	if (m_pGameInstance->Key_Down(DIK_F2))
	{
		m_eSpell = STATEANIM::DESCENDO;
	}
	if (m_pGameInstance->Key_Down(DIK_F3))
	{
		m_eSpell = STATEANIM::DEPULSO;
	}
	if (m_pGameInstance->Key_Down(DIK_F4))
	{
		m_eSpell = STATEANIM::DIFFINDO;
	}
	if (m_pGameInstance->Key_Down(DIK_F5))
	{
		m_eSpell = STATEANIM::DISILLUSION_ENTER;
	}
	if (m_pGameInstance->Key_Down(DIK_F6))
	{
		m_eSpell = STATEANIM::DISILLUSION_EXIT;
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
	if (m_pGameInstance->Mouse_Pressing(DIM_RBUTTON)||
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
		else if (m_pGameInstance->Key_Down(DIK_B)) {
			m_pFSM->Change_State(FSMSTATE::BROOM_RIDE);
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
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,m_fAmount);
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
			if (SUCCEEDED(InputMove()))
			{
				pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
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
			if (true == bFoward) {
				pairAnimInfo = m_Animation[STATEANIM::WALK_FWD];
			}
		}
		else {
			m_pFSM->Enable_State(FSMSTATE::JOG);
			m_bSprintToggle = false;
			m_bWalkToggle = false;
			pairAnimInfo = m_Animation[STATEANIM::JOG_FWD];
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
		GUI::Text("%.1f, %.1f", vInputDir.x, vInputDir.y);
		_vector xmvCurLook = XMVector4Normalize(
			XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
		_float2 vCurLook = { XMVectorGetX(xmvCurLook),XMVectorGetZ(xmvCurLook) };

		_float vDir = CMyTools::Get_Direction2D(vCurLook, vInputDir);
		_float absDir = fabsf(vDir);

		_float cross = vCurLook.x * vInputDir.y - vCurLook.y * vInputDir.x;

		_bool bSkipAngleCheck = { false };
		if (m_pFSM->IsEnable(FSMSTATE::JOG | FSMSTATE::WALK)) {
			if (iCurrentAnimIndex != m_Animation[STATEANIM::JOG_FWD].first &&
				iCurrentAnimIndex != m_Animation[STATEANIM::WALK_FWD].first) {
				bSkipAngleCheck = true;
				if (m_pModelCom->IsFinishedAnim() || m_pFSM->IsEnable(FSMSTATE::STOP)|| SUCCEEDED(InputAim())) {
					m_pFSM->Disable_State(FSMSTATE::STOP);
					bSkipAngleCheck = false;
				}
			}
			if (!bSkipAngleCheck) {
				_float absDir = fabsf(vDir);
				if (absDir <= XMConvertToRadians(90.f)) {
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
							pairAnimInfo = m_Animation[STATEANIM::JOG_LEFT];
						}
						
						/*if (m_pFSM->IsEnable(FSMSTATE::WALK))
						{
							pairAnimInfo = m_Animation[STATEANIM::WALK_LEFT];
						}*/
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
							pairAnimInfo = m_Animation[STATEANIM::JOG_RIGHT];
						}
						
						/*if (m_pFSM->IsEnable(FSMSTATE::WALK))
						{
							pairAnimInfo = m_Animation[STATEANIM::WALK_RIGHT];
						}*/
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
						pairAnimInfo = m_Animation[STATEANIM::JOG_BWD];
					}
					else if (m_pFSM->IsEnable(FSMSTATE::WALK))
					{
						pairAnimInfo = m_Animation[STATEANIM::WALK_BWD];
					}
					m_fAmount = 0.5f;
					m_bRatio = true;
				}

				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second,m_fAmount, m_bRatio);
				m_fAmount = 1.f;
			}
		}

		Player_InterpTurn(fTimeDelta);

		return S_OK;
	}

	if (m_pFSM->IsEnable(FSMSTATE::JOG | FSMSTATE::WALK | FSMSTATE::SPRINT)||
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
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_DodgeExitCheck()
{
	if (m_pModelCom->IsFinishedAnim()) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}

	if (SUCCEEDED(InputMove()) && IsCurrentKeyFrame("Throw")) {
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

	if (m_pGameInstance->Key_Down(DIK_R)) {
		m_pFSM->Enable_State(FSMSTATE::SKILL);
		pairAnimInfo = m_Animation[STATEANIM::SKILL];
	}
	else if (m_pGameInstance->Key_Down(DIK_Q)) {
		m_pFSM->Enable_State(FSMSTATE::SKILL2);
		pairAnimInfo = m_Animation[STATEANIM::SKILL2];

		m_pEffectPool->Use_Skill(SKILL_TYPE::PROTEGO, this);
	}
	else if (m_pGameInstance->Mouse_Up(DIM_LBUTTON)) {
		m_pFSM->Enable_State(FSMSTATE::LIGHT_ATTACK);
		pairAnimInfo = m_Animation[STATEANIM::LIGHT_ATTACK];
		m_pEffectPool->Use_Skill(SKILL_TYPE::JAP, this);
		/*m_pModelCom->Anim_Event(0.1f, m_Animation[STATEANIM::LIGHT_ATTACK].first, [this]() {
			m_pEffectPool->Use_Skill(SKILL_TYPE::JAP, this);
			return E_FAIL;
		});*/
	}
	else if (SUCCEEDED(InputSpell())) {
		m_pFSM->Enable_State(FSMSTATE::SPELL);
		if (m_eSpell != STATEANIM::END)
		{
			switch (m_eSpell)
			{
			case STATEANIM::DEPULSO:
				pairAnimInfo = m_Animation[STATEANIM::DEPULSO];
				m_eSpell = STATEANIM::END;
				break;
			case STATEANIM::DIFFINDO:
				pairAnimInfo = m_Animation[STATEANIM::DIFFINDO];
				m_eSpell = STATEANIM::END;
				break;
			case STATEANIM::DISILLUSION_ENTER:
				pairAnimInfo = m_Animation[STATEANIM::DISILLUSION_ENTER];
				m_eSpell = STATEANIM::END;
				break;
			case STATEANIM::DISILLUSION_EXIT:
				pairAnimInfo = m_Animation[STATEANIM::DISILLUSION_EXIT];
				m_eSpell = STATEANIM::END;
				break;
			default:
				pairAnimInfo = m_Animation[STATEANIM::SPELL];
				break;
			}
		}
		else
			pairAnimInfo = m_Animation[STATEANIM::SPELL];
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
		m_pFSM->Enable_State(FSMSTATE::POTION);
		pairAnimInfo = m_Animation[STATEANIM::POTION];
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

					m_pEffectPool->Use_Skill(SKILL_TYPE::JAP, this);

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
			m_pFSM->Enable_State(FSMSTATE::POTION);
			pairAnimInfo = m_Animation[STATEANIM::POTION];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
		}
	}

	if (m_pFSM->IsEnable(FSMSTATE::SPELL) && IsCurrentKeyFrame("Combat"))
	{
		if (m_eSpell != STATEANIM::END)
		{
			switch (m_eSpell)
			{
			case STATEANIM::ACCIO:
			{
				pairAnimInfo = m_Animation[STATEANIM::ACCIO];
			}
				break;
			case STATEANIM::DESCENDO:
				pairAnimInfo = m_Animation[STATEANIM::DESCENDO];

				break;
			default:
				break;
			}

			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);

			m_pModelCom->Anim_Event(0.f, m_Animation[STATEANIM::ACCIO].first, [this]() {
				m_pEffectPool->Use_Skill(SKILL_TYPE::BOMBARDA, this);

				m_eSpell = STATEANIM::END;
				return E_FAIL;
				});

			m_pModelCom->Anim_Event(0.f, m_Animation[STATEANIM::DESCENDO].first, [this]() {
				m_pEffectPool->Use_Skill(SKILL_TYPE::DESCENDO, this);

				m_eSpell = STATEANIM::END;
				return E_FAIL;
				});
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
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}

	return E_FAIL;
}

void CPlayer::Behavior_CombatExit()
{
	m_pFSM->Disable_State(FSMSTATE::COMBAT | FSMSTATE::LIGHT_ATTACK | FSMSTATE::SPELL | FSMSTATE::SKILL |
		FSMSTATE::SKILL2 | FSMSTATE::MAPHELP | FSMSTATE::ANCIENT_THROW | FSMSTATE::POTION);
}

void CPlayer::Behavior_HitEnter()
{
	m_pFSM->Enable_State(FSMSTATE::HIT);
	pair<_uint, _bool> pairAnimInfo;
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> Index(0, 1);
	auto RandIndex = Index(mt);
	if (RandIndex == 0)
		pairAnimInfo = m_Animation[STATEANIM::HIT_R];
	else
		pairAnimInfo = m_Animation[STATEANIM::HIT_L];
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
	pair<_uint, _bool> pairAnimInfo;
	pairAnimInfo = m_Animation[STATEANIM::BROOM_MOUNT];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CPlayer::Behavior_Broom_RideExitCheck()
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (iCurrAnimIndex == m_Animation[STATEANIM::BROOM_MOUNT].first)
	{
		if (m_pModelCom->IsFinishedAnim()) {
			pair<_uint, _bool> pairAnimInfo;
			pairAnimInfo = m_Animation[STATEANIM::BROOM_MOUNT_END];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, true);
		}
		return E_FAIL;
	}
	/*if (m_pModelCom->IsFinishedAnim()) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}*/

	if (SUCCEEDED(InputMove()))
	{
		pair<_uint, _bool> pairAnimInfo;
		pairAnimInfo = Get_AnimInfo(STATEANIM::BROOM_HOVER_IDLE);
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	}
	else
	{
		pair<_uint, _bool> pairAnimInfo;
		pairAnimInfo = Get_AnimInfo(STATEANIM::BROOM_HOVER_IDLE);
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
	}

	if (m_pGameInstance->Key_Down(DIK_N)) {
		m_pFSM->Change_State(FSMSTATE::IDLE);
		return E_FAIL;
	}
	return E_FAIL;
}

void CPlayer::Behavior_Broom_RideExit()
{
	m_pFSM->Disable_State(FSMSTATE::BROOM_RIDE);
}

void CPlayer::Player_InterpTurn(_float fTimeDelta)
{
	_uint iCurrIndex = m_pModelCom->Get_AnimIndex();
	if (iCurrIndex != m_Animation[STATEANIM::JOG_FWD].first&&
		iCurrIndex != m_Animation[STATEANIM::WALK_FWD].first&&
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

	if (angleDiff > 180.f)  angleDiff -= 360.f;
	if (angleDiff < -180.f) angleDiff += 360.f;

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
					_float3	fMove = m_pGameInstance->Get_MouseMove();
					m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * fMove.x * 0.05f);
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
				if (!m_pFSM->IsEnable(FSMSTATE::STOP))
				{
					_float3	fMove = m_pGameInstance->Get_MouseMove();
					m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * fMove.x * 0.05f);
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
		Desc.funcExitCheck = [this](_float fTimeDelta) { return Behavior_DodgeExitCheck(); };
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

#pragma region Behavior_Broom_Ride
	{
		CState_Broom_Ride::STATE_BROOM_RIDE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_RideEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_RideExitCheck(); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_RideExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			if (m_pGameInstance->Key_Pressing(DIK_W))
			{
				_float3	fMove = m_pGameInstance->Get_MouseMove();
				m_pBroomTransform->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * fMove.x * 0.03f);
			}
			_matrix BroomWorld = XMLoadFloat4x4(m_pBroomTransform->Get_WorldMatrixPtr());
			_matrix BoneLocal = XMLoadFloat4x4(m_pBroomModel->Get_BoneMatrixPtr("broomSocket"));

			XMVECTOR Scale, Rot, Trans;
			XMMatrixDecompose(&Scale, &Rot, &Trans, BoneLocal);

			_matrix BoneNoScale = XMMatrixRotationQuaternion(Rot) * XMMatrixTranslationFromVector(Trans);
			static 	_float3 OffsetPos = { 0.f, 0.f, 0.f };
			GUI::DragFloat3("BroomOffset", (_float*)&OffsetPos, 0.01f);
			_matrix Offset = XMMatrixTranslation(OffsetPos.x,
				OffsetPos.y, OffsetPos.z);

			_matrix SocketWorld = BoneNoScale * Offset* BroomWorld;
		
			m_pTransformCom->Set_WorldMatrix(SocketWorld);
			m_pCharacter_Controller->Set_Position(SocketWorld.r[3]);
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::BROOM_RIDE, CState_Broom_Ride::Create(&Desc));
	}

#pragma endregion

}

void CPlayer::Set_Anim()
{
	m_Animation[STATEANIM::IDLE] = { 266,true };
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
	m_Animation[STATEANIM::JOG_LEFT] = { 164,false }; // Turn
	m_Animation[STATEANIM::JOG_RIGHT] = { 185,false };// Turn
	m_Animation[STATEANIM::JOG_BWD] = { 154,false }; // Turn
	m_Animation[STATEANIM::JOG_STOP] = { 289,false }; 

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
	m_Animation[STATEANIM::SPELL] = { 772,false };
	m_Animation[STATEANIM::MAPHELP] = { 122,false };
	m_Animation[STATEANIM::POTION] = { 114,false };
	m_Animation[STATEANIM::ACCIO] = { 417,false };
	m_Animation[STATEANIM::DESCENDO] = { 857,false };
	m_Animation[STATEANIM::DEPULSO] = { 858,false };
	m_Animation[STATEANIM::DIFFINDO] = { 862,false };
	m_Animation[STATEANIM::LUMOS] = { 851,false };
	m_Animation[STATEANIM::DISILLUSION_ENTER] = { 585,false };
	m_Animation[STATEANIM::DISILLUSION_EXIT] = { 586,false };
	m_Animation[STATEANIM::ANCIENT_THROW] = { 919,false };


	m_Animation[STATEANIM::HIT_L] = { 1124,false };
	m_Animation[STATEANIM::HIT_R] = { 1125,false };

	m_Animation[STATEANIM::BROOM_IDLE] = { 679,true }; 
	m_Animation[STATEANIM::BROOM_FWD] = { 680,true }; 

	m_Animation[STATEANIM::BROOM_MOUNT] = { 734,false };
	m_Animation[STATEANIM::BROOM_MOUNT_END] = { 737,false };
	m_Animation[STATEANIM::BROOM_HOVER_START] = { 699,false };
	m_Animation[STATEANIM::BROOM_HOVER_IDLE] = { 703,true };


	m_Animation[STATEANIM::JOG_AIM_STOP_L] = { 295,false };
	m_Animation[STATEANIM::JOG_AIM_STOP_R] = { 456,false };
	m_Animation[STATEANIM::JOG_AIM_STOP_BWD] = { 444,false };
}
#pragma endregion State

