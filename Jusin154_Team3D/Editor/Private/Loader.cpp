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
#include "DummyObject.h"
#include <filesystem>

#pragma endregion

#pragma region UI

#include "GamePlay_Canvas.h"

#include "Mission_Panel.h"
#include "MissionBanner_Border.h"
#include "MissionBanner_Key.h"
#include "Mission_Icon.h"
#include "Mission_Key.h"
#include "Mission_KeyHold.h"
#include "Mouse_Cursor.h"
#include "MiniMap_TrimBorder.h"
#include "Active_Icon.h"

#include "MiniMap_Panel.h"
#include "MiniMap_TrimBorder.h"

#include "Loading_Panel.h"
#include "LoadingWidget.h"
#include "LoadingWidget_Flame.h"

#include "IMGUIUI.h"

#pragma endregion

#pragma region MAP_HEADER
#include "VIBuffer_Terrain.h"
#include "Terrain.h"
#include "BuildingContainer.h"
#include "MapObject_Static.h"
#include "MapObject_LOD.h"
#include "MapObject_Manager.h"
#pragma endregion


#pragma region EFFECT_HEADER

#include "TestEffect.h"
#include "EditEffect.h"
#include "Effect_Editor.h"

#pragma endregion

#pragma region PHYSX_HEADER

#include "Dummy_PhysXBox.h"

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
	case LEVEL::PHYSX:
		hr = Loading_For_PhysXLevel();
		break;
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
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/DororongDoro.png"), 0)))) {
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

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("ActiveMission_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Mission/ActiveMission_Icon_%d.png"), 2)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("HUD_MiniMap_TrimBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/MiniMap/HUD_MiniMap_TrimBorder.png"), 0)))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Mission_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::INCREMENTAL, TEXT("../Bin/Resources/Textures/Mission/Mission_Icon_%d.png"), 2)))) {
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

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("HUD_MiniMap_TrimBorder"),
		CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, TEXT("../Bin/Resources/Textures/MiniMap/HUD_MiniMap_TrimBorder.png"), 0)))) {
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


	if (FAILED(m_pGameInstance->Add_Prototype<CGamePlay_Canvas>(g_iStaticLevel, CGamePlay_Canvas::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CMouse_Cursor>(g_iStaticLevel, CMouse_Cursor::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CLoading_Panel>(g_iStaticLevel, CLoading_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CLoadingWidget>(g_iStaticLevel, CLoadingWidget::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype<CLoadingWidget_Flame>(g_iStaticLevel, CLoadingWidget_Flame::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CMission_Panel>(g_iStaticLevel, CMission_Panel::Create(m_pDevice, m_pContext))))
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
	if (FAILED(m_pGameInstance->Add_Prototype<CActive_Icon>(g_iStaticLevel, CActive_Icon::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CMiniMap_Panel>(g_iStaticLevel, CMiniMap_Panel::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CMiniMap_TrimBorder>(g_iStaticLevel, CMiniMap_TrimBorder::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype<CIMGUIUI>(g_iStaticLevel, CIMGUIUI::Create(m_pDevice, m_pContext))))
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

HRESULT CLoader::Loading_For_PhysXLevel()
{
	m_strMessage = TEXT("PhysX Bodies Loading..");

	{
		CRigidBody::RIGIDBODY_PROTOTYPEDESC Desc{};
		Desc.tRigidDynamicDesc.bExclusive = false;
		Desc.tRigidDynamicDesc.vhalfGeometryInfo = { 0.5f, 0.5f, 0.5f };
		Desc.tRigidDynamicDesc.vMatInfo = { 0.5f, 0.5f, 0.6f };
		Desc.tRigidDynamicDesc.ePxShapeFlag = { PSX::PxShapeFlag::eVISUALIZATION | PSX::PxShapeFlag::eSCENE_QUERY_SHAPE | PSX::PxShapeFlag::eSIMULATION_SHAPE };

		m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX"), CRigidBody::Create(m_pDevice, m_pContext, Desc));
	}
	//{
	//	CRigidBody::RIGIDBODY_PROTOTYPEDESC Desc{};
	//	Desc.tRigidStaticDesc.pMesh;
	//	Desc.tRigidStaticDesc.vMatInfo = { 0.5f, 0.5f, 0.6f };
	//	Desc.tRigidStaticDesc.pMeshKey = TEXT("PHYSX_STATIC_MESH");
	//}

	m_strMessage = TEXT("Texture Loading..");

	m_strMessage = TEXT("Model Loading..");

	m_strMessage = TEXT("Shader Loading..");

	m_strMessage = TEXT("Prototype Loading..");

	if (FAILED(m_pGameInstance->Add_Prototype<CDummy_PhysXBox>(g_iStaticLevel, CDummy_PhysXBox::Create(m_pDevice, m_pContext)))) {
		return E_FAIL;
	}

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


	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_DADA_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Human/Body/DADA/DADA.bin", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_DarkWizardSoldier_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Human/Body/DarkWizardSoldier/DarkWizardSoldier.bin", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_DarkWizardGrunt_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Human/Body/DarkWizardGrunt/DarkWizardGrunt.bin", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_DarkWizardTank_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Human/Body/DarkWizardTank/DarkWizardTank.bin", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Young_M_Head_Ivory_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Human/Head/Young_M_Head_Ivory.bin", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Hair1_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Human/Hair/Hair1/Hair1.bin", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Hair2_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Human/Hair/Hair2/Hair2.bin", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_GoblinHead_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Goblin/GoblinHead.bin", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_GoblinBody_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Goblin/GoblinBody.bin", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Steve_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Steve/Steve.bin", XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_TombProtector_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/TombProtector/TombProtector.bin", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Troll_Model"),
		CModel::Create(m_pDevice, m_pContext, MODEL::ANIM, "../Bin/Resources/Models/Monster/Troll/Troll.bin", XMMatrixScaling(0.001f, 0.001f, 0.001f) * XMMatrixIdentity()))))
		return E_FAIL;

	m_strMessage = TEXT("Shader Loading..");

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

	/* For.Prototype_GameObject_DummyObject */
	if (FAILED(m_pGameInstance->Add_Prototype<CDummyObject>(g_iStaticLevel, CDummyObject::Create(m_pDevice, m_pContext))))
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

		/*For Prototype_Component_Model_*/
		if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, strFileName,
			CModel::Create(m_pDevice, m_pContext,szFilePath, MODEL::ENVIROMENT, XMMatrixIdentity(), 0))))
			return E_FAIL;

		ModelPrototypeTags.push_back(strFileName);
	}


	m_strMessage = TEXT("쉐이더를(을) 로딩 중 입니다.");

	m_strMessage = TEXT("객체원형를(을) 로딩 중 입니다.");

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Asset_Prototype(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, nullptr, 100, 100))))
		return E_FAIL;

	/* For.Prototype_GameObject_BuildingContainer */
	if (FAILED(m_pGameInstance->Add_Prototype<CBuildingContainer>(g_iStaticLevel, CBuildingContainer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapObject_Static */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject_Static>(g_iStaticLevel, CMapObject_Static::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_MapObject_LOD */
	if (FAILED(m_pGameInstance->Add_Prototype<CMapObject_LOD>(g_iStaticLevel, CMapObject_LOD::Create(m_pDevice, m_pContext))))
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
