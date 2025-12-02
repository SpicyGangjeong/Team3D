#include "pch.h"
#include "PlayerInfo.h"
#include "GameInstance.h"
#include "InfoInstance.h"

CPlayerInfo::CPlayerInfo()
{
}

void CPlayerInfo::Update(_float fTimeDelta)
{
}

void CPlayerInfo::Update_CameraCoordinateSystem(_float3& vLook, _float3& vRight)
{
	m_vCameraLookDir = vLook;
	m_vCameraRightDir = vRight;
}

pair<_float3, _float3> CPlayerInfo::Get_CameraCoordinateSystem()
{
	return { m_vCameraLookDir, m_vCameraRightDir };
}

void CPlayerInfo::Change_Level()
{
}

HRESULT CPlayerInfo::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
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

CPlayerInfo* CPlayerInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContex)
{
	CPlayerInfo* pInstance = new CPlayerInfo();

	if (FAILED(pInstance->Initialize(pDevice, pContex))) {
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CPlayerInfo::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pInfoInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);

}
