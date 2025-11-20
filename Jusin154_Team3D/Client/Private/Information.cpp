#include "pch.h"
#include "Information.h"
#include "GameInstance.h"


CInformation::CInformation()
{
}

HRESULT CInformation::Initialize_Information(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_pDevice = pDevice;
	m_pContext = pContext;
	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
	return S_OK;
}

void CInformation::Release_Information()
{

}

void CInformation::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pGameInstance);
}
