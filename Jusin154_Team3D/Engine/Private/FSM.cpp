#include "pch.h"
#include "FSM.h"
#include "State.h"

CFSM::CFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
	m_States.resize(FSMSTATE::END);
}

CFSM::CFSM(const CFSM& rhs)
	: CComponent(rhs),
	m_States(rhs.m_States)
{
}

HRESULT CFSM::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFSM::Initialize(void* pArg)
{
    return S_OK;
}

void CFSM::Add_State(_uint iIndex, CState* state)
{
	m_States[iIndex] = state;
	m_States[iIndex]->Set_State(iIndex);
	m_States[iIndex]->Set_Owner(m_pOwner);
	m_States[iIndex]->Set_Component();
}

void CFSM::Change_State(_uint iIndex)
{
	m_pPrevious = m_pCurrent;

	if (m_pCurrent)
		m_pCurrent->Exit();

	m_pCurrent = m_States[iIndex];

	if (m_pCurrent)
		m_pCurrent->Enter();
}

void CFSM::Update(_float fTimeDelta)
{
	vector<CState*> States;

	CState* pCurrent = m_pCurrent;

	while (pCurrent) 
	{
		States.push_back(pCurrent);
		pCurrent = pCurrent->Get_Parent();
	}

	for (_int i = (_int)States.size() - 1; i >= 0; --i) {
		States[i]->Update(fTimeDelta);
	}
}

_uint CFSM::Get_CurrState()
{
	return m_pCurrent->Get_State();
}

_uint CFSM::Get_PrevState()
{
	return m_pPrevious->Get_State();
}

void CFSM::Set_Parent(FSMSTATE::ESTATE Child, FSMSTATE::ESTATE Parent)
{
	CState* pChild = m_States[Child];
	CState* pParent = m_States[Parent];
	if (pChild && pParent)
		pChild->Set_Parent(pParent);
}


CFSM* CFSM::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFSM* pInstance = new CFSM(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFSM");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CFSM::Clone(void* pArg, class CGameObject* pOwner)
{
	CFSM* pInstance = new CFSM(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFSM");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CFSM::Free()
{
    __super::Free();

	SAFE_RELEASE(m_pCurrent);
	SAFE_RELEASE(m_pPrevious);

	for (auto& states : m_States)
	{
		SAFE_RELEASE(states);
	}
	m_States.clear();
}

void CFSM::Describe_Entity()
{
}



