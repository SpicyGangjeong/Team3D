#include "pch.h"
#include "Level_MapViewer.h"
#include "GameInstance.h"

#include "Level_Loading.h"
#include "Camera_Debug.h"
#include "Terrain.h"
#include "MapObject_Manager.h"
#include "BuildingContainer.h"
#include "DummySkyBox.h"
#include "MainLight.h"
#include "Land.h"
#include "InstancedProp.h"
#include "Player.h"
#include "Ranrok.h"
#include "Unified.h"
#include "MapElement_Lake.h"
#include "MapElement_Door.h"
#include "MapElement_Chest.h"
#include "DummyDecal.h"
#include "EffectPool.h"
#include "InstancedProp_Light.h"

CLevel_MapViewer::CLevel_MapViewer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{

}

void CLevel_MapViewer::Update(_float fTimeDelta)
{

}

HRESULT CLevel_MapViewer::Render()
{
	SetWindowText(g_hWnd, TEXT("MapEditor레벨입니다"));
	GUI::ShowDemoWindow();
	return S_OK;
}

HRESULT CLevel_MapViewer::Initialize()
{
	if (FAILED(Ready_Layer_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Background(TEXT("Layer_SkyBox")))) {
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEffectPool>(g_iStaticLevel, NEXT_LEVEL, TEXT("Z_Layer_EffectPool")))) //플레이어보다 먼저 생성해야함!
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Player_And_Ranrok())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain")))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Land(TEXT("Layer_Land")))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_InstanceProp(TEXT("Layer_InstanceProp")))) {
		return E_FAIL;
	}

	//if (FAILED(Ready_Layer_BuildingContainer(TEXT("Layer_Building")))) {
	//	return E_FAIL;
	//}

	if (FAILED(Ready_Layer_Unified(TEXT("Layer_Unified")))) {
		return E_FAIL;
	}

	/*if (FAILED(Ready_Layer_Door(TEXT("Layer_Door")))) {
		return E_FAIL;
	}*/

	/*if (FAILED(Ready_Layer_Chest(TEXT("Layer_Chest")))) {
		return E_FAIL;
	}*/

	if (FAILED(Ready_Layer_MapObjectManager(LAYER_MAPOBJECTMANAGER))) {
		return E_FAIL;
	}

	//m_pGameInstance->Setting_Volumetirc(1.812f, 0.003f, 0.56f, 1.f, 0.031f);
	m_pGameInstance->Setting_Volumetirc(0.f, 0.003f, 0.56f, 1.f, 0.031f);

	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_Light()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMainLight>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_LIGHT)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CCamera_Debug::CAMERA_DEBUG_DESC            Camera_Desc{};
	Camera_Desc.fFovy = XMConvertToRadians(60.0f);
	Camera_Desc.fNear = 0.1f;
	Camera_Desc.fFar = 600.f;
	Camera_Desc.vEye = _float3(0.f, 10.f, -10.f);
	Camera_Desc.vAt = _float3(0.f, 0.f, 0.f);
	Camera_Desc.fSpeedPerSec = 5.f;
	Camera_Desc.pCameraKey = CAMERA_DEBUG;
	Camera_Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Camera_Desc.fMouseSensor = 0.1f;
	Camera_Desc.iPriority = 70;
	Camera_Desc.pFollowTarget = { nullptr };
	Camera_Desc.pLookTarget = { nullptr };

	CCamera_Debug* pCamera = { nullptr };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCamera_Debug>(g_iStaticLevel, NEXT_LEVEL, LAYER_CAMERA, &Camera_Desc, nullptr, &pCamera))) {
		return E_FAIL;
	}
	m_pGameInstance->Add_Camera(g_iStaticLevel, pCamera, CAMERA_DEBUG);
	if (FAILED(m_pGameInstance->Bind_Camera(g_iStaticLevel, CAMERA_DEBUG, true))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_Background(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummySkyBox>(g_iStaticLevel, NEXT_LEVEL, strLayerTag)))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_Terrain(const _wstring& strLayerTag)
{
	CTerrain::TERRAIN_DESC Desc = {};

	///* Hogsmeade */
	Desc.isEdit = false;
	Desc.iAlphaSizeX = 2048; 
	Desc.iAlphaSizeY = 2048;
	Desc.vPosition = _float3(-194, 18.5f, -153.f);
	Desc.strAlphaMapTag = "Hogsmeade_AlphaMap.bin";
	Desc.strHeightMapTag = "Hogsmeade_HeightMap.bin";
	Desc.strBufferTag = TEXT("Prototype_Component_VIBuffer_Terrain_Hogsmeade");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTerrain>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* Hogwart */
	Desc.isEdit = false;
	Desc.iAlphaSizeX = 2048;
	Desc.iAlphaSizeY = 2560;
	Desc.strAlphaMapTag = "Hogwart_AlphaMap.bin";
	Desc.strHeightMapTag = "Hogwart_HeightMap.bin";
	Desc.vPosition = _float3(-451.f, 18.5f, -791.f);
	Desc.strBufferTag = TEXT("Prototype_Component_VIBuffer_Terrain_Hogwart");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTerrain>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* North_Hogwart */
	//Desc.isEdit = false;
	//Desc.iAlphaSizeX = 1536;
	//Desc.iAlphaSizeY = 1024;
	//Desc.vPosition = _float3(-577.f, 18.5f, -153.f);
	//Desc.strBufferTag = TEXT("Prototype_Component_VIBuffer_Terrain_North_Hogwart");
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTerrain>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
	//	return E_FAIL;

	/* South_Hogwart */
	/*Desc.isEdit = true;
	Desc.iAlphaSizeX = 2048;
	Desc.iAlphaSizeY = 1, 536;
	Desc.vPosition = _float3(61.f, 18.5f, -535.f);
	Desc.strBufferTag = TEXT("Prototype_Component_VIBuffer_Terrain_South_Hogwart");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTerrain>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;*/

	CMapElement_Lake::MAPOBJECT_LAKE_DESC Lake_Desc = {};

	vector<_wstring>		ModelPrototypeTags;
	vector<_wstring>		ShallowModelPrototypeTags;
	ModelPrototypeTags.push_back(TEXT("Prototype_Component_Hogwart_Lake"));
	ShallowModelPrototypeTags.push_back(TEXT("Prototype_Component_Hogwart_LakeSurFace"));
	Lake_Desc.bEdit = false;
	Lake_Desc.iMaxLodLevel = 0;
	Lake_Desc.iRenderType = 4;
	Lake_Desc.vPosition = _float3(-89.2f, -61.6f, -657.8f);
	Lake_Desc.vRotation = _float3(0.f, 0.f, 0.f);
	Lake_Desc.vScale = _float3(1.3f, 1.f, 1.3f);
	Lake_Desc.fTimeSpeed = _float(0.02f);
	Lake_Desc.fRefractionStrength = 0.08f;
	Lake_Desc.fRefractionPow = 1.5f;
	Lake_Desc.fUVValue1 = 10.f;
	Lake_Desc.fUVValue2 = 20.f;
	Lake_Desc.fUVValue3 = 20.f;
	Lake_Desc.vUVSpeed = _float2(0.1f, 0.f);
	Lake_Desc.vLargeUVSpeed = _float2(0.3f, 0.1f);
	Lake_Desc.vSubUVSpeed3 = _float2(10.f, 0.f);
	Lake_Desc.vRefractionColor = _float4(0.35f, 0.35f, 0.35f, 1.f);
	Lake_Desc.vSurfaceColor = _float4(0.25f, 0.f, 1.f, 1.f);
	Lake_Desc.ModelPrototypeTags = ModelPrototypeTags;
	Lake_Desc.ShallowModelPrototypeTags = ShallowModelPrototypeTags;
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Lake>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Lake_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_Land(const _wstring& strLayerTag)
{
	CLand::LAND_DESC Land_Desc = {};

	/* South_Hogwart_Land */
	Land_Desc.bEdit = false;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(290.5f, 59.5f, -347.f);
	Land_Desc.vScale = _float3(1.f, 1.25f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_South_Hogwart_Land_LOD1";
	Land_Desc.strAlphaMapTag = "Land_HN_BC_AlphaMap.bin";

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGWART, &Land_Desc)))
		return E_FAIL;

	/* North_Hogwart_Land */
	Land_Desc.bEdit = false;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(-370.f, -37.5f, -21.4f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_North_Hogwart_Land_LOD1";
	Land_Desc.strAlphaMapTag = "Land_HN_AV_AlphaMap.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGWART, &Land_Desc)))
		return E_FAIL;

	/* North_Hogwart2_Land */
	Land_Desc.bEdit = false;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(-378.f, -17.5f, 285.6f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_North_Hogwart2_Land_LOD1";
	Land_Desc.strAlphaMapTag = "Land_HN_AU_AlphaMap.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGWART, &Land_Desc)))
		return E_FAIL;

	/* West_Hogwart_Land */
	Land_Desc.bEdit = false;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(-653.f, 20.f, -327.4f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_West_Hogwart_Land_LOD1";
	Land_Desc.strAlphaMapTag = "Land_HN_AZ_AlphaMap.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGWART, &Land_Desc)))
		return E_FAIL;

	/* East_Hogsmeade_Land */
	Land_Desc.bEdit = false;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(436.f, 55.f, 60.3f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_East_Hogsmeade_Land_LOD1";
	Land_Desc.strAlphaMapTag = "Land_HN_AY_AlphaMap.bin";

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Land_Desc)))
		return E_FAIL;

	/* HN_BA */
	Land_Desc.bEdit = false;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(-606.f, 70.f, -647.7f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BA";
	Land_Desc.strAlphaMapTag = "Land_HN_BA_AlphaMap.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGWART, &Land_Desc)))
		return E_FAIL;

	/* HN_BD */
	Land_Desc.bEdit = false;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(201.4f, -22.9f, -650.1f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BD";
	Land_Desc.strAlphaMapTag = "Land_HN_BD_AlphaMap.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Land_Desc)))
		return E_FAIL;

	/* HN_BG */
	Land_Desc.bEdit = false;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(-377.8f, 5.f, -1038.49f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BG";
	Land_Desc.strAlphaMapTag = "Land_HN_BG_AlphaMap.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Land_Desc)))
		return E_FAIL;

	/* HN_BH */
	Land_Desc.bEdit = false;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(80.1f, -63.f, -908.3f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BH";
	Land_Desc.strAlphaMapTag = "Land_HN_BH_AlphaMap.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Land_Desc)))
		return E_FAIL;

	/* HN_BE */
	Land_Desc.bEdit = false;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(431.2f, 100.1f, -658.3f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BE";
	Land_Desc.strAlphaMapTag = "Land_HN_BE_AlphaMap.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Land_Desc)))
		return E_FAIL;

	/* HN_BF */
	Land_Desc.bEdit = false;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(-688.9f, 170.7f, -1066.f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BF";
	Land_Desc.strAlphaMapTag = "Land_HN_BF_AlphaMap.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Land_Desc)))
		return E_FAIL;

	/* HN_BI */
	Land_Desc.bEdit = false;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(82.7f, -61.f, -1141.6f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BI";
	Land_Desc.strAlphaMapTag = "Land_HN_BI_AlphaMap.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Land_Desc)))
		return E_FAIL;

	/* HN_BJ */
	Land_Desc.bEdit = false;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(367.1f, 44.8f, -1016.f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_HN_BJ";
	Land_Desc.strAlphaMapTag = "Land_HN_BJ_AlphaMap.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Land_Desc)))
		return E_FAIL;

	/* TU_BB */
	Land_Desc.bEdit = true;
	Land_Desc.isLoadAlphaMap = true;
	Land_Desc.vPosition = _float3(-829.5f, 58.3f, 112.9f);
	Land_Desc.vScale = _float3(1.f, 1.2f, 1.f);
	Land_Desc.strModelComTag = L"Prototype_Component_Land_TU_BB";
	Land_Desc.strAlphaMapTag = "Land_TU_BB_AlphaMap.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, LAYER_HOGSMEADE, &Land_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_InstanceProp(const _wstring& strLayerTag)
{
	CInstancedProp::INSTANCE_PROP_DESC Desc = {};

	/* Oak_Tree */
	Desc.bEditMode = false;
	Desc.isShake = true;
	Desc.iShaderPassIndex = 0;
	Desc.vRadius = _float2(0.015f, 0.03f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_OakTree_MedA";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/OakTree_MedA.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.isShake = false;
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/OakTree_MedA_HN_BB.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* BearBerry */
	Desc.bEditMode = false;
	Desc.isShake = true;
	Desc.vRadius = _float2(0.015f, 0.02f);
	Desc.vSpeed = _float2(0.6f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_BearBerry_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/BearBerry_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* SM_HM_OwlPost_Window_A */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_HM_OwlPost_Window_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/OwlPost_Window.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	///* WA_Rectangle_Double_A */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WA_Rectangle_Double_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Rectangle_Double_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	///* WC_Retangle_Double_A */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WC_Retangle_Double_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/WC_Retangle_Double_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	///* WA_Square_Double_C */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WA_Square_Double_C";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/WA_Square_Double_C.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* Quid_Window_A */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Quid_Window_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Quid_Window_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* Ollivanders_Box_Window */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Ollivanders_Box_Window";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Ollivanders_Box.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* WC_L_DoubleS_A */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WC_L_DoubleS_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/WC_L_DoubleS_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* WC_Round_Double_A */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WC_Round_Double_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/WC_Round_Double_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* SM_HM_Door1a */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door1a";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Door1a.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* SM_HM_Door1a */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door2b";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Door2b.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* OakTree_TallA */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.vRadius = _float2(0.01f, 0.01f);
	Desc.vSpeed = _float2(0.01f, 0.021f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_OakTree_TallA";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/OakTree_TallA.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* Shrub_B */
	Desc.bEditMode = false;
	Desc.isShake = true;
	Desc.vRadius = _float2(0.015f, 0.04f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Shrub_B";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Shrub_B.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* BogMyrtle_A */
	Desc.bEditMode = false;
	Desc.isShake = true;
	Desc.vRadius = _float2(0.015f, 0.04f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_BogMyrtle_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/BogMyrtle_A.bin";
	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.isShake = false;
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/BogMyrtle_A_HN_BB.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.isShake = false;
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/BogMyrtle_A_Dungeon.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* Dogwood_B */
	Desc.bEditMode = false;
	Desc.isShake = true;
	Desc.vRadius = _float2(0.015f, 0.02f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Dogwood_B";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Dogwood_B.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* ScotsPine_LargeA */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.vRadius = _float2(0.f, 0.f);
	Desc.vSpeed = _float2(0.f, 0.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_ScotsPine_LargeA";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/ScotsPine_LargeA.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/ScotsPine_LargeA_HN_BB.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/ScotsPine_LargeA_HN_BB_2.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/ScotsPine_LargeA_HN_AZ.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;
	
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/ScotsPine_LargeA_HN_AV.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;


	/* StratifiedCliff_A1 */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.vRadius = _float2(0.f, 0.f);
	Desc.vSpeed = _float2(0.f, 0.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_StratifiedCliff_A1";

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/CliffwallA_HN_BC.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/CliffwallA_HN_BB.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/CliffwallA_HN_AZ.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/CliffwallA_HN_AV.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/CliffwallA_HN_AW.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;


	/* StratifiedRock_B */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.vRadius = _float2(0.015f, 0.02f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_StratifiedRock_B";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/StratifiedRock_B_HN_BB.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/StratifiedRock_B_HN_AZ.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/StratifiedRock_B_HN_AW.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* StratifiedRock_D_B */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.vRadius = _float2(0.015f, 0.02f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_StratifiedRock_D_B";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/StratifiedRock_D_B_HN_BB.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/StratifiedRock_D_B_BACK.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* Stone_FrontSteps */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.vRadius = _float2(0.015f, 0.02f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Stone_FrontSteps";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/FrontSteps_A_HN_AW.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	/* StoneKit_A */
	Desc.bEditMode = false;
	Desc.isShake = false;
	Desc.vRadius = _float2(0.015f, 0.02f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_StoneKit_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/StoneKit_A_HW.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.bEditMode = true;
	Desc.isShake = false;
	Desc.iShaderPassIndex = 4;
	Desc.vRadius = _float2(0.f, 0.f);
	Desc.vSpeed = _float2(0.f, 0.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SK_BRR_RouteMarker";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/RouteMarker.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	CInstancedProp_Light::INSTANCE_PROP_LIGHT_DESC LightDesc = {};

	/* LightPost */
	LightDesc.bEditMode = false;
	LightDesc.isShake = false;
	LightDesc.iGlassMeshIndex = 0;
	LightDesc.vRadius = _float2(0.f, 0.f);
	LightDesc.vSpeed = _float2(0.f, 0.f);
	LightDesc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_LightPost";
	LightDesc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/LightPost.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp_Light>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &LightDesc)))
		return E_FAIL;

	/* LightPost_Floating */
	LightDesc.bEditMode = false;
	LightDesc.isShake = false;
	LightDesc.iGlassMeshIndex = 1;
	LightDesc.vRadius = _float2(0.f, 0.f);
	LightDesc.vSpeed = _float2(0.f, 0.f);
	LightDesc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Light_Post_Floating";
	LightDesc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Light_Post_Floating.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp_Light>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &LightDesc)))
		return E_FAIL;

	///* LightFixture_Base_D */
	//LightDesc.bEditMode = false;
	//LightDesc.isShake = false;
	//LightDesc.iGlassMeshIndex = 1;
	//LightDesc.vRadius = _float2(0.f, 0.f);
	//LightDesc.vSpeed = _float2(0.f, 0.f);
	//LightDesc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_LightFixture_Base_D";
	//LightDesc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/SM_HW_LightFixture_Base_D.bin";
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp_Light>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &LightDesc)))
	//	return E_FAIL;



	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_BuildingContainer(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBuildingContainer>(g_iStaticLevel, NEXT_LEVEL, strLayerTag)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_Unified(const _wstring& strLayerTag)
{
	CUnified::UNIFIED_DESC Desc = {};

	/* Hogwart Unified*/
	Desc.fLodSwitchDistnace = 500.f;
	Desc.vUnifiedCenterPos = _float4(-291.2f, 17.2f, -474.7f, 1.f);
	Desc.vPosition = _float3(-291.2f, 17.2f, -474.7f);
	Desc.vRotation = _float3(0.f, 0.f, 0.f);
	Desc.vScale = _float3(1.f ,1.f, 1.f);
	Desc.srtLayerTag = LAYER_HOGWART;
	Desc.srtModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HW_HogwartsShell_B"));

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CUnified>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
		return E_FAIL;

	Desc.srtModelPrototypeTags.clear();
	/* Hogsmeade Unified*/
	//Desc.fLodSwitchDistnace = 550.f;
	//Desc.vUnifiedCenterPos = _float4(62.f, 10.f, 93.f, 1.f);
	//Desc.vPosition = _float3(62.f, 5.6f, 93.f);
	//Desc.vRotation = _float3(0.f, 0.f, 0.f);
	//Desc.vScale = _float3(1.f, 1.f, 1.f);
	//Desc.srtLayerTag = LAYER_HOGSMEADE;
	//Desc.srtModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HM_Unified_LOD1_Baked_0"));
	//Desc.srtModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HM_Unified_LOD1_Baked_1"));
	//Desc.srtModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HM_Unified_LOD1_Baked_2"));
	//Desc.srtModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HM_Unified_LOD1_Baked_3"));
	//Desc.srtModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HM_Unified_Streets_LOD1_Merged_0"));

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CUnified>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Desc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_Door(const _wstring& strLayerTag)
{
	vector<_wstring> ModelPrototypeTags;

	ModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HM_Ollivanders_Front_Door"));
	ModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HM_Ollivanders_Front_Door_Lod1"));
	ModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_HM_Ollivanders_Front_Door_Lod2"));

	CMapElement_Door::ELEMENT_DOOR_DESC Door_Desc = {};
	Door_Desc.isEdit = true;
	Door_Desc.iSubKind = {};
	Door_Desc.vBoxLocalPosition = {};
	Door_Desc.vBoxSize = {};
	Door_Desc.iMaxLodLevel = 2;
	Door_Desc.ModelPrototypeTags = ModelPrototypeTags;
	Door_Desc.vPosition = _float3(0.f, 0.f, 0.f);
	Door_Desc.vRotation = _float3(0.f, 0.f, 0.f);
	Door_Desc.vScale = _float3(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Door>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Door_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_Chest(const _wstring& strLayerTag)
{
	vector<_wstring> ModelPrototypeTags = {};
	vector<_wstring> ModelPrototypeTagsLid = {};

	ModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_Chest_RewardContainer_C"));
	ModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_Chest_RewardContainer_C_Lod1"));
	ModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_Chest_RewardContainer_C_Lod2"));
	ModelPrototypeTags.push_back(TEXT("Prototype_GameObject_SM_Chest_RewardContainer_C_Lod3"));

	ModelPrototypeTagsLid.push_back(TEXT("Prototype_GameObject_SM_Chest_RewardContainer_C_Lid"));
	ModelPrototypeTagsLid.push_back(TEXT("Prototype_GameObject_SM_Chest_RewardContainer_C_Lid_Lod1"));
	ModelPrototypeTagsLid.push_back(TEXT("Prototype_GameObject_SM_Chest_RewardContainer_C_Lid_Lod2"));


	CMapElement_Chest::ELEMENT_CHEST_DESC Chest_Desc = {};
	Chest_Desc.iMaxLodLevel = 3;
	Chest_Desc.iSubKind = ENUM_CLASS(PXOBJECT::BOX);
	Chest_Desc.ModelPrototypeTags = ModelPrototypeTags;
	Chest_Desc.vPosition = _float3(0.f, 0.f, 0.f);
	Chest_Desc.vRotation = _float3(0.f, 0.f, 0.f);
	Chest_Desc.vScale = _float3(1.f, 1.f, 1.f);
	Chest_Desc.vBoxSize = _float3(0.3f, 0.25f, 0.27f);
	Chest_Desc.vBoxLocalPosition = _float3(-0.01f, 0.3f, -0.25f);

	Chest_Desc.iMaxLodLevel_Lid = 2;
	Chest_Desc.ModelPrototypeTags_Lid= ModelPrototypeTagsLid;
	Chest_Desc.vLid_Offset = _float3(0.f, 0.5f, 0.f);
	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMapElement_Chest>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &Chest_Desc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_MapObjectManager(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMapObject_Manager>(g_iStaticLevel, NEXT_LEVEL, strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_Player_And_Ranrok()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CPlayer>(g_iStaticLevel, NEXT_LEVEL, LAYER_PLAYER))) {
		return E_FAIL;
	}


	/*if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRanrok>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Monster"))))
		return E_FAIL;*/

	return S_OK;
}

CLevel_MapViewer* CLevel_MapViewer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_MapViewer* pInstance = new CLevel_MapViewer(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_MapViewer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}



void CLevel_MapViewer::Free()
{
	__super::Free();
}
