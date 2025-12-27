#include "pch.h"
#include "Broom.h"

#include "GameInstance.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Move.h"
#include "State_Hover.h"
#include "State_Fly.h"
#include "State_Broom_TurboFly.h"
#pragma endregion

#pragma region States

HRESULT CBroom::InputAction()
{
	if (m_Input.bTurbo || m_Input.bHoverToggle)
		return S_OK;

	return E_FAIL;
}


HRESULT CBroom::InputMove()
{
	if (fabs(m_Input.X) > 0.01f ||
		fabs(m_Input.Y) > 0.01f ||
		m_Input.Z > 0.f)
		return S_OK;

	return E_FAIL;
}


HRESULT CBroom::InputKeyUpMove()
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

HRESULT CBroom::InputSpell()
{
	if (
/*		m_pGameInstance->Key_Down(DIK_1)
		||*/ m_pGameInstance->Key_Down(DIK_2)
		|| m_pGameInstance->Key_Down(DIK_3)
			|| m_pGameInstance->Key_Down(DIK_4)
			)
			{
				return S_OK;
	}
	return E_FAIL;
}


HRESULT CBroom::InputAim()
{
	if (m_pGameInstance->Mouse_Pressing(DIM_RBUTTON) ||
		m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		return S_OK;
	}
	return E_FAIL;
}

void CBroom::Behavior_IdleEnter() 
{
	m_pFSM->Enable_State(FSMSTATE::IDLE);
	pair<_uint, _bool> pairAnimInfo;

	pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE_B];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
}

HRESULT CBroom::Behavior_IdleExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();
	pair<_uint, _bool> pairAnimInfo;
	if (m_bRide && !m_pFSM->IsEnable(FSMSTATE::MOUNT_B)) {
		pairAnimInfo = m_Animation[STATEANIM::BROOM_MOUNT_B];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
		m_pFSM->Enable_State(FSMSTATE::MOUNT_B);
	}

	if (SUCCEEDED(InputMove()) || SUCCEEDED(InputAction()))
	{
		if (m_bRide)
		{
			m_pFSM->Change_State(FSMSTATE::MOVE);
			return E_FAIL;
		}
	}

	if (iCurrAnimIndex == m_Animation[STATEANIM::BROOM_MOUNT_B].first)
	{
		if (m_pModelCom->IsFinishedAnim())
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE_B];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
		}
	}

	return S_OK;
}

void CBroom::Behavior_IdleExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLE | FSMSTATE::MOUNT_B);
}

void CBroom::Behavior_MoveEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::MOVE);
}

HRESULT CBroom::Behavior_MoveExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrentAnimIndex = m_pModelCom->Get_AnimIndex();

	_bool bFwd = m_Input.Z > 0.f;
	_bool bLeft = m_Input.X < 0.f;
	_bool bRight = m_Input.X > 0.f;
	_bool bUp = m_Input.Y > 0.f;
	_bool bDown = m_Input.Y < 0.f;
	_bool bShift = m_Input.bHoverToggle;
	_bool bTurbo = m_Input.bTurbo;


	if (SUCCEEDED(InputMove()) || SUCCEEDED(InputAction()))
	{
		if (bFwd)
		{
			m_pFSM->Change_State(FSMSTATE::HOVER);
		}
		else if (bLeft)
		{
			m_pFSM->Change_State(FSMSTATE::HOVER);
		}
		else if (bRight)
		{
			m_pFSM->Change_State(FSMSTATE::HOVER);
		}
		else if (bDown)
		{
			m_pFSM->Change_State(FSMSTATE::HOVER);
		}
		else if (bUp)
		{
			m_pFSM->Change_State(FSMSTATE::HOVER);
		}
		else if (bShift)
		{
			m_pFSM->Change_State(FSMSTATE::FLY);
		}
		else if (bTurbo)
		{
			m_pFSM->Change_State(FSMSTATE::TURBOFLY);
		}
		return E_FAIL;
	}
	
	return S_OK;
}

