#include "pch.h"
#include "Level_PhysXLab.h"
#include "GameInstance.h"

#include "Level_Loading.h"
#include "DebugCamera.h"
#include "Terrain.h"
#include "Dummy_PhysXBox.h"

CLevel_PhysXLab::CLevel_PhysXLab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{

}

void CLevel_PhysXLab::Update(_float fTimeDelta)
{

}

HRESULT CLevel_PhysXLab::Render()
{
	SetWindowText(g_hWnd, TEXT("PhysXEditor레벨입니다"));
	GUI::ShowDemoWindow();
	return S_OK;
}

HRESULT CLevel_PhysXLab::Initialize()
{
	if (FAILED(Ready_Layer_Light()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Terrain(TEXT("Layer_Terrain")))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_PhysXObjects(TEXT("Layer_PhysXObject")))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_PhysXLab::Ready_Layer_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 0.f);
	LightDesc.vAmbient = _float4(0.8f, 0.8f, 0.8f, 0.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->On_Light(NEXT_LEVEL, TEXT("Main_Light"), LightDesc, nullptr))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_PhysXLab::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	CDebugCamera::CAMERA_DEBUG_DESC            CameraDesc{};
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 500.f;
	CameraDesc.vEye = _float3(0.f, 10.f, -10.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.fSpeedPerSec = 2.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	CameraDesc.fMouseSensor = 0.1f;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDebugCamera>(g_iStaticLevel, NEXT_LEVEL,
		strLayerTag, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_PhysXLab::Ready_Layer_Terrain(const _wstring& strLayerTag)
{

	return S_OK;
}

HRESULT CLevel_PhysXLab::Ready_Layer_PhysXObjects(const _wstring& strLayerTag)
{
	for (int i = 0; i < 50; ++i) {
		CDummy_PhysXBox::BOXSTARTPOS_DESC Desc{};
		Desc.vPos = { m_pGameInstance->Random_Float(0.f, 30.f), m_pGameInstance->Random_Float(3.f, 33.f), m_pGameInstance->Random_Float(0.f, 30.f) };
		Desc.vRotRPY = { m_pGameInstance->Random_Float(0.f, XM_2PI), m_pGameInstance->Random_Float(0.f, XM_2PI), m_pGameInstance->Random_Float(0.f, XM_2PI) };

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXBox>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

CLevel_PhysXLab* CLevel_PhysXLab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_PhysXLab* pInstance = new CLevel_PhysXLab(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_PhysXLab");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}



void CLevel_PhysXLab::Free()
{
	__super::Free();
}
