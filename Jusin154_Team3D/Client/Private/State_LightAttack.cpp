#include "pch.h"
#include "State_LightAttack.h"
#include "Unit.h"


CState_LightAttack::CState_LightAttack()
    :CState_Root()
{
}

void CState_LightAttack::Enter()
{
    __super::Enter();
}

HRESULT CState_LightAttack::Update(_float fTimeDelta)
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

void CState_LightAttack::Exit()
{
    __super::Exit();
}

HRESULT CState_LightAttack::Initialize(STATE_LIGHTATTACK_DESC* pDesc)
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

CState_LightAttack* CState_LightAttack::Create(STATE_LIGHTATTACK_DESC* pDesc)
{
    CState_LightAttack* pInstance = new CState_LightAttack;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_LightAttack::Free()
{
    __super::Free();
}

void CState_LightAttack::Describe_Entity()
{
}
