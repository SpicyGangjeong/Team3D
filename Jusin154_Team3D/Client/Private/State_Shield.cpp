#include "pch.h"
#include "State_Shield.h"
#include "Unit.h"


CState_Shield::CState_Shield()
    :CState_Root()
{
}

void CState_Shield::Enter()
{
    __super::Enter();
}

HRESULT CState_Shield::Update(_float fTimeDelta)
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

void CState_Shield::Exit()
{
    __super::Exit();
}

HRESULT CState_Shield::Initialize(STATE_SHIELD_DESC* pDesc)
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

CState_Shield* CState_Shield::Create(STATE_SHIELD_DESC* pDesc)
{
    CState_Shield* pInstance = new CState_Shield;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Shield::Free()
{
    __super::Free();
}

void CState_Shield::Describe_Entity()
{
}
