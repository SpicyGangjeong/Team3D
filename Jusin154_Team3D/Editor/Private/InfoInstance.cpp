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
	m_pPlayerInfo->Update(fTimeDelta);
	m_pMonsterInfo->Update(fTimeDelta);
	//m_pMapInfo->Update(fTimeDelta);
}

void CInfoInstance::Change_Level()
{
	m_pPlayerInfo->Change_Level();
	m_pMonsterInfo->Change_Level();
	//m_pMapInfo->Change_Level();
}

void CInfoInstance::Update_CameraCoordinateSystem(_float3& vLook, _float3& vRight)
{
	m_pPlayerInfo->Update_CameraCoordinateSystem(vLook, vRight);
}

pair<_float3, _float3> CInfoInstance::Get_CameraCoordinateSystem()
{
	return m_pPlayerInfo->Get_CameraCoordinateSystem();
}

#pragma region MONSTER_INFO
HRESULT CInfoInstance::Regist_PlayerAlly(CUnit* pUnit)
{
	return m_pMonsterInfo->Regist_PlayerAlly(pUnit);
}

HRESULT CInfoInstance::Deregist_PlayerAlly(CUnit* pUnit)
{
	if (nullptr == m_pMonsterInfo) {
		return S_OK; // 게임 종료 된 상태
	}
	return m_pMonsterInfo->Deregist_PlayerAlly(pUnit);
}

HRESULT CInfoInstance::Regist_ActiveMonster(CMonster* pUnit)
{
	return m_pMonsterInfo->Regist_ActiveMonster(pUnit);
}

HRESULT CInfoInstance::Deregist_ActiveMonster(CMonster* pUnit)
{
	if (nullptr == m_pMonsterInfo) {
		return S_OK; // 게임 종료 된 상태
	}
	return m_pMonsterInfo->Deregist_ActiveMonster(pUnit);
}

CUnit* CInfoInstance::Get_LockOnUnit()
{
	CUnit* pUnit = { nullptr };
	pUnit = m_pMonsterInfo->Get_LockOnUnit();
	// pUnit = m_pInfo->Get_LockOnInfo();
	return pUnit;
}

pair<CUnit*, CTransform*> CInfoInstance::Get_NearestPlayerAlly(_fvector vPos)
{
	return m_pMonsterInfo->Get_NearestPlayerAlly(vPos);
}
#pragma endregion
#pragma region MAP_INFO
//HRESULT CInfoInstance::Load_MapObjects(const _char* pFilePath)
//{
//	return m_pMapInfo->Load_MapObjects(pFilePath);
//}
//HRESULT CInfoInstance::Load_LightElements(const _char* pFilePath)
//{
//	return m_pMapInfo->Load_LightElements(pFilePath);
//}
#pragma endregion
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

	//m_pMapInfo = CMapInfo::Create(pDevice, pContext);
	//if (nullptr == m_pMapInfo) {
	//	return E_FAIL;
	//}

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
