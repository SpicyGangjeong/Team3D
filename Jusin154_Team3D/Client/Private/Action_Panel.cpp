#include "pch.h"
#include "Action_Panel.h"
#include "GameInstance.h"
#include "Spell_Slot.h"
#include "Spell_Overlay.h"
#include "Slot_Number.h"
#include "HpBarBG.h"
#include "Magic_Meter.h"
#include "Magic_Icon.h"
#include "Spell_UI.h"
#include "Potion.h"
#include "Magic_Item.h"
#include "InfoInstance.h"

CAction_Panel::CAction_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CAction_Panel::CAction_Panel(const CAction_Panel& rhs)
	:CPanelObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CAction_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAction_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 1150.f;
	Desc.fY = -850.f;
	Desc.fSizeX = 1650.f;
	Desc.fSizeY = 450.f;
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


	Magic_Meter_UV();
	Visible(true);
	ElementAllVisible(true);
	Magic_Meter_Visible(1, true);
	Magic_Meter_Visible(2, false);
	Magic_Meter_Visible(3, false);
	Magic_Meter_Visible(4, false);
	static_cast<CMagic_Meter*>(m_pLastMagic_Meter)->Visible(true);
	m_pInfoInstance->Add_Event(TEXT("Spell"), [this](void* p) {this->Use_Spell(*reinterpret_cast<_int*>(p)); });
	m_pInfoInstance->Add_Event(TEXT("GetSpell"), [this](void* p) {this->Spell_Setting(p); });
	m_pInfoInstance->Add_Event(TEXT("Monster_Hit"), [this](void* p) {this->Magic_Meter_Update(); });
	m_pInfoInstance->Add_Event(TEXT("Ancient_Magic_Throw"), [this](void* p) {this->Ancient_Magic_Throw(); });
	m_pInfoInstance->Add_Event(TEXT("Use_Potion"), [this](void* p) {this->Use_Potion(); });
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

	Magic_Meter_Move();
	m_fTime -= fTimeDelta;
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
	return S_OK;
}

_vector CAction_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CAction_Panel::Magic_Meter_Visible(_uint iIndex, _bool bVisible)
{
	if (bVisible == false)
		return;

	switch (iIndex)
	{
	case 1:
		if (m_iMagic_Meter_Count == 0)
		{
			static_cast<CMagic_Meter*>(m_pMagic_Meter1)->Visible(bVisible);
			m_Magic_Meters.push_back(m_pMagic_Meter1);
			m_iMagic_Meter_Count++;
		}
		break;
	case 2:
		if (m_iMagic_Meter_Count == 1)
		{
			static_cast<CMagic_Meter*>(m_pMagic_Meter2)->Visible(bVisible);
			m_Magic_Meters.push_back(m_pMagic_Meter2);
			m_iMagic_Meter_Count++;
		}
		break;
	case 3:
		if (m_iMagic_Meter_Count == 2)
		{
			static_cast<CMagic_Meter*>(m_pMagic_Meter3)->Visible(bVisible);
			m_Magic_Meters.push_back(m_pMagic_Meter3);
			m_iMagic_Meter_Count++;
		}
		break;
	case 4:
		if (m_iMagic_Meter_Count == 3)
		{
			static_cast<CMagic_Meter*>(m_pMagic_Meter4)->Visible(bVisible);
			m_Magic_Meters.push_back(m_pMagic_Meter4);
			m_iMagic_Meter_Count++;
		}
		break;
	default:
		break;
	}
}

void CAction_Panel::Magic_Meter_Update()
{
	for (_int i = 0; i < m_iMagic_Meter_Count; ++i)
	{
		if (static_cast<CMagic_Meter*>(m_Magic_Meters[i])->Charge_Meter() == 0)
			return;
	}

	static_cast<CMagic_Meter*>(m_pLastMagic_Meter)->Charge_Meter();
}

