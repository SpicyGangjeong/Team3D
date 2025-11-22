#include "pch.h"
#include "State_Player_Land.h"
#include "Unit.h"


CState_Player_Land::CState_Player_Land()
    :CState_Player()
{
}

void CState_Player_Land::Enter()
{
    __super::Enter();
}

HRESULT CState_Player_Land::Update(_float fTimeDelta)
{
    if (E_FAIL == (__super::Update(fTimeDelta))) {
        return E_FAIL;
    }
    return S_OK;
}

void CState_Player_Land::Exit()
{
    __super::Exit();
}

HRESULT CState_Player_Land::Initialize(STATE_PLAYER_LAND_DESC* pDesc)
{
    if (FAILED(__super::Initialize(pDesc))) {
        return E_FAIL;
    }
    m_pModel = m_pOwner->Get_Component<CModel>();
    m_pFSM = m_pOwner->Get_Component<CFSM>();

    return S_OK;
}

CState_Player_Land* CState_Player_Land::Create(STATE_PLAYER_LAND_DESC* pDesc)
{
    CState_Player_Land* pInstance = new CState_Player_Land;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_Player_Land::Free()
{
    __super::Free();
}

void CState_Player_Land::Describe_Entity()
{
}
