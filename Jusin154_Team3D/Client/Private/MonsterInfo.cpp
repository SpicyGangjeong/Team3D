#include "pch.h"
#include "MonsterInfo.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "Monster.h"
#include "Layer.h"
#include "Player.h"

CMonsterInfo::CMonsterInfo()
{
}

void CMonsterInfo::Update(_float fTimeDelta)
{
	Refresh_PlayerAllies();
	Refresh_LockOnMonsters();
	Refresh_ActiveMonsters();
}
void CMonsterInfo::Change_Level()
{
	SAFE_RELEASE(m_pLockOnMonster);
	for (CMonster* pMonster : m_ActiveMonsters) {
		SAFE_RELEASE(pMonster);
	} m_ActiveMonsters.clear();
	for (CUnit* pPlayerAlly : m_PlayerAllies) {
		SAFE_RELEASE(pPlayerAlly);
	} m_PlayerAllies.clear();
}

HRESULT CMonsterInfo::Regist_PlayerAlly(CUnit* pUnit)
{
	for (CUnit* pAlly : m_PlayerAllies) {
		if (pAlly == pUnit) {
			return E_ACCESSDENIED;
		}
	} m_PlayerAllies.push_back(pUnit);
	SAFE_ADDREF(pUnit);
	return S_OK;
}

HRESULT CMonsterInfo::Deregist_PlayerAlly(CUnit* pUnit)
{
	for (list<CUnit*>::iterator iter = m_PlayerAllies.begin(); iter != m_PlayerAllies.end();) {
		if (*iter == pUnit) {
			SAFE_RELEASE(*iter);
			iter = m_PlayerAllies.erase(iter);
			break;
		}
		else {
			++iter;
		}
	}
	return E_FAIL;
}

HRESULT CMonsterInfo::Regist_ActiveMonster(CMonster* pUnit)
{
	for (CMonster* pMonster : m_ActiveMonsters) {
		if (pMonster == pUnit) {
			return E_ACCESSDENIED;
		}
	} m_ActiveMonsters.push_back(pUnit);
	SAFE_ADDREF(pUnit);
	return S_OK;
}

HRESULT CMonsterInfo::Deregist_ActiveMonster(CMonster* pUnit)
{
	for (list<CMonster*>::iterator iter = m_ActiveMonsters.begin(); iter != m_ActiveMonsters.end();) {
		if (*iter == pUnit) {
			SAFE_RELEASE(*iter);
			iter = m_ActiveMonsters.erase(iter);
			break;
		}
		else {
			++iter;
		}
	}
	return E_FAIL;
}

CUnit* CMonsterInfo::Get_LockOnUnit()
{
	return m_pLockOnMonster;
}

pair<CUnit*, CTransform*> CMonsterInfo::Get_NearestPlayerAlly(_fvector vPos)
{
	_float fLengthMAX = FLT_MAX;
	CTransform* pNearestPlayerAllyTransform = { nullptr };
	CUnit* pNearestPlayerAlly = { nullptr };

	for (list<CUnit*>::iterator iter = m_PlayerAllies.begin(); iter != m_PlayerAllies.end(); ++iter) {

		CTransform* pTransform = (*iter)->Get_Component<CTransform>();
		_float fNewLength = XMVectorGetX(XMVector3LengthSq(pTransform->Get_State(STATE::POSITION) - vPos));

		if (nullptr == pNearestPlayerAlly) {
			pNearestPlayerAlly = (*iter);
			fLengthMAX = fNewLength;
			pNearestPlayerAllyTransform = pTransform;
			continue;
		}

		if (fNewLength < fLengthMAX) {
			pNearestPlayerAlly = (*iter);
			pNearestPlayerAllyTransform = pTransform;
			fLengthMAX = fNewLength;
		}
	}
	return { pNearestPlayerAlly, pNearestPlayerAllyTransform };
}

