#include "pch.h"
#include "Level_Logo.h"

#include "GameInstance.h"
#include "Level_Loading.h"


CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{

}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer_UI(LAYER_UI))) {
		return E_FAIL;
	}
	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Mouse_Up(DIM_RBUTTON))
	{
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::GAMEPLAY)))) {
			return;
		}
	}
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다"));
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_UI(const _wstring& strLayerTag)
{
	return S_OK;
}

pair<CLevel_Logo*, function<void()>> CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		SAFE_RELEASE(pInstance);
	}

	return { pInstance, nullptr };
}

void CLevel_Logo::Free()
{
	__super::Free();


}
