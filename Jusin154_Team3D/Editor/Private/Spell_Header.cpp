#include "pch.h"
#include "Spell_Header.h"
#include "GameInstance.h"

CSpell_Header::CSpell_Header(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSpell_Header::CSpell_Header(const CSpell_Header& rhs)
	:CElementObject(rhs)
{
}

HRESULT CSpell_Header::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpell_Header::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 600.f;
	Desc.fY = -385.f;
	Desc.fSizeX = 512.f;
	Desc.fSizeY = 80.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_fTimeMult = 3.f;
	m_fAlpha = 0.f;
	m_fAlphaTime = 5.f;
	m_fMoveSpeed = 5.f;
	m_fLerpX = m_fX;
	m_fLerpY = 45;
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("Slot_Hover"), [this](void* p) {this->Set_SkillType(*reinterpret_cast<_int*>(p)); });
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("FadeIn"), [this](void* p) {this->Set_FadeIn(); });
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("FadeOut"), [this](void* p) {this->Set_FadeOut(); });
	m_fFontX = 920.f;
	m_fFontY = 510.f;
	m_fType = _float2(935.f, 550.f);
	m_iPerSpellIndex = -1;
	return S_OK;
}

void CSpell_Header::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpell_Header::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bFadeIn == true)
	{
		if (m_fAlpha <= 1.f)
		{
			m_fAlpha += fTimeDelta * m_fAlphaTime;
			m_bHover = true;
		}

		if (m_fAlpha >= 1.f)
		{
			m_bFadeIn = false;
			m_fAlpha = 1.f;
		}
	}

	if (m_bFadeOut == true)
	{
		if (m_fAlpha >= 0.f)
		{
			m_fAlpha -= fTimeDelta * m_fAlphaTime;
		}

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_fAlpha = 0.f;
			m_bHover = false;
		}
	}

	if (m_iSpellType != -1 && m_iPerSpellIndex != m_iSpellType)
	{
		m_iSkillType = static_cast<CUIObject*>(m_pOwner)->Get_Info(m_iSpellType).iSpell_Type;
		m_pSpell_Name = static_cast<CUIObject*>(m_pOwner)->Get_Info(m_iSpellType).pSpell_Name;
		m_fSpell_Type = static_cast<CUIObject*>(m_pOwner)->Get_Info(m_iSpellType).pType_Name;
		m_iPerSpellIndex = m_iSpellType;
	}

	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}

void CSpell_Header::Late_Update(_float fTimeDelta)
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

HRESULT CSpell_Header::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::SPELL_HEADER)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}


	m_fFontOffSet = (m_pGameInstance->FontSizeX(TEXT("Font_size20"), m_pSpell_Name.c_str()) - 53.f) * 0.5f;
	m_pGameInstance->Render_Text(TEXT("Font_size20"), m_pSpell_Name.c_str(), _float2((m_fFontX + m_fX) - m_fFontOffSet, m_fFontY + m_fY), XMVectorSet((215.f / 255.f) * m_fAlpha, (185.f / 255.f) * m_fAlpha, (95.f / 255.f) * m_fAlpha, m_fAlpha));
	m_pGameInstance->Render_Text(TEXT("Font_size13"), m_fSpell_Type.c_str(), _float2(m_fType.x + m_fX, m_fType.y + m_fY), XMVectorSet(1.f * m_fAlpha, 1.f * m_fAlpha, 1.f * m_fAlpha, m_fAlpha));

	return S_OK;
}

_vector CSpell_Header::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CSpell_Header::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iSpellType", &m_iSkillType, sizeof(_int))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSpell_Header::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_LighthouseHeaderBack"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSpell_Header* CSpell_Header::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpell_Header* pInstance = new CSpell_Header(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpell_Header");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpell_Header::Clone(void* pArg, CGameObject* pOwner)
{
	CSpell_Header* pInstance = new CSpell_Header(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Header");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpell_Header::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CSpell_Header::Describe_Entity()
{
}
