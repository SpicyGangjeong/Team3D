#include "pch.h"
#include "State_AncientSpell.h"
#include "Unit.h"


CState_AncientSpell::CState_AncientSpell()
    :CState_Root()
{
}

void CState_AncientSpell::Enter()
{
    __super::Enter();
}

HRESULT CState_AncientSpell::Update(_float fTimeDelta)
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

void CState_AncientSpell::Exit()
{
    __super::Exit();
}

HRESULT CState_AncientSpell::Initialize(STATE_ANCIENTSPELL_DESC* pDesc)
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

CState_AncientSpell* CState_AncientSpell::Create(STATE_ANCIENTSPELL_DESC* pDesc)
{
    CState_AncientSpell* pInstance = new CState_AncientSpell;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_AncientSpell::Free()
{
    __super::Free();
}

void CState_AncientSpell::Describe_Entity()
{
}
