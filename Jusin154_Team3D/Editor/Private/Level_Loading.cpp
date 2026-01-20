#include "pch.h"
#include "Level_Loading.h"
#include "Loader.h"
#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_UI.h"
#include "Level_ObjectViewer.h"
#include "Level_EffectViewer.h"
#include "Level_MapViewer.h"
#include "Level_BloomViewer.h"
#include "Level_PhysXLab.h"

#include "Intro_Image.h"
#include "Intro_BG.h"
#include "Loding_Panel.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID)
	: CLevel{ pDevice, pContext, ENUM_CLASS(eLevelID) }
{

}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader) {
		return E_FAIL;
	}

	if (FAILED(Ready_Layer_BackGround(LAYER_UI))) {
		return E_FAIL;

	}
	m_bNextLevel = false;
	m_fTimeDelta = 1.2f;
	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if (true == m_pLoader->isFinished())
	{
		if(m_eNextLevelID == LEVEL::UI)
			m_bNextLevel = true;

#ifdef 기무리
		m_bNextLevel = true;
#endif // 기무리
		if (m_pGameInstance->Key_Down(DIK_F1))
		{
			if (m_eNextLevelID != LEVEL::LOGO){
				m_bNextLevel = true;
			}
			else
			{
				static_cast<CUIObject*>(m_pIntro_Image)->Set_Hover(true);
				m_bDelay = true;
			}
		}
		

	}

	if (m_bDelay == true)
	{
		m_fTimeDelta -= fTimeDelta;
		if (m_fTimeDelta <= 0.f)
		{
			m_bNextLevel = true;
		}
	}


	if (m_bNextLevel == true)
	{
		CLevel* pNewLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL::LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::UI:
			pNewLevel = CLevel_UI::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::MAP:
			pNewLevel = CLevel_MapViewer::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::OBJECT:
			pNewLevel = CLevel_ObjectViewer::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
			//case LEVEL::COMBINED:
			//	pNewLevel = CLevel_LevelViewer::Create(m_pDevice, m_pContext, m_eNextLevelID);
			//	break;
		case LEVEL::EFFECT:
			pNewLevel = CLevel_EffectViewer::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::PHYSX:
			pNewLevel = CLevel_PhysXLab::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		case LEVEL::BLOOM:
			pNewLevel = CLevel_BloomViewer::Create(m_pDevice, m_pContext, m_eNextLevelID);
			break;
		}

		if (FAILED(m_pGameInstance->Change_Level(pNewLevel)))
			return;
	}
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Output();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const _wstring& strLayerTag)
{
	if (m_eNextLevelID == LEVEL::LOGO)
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CIntro_BG>(g_iStaticLevel, ENUM_CLASS(LEVEL::LOADING), strLayerTag))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CIntro_Image>(g_iStaticLevel, ENUM_CLASS(LEVEL::LOADING), strLayerTag, nullptr, nullptr, reinterpret_cast<CIntro_Image**>(&m_pIntro_Image)))) {
			return E_FAIL;
		}
	}

	else
	{
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLoding_Panel>(g_iStaticLevel, ENUM_CLASS(LEVEL::LOADING), strLayerTag))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevelID, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pContext, eLevelID);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}



void CLevel_Loading::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pLoader);


}

HRESULT CLevel_Loading::Initialize()
{
	assert(false && TEXT("Denied"));
	return E_NOTIMPL;
}
