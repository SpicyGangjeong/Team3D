#include "pch.h"
#include "Logo_Glow.h"
#include "GameInstance.h"

CLogo_Glow::CLogo_Glow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject(pDevice, pContext)
{
}

CLogo_Glow::CLogo_Glow(const CLogo_Glow& rhs)
	:CUIObject(rhs)
{
}

HRESULT CLogo_Glow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogo_Glow::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 470.f;
	Desc.fY = 385.f;
	Desc.fSizeX = 256.f;
	Desc.fSizeY = 128.f;

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_fOwnerAlpha = 1.f;
	m_fCanvasAlpha = 1.f;
	m_fAlpha = 0.f;
	m_bHover = false;
	return S_OK;
}

void CLogo_Glow::Priority_Update(_float fTimeDelta)
{
}

void CLogo_Glow::Update(_float fTimeDelta)
{
	if (m_bHover == false)
	{
		m_fTime += fTimeDelta;
		if (m_fTime >= 0.7f)
		{
			m_bHover = true;
		}
	}
	else
	{
		if (m_fAlpha <= 1.f)
		{
			m_fAlpha += fTimeDelta;
		}
	}
}

void CLogo_Glow::Late_Update(_float fTimeDelta)
{
	if (m_bHover == true)
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CLogo_Glow::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::LOGO_GLOW)))) {
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

_vector CLogo_Glow::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CLogo_Glow::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLogo_Glow::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom))) {
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_NavBarHighlightGlow"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_WhiteStarBurst"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CLogo_Glow* CLogo_Glow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLogo_Glow* pInstance = new CLogo_Glow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLogo_Glow");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CLogo_Glow::Clone(void* pArg, CGameObject* pOwner)
{
	CLogo_Glow* pInstance = new CLogo_Glow(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLogo_Glow");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLogo_Glow::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CLogo_Glow::Describe_Entity()
{
}
