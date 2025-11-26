#include "pch.h"
#include "State.h"
#include "GameInstance.h"
#include "Unit.h"

CState::CState()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    SAFE_ADDREF(m_pGameInstance);
}

HRESULT CState::Initialize(STATEDESC* pArg)
{
    m_pOwner = pArg->pOwner;
    return S_OK;
}

void CState::Free()
{
    __super::Free();

    SAFE_RELEASE(m_pGameInstance);
}

void CState::Describe_Entity()
{
}
