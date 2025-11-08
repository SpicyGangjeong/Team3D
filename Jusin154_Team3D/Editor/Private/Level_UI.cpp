#include "pch.h"
#include "Level_UI.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "DummyRect.h"
#include "Mission.h"
#include "Mouse_Cursor.h"

CLevel_UI::CLevel_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{
}

HRESULT CLevel_UI::Initialize()
{
	if (FAILED(Ready_Layer_UI(LAYER_BACKGROUND))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Layer_Cursor(LAYER_UI))) {
		return E_FAIL;
	}
	return S_OK;
}


void CLevel_UI::Update(_float fTimeDelta)
{
	GUI::Begin("SelectEditor");

	GUI::End();
}

HRESULT CLevel_UI::Render()
{
	SetWindowText(g_hWnd, TEXT("UI레벨입니다."));
	GUI::ShowDemoWindow();
	return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_UI(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMission>(g_iStaticLevel, NEXT_LEVEL, strLayerTag)))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_UI::Ready_Layer_Cursor(const _wstring& strLayerTag)
{

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMouse_Cursor>(g_iStaticLevel, NEXT_LEVEL, strLayerTag)))
	{
		return E_FAIL;
	}
	return S_OK;
}


CLevel_UI* CLevel_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_UI* pInstance = new CLevel_UI(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_UI");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}



void CLevel_UI::Free()
{
	__super::Free();


}
