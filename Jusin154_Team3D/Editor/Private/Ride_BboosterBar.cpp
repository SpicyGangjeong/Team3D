#include "pch.h"
#include "Ride_BboosterBar.h"
#include "GameInstance.h"

CRide_BboosterBar::CRide_BboosterBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CRide_BboosterBar::CRide_BboosterBar(const CRide_BboosterBar& rhs)
	:CElementObject(rhs)
{
}

HRESULT CRide_BboosterBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRide_BboosterBar::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = -20.f;
	Desc.fY = -180.f;
	Desc.fSizeX = 144.f;
	Desc.fSizeY = 50.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_fTimeMult = 10.f;
	m_fAlpha = 1.f;
	m_fAlphaTime = 10.f;
	m_vNine_Slice = _float4(27.f, 125.f, m_fSizeY * 0.5f, m_fSizeY * 0.5f);
	m_fMaxGauge = 100.f;
	m_fCurrentGauge = m_fMaxGauge;
	m_fTargetGauge = m_fMaxGauge;
	m_fMoveSpeed = 5.f;
	m_fGaugeBGSize = _float2(144.f, 24.f);
	m_fGaugeBGPos = _float2(0.f, 13.f);
	m_fGaugeBG = _float2(0, m_fSizeX);
	SizeUpX(200.f);
	return S_OK;
}

void CRide_BboosterBar::Active(_float fTimeDelta)
{
	m_fCurrentGauge = CMyTools::Lerp_f1D(m_fCurrentGauge, m_fTargetGauge, fTimeDelta * m_fMoveSpeed);
}

void CRide_BboosterBar::NonActive(_float fTimeDelta)
{
	m_fCurrentGauge = CMyTools::Lerp_f1D(m_fTargetGauge, m_fCurrentGauge, fTimeDelta * m_fMoveSpeed);
}

void CRide_BboosterBar::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CRide_BboosterBar::Update(_float fTimeDelta)
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


	if (m_iClick == 1)
		Active(fTimeDelta);
	else
		NonActive(fTimeDelta);

	m_fGaugeBar = m_fCurrentGauge / m_fMaxGauge;

	if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		m_fTargetGauge -= fTimeDelta * m_fTimeMult;
		m_iClick = 1;
	}
	else
	{
		m_iClick = 0;
	}

	if (m_fTargetGauge <= 0.f)
		m_fTargetGauge = 0.f;
	if (m_fTargetGauge >= m_fMaxGauge)
		m_fTargetGauge = m_fMaxGauge;

	if (m_iClick == 0)
	{
		m_fTargetGauge += fTimeDelta * m_fTimeMult;
	}

	if (m_iClick == 1)
	{
		m_fBlinkTime += fTimeDelta;
		if (m_fTime <= 1.f)
		{
			m_fTime += fTimeDelta;
		}
	}
	else
	{
		m_fBlinkTime = 0.f;
		m_fTime = 0.f;
	}

	__super::Update(fTimeDelta);
}

void CRide_BboosterBar::Late_Update(_float fTimeDelta)
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

HRESULT CRide_BboosterBar::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::BOOSTERGAUGE)))) {
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

_vector CRide_BboosterBar::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CRide_BboosterBar::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBlinkTime", &m_fBlinkTime, sizeof(_float))))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNine_Slice", &m_vNine_Slice, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOrigin_Size", &m_fOrigin_Size, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_vScale, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHp", &m_fGaugeBar, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHpBG", &m_fGaugeBG, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSize", &m_fGaugeBGSize, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Pos", &m_fGaugeBGPos, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iClick", &m_iClick, sizeof(_int))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CRide_BboosterBar::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_GlowBar"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_HpBarBG"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_HUD_HealthMeterFill"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CRide_BboosterBar* CRide_BboosterBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRide_BboosterBar* pInstance = new CRide_BboosterBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRide_BboosterBar");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CRide_BboosterBar::Clone(void* pArg, CGameObject* pOwner)
{
	CRide_BboosterBar* pInstance = new CRide_BboosterBar(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRide_BboosterBar");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRide_BboosterBar::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CRide_BboosterBar::Describe_Entity()
{
}
