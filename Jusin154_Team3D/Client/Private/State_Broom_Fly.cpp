#include "pch.h"
#include "State_Broom_Fly.h"
#include "Unit.h"


CState_Broom_Fly::CState_Broom_Fly()
    :CState_Root()
{
}

void CState_Broom_Fly::Enter()
{
    __super::Enter();
}

HRESULT CState_Broom_Fly::Update(_float fTimeDelta)
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

void CState_Broom_Fly::Exit()
{
    __super::Exit();
}

HRESULT CState_Broom_Fly::Initialize(STATE_BROOM_FLY_DESC* pDesc)
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

CState_Broom_Fly* CState_Broom_Fly::Create(STATE_BROOM_FLY_DESC* pDesc)
{
    CState_Broom_Fly* pInstance = new CState_Broom_Fly;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Broom_Fly::Free()
{
    __super::Free();
}

void CState_Broom_Fly::Describe_Entity()
{
}
