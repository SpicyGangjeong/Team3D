#include "pch.h"
#include "Level_Manager.h"
#include "GameInstance.h"

#include "Level.h"

CLevel_Manager::CLevel_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pGameInstance);
}

HRESULT CLevel_Manager::Change_Level(CLevel* pNewLevel)
{
	if (nullptr != m_pCurrentLevel)
	{
		m_pGameInstance->Clear_Resources(m_pCurrentLevel->Get_LevelID());

		SAFE_RELEASE(m_pCurrentLevel);
	}
	m_bIsChangeLevel = false;
	m_pCurrentLevel = pNewLevel;

	return S_OK;
}

_bool CLevel_Manager::Check_LevelShouldChange() const
{
	return m_bIsChangeLevel;
}

void CLevel_Manager::Set_LevelToChange()
{
	m_bIsChangeLevel = true;;
}

_uint CLevel_Manager::Get_Current_LevelID()
{
	if (m_pCurrentLevel == nullptr) {
		return UINT_MAX;
	}
	return m_pCurrentLevel->Get_LevelID();
}

void CLevel_Manager::ResetLevel_Environment()
{
	m_pCurrentLevel->ResetLevel_Environment();
}

_uint CLevel_Manager::Get_NextLevelID()
{
	if (m_pCurrentLevel == nullptr) {
		return UINT_MAX;
	}
	return m_pCurrentLevel->Get_NextLevelID();
}

void CLevel_Manager::Update(_float fTimeDelta)
{
	m_pGameInstance->Compute_TimeDelta(TEXT("Timer_Level"));
	if (nullptr == m_pCurrentLevel){
		return;
	}

	m_pCurrentLevel->Update(fTimeDelta);

	m_pGameInstance->Compute_TimeDelta(TEXT("Timer_Level"));
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel){
		return E_FAIL;
	}

	return m_pCurrentLevel->Render();
}

CLevel_Manager* CLevel_Manager::Create()
{
	return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pCurrentLevel);
	SAFE_RELEASE(m_pGameInstance);
}
