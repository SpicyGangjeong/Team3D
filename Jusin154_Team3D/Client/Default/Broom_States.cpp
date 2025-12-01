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
		|| m_pGameInstance->Key_Pressing(DIK_LSHIFT)
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

	m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
}

HRESULT CBroom::Behavior_IdleExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo;
	if (SUCCEEDED(InputAction())) {
		if (m_pGameInstance->Key_Down(DIK_B)) {
			pairAnimInfo = m_Animation[STATEANIM::BROOM_MOUNT_B];
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
			m_pFSM->Enable_State(FSMSTATE::MOUNT_B);
		}
	}

	if (SUCCEEDED(InputMove()) && m_pFSM->IsEnable(FSMSTATE::MOUNT_B))
	{
		m_pFSM->Change_State(FSMSTATE::MOVE);
		return E_FAIL;
	}
	if (SUCCEEDED(InputMove()))
	{
		if (m_pFSM->IsEnable_Previous(FSMSTATE::MOVE))
		{
			m_pFSM->Change_State(FSMSTATE::MOVE);
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
	pair<_uint, _bool> pairAnimInfo = {};
	m_pFSM->Enable_State(FSMSTATE::MOVE);
	if (m_pFSM->IsEnable_Previous(FSMSTATE::IDLE)) {
		m_pFSM->Enable_State(FSMSTATE::JOG);

		if (SUCCEEDED(InputMove()) || SUCCEEDED(InputAction()))
		{
			if (m_pGameInstance->Key_Pressing(DIK_W))
			{
				pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_B];
			}
			else if (m_pGameInstance->Key_Pressing(DIK_A))
			{
				pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_LEFT_B];
			}
			else if (m_pGameInstance->Key_Pressing(DIK_D))
			{
				pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_RIGHT_B];
			}
			else if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
			{
				pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_DOWN_B];
			}
			else if (m_pGameInstance->Key_Pressing(DIK_SPACE))
			{
				pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_UP_B];
			}
			m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 0.5f, true);
		}
	}


}

HRESULT CBroom::Behavior_MoveExitCheck(_float fTimeDelta)
{
	pair<_uint, _bool> pairAnimInfo = {};
	_uint iCurrentAnimIndex = m_pModelCom->Get_AnimIndex();

	if (SUCCEEDED(InputMove()) || SUCCEEDED(InputAction()))
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_B];
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		else if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_LEFT_B];
		}
		else if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_RIGHT_B];
		}
		else if (m_pGameInstance->Key_Pressing(DIK_LCONTROL))
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_DOWN_B];
		}
		else if (m_pGameInstance->Key_Pressing(DIK_SPACE))
		{
			pairAnimInfo = m_Animation[STATEANIM::BROOM_FLY_UP_B];
		}
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second, 0.5f, true);
		return S_OK;
	}
	else
	{
		pairAnimInfo = m_Animation[STATEANIM::BROOM_HOVER_STOP_B];
		m_pModelCom->Set_AnimationIndex(pairAnimInfo.first, pairAnimInfo.second);
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
			Desc.funcPriorityUpdate = nullptr;
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
	m_Animation[STATEANIM::BROOM_FLY_B] = { 5,true };
	m_Animation[STATEANIM::BROOM_FLY_DOWN_B] = { 13,true };
	m_Animation[STATEANIM::BROOM_FLY_LEFT_B] = { 15,true };
	m_Animation[STATEANIM::BROOM_FLY_RIGHT_B] = { 16,true };
	m_Animation[STATEANIM::BROOM_FLY_UP_B] = { 17,true };
	//Mount 29
	//호버 스탑 6
	//호버 제자리 24
	//플라이 업 17 다운 13 왼쪽15 오른쪽 16
}
#pragma endregion State


