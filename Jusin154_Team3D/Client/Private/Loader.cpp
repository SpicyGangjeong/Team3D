#include "pch.h"
#include "Loader.h"
#include "GameInstance.h"
#include "MainApp.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pGameInstance);
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
}

unsigned int APIENTRY LoadingMain(void* pArg)
{
	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading())) {
		assert(false);
		return 1;
	}

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{

	if (FAILED(CoInitializeEx(nullptr, 0))) {
		return E_FAIL;
	}

	EnterCriticalSection(&m_CriticalSection);

	HRESULT		hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_Logo();
		break;
	case LEVEL::GAMEPLAY:
		hr = Loading_For_GamePlay();
		break;
	default:
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

void CLoader::Output()
{
	SetWindowText(g_hWnd, m_strMessage.c_str());
}

HRESULT CLoader::Loading_For_Logo()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("사운드를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");


	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("셰이더를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("이펙트를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("정보를 불러오는 중입니다.");

	m_strMessage = TEXT("로딩이 완료되었습니다..");

	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);
}
