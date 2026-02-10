#include "pch.h"
#include "FSM.h"
#include "State.h"

static unordered_map<size_t, const char*> g_FSMStateName =
{
	{ FSMSTATE::IDLE, "IDLE" },
	{ FSMSTATE::MOVE, "MOVE" },
	{ FSMSTATE::WALK, "WALK" },
	{ FSMSTATE::JOG, "JOG" },
	{ FSMSTATE::SPRINT, "SPRINT" },
	{ FSMSTATE::JUMP, "JUMP" },
	{ FSMSTATE::LAND, "LAND" },
	{ FSMSTATE::DODGE, "DODGE" },
	{ FSMSTATE::LIGHT_ATTACK, "LIGHT_ATTACK" },
	{ FSMSTATE::SPELL, "SPELL" },
	{ FSMSTATE::HIT, "HIT" },
	{ FSMSTATE::DEAD, "DEAD" },
	{ FSMSTATE::RUSH, "RUSH" },
	{ FSMSTATE::TUCKED, "TUCKED" },
	{ FSMSTATE::SLASH, "SLASH" },
	{ FSMSTATE::DASH, "DASH" },
	{ FSMSTATE::CUTSCENE, "CUTSCENE" }
};


CFSM::CFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CFSM::CFSM(const CFSM& rhs)
	: CComponent(rhs)
{
}

HRESULT CFSM::Bind_States(FSM_DESC& Desc)
{
	if (nullptr != m_pStateMask) {
		return E_FAIL;
	}

	m_pStateMask = Desc.pStateMask;
	m_pStates = Desc.pStates;
	return S_OK;
}

HRESULT CFSM::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFSM::Initialize(void* pArg)
{
    return S_OK;
}

void CFSM::Change_State(size_t iStateMask)
{
	if (nullptr != m_pCurrentState) {	// 현재 스테이트가 비워져 있으면 우회
		if (IsEnable(iStateMask)) {		// 이미 활성화 된 스테이트면 리턴
			return;
		}
		m_iPreviousStateMask = *m_pStateMask;

		m_pCurrentState->Exit();
		SAFE_RELEASE(m_pPreviousState); // Prev State 를 지움
		m_pPreviousState = m_pCurrentState;
	}

	m_pCurrentState = m_pStates->at(iStateMask);
	SAFE_ADDREF(m_pCurrentState);
	m_pCurrentState->Enter();
}

HRESULT CFSM::Update_State(_float fTimeDelta)
{
	if (nullptr != m_pCurrentState)
	{
		return m_pCurrentState->Update(fTimeDelta);
	}
	return S_OK;
}

const char* CFSM::Get_CurrentStateName()
{
	if (nullptr == m_pStateMask)
		return "NONE";

	size_t mask = *m_pStateMask;

	for (auto& pair : g_FSMStateName)
	{
		if (mask & pair.first)
			return pair.second;
	}

	return "UNKNOWN";
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

CFSM* CFSM::Clone(void* pArg, class CGameObject* pOwner)
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

	SAFE_RELEASE(m_pCurrentState);
	SAFE_RELEASE(m_pPreviousState);

	if (nullptr != m_pStates) {
		unordered_map<size_t, class CState*>::iterator iter = m_pStates->begin();
		for (; iter != m_pStates->end(); ++iter) {
			SAFE_RELEASE(iter->second);
		}
		m_pStates = nullptr;
	}
}
#ifdef _DEBUG

void CFSM::Describe_Entity()
{
}



#endif // _DEBUG