HRESULT CMonsterInfo::Refresh_LockOnMonsters()
{
	SAFE_RELEASE(m_pLockOnMonster);
	_vector vCameraLook = m_pGameInstance->Get_CameraLook();
	_vector vCameraPos = m_pGameInstance->Get_CamXMPosition();
	_float fMaxDot = { 0.f };

	_vector vMonsterPos;
	_vector vToMonsterDir;
	_bool Aim = false;
	_float fFovy = 0.f;
	if (m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::GAMEPLAY), LAYER_PLAYER) != nullptr)
	{
		Aim = m_pGameInstance->Get_Layer(ENUM_CLASS(LEVEL::GAMEPLAY), LAYER_PLAYER)->Get_Object<CPlayer>()->Get_Aim();
	}
	if (Aim)
	{
		fFovy = max(m_pGameInstance->Get_CameraFov() - XMConvertToRadians(50.f), XMConvertToRadians(5.f));
	}
	else {
		fFovy = max(m_pGameInstance->Get_CameraFov() - XMConvertToRadians(30.f), XMConvertToRadians(20.f));
	}

	{ // 뷰프러스텀 순회해서 가장 중앙에 근접한 몬스터 찾기
		for (list<CMonster*>::iterator iter = m_ActiveMonsters.begin(); iter != m_ActiveMonsters.end(); ++iter) {

			// (카메라의 룩)과 (카메라 -> 몬스터 방향 벡터)를 내적해서 가장 큰 크기가 나온 몬스타가 락온 대상
			CMonster* pMonster = (*iter);
			if (pMonster->Get_Hp().x <= 0) {
				continue;
			}
			vMonsterPos = pMonster->Get_WorldPostion();
			vToMonsterDir = vMonsterPos - vCameraPos;
			_float fDotResult = CMyTools::DirectionCompare(vCameraLook, vToMonsterDir);
			if (fDotResult <= cosf(fFovy)) {
				continue;
			}

			if (nullptr == m_pLockOnMonster) {
				m_pLockOnMonster = pMonster;
				fMaxDot = fDotResult;
				continue;
			}

			if (fMaxDot < fDotResult) {
				m_pLockOnMonster = pMonster;
				fMaxDot = fDotResult;
			}
		}
	}
	if (nullptr != m_pLockOnMonster) {
		SAFE_ADDREF(m_pLockOnMonster);
	}
	return S_OK;
}

HRESULT CMonsterInfo::Refresh_ActiveMonsters()
{
	list<CMonster*>::iterator iter = m_ActiveMonsters.begin();
	for (; iter != m_ActiveMonsters.end();) {
		if (true == (*iter)->isDead()) {
			SAFE_RELEASE(*iter);
			iter = m_ActiveMonsters.erase(iter);
		}
		else {
			++iter;
		}
	}
	return S_OK;
}


HRESULT CMonsterInfo::Refresh_PlayerAllies()
{
	for (list<CUnit*>::iterator iter = m_PlayerAllies.begin(); iter != m_PlayerAllies.end();) {
		if (true == (*iter)->isDead()) {
			SAFE_RELEASE(*iter);
			iter = m_PlayerAllies.erase(iter);
		}
		else {
			iter++;
		}
	}
	return S_OK;
}

HRESULT CMonsterInfo::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_pInfoInstance = CInfoInstance::GetInstance();
	m_pDevice = pDevice;
	m_pContext = pContex;

	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pInfoInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);

	return S_OK;
}

CMonsterInfo* CMonsterInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	CMonsterInfo* pInstance = new CMonsterInfo();

	if (FAILED(pInstance->Initialize(pDevice, pContex))) {
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMonsterInfo::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLockOnMonster);
	for (CMonster* pMonster : m_ActiveMonsters) {
		SAFE_RELEASE(pMonster);
	} m_ActiveMonsters.clear();
	for (CUnit* pAlly : m_PlayerAllies) {
		SAFE_RELEASE(pAlly);
	} m_PlayerAllies.clear();

	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pInfoInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
