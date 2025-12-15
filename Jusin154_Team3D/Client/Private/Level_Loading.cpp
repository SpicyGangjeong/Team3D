#include "pch.h"
#include "Level_Loading.h"

#include "Loader.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Restart.h"
#include "Level_Field.h"

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
	//m_ShadowDesc.vAt = _float4(-10.f, -10.f, 10.f, 1.f);
	//m_ShadowDesc.fWidth = 300.f;
	//m_ShadowDesc.fHeight = 300.f;
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
		if (m_eNextLevelID != LEVEL::LOGO)
			m_bNextLevel = true;
		else
		{
			if (m_pGameInstance->Key_Down(DIK_F1))
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
		pair<CLevel*, function<void()>> pairLevel = { nullptr, nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL::LOGO:
		{
			pairLevel = CLevel_Logo::Create(m_pDevice, m_pContext, m_eNextLevelID);
		} break;
		case LEVEL::GAMEPLAY:
		{
			//m_ShadowDesc.vAt = _float4(-194, 18.5f, -153.f, 1.f);
			//m_ShadowDesc.fHeight = 1000.f;
			//m_ShadowDesc.fWidth = 1000.f;
			pairLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext, m_eNextLevelID, nullptr);
		} break;
		case LEVEL::FIELD:
		{
			pairLevel = CLevel_Field::Create(m_pDevice, m_pContext, m_eNextLevelID, nullptr);
		} break;
		case LEVEL::RESTART:
		{
			pairLevel = CLevel_Restart::Create(m_pDevice, m_pContext, m_eNextLevelID);
		} break;

		default:
			assert(false); // E_IMPL
			break;
		}
//		Capture_TerrainPreShadow(m_ShadowDesc);
		if (SUCCEEDED(m_pGameInstance->Change_Level(pairLevel.first))) {
			if (nullptr != pairLevel.second)
			{
				pairLevel.second();
			}
			return;
		}
		else /* FAILED */ {
			assert(false); // 레벨 변경 실패
		}
	}
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Output();

	return S_OK;
}
//
//void CLevel_Loading::Capture_TerrainPreShadow(SHADOW_LIGHT_DESC& ShadowDesc)
//{
//	CLayer* pLayer = m_pGameInstance->Get_Layer(NEXT_LEVEL, LAYER_BACKGROUND);
//	if (nullptr != pLayer) {
//		ShadowDesc.fFar = 1000.f;
//		ShadowDesc.fNear = 0.1f;
//		_float3 vRollPichYaw = { 35.2643897f, 45.f, 0.f }; // (1, -1, 1) 방향 look
//
//		_matrix matRotation = XMMatrixRotationRollPitchYaw(
//			XMConvertToRadians(vRollPichYaw.x), XMConvertToRadians(vRollPichYaw.y), XMConvertToRadians(vRollPichYaw.z));
//
//		XMStoreFloat4(&ShadowDesc.vEye, XMLoadFloat4(&ShadowDesc.vAt) - matRotation.r[2] * (ShadowDesc.fFar * 0.25f));
//
//		if (FAILED(m_pGameInstance->Ready_Shadow_Light(ShadowDesc))) {
//			assert(false);
//		}
//
//		const list<CGameObject*>* pObjects = pLayer->Get_Objects();
//		_float4 vPos = {};
//		for (auto& pObject : *pObjects) {
//			if (FAILED(m_pGameInstance->Add_RenderGroup(RENDER::PRESHADOW, pObject))) {
//				assert(false);
//			}
//		}
//		m_pGameInstance->Render_PreShadow();
//	}
//}

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
