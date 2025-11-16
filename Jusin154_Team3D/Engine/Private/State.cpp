#include "pch.h"
#include "State.h"
#include "GameInstance.h"
#include "Unit.h"

CState::CState()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    SAFE_ADDREF(m_pGameInstance);
}

void CState::Enter()
{
}

void CState::Update(_float fTimeDelta)
{
}

void CState::Exit()
{
}

_bool CState::CheckExitState()
{
    return true;
}

void CState::Set_Owner(CGameObject* pOwner)
{
    m_pOwner = dynamic_cast<CUnit*>(pOwner);
};

void CState::Set_Component()
{
    m_pModel = m_pOwner->Get_Component<CModel>();
    m_pFSM = m_pOwner->Get_Component<CFSM>();
}

void CState::Free()
{
    __super::Free();

    SAFE_RELEASE(m_pGameInstance);
}

void CState::Describe_Entity()
{
}
