#include "pch.h"
#include "State_Slam.h"
#include "Unit.h"


CState_Slam::CState_Slam()
    :CState_Root()
{
}

void CState_Slam::Enter()
{
    __super::Enter();
}

HRESULT CState_Slam::Update(_float fTimeDelta)
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

void CState_Slam::Exit()
{
    __super::Exit();
}

HRESULT CState_Slam::Initialize(STATE_SLAM_DESC* pDesc)
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

CState_Slam* CState_Slam::Create(STATE_SLAM_DESC* pDesc)
{
    CState_Slam* pInstance = new CState_Slam;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Slam::Free()
{
    __super::Free();
}

void CState_Slam::Describe_Entity()
{
}
