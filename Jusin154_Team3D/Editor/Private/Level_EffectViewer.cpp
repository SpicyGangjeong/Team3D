#include "pch.h"
#include "Level_EffectViewer.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Debug.h"
#include "TestEffect.h"
#include "Effect_Editor.h"
#include "Dummy_Cube.h"
#include "MainLight.h"
#include "Dummy_Plane.h"
#include "DummySkyBox.h"
#include "Dummy_PhysXBox.h"
#include "Dummy_PhysXPlayable.h"
#include "Dummy_PhysXMonster.h"
#include "Dummy_PhysXWall.h"
#include "Player.h"
#include "EffectPool.h"
#include "InfoInstance.h"
#include "Goblin.h"
#include "Terrain.h"
#include "Troll.h"
#include "Ranrok.h"
#include "RaceRing.h"

CLevel_EffectViewer::CLevel_EffectViewer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{
	m_pInfoInstance = CInfoInstance::GetInstance();
}

HRESULT CLevel_EffectViewer::Initialize()
{
	if (FAILED(Ready_Layer_Light())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_PhysX(TEXT("Layer_PhysX"))))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	{
		return E_FAIL;
	}


	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	{
		return E_FAIL;
	}



	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEffectPool>(g_iStaticLevel, NEXT_LEVEL, TEXT("Z_Layer_EffectPool")))) //플레이어보다 먼저 생성해야함!
	{
		return E_FAIL;
	}



	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEffect_Editor>(ENUM_CLASS(LEVEL::EFFECT), NEXT_LEVEL, TEXT("Z_Layer_Editor")))){
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CPlayer>(g_iStaticLevel, NEXT_LEVEL, LAYER_PLAYER))){
		return E_FAIL;
	}

 //    	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_Plane>(NEXT_LEVEL, NEXT_LEVEL, TEXT("Layer_Plane")))) //플레이어보다 먼저 생성해야함!
	//{
	//	return E_FAIL;     
	//}

/*	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CGoblin>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Monster"))))
		return E_FAIL;*/	

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTroll>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Monster"))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRanrok>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummySkyBox>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Sky")))){
		return E_FAIL;
	}


	CRaceRing::RACERING_DESC pDesc = {};

	pDesc.vPosition = _float3(0.f, 0.f, 0.f);
	pDesc.vScale = _float3(1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRaceRing>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Sky") , &pDesc))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Background"))))
	{
		return E_FAIL;
	}

	ZeroMemory(&m_PlaneData, sizeof(m_PlaneData));
	m_PlaneData.eKind = PHYSX_KIND::BODY_STATIC;
	m_PlaneData.iSubKind = ENUM_CLASS(PXOBJECT::TERRAIN);

	m_pGameInstance->Add_Editor_Plane(m_PlaneData);


/* 호그 스미드 */
	
	//m_pGameInstance->Setting_Volumetirc(
	//	1.251f,                         // 밀도
	//	0.0253f,                          // 빛 강도
	//	0.9f,                          // 산란 계수
	//	1.78f,                           // 깊이 분포 계수
	//	0.f);

/* 란록 맵*/

	//pLight->Capture_PreShadow();


	return S_OK;
}

void CLevel_EffectViewer::Update(_float fTimeDelta)
{
	m_pInfoInstance->Update(fTimeDelta);
}

HRESULT CLevel_EffectViewer::Render()
{
	SetWindowText(g_hWnd, TEXT("이펙트 레벨입니다"));
	GUI::ShowDemoWindow();
	return S_OK;
}