void CBroom::Behavior_MoveExit()
{
	m_pFSM->Disable_State(FSMSTATE::MOVE | FSMSTATE::SPRINT | FSMSTATE::JOG | FSMSTATE::WALK);
}

void CBroom::Behavior_Broom_HoverEnter()
{
	m_pFSM->Enable_State(FSMSTATE::HOVER);
}

HRESULT CBroom::Behavior_Broom_HoverExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrentAnimIndex = m_pModelCom->Get_AnimIndex();
	_bool bFwd = m_Input.Z > 0.f;
	_bool bLeft = m_Input.X < 0.f;
	_bool bRight = m_Input.X > 0.f;
	_bool bUp = m_Input.Y > 0.f;
	_bool bDown = m_Input.Y < 0.f;
	_bool bShift = m_Input.bHoverToggle;
	_bool bTurbo = m_Input.bTurbo;

	if (m_bHoverToggle)
	{
		_float fTargetVertSpeed = 0.f;
		if (bUp)
			fTargetVertSpeed = m_fTurnMaxSpeed;
		else if (bDown)
			fTargetVertSpeed = -m_fTurnMaxSpeed;
		else
			fTargetVertSpeed = 0.f;

		_float fFactor = (fabsf(fTargetVertSpeed) > fabsf(m_fVerticalSpeed)) ? m_fAccel : m_fDecel;
		m_fVerticalSpeed += (fTargetVertSpeed - m_fVerticalSpeed) * fTimeDelta * fFactor;

		if (fabsf(m_fVerticalSpeed) > 0.001f)
		{
			if (m_fVerticalSpeed > 0.f)
				m_pTransformCom->Go_LerpUp(m_fVerticalSpeed, fTimeDelta);
			else
				m_pTransformCom->Go_LerpDown(-m_fVerticalSpeed, fTimeDelta);
		}

		if (SUCCEEDED(InputAction()) || SUCCEEDED(InputMove()))
		{
			_float3 vInput = { 0.f, 0.f, 0.f };

			if (bFwd)  vInput.z += 1.f;
			if (bLeft)  vInput.x -= 1.f;
			if (bRight)  vInput.x += 1.f;
			if (bUp)   vInput.y += 1.f;
			if (bDown) vInput.y -= 1.f;

			_float ax = fabsf(vInput.x);
			_float ay = fabsf(vInput.y);
			_float az = fabsf(vInput.z);

			_bool bHasInput = !(vInput.x == 0 && vInput.y == 0 && vInput.z == 0);
			_vector vDir = XMVector3Normalize(XMLoadFloat3(&vInput));

			if (bFwd)
			{
				m_fTargetSpeed = m_fHoverMaxSpeed;
				m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fAccel;
				m_pTransformCom->Go_LerpStraight(m_fSpeed, fTimeDelta);
			}

			if (vInput.x < 0.f)
			{
				if (!bFwd)
				{
					m_pTransformCom->Turn(-m_pTransformCom->Get_State(STATE::UP), fTimeDelta * 0.5f);
					m_fTargetSpeed = m_fHoverMaxSpeed;
					m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fAccel;
					m_pTransformCom->Go_LerpStraight(m_fSpeed, fTimeDelta);
				}
				else {
					m_pTransformCom->Turn(-m_pTransformCom->Get_State(STATE::UP), fTimeDelta * 0.5f);
				}

			}
			else if (vInput.x > 0.f)
			{
				if (!bFwd)
				{
					m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * 0.5f);
					m_fTargetSpeed = m_fHoverMaxSpeed;
					m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fAccel;
					m_pTransformCom->Go_LerpStraight(m_fSpeed, fTimeDelta);
				}
				else {
					m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * 0.5f);
				}
			}


			if (ay > ax && ay > az)
			{
				if (vInput.y > 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_UP_B];
				else
					pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_DOWN_B];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
			}
			else if (ax >= az)
			{
				if (ay >= ax)
				{
					if (vInput.y > 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_UP_B];
					else
						pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_DOWN_B];
				}
				else {
					if (vInput.x < 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_LEFT_B];
					else if (vInput.x > 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_RIGHT_B];
					else {
						pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE_B];
					}
				}
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
			}
			else
			{
				if (vInput.y > 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_UP_B];
				else if (vInput.y < 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_DOWN_B];
				else {
					pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE_B];
				}
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
			}
		}
		else {
			m_fTargetSpeed = 0.f;

			m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fDecel;


			m_pTransformCom->Go_LerpStraight(m_fSpeed, fTimeDelta);
			pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_STOP_B];
		}

		if (bShift)
		{
			m_bHoverToggle = false;
		}
		if (bTurbo)
		{
			m_bTurbo = true;
		}

		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
	}

	if (!m_bHoverToggle)
	{
		m_pFSM->Change_State(FSMSTATE::FLY);
		return E_FAIL;
	}

	if (m_bTurbo)
	{
		m_pFSM->Change_State(FSMSTATE::TURBOFLY);
		return E_FAIL;
	}

	return S_OK;
}

