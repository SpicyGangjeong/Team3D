#include "pch.h"
#include "State_Broom_Ride_Move.h"
#include "Unit.h"


CState_Broom_Ride_Move::CState_Broom_Ride_Move()
    :CState_Root()
{
}

void CState_Broom_Ride_Move::Enter()
{
    __super::Enter();
}

HRESULT CState_Broom_Ride_Move::Update(_float fTimeDelta)
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

void CState_Broom_Ride_Move::Exit()
{
    __super::Exit();
}

HRESULT CState_Broom_Ride_Move::Initialize(STATE_BROOM_RIDE_MOVE_DESC* pDesc)
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

CState_Broom_Ride_Move* CState_Broom_Ride_Move::Create(STATE_BROOM_RIDE_MOVE_DESC* pDesc)
{
    CState_Broom_Ride_Move* pInstance = new CState_Broom_Ride_Move;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Broom_Ride_Move::Free()
{
    __super::Free();
}

void CState_Broom_Ride_Move::Describe_Entity()
{
}
