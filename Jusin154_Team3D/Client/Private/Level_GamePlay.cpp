#include "pch.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Light_Main.h"
#include "Camera_Debug.h"
#include "InfoInstance.h"
#include "UI_Manager.h"
#include "Layer.h"
#include "SkyBox.h"
#include "Broom.h"
#include "Terrain.h"
#include "EffectPool.h"
#include "InstancedProp.h"
#include "InstancedProp_Light.h"
#include "Land.h"
#include "Unified.h"
#include "MapElement_Lake.h"
#include "RaceRing.h"
#include "BroomRaceManager.h"
#include "ReparoObject.h"
#include "ThestralCarriage.h"
#include "Camera_Cinematic.h"

#pragma region ACTOR
#include "Player.h"
#include "Human_Duelist.h"
#include "Troll.h"
#include "Goblin.h"
#include "Goblin_Mage.h"
#include "Goblin_Assassin.h"
#include "Goblin_Spector.h"
#include "NPC_Ollivander.h"
#include "NPC_EleazarFig.h"
#include "BroomRacerAI.h"
#include "Ranrok.h"
#include "RandomNpc.h"
#include "Elf.h"
#pragma endregion


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{
	m_pInfoInstance = CInfoInstance::GetInstance();
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CLevel_GamePlay::Initialize(void* pArg)
{
#ifdef _DEBUG
	// 낮, 밤 설정
#ifdef gimch
	m_isDay = false;
#endif // gimch
#ifdef Bin
	m_isDay = true;
#endif // 
#ifdef 진우
	m_isDay = false;
#endif // 
#ifdef 기무리
	m_isDay = true;
#endif // 
#ifdef 나
	m_isDay = false;
#endif // 
#endif // _DEBUG

	if (FAILED(Ready_Lights())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Volumetric())) {
		return E_FAIL;
	}

	//플레이어 , 맵 보다 먼저 생성해야함 !
	if (FAILED(Reday_Layer_EffectPool())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Background())) {
		return E_FAIL;
	}

	map<_string, CLand*> Lands = {};

	if (FAILED(Ready_Land(&Lands))) {
		return E_FAIL;
	}

	if (FAILED(Ready_IntstanceProp(&Lands)))
		return E_FAIL;

	if (FAILED(Ready_Markers())) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_UI(LAYER_UI))) {
		return E_FAIL;
	}

	// 이것도 플레이어보다 먼저 생성해야함!
	if (FAILED(Ready_Layer_Item(LAYER_ITEM))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Manager(TEXT("Layer_Manager")))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_ReparoObject(TEXT("Layer_ReparoObject")))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_SkyBox(TEXT("Layer_SkyBox")))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Player(LAYER_PLAYER))) {
		return E_FAIL;
	}

	_bool bLoadNPC = { true };
#ifdef _DEBUG
#ifdef 기무리
	bLoadNPC = false;
#endif
#endif // _DEBUG
	if (true == bLoadNPC) {
		if (FAILED(Ready_Layer_RacerAI(LAYER_RACERAI))) {
			return E_FAIL;
		}
		if (FAILED(Ready_Layer_Duelist())) {
			return E_FAIL;
		}

		if (FAILED(Ready_Layer_Npc())) {
			return E_FAIL;
		}
	}
	

	if (FAILED(Ready_Layer_Monster())) {
		return E_FAIL;
	}

	if (FAILED(m_pInfoInstance->Late_Initialize()))
		return E_FAIL;

	m_bLevel = true;
	m_pInfoInstance->Event_CallBack(TEXT("UIManagerFadeIn"));


	return S_OK;
}

HRESULT CLevel_GamePlay::Initialize()
{
	assert(false);
	return E_FAIL;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{

	if (m_pGameInstance->Key_Pressing(DIK_0)) {
		if (m_pGameInstance->Key_Up(DIK_1))
		{
			m_pGameInstance->Set_LevelToChange();
		}
	}

	m_pInfoInstance->Update(fTimeDelta);

	if (true == m_pGameInstance->Check_LevelShouldChange()) {
		m_pInfoInstance->Change_Level();

		UI_STATE eState = UI_STATE::LEVELCHANGE;
		m_pInfoInstance->Event_CallBack(TEXT("Canvas_Change"), &eState);

		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::FIELD)))) {
			return;
		}
	}
}

