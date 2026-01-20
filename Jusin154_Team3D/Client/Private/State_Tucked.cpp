#include "pch.h"
#include "State_Tucked.h"
#include "GameInstance.h"
#include "Unit.h"

CState_Tucked::CState_Tucked()
	:CState_Root()
{
}

void CState_Tucked::Enter()
{
	__super::Enter();
	XMStoreFloat3(&m_vSoundPos, m_pOwner->Get_WorldPostion());
	m_vTuckedSoundTimer.y = 0.025f;
}

HRESULT CState_Tucked::Update(_float fTimeDelta)
{
	if (nullptr != m_funcPriorityUpdate) {
		m_funcPriorityUpdate(fTimeDelta);
	}
	if (E_FAIL == (__super::Update(fTimeDelta))) {
		return E_FAIL;
	}
	XMStoreFloat3(&m_vSoundPos, m_pOwner->Get_WorldPostion());
	m_vTuckedSoundTimer.x += fTimeDelta;
	if (m_vTuckedSoundTimer.x >= m_vTuckedSoundTimer.y) {
		m_vTuckedSoundTimer.x = 0.f;
		_uint iSwamSound = ENUM_CLASS(SOUND::SD_KIND::TUCKED_SWAM0) + m_pGameInstance->Random_Int(0, ENUM_CLASS(SOUND::SD_KIND::TUCKED_SWAM37) - ENUM_CLASS(SOUND::SD_KIND::TUCKED_SWAM0));
		m_pGameInstance->Sound_Play_3DPos((SOUND::SD_KIND)iSwamSound, SD_CHANNEL_GROUP::EFFECT, m_vSoundPos, 10.f, 200.f, false);
	}

	if (nullptr != m_funcLateUpdate) {
		m_funcLateUpdate(fTimeDelta);
	}
	return S_OK;
}

void CState_Tucked::Exit()
{
	__super::Exit();
}

HRESULT CState_Tucked::Initialize(STATE_TUCKED_DESC* pDesc)
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

CState_Tucked* CState_Tucked::Create(STATE_TUCKED_DESC* pDesc)
{
	CState_Tucked* pInstance = new CState_Tucked;
	if (FAILED(pInstance->Initialize(pDesc))) {
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CState_Tucked::Free()
{
	__super::Free();
}

void CState_Tucked::Describe_Entity()
{
}
