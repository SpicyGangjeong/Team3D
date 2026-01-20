#include "pch.h"
#include "Magic_Meter.h"
#include "GameInstance.h"

CMagic_Meter::CMagic_Meter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CMagic_Meter::CMagic_Meter(const CMagic_Meter& rhs)
	:CElementObject(rhs)
{
}

HRESULT CMagic_Meter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMagic_Meter::Initialize(void* pArg)
{

	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 190.f;
	Desc.fY = -145.f;
	Desc.fSizeX = 70.f;
	Desc.fSizeY = 20.f;

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
	m_fAlphaTime = 1.f;
	m_fMaxGauge = 15.f;
	m_fCurrentGauge = 15.f;
	return S_OK;
}

void CMagic_Meter::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CMagic_Meter::Update(_float fTimeDelta)
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
	if (m_fCurrentGauge < 0.f)
		m_fCurrentGauge = 0.f;

	//if (m_fDamage <= 0.f)
	//	m_fDamage = 0.f;
	//m_fTargetHp = m_fMaxHp - m_fDamage;

	m_fGaugeBar = m_fCurrentGauge / m_fMaxGauge;

	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}


void CMagic_Meter::Late_Update(_float fTimeDelta)
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

HRESULT CMagic_Meter::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::MAGIC_METER))))
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


_vector CMagic_Meter::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CMagic_Meter::Meter_Index(_uint Number)
{
	m_iImageCount = Number;
}

_float CMagic_Meter::Charge_Meter()
{
	_float NewCurrentGauge{};
	m_fCurrentGauge++;

	if (m_fMaxGauge < m_fCurrentGauge)
	{
		NewCurrentGauge = m_fCurrentGauge - m_fMaxGauge;
		m_fCurrentGauge = m_fMaxGauge;
	}

	return NewCurrentGauge;
}

_float CMagic_Meter::Get_Meter()
{
	return m_fCurrentGauge;
}

_float CMagic_Meter::Use_Meter(_float Meter)
{
	if (Meter >= m_fCurrentGauge)
	{
		Meter -= m_fCurrentGauge;
		m_fCurrentGauge = 0.f;
		return Meter;
	}
	else
	{
		m_fCurrentGauge -= Meter;
		return 0;
	}
}

HRESULT CMagic_Meter::Bind_ShaderResources()
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
	if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iImageCount)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom1->Bind_ShaderResource(m_pShaderCom, "g_Texture1", 0)))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHp", &m_fGaugeBar, sizeof(_float))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMagic_Meter::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Megic_Metar"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_HUD_HealthMeterFill"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}


CMagic_Meter* CMagic_Meter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMagic_Meter* pInstance = new CMagic_Meter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMagic_Meter");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMagic_Meter::Clone(void* pArg, CGameObject* pOwner)
{
	CMagic_Meter* pInstance = new CMagic_Meter(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Slot");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMagic_Meter::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CMagic_Meter::Describe_Entity()
{
}
#endif // _DEBUG
