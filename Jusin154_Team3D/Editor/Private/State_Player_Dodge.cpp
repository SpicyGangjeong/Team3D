#include "pch.h"
#include "State_Player_Dodge.h"
#include "Unit.h"


CState_Player_Dodge::CState_Player_Dodge()
	:CState_Player()
{
}

void CState_Player_Dodge::Enter()
{
	__super::Enter();
}

HRESULT CState_Player_Dodge::Update(_float fTimeDelta)
{
	if (E_FAIL == (__super::Update(fTimeDelta))) {
		return E_FAIL;
	}
	return S_OK;
}

void CState_Player_Dodge::Exit()
{
	__super::Exit();
}

HRESULT CState_Player_Dodge::Initialize(STATE_PLAYER_DODGE_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc))) {
		return E_FAIL;
	}
	m_pModel = m_pOwner->Get_Component<CModel>();
	m_pFSM = m_pOwner->Get_Component<CFSM>();

	return S_OK;
}

CState_Player_Dodge* CState_Player_Dodge::Create(STATE_PLAYER_DODGE_DESC* pDesc)
{
	CState_Player_Dodge* pInstance = new CState_Player_Dodge;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_Player_Dodge::Free()
{
	__super::Free();
}

void CState_Player_Dodge::Describe_Entity()
{
}
