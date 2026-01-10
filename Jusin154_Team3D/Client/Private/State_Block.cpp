#include "pch.h"
#include "State_Block.h"
#include "Unit.h"


CState_Block::CState_Block()
    :CState_Root()
{
}

void CState_Block::Enter()
{
    __super::Enter();
}

HRESULT CState_Block::Update(_float fTimeDelta)
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

void CState_Block::Exit()
{
    __super::Exit();
}

HRESULT CState_Block::Initialize(STATE_BLOCK_DESC* pDesc)
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

CState_Block* CState_Block::Create(STATE_BLOCK_DESC* pDesc)
{
    CState_Block* pInstance = new CState_Block;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Block::Free()
{
    __super::Free();
}

void CState_Block::Describe_Entity()
{
}
