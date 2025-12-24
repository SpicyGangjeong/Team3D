#include "pch.h"
#include "State_FireBreath.h"
#include "Unit.h"


CState_FireBreath::CState_FireBreath()
    :CState_Root()
{
}

void CState_FireBreath::Enter()
{
    __super::Enter();
}

HRESULT CState_FireBreath::Update(_float fTimeDelta)
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

void CState_FireBreath::Exit()
{
    __super::Exit();
}

HRESULT CState_FireBreath::Initialize(STATE_FIREBREATH_DESC* pDesc)
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

CState_FireBreath* CState_FireBreath::Create(STATE_FIREBREATH_DESC* pDesc)
{
    CState_FireBreath* pInstance = new CState_FireBreath;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_FireBreath::Free()
{
    __super::Free();
}

void CState_FireBreath::Describe_Entity()
{
}
