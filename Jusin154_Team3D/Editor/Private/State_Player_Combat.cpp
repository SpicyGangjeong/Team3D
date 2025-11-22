#include "pch.h"
#include "State_Player_Combat.h"
#include "Unit.h"


CState_Player_Combat::CState_Player_Combat()
    :CState_Player()
{
}

void CState_Player_Combat::Enter()
{
    __super::Enter();
}

HRESULT CState_Player_Combat::Update(_float fTimeDelta)
{
    if (E_FAIL == (__super::Update(fTimeDelta))) {
        return E_FAIL;
    }
    return S_OK;
}

void CState_Player_Combat::Exit()
{
    __super::Exit();
}

HRESULT CState_Player_Combat::Initialize(STATE_PLAYER_COMBAT_DESC* pDesc)
{
    if (FAILED(__super::Initialize(pDesc))) {
        return E_FAIL;
    }
    m_pOwner = pDesc->pOwner;
    m_pModel = m_pOwner->Get_Component<CModel>();
    m_pFSM = m_pOwner->Get_Component<CFSM>();

    return S_OK;
}

CState_Player_Combat* CState_Player_Combat::Create(STATE_PLAYER_COMBAT_DESC* pDesc)
{
    CState_Player_Combat* pInstance = new CState_Player_Combat;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Player_Combat::Free()
{
    __super::Free();
}

void CState_Player_Combat::Describe_Entity()
{
}
