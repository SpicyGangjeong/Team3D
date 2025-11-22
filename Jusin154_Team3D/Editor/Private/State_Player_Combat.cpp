#include "pch.h"
#include "State_Player_Combat.h"
#include "Unit.h"


CState_Combat::CState_Combat()
    :CState_Root()
{
}

void CState_Combat::Enter()
{
    __super::Enter();
}

HRESULT CState_Combat::Update(_float fTimeDelta)
{
    if (E_FAIL == (__super::Update(fTimeDelta))) {
        return E_FAIL;
    }
    return S_OK;
}

void CState_Combat::Exit()
{
    __super::Exit();
}

HRESULT CState_Combat::Initialize(STATE_COMBAT_DESC* pDesc)
{
    if (FAILED(__super::Initialize(pDesc))) {
        return E_FAIL;
    }
    m_pOwner = pDesc->pOwner;
    m_pModel = m_pOwner->Get_Component<CModel>();
    m_pFSM = m_pOwner->Get_Component<CFSM>();

    return S_OK;
}

CState_Combat* CState_Combat::Create(STATE_COMBAT_DESC* pDesc)
{
    CState_Combat* pInstance = new CState_Combat;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Combat::Free()
{
    __super::Free();
}

void CState_Combat::Describe_Entity()
{
}
