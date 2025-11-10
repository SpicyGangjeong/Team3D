#include "pch.h"
#include "Loader.h"
#include "DebugCamera.h"
#include "GameInstance.h"
#include "DummyRect.h"
#include "MainApp.h"
#include "Dummy_Cube.h"

#pragma region OBJECT_HEADER

#include "RootModelPart.h"
#include "Head.h"
#include "Body.h"
#include "Hair.h"
#include "Dummy_Goblin.h"
#include "Dummy_Goblin.h"
#include "Monster.h"
#include <filesystem>

#pragma endregion

#pragma region UI

#include "Mission.h"
#include "MissionBanner_Border.h"
#include "MissionBanner_Key.h"
#include "Mission_Icon.h"
#include "Mission_Key.h"
#include "Mission_KeyHold.h"
#include "Mouse_Cursor.h"

#include "Dummy_Cube.h"
#include "LodingWidget1.h"
#include "MiniMap_TrimBorder.h"
#include "Active_Icon.h"

#pragma endregion

#pragma region MAP_HEADER
#include "VIBuffer_Terrain.h"
#include "Terrain.h"
#include "MapObject.h"
#include "MapObject_Manager.h"
#pragma endregion


#pragma region EFFECT_HEADER

#include "TestEffect.h"
#include "EditEffect.h"
#include "Effect_Editor.h"

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
	case LEVEL::MAP:
		hr = Loading_For_MapViewer();
		break;
	case LEVEL::OBJECT:
		hr = Loading_For_ObjectViewer();
		break;
	//case LEVEL::COMBINED:
	//	hr = Loading_For_CombinedViewer();
	//	break;
	case LEVEL::EFFECT:
		hr = Loading_For_Effect();
		break;
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

	m_strMessage = TEXT("Texture Loading..");


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Dororong"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/DororongDoro.png"), 0)))){
		return E_FAIL;
	}

	m_strMessage = TEXT("Model Loading..");


	m_strMessage = TEXT("Shader Loading..");

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

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Desc_Box"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Box/Box.fbx", MODEL::NONANIM, XMMatrixIdentity(), 0))))
		return E_FAIL;


	m_strMessage = TEXT("Prototype Loading..");

	if (FAILED(m_pGameInstance->Add_Prototype<CDummyRect>(g_iStaticLevel, CDummyRect::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDebugCamera>(g_iStaticLevel, CDebugCamera::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}
	m_strMessage = TEXT("Loading Success!");

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_For_UI()
{


	m_strMessage = TEXT("Texture Loading..");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Keyboard"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Keyboard/Keyboard_%d.png"), 10)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Cursor"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Cursor/UI_T_CursorRings.dds"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("LodingWidget1"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Loading/LoadingWidget.png"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("MissionBanner_Border"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Mission/MissionBanner_Border.png"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("MissionBanner_Key"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Mission/MissionBanner_Key.png"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Finishi_Rect"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Mission/Finishi_Rect.png"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Mission_Main"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Mission/Mission_Main.png"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("HUD_MiniMap_TrimBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/MiniMap/HUD_MiniMap_TrimBorder.png"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("ActiveMission_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/MiniMap/ActiveMission_Icon_%d.png"), 2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Mission_Icon_"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/MiniMap/Mission_Icon_%d.png"), 2)))) {
		return E_FAIL;

	}


	m_strMessage = TEXT("Model Loading..");

	m_strMessage = TEXT("Shader Loading..");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_UIEDITOR,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_UIEditor.hlsl"),
			VTXPOSTEX::Elements, VTXPOSTEX::iNumElements)))) {
		return E_FAIL;
	}
	m_strMessage = TEXT("Prototype Loading..");


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
  

	m_strMessage = TEXT("Loading Success!");


	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Effect()
{


	m_strMessage = TEXT("Texture Loading..");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), TEXT("Prototype_Texture_Test_Noise"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Effect/Test/VFX_T_Noise08_D.png"), 0)))) {
		return E_FAIL;
	}



	Asset_FileLoad("../Bin/Resources/Textures/Effect/Noises", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);
		

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

	});

	Asset_FileLoad("../Bin/Resources/Textures/Effect/Diffuse", L"Prototype_Texture_", [&](_wstring wstrFileName, const _char* pFilePath) {

		_string strFilePath = pFilePath;
		_wstring wstrFilePath = CMyTools::ToWstring(strFilePath);


		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), wstrFileName,
			CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, wstrFilePath.c_str(), 0)))) {
			return E_FAIL;
		}

		return S_OK;

		});

	m_strMessage = TEXT("Model Loading..");

	Asset_FileLoad("../Bin/Resources/Models/Effect/ParticleMesh", L"Prototype_Instance_Model_", [&](_wstring wstrFileName, const _char* pFilePath) {

		if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), wstrFileName,
			CInstance_Model::Create(m_pDevice, m_pContext, pFilePath, MODEL::NONANIM, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixIdentity(), 0))))
			return E_FAIL;

		return S_OK;

	});



	m_strMessage = TEXT("Shader Loading..");


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, FX_INSTANCE_MODEL,
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/ShaderFiles/Shader_VtxModelInstance.hlsl"),
			VTX_MODEL_INSTANCE_PARTICLE::Elements, VTX_MODEL_INSTANCE_PARTICLE::iNumElements)))) {
		return E_FAIL;
	}


	m_strMessage = TEXT("Prototype Loading..");


	if (FAILED(m_pGameInstance->Add_Prototype<CDebugCamera>(ENUM_CLASS(LEVEL::EFFECT), CDebugCamera::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CTestEffect>(ENUM_CLASS(LEVEL::EFFECT), CTestEffect::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CEffect_Editor>(ENUM_CLASS(LEVEL::EFFECT), CEffect_Editor::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CEditEffect>(ENUM_CLASS(LEVEL::EFFECT), CEditEffect::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_Cube>(ENUM_CLASS(LEVEL::EFFECT), CDummy_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strMessage = TEXT("Loading Success!");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Asset_FileLoad(const _char* pDirectoryPath, const _tchar* pPreName, function<HRESULT(_wstring, const _char*)> AddPrototypeEvent)
{
	for (const auto& file : filesystem::directory_iterator(pDirectoryPath))
	{
		if (file.is_directory())
			continue;

		string ext = file.path().extension().string();

		if (!strcmp(ext.c_str(), ".txt"))
			continue;

		_char szFilePath[MAX_PATH] = {};

		strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

		wstring wstrFileName = pPreName + file.path().stem().wstring();

		AddPrototypeEvent(wstrFileName, szFilePath);

	}

	return S_OK;
}



HRESULT CLoader::Loading_For_ObjectViewer()
{
	m_strMessage = TEXT("Texture Loading..");

	m_strMessage = TEXT("Model Loading..");

	
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_GoblinBody_Model"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Models/Goblin/GoblinBody.fbx", MODEL::ANIM, XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixIdentity(), 0))))

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_HumanBody_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Human/Body/Body", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_HumanHead_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Human/Head/Head", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_HumanHair_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Human/Hair/Hair", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_GoblinHead_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Goblin/GoblinHead", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	m_strMessage = TEXT("Shader Loading..");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_GoblinBody_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Goblin/GoblinBody", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Steve_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Steve/Steve",XMMatrixScaling(0.01f,0.01f,0.01f)* XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_TombProtector_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/TombProtector/TombProtector", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Troll_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Troll/Troll", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	m_strMessage = TEXT("Prototype Loading..");

	/* For.Prototype_GameObject_RootModelPart */
	if (FAILED(m_pGameInstance->Add_Prototype<CRootModelPart>(g_iStaticLevel, CRootModelPart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body */
	if (FAILED(m_pGameInstance->Add_Prototype<CBody>(g_iStaticLevel, CBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Head */
	if (FAILED(m_pGameInstance->Add_Prototype<CHead>(g_iStaticLevel, CHead::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hair */
	if (FAILED(m_pGameInstance->Add_Prototype<CHair>(g_iStaticLevel, CHair::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype<CMonster>(g_iStaticLevel, CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Dummy_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_Cube>(g_iStaticLevel, CDummy_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strMessage = TEXT("Loading Success!");

	m_isFinished = true;

	return S_OK;
}


HRESULT CLoader::Loading_For_MapViewer()
{
	m_strMessage = TEXT("텍스쳐를(을) 로딩 중 입니다.");

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("TerrainTest"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/Cursor/UI_T_CursorRings.dds"), 0)))) {
		return E_FAIL;
	}

	m_strMessage = TEXT("모델를(을) 로딩 중 입니다.");

	vector<_wstring> ModelPrototypeTags;

	for (const auto& file : filesystem::directory_iterator("C:\\Users\\82103\\Desktop\\MapRe\\Game\\Environment\\Hogsmeade\\BLDG_ThreeBroomsticks\\Meshes"))

	{
		if (file.is_directory())
			continue;

		string ext = file.path().extension().string();

		if (strcmp(ext.c_str(), ".fbx"))

			continue;

		_char szFilePath[MAX_PATH] = {};

		strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

		_wstring strFileName = L"Prototype_GameObject_" + file.path().stem().wstring();

		/*For Prototype_Component_Model_Auro*/
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, strFileName,
			CModel::Create(m_pDevice, m_pContext, szFilePath, MODEL::ENVIROMENT, XMMatrixIdentity(), 0))))
			return E_FAIL;

		ModelPrototypeTags.push_back(strFileName);
	}


	m_strMessage = TEXT("쉐이더를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");
	
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, nullptr, 100, 100))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapObject */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject>(g_iStaticLevel, CMapObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype<CTerrain>(g_iStaticLevel, CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapObject_Manager */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject_Manager>(g_iStaticLevel, CMapObject_Manager::Create(m_pDevice, m_pContext, ModelPrototypeTags))))
		return E_FAIL;

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
