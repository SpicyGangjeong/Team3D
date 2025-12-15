#include "pch.h"
#include "State_Blink.h"
#include "Unit.h"


CState_Blink::CState_Blink()
    :CState_Root()
{
}

void CState_Blink::Enter()
{
    __super::Enter();
}

HRESULT CState_Blink::Update(_float fTimeDelta)
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

void CState_Blink::Exit()
{
    __super::Exit();
}

HRESULT CState_Blink::Initialize(STATE_BLINK_DESC* pDesc)
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

CState_Blink* CState_Blink::Create(STATE_BLINK_DESC* pDesc)
{
    CState_Blink* pInstance = new CState_Blink;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Blink::Free()
{
    __super::Free();
}

void CState_Blink::Describe_Entity()
{
}
