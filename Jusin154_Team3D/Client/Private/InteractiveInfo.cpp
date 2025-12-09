#include "pch.h"
#include "InteractiveInfo.h"
#include "GameInstance.h"
#include "InfoInstance.h"
#include "MapElement_Interactable.h"
#include "Monster.h"
#include "Layer.h"
#include "Player.h"

CInteractiveInfo::CInteractiveInfo()
{
}

void CInteractiveInfo::Update(_float fTimeDelta)
{
	Refresh_LockOnTarget();
}
void CInteractiveInfo::Change_Level()
{
	SAFE_RELEASE(m_pLockOnInteractive);
	for (CMapElement_Interactable* pInteractive : m_ActiveInteractive) {
		SAFE_RELEASE(pInteractive);
	} m_ActiveInteractive.clear();
}

HRESULT CInteractiveInfo::Regist_ActiveInteractive(CMapElement_Interactable* pUnit)
{
	for (CMapElement_Interactable* pInteractive : m_ActiveInteractive) {
		if (pInteractive == pUnit) {
			return E_ACCESSDENIED;
		}
	} m_ActiveInteractive.push_back(pUnit);
	SAFE_ADDREF(pUnit);
	return S_OK;
}

HRESULT CInteractiveInfo::Deregist_ActiveInteractive(CMapElement_Interactable* pUnit)
{
	for (list<CMapElement_Interactable*>::iterator iter = m_ActiveInteractive.begin(); iter != m_ActiveInteractive.end();) {
		if (*iter == pUnit) {
			SAFE_RELEASE(*iter);
			iter = m_ActiveInteractive.erase(iter);
			break;
		}
		else {
			++iter;
		}
	}
	return E_FAIL;
}

CMapElement_Interactable* CInteractiveInfo::Get_LockOnUnit()
{
	return m_pLockOnInteractive;
}

HRESULT CInteractiveInfo::Refresh_LockOnTarget()
{
	SAFE_RELEASE(m_pLockOnInteractive);
	_vector vCameraLook = m_pGameInstance->Get_CameraLook();
	_vector vCameraPos = m_pGameInstance->Get_CamXMPosition();
	_float fMaxDot = { 0.f };

	_vector vMonsterPos;
	_vector vToMonsterDir;
	_bool bAim = false;
	_float fFovy = 0.f;
	if (m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_PLAYER) != nullptr)
	{
		bAim = m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_PLAYER)->Get_Object<CPlayer>()->Get_Aim();
	}
	if (true == bAim)
	{
		fFovy = max(m_pGameInstance->Get_CameraFov() - XMConvertToRadians(50.f), XMConvertToRadians(5.f));
	}
	else {
		fFovy = max(m_pGameInstance->Get_CameraFov() - XMConvertToRadians(30.f), XMConvertToRadians(20.f));
	}

	{ // 뷰프러스텀 순회해서 가장 중앙에 근접한 물체 찾기
		for (list<CMapElement_Interactable*>::iterator iter = m_ActiveInteractive.begin(); iter != m_ActiveInteractive.end(); ++iter) {

			// (카메라의 룩)과 (카메라 -> 몬스터 방향 벡터)를 내적해서 가장 큰 크기가 나온 몬스타가 락온 대상
			CMapElement_Interactable* pInteractive = (*iter);

			vMonsterPos = pInteractive->Get_LockOnPos();
			vToMonsterDir = vMonsterPos - vCameraPos;
			_float fDotResult = CMyTools::DirectionCompare(vCameraLook, vToMonsterDir);
			if (fDotResult <= cosf(fFovy)) {
				continue;
			}

			if (nullptr == m_pLockOnInteractive) {
				m_pLockOnInteractive = pInteractive;
				fMaxDot = fDotResult;
				continue;
			}

			if (fMaxDot < fDotResult) {
				m_pLockOnInteractive = pInteractive;
				fMaxDot = fDotResult;
			}
		}
	}
	if (nullptr != m_pLockOnInteractive) {
		SAFE_ADDREF(m_pLockOnInteractive);
	}
	return S_OK;
}
HRESULT CInteractiveInfo::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
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

CInteractiveInfo* CInteractiveInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	CInteractiveInfo* pInstance = new CInteractiveInfo();

	if (FAILED(pInstance->Initialize(pDevice, pContex))) {
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CInteractiveInfo::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLockOnInteractive);
	for (CMapElement_Interactable* pInteractive : m_ActiveInteractive) {
		SAFE_RELEASE(pInteractive);
	} m_ActiveInteractive.clear();

	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pInfoInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
