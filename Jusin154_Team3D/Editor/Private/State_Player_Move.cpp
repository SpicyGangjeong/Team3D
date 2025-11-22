#include "pch.h"
#include "State_Player_Move.h"
#include "Unit.h"


CState_Player_Move::CState_Player_Move()
    :CState_Player()
{
}

void CState_Player_Move::Enter()
{
    __super::Enter();
}

HRESULT CState_Player_Move::Update(_float fTimeDelta)
{
    if (E_FAIL == (__super::Update(fTimeDelta))) {
        return E_FAIL;
    }
    return S_OK;
}

void CState_Player_Move::Exit()
{
    __super::Exit();
}

HRESULT CState_Player_Move::Initialize(STATE_PLAYER_MOVE_DESC* pDesc)
{
    if (FAILED(__super::Initialize(pDesc))) {
        return E_FAIL;
    }
    m_pModel = m_pOwner->Get_Component<CModel>();
    m_pFSM = m_pOwner->Get_Component<CFSM>();

    return S_OK;
}

CState_Player_Move* CState_Player_Move::Create(STATE_PLAYER_MOVE_DESC* pDesc)
{
    CState_Player_Move* pInstance = new CState_Player_Move;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Player_Move::Free()
{
    __super::Free();
}

void CState_Player_Move::Describe_Entity()
{
}
