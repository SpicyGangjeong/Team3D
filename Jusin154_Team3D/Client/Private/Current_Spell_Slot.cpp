#include "pch.h"
#include "Current_Spell_Slot.h"
#include "GameInstance.h"

CCurrent_Spell_Slot::CCurrent_Spell_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CElementObject(pDevice, pContext)
{
}

CCurrent_Spell_Slot::CCurrent_Spell_Slot(const CCurrent_Spell_Slot& rhs)
    :CElementObject(rhs)
{
}

HRESULT CCurrent_Spell_Slot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCurrent_Spell_Slot::Initialize(void* pArg)
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
	m_fTimeMult = 3.f;
	m_fAngle = XMConvertToRadians(-135);
	m_fAlphaTime = 1.f;
	m_fOffSetX = 105.f;
	m_fOffSetY = 150.f;
	m_iCols = 4;
	m_pVIBufferCom->Set_Cloned(true);
	m_pVIBufferCom->Set_Pos(-780.f, 340.f, m_fOffSetX, m_fOffSetY, m_iCols);
	m_pVIBufferCom->Set_Size(m_fSizeX, m_fSizeY);
	return S_OK;
}

void CCurrent_Spell_Slot::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CCurrent_Spell_Slot::Update(_float fTimeDelta)
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
	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}

void CCurrent_Spell_Slot::Late_Update(_float fTimeDelta)
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

HRESULT CCurrent_Spell_Slot::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIINTANCE::DEFAULT))))
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

_vector CCurrent_Spell_Slot::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CCurrent_Spell_Slot::SizeUpX(_float fSizeX)
{
	m_fSizeX = fSizeX;
	m_pVIBufferCom->Set_SizeX(m_fSizeX);
}

void CCurrent_Spell_Slot::SizeUpY(_float fSizeY)
{
	m_fSizeY = fSizeY;
	m_pVIBufferCom->Set_SizeY(m_fSizeY);
}

void CCurrent_Spell_Slot::SizeUpdate(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	m_pVIBufferCom->Set_Size(m_fSizeX, m_fSizeY);
}

HRESULT CCurrent_Spell_Slot::Bind_ShaderResources()
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

HRESULT CCurrent_Spell_Slot::Ready_Components(void* pArg)
{
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Current_Spell_Slot"), (CComponent**)&m_pVIBufferCom, nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_ActionItemGoldleaf_4K"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIINSTANCE, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CCurrent_Spell_Slot* CCurrent_Spell_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCurrent_Spell_Slot* pInstance = new CCurrent_Spell_Slot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCurrent_Spell_Slot");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CCurrent_Spell_Slot::Clone(void* pArg, CGameObject* pOwner)
{
	CCurrent_Spell_Slot* pInstance = new CCurrent_Spell_Slot(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Slot");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CCurrent_Spell_Slot::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CCurrent_Spell_Slot::Describe_Entity()
{
}
#endif // _DEBUG
