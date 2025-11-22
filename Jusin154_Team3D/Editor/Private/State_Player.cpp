#include "pch.h"
#include "State_Player.h"
#include "Unit.h"

CState_Player::CState_Player()
    :CState()
{
}

void CState_Player::Enter()
{
	if (nullptr != m_funcEnterEvent) {
		m_funcEnterEvent();
	}
}

HRESULT CState_Player::Update(_float fTimeDelta)
{
	HRESULT hr = { S_OK };
	if (nullptr != m_funcExitCheck) {
		hr = m_funcExitCheck(fTimeDelta);
	}
	return hr;
}

void CState_Player::Exit()
{
	if (nullptr != m_funcExitEvent) {
		m_funcExitEvent();
	}
}

HRESULT CState_Player::Initialize(STATE_PLAYER_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}
    m_funcExitCheck = pArg->funcExitCheck;
    m_funcEnterEvent = pArg->funcEnterEvent;
    m_funcExitEvent = pArg->funcExitEvent;
    return S_OK;
}

void CState_Player::Free()
{
    __super::Free();
}

void CState_Player::Describe_Entity()
{
}
