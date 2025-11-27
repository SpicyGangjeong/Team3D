#include "pch.h"
#include "InfoInstance.h"
#include "GameInstance.h"
#include "MapInfo.h"
#include "MonsterInfo.h"
#include "PlayerInfo.h"

IMPLEMENT_SINGLETON(CInfoInstance)

CInfoInstance::CInfoInstance()
{
}


void CInfoInstance::Update(_float fTimeDelta)
{

}

void CInfoInstance::Change_Level()
{
	m_pPlayerInfo->Change_Level();
	m_pMonsterInfo->Change_Level();
	m_pMapInfo->Change_Level();
}

HRESULT CInfoInstance::Load_MapObjects(const _char* pFilePath)
{
	return m_pMapInfo->Load_MapObjects(pFilePath);
}

LEVEL CInfoInstance::Get_RestartLevel()
{
	return LEVEL::GAMEPLAY;
}

HRESULT CInfoInstance::Initialize_Information(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_pDevice = pDevice;
	m_pContext = pContext;
	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);

	m_pMapInfo = CMapInfo::Create(pDevice, pContext);
	if (nullptr == m_pMapInfo) {
		return E_FAIL;
	}
	m_pPlayerInfo = CPlayerInfo::Create(pDevice, pContext);
	if (nullptr == m_pPlayerInfo) {
		return E_FAIL;
	}
	m_pMonsterInfo = CMonsterInfo::Create(pDevice, pContext);
	if (nullptr == m_pMonsterInfo) {
		return E_FAIL;
	}


	return S_OK;
}

void CInfoInstance::Release_Information()
{
	DestroyInstance();


	SAFE_RELEASE(m_pMapInfo);
	SAFE_RELEASE(m_pPlayerInfo);
	SAFE_RELEASE(m_pMonsterInfo);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pGameInstance);
}

void CInfoInstance::Free()
{
	__super::Free();

}
