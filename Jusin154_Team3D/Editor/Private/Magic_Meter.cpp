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

	Desc.fX = 130.f;
	Desc.fY = 145.f;
	Desc.fSizeX = 80.f;
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
	m_fAngle = XMConvertToRadians(45);
	m_fAlphaTime = 1.f;
	m_iCols = 5;
	UV();
	m_pVIBufferCom->Set_Cloned(true);
	m_pVIBufferCom->Set_Pos(0.f, 0.f, -65.f, m_fSizeY, m_iCols);
	m_pVIBufferCom->Set_Size(m_fSizeX, m_fSizeY);
	m_pVIBufferCom->Set_ImageUV(pUVDesc);
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
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIINTANCE::MAGIC_METER))))
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

void CMagic_Meter::UV()
{
	pUVDesc[0].fUVStart = Compute_UVX(1);
	pUVDesc[0].fUVEnd = Compute_UVY(1);
	pUVDesc[1].fUVStart = Compute_UVX(2);
	pUVDesc[1].fUVEnd = Compute_UVY(2);
	pUVDesc[2].fUVStart = Compute_UVX(2);
	pUVDesc[2].fUVEnd = Compute_UVY(2);
	pUVDesc[3].fUVStart = Compute_UVX(2);
	pUVDesc[3].fUVEnd = Compute_UVY(2);
	pUVDesc[4].fUVStart = Compute_UVX(0);
	pUVDesc[4].fUVEnd = Compute_UVY(0);
}

_float2 CMagic_Meter::Compute_UVX(_uint Number)
{
	m_fIamge_Size = { 128, 96 };

	_uint iXCount = 1;
	_uint iYCount = 3;

	_float frameWidth = 128.f;
	_float frameHeight = 32.f;

	_uint frameX = Number % iXCount;
	_uint frameY = Number / iXCount;

	_float2 UVStart;
	UVStart.x = frameX * frameWidth / m_fIamge_Size.x;
	UVStart.y = frameY * frameHeight / m_fIamge_Size.y;

	return UVStart;
}

_float2 CMagic_Meter::Compute_UVY(_uint Number)
{
	m_fIamge_Size = { 320.f, 128.f };

	_float frameWidth = 128.f;
	_float frameHeight = 32.f;

	// Start 구함
	_float2 UVStart = Compute_UVX(Number);

	_float2 UVEnd;
	UVEnd.x = UVStart.x + (frameWidth / m_fIamge_Size.x);
	UVEnd.y = UVStart.y + (frameHeight / m_fIamge_Size.y);

	return UVEnd;
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
	if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
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

	return S_OK;
}

HRESULT CMagic_Meter::Ready_Components(void* pArg)
{
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Magic_Meter_UI_Instance"), (CComponent**)&m_pVIBufferCom, nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_FinisherMeterOutline"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_HUD_HealthMeterFill"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIINSTANCE, (CComponent**)&m_pShaderCom, nullptr)))
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

void CMagic_Meter::Describe_Entity()
{
}
