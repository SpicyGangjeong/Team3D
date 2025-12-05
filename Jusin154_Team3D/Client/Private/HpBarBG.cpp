#include "pch.h"
#include "HpBarBG.h"
#include "GameInstance.h"

CHpBarBG::CHpBarBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CHpBarBG::CHpBarBG(const CHpBarBG& rhs)
	:CElementObject(rhs)
{
}

HRESULT CHpBarBG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHpBarBG::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 600.f;
	Desc.fY = 180.f;
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

	m_fTimeMult = 5.f;
	m_fAlpha = 1.f;
	m_fAlphaTime = 1.f;
	m_vNine_Slice = _float4(27.f, 125.f, m_fSizeY * 0.5f, m_fSizeY * 0.5f);
	m_fMaxHp = 1000.f;
	m_fCurrentHp = m_fMaxHp;
	m_fMoveSpeed = 5.f;
	m_fHpBGSize = _float2(144.f, 24.f);
	m_fHpBGPos = _float2(0.f, 13.f);
	m_fHpBG = _float2(0, m_fSizeX);
	SizeUpX(240.f);
	return S_OK;
}

void CHpBarBG::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CHpBarBG::Update(_float fTimeDelta)
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

	if (m_fCurrentHp < 0.f)
		m_fCurrentHp = 0.f;
	if (m_fDamage <= 0.f)
		m_fDamage = 0.f;
	m_fTargetHp = m_fMaxHp - m_fDamage;

	if (m_fTargetHp > m_fCurrentHp)
		Heal(fTimeDelta);
	else if (m_fTargetHp < m_fCurrentHp)
		Hit(fTimeDelta);
	m_fHpBar = m_fCurrentHp / m_fMaxHp;

	if (m_pGameInstance->Key_Down(DIK_3))
	{
		Lerp_PosX(100.f);
	}

	if (m_pGameInstance->Key_Down(DIK_4))
	{
		Lerp_PosX(-101.f);
	}
	if (m_fHpBar <= 0.3f)
	{
		m_fBlinkTime += fTimeDelta * m_fTimeMult;
		if (m_fTime <= 1.f)
		{
			m_fTime += fTimeDelta * m_fTimeMult;
		}
	}
	else
	{
		m_fBlinkTime = 0.f;
		m_fTime = 0.f;
	}
	__super::Update(fTimeDelta);
}

void CHpBarBG::Late_Update(_float fTimeDelta)
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

HRESULT CHpBarBG::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::HPBAR)))) {
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

_vector CHpBarBG::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CHpBarBG::Lerp_PosX(_float X)
{
	m_fDamage += X;
}

void CHpBarBG::Heal(_float fTimeDelta)
{
	m_fCurrentHp = CMyTools::Lerp_f1D(m_fTargetHp, m_fCurrentHp, fTimeDelta * m_fMoveSpeed);
}

void CHpBarBG::Hit(_float fTimeDelta)
{
	m_fCurrentHp = CMyTools::Lerp_f1D(m_fCurrentHp, m_fTargetHp, fTimeDelta * m_fMoveSpeed);
}

void CHpBarBG::SizeUpX(_float fSizeX)
{
	m_fSizeX = fSizeX;
	m_fX -= (fSizeX - m_vScale.x) * 0.5f;
}

HRESULT CHpBarBG::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHp", &m_fHpBar, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHpBG", &m_fHpBG, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSize", &m_fHpBGSize, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Pos", &m_fHpBGPos, sizeof(_float2))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CHpBarBG::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_GlowBar"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_HpBarBG"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_HUD_HealthMeterFill"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CHpBarBG* CHpBarBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHpBarBG* pInstance = new CHpBarBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHpBarBG");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CHpBarBG::Clone(void* pArg, CGameObject* pOwner)
{
	CHpBarBG* pInstance = new CHpBarBG(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Image");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CHpBarBG::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CHpBarBG::Describe_Entity()
{
}
#endif // _DEBUG
