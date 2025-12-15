#include "pch.h"
#include "Spell_Panel.h"
#include "GameInstance.h"
#include "Current_Spell_Slot.h"
#include "Spell_List.h"
#include "Eessential_Spell_Slot.h"
#include "Eessential_Spell.h"
#include "Spell_List_Image.h"
#include "Spell_State.h"
#include "Spell_Hover.h"
#include "Spell_Hover_Effect.h"
#include "Spell_Preview.h"
#include "Spell_Vidio_Border.h"
#include "Current_Slot_Number.h"
#include "Spell_Header.h"
#include "Spell_Header_Line.h"
#include "Spell_Anim.h"
#include "Spell_Drag.h"
#include "InfoInstance.h"

CSpell_Panel::CSpell_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CSpell_Panel::CSpell_Panel(const CSpell_Panel& rhs)
	:CPanelObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CSpell_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpell_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 960.f;
	Desc.fY = 540.f;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

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
	m_fSortZ = 0.04f;
	m_iSpellType = -1;
	m_iPendingSpell = -1;
	m_fDelayTime = 1.f;
	m_bActive = true;
	Add_Function(TEXT("Hover"), [this](void* p) {this->Slot_Chack(p); });
	Add_Function(TEXT("Click"), [this](void* p) {this->Click_Slot(*reinterpret_cast<_bool*>(p)); });
	Add_Function(TEXT("Current_Slot_Click"), [this](void* p) {this->Current_Slot_Chack(*reinterpret_cast<_int*>(p)); });
	Visible(true);
	ElementAllVisible(true);
	return S_OK;
}

void CSpell_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpell_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_iSpellType != -1)
	{
		m_fHoverTimer += fTimeDelta;
	}
	else
	{
		m_fHoverTimer = 0.f;
		Function_Callback(TEXT("FadeOut"));
	}

	if (m_fHoverTimer >= m_fDelayTime)
	{

		Function_Callback(TEXT("FadeIn"));
	}

	__super::Update(fTimeDelta);
}

void CSpell_Panel::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
		__super::Late_Update(fTimeDelta);
	}
}

HRESULT CSpell_Panel::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::ALPHABLEND)))) {
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

_vector CSpell_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

const Client::SPELL_INFO CSpell_Panel::Get_SpellInfo(_int SkillID)
{
	return m_pInfoInstance->Get_Spell_Info(SkillID);
}


void CSpell_Panel::Slot_Chack(void* pArg)
{
	CUIObject::HOVER_INFO* Info = static_cast<CUIObject::HOVER_INFO*>(pArg);

	_int iSlot = Info->iSlotID;
	m_iHoverSlot[iSlot] = Info->iHover_Index;

	int finalHover = -1;
	for (int i = 0; i < 2; ++i)
	{
		if (m_iHoverSlot[i] != -1)
		{
			finalHover = m_iHoverSlot[i];
			break;
		}
	}

	m_iSpellType = finalHover;
	Function_Callback(TEXT("Slot_Hover"), &m_iSpellType);
}

void CSpell_Panel::Click_Slot(_bool bClick)
{
	if (bClick == true)
	{
		m_bHover = true;
	}
	else
	{
		m_bHover = false;
	}
}

void CSpell_Panel::Current_Slot_Chack(_int Index)
{
	m_iCurrent_Slot_Index = Index;
}

HRESULT CSpell_Panel::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOwnerAlpha", &m_fOwnerAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCanvasAlpha", &m_fCanvasAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSpell_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Spell_Widget"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpell_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCurrent_Spell_Slot>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CCurrent_Spell_Slot**>(&m_pCurrent_Spell_Slot))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Current_Spell_Slot"), m_pCurrent_Spell_Slot);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_List>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_List**>(&m_pSpell_List))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_List"), m_pSpell_List);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEessential_Spell_Slot>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CEessential_Spell_Slot**>(&m_pEessential_Spell_Slot))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Eessential_Spell_Slot"), m_pEessential_Spell_Slot);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEessential_Spell>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CEessential_Spell**>(&m_pEessential_Spell))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Eessential_Spell"), m_pEessential_Spell);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_List_Image>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_List_Image**>(&m_pSpell_List_Image))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_List_Image"), m_pSpell_List_Image);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_State>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_State**>(&m_pSpell_State))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_State"), m_pSpell_State);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Hover>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Hover**>(&m_pSpell_Hover))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Hover"), m_pSpell_Hover);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Hover_Effect>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Hover_Effect**>(&m_pSpell_Hover_Effect))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Hover_Effect"), m_pSpell_Hover_Effect);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Preview>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Preview**>(&m_pSpell_Preview))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Preview"), m_pSpell_Preview);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Vidio_Border>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Vidio_Border**>(&m_pSpell_Vidio_Border))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Vidio_Border"), m_pSpell_Vidio_Border);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CCurrent_Slot_Number>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CCurrent_Slot_Number**>(&m_pCurrent_Slot_Number))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Current_Slot_Number"), m_pCurrent_Slot_Number);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Header>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Header**>(&m_pSpell_Header))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Header"), m_pSpell_Header);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Header_Line>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Header_Line**>(&m_pSpell_Header_Line))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Header_Line"), m_pSpell_Header_Line);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Anim>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Anim**>(&m_pSpell_Anim))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Anim"), m_pSpell_Anim);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Drag>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Drag**>(&m_pSpell_Drag))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Drag"), m_pSpell_Drag);

	//if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Data>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, m_Info, nullptr, reinterpret_cast<CSpell_Data**>(&m_pSpell_Data))))
	//{
	//	return E_FAIL;
	//}
	return S_OK;
}

CSpell_Panel* CSpell_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpell_Panel* pInstance = new CSpell_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpell_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpell_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CSpell_Panel* pInstance = new CSpell_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpell_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CSpell_Panel::Describe_Entity()
{
}

#endif // _DEBUG
