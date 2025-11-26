#include "pch.h"
#include "State_Root.h"
#include "Unit.h"

CState_Root::CState_Root()
    :CState()
{
}

void CState_Root::Enter()
{
	if (nullptr != m_funcEnterEvent) {
		m_funcEnterEvent();
	}
}

HRESULT CState_Root::Update(_float fTimeDelta)
{
	HRESULT hr = { S_OK };
	if (nullptr != m_funcExitCheck) {
		hr = m_funcExitCheck(fTimeDelta);
	}
	return hr;
}

void CState_Root::Exit()
{
	if (nullptr != m_funcExitEvent) {
		m_funcExitEvent();
	}
}

HRESULT CState_Root::Initialize(STATE_ROOT_DESC* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}
    m_funcExitCheck = pArg->funcExitCheck;
    m_funcEnterEvent = pArg->funcEnterEvent;
    m_funcExitEvent = pArg->funcExitEvent;
    return S_OK;
}

void CState_Root::Free()
{
    __super::Free();
}

void CState_Root::Describe_Entity()
{
}
