#include "pch.h"
#include "State_Throw.h"
#include "Unit.h"


CState_Throw::CState_Throw()
    :CState_Root()
{
}

void CState_Throw::Enter()
{
    __super::Enter();
}

HRESULT CState_Throw::Update(_float fTimeDelta)
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

void CState_Throw::Exit()
{
    __super::Exit();
}

HRESULT CState_Throw::Initialize(STATE_THROW_DESC* pDesc)
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

CState_Throw* CState_Throw::Create(STATE_THROW_DESC* pDesc)
{
    CState_Throw* pInstance = new CState_Throw;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Throw::Free()
{
    __super::Free();
}

void CState_Throw::Describe_Entity()
{
}
