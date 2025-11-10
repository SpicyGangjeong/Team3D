#include "pch.h"
#include "Character_Controller.h"
#include "GameInstance.h"

CCharacter_Controller::CCharacter_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) :
	m_pDevice(pDevice),
	m_pContext(pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
	SAFE_ADDREF(m_pGameInstance);
}

CCharacter_Controller* CCharacter_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PSX::PxScene* pScene)
{
	CCharacter_Controller* pInstance = new CCharacter_Controller(pDevice, pContext);
	if (FAILED(pInstance->Initialize(pScene))) {
		SAFE_RELEASE(pInstance);
		assert(false);
	}
	return pInstance;
}

HRESULT CCharacter_Controller::Initialize(PSX::PxScene* pScene)
{
	m_pScene = pScene;
	return S_OK;
}

void CCharacter_Controller::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pGameInstance);
}