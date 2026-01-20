#include "pch.h"
#include "State_Slash.h"
#include "Unit.h"


CState_Slash::CState_Slash()
    :CState_Root()
{
}

void CState_Slash::Enter()
{
    __super::Enter();
}

HRESULT CState_Slash::Update(_float fTimeDelta)
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

void CState_Slash::Exit()
{
    __super::Exit();
}

HRESULT CState_Slash::Initialize(STATE_SLASH_DESC* pDesc)
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

CState_Slash* CState_Slash::Create(STATE_SLASH_DESC* pDesc)
{
    CState_Slash* pInstance = new CState_Slash;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Slash::Free()
{
    __super::Free();
}

void CState_Slash::Describe_Entity()
{
}
