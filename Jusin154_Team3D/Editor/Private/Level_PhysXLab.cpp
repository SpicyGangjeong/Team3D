#include "pch.h"
#include "Level_PhysXLab.h"
#include "GameInstance.h"

#include "Level_Loading.h"
#include "DebugCamera.h"
#include "Terrain.h"
#include "Dummy_PhysXBox.h"
#include "Dummy_PhysXPlayable.h"
#include "Dummy_PhysXMonster.h"
#include "Dummy_PhysXMesh.h"
#include "Dummy_PhysXWall.h"
#include "Dummy_PhysXPlatform.h"
#include "Dummy_PhysXFreeDoor.h"
#include "Dummy_PhysXDoorSet.h"
#include "MapObject_Manager.h"
#include "BuildingContainer.h"
#include "MainLight.h"

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
	if (FAILED(Ready_Layer_Light())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Camera(LAYER_CAMERA))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_PhysXObjects(TEXT("Layer_PhysXObject")))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_PhysXLab::Ready_Layer_Light()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMainLight>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_LIGHT)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_PhysXLab::Ready_Layer_Camera(const _wstring& strLayerTag)
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
	m_pGameInstance->Add_Camera(NEXT_LEVEL, pCamera, CAMERA_DEBUG);
	if (FAILED(m_pGameInstance->Bind_Camera(NEXT_LEVEL, CAMERA_DEBUG, true))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_PhysXLab::Ready_Layer_PhysXObjects(const _wstring& strLayerTag)
{
	for (int i = 0; i < 10; ++i) {
		CDummy_PhysXBox::PHYSXDUMMY_DESC Desc{};
		Desc.vPos = { m_pGameInstance->Random_Float(0.f, 30.f), m_pGameInstance->Random_Float(3.f, 33.f), m_pGameInstance->Random_Float(0.f, 30.f) };
		Desc.vRotRPY = { m_pGameInstance->Random_Float(0.f, XM_2PI), m_pGameInstance->Random_Float(0.f, XM_2PI), m_pGameInstance->Random_Float(0.f, XM_2PI) };
		Desc.iSubKind = 0;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXBox>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
			return E_FAIL;
		}
	}
	{
		CDummy_PhysXWall::PHYSXDUMMY_DESC Desc{};
		Desc.vPos = { -15.f, 3.f, 15.f };
		Desc.vRotRPY = { 0.f, m_pGameInstance->Random_Float(0.f, XM_2PI), 0.f };
		Desc.iSubKind = 23;

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXWall>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
			return E_FAIL;
		}
	}
	//{
	//	CDummy_PhysXDoorSet::PHYSXDUMMY_DESC Desc{};
	//	Desc.vPos = { -15.f, 3.f, -15.f };
	//	Desc.vRotRPY = { 0.f, m_pGameInstance->Random_Float(0.f, XM_2PI), 0.f };
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXDoorSet>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
	//		return E_FAIL;
	//	}
	//}
	{
		CDummy_PhysXPlatform::PHYSXDUMMY_DESC Desc{};
		Desc.vPos = { -3.f, 2.f, 10.f };
		Desc.vRotRPY = { 0.f, m_pGameInstance->Random_Float(0.f, XM_2PI), 0.f };
		Desc.iSubKind = 20;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXPlatform>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
			return E_FAIL;
		}
	}
	{
		CDummy_PhysXPlatform::PHYSXDUMMY_DESC Desc{};
		Desc.vPos = { 0.f, 2.f, 8.f };
		Desc.vRotRPY = { 0.f, m_pGameInstance->Random_Float(0.f, XM_2PI), 0.f };
		Desc.iSubKind = 21;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXPlatform>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
			return E_FAIL;
		}
	}
	{
		CDummy_PhysXFreeDoor::PHYSXDUMMY_DESC Desc{};
		Desc.vPos = { 10.f, 2.f, 10.f };
		Desc.vRotRPY = { 0.f, XMConvertToRadians(-XM_1DIV2PI), 0.f};
		Desc.iSubKind = 24;
		Desc.fAngleLimit = 70.f;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXFreeDoor>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
			return E_FAIL;
		}
	}
	//{
	//	CDummy_PhysXWall::PHYSXDUMMY_DESC Desc{};
	//	Desc.vPos = { 14.f, 2.f, 15.f };
	//	Desc.vRotRPY = { 0.f, XMConvertToRadians(-XM_1DIV2PI), 0.f };
	//	Desc.iSubKind = 23;
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXWall>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
	//		return E_FAIL;
	//	}
	//}
	{
		CDummy_PhysXPlayable::PHYSXDUMMY_DESC Desc{};
		Desc.vPos = { 0.f, 100.f, 0.f };
		Desc.vRotRPY = { 0.f, 0.f, 0.f };
		Desc.iSubKind = 10;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXPlayable>(g_iStaticLevel, NEXT_LEVEL, LAYER_PLAYER, &Desc))) {
			return E_FAIL;
		}
	}
	{
		CDummy_PhysXMonster::PHYSXDUMMY_DESC Desc{};
		Desc.vPos = { 0.f, 100.f, 0.f };
		Desc.vRotRPY = { 12.f, 0.f, 0.f };
		Desc.iSubKind = 10;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXMonster>(g_iStaticLevel, NEXT_LEVEL, LAYER_PLAYER, &Desc))) {
			return E_FAIL;
		}
	}

	//{
	//	CDummy_PhysXMesh::PHYSXDUMMY_DESC Desc{};
	//	Desc.vPos = { 0.f, 10.f, 0.f };
	//	Desc.vRotRPY = { 0.f, 0.f, 0.f };
	//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXMesh>(g_iStaticLevel, NEXT_LEVEL, LAYER_CHUNK, &Desc))) {
	//		return E_FAIL;
	//	}
	//}
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
