#include "pch.h"
#include "Ride_Panel.h"
#include "GameInstance.h"
#include "Ride_Info_Key.h"
#include "Ride_Info.h"
#include "Ride_InfoBG.h"
#include "Ride_Booster_Slot.h"
#include "Ride_BoosterBar.h"
#include "Ride_HpBar.h"
#include "Ride_HpSlot.h"

CRide_Panel::CRide_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CRide_Panel::CRide_Panel(const CRide_Panel& rhs)
	:CPanelObject(rhs)
{
}

HRESULT CRide_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRide_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 1650.f;
	Desc.fY = -800.f;
	Desc.fSizeX = 500.f;
	Desc.fSizeY = 500.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };


	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Element(pArg)))
	{
		return E_FAIL;
	}

	m_fCanvasAlpha = 1.f;
	m_fMoveSpeed = 100.f;
	m_fAlphaTime = 3.f;
	Visible(false);
	ElementAllVisible(true);
	m_vLerp_Position = _float4(1750.f, m_fY, 0.f, 1.f);
	static_cast<CUIObject*>(m_pRide_Info)->Visible(false);
	static_cast<CUIObject*>(m_pRide_InfoBG)->Visible(false);
	return S_OK;
}

void CRide_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CRide_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_bFadeIn == true)
	{

		if (m_bHover == true)
			m_bHover = false;

		if (m_fAlpha <= 1.f)
			m_fAlpha += fTimeDelta * m_fAlphaTime;

		if (m_fAlpha >= 1.f)
		{
			m_bFadeIn = false;
			m_fAlpha = 1.f;
		}
	}

	if (m_bFadeOut == true)
	{

		if (m_bHover == false)
			m_bHover = true;

		if (m_fAlpha >= 0.f)
			m_fAlpha -= fTimeDelta;

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_fAlpha = 0.f;
			Visible(false);
		}
	}

	if (m_bHover == true)
	{
		Start_Lerp_Translation(fTimeDelta);
	}
	else
	{
		Move(1650.f, -800.f);
	}

	if (m_pGameInstance->Key_Down(DIK_Q))
	{
		static_cast<CUIObject*>(m_pRide_Info)->Set_Hover(true);
		static_cast<CUIObject*>(m_pRide_InfoBG)->Set_Hover(true);
	}

	__super::Update(fTimeDelta);
}

void CRide_Panel::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_bVisible) {
	}
	__super::Late_Update(fTimeDelta);

}

HRESULT CRide_Panel::Render()
{

	return S_OK;
}

_vector CRide_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CRide_Panel::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CRide_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRide_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRide_InfoBG>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CRide_InfoBG**>(&m_pRide_InfoBG))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Ride_InfoBG"), m_pRide_InfoBG);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRide_Info_Key>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CRide_Info_Key**>(&m_pRide_Info_Key))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Ride_Info_Key"), m_pRide_Info_Key);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRide_Info>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CRide_Info**>(&m_pRide_Info))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Ride_Info"), m_pRide_Info);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRide_Booster_Slot>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CRide_Booster_Slot**>(&m_pRide_Booster_Slot))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Ride_Bbooster_Slot"), m_pRide_Booster_Slot);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRide_BoosterBar>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CRide_BoosterBar**>(&m_pRide_BoosterBar))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Ride_BboosterBar"), m_pRide_BoosterBar);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRide_HpBar>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CRide_HpBar**>(&m_pRide_HpBar))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Ride_HpBar"), m_pRide_HpBar);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CRide_HpSlot>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CRide_HpSlot**>(&m_pRide_HpSlot))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Ride_HpSlot"), m_pRide_HpSlot);

	return S_OK;
}

CRide_Panel* CRide_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRide_Panel* pInstance = new CRide_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRide_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CRide_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CRide_Panel* pInstance = new CRide_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRide_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRide_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

void CRide_Panel::Describe_Entity()
{
}
