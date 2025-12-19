#include "pch.h"
#include "Level_Field.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "InfoInstance.h"
#include "Light_Main.h"
#include "Layer.h"
#include "Camera_Debug.h"
#include "Terrain.h"
#include "SkyBox.h"

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

	if (FAILED(Ready_Layer_SkyBox())) {
		return E_FAIL;
	}
	if (FAILED(Ready_Background())) {
		return E_FAIL;
	}

	if (FAILED(Ready_Camera())) {
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

	return S_OK;
}

HRESULT CLevel_Field::Initialize()
{
	assert(false);
	return E_FAIL;
}

void CLevel_Field::Update(_float fTimeDelta)
{
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
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLight_Main>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_LIGHT))) {
		return E_FAIL;
	}
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

	m_pGameInstance->Add_Camera(g_iStaticLevel, pCamera, CAMERA_DEBUG);

#endif // _DEBUG

	if (FAILED(m_pGameInstance->Bind_Camera(g_iStaticLevel, CAMERA_DEBUG, true))) {
		return E_FAIL;
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
	_float4 vColor = _float4(0.2f, 0.2f, 0.2f, 1.f);
	m_pGameInstance->Set_FogColor(vColor);
	m_pGameInstance->Set_Fog(10.f, 5.f);

	CInfoInstance::GetInstance()->Load_MapObjects("Dungeon_Map_Data");

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

HRESULT CLevel_Field::Ready_Layer_Effect(const _wstring& strLayerTag)
{
	return S_OK;
}

pair<CLevel*, function<void()>> CLevel_Field::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, void* pArg)
{
	CLevel_Field* pInstance = new CLevel_Field(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CLevel_Field");
		SAFE_RELEASE(pInstance);
	}

	return { pInstance, [pInstance]() { pInstance->Ready_Layer_Camera(); pInstance->Ready_Layer_Sound(); } };
}

void CLevel_Field::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pInfoInstance);
}
