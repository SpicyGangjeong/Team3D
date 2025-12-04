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
#include "Dummy_PhysXWall.h"
#include "Dummy_PhysXPlayable.h"
#include "Terrain.h"
#include "EffectPool.h"
#include "InstancedProp.h"

#pragma region ACTOR
#include "Player.h"
#include "Troll.h"
#include "Goblin.h"
#pragma endregion


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{
	m_pInfoInstance = CInfoInstance::GetInstance();
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CLevel_GamePlay::Initialize(void* pArg)
{
	if (FAILED(Ready_Lights())) {
		return E_FAIL;
	}
	if (FAILED(Ready_Background())) {
		return E_FAIL;
	}
	if (FAILED(Ready_Markers())) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_UI(LAYER_UI))) {
		return E_FAIL;
	}
	//플레이어보다 먼저 생성해야함!
	if (FAILED(Reday_Layer_EffectPool())) {	
		return E_FAIL;
	}
	// 이것도 플레이어보다 먼저 생성해야함!
	if (FAILED(Ready_Layer_Item(LAYER_ITEM))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_SkyBox(TEXT("Layer_SkyBox")))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Player(LAYER_PLAYER))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Monster())) {
		return E_FAIL;
	}


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
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLight_Main>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_LIGHT))){
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Background()
{
	/* Terrain */
	CTerrain::TERRAIN_DESC Terrain_Desc = {};
	Terrain_Desc.vPosition = _float3(-194, 18.5f, -153.f);
	Terrain_Desc.strDiffuseTextureTag = TEXT("Terrain_Diffuse");
	Terrain_Desc.strNormalTextureTag = TEXT("Terrain_Normal");
	Terrain_Desc.strMROTextureTag = TEXT("Terrain_MRO");
	Terrain_Desc.strAlphaMapTextureTag = TEXT("HogsmeadeAlphaMap");
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTerrain>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_BACKGROUND, &Terrain_Desc))) {
		return E_FAIL;
	}

	/* Map Containters */
	/* 테스트용 맵 */

	//CInfoInstance::GetInstance()->Load_MapObjects("ClientTest");
	/* 전체 맵 */
	CInfoInstance::GetInstance()->Load_MapObjects("Map1129");

	/* 조명 오브젝트 */
	CInfoInstance::GetInstance()->Load_LightElements("LightElement");

	
	CInstancedProp::INSTANCE_PROP_DESC Desc = {};

	/* InstanceProp Oak_Tree*/
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_OakTree_MedA";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/OakTree_MedA.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc))){
		return E_FAIL;
	}

	/* BearBerry */
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_BearBerry_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/BearBerry_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc))){
		return E_FAIL;
	}

	/* SM_HM_OwlPost_Window_A */
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_HM_OwlPost_Window_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/OwlPost_Window.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	///* WA_Rectangle_Double_A */
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WA_Rectangle_Double_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Rectangle_Double_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	///* WC_Retangle_Double_A */
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WC_Retangle_Double_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/WC_Retangle_Double_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	///* WA_Square_Double_C */
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WA_Square_Double_C";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/WA_Square_Double_C.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	/* Quid_Window_A */
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Quid_Window_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Quid_Window_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	/* Ollivanders_Box_Window */
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_Ollivanders_Box_Window";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Ollivanders_Box.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	/* WC_L_DoubleS_A */
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WC_L_DoubleS_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/WC_L_DoubleS_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	/* WC_Round_Double_A */
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_WC_Round_Double_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/WC_Round_Double_A.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	/* SM_HM_Door1a */
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door1a";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Door1a.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	/* SM_HM_Door1a */
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_SM_HM_Door2b";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/Door2b.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
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
	CCamera_Debug::CAMERA_DEBUG_DESC            Camera_Desc{};
	Camera_Desc.fFovy = XMConvertToRadians(60.0f);
	Camera_Desc.fNear = 0.1f;
	Camera_Desc.fFar = 200.f;
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
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCamera_Debug>(g_iStaticLevel, NEXT_LEVEL, LAYER_CAMERA, &Camera_Desc, nullptr, &pCamera))){
		return E_FAIL;
	}

	m_pGameInstance->Add_Camera(g_iStaticLevel, pCamera, CAMERA_DEBUG);
	if (FAILED(m_pGameInstance->Bind_Camera(g_iStaticLevel, CAMERA_DEBUG, true))) {
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
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CPlayer>(g_iStaticLevel, NEXT_LEVEL, strLayerTag))){
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Item(const _wstring& strLayerTag)
{

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom>(g_iStaticLevel, NEXT_LEVEL, LAYER_ITEM, nullptr, nullptr))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_SkyBox(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSkyBox>(g_iStaticLevel, NEXT_LEVEL, LAYER_SKYBOX))){
		return E_FAIL;
	}

	CDummy_PhysXWall::PHYSXDUMMY_DESC Desc{};
	Desc.vPos = { 0.f, 0.f, 0.f };
	Desc.vRotRPY = { 0.f, m_pGameInstance->Random_Float(0.f, XM_2PI), 0.f };
	Desc.iSubKind = ENUM_CLASS(PXOBJECT::WALL);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXWall>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster()
{
	for (_uint i = 0; i < 2; ++i)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CGoblin>(g_iStaticLevel, NEXT_LEVEL, LAYER_MONSTER,&i))) {
			return E_FAIL;
		}
	}
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTroll>(g_iStaticLevel, NEXT_LEVEL, LAYER_MONSTER))) {
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

pair<CLevel*, function<void()>> CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, void* pArg)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		SAFE_RELEASE(pInstance);
	}

	return { pInstance, [pInstance]() { pInstance->Ready_Layer_Camera(); pInstance->Ready_Layer_Sound(); } };
}

void CLevel_GamePlay::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);
}
