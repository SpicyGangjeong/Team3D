#include "pch.h"
#include "Level_Field.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "InfoInstance.h"
#include "Light_Main.h"
#include "Layer.h"

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
	if (m_pGameInstance->Key_Up(DIK_0))
	{
		m_pGameInstance->Set_LevelToChange();
	}

	m_pInfoInstance->Update(fTimeDelta);

	if (true == m_pGameInstance->Check_LevelShouldChange()) {
		if (FAILED(m_pGameInstance->Change_Level(CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL::LOADING, LEVEL::RESTART)))) {
			return;
		}
	}
}

HRESULT CLevel_Field::Render()
{
	SetWindowText(g_hWnd, TEXT("필드레벨"));
	return S_OK;
}

HRESULT CLevel_Field::Ready_Lights()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLight_Main>(ENUM_CLASS(LEVEL::STATIC), NEXT_LEVEL, LAYER_LIGHT))) {
		return E_FAIL;
	}
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
