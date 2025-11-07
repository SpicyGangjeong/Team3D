#include "pch.h"
#include "Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "DummyRect.h"


CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{

}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_UI(LAYER_BACKGROUND))) {
		return E_FAIL;
	}
	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	GUI::Begin("SelectEditor");
	//if (GUI::Button("Map Editor", { 100, 100 })) {
	//	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::MAP))))
	//		return;
	//}
	/*else */if (GUI::Button("Object Editor", { 100, 100 })) {
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::OBJECT))))
			return;
	}
	//else if (GUI::Button("Combined Editor", { 100, 100 })) {
	//	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::COMBINED))))
	//		return;
	//}
	//else if (GUI::Button("Effect Editor", { 100, 100 })) {
	//	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::EFFECT))))
	//		return;
	//}
	//else if (GUI::Button("Skill Editor", { 100, 100 })) {
	//	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::SKIllSTUDIO))))
	//		return;
	//}
	//else if (GUI::Button("Particle Editor", { 100, 100 })) {
	//	if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::PARTICLE))))
	//		return;
	//}
	GUI::End();
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다"));
	GUI::ShowDemoWindow();
	return S_OK;
}



HRESULT CLevel_Logo::Ready_Layer_Camera(const _wstring& strLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const _wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDummyRect>(g_iStaticLevel, NEXT_LEVEL, strLayerTag))) {
		return E_FAIL;
	}
	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}



void CLevel_Logo::Free()
{
	__super::Free();


}
