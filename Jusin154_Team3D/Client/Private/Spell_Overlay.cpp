#include "pch.h"
#include "Spell_Overlay.h"
#include "GameInstance.h"
#include "InfoInstance.h"

CSpell_Overlay::CSpell_Overlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSpell_Overlay::CSpell_Overlay(const CSpell_Overlay& rhs)
	:CElementObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CSpell_Overlay::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpell_Overlay::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 380.f;
	Desc.fY = 30.f;
	Desc.fSizeX = 105.f;
	Desc.fSizeY = 105.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_iImageFrameX = 8;
	m_iImageFrameY = 8;
	m_fFrame = 0.2f;
	m_fTimeMult = 3.f;
	m_fAngle = XMConvertToRadians(-135.f);
	m_fAlpha = 1.f;
	m_fAlphaTime = 1.f;
	m_vUVScale.x = 1.f;
	m_vUVScale.y = 1.f;
	m_fCoolTime = 5.f;
	m_fSortZ = 0.01f;
	m_iSlotCount = 4;
	m_iCurrent_Slot = 0;
	Array_Clear(-1);
	Bool_Array_Clear(false);
	return S_OK;
}

_float4 CSpell_Overlay::Compute_UI(_uint SpellID)
{
	_float2 fImage_Size = { 1024.f, 1792.f };

	_uint iCountX = 4;
	_uint iCountY = 6;

	_float iImageX = 256.f;
	_float iImageY = 256.f;

	_uint iframeX = SpellID % iCountX;
	_uint iframeY = SpellID / iCountX;

	_float2 UVStart;
	UVStart.x = iframeX * iImageX / fImage_Size.x;
	UVStart.y = iframeY * iImageY / fImage_Size.y;

	_float2 UVEnd;
	UVEnd.x = UVStart.x + (iImageX / fImage_Size.x);
	UVEnd.y = UVStart.y + (iImageY / fImage_Size.y);

	_float4 UV = _float4{ UVStart.x, UVStart.y, UVEnd.x, UVEnd.y };
	return UV;
}

void CSpell_Overlay::Array_Clear(_int Value)
{
	for (_int i = 0; i < m_iSlotCount; ++i)
	{
		m_Spells[i] = Value;
	}
	for (_int i = 0; i < m_iSlotCount; ++i)
	{
		m_iSpellType[i] = Value;
	}
	for (_int i = 0; i < m_iSlotCount; ++i)
	{
		m_vUV[i] = _float4(_float(Value), _float(Value), _float(Value), _float(Value));
	}
}

void CSpell_Overlay::Bool_Array_Clear(_bool Bool)
{
	for (_int i = 0; i < m_iSlotCount; ++i)
	{
		m_bEquipped[i] = Bool;
	}
	for (_int i = 0; i < m_iSlotCount; ++i)
	{
		m_bSpellUsed[i] = Bool;
	}
}

void CSpell_Overlay::Spell_Setting(_int SlotIndex, _int SpellIndex)
{
	if (SlotIndex < 0 || SlotIndex > m_iSlotCount)
	{
		return;
	}

	m_Spells[SlotIndex] = SpellIndex;
	m_bEquipped[SlotIndex] = true;
	m_bSpellUsed[SlotIndex] = m_pInfoInstance->Get_Spell_Info(SpellIndex).bUse_Skill;
	m_vUV[SlotIndex] = Compute_UI(SpellIndex);
	m_iSpellType[SlotIndex] = m_pInfoInstance->Get_Spell_Info(SpellIndex).iSpell_Type;
}

void CSpell_Overlay::Use_Spell()
{
	if (m_bEquipped[m_iCurrent_Slot] == true && m_bSpellUsed[m_iCurrent_Slot] == true)
	{
		m_pInfoInstance->Update_Spell(m_Spells[m_iCurrent_Slot]);
		m_pInfoInstance->Event_CallBack(TEXT("JAP"), &m_Spells[m_iCurrent_Slot]);
	}
}


void CSpell_Overlay::Set_SpellSlot(_int Index)
{
	if (Index < 0)
	{
		m_iCurrent_Slot = m_iSlotCount - 1;
	}
	else if (Index >= m_iSlotCount)
	{
		m_iCurrent_Slot = 0;
	}
	else
	{
		m_iCurrent_Slot = Index;
	}
}

void CSpell_Overlay::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpell_Overlay::Update(_float fTimeDelta)
{


	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_bFadeIn == true)
	{
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
		if (m_fAlpha >= 0.f)
			m_fAlpha -= fTimeDelta;

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_fAlpha = 0.f;
		}
	}

	for (_int i = 0; i < m_iSlotCount; ++i)
	{
		if (m_bEquipped[i] == true)
		{
			m_bSpellUsed[i] = m_pInfoInstance->Get_Spell_Info(m_Spells[i]).bUse_Skill;
			m_fCoolTime = m_pInfoInstance->Get_CoolTime(m_Spells[m_iCurrent_Slot]);
		}
	}

	if (m_pGameInstance->Key_Down(DIK_N))
	{
		Nocool = true;
		m_pInfoInstance->Event_CallBack(TEXT("NoCooL"),&Nocool);
	}
	if (m_pGameInstance->Key_Down(DIK_M))
	{
		Nocool = false;
		m_pInfoInstance->Event_CallBack(TEXT("NoCooL"),&Nocool);
	}

	if (m_pGameInstance->Key_Down(DIK_N))
	{
		Nocool = true;
		m_pInfoInstance->Event_CallBack(TEXT("NoCooL"), &Nocool);
	}
	if (m_pGameInstance->Key_Down(DIK_M))
	{
		Nocool = false;
		m_pInfoInstance->Event_CallBack(TEXT("NoCooL"), &Nocool);
	}

	m_vUVScale.y = m_fCoolTime;

	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);

}

void CSpell_Overlay::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		if (m_bEquipped[m_iCurrent_Slot] == true)
		{
			m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
		}
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CSpell_Overlay::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::TYPE))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

_vector CSpell_Overlay::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CSpell_Overlay::Bind_ShaderResources()
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
	if (FAILED(m_pDiffuse_TextureCom1->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom2->Bind_ShaderResource(m_pShaderCom, "g_Texture2", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom3->Bind_ShaderResource(m_pShaderCom, "g_Texture3", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFrame", &m_fFrame, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAngle", &m_fAngle, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iSpellType", &m_iSpellType[m_iCurrent_Slot], sizeof(_int))))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iImageCountX", &m_iImageFrameX, sizeof(_int))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iImageCountY", &m_iImageFrameY, sizeof(_int))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDeltaU", &m_vUVScale.x, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDeltaV", &m_vUVScale.y, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCoolTime", &m_fCoolTime, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageUV", &m_vUV[m_iCurrent_Slot], sizeof(_float4))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSpell_Overlay::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_ActionItemBack_4K"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_spellmeter_Generic"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Atlas_Spell"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_tillingSmokes"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom3), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSpell_Overlay* CSpell_Overlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpell_Overlay* pInstance = new CSpell_Overlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpell_Overlay");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpell_Overlay::Clone(void* pArg, CGameObject* pOwner)
{
	CSpell_Overlay* pInstance = new CSpell_Overlay(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Overlay");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpell_Overlay::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pDiffuse_TextureCom3);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CSpell_Overlay::Describe_Entity()
{
}
#endif // _DEBUG
