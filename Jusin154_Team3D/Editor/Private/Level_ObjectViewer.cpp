#include "pch.h"
#include "Level_ObjectViewer.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Dummy_Goblin.h"
#include "DebugCamera.h"

CLevel_ObjectViewer::CLevel_ObjectViewer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{

}

HRESULT CLevel_ObjectViewer::Initialize()
{

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	{
		return E_FAIL;
	}
	

	if (FAILED(Ready_Layer_Dummy(TEXT("Layer_Dummy"))))
	{
		return E_FAIL;
	}


	return S_OK;
}

void CLevel_ObjectViewer::Update(_float fTimeDelta)
{

}

HRESULT CLevel_ObjectViewer::Render()
{
	SetWindowText(g_hWnd, TEXT("오브젝트 레벨입니다"));
	GUI::ShowDemoWindow();
	return S_OK;
}



HRESULT CLevel_ObjectViewer::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	//CDebugCamera::CAMERA_DEBUG_DESC			CameraDesc{};
	//CameraDesc.fFovy = XMConvertToRadians(60.0f);
	//CameraDesc.fNear = 0.1f;
	//CameraDesc.fFar = 500.f;
	//CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
	//CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	//CameraDesc.fSpeedPerSec = 5.f;
	//CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	//CameraDesc.fMouseSensor = 0.1f;

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDebugCamera>(ENUM_CLASS(LEVEL::STATIC),ENUM_CLASS(LEVEL::OBJECT), 
	//	strLayerTag, &CameraDesc)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_ObjectViewer::Ready_Layer_UI(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_ObjectViewer::Ready_Layer_Dummy(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_Goblin>(ENUM_CLASS(LEVEL::OBJECT), ENUM_CLASS(LEVEL::OBJECT), TEXT("Layer_Dummy"))))
		return E_FAIL;

	return S_OK;
}

CLevel_ObjectViewer* CLevel_ObjectViewer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_ObjectViewer* pInstance = new CLevel_ObjectViewer(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_ObjectViewer");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}



void CLevel_ObjectViewer::Free()
{
	__super::Free();


}
