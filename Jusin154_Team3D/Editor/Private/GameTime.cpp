#include "pch.h"
#include "GameTime.h"
#include "GameInstance.h"

CGameTime::CGameTime()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	SAFE_ADDREF(m_pGameInstance);
}

HRESULT CGameTime::Initialize()
{
	return S_OK;
}

void CGameTime::Update_GameTime(_float fTiemDelta)
{
	m_fGameTime += fTiemDelta;
	m_fDayTime += fTiemDelta;

	if (SECONDS_PER_GAME_DAY <= m_fDayTime)
	{
		m_fDayTime = 0.f;
	}


}

CGameTime* CGameTime::Create()
{
	CGameTime* pInstance = new CGameTime();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CGameTime");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CGameTime::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pGameInstance);
}
