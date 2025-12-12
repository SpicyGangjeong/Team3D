#include "pch.h"
#include "Spell_Vidio_Border.h"
#include "GameInstance.h"

CSpell_Vidio_Border::CSpell_Vidio_Border(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSpell_Vidio_Border::CSpell_Vidio_Border(const CSpell_Vidio_Border& rhs)
	:CElementObject(rhs)
{
}

HRESULT CSpell_Vidio_Border::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpell_Vidio_Border::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 590.f;
	Desc.fY = -100.f;
	Desc.fSizeX = 520.f;
	Desc.fSizeY = 275.f;

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
	m_fAlphaTime = 9.f;
	m_fMoveSpeed = 5.f;
	m_fLerpX = m_fX;
	m_fLerpY = 45;
	m_bStart = false;
	m_fPreviewOffSet = 0;
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("Slot_Hover"), [this](void* p) {this->Set_SkillType(*reinterpret_cast<_int*>(p)); });
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("FadeIn"), [this](void* p) {this->Set_FadeIn(); });
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("FadeOut"), [this](void* p) {this->Set_FadeOut(); });
	return S_OK;
}

void CSpell_Vidio_Border::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpell_Vidio_Border::Update(_float fTimeDelta)
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
			m_fAlpha -= fTimeDelta * m_fAlphaTime;

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_bStart = false;
			m_fAlpha = 0.f;
		}
	}

	m_fTime += fTimeDelta * m_fTimeMult;

	if (m_fAlpha >= 0.3f)
		m_bStart = true;

	if (m_iSpellType != -1)
	{
		m_fY = m_fOrigin_Position.y + static_cast<CUIObject*>(m_pOwner)->Get_Info(m_iSpellType).fVidio;
	}

	__super::Update(fTimeDelta);

}

void CSpell_Vidio_Border::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CSpell_Vidio_Border::Render()
{
	if (m_bStart == true)
	{
		if (FAILED(Bind_ShaderResources()))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::ALPHABLEND))))
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

	}

	return S_OK;
}

_vector CSpell_Vidio_Border::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CSpell_Vidio_Border::Bind_ShaderResources()
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
	return S_OK;
}

HRESULT CSpell_Vidio_Border::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_TalentVideoBorder"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CSpell_Vidio_Border* CSpell_Vidio_Border::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpell_Vidio_Border* pInstance = new CSpell_Vidio_Border(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpell_Vidio_Border");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpell_Vidio_Border::Clone(void* pArg, CGameObject* pOwner)
{
	CSpell_Vidio_Border* pInstance = new CSpell_Vidio_Border(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Vidio_Border");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpell_Vidio_Border::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CSpell_Vidio_Border::Describe_Entity()
{
}
