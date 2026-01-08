#include "pch.h"
#include "State_CutScene.h"
#include "Unit.h"

CState_CutScene::CState_CutScene()
    :CState_Root()
{
}

void CState_CutScene::Enter()
{
    __super::Enter();
}

HRESULT CState_CutScene::Update(_float fTimeDelta)
{
    if (E_FAIL == (__super::Update(fTimeDelta))) {
        return E_FAIL;
    }
    return S_OK;
}

void CState_CutScene::Exit()
{
    __super::Exit();
}

HRESULT CState_CutScene::Initialize(STATE_CUTSCENE_DESC* pDesc)
{
    if (FAILED(__super::Initialize(pDesc))) {
        return E_FAIL;
    }
    m_pModel = m_pOwner->Get_Component<CModel>();
    m_pFSM = m_pOwner->Get_Component<CFSM>();

    return S_OK;
}

CState_CutScene* CState_CutScene::Create(STATE_CUTSCENE_DESC* pDesc)
{
    CState_CutScene* pInstance = new CState_CutScene;
    if (FAILED(pInstance->Initialize(pDesc))) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CState_CutScene::Free()
{
    __super::Free();
}

void CState_CutScene::Describe_Entity()
{
}
