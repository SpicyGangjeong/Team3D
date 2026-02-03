#include "pch.h"
#include "State_Swipe.h"
#include "Unit.h"


CState_Swipe::CState_Swipe()
    :CState_Root()
{
}

void CState_Swipe::Enter()
{
    m_bPlayerHit = false;
    __super::Enter();
}

HRESULT CState_Swipe::Update(_float fTimeDelta)
{
    if (nullptr != m_funcPriorityUpdate) {
        m_funcPriorityUpdate(fTimeDelta);
    }
    if (E_FAIL == (__super::Update(fTimeDelta))) {
        return E_FAIL;
    }
    if (nullptr != m_funcLateUpdate) {
        m_funcLateUpdate(fTimeDelta, m_bPlayerHit);
    }
    return S_OK;
}

void CState_Swipe::Exit()
{
    m_bPlayerHit = false;
    __super::Exit();
}

HRESULT CState_Swipe::Initialize(STATE_SWIPE_DESC* pDesc)
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

CState_Swipe* CState_Swipe::Create(STATE_SWIPE_DESC* pDesc)
{
    CState_Swipe* pInstance = new CState_Swipe;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Swipe::Free()
{
    __super::Free();
}

void CState_Swipe::Describe_Entity()
{
}