void CBroom::Behavior_Broom_HoverExit()
{
	m_pFSM->Disable_State(FSMSTATE::HOVER);
}

void CBroom::Behavior_Broom_FlyEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::FLY);
	m_bHoverToggle = false;
	pairAnimInfo = pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_B];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
}

HRESULT CBroom::Behavior_Broom_FlyExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrentAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	_bool bFwd = m_Input.Z > 0.f;
	_bool bLeft = m_Input.X < 0.f;
	_bool bRight = m_Input.X > 0.f;
	_bool bUp = m_Input.Y > 0.f;
	_bool bDown = m_Input.Y < 0.f;
	_bool bShift = m_Input.bHoverToggle;
	_bool bTurbo = m_Input.bTurbo;

	if (!m_bHoverToggle) {

		if (SUCCEEDED(InputAction()) || SUCCEEDED(InputMove()))
		{
			if (bFwd) {
				m_fTargetSpeed = m_fFlyMaxSpeed;
			}
			else {
				m_fTargetSpeed = m_fFlyTurnMaxSpeed;
			}
		}
		else {
			m_fTargetSpeed = m_fFlyMaxSpeed;
		}

		m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fFlyAccel;
		m_pTransformCom->Go_LerpStraight(m_fSpeed, fTimeDelta);

		_float fTargetVertSpeed = 0.f;

		if (bUp)
			fTargetVertSpeed = m_fTurnMaxSpeed;
		else if (bDown)
			fTargetVertSpeed = -m_fTurnMaxSpeed;
		else
			fTargetVertSpeed = 0.f;

		_float fFactor = (fabsf(fTargetVertSpeed) > fabsf(m_fVerticalSpeed)) ? m_fFlyAccel : m_fFlyDecel;
		m_fVerticalSpeed += (fTargetVertSpeed - m_fVerticalSpeed) * fTimeDelta * fFactor;

		if (fabsf(m_fVerticalSpeed) > 0.001f)
		{
			if (m_fVerticalSpeed > 0.f)
				m_pTransformCom->Go_LerpUp(m_fVerticalSpeed, fTimeDelta);
			else
				m_pTransformCom->Go_LerpDown(-m_fVerticalSpeed, fTimeDelta);
		}

		if (SUCCEEDED(InputAction()) || SUCCEEDED(InputMove()))
		{
			_float3 vInput = { 0.f, 0.f, 0.f };

			if (bFwd)  vInput.z += 1.f;
			if (bLeft)  vInput.x -= 1.f;
			if (bRight)  vInput.x += 1.f;
			if (bUp)   vInput.y += 1.f;
			if (bDown) vInput.y -= 1.f;

			_float ax = fabsf(vInput.x);
			_float ay = fabsf(vInput.y);        
			_float az = fabsf(vInput.z);

			_bool bHasInput = !(vInput.x == 0 && vInput.y == 0 && vInput.z == 0);
			_vector vDir = XMVector3Normalize(XMLoadFloat3(&vInput));

			if (vInput.x < 0.f)
			{
				if (vInput.z <= 0.f)
				{
					m_pTransformCom->Turn(-m_pTransformCom->Get_State(STATE::UP), fTimeDelta * 0.4f);
				}
				else {
					m_pTransformCom->Turn(-m_pTransformCom->Get_State(STATE::UP), fTimeDelta * 0.2f);
				}
			}
			else if (vInput.x > 0.f)
			{
				if (vInput.z <= 0.f)
				{
					m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * 0.4f);
				}
				else {
					m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * 0.2f);
				}
			}


			if (ay > ax && ay > az)
			{
				if (vInput.y > 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_UP_B];
				else
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_DOWN_B];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
			}
			else if (ax >= az)
			{
				if (ay >= ax)
				{
					if (vInput.y > 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_UP_B];
					else
						pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_DOWN_B];
				}
				else {
					if (vInput.x < 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_LEFT_B];
					else if (vInput.x > 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_RIGHT_B];
					else {
						pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_B];
					}
				}
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
			}
			else
			{
				if (vInput.y > 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_UP_B];
				else if (vInput.y < 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_DOWN_B];
				else {
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_B];
				}
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
			}
		}
		else {
			pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_B];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
		}
	}

	if (bShift)
	{
		m_bHoverToggle = true;
	}
	if (bTurbo)
	{
		m_bTurbo = true;
	}

	if (m_bHoverToggle)
	{
		m_fTargetSpeed = 0.f;

		m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fFlyDecel;

		m_pTransformCom->Go_LerpStraight(m_fSpeed, fTimeDelta);

		pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_STOP_B];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);

		if (bShift && iCurrentAnimIndex == m_Animation[STATEANIM::BROOM_HOVER_STOP_B].first)
		{
			m_bHoverToggle = false;
		}
		if (SUCCEEDED(InputAction()) || SUCCEEDED(InputMove()))
		{
			if (fRatio >= 0.5f && iCurrentAnimIndex == m_Animation[STATEANIM::BROOM_HOVER_STOP_B].first)
			{
				m_pFSM->Change_State(FSMSTATE::HOVER);
			}
			return E_FAIL;
		}
		if (m_pModelCom->IsFinishedAnim())
		{
			m_pFSM->Change_State(FSMSTATE::HOVER);
			return E_FAIL;
		}
	}

	if (m_bTurbo)
	{
		m_pFSM->Change_State(FSMSTATE::TURBOFLY);
		return E_FAIL;
	}

	
	return S_OK;
}

