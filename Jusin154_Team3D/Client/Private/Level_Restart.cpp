#include "pch.h"
#include "Level_Restart.h"
#include "InfoInstance.h"
#include "GameInstance.h"
#include "Level_Loading.h"



CLevel_Restart::CLevel_Restart(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{
	m_pInfoInstance = CInfoInstance::GetInstance();
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CLevel_Restart::Initialize()
{
	m_eNextLevel = m_pInfoInstance->Get_RestartLevel();

	return S_OK;
}

void CLevel_Restart::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Up(DIK_F1))
	{
		m_pGameInstance->Set_LevelToChange();
	}

	//m_pInfoInstance->Update(fTimeDelta);

	if (true == m_pGameInstance->Check_LevelShouldChange()) {
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, m_eNextLevel)))) {
			return;
		}
	}
}

HRESULT CLevel_Restart::Render()
{
	SetWindowText(g_hWnd, TEXT("리스타트레벨"));
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

	SAFE_RELEASE(m_pInfoInstance);
}
