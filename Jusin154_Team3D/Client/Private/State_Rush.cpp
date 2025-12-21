#include "pch.h"
#include "State_Rush.h"
#include "GameInstance.h"
#include "Unit.h"

CState_Rush::CState_Rush()
	:CState_Root()
{
}

void CState_Rush::Enter()
{
	*m_pRushPlayerHit = false;
	__super::Enter();
}

HRESULT CState_Rush::Update(_float fTimeDelta)
{
	if (E_FAIL == (__super::Update(fTimeDelta))) {
		return E_FAIL;
	}
	return S_OK;
}

void CState_Rush::Exit()
{
	*m_pRushPlayerHit = false;
	__super::Exit();
}

HRESULT CState_Rush::Initialize(STATE_RUSH_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc))) {
		return E_FAIL;
	}
	m_pRushPlayerHit = pDesc->pCollisionPlayer;
	m_pModel = m_pOwner->Get_Component<CModel>();
	m_pFSM = m_pOwner->Get_Component<CFSM>();

	return S_OK;
}

CState_Rush* CState_Rush::Create(STATE_RUSH_DESC* pDesc)
{
	CState_Rush* pInstance = new CState_Rush;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_Rush::Free()
{
	__super::Free();
}

void CState_Rush::Describe_Entity()
{
}