void CBroom::Behavior_Broom_FlyExit()
{
	m_pFSM->Disable_State(FSMSTATE::FLY);
}

void CBroom::Behavior_Broom_TurboFlyEnter()
{
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::TURBOFLY);
	m_bTurbo = true;
	m_bHoverToggle = false;
	pairAnimInfo = pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_B];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
}

HRESULT CBroom::Behavior_Broom_TurboFlyExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrentAnimIndex = m_pModelCom->Get_AnimIndex();
	_float fRatio = m_pModelCom->Get_CurrentTrackProgressRatio();

	_bool bFwd = m_Input.Z > 0.f;
	_bool bLeft = m_Input.X < 0.f;
	_bool bRight = m_Input.X > 0.f;
	_bool bUp = m_Input.Y > 0.f;
	_bool bDown = m_Input.Y < 0.f;
	_bool bShift = m_Input.bHoverToggle;
	_bool bTurbo = m_Input.bTurbo;

	if (bShift)
	{
		m_bHoverToggle = true;
	}

	if (bTurbo)
	{
		m_bTurbo = true;
	}
	else {
		m_bTurbo = false;
	}

	if (!m_bHoverToggle)
	{
		if (SUCCEEDED(InputMove()))
		{
			if (bFwd) {
				m_fTargetSpeed = m_fTurboMaxSpeed;
			}
			else {
				m_fTargetSpeed = m_fTurboTurnMaxSpeed;
			}
		}
		else {
			m_fTargetSpeed = m_fTurboMaxSpeed;
		}

		m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fTurboAccel;
		m_pTransformCom->Go_LerpStraight(m_fSpeed, fTimeDelta);
	}


	if (!m_bHoverToggle && m_bTurbo) {

		_float fTargetVertSpeed = 0.f;

		if (bUp)
			fTargetVertSpeed = m_fTurnMaxSpeed;
		else if (bDown)
			fTargetVertSpeed = -m_fTurnMaxSpeed;
		else
			fTargetVertSpeed = 0.f;

		_float fFactor = (fabsf(fTargetVertSpeed) > fabsf(m_fVerticalSpeed)) ? m_fTurboAccel : m_fTurboDecel;
		m_fVerticalSpeed += (fTargetVertSpeed - m_fVerticalSpeed) * fTimeDelta * fFactor;

		if (fabsf(m_fVerticalSpeed) > 0.001f)
		{
			if (m_fVerticalSpeed > 0.f)
				m_pTransformCom->Go_LerpUp(m_fVerticalSpeed, fTimeDelta);
			else
				m_pTransformCom->Go_LerpDown(-m_fVerticalSpeed, fTimeDelta);
		}

		if (SUCCEEDED(InputMove()))
		{
			_float3 vInput = { 0.f, 0.f, 0.f };

			if (bFwd)  vInput.z += 1.f;
			if (bLeft)  vInput.x -= 1.f;
			if (bRight)  vInput.x += 1.f;
			if (bUp)   vInput.y += 1.f;
			if (bDown) vInput.y -= 1.f;

			_float ax = fabsf(vInput.x);
			_float ay = fabsf(vInput.y);
			_float az = fabsf(vInput.z);

			_bool bHasInput = !(vInput.x == 0 && vInput.y == 0 && vInput.z == 0);
			_vector vDir = XMVector3Normalize(XMLoadFloat3(&vInput));

			if (vInput.x < 0.f)
			{
				if (vInput.z <= 0.f)
				{
					Camera_InterpTurn(fTimeDelta * 0.3f);
				}
				else {
					Camera_InterpTurn(fTimeDelta * 0.15f);
				}
			}
			else if (vInput.x > 0.f)
			{
				if (vInput.z <= 0.f)
				{
					Camera_InterpTurn(fTimeDelta * 0.3f);
				}
				else {
					Camera_InterpTurn(fTimeDelta * 0.15f);
				}
			}


			if (ay > ax && ay > az)
			{
				if (vInput.y > 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_UP_B];
				else
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_DOWN_B];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
			}
			else if (ax >= az)
			{
				if (ay >= ax)
				{
					if (vInput.y > 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_UP_B];
					else
						pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_DOWN_B];
				}
				else {
					if (vInput.x < 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_LEFT_B];
					else if (vInput.x > 0)
						pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_RIGHT_B];
					else {
						pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_B];
					}
				}
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
			}
			else
			{
				if (vInput.y > 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_UP_B];
				else if (vInput.y < 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_DOWN_B];
				else {
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_B];
				}
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
			}
		}
		else {
			pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_B];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);
		}
	}

	if (m_bHoverToggle)
	{
		m_fTargetSpeed = 0.f;

		m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fTurboDecel;

		m_pTransformCom->Go_LerpStraight(m_fSpeed, fTimeDelta);

		pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_STOP_B];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true, 1.f, false);

		if (bShift && iCurrentAnimIndex == m_Animation[STATEANIM::BROOM_HOVER_STOP_B].first)
		{
			m_bHoverToggle = false;
		}
		if (SUCCEEDED(InputAction()) || SUCCEEDED(InputMove()))
		{
			if (fRatio >= 0.5f && iCurrentAnimIndex == m_Animation[STATEANIM::BROOM_HOVER_STOP_B].first)
			{
				m_pFSM->Change_State(FSMSTATE::HOVER);
			}
			return E_FAIL;
		}
		if (m_pModelCom->IsFinishedAnim())
		{
			m_pFSM->Change_State(FSMSTATE::HOVER);
			return E_FAIL;
		}
	}

	if (!m_bHoverToggle && !m_bTurbo)
	{
		m_pFSM->Change_State(FSMSTATE::FLY);
		return E_FAIL;
	}

	return S_OK;
}

