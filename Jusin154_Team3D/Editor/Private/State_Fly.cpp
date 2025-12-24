#include "pch.h"
#include "State_Fly.h"
#include "Unit.h"


CState_Fly::CState_Fly()
    :CState_Root()
{
}

void CState_Fly::Enter()
{
    __super::Enter();
}

HRESULT CState_Fly::Update(_float fTimeDelta)
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

void CState_Fly::Exit()
{
    __super::Exit();
}

HRESULT CState_Fly::Initialize(STATE_FLY_DESC* pDesc)
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

CState_Fly* CState_Fly::Create(STATE_FLY_DESC* pDesc)
{
    CState_Fly* pInstance = new CState_Fly;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Fly::Free()
{
    __super::Free();
}

void CState_Fly::Describe_Entity()
{
}