HRESULT CLevel_GamePlay::Render()
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
		TEXT("게임플레이레벨입니다 | Frame: %.3f ms | Avg: %.3f ms | FPS: %.1f"),
		fCurrentFrameTimeMilliseconds,
		fAverageFrameTimeMilliseconds,
		fCurrentFps
	);

	SetWindowText(g_hWnd, szWindowTitle);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{

	LIGHT_DESC Desc = {};
	if (m_isDay)
	{
		Desc.vDiffuse = _float4(0.6529f, 0.6157f, 0.7843f, 1.0f);
		Desc.vAmbient = _float4(0.6275f, 0.6275f, 0.6275f, 0.0314f);
		Desc.vSpecular = _float4(0.05f, 0.05f, 0.05f, 0.05f);
	}
	else
	{
		Desc.vDiffuse = _float4(0.0471f, 0.0745f, 0.1294f, 0.2549f);
		Desc.vAmbient = _float4(0.1686f, 0.1765f, 0.1373f, 0.0f);
		Desc.vSpecular = _float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLight_Main>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_LIGHT, &Desc, nullptr, &m_pLight))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Volumetric()
{
	// Volumetric 설정
	if (m_isDay)
	{
		m_pGameInstance->Setting_Volumetirc(
			1.251f,                         // 밀도
			0.0253f,                          // 빛 강도
			0.9f,                          // 산란 계수
			1.78f,                           // 깊이 분포 계수
			0.f
		);
	}
	else
	{
		m_pGameInstance->Setting_Volumetirc(
			0.626f,                         // 밀도
			0.01f,                          // 빛 강도
			0.11f,                          // 산란 계수
			1.0f,                           // 깊이 분포 계수
			0.f
		);
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Background()
{
	CTerrain::TERRAIN_DESC Terrain_Desc = {};
	/* Hogsmeade Terrain */
	Terrain_Desc.vPosition = _float3(-194, 18.5f, -153.f);
	Terrain_Desc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Terrain_Hogsmeade");
	Terrain_Desc.strDiffuseTextureTag = TEXT("Terrain_Diffuse");
	Terrain_Desc.strNormalTextureTag = TEXT("Terrain_Normal");
	Terrain_Desc.strMROTextureTag = TEXT("Terrain_MRO");
	Terrain_Desc.strAlphaMapTextureTag = TEXT("Hogsmeade_AlphaMap");
	Terrain_Desc.strRigidBody_MeshName = TEXT("Hogsmeade_HeightMap");
	Terrain_Desc.strRigidBody_ComponentTag = TEXT("Prototype_Component_RigidBody_Static_Terrain_Hogsmeade");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTerrain>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_BACKGROUND, &Terrain_Desc))) {
		return E_FAIL;
	}
	/* Hogwart Terrain */
	Terrain_Desc.vPosition = _float3(-451.f, 18.5f, -791.f);
	Terrain_Desc.strVIBufferTag = TEXT("Prototype_Component_VIBuffer_Terrain_Hogwart");
	Terrain_Desc.strDiffuseTextureTag = TEXT("Terrain_Diffuse");
	Terrain_Desc.strNormalTextureTag = TEXT("Terrain_Normal");
	Terrain_Desc.strMROTextureTag = TEXT("Terrain_MRO");
	Terrain_Desc.strAlphaMapTextureTag = TEXT("Hogwart_AlphaMap");
	Terrain_Desc.strRigidBody_MeshName = TEXT("Hogwart_HeightMap");
	Terrain_Desc.strRigidBody_ComponentTag = TEXT("Prototype_Component_RigidBody_Static_Terrain_Hogwart");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTerrain>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_BACKGROUND, &Terrain_Desc))) {
		return E_FAIL;
	}

	/* 물 오브젝트 */
	if (FAILED(CInfoInstance::GetInstance()->Load_WaterElemet("Element_Water_Info"))) {
		return E_FAIL;
	}

	// ---------------------------------
	// >> M A P Configuration <<
	// 맵 로드할지 안할지 bool 설정
	// ---------------------------------
	_bool isReady_Background = { true };
	_bool isReady_Hogsmeade = { true };
	_bool isReady_Hogwart = { true };
#ifdef _DEBUG

#ifdef gimch
	isReady_Background = true;
	isReady_Hogsmeade = true;
	isReady_Hogwart = false;
#endif // gimch
#ifdef Bin
	isReady_Background = false;
	isReady_Hogsmeade = false;
	isReady_Hogwart = false;
#endif // 
#ifdef 진우
	isReady_Background = false;
	isReady_Hogsmeade = false;
	isReady_Hogwart = false;
#endif // 
#ifdef 기무리
	isReady_Background = false;
	isReady_Hogsmeade = false;
	isReady_Hogwart = false;
#endif // 
#ifdef 나
	isReady_Background = false;
	isReady_Hogsmeade = false;
	isReady_Hogwart = false;