void CBroom::Behavior_Broom_TurboFlyExit()
{
	m_pFSM->Disable_State(FSMSTATE::TURBOFLY);
}


void CBroom::Camera_InterpTurn(_float fTimeDelta)
{
	if (m_pParentUnit->IsAI())
		return;
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

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

	if (m_Input.Z> 0.f)
		xmvInput += vCameraLook;
	if (m_Input.X > 0.f)
		xmvInput -= vCameraRight;
	if (m_Input.X < 0.f)
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
		m_pTransformCom->Turn(-m_pTransformCom->Get_State(STATE::UP), fTimeDelta * 0.5f);
	}
	else if (angleDiff < -Offset)
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * 0.5f);
	}
}

void CBroom::Add_FSM()
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
		CState_Move::STATE_MOVE_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_MoveEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_MoveExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_MoveExit(); };
		Desc.funcPriorityUpdate = [this](_float fTimeDelta) {
			if (m_bHoverToggle)
			{
				_float3	fMove = m_pGameInstance->Get_MouseMove();
				m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * fMove.x * 0.02f);
			}
			};
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::MOVE, CState_Move::Create(&Desc));
	}

	{
		CState_Hover::STATE_HOVER_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_HoverEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_HoverExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_HoverExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::HOVER, CState_Hover::Create(&Desc));
	}

	{
		CState_Fly::STATE_FLY_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_FlyEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_FlyExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_FlyExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::FLY, CState_Fly::Create(&Desc));
	}

	{
		CState_Broom_TurboFly::STATE_BROOM_TURBOFLY_DESC Desc{};
		Desc.pOwner = this;
		Desc.funcEnterEvent = [this]() { Behavior_Broom_TurboFlyEnter(); };
		Desc.funcExitCheck = [this](_float fTimedelta) { return Behavior_Broom_TurboFlyExitCheck(fTimedelta); };
		Desc.funcExitEvent = [this]() { Behavior_Broom_TurboFlyExit(); };
		Desc.funcPriorityUpdate = nullptr;
		Desc.funcLateUpdate = nullptr;
		m_States.emplace(FSMSTATE::TURBOFLY, CState_Broom_TurboFly::Create(&Desc));
	}
#pragma endregion
}



void CBroom::Set_Anim()
{
	m_Animation[STATEANIM::BROOM_MOUNT_B] = { 29,false };
	m_Animation[STATEANIM::BROOM_HOVER_IDLE_B] = { 24,true };
	m_Animation[STATEANIM::BROOM_HOVER_STOP_B] = { 6,false };

	m_Animation[STATEANIM::BROOM_HOVER_DOWN_B] = { 25,true };
	m_Animation[STATEANIM::BROOM_HOVER_LEFT_B] = { 26,true };
	m_Animation[STATEANIM::BROOM_HOVER_RIGHT_B] = { 27,true };
	m_Animation[STATEANIM::BROOM_HOVER_UP_B] = { 28,true };

	m_Animation[STATEANIM::BROOM_FLY_B] = { 12,true };
	m_Animation[STATEANIM::BROOM_FLY_DOWN_B] = { 13,true };
	m_Animation[STATEANIM::BROOM_FLY_LEFT_B] = { 15,true };
	m_Animation[STATEANIM::BROOM_FLY_RIGHT_B] = { 16,true };
	m_Animation[STATEANIM::BROOM_FLY_UP_B] = { 17,true }; 
}
#pragma endregion State


