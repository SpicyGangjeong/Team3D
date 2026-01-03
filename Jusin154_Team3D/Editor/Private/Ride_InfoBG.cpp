#include "pch.h"
#include "Ride_InfoBG.h"
#include "GameInstance.h"

CRide_InfoBG::CRide_InfoBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CElementObject(pDevice, pContext)
{
}

CRide_InfoBG::CRide_InfoBG(const CRide_InfoBG& rhs)
    :CElementObject(rhs)
{
}

HRESULT CRide_InfoBG::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRide_InfoBG::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 80.f;
	Desc.fY = 100.f;
	Desc.fSizeX = 400.f;
	Desc.fSizeY = 370.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_fTimeMult = 0.6f;
	m_fAlpha = 1.f;
	m_fAlphaTime = 10.f;
	return S_OK;
}

void CRide_InfoBG::Set_Hover(_bool bVisible)
{
	Visible(bVisible);
	m_bHover = true;
	m_fTime = 0.f;
	m_fAlpha = 1.f;
}

void CRide_InfoBG::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CRide_InfoBG::Update(_float fTimeDelta)
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
			Visible(false);
		}
	}

	if (m_bHover == true)
		m_fTime += fTimeDelta * m_fTimeMult;

	if (m_fTime >= 1.f)
	{
		Set_FadeOut();
	}

	__super::Update(fTimeDelta);
}

void CRide_InfoBG::Late_Update(_float fTimeDelta)
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

HRESULT CRide_InfoBG::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::DEFAULT)))) {
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
_vector CRide_InfoBG::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CRide_InfoBG::Bind_ShaderResources()
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

HRESULT CRide_InfoBG::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_DropShadow"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CRide_InfoBG* CRide_InfoBG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRide_InfoBG* pInstance = new CRide_InfoBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRide_InfoBG");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CRide_InfoBG::Clone(void* pArg, CGameObject* pOwner)
{
	CRide_InfoBG* pInstance = new CRide_InfoBG(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRide_InfoBG");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CRide_InfoBG::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CRide_InfoBG::Describe_Entity()
{
}
