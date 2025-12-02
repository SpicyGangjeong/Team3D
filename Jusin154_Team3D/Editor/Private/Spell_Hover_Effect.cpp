#include "pch.h"
#include "Spell_Hover_Effect.h"
#include "GameInstance.h"

CSpell_Hover_Effect::CSpell_Hover_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSpell_Hover_Effect::CSpell_Hover_Effect(const CSpell_Hover_Effect& rhs)
	:CElementObject(rhs)
{
}

HRESULT CSpell_Hover_Effect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpell_Hover_Effect::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 0.f;
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

	m_fAlpha = 1.f;
	m_fTimeMult = 1.f;
	m_fAlphaTime = 1.f;
	m_fOffSetX = 120.f;
	m_fOffSetY = 122.f;
	m_iCols = 4;
	m_fSortZ = 0.02f;
	m_pVIBufferCom->Set_Cloned(true);
	m_pVIBufferCom->Set_Pos(-185.f, 375.f, m_fOffSetX, m_fOffSetY, m_iCols);
	m_pVIBufferCom->Set_Size(m_fSizeX, m_fSizeY);
	return S_OK;
}

void CSpell_Hover_Effect::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpell_Hover_Effect::Update(_float fTimeDelta)
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
	Hover();
	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}

void CSpell_Hover_Effect::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CSpell_Hover_Effect::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIINTANCE::SPELL_SLOT_EFFECT))))
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

_vector CSpell_Hover_Effect::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CSpell_Hover_Effect::SizeUpX(_float fSizeX)
{
	m_fSizeX = fSizeX;
	m_pVIBufferCom->Set_SizeX(m_fSizeX);
}

void CSpell_Hover_Effect::SizeUpY(_float fSizeY)
{
	m_fSizeY = fSizeY;
	m_pVIBufferCom->Set_SizeY(m_fSizeY);
}

void CSpell_Hover_Effect::SizeUpdate(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	m_pVIBufferCom->Set_Size(m_fSizeX, m_fSizeY);
}

void CSpell_Hover_Effect::Hover()
{
	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_float2 fMouse;
	fMouse.x = ptMouse.x - (g_iWinSizeX * 0.5f);
	fMouse.y = -(ptMouse.y - (g_iWinSizeY * 0.5f));

	fMouse.x -= m_pOwner->Get_WorldPostion().m128_f32[0];
	fMouse.y -= m_pOwner->Get_WorldPostion().m128_f32[1];

	m_pVIBufferCom->Set_Mouse_Hover(fMouse);

}

HRESULT CSpell_Hover_Effect::Bind_ShaderResources()
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
	if (FAILED(m_pDiffuse_TextureCom1->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float))))
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


HRESULT CSpell_Hover_Effect::Ready_Components(void* pArg)
{
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Spell_List"), (CComponent**)&m_pVIBufferCom, nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_SelectedBorderMask"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_VFX_T_WhispLine_D"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIINSTANCE, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSpell_Hover_Effect* CSpell_Hover_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpell_Hover_Effect* pInstance = new CSpell_Hover_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpell_Hover_Effect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpell_Hover_Effect::Clone(void* pArg, CGameObject* pOwner)
{
	CSpell_Hover_Effect* pInstance = new CSpell_Hover_Effect(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Hover_Effect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpell_Hover_Effect::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CSpell_Hover_Effect::Describe_Entity()
{
}
