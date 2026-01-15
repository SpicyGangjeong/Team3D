#include "pch.h"
#include "Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "InfoInstance.h"

#include "Logo.h"
#include "Logo_Text.h"
#include "Logo_Glow.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{
	m_pInfoInstance = CInfoInstance::GetInstance();
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_UI(LAYER_UI))) {
		return E_FAIL;
	}

	m_pGameInstance->Sound_Play(SOUND::SD_KIND::LOGO_INTRO, SD_CHANNEL_GROUP::EFFECT, false, 0.4f);
	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
#ifdef _DEBUG
#ifdef 기무리
	m_pGameInstance->Set_LevelToChange();
#elif Bin
	m_pGameInstance->Set_LevelToChange();
#endif // 기무리
#endif // _DEBUG

	if (m_pGameInstance->Key_Down(DIK_F1))
	{
		m_pGameInstance->Set_LevelToChange();
	}

	//m_pInfoInstance->Update(fTimeDelta);

	if (true == m_pGameInstance->Check_LevelShouldChange()) {
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::GAMEPLAY)))) {
			return;
		}
	}
 


}

HRESULT CLevel_Logo::Render()
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
		TEXT("로고 레벨입니다 | Frame: %.3f ms | Avg: %.3f ms | FPS: %.1f"),
		fCurrentFrameTimeMilliseconds,
		fAverageFrameTimeMilliseconds,
		fCurrentFps
	);

	SetWindowText(g_hWnd, szWindowTitle);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLogo>(g_iStaticLevel, NEXT_LEVEL, strLayerTag))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLogo_Text>(g_iStaticLevel, NEXT_LEVEL, strLayerTag))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLogo_Glow>(g_iStaticLevel, NEXT_LEVEL, strLayerTag))) {
		return E_FAIL;
	}

	return S_OK;
}

pair<CLevel_Logo*, function<void()>> CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		SAFE_RELEASE(pInstance);
	}

	return { pInstance, nullptr };
}

void CLevel_Logo::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);
}