HRESULT CLevel_EffectViewer::Ready_Layer_Camera(const _wstring& strLayerTag)
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
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCamera_Debug>(g_iStaticLevel, NEXT_LEVEL, LAYER_CAMERA, &Camera_Desc, nullptr, &pCamera))) {
		return E_FAIL;
	}
	m_pGameInstance->Add_Camera(g_iStaticLevel, pCamera, CAMERA_DEBUG);
	if (FAILED(m_pGameInstance->Bind_Camera(g_iStaticLevel, CAMERA_DEBUG, true))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_EffectViewer::Ready_Layer_Light()
{



	return S_OK;
}

HRESULT CLevel_EffectViewer::Ready_Layer_UI(const _wstring& strLayerTag)
{
	return S_OK;
}


HRESULT CLevel_EffectViewer::Ready_Layer_Effect(const _wstring& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_EffectViewer::Ready_Layer_PhysX(const _wstring& strLayerTag)
{
	for (int i = 0; i < 10; ++i) {
		CDummy_PhysXBox::PHYSXDUMMY_DESC Desc{};
		Desc.vPos = { m_pGameInstance->Random_Float(0.f, 30.f), m_pGameInstance->Random_Float(3.f, 33.f), m_pGameInstance->Random_Float(0.f, 30.f) };
		Desc.vRotRPY = { m_pGameInstance->Random_Float(0.f, XM_2PI), m_pGameInstance->Random_Float(0.f, XM_2PI), m_pGameInstance->Random_Float(0.f, XM_2PI) };
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::BOX);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXBox>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
			return E_FAIL;
		}
	}

	{
		CDummy_PhysXWall::PHYSXDUMMY_DESC Desc{};
		Desc.vPos = { -15.f, 3.f, 15.f };
		Desc.vRotRPY = { 0.f, m_pGameInstance->Random_Float(0.f, XM_2PI), 0.f };
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::WALL);

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXWall>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
			return E_FAIL;
		}
	} 

	{
		CDummy_PhysXMonster::PHYSXDUMMY_DESC Desc{};
		Desc.vPos = { 0.f, 150.f, 5.f };
		Desc.vRotRPY = { 12.f, 0.f, 0.f };
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::GOBLIN_WARRIOR);
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXMonster>(g_iStaticLevel, NEXT_LEVEL, LAYER_MONSTER, &Desc))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CLevel_EffectViewer::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	CMainLight* pLight = { nullptr };

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMainLight>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_LIGHT, nullptr, nullptr, &pLight))) {
		return E_FAIL;
	}


	_float4 vDiffuse = CMyTools::ColorRGBA_HEXtoFLOAT4(0x1E1C1B3C);
	_float4 vAmbient = CMyTools::ColorRGBA_HEXtoFLOAT4(0x1A191900);
	_float4 vSpecular = CMyTools::ColorRGBA_HEXtoFLOAT4(0x12121200);

	pLight->Get_Component<CLight>()->Set_Color(vDiffuse, vAmbient, vSpecular);
	pLight->Get_Component<CTransform>()->RotationQ(XMVectorSet(-0.574f, -0.409f, 0.584f, -0.402f));
	pLight->Get_Component<CTransform>()->Translation(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pGameInstance->Setting_Volumetirc(1.812f, 0.003f, 0.56f, 1.f, 0.031f);

	 vDiffuse = CMyTools::ColorRGBA_HEXtoFLOAT4(0x1E1C1B3C);
	 vAmbient = CMyTools::ColorRGBA_HEXtoFLOAT4(0x1A191900);
	 vSpecular = CMyTools::ColorRGBA_HEXtoFLOAT4(0x12121200);
	pLight->Get_Component<CLight>()->Set_Color(vDiffuse, vAmbient, vSpecular);
	pLight->Get_Component<CTransform>()->RotationQ(XMVectorSet(-0.574f, -0.409f, 0.584f, -0.402f));
	pLight->Get_Component<CTransform>()->Translation(XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Set_Environment(_float3(0.585f, 0.182f, 2.83f), _float(2.f), _float2(0.075f, 0.150f), _float2(2.300f, 10.000f), _float4(0.0360f, 0.0326f, 0.0057f, 0.0018f), _float4(2.0000f, 1.f, 1.f, 1.f), _float3(0.f, 0.f, -100.f), _float3(0.f, 0.f, 100.f));
	m_pGameInstance->Setting_Volumetirc(1.743f, 0.0001f, 0.2000f, 1.050f, 0.04862f);
	/* Hogsmeade */

	//Desc.isEdit = false;
	//Desc.iAlphaSizeX = 2048;
	//Desc.iAlphaSizeY = 2048;
	//Desc.vPosition = _float3(-194, 18.5f, -153.f);
	//Desc.strAlphaMapTag = "Hogsmeade_AlphaMap.bin";
	//Desc.strHeightMapTag = "Hogsmeade_HeightMap.bin";
	//Desc.strBufferTag = TEXT("Prototype_Component_VIBuffer_Terrain_Hogsmeade");

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTerrain>(g_iStaticLevel, NEXT_LEVEL, TEXT("Layer_Terrain"), &Desc)))
	//	return E_FAIL;


	_float4 vColor = _float4(0.2f, 0.2f, 0.2f, 1.f);
	m_pGameInstance->Set_FogColor(vColor);
	m_pGameInstance->Set_Fog(10.f, 5.f);
	m_pInfoInstance->Load_MapObjects("Dungeon_Map_Data");

	CInfoInstance::GetInstance()->Load_PointLights("Duengon_PointLight_Data");

	return S_OK;
}



CLevel_EffectViewer* CLevel_EffectViewer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_EffectViewer* pInstance = new CLevel_EffectViewer(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_EffectViewer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}



void CLevel_EffectViewer::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);
}
