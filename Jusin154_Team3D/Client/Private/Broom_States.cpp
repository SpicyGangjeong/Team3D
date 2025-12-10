#include "pch.h"
#include "Broom.h"

#include "GameInstance.h"

#pragma region STATE
#include "State_Idle.h"
#include "State_Move.h"
#pragma endregion

#pragma region States

HRESULT CBroom::InputAction()
{
	if (
		m_pGameInstance->Key_Pressing(DIK_SPACE)
		|| m_pGameInstance->Key_Pressing(DIK_LCONTROL)
		|| m_pGameInstance->Key_Pressing(DIK_E)
		|| m_pGameInstance->Key_Pressing(DIK_R)
		|| m_pGameInstance->Key_Pressing(DIK_Q)
		|| m_pGameInstance->Key_Down(DIK_LSHIFT)
		|| m_pGameInstance->Key_Pressing(DIK_C)
		|| m_pGameInstance->Key_Pressing(DIK_V)
		|| m_pGameInstance->Key_Pressing(DIK_Z)
		|| m_pGameInstance->Key_Pressing(DIK_G)
		|| m_pGameInstance->Key_Pressing(DIK_B)
		)
	{
		return S_OK;
	}
	return E_FAIL;
}

HRESULT CBroom::InputMove()
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

void CBroom::Behavior_IdleEnter() {
	m_pFSM->Enable_State(FSMSTATE::IDLE);
	pair<_uint, _bool> pairAnimInfo;

	pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE_B];
	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
}

HRESULT CBroom::Behavior_IdleExitCheck(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_Animation[STATEANIM::BROOM_MOUNT_B].first;
	pair<_uint, _bool> pairAnimInfo;
	if (SUCCEEDED(InputAction())) {
		if (m_pGameInstance->Key_Down(DIK_B)) {
			pairAnimInfo = m_Animation[STATEANIM::BROOM_MOUNT_B];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
			m_pFSM->Enable_State(FSMSTATE::MOUNT_B);
		}
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
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
		}
	}

	return E_FAIL;
}

void CBroom::Behavior_IdleExit()
{
	m_pFSM->Disable_State(FSMSTATE::IDLE | FSMSTATE::MOUNT_B);
}

void CBroom::Behavior_MoveEnter()
{
	if (!m_bRide)
		return;

	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::MOVE);
	if (m_pFSM->IsEnable_Previous(FSMSTATE::IDLE)) {
		m_pFSM->Enable_State(FSMSTATE::JOG);

		if (SUCCEEDED(InputMove()) || SUCCEEDED(InputAction()))
		{
			if (m_pGameInstance->Key_Pressing(DIK_W))
			{
				pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE_B];
			}
			else if (m_pGameInstance->Key_Pressing(DIK_A))
			{
				pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_LEFT_B];
			}
			else if (m_pGameInstance->Key_Pressing(DIK_D))
			{
				pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_RIGHT_B];
			}
			else if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
			{
				pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_DOWN_B];
			}
			else if (m_pGameInstance->Key_Pressing(DIK_SPACE))
			{
				pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_UP_B];
			}
			else if (m_pGameInstance->Key_Down(DIK_LSHIFT))
			{
				pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_B];
			}

			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
		}
	}
}

