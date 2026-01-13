#include "pch.h"
#include "Level_Field.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "InfoInstance.h"
#include "Player.h"
#include "EffectPool.h"
#include "Light_Main.h"
#include "Layer.h"
#include "Camera_Cinematic.h"
#include "TimeSocket.h"
#include "Camera_Debug.h"
#include "Terrain.h"
#include "SkyBox.h"
#include "Ranrok.h"
#include "UI_Manager.h"
#include "InstancedProp.h"


CLevel_Field::CLevel_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{
	m_pInfoInstance = CInfoInstance::GetInstance();
	SAFE_ADDREF(m_pInfoInstance);
}

HRESULT CLevel_Field::Initialize(void* pArg)
{
	if (FAILED(Ready_Lights())) {
		return E_FAIL;
	}
	if (FAILED(Ready_Volumetric())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_SkyBox())) {
		return E_FAIL;
	}
	if (FAILED(Ready_Background())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Camera())) {
		return E_FAIL;
	}

	if (FAILED(Reday_Layer_EffectPool())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Player(LAYER_PLAYER))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Markers())) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_UI(LAYER_UI))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Effect(LAYER_EFFECT))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Monster())) {
		return E_FAIL;
	}
	ResetLevel_Environment();
	

	m_bLevel = true;
	m_pInfoInstance->Event_CallBack(TEXT("UIManagerFadeIn"));

	return S_OK;
}

HRESULT CLevel_Field::Initialize()
{
	assert(false);
	return E_FAIL;
}

void CLevel_Field::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

	if (m_pGameInstance->Key_Up(DIK_F1))
	{
		//m_pGameInstance->Set_LevelToChange();
	}

	m_pInfoInstance->Update(fTimeDelta);

	if (true == m_pGameInstance->Check_LevelShouldChange()) {
		m_pInfoInstance->Change_Level();
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::RESTART)))) {
			return;
		}
	}

	if (m_pGameInstance->Key_Up(DIK_7))
	{
		_string strCutSceneName = "RanrokIntro";
		m_pInfoInstance->Active_Event(strCutSceneName);
	}

	if (m_bLevel != m_bCurrentLevel)
	{
		m_bCurrentLevel = m_bLevel;
		UI_STATE eState = UI_STATE::GAMEPLAYER;
		m_pInfoInstance->Event_CallBack(TEXT("Canvas_Change"), &eState);
	}
}

HRESULT CLevel_Field::Render()
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
		TEXT("필드레벨입니다 | Frame: %.3f ms | Avg: %.3f ms | FPS: %.1f"),
		fCurrentFrameTimeMilliseconds,
		fAverageFrameTimeMilliseconds,
		fCurrentFps
	);

	SetWindowText(g_hWnd, szWindowTitle);

	return S_OK;
}

HRESULT CLevel_Field::Ready_Lights()
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

	// _float4 vDiffuse = _float4(0.745f, 0.797f, 0.8f, 0.f);
	// _float4 vAmbient = _float4(0.1f, 0.13f, 0.13f, 0.f);
	// _float4 vSpecular = _float4(0.05f, 0.05f, 0.05f, 0.f);

	_float4 vDiffuse = CMyTools::ColorRGBA_HEXtoFLOAT4(0x1E1C1B3C);
	_float4 vAmbient = CMyTools::ColorRGBA_HEXtoFLOAT4(0x1A191900);
	_float4 vSpecular = CMyTools::ColorRGBA_HEXtoFLOAT4(0x12121200);

#if gimch || 진우
	vDiffuse = _float4(0.361f, 0.451f, 0.451f, 0.204f);
	vAmbient = _float4(0.161f, 0.161f, 0.161f, 0.0f);
	vSpecular = _float4(0.05f, 0.05f, 0.05f, 0.f);
