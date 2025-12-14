#include "pch.h"
#include "State_Broom_TurboFly.h"
#include "Unit.h"


CState_Broom_TurboFly::CState_Broom_TurboFly()
    :CState_Root()
{
}

void CState_Broom_TurboFly::Enter()
{
    __super::Enter();
}

HRESULT CState_Broom_TurboFly::Update(_float fTimeDelta)
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

void CState_Broom_TurboFly::Exit()
{
    __super::Exit();
}

HRESULT CState_Broom_TurboFly::Initialize(STATE_BROOM_TURBOFLY_DESC* pDesc)
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

CState_Broom_TurboFly* CState_Broom_TurboFly::Create(STATE_BROOM_TURBOFLY_DESC* pDesc)
{
    CState_Broom_TurboFly* pInstance = new CState_Broom_TurboFly;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Broom_TurboFly::Free()
{
    __super::Free();
}

void CState_Broom_TurboFly::Describe_Entity()
{
}
