#include "pch.h"
#include "Level_Restart.h"
#include "Information.h"
#include "GameInstance.h"
#include "Level_Loading.h"



CLevel_Restart::CLevel_Restart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{

}

HRESULT CLevel_Restart::Initialize()
{
	//m_eNextLevel = CInformation::GetInstance()->Get_RestartLevel();

	return S_OK;
}

void CLevel_Restart::Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, m_eNextLevel)))) {
		return;
	}
}

HRESULT CLevel_Restart::Render()
{
	return S_OK;
}

pair<CLevel*, function<void()>> CLevel_Restart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_Restart* pInstance = new CLevel_Restart(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Restart");
		SAFE_RELEASE(pInstance);
	}

	return { pInstance, nullptr };
}

void CLevel_Restart::Free()
{
	__super::Free();

}
