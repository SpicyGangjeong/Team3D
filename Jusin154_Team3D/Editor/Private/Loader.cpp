#include "pch.h"
#include "Loader.h"
#include "DebugCamera.h"
#include "GameInstance.h"
#include "DummyRect.h"
#include "MainApp.h"

#include "Dummy_Cube.h"
#include "Dummy_Goblin.h"

#pragma region UI

#include "Mission.h"
#include "MissionBanner_Border.h"
#include "MissionBanner_Key.h"
#include "Mission_Icon.h"
#include "Mission_Key.h"
#include "Mission_KeyHold.h"
#include "Mouse_Cursor.h"
#include "LodingWidget1.h"
#include "MiniMap_TrimBorder.h"
#include "Active_Icon.h"

#pragma endregion


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
	case LEVEL::UI:
		hr = Loading_For_UI();
		break;
	//case LEVEL::MAP:
	//	hr = Loading_For_MapViewer();
	//	break;
	case LEVEL::OBJECT:
		hr = Loading_For_ObjectViewer();
		break;
	//case LEVEL::COMBINED:
	//	hr = Loading_For_CombinedViewer();
	//	break;
	//case LEVEL::EFFECT:
	//	hr = Loading_For_Effect();
	//	break;
	//case LEVEL::SKIllSTUDIO:
	//	hr = Loading_For_SkillStudio();
	//	break;
	//case LEVEL::PARTICLE:
	//	hr = Loading_For_Particle();
	//	break;
	default:
		assert(false);
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
	m_strMessage = TEXT("�ؽ��ĸ�(��) �ε� �� �Դϴ�.");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Dororong"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/DororongDoro.png"), TEXT("Dororong"), 0)))){
		return E_FAIL;
	}

	m_strMessage = TEXT("�𵨸�(��) �ε� �� �Դϴ�.");

	m_strMessage = TEXT("���̴���(��) �ε� �� �Դϴ�.");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_POSTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxPosTex.hlsl"),
			VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_NORTEX,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxNorTex.hlsl"),
			VTXNORTEX::Elements, VTXNORTEX::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_MESH,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxMesh.hlsl"),
			VTXMESH::Elements, VTXMESH::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_ANIMMESH,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxAnimMesh.hlsl"),
			VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_CELL,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_Cell.hlsl"),
			VTXPOS::Elements, VTXPOS::iNumElements)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_PARTICLE,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxPointParticle.hlsl"),
			VTX_POS_INSTANCE_PARTICLE::Elements, VTX_POS_INSTANCE_PARTICLE::iNumElements)))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("��ü������(��) �ε� �� �Դϴ�.");

	if (FAILED(m_pGameInstance->Add_Prototype<CDummyRect>(g_iStaticLevel, CDummyRect::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDebugCamera>(g_iStaticLevel, CDebugCamera::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	m_strMessage = TEXT("�ε��� �Ϸ�Ǿ����ϴ�..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_UI()
{
	m_strMessage = TEXT("�ؽ��ĸ�(��) �ε� �� �Դϴ�.");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Keyboard"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Keyboard/Keyboard_%d.png"), TEXT("Keyboard"), 10)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Cursor"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Cursor/UI_T_CursorRings.dds"), TEXT("Cursor"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("LodingWidget1"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Loading/LoadingWidget.png"), TEXT("LodingWidget1"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("MissionBanner_Border"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Mission/MissionBanner_Border.png"), TEXT("MissionBanner_Border"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("MissionBanner_Key"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Mission/MissionBanner_Key.png"), TEXT("MissionBanner_Key"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Finishi_Rect"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Mission/Finishi_Rect.png"), TEXT("Finishi_Rect"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("ActiveMission_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Mission/ActiveMission_Icon_%d.png"), TEXT("ActiveMission_Icon"), 2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Mission_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Mission/Mission_Icon_%d.png"), TEXT("Mission_Icon"), 2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("HUD_MiniMap_TrimBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/MiniMap/HUD_MiniMap_TrimBorder.png"), TEXT("HUD_MiniMap_TrimBorder"), 0)))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("�𵨸�(��) �ε� �� �Դϴ�.");

	m_strMessage = TEXT("���̴���(��) �ε� �� �Դϴ�.");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_UIEDITOR,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_UIEditor.hlsl"),
			VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("��ü������(��) �ε� �� �Դϴ�.");

	if (FAILED(m_pGameInstance->Add_Prototype<CMission>(g_iStaticLevel, CMission::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CMouse_Cursor>(g_iStaticLevel, CMouse_Cursor::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLodingWidget1>(g_iStaticLevel, CLodingWidget1::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
		
	if (FAILED(m_pGameInstance->Add_Prototype<CMissionBanner_Border>(g_iStaticLevel, CMissionBanner_Border::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMissionBanner_Key>(g_iStaticLevel, CMissionBanner_Key::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMission_Icon>(g_iStaticLevel, CMission_Icon::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMission_Key>(g_iStaticLevel, CMission_Key::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMission_KeyHold>(g_iStaticLevel, CMission_KeyHold::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CMiniMap_TrimBorder>(g_iStaticLevel, CMiniMap_TrimBorder::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CActive_Icon>(g_iStaticLevel, CActive_Icon::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	m_strMessage = TEXT("�ε��� �Ϸ�Ǿ����ϴ�..");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_ObjectViewer()
{
	m_strMessage = TEXT("�ؽ��ĸ�(��) �ε� �� �Դϴ�.");

	m_strMessage = TEXT("�𵨸�(��) �ε� �� �Դϴ�.");
	
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_GoblinBody_Model"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Goblin/GoblinBody.fbx", MODEL::ANIM, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixIdentity(), 0))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Desc_Box"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Box/Box.fbx", MODEL::NONANIM, XMMatrixIdentity(), 0))))
		return E_FAIL;

	m_strMessage = TEXT("���̴���(��) �ε� �� �Դϴ�.");

	m_strMessage = TEXT("����Ʈ��(��) �ε� �� �Դϴ�.");

	m_strMessage = TEXT("��ü������(��) �ε� �� �Դϴ�.");

	/* For.Prototype_GameObject_Dummy_Goblin */
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_Goblin>(g_iStaticLevel, CDummy_Goblin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dummy_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_Cube>(g_iStaticLevel, CDummy_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strMessage = TEXT("������ �ҷ����� ���Դϴ�.");

	m_strMessage = TEXT("�ε��� �Ϸ�Ǿ����ϴ�..");

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