void CAction_Panel::Magic_Meter_UV()
{
	static_cast<CMagic_Meter*>(m_pMagic_Meter1)->Meter_Index(2);
	static_cast<CMagic_Meter*>(m_pMagic_Meter2)->Meter_Index(1);
	static_cast<CMagic_Meter*>(m_pMagic_Meter3)->Meter_Index(1);
	static_cast<CMagic_Meter*>(m_pMagic_Meter4)->Meter_Index(1);
	static_cast<CMagic_Meter*>(m_pLastMagic_Meter)->Meter_Index(0);

	for (_uint i = 0; i < 5; ++i)
	{
		m_vMagic_MeterUV[i].x = 570.f - (55.f * i);
		m_vMagic_MeterUV[i].y = -145.f;
	}
}

void CAction_Panel::Magic_Meter_Move()
{
	static_cast<CMagic_Meter*>(m_Magic_Meters[0])->Move(m_vMagic_MeterUV[0].x, m_vMagic_MeterUV[0].y);
	static_cast<CMagic_Meter*>(m_pLastMagic_Meter)->Move(m_vMagic_MeterUV[1].x, m_vMagic_MeterUV[1].y);

	if (m_iMagic_Meter_Count == 2)
	{
		static_cast<CMagic_Meter*>(m_Magic_Meters[1])->Move(m_vMagic_MeterUV[1].x, m_vMagic_MeterUV[1].y);
		static_cast<CMagic_Meter*>(m_pLastMagic_Meter)->Move(m_vMagic_MeterUV[2].x, m_vMagic_MeterUV[2].y);
	}
	if (m_iMagic_Meter_Count == 3)
	{
		static_cast<CMagic_Meter*>(m_Magic_Meters[1])->Move(m_vMagic_MeterUV[1].x, m_vMagic_MeterUV[1].y);
		static_cast<CMagic_Meter*>(m_Magic_Meters[2])->Move(m_vMagic_MeterUV[2].x, m_vMagic_MeterUV[2].y);
		static_cast<CMagic_Meter*>(m_pLastMagic_Meter)->Move(m_vMagic_MeterUV[3].x, m_vMagic_MeterUV[3].y);
	}
	if (m_iMagic_Meter_Count == 4)
	{
		static_cast<CMagic_Meter*>(m_Magic_Meters[1])->Move(m_vMagic_MeterUV[1].x, m_vMagic_MeterUV[1].y);
		static_cast<CMagic_Meter*>(m_Magic_Meters[2])->Move(m_vMagic_MeterUV[2].x, m_vMagic_MeterUV[2].y);
		static_cast<CMagic_Meter*>(m_Magic_Meters[3])->Move(m_vMagic_MeterUV[3].x, m_vMagic_MeterUV[3].y);
		static_cast<CMagic_Meter*>(m_pLastMagic_Meter)->Move(m_vMagic_MeterUV[4].x, m_vMagic_MeterUV[4].y);
	}

}

void CAction_Panel::Ancient_Magic_Throw()
{
	if (static_cast<CMagic_Meter*>(m_Magic_Meters[0])->Get_Meter() != 15.f)
		return;

	_float MaxGauge = 15;
	MaxGauge = static_cast<CMagic_Meter*>(m_pLastMagic_Meter)->Use_Meter(MaxGauge);

	for (_int i = m_iMagic_Meter_Count - 1; static_cast<_int>(MaxGauge) > 0; --i)
	{
		MaxGauge = static_cast<CMagic_Meter*>(m_Magic_Meters[i])->Use_Meter(MaxGauge);
	}
}

void CAction_Panel::Use_Spell(_int Index)
{
	if (m_fTime >= 0.f)
		return;

	m_fTime = 0.5f;
	switch (Index)
	{
	case 1:
		static_cast<CSpell_Overlay*>(m_pSpell_Overlay1)->Use_Spell();
		break;
	case 2:
		static_cast<CSpell_Overlay*>(m_pSpell_Overlay2)->Use_Spell();
		break;
	case 3:
		static_cast<CSpell_Overlay*>(m_pSpell_Overlay3)->Use_Spell();
		break;
	case 4:
		static_cast<CSpell_Overlay*>(m_pSpell_Overlay4)->Use_Spell();
		break;
	default:
		break;
	}
}

