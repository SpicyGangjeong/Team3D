#include "pch.h"
#include "Level_Loading.h"
#include "Loader.h"
#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_UI.h"
#include "Level_ObjectViewer.h"
#include "Level_EffectViewer.h"
#include "Level_MapViewer.h"

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
	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if (true == m_pLoader->isFinished() &&
		m_pGameInstance->Key_Down(DIK_F1))
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
		//case LEVEL::SKIllSTUDIO:
		//	pNewLevel = CLevel_SkillStudio::Create(m_pDevice, m_pContext, m_eNextLevelID);
		//	break;
		//case LEVEL::PARTICLE:
		//	pNewLevel = CLevel_ParticleViewer::Create(m_pDevice, m_pContext, m_eNextLevelID);
		//	break;
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
