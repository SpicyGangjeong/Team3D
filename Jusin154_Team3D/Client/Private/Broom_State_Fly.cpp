#include "pch.h"
#include "Broom_State_Fly.h"
#include "Unit.h"


CBroom_State_Fly::CBroom_State_Fly()
    :CState_Root()
{
}

void CBroom_State_Fly::Enter()
{
    __super::Enter();
}

HRESULT CBroom_State_Fly::Update(_float fTimeDelta)
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

void CBroom_State_Fly::Exit()
{
    __super::Exit();
}

HRESULT CBroom_State_Fly::Initialize(STATE_FLY_DESC* pDesc)
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

CBroom_State_Fly* CBroom_State_Fly::Create(STATE_FLY_DESC* pDesc)
{
    CBroom_State_Fly* pInstance = new CBroom_State_Fly;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CBroom_State_Fly::Free()
{
    __super::Free();
}

void CBroom_State_Fly::Describe_Entity()
{
}