void CAction_Panel::Spell_Setting(void* pArg)
{
	GETSKILLINFO Info = *static_cast<GETSKILLINFO*>(pArg);

	switch (Info.iSlotIndexX + 1)
	{
	case 1:
		static_cast<CSpell_Overlay*>(m_pSpell_Overlay1)->Spell_Setting(Info.iSlotIndexY, Info.iSpellIndex);
		break;
	case 2:
		static_cast<CSpell_Overlay*>(m_pSpell_Overlay2)->Spell_Setting(Info.iSlotIndexY, Info.iSpellIndex);
		break;
	case 3:
		static_cast<CSpell_Overlay*>(m_pSpell_Overlay3)->Spell_Setting(Info.iSlotIndexY, Info.iSpellIndex);
		break;
	case 4:
		static_cast<CSpell_Overlay*>(m_pSpell_Overlay4)->Spell_Setting(Info.iSlotIndexY, Info.iSpellIndex);
		break;
	default:
		break;
	}
}

void CAction_Panel::Use_Potion()
{
	if (m_pInfoInstance->Get_PlayerStatPtr()->Get_Stat().fCurrentHp >= m_pInfoInstance->Get_PlayerStatPtr()->Get_Stat().fMaxHp)
		return;

	_float Heal = static_cast<CPotion*>(m_pPotion)->Use_Potion();
	if (Heal == -1)
		return;

	m_pInfoInstance->Get_PlayerStatPtr()->Add_Hp(Heal);
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
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Slot>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast <CSpell_Slot**>(&m_pSpell_Slot))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Slot"), m_pSpell_Slot);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Overlay>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Overlay**>(&m_pSpell_Overlay1))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Overlay1"), m_pSpell_Overlay1);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Overlay>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Overlay**>(&m_pSpell_Overlay2))))
	{
		return E_FAIL;
	}
	static_cast<CUIObject*>(m_pSpell_Overlay2)->MoveX(481.f);
	Add_Element(TEXT("Spell_Overlay2"), m_pSpell_Overlay2);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Overlay>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Overlay**>(&m_pSpell_Overlay3))))
	{
		return E_FAIL;
	}
	static_cast<CUIObject*>(m_pSpell_Overlay3)->MoveX(582.f);
	Add_Element(TEXT("Spell_Overlay3"), m_pSpell_Overlay3);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Overlay>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Overlay**>(&m_pSpell_Overlay4))))
	{
		return E_FAIL;
	}
	static_cast<CUIObject*>(m_pSpell_Overlay4)->MoveX(683.f);
	Add_Element(TEXT("Spell_Overlay4"), m_pSpell_Overlay4);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSlot_Number>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSlot_Number**>(&m_pSlot_Number))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Slot_Number"), m_pSlot_Number);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CHpBarBG>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CHpBarBG**>(&m_pHpBarBG))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("HpBarBG"), m_pHpBarBG);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMagic_Meter>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CMagic_Meter**>(&m_pMagic_Meter1))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMagic_Meter>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CMagic_Meter**>(&m_pMagic_Meter2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMagic_Meter>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CMagic_Meter**>(&m_pMagic_Meter3))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMagic_Meter>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CMagic_Meter**>(&m_pMagic_Meter4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMagic_Meter>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CMagic_Meter**>(&m_pLastMagic_Meter))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMagic_Icon>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CMagic_Icon**>(&m_pMagic_Icon))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Magic_Icon"), m_pMagic_Icon);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_UI>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_UI**>(&m_pSpell_UI))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_UI"), m_pSpell_UI);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CPotion>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CPotion**>(&m_pPotion))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Potion"), m_pPotion);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CMagic_Item>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CMagic_Item**>(&m_pMagic_Item))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Magic_Item"), m_pMagic_Item);
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

#ifdef _DEBUG
void CAction_Panel::Describe_Entity()
{
}
#endif // _DEBUG
