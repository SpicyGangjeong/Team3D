#include "pch.h"
#include "State_Skill.h"
#include "Unit.h"


CState_Skill::CState_Skill()
    :CState_Root()
{
}

void CState_Skill::Enter()
{
    __super::Enter();
}

HRESULT CState_Skill::Update(_float fTimeDelta)
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

void CState_Skill::Exit()
{
    __super::Exit();
}

HRESULT CState_Skill::Initialize(STATE_SKILL_DESC* pDesc)
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

CState_Skill* CState_Skill::Create(STATE_SKILL_DESC* pDesc)
{
    CState_Skill* pInstance = new CState_Skill;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Skill::Free()
{
    __super::Free();
}

void CState_Skill::Describe_Entity()
{
}
