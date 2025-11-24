#include "pch.h"
#include "Action_Panel.h"
#include "GameInstance.h"
#include "Spell_Slot.h"
#include "Spell_Image.h"
#include "Spell_Overlay.h"
#include "Slot_Number.h"
#include "HpBarBG.h"
#include "Magic_Meter.h"
#include "Magic_Icon.h"

CAction_Panel::CAction_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CAction_Panel::CAction_Panel(const CAction_Panel& rhs)
	:CPanelObject(rhs)
{
}

HRESULT CAction_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAction_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 1515.f;
	Desc.fY = 850.f;
	Desc.fSizeX = 700.f;
	Desc.fSizeY = 400.f;


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

	m_bVisible = true;
	m_bActive = true;
	Magic_Meter_UV();
	Magic_Meter_Visible(1, true);
	Magic_Meter_Visible(5, true);
	return S_OK;
}

void CAction_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CAction_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	
	Matic_Meter_Move();

	__super::Update(fTimeDelta);
}

void CAction_Panel::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_bVisible) {

	}
	__super::Late_Update(fTimeDelta);

}

HRESULT CAction_Panel::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::DEFAULT)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	return S_OK;
}

_vector CAction_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CAction_Panel::Magic_Meter_Visible(_uint iIndex, _bool bVisible)
{
	switch (iIndex)
	{
	case 1:
		static_cast<CMagic_Meter*>(m_pMagic_Meter1)->Visible(bVisible);
		break;
	case 2:
		static_cast<CMagic_Meter*>(m_pMagic_Meter2)->Visible(bVisible);
		break;
	case 3:
		static_cast<CMagic_Meter*>(m_pMagic_Meter3)->Visible(bVisible);
		break;
	case 4:
		static_cast<CMagic_Meter*>(m_pMagic_Meter4)->Visible(bVisible);
		break;
	case 5:
		static_cast<CMagic_Meter*>(m_pMagic_Meter5)->Visible(bVisible);
		break;

	default:
		return;
	}
	if (bVisible)
		m_iMagic_Meter_Count++;
	else
		m_iMagic_Meter_Count--;
}

void CAction_Panel::Magic_Meter_UV()
{
	static_cast<CMagic_Meter*>(m_pMagic_Meter1)->Meter_Index(2);
	static_cast<CMagic_Meter*>(m_pMagic_Meter2)->Meter_Index(1);
	static_cast<CMagic_Meter*>(m_pMagic_Meter3)->Meter_Index(1);
	static_cast<CMagic_Meter*>(m_pMagic_Meter4)->Meter_Index(1);
	static_cast<CMagic_Meter*>(m_pMagic_Meter5)->Meter_Index(0);

	for (_uint i = 0; i < 5; ++i)
	{
		m_vMagic_MeterUV[i].x = 190.f - (55.f * i);
		m_vMagic_MeterUV[i].y = 145.f;
	}
}

void CAction_Panel::Matic_Meter_Move()
{

	static_cast<CMagic_Meter*>(m_pMagic_Meter1)->Move(m_vMagic_MeterUV[0].x, m_vMagic_MeterUV[0].y);
	static_cast<CMagic_Meter*>(m_pMagic_Meter5)->Move(m_vMagic_MeterUV[1].x, m_vMagic_MeterUV[1].y);

	if (static_cast<CMagic_Meter*>(m_pMagic_Meter2)->Get_Active() == true)
	{
		static_cast<CMagic_Meter*>(m_pMagic_Meter2)->Move(m_vMagic_MeterUV[1].x, m_vMagic_MeterUV[1].y);
		static_cast<CMagic_Meter*>(m_pMagic_Meter5)->Move(m_vMagic_MeterUV[2].x, m_vMagic_MeterUV[2].y);
	}
	if (static_cast<CMagic_Meter*>(m_pMagic_Meter3)->Get_Active() == true)
	{
		static_cast<CMagic_Meter*>(m_pMagic_Meter2)->Move(m_vMagic_MeterUV[1].x, m_vMagic_MeterUV[1].y); 
		static_cast<CMagic_Meter*>(m_pMagic_Meter3)->Move(m_vMagic_MeterUV[2].x, m_vMagic_MeterUV[2].y);
		static_cast<CMagic_Meter*>(m_pMagic_Meter5)->Move(m_vMagic_MeterUV[3].x, m_vMagic_MeterUV[3].y);
	}
	if (static_cast<CMagic_Meter*>(m_pMagic_Meter4)->Get_Active() == true)
	{
		static_cast<CMagic_Meter*>(m_pMagic_Meter2)->Move(m_vMagic_MeterUV[1].x, m_vMagic_MeterUV[1].y); 
		static_cast<CMagic_Meter*>(m_pMagic_Meter3)->Move(m_vMagic_MeterUV[2].x, m_vMagic_MeterUV[2].y);
		static_cast<CMagic_Meter*>(m_pMagic_Meter4)->Move(m_vMagic_MeterUV[3].x, m_vMagic_MeterUV[3].y);
		static_cast<CMagic_Meter*>(m_pMagic_Meter5)->Move(m_vMagic_MeterUV[4].x, m_vMagic_MeterUV[4].y);
	}

}

HRESULT CAction_Panel::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CAction_Panel::Ready_Components(void* pArg)
{
	return S_OK;
}

HRESULT CAction_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Slot>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CSpell_Slot**>(&m_pSpell_Slot))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Slot"), m_pSpell_Slot);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Image>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Image**>(&m_pSpell_Image))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Image"), m_pSpell_Image);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Overlay>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Overlay**>(&m_pSpell_Overlay))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Overlay"), m_pSpell_Overlay);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSlot_Number>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSlot_Number**>(&m_pSlot_Number))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Slot_Number"), m_pSlot_Number);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CHpBarBG>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CHpBarBG**>(&m_pHpBarBG))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("HpBarBG"), m_pHpBarBG);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMagic_Meter>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CMagic_Meter**>(&m_pMagic_Meter1))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Magic_Meter1"), m_pMagic_Meter1);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMagic_Meter>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CMagic_Meter**>(&m_pMagic_Meter2))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Magic_Meter2"), m_pMagic_Meter2);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMagic_Meter>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CMagic_Meter**>(&m_pMagic_Meter3))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Magic_Meter3"), m_pMagic_Meter3);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMagic_Meter>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CMagic_Meter**>(&m_pMagic_Meter4))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Magic_Meter4"), m_pMagic_Meter4);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMagic_Meter>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CMagic_Meter**>(&m_pMagic_Meter5))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Magic_Meter5"), m_pMagic_Meter5);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMagic_Icon>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CMagic_Icon**>(&m_pMagic_Icon))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Magic_Icon"), m_pMagic_Icon);
	return S_OK;
}

CAction_Panel* CAction_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAction_Panel* pInstance = new CAction_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAction_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CAction_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CAction_Panel* pInstance = new CAction_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAction_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CAction_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CAction_Panel::Describe_Entity()
{
}
