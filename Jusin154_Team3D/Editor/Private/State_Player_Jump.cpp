#include "pch.h"
#include "State_Player_Jump.h"
#include "Unit.h"


CState_Player_Jump::CState_Player_Jump()
    :CState_Player()
{
}

void CState_Player_Jump::Enter()
{
    __super::Enter();
}

HRESULT CState_Player_Jump::Update(_float fTimeDelta)
{
    if (E_FAIL == (__super::Update(fTimeDelta))) {
        return E_FAIL;
    }
    return S_OK;
}

void CState_Player_Jump::Exit()
{
    __super::Exit();
}

HRESULT CState_Player_Jump::Initialize(STATE_PLAYER_JUMP_DESC* pDesc)
{
    if (FAILED(__super::Initialize(pDesc))) {
        return E_FAIL;
    }
    m_pModel = m_pOwner->Get_Component<CModel>();
    m_pFSM = m_pOwner->Get_Component<CFSM>();

    return S_OK;
}

CState_Player_Jump* CState_Player_Jump::Create(STATE_PLAYER_JUMP_DESC* pDesc)
{
    CState_Player_Jump* pInstance = new CState_Player_Jump;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Player_Jump::Free()
{
    __super::Free();
}

void CState_Player_Jump::Describe_Entity()
{
}
