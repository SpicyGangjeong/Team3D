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
	_float fDeltaTimeSeconds = m_pGameInstance->Get_TimeDelta(TEXT("Timer_60"));

	static _float fAccumulatedTimeSeconds = 0.0f;
	static _uint  iFrameCountForFps = 0;
	static _float fCurrentFps = 0.0f;
	static _float fAverageFrameTimeMilliseconds = 0.0f;

	fAccumulatedTimeSeconds += fDeltaTimeSeconds;
	++iFrameCountForFps;

	if (fAccumulatedTimeSeconds >= 1.0f)
	{
		if (fAccumulatedTimeSeconds > 0.0f)
		{
			fCurrentFps = static_cast<_float>(iFrameCountForFps) / fAccumulatedTimeSeconds;
			if (fCurrentFps > 0.0f)
			{
				fAverageFrameTimeMilliseconds = 1000.0f / fCurrentFps;
			}
		}

		fAccumulatedTimeSeconds = 0.0f;
		iFrameCountForFps = 0;
	}

	_float fCurrentFrameTimeMilliseconds = fDeltaTimeSeconds * 1000.0f;

	_tchar szWindowTitle[256] = {};

	// 현재 프레임 시간 + 평균 FPS / 평균 프레임 타임(ms)
	_stprintf_s(
		szWindowTitle,
		TEXT("리스타트레벨입니다 | Frame: %.3f ms | Avg: %.3f ms | FPS: %.1f"),
		fCurrentFrameTimeMilliseconds,
		fAverageFrameTimeMilliseconds,
		fCurrentFps
	);

	SetWindowText(g_hWnd, szWindowTitle);

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