HRESULT CBroom::Behavior_MoveExitCheck(_float fTimeDelta)
{
	if (!m_bRide)
		return E_FAIL;

	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrentAnimIndex = m_pModelCom->Get_AnimIndex();
	_bool bFwd = m_pGameInstance->Key_Pressing(DIK_W);
	_bool bLft = m_pGameInstance->Key_Pressing(DIK_A);
	_bool bRht = m_pGameInstance->Key_Pressing(DIK_D);
	_bool bDown = m_pGameInstance->Key_Pressing(DIK_LCONTROL);
	_bool bUp = m_pGameInstance->Key_Pressing(DIK_SPACE);

	
	if (m_pGameInstance->Key_Up(DIK_LSHIFT))
	{
		m_bHoverToggle = !m_bHoverToggle;
	}

	if (m_bHoverToggle)
	{
		if (bFwd)
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_IDLE_B];

			m_fTargetSpeed = m_fHoverMaxSpeed;
			m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fAccel;
			m_pTransformCom->Go_LerpStraight(m_fSpeed, fTimeDelta);
		}
		else if (bLft)
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_LEFT_B];
		}
		else if (bRht)
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_RIGHT_B];

		}
		else if (bDown)
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_DOWN_B];
			m_fTargetSpeed = m_fTurnMaxSpeed;
			m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fAccel;
			m_pTransformCom->Go_LerpDown(m_fSpeed, fTimeDelta);
		}
		else if (bUp)
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_UP_B];
			m_fTargetSpeed = m_fTurnMaxSpeed;
			m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fAccel;
			m_pTransformCom->Go_LerpUp(m_fSpeed, fTimeDelta);
		}
		else {
			pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_STOP_B];
		}

		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
	}
	else{
		m_fTargetSpeed = m_fFlyMaxSpeed;
		m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fAccel;
		m_pTransformCom->Go_LerpStraight(m_fSpeed, fTimeDelta);

		if (SUCCEEDED(InputAction()) || SUCCEEDED(InputMove()))
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
				m_fTargetSpeed = m_fTurnMaxSpeed;
				m_fTurnSpeed += (m_fTurnMaxSpeed - m_fTurnSpeed) * fTimeDelta * m_fAccel;

				if (bUp)
				{
					m_pTransformCom->Go_LerpUp(m_fTurnSpeed, fTimeDelta);
					m_fVerticalSpeed = +m_fTurnSpeed;
				}
				else
				{
					m_pTransformCom->Go_LerpDown(m_fTurnSpeed, fTimeDelta);
					m_fVerticalSpeed = -m_fTurnSpeed;
				}
			}

			if (vInput.x < 0.f)
			{
				m_pTransformCom->Turn(-m_pTransformCom->Get_State(STATE::UP), fTimeDelta * 0.1f);
			}
			else if (vInput.x > 0.f)
			{
				m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * 0.1f);
			}

			_float ax = fabsf(vInput.x);
			_float ay = fabsf(vInput.y);
			_float az = fabsf(vInput.z);

			if (ay > ax && ay > az)
			{
				if (vInput.y > 0)
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_UP_B];
				else
					pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_DOWN_B];
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
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
						Camera_InterpTurn(fTimeDelta);
						pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_B];
					}
				}
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
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
				m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
			}
		}
		else {

			m_fVerticalSpeed += (0.f - m_fVerticalSpeed) * fTimeDelta * m_fDecel;

			if (fabs(m_fVerticalSpeed) > 0.001f)
			{
				if (m_fVerticalSpeed > 0.f)
					m_pTransformCom->Go_LerpUp(m_fVerticalSpeed, fTimeDelta);
				else
					m_pTransformCom->Go_LerpDown(-m_fVerticalSpeed, fTimeDelta);
			}

			pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_B];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
		}
	}

	if (m_bHoverToggle && !SUCCEEDED(InputMove()) && !SUCCEEDED(InputAction()))
	{
		m_fTargetSpeed = 0.f;
			
		m_fSpeed += (m_fTargetSpeed - m_fSpeed) * fTimeDelta * m_fDecel;

		m_pTransformCom->Go_LerpStraight(m_fSpeed, fTimeDelta);

		pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_STOP_B];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 1.f, true);
		if (m_pModelCom->IsFinishedAnim() && iCurrentAnimIndex == m_Animation[STATEANIM::BROOM_HOVER_STOP_B].first)
		{
			m_pFSM->Change_State(FSMSTATE::IDLE);
			return E_FAIL;
		}
		return S_OK;
	}
	return E_FAIL;
}

void CBroom::Behavior_MoveExit()
{
	m_pFSM->Disable_State(FSMSTATE::MOVE | FSMSTATE::SPRINT | FSMSTATE::JOG | FSMSTATE::WALK);
}


void CBroom::Camera_InterpTurn(_float fTimeDelta)
{
	_uint iCurrAnimIndex = m_pModelCom->Get_AnimIndex();

	if (iCurrAnimIndex != m_Animation[STATEANIM::BROOM_FLY_B].first)
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
		m_pTransformCom->Turn(-m_pTransformCom->Get_State(STATE::UP), fTimeDelta*0.1f);
	}
	else if (angleDiff < -Offset)
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(STATE::UP), fTimeDelta * 0.1f);
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
	//Mount 29
	//호버 스탑 6
	//호버 제자리 24
	//플라이 업 17 다운 13 왼쪽15 오른쪽 16

	//호버 다운		25
	//호버 왼쪽 26
	// 호버 오른쪽 27
	//호버 업 28
}
#pragma endregion State


