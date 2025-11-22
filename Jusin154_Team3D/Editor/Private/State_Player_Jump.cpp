#include "pch.h"
#include "State_Player_Jump.h"
#include "Unit.h"


CState_Jump::CState_Jump()
    :CState_Root()
{
}

void CState_Jump::Enter()
{
    __super::Enter();
}

HRESULT CState_Jump::Update(_float fTimeDelta)
{
    if (E_FAIL == (__super::Update(fTimeDelta))) {
        return E_FAIL;
    }
    return S_OK;
}

void CState_Jump::Exit()
{
    __super::Exit();
}

HRESULT CState_Jump::Initialize(STATE_JUMP_DESC* pDesc)
{
    if (FAILED(__super::Initialize(pDesc))) {
        return E_FAIL;
    }
    m_pModel = m_pOwner->Get_Component<CModel>();
    m_pFSM = m_pOwner->Get_Component<CFSM>();

    return S_OK;
}

CState_Jump* CState_Jump::Create(STATE_JUMP_DESC* pDesc)
{
    CState_Jump* pInstance = new CState_Jump;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Jump::Free()
{
    __super::Free();
}

void CState_Jump::Describe_Entity()
{
}