#endif // 
#endif // _DEBUG



	/* Map Containters */
	if (false == isReady_Background)
	{
		/* 테스트용 맵 */
		CInfoInstance::GetInstance()->Load_MapObjects("Map1215", LAYER_HOGSMEADE);
	}
	else
	{
#if 진우

#else
	
#endif
		if (isReady_Hogsmeade)
		{
			if (FAILED(Ready_Layer_Hogsmeade()))
				return E_FAIL;
		}

		if (isReady_Hogwart)
		{
			if (FAILED(Ready_Layer_Hogwart()))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Land(map<_string, CLand*>* Lands)
{
	CLand* pLand = { nullptr };

	CLand::LAND_DESC Land_Desc = {};

	/* HN_BC */
	Land_Desc.vPosition = _float3(290.5f, 59.5f, -347.f);
	Land_Desc.vScale = _float3(1.f, 1.25f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_South_Hogwart_Land_LOD1";
	Land_Desc.strAlphaMapTag = L"Land_HN_BC_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;
	Lands->emplace("HN_BC", pLand);

	/* HN_AV */
	Land_Desc.vPosition = _float3(-370.f, -37.5f, -21.4f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_North_Hogwart_Land_LOD1";
	Land_Desc.strAlphaMapTag = L"Land_HN_AV_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;
	Lands->emplace("HN_AV", pLand);


	/* HN_AU */
	Land_Desc.vPosition = _float3(-378.f, -17.5f, 285.6f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_North_Hogwart2_Land_LOD1";
	Land_Desc.strAlphaMapTag = L"Land_HN_AU_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;
	Lands->emplace("HN_AU", pLand);

	/* HN_AZ */
	Land_Desc.vPosition = _float3(-653.f, 20.f, -327.4f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_West_Hogwart_Land_LOD1";
	Land_Desc.strAlphaMapTag = L"Land_HN_AZ_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;
	Lands->emplace("HN_AZ", pLand);

	/* HN_AY */
	Land_Desc.vPosition = _float3(436.f, 55.f, 60.3f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_East_Hogsmeade_Land_LOD1";
	Land_Desc.strAlphaMapTag = L"Land_HN_AY_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;

	Lands->emplace("HN_AY", pLand);

	/* HN_BA */
	Land_Desc.vPosition = _float3(-606.f, 70.f, -647.7f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BA";
	Land_Desc.strAlphaMapTag = L"Land_HN_BA_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;
	Lands->emplace("HN_BA", pLand);

	/* HN_BD */
	Land_Desc.vPosition = _float3(201.4f, -22.9f, -650.1f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BD";
	Land_Desc.strAlphaMapTag = L"Land_HN_BD_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;
	Lands->emplace("HN_BD", pLand);

	/* HN_BG */
	Land_Desc.vPosition = _float3(-377.8f, 5.f, -1038.49f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BG";
	Land_Desc.strAlphaMapTag = L"Land_HN_BG_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;
	Lands->emplace("HN_BG", pLand);

	/* HN_BH */
	Land_Desc.vPosition = _float3(80.1f, -63.f, -908.3f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BH";
	Land_Desc.strAlphaMapTag = L"Land_HN_BH_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;
	Lands->emplace("HN_BH", pLand);

	/* HN_BE */
	Land_Desc.vPosition = _float3(431.2f, 100.1f, -658.3f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BE";
	Land_Desc.strAlphaMapTag = L"Land_HN_BE_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;
	Lands->emplace("HN_BE", pLand);

	/* HN_BF */
	Land_Desc.vPosition = _float3(-688.9f, 170.7f, -1066.f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BF";
	Land_Desc.strAlphaMapTag = L"Land_HN_BF_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;
	Lands->emplace("HN_BF", pLand);

	/* HN_BI */
	Land_Desc.vPosition = _float3(82.7f, -61.f, -1141.6f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BI";
	Land_Desc.strAlphaMapTag = L"Land_HN_BI_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;
	Lands->emplace("HN_BI", pLand);

	/* HN_BJ */
	Land_Desc.vPosition = _float3(367.1f, 44.8f, -1016.f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BJ";
	Land_Desc.strAlphaMapTag = L"Land_HN_BJ_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;
	Lands->emplace("HN_BJ", pLand);

	/* TU_BB */
	Land_Desc.vPosition = _float3(-829.5f, 58.3f, 112.9f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_TU_BB";
	Land_Desc.strAlphaMapTag = L"Land_TU_BB_AlphaMap";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Land_Desc, nullptr, &pLand)))
		return E_FAIL;
	Lands->emplace("TU_BB", pLand);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Hogsmeade()
{
	/* 전체 맵 */
	CInfoInstance::GetInstance()->Load_MapObjects("Hogsmeade_MapContainer_Data", LAYER_HOGSMEADE);

	/* 조명 오브젝트 */
	CInfoInstance::GetInstance()->Load_LightElements("LightElement", LAYER_HOGSMEADE);

	/* 상호작용 오브젝트 */
	CInfoInstance::GetInstance()->Load_InteractableElements("E_INTER_Barrel", LAYER_HOGSMEADE);
	CInfoInstance::GetInstance()->Load_InteractableElements("E_INTER_PostPackage_B", LAYER_HOGSMEADE);
	CInfoInstance::GetInstance()->Load_InteractableElements("E_INTER_PostPackage_F", LAYER_HOGSMEADE);
	CInfoInstance::GetInstance()->Load_InteractableElements("E_INTER_TeaShopTable", LAYER_HOGSMEADE);
	CInfoInstance::GetInstance()->Load_InteractableElements("E_INTER_TeaShopChair", LAYER_HOGSMEADE);

	/* Doors */
	CInfoInstance::GetInstance()->Load_DoorElemet("Element_Door_Info", LAYER_HOGSMEADE);

	/* Chests */
	CInfoInstance::GetInstance()->Load_ChestElemet("Element_Chest_Info", LAYER_HIDDEN);

	/* Hogsmeade Unified*/
	CUnified::UNIFIED_DESC Desc = {};
	Desc.fLodSwitchDistnace = 500.f;
	Desc.vUnifiedCenterPos = _float4(62.f, 10.f, 93.f, 1.f);
	Desc.vPosition = _float3(62.f, 10.f, 93.f);
	Desc.vRotation = _float3(0.f, 0.f, 0.f);
	Desc.vScale = _float3(1.f, 1.f, 1.f);
	Desc.srtLayerTag = LAYER_HOGSMEADE;
	Desc.srtModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HM_Unified_LOD1_Baked_0"));
	Desc.srtModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HM_Unified_LOD1_Baked_1"));
	Desc.srtModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HM_Unified_LOD1_Baked_2"));
	Desc.srtModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HM_Unified_LOD1_Baked_3"));
	Desc.srtModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HM_Unified_Streets_LOD1_Merged_0"));

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CUnified>(g_iStaticLevel, NEXT_LEVEL, LAYER_UNIFIED, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Hogwart()
{
	CInfoInstance::GetInstance()->Load_MapObjects("Hogwart_MapContainer_Data", LAYER_HOGWART);
	CInfoInstance::GetInstance()->Load_MapObjects("HogwartMap1221", LAYER_HOGWART);

	CUnified::UNIFIED_DESC Desc = {};

	/* Hogwart Unified*/
	Desc.fLodSwitchDistnace = 500.f;
	Desc.vUnifiedCenterPos = _float4(-291.2f, 17.2f, -500.7f, 1.f);
	Desc.vPosition = _float3(-291.2f, 17.2f, -474.7f);
	Desc.vRotation = _float3(0.f, 0.f, 0.f);
	Desc.vScale = _float3(1.f, 1.f, 1.f);
	Desc.srtLayerTag = LAYER_HOGWART;
	Desc.srtModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HW_HogwartsShell_B"));

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CUnified>(g_iStaticLevel, NEXT_LEVEL, LAYER_UNIFIED, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_IntstanceProp(map<_string, CLand*>* Lands)
{
	CInstancedProp* pInstancedProp = { nullptr };
	CInstancedProp::INSTANCE_PROP_DESC Desc = {};

	/* InstanceProp Oak_Tree*/
	Desc.isShake = true;
	Desc.eShaderPass = SHADER_PASS_WORLDMODLE_INSTANCE::DEFAULT;
	Desc.bEnableRigidbody = true;
	Desc.vRadius = _float2(0.015f, 0.02f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_OakTree_MedA";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/OakTree_MedA.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc))) {
		return E_FAIL;
	}
	Desc.isShake = false;
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/OakTree_MedA_HN_BB.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGWART, &Desc))) {
		return E_FAIL;
	}


	/* BearBerry */
	Desc.isShake = true;
	Desc.bEnableRigidbody = false;
	Desc.vRadius = _float2(0.015f, 0.02f);
	Desc.vSpeed = _float2(0.6f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_BearBerry_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/BearBerry_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc))) {
		return E_FAIL;
	}

	/* SM_HM_OwlPost_Window_A */
	Desc.isShake = false;
	Desc.bEnableRigidbody = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_HM_OwlPost_Window_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/OwlPost_Window.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc)))
		return E_FAIL;

	///* WA_Rectangle_Double_A */
	Desc.isShake = false;
	Desc.bEnableRigidbody = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WA_Rectangle_Double_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Rectangle_Double_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc)))
		return E_FAIL;

	///* WC_Retangle_Double_A */
	Desc.isShake = false;
	Desc.bEnableRigidbody = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WC_Retangle_Double_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/WC_Retangle_Double_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc)))
		return E_FAIL;

	///* WA_Square_Double_C */
	Desc.isShake = false;
	Desc.bEnableRigidbody = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WA_Square_Double_C";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/WA_Square_Double_C.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc)))
		return E_FAIL;

	/* Quid_Window_A */
	Desc.isShake = false;
	Desc.bEnableRigidbody = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Quid_Window_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Quid_Window_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc)))
		return E_FAIL;

	/* Ollivanders_Box_Window */
	Desc.isShake = false;
	Desc.bEnableRigidbody = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Ollivanders_Box_Window";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Ollivanders_Box.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc)))
		return E_FAIL;

	/* WC_L_DoubleS_A */
	Desc.isShake = false;
	Desc.bEnableRigidbody = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WC_L_DoubleS_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/WC_L_DoubleS_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc)))
		return E_FAIL;

	/* WC_Round_Double_A */
	Desc.isShake = false;
	Desc.bEnableRigidbody = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WC_Round_Double_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/WC_Round_Double_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc)))
		return E_FAIL;

	/* SM_HM_Door1a */
	Desc.isShake = false;
	Desc.bEnableRigidbody = true;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door1a";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Door1a.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc)))
		return E_FAIL;

	/* SM_HM_Door2b */
	Desc.isShake = false;
	Desc.bEnableRigidbody = true;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door2b";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Door2b.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc)))
		return E_FAIL;

	/* OakTree_TallA */
	Desc.isShake = false;
	Desc.bEnableRigidbody = true;
	Desc.vRadius = _float2(0.015f, 0.016f);
	Desc.vSpeed = _float2(0.01f, 0.1f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_OakTree_TallA";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/OakTree_TallA.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	/* Shrub_B */
	Desc.isShake = true;
	Desc.bEnableRigidbody = false;
	Desc.vRadius = _float2(0.015f, 0.04f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Shrub_B";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Shrub_B.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	/* BogMyrtle_A */
	Desc.isShake = true;
	Desc.bEnableRigidbody = false;
	Desc.vRadius = _float2(0.015f, 0.04f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_BogMyrtle_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/BogMyrtle_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;
	Desc.isShake = false;
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/BogMyrtle_A_HN_BB.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGWART, &Desc)))
		return E_FAIL;

	/* Dogwood_B */
	Desc.isShake = true;
	Desc.bEnableRigidbody = true;
	Desc.vRadius = _float2(0.015f, 0.02f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Dogwood_B";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Dogwood_B.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	/* ScotsPine_LargeA */
	Desc.isShake = false;
	Desc.bEnableRigidbody = true;
	Desc.vRadius = _float2(0.015f, 0.02f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_ScotsPine_LargeA";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/ScotsPine_LargeA.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/ScotsPine_LargeA_HN_BB.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGWART, &Desc)))
		return E_FAIL;
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/ScotsPine_LargeA_HN_BB_2.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGWART, &Desc)))
		return E_FAIL;
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/ScotsPine_LargeA_HN_AZ.bin";
	if (FAILED(Bind_InstanceProp(Lands, m_pGameInstance->Clone_Prototype<CInstancedProp>(g_iStaticLevel, &Desc), "HN_AZ")))
		return E_FAIL;
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/ScotsPine_LargeA_HN_AV.bin";
	if (FAILED(Bind_InstanceProp(Lands, m_pGameInstance->Clone_Prototype<CInstancedProp>(g_iStaticLevel, &Desc), "HN_AV")))
		return E_FAIL;

	/* StratifiedCliff_A1 */
	Desc.isShake = false;
	Desc.bEnableRigidbody = true;
	Desc.vRadius = _float2(0.f, 0.f);
	Desc.vSpeed = _float2(0.f, 0.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_StratifiedCliff_A1";

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/CliffwallA_HN_BC.bin";
	if (FAILED(Bind_InstanceProp(Lands, m_pGameInstance->Clone_Prototype<CInstancedProp>(g_iStaticLevel, &Desc), "HN_BC")))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/CliffwallA_HN_BB.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGWART, &Desc)))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/CliffwallA_HN_AZ.bin";
	if (FAILED(Bind_InstanceProp(Lands, m_pGameInstance->Clone_Prototype<CInstancedProp>(g_iStaticLevel, &Desc), "HN_AZ")))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/CliffwallA_HN_AV.bin";
	if (FAILED(Bind_InstanceProp(Lands, m_pGameInstance->Clone_Prototype<CInstancedProp>(g_iStaticLevel, &Desc), "HN_AV")))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/CliffwallA_HN_AW.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc)))
		return E_FAIL;

	/* StratifiedRock_B */
	Desc.isShake = false;
	Desc.bEnableRigidbody = true;
	Desc.vRadius = _float2(0.f, 0.f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_StratifiedRock_B";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/StratifiedRock_B_HN_BB.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGWART, &Desc)))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/StratifiedRock_B_HN_AZ.bin";
	if (FAILED(Bind_InstanceProp(Lands, m_pGameInstance->Clone_Prototype<CInstancedProp>(g_iStaticLevel, &Desc), "HN_AZ")))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/StratifiedRock_B_HN_AW.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc)))
		return E_FAIL;

	/* StratifiedRock_D_B */
	Desc.isShake = false;
	Desc.bEnableRigidbody = true;
	Desc.vRadius = _float2(0.f, 0.f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_StratifiedRock_D_B";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/StratifiedRock_D_B_HN_BB.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGWART, &Desc)))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/StratifiedRock_D_B_BACK.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	/* Stone_FrontSteps */
	Desc.isShake = false;
	Desc.bEnableRigidbody = true;
	Desc.vRadius = _float2(0.f, 0.f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Stone_FrontSteps";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/FrontSteps_A_HN_AW.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Desc)))
		return E_FAIL;

	/* StoneKit_A */
	Desc.isShake = false;
	Desc.bEnableRigidbody = true;
	Desc.vRadius = _float2(0.f, 0.f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_StoneKit_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/StoneKit_A_HW.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;



	CInstancedProp_Light::INSTANCE_PROP_LIGHT_DESC LightDesc = {};

	/* LightPost */
	LightDesc.isShake = false;
	LightDesc.bEnableRigidbody = true;
	LightDesc.iGlassMeshIndex = 0;
	LightDesc.vRadius = _float2(0.f, 0.f);
	LightDesc.vSpeed = _float2(0.f, 0.f);
	LightDesc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_LightPost";
	LightDesc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/LightPost.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp_Light>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &LightDesc)))
		return E_FAIL;

	/* LightPost_Floating */
	LightDesc.isShake = false;
	LightDesc.bEnableRigidbody = true;
	LightDesc.iGlassMeshIndex = 1;
	LightDesc.vRadius = _float2(0.f, 0.f);
	LightDesc.vSpeed = _float2(0.f, 0.f);
	LightDesc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Light_Post_Floating";
	LightDesc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Light_Post_Floating.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp_Light>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CUI_Manager>(g_iStaticLevel, g_iStaticLevel, LAYER_UI))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera()
{
#ifdef _DEBUG
	{
		CCamera_Debug::CAMERA_DEBUG_DESC            Camera_Desc{};
		Camera_Desc.fFovy = XMConvertToRadians(60.0f);
		Camera_Desc.fNear = 0.1f;
		Camera_Desc.fFar = 500.f;
		Camera_Desc.vEye = _float3(0.f, 10.f, -10.f);
		Camera_Desc.vAt = _float3(0.f, 0.f, 0.f);
		Camera_Desc.fSpeedPerSec = 5.f;
		Camera_Desc.pCameraKey = CAMERA_DEBUG;
		Camera_Desc.fRotationPerSec = XMConvertToRadians(90.0f);
		Camera_Desc.fMouseSensor = 0.1f;
		Camera_Desc.iPriority = 53;
		Camera_Desc.pFollowTarget = { nullptr };
		Camera_Desc.pLookTarget = { nullptr };

		CCamera_Debug* pCamera = { nullptr };
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCamera_Debug>(g_iStaticLevel, NEXT_LEVEL, LAYER_CAMERA, &Camera_Desc, nullptr, &pCamera))) {
			return E_FAIL;
		}

		m_pGameInstance->Add_Camera(NEXT_LEVEL, pCamera, CAMERA_DEBUG);
	}

