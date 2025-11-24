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
#include "Player.h"

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

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain")))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_BuildingContainer(TEXT("Layer_Building")))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_MapObjectManager(LAYER_MAPOBJECTMANAGER))) {
		return E_FAIL;
	}

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

HRESULT CLevel_MapViewer::Ready_Layer_Background(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummySkyBox>(g_iStaticLevel, NEXT_LEVEL, strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLand>(g_iStaticLevel, NEXT_LEVEL, strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_Terrain(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CTerrain>(g_iStaticLevel, NEXT_LEVEL, strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_BuildingContainer(const _wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBuildingContainer>(g_iStaticLevel, NEXT_LEVEL, strLayerTag)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_MapViewer::Ready_Layer_MapObjectManager(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMapObject_Manager>(g_iStaticLevel, NEXT_LEVEL, strLayerTag)))
		return E_FAIL;

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
