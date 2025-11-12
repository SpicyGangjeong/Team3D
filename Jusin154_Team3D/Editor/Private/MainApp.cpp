#include "pch.h"
#include "MainApp.h"

#include "GameInstance.h"

#include "Level_Loading.h"

_float g_fTimeMult = 1.f;
CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	SAFE_ADDREF(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC				EngineDesc{};
	EngineDesc.hInstance = g_hInstance;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iStaticLevel = g_iStaticLevel;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);
	EngineDesc.iNumCollidableGroup = ENUM_CLASS(COLLIDABLEOBJECT::END);
	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Default_Setting())){
		return E_FAIL;
	}

	if (FAILED(Ready_Prototypes())){
		return E_FAIL;
	}

	if (FAILED(Start_Level(LEVEL::LOGO))){
		return E_FAIL;
	}

	if (FAILED(Ready_IMGUI())){
		return E_FAIL;
	}

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	GUI::NewFrame();
#ifdef _DEBUG
	m_pGameInstance->Present_TimeCost();
#endif // _DEBUG
	GUI::DragFloat("fTimeMult", &g_fTimeMult, 0.01f, 0.f, 4.f);
	m_pGameInstance->Update_Engine(fTimeDelta * g_fTimeMult);
}

HRESULT CMainApp::Render()
{
	_float4			vClearColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pGameInstance->Render_Begin(&vClearColor);

	m_pGameInstance->Draw();
	GUI::Render();
	ImGui_ImplDX11_RenderDrawData(GUI::GetDrawData());

	m_pGameInstance->Render_End();

	return S_OK;
}

void CMainApp::Compute_FrameCount()
{
	m_pGameInstance->Compute_FrameCount();
}

HRESULT CMainApp::Ready_Default_Setting()
{
	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototypes()
{
	return S_OK;
}

HRESULT CMainApp::Ready_IMGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImFont* font = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	//string strFontTag = "C:\\Windows\\Fonts\\malgun.ttf";
	string strFontTag = "../Bin/Resources/Fonts/malgunsl.TTF";
	ImFontConfig cfg;
	cfg.OversampleH = 2;  // 가독성 향상
	cfg.OversampleV = 2;
	cfg.PixelSnapH = true;
	ifstream ifile;

	ifile.open(strFontTag);
	if (ifile)
	{
		font = io.Fonts->AddFontFromFileTTF(strFontTag.c_str(), 16.f, NULL, io.Fonts->GetGlyphRangesKorean());
	}
	IM_ASSERT(font != NULL);
	io.FontDefault = font;
	return S_OK;
}

HRESULT CMainApp::Release_IMGUI()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();


	Release_IMGUI();

	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);

	m_pGameInstance->Release_Engine();

	SAFE_RELEASE(m_pGameInstance);
}