#endif // _DEBUG

	{
		CCamera_Cinematic::Camera_Cinematic_DESC            Camera_Desc{};
		Camera_Desc.fSpeedPerSec = 5.f;
		Camera_Desc.fRotationPerSec = XMConvertToRadians(90.0f);
		Camera_Desc.fFovy = XMConvertToRadians(60.0f);
		Camera_Desc.fNear = 0.3f;
		Camera_Desc.fFar = 500.f;
		Camera_Desc.pCameraKey = CAMERA_CINEMATIC;
		Camera_Desc.iPriority = 52;
		Camera_Desc.bEnableTransitionLerp = false;
		Camera_Desc.bEnableLookLerp = false;
		Camera_Desc.bEnableFollowLerp = false;
		Camera_Desc.vTransitionTime.y = 1.f;
		Camera_Desc.pFollowTarget = { nullptr };
		Camera_Desc.pLookTarget = { nullptr };

		CCamera_Cinematic* pCamera = { nullptr };
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCamera_Cinematic>(g_iStaticLevel, NEXT_LEVEL, LAYER_CAMERA, &Camera_Desc, nullptr, &pCamera))) {
			return E_FAIL;
		}
		m_pGameInstance->Add_Camera(NEXT_LEVEL, pCamera, CAMERA_CINEMATIC);
	}
	if (FAILED(m_pGameInstance->Bind_Camera(NEXT_LEVEL, CAMERA_SHOULDER, true))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Sound()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Markers()
{

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
	CPlayer::PLAYERDESC playerDesc = {};
	playerDesc.vPos = _float4(-21.f, 0.f, -14.f, 1.f);
	playerDesc.vRotQ = _float4(0.f, 0.f, 0.f, 1.f);
	playerDesc.pBroomRaceManager = m_pBroomRaceManager;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CPlayer>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &playerDesc))) {
		return E_FAIL;

	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_RacerAI(const _wstring& strLayerTag)
{
	CBroomRacerAI::RacerDesc Desc = {};
	for (_uint i = 1; i < 4; i++)
	{
		Desc.pRacerManager = m_pBroomRaceManager;
		Desc.iIndex = i;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroomRacerAI>(g_iStaticLevel, NEXT_LEVEL, LAYER_RACERAI, &Desc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Item(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CThestralCarriage>(g_iStaticLevel, NEXT_LEVEL, strLayerTag))) {
		return E_FAIL;
	}
	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_ReparoObject(const _wstring& strLayerTag)
{
	for (_uint i = 0; i < 1; ++i) {
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CReparoObject>(g_iStaticLevel, NEXT_LEVEL, strLayerTag))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_SkyBox(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSkyBox>(g_iStaticLevel, NEXT_LEVEL, LAYER_SKYBOX))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster()
{
	_bool isLoad_Monster = { true };
#ifdef _DEBUG
#ifdef gimch

#endif // gimch
#ifdef 진우
	isLoad_Monster = true;
#endif // 
#ifdef 기무리
	isLoad_Monster = false;
#endif // 
#ifdef 나

#endif // 
#ifdef Bin
	isLoad_Monster = true;
#endif // Bin
#endif // _DEBUG
	if (true == isLoad_Monster) {
		for (_uint i = 0; i < 1; ++i)
		{
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CGoblin>(g_iStaticLevel, NEXT_LEVEL, LAYER_MONSTER))) {
				return E_FAIL;
			}
		}

		for (_uint i = 0; i < 1; ++i)
		{
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CGoblin_Mage>(g_iStaticLevel, NEXT_LEVEL, LAYER_MONSTER))) {
				return E_FAIL;
			}
		}

		for (_uint i = 0; i < 1; ++i)
		{
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CGoblin_Assassin>(g_iStaticLevel, NEXT_LEVEL, LAYER_MONSTER))) {
				return E_FAIL;
			}
		}


		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTroll>(g_iStaticLevel, NEXT_LEVEL, LAYER_MONSTER))) {
			return E_FAIL;
		}

	/*	CRanrok::RANROKDESC RanrokDesc = {};
		RanrokDesc.vPos = _float4(-44.704f, 6.860f, 16.071f, 1.f);
		RanrokDesc.vRotQ = _float4(0.f, 0.f, 0.f, 1.f);
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRanrok>(g_iStaticLevel, NEXT_LEVEL, LAYER_MONSTER,&RanrokDesc))) {
			return E_FAIL;
		}*/



	}
