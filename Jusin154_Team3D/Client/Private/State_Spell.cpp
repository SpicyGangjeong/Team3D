#include "pch.h"
#include "State_Spell.h"
#include "Unit.h"


CState_Spell::CState_Spell()
    :CState_Root()
{
}

void CState_Spell::Enter()
{
    __super::Enter();
}

HRESULT CState_Spell::Update(_float fTimeDelta)
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

void CState_Spell::Exit()
{
    __super::Exit();
}

HRESULT CState_Spell::Initialize(STATE_SPELL_DESC* pDesc)
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

CState_Spell* CState_Spell::Create(STATE_SPELL_DESC* pDesc)
{
    CState_Spell* pInstance = new CState_Spell;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Spell::Free()
{
    __super::Free();
}

void CState_Spell::Describe_Entity()
{
}
