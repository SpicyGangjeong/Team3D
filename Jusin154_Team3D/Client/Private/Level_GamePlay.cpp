#include "pch.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Layer.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{

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

	return S_OK;
}

HRESULT CLevel_GamePlay::Initialize()
{
	assert(false);
	return E_FAIL;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	if (true == m_pGameInstance->Check_LevelShouldChange()) {
		//if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::WOODS)))) {
		//	return;
		//}
	}
}

HRESULT CLevel_GamePlay::Render()
{
	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다"));
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT::DIRECTIONAL;
	LightDesc.vDiffuse = _float4(0.8f, 0.8f, 0.8f, 0.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 0.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	if (FAILED(m_pGameInstance->On_Light(NEXT_LEVEL, TEXT("Main_Light"), LightDesc, nullptr))) {
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera()
{
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

}
