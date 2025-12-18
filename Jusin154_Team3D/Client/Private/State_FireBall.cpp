#include "pch.h"
#include "State_FireBall.h"
#include "Unit.h"


CState_FireBall::CState_FireBall()
    :CState_Root()
{
}

void CState_FireBall::Enter()
{
    __super::Enter();
}

HRESULT CState_FireBall::Update(_float fTimeDelta)
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

void CState_FireBall::Exit()
{
    __super::Exit();
}

HRESULT CState_FireBall::Initialize(STATE_FIREBALL_DESC* pDesc)
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

CState_FireBall* CState_FireBall::Create(STATE_FIREBALL_DESC* pDesc)
{
    CState_FireBall* pInstance = new CState_FireBall;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_FireBall::Free()
{
    __super::Free();
}

void CState_FireBall::Describe_Entity()
{
}