#endif // gimch

	
	m_pLight->Get_Component<CLight>()->Set_Color(vDiffuse, vAmbient, vSpecular);
	m_pLight->Get_Component<CTransform>()->RotationQ(XMVectorSet(-0.574f, -0.409f, 0.584f, -0.402f));
	m_pLight->Get_Component<CTransform>()->Translation(XMVectorSet(0.f, 0.f, 0.f, 1.f));
	return S_OK;
}

HRESULT CLevel_Field::Ready_Volumetric()
{
	m_pGameInstance->Setting_Volumetirc(1.812f, 0.003f, 0.56f, 1.f, 0.031f);


	return S_OK;
}

HRESULT CLevel_Field::Ready_Camera()
{
#ifdef _DEBUG
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

	if (FAILED(m_pGameInstance->Bind_Camera(NEXT_LEVEL, CAMERA_DEBUG, true))) {
		return E_FAIL;
	}
#endif // _DEBUG

	{
		CCamera_Cinematic::Camera_Cinematic_DESC            Camera_Desc{};
		Camera_Desc.fSpeedPerSec = 5.f;
		Camera_Desc.fRotationPerSec = XMConvertToRadians(90.0f);
		Camera_Desc.fFovy = XMConvertToRadians(60.0f);
		Camera_Desc.fNear = 0.1f;
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

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_SkyBox()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSkyBox>(g_iStaticLevel, NEXT_LEVEL, LAYER_SKYBOX))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Field::Ready_Background()
{
	m_pGameInstance->Setting_Volumetirc(1.812f, 0.003f, 0.56f, 1.f, 0.031f);

	CInfoInstance::GetInstance()->Load_MapObjects("Dungeon_Map_Data", LAYER_BACKGROUND);
	CInfoInstance::GetInstance()->Load_WorldDecal("Duengon_Decal_Data", LAYER_BACKGROUND);
	CInfoInstance::GetInstance()->Load_PointLights("Duengon_PointLight_Data", LAYER_BACKGROUND);

	CInstancedProp::INSTANCE_PROP_DESC Desc = {};

	Desc.bEnableRigidbody = false;
	Desc.isShake = false;
	Desc.vRadius = _float2(0.015f, 0.04f);
	Desc.vSpeed = _float2(0.3f, 1.f);
	Desc.strPrototypeTag = L"Prototype_Component_VIBuffer_Model_Instancel_BogMyrtle_A";
	Desc.strInstanceDataPath = "../Bin/Resources/Data/Map/Instance/BogMyrtle_A_Dungeon.bin";
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CInstancedProp>(g_iStaticLevel, NEXT_LEVEL, LAYER_BACKGROUND, &Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_UI(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Camera()
{
	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Sound()
{
	return S_OK;
}

HRESULT CLevel_Field::Ready_Markers()
{

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Player(const _wstring& strLayerTag)
{
	CPlayer::PLAYERDESC playerDesc = {};
	playerDesc.vPos = _float4(-17.96f, 18.74f, 9.11f, 1.f);
	playerDesc.vRotQ = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CPlayer>(g_iStaticLevel, NEXT_LEVEL, strLayerTag, &playerDesc))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	if (FAILED(m_pInfoInstance->Load_EffectParts("Goo0_Data", "../Bin/Resources/Data/Effect/MapEffect/Ranrok_Goo0")))
		return E_FAIL;
	if (FAILED(m_pInfoInstance->Load_EffectParts("Goo1_Data", "../Bin/Resources/Data/Effect/MapEffect/Ranrok_Goo1")))
		return E_FAIL;
	if (FAILED(m_pInfoInstance->Load_EffectParts("Ranrok_Goo2_Data", "../Bin/Resources/Data/Effect/MapEffect/Ranrok_Goo2")))
		return E_FAIL;
	if (FAILED(m_pInfoInstance->Load_EffectParts("Ranrok_Goo3_Data", "../Bin/Resources/Data/Effect/MapEffect/Ranrok_Goo3")))
		return E_FAIL;
	if (FAILED(m_pInfoInstance->Load_EffectParts("Ranrok_Goo4_Data", "../Bin/Resources/Data/Effect/MapEffect/Ranrok_Goo4")))
		return E_FAIL;
	if (FAILED(m_pInfoInstance->Load_EffectParts("Ranrok_Decal_15_Data", "../Bin/Resources/Data/Effect/MapEffect/Ranrok_Decal_15")))
		return E_FAIL;
	if (FAILED(m_pInfoInstance->Load_EffectParts("Rotate_Rock_Small_Data", "../Bin/Resources/Data/Effect/MapEffect/Rotate_Rock_Small")))
		return E_FAIL;
	if (FAILED(m_pInfoInstance->Load_EffectParts("Ranrok_Particle_Black_Data", "../Bin/Resources/Data/Effect/MapEffect/Ranrok_Particle_Black")))
		return E_FAIL;
	if (FAILED(m_pInfoInstance->Load_EffectParts("Ranrok_Particle_Red_Data", "../Bin/Resources/Data/Effect/MapEffect/Ranrok_Particle_Red")))
		return E_FAIL;
	if (FAILED(m_pInfoInstance->Load_EffectParts("Ranrok_Decal_35_Data", "../Bin/Resources/Data/Effect/MapEffect/Ranrok_Decal_35")))
		return E_FAIL;
	if (FAILED(m_pInfoInstance->Load_EffectParts("Rotate_Rock_Large1_Data", "../Bin/Resources/Data/Effect/MapEffect/Rotate_Rock_Large1")))
		return E_FAIL;
	if (FAILED(m_pInfoInstance->Load_EffectParts("Rotate_Rock_Large2_Data", "../Bin/Resources/Data/Effect/MapEffect/Rotate_Rock_Large2")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Monster()
{
	CRanrok::RANROKDESC RanrokDesc = {};
	RanrokDesc.vPos = _float4(75.550f, 33.734f, 164.013f, 1.f);
	RanrokDesc.vRotQ = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRanrok>(g_iStaticLevel, NEXT_LEVEL, LAYER_MONSTER, &RanrokDesc))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Field::Reday_Layer_EffectPool()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEffectPool>(g_iStaticLevel, NEXT_LEVEL, LAYER_EFFECTPOOL))) {
		return E_FAIL;
	}
	return S_OK;
}

void CLevel_Field::ResetLevel_Environment()
{
	_float4 vDiffuse = CMyTools::ColorRGBA_HEXtoFLOAT4(0x1E1C1B3C);
	_float4 vAmbient = CMyTools::ColorRGBA_HEXtoFLOAT4(0x1A191900);
	_float4 vSpecular = CMyTools::ColorRGBA_HEXtoFLOAT4(0x12121200);
	m_pLight->Get_Component<CLight>()->Set_Color(vDiffuse, vAmbient, vSpecular);
	m_pLight->Get_Component<CTransform>()->RotationQ(XMVectorSet(-0.574f, -0.409f, 0.584f, -0.402f));
	m_pLight->Get_Component<CTransform>()->Translation(XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Set_Environment(_float3(0.585f, 0.182f, 2.83f), _float2(0.075f, 0.150f), _float2(2.300f, 10.000f), _float4(0.0360f, 0.0326f, 0.0057f, 0.0018f), _float4(2.0000f, 1.f, 1.f, 1.f));
	m_pGameInstance->Setting_Volumetirc(1.743f, 0.0001f, 0.2000f, 1.050f, 0.04862f);
	m_pLight->Capture_PreShadow();
}

pair<CLevel*, function<void()>> CLevel_Field::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, void* pArg)
{
	CLevel_Field* pInstance = new CLevel_Field(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CLevel_Field");
		SAFE_RELEASE(pInstance);
	}

	return { pInstance, [pInstance]() { pInstance->Ready_Layer_Camera(); pInstance->Ready_Layer_Sound(); pInstance->m_pInfoInstance->Load_CutScenes(); } };
}

void CLevel_Field::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);
}


#ifdef _DEBUG
void CLevel_Field::Describe_Entity()
{
}
#endif // _DEBUG
