#include "pch.h"
#include "Level_BloomViewer.h"
#include "GameInstance.h"
#include "DebugCamera.h"
#include "Dummy_Cube.h"
#include "Dummy_Globe.h"
#include "MainLight.h"
#include "Dummy_Plane.h"

CLevel_BloomViewer::CLevel_BloomViewer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{

}

HRESULT CLevel_BloomViewer::Initialize()
{
	if (FAILED(Ready_Layer_Light())) {
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

	{
		CDummy_Globe::GLOBE_DESC Desc{};
		Desc.wstrKey = TEXT("Prototype_GameObject_SM_DragonGlobe");
		Desc.vPos = { 0.f, 0.f, 0.f };
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_Globe>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
			return E_FAIL;
		}
	}

	{
		CDummy_Globe::GLOBE_DESC Desc{};
		Desc.wstrKey = TEXT("Desc_Globe");
		Desc.vPos = { 3.f, 0.f, 0.f };
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_Globe>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
			return E_FAIL;
		}
	}

	{
		CDummy_Globe::GLOBE_DESC Desc{};
		Desc.wstrKey = TEXT("Desc_Globe2");
		Desc.vPos = { 6.f, 0.f, 0.f };
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_Globe>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
			return E_FAIL;
		}
	}
	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_Cube>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE))){
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_Plane>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE))){
	//	return E_FAIL;
	//}

	return S_OK;
}

void CLevel_BloomViewer::Update(_float fTimeDelta)
{

}

HRESULT CLevel_BloomViewer::Render()
{
	SetWindowText(g_hWnd, TEXT("이펙트 레벨입니다"));
	GUI::ShowDemoWindow();
	return S_OK;
}



HRESULT CLevel_BloomViewer::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CDebugCamera::CAMERA_DEBUG_DESC			CameraDesc{};
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fSpeedPerSec = 5.f;
	CameraDesc.pCameraKey = TEXT("Debug_Camera");
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDebugCamera>(g_iStaticLevel, NEXT_LEVEL,
		strLayerTag, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_BloomViewer::Ready_Layer_Light()
{

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMainLight>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_LIGHT))){
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_BloomViewer::Ready_Layer_UI(const _wstring& strLayerTag)
{
	return S_OK;
}


HRESULT CLevel_BloomViewer::Ready_Layer_Effect(const _wstring& strLayerTag)
{

	return S_OK;
}



CLevel_BloomViewer* CLevel_BloomViewer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_BloomViewer* pInstance = new CLevel_BloomViewer(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_BloomViewer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}



void CLevel_BloomViewer::Free()
{
	__super::Free();


}