#if 진우
#else

#endif 

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Manager(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroomRaceManager>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, nullptr, nullptr, &m_pBroomRaceManager))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Reday_Layer_EffectPool()
{
	//플레이어보다 먼저 생성해야함!
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEffectPool>(g_iStaticLevel, NEXT_LEVEL, LAYER_EFFECTPOOL))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Bind_InstanceProp(map<_string, class CLand*>* Lands, CInstancedProp* pInstnace, const _char* LandTag)
{
	auto iter = Lands->find(LandTag);

	if (iter == Lands->end())
		return E_FAIL;

	iter->second->Add_InstanceProp(pInstnace);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Duelist()
{

	CHuman_Duelist::DUELISTDESC DuelistDesc = {};
	DuelistDesc.vPos = _float4(1007.23f, 1.775f, 1015.f, 1.f);
	DuelistDesc.vRotQ = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CHuman_Duelist>(g_iStaticLevel, NEXT_LEVEL, LAYER_DUELIST, &DuelistDesc))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Npc()
{
	_bool isLoad_NPC = { true };
	_bool isLoad_RandomNPC = { true };
	_bool isRandomPosition = { false };
#ifdef _DEBUG
#ifdef gimch
	isLoad_NPC = true;
	isLoad_RandomNPC = true;
	isRandomPosition = false;
#endif // gimch
#ifdef 진우
	isLoad_RandomNPC = true;
	isLoad_NPC = true;
#endif // 
#ifdef 기무리
	isLoad_NPC = false;
	isLoad_RandomNPC = false;
#endif // 
#ifdef 나
	isLoad_RandomNPC = true;
#endif // 
#ifdef Bin
	isLoad_NPC = true;
	isLoad_RandomNPC = true;
#endif // Bin
#endif // _DEBUG

	if (true == isLoad_NPC)
	{
		{
			CNPC_Ollivander::NPCDESC NPCDesc{};
			NPCDesc.vPos = _float4(40.f, 4.f, 68.9f, 1.f);
			NPCDesc.vRotQ = _float4(0.f, 0.f, 0.f, 1.f);
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CNPC_Ollivander>(g_iStaticLevel, NEXT_LEVEL, LAYER_NPC, &NPCDesc))) {
				return E_FAIL;
			}
		}
		{
			CNPC_EleazarFig::NPCDESC NPCDesc{};
			NPCDesc.vPos = _float4(101.f, 14.f, 100.f, 1.f);
			NPCDesc.vRotQ = _float4(0.f, 0.f, 0.f, 1.f);
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CNPC_EleazarFig>(g_iStaticLevel, NEXT_LEVEL, LAYER_NPC, &NPCDesc))) {
				return E_FAIL;
			}
		}
		{
			CElf::ELFDESC ElfDesc{};
			ElfDesc.vPos = _float4(-84.20f, -30.98f, -60.13f, 1.f);
			ElfDesc.vRotQ = _float4(0.f, 0.f, 0.f, 1.f);

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CElf>(g_iStaticLevel, NEXT_LEVEL, LAYER_NPC, &ElfDesc))) {
				return E_FAIL;
			}
		}
	}

	if (true == isLoad_RandomNPC)
	{
		if(isRandomPosition)
		{
			for (_uint i = 0; i < 10; i++)
			{
				CRandomNpc::NPCDESC NPCDesc{};
				_float X = m_pGameInstance->Real_Random_Float(22.f, 29.f);
				_float Z = m_pGameInstance->Real_Random_Float(20.f, 29.f);
				NPCDesc.vPos = _float4(X, 1.f, Z, 1.f);
				NPCDesc.vRotQ = _float4(0.f, 0.f, 0.f, 1.f);
				NPCDesc.iIndex = i;
				if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRandomNpc>(g_iStaticLevel, NEXT_LEVEL, LAYER_NPC, &NPCDesc))) {
					return E_FAIL;
				}
			}
		}
		else
			m_pInfoInstance->Load_Npc();
	}
	return S_OK;
}

void CLevel_GamePlay::ResetLevel_Environment()
{
	Ready_Volumetric();
	_float4 vDiffuse;
	_float4 vAmbient;
	_float4 vSpecular;
	if (m_isDay)
	{
		vDiffuse = _float4(0.6529f, 0.6157f, 0.7843f, 1.0f);
		vAmbient = _float4(0.6275f, 0.6275f, 0.6275f, 0.0314f);
		vSpecular = _float4(0.05f, 0.05f, 0.05f, 0.05f);
	}
	else
	{
		vDiffuse = _float4(0.0471f, 0.0745f, 0.1294f, 0.2549f);
		vAmbient = _float4(0.1686f, 0.1765f, 0.1373f, 0.0f);
		vSpecular = _float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	m_pLight->Get_Component<CLight>()->Set_Color(vDiffuse, vAmbient, vSpecular);
}

pair<CLevel*, function<void()>> CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, void* pArg)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		SAFE_RELEASE(pInstance);
	}

	return { pInstance, [=]() { pInstance->Ready_Layer_Camera(); pInstance->Ready_Layer_Sound();
	pInstance->m_pInfoInstance->Load_CutScenes(); } };
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);
}
