#include "pch.h"
#include "State_Parry.h"
#include "Unit.h"


CState_Parry::CState_Parry()
    :CState_Root()
{
}

void CState_Parry::Enter()
{
    __super::Enter();
}

HRESULT CState_Parry::Update(_float fTimeDelta)
{
    if (nullptr != m_funcPriorityUpdate) {
        m_funcPriorityUpdate(fTimeDelta);
    }
    if (E_FAIL == (__super::Update(fTimeDelta))) {
        return E_FAIL;
    }
    if (nullptr != m_funcLateUpdate) {
        m_funcLateUpdate(fTimeDelta);
    }
    return S_OK;
}

void CState_Parry::Exit()
{
    __super::Exit();
}

HRESULT CState_Parry::Initialize(STATE_PARRY_DESC* pDesc)
{
    if (FAILED(__super::Initialize(pDesc))) {
        return E_FAIL;
    }
    m_funcPriorityUpdate = pDesc->funcPriorityUpdate;
    m_funcLateUpdate = pDesc->funcLateUpdate;
    m_pModel = m_pOwner->Get_Component<CModel>();
    m_pFSM = m_pOwner->Get_Component<CFSM>();

    return S_OK;
}

CState_Parry* CState_Parry::Create(STATE_PARRY_DESC* pDesc)
{
    CState_Parry* pInstance = new CState_Parry;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Parry::Free()
{
    __super::Free();
}

void CState_Parry::Describe_Entity()
{
}
