#include "pch.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Light_Main.h"
#include "Camera_Debug.h"
#include "InfoInstance.h"
#include "GamePlay_Canvas.h"
#include "Layer.h"
#include "Player.h"
#include "SkyBox.h"
#include "Broom.h"
#include "Dummy_PhysXWall.h"
#include "Dummy_PhysXPlayable.h"

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
	if (FAILED(Ready_Markers())) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_UI(LAYER_UI))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Effect(LAYER_EFFECT))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_Player(LAYER_PLAYER))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_SkyBox(TEXT("Layer_SkyBox")))) {
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
	if (m_pGameInstance->Mouse_Up(DIM_RBUTTON))
	{
		m_pGameInstance->Set_LevelToChange();
	}

	m_pInfoInstance->Update(fTimeDelta);

	if (true == m_pGameInstance->Check_LevelShouldChange()) {
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::FIELD)))) {
			return;
		}
	}
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다"));
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLight_Main>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_LIGHT))){
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CGamePlay_Canvas>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_UI))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera()
{
	/*CCamera_Debug::CAMERA_DEBUG_DESC            Camera_Desc{};
	Camera_Desc.fFovy = XMConvertToRadians(60.0f);
	Camera_Desc.fNear = 0.1f;
	Camera_Desc.fFar = 200.f;
	Camera_Desc.vEye = _float3(0.f, 10.f, -10.f);
	Camera_Desc.vAt = _float3(0.f, 0.f, 0.f);
	Camera_Desc.fSpeedPerSec = 5.f;
	Camera_Desc.pCameraKey = CAMERA_DEBUG;
	Camera_Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Camera_Desc.fMouseSensor = 0.1f;
	Camera_Desc.iPriority = 99;
	Camera_Desc.pFollowTarget = { nullptr };
	Camera_Desc.pLookTarget = { nullptr };

	CCamera_Debug* pCamera = { nullptr };
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCamera_Debug>(g_iStaticLevel, NEXT_LEVEL, LAYER_CAMERA, &Camera_Desc, nullptr, &pCamera))){
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Camera(NEXT_LEVEL, CAMERA_DEBUG, true))) {
		return E_FAIL;
	}*/

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

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CPlayer>(g_iStaticLevel, NEXT_LEVEL, strLayerTag)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom>(g_iStaticLevel, NEXT_LEVEL, strLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_SkyBox(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSkyBox>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE)))
		return E_FAIL;

	CDummy_PhysXWall::PHYSXDUMMY_DESC Desc{};
	Desc.vPos = { 0.f, 0.f, 0.f };
	Desc.vRotRPY = { 0.f, m_pGameInstance->Random_Float(0.f, XM_2PI), 0.f };
	Desc.iSubKind = 23;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummy_PhysXWall>(g_iStaticLevel, NEXT_LEVEL, LAYER_CUBE, &Desc))) {
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
