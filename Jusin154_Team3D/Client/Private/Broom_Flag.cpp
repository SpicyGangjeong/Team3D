#include "pch.h"
#include "Broom_Flag.h"
#include "GameInstance.h"

CBroom_Flag::CBroom_Flag(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CBroom_Flag::CBroom_Flag(const CBroom_Flag& rhs)
	:CElementObject(rhs)
{
}

HRESULT CBroom_Flag::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom_Flag::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 100.f;
	Desc.fSizeX = 437.5f;
	Desc.fSizeY = 160.f;

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
	m_fAlpha = 1.f;
	m_fAlphaTime = 10.f;
	m_fStart = 500.f;
	return S_OK;
}

void CBroom_Flag::Set_Start()
{
	m_fSizeX = m_fOrigin_Size.x;
	m_fAlpha = 1.f;
}

void CBroom_Flag::Set_Finish()
{
	Visible(true);
	m_fFinish = true;
}

void CBroom_Flag::Rece_Results()
{
	Visible(true);
}

void CBroom_Flag::Finish_SizeUp(_float fTime)
{
	
}

void CBroom_Flag::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CBroom_Flag::Update(_float fTimeDelta)
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

	if (m_bHover == true)
	{
		m_fSizeX -= fTimeDelta * m_fStart;
		m_fAlpha -= fTimeDelta * m_fAlphaTime;
	}

	if (m_fAlpha <= 0.f)
	{
		m_bHover = false;
		m_fAlpha = 0.f;
		Visible(false);
	}

	if (m_fFinish == true)
	{

	}

	m_fTime += fTimeDelta * m_fTimeMult;

	__super::Update(fTimeDelta);
}

void CBroom_Flag::Late_Update(_float fTimeDelta)
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

HRESULT CBroom_Flag::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::FLAGWAVE)))) {
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

_vector CBroom_Flag::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CBroom_Flag::Bind_ShaderResources()
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

HRESULT CBroom_Flag::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_BRFlag"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CBroom_Flag* CBroom_Flag::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_Flag* pInstance = new CBroom_Flag(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBroom_Flag");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CBroom_Flag::Clone(void* pArg, CGameObject* pOwner)
{
	CBroom_Flag* pInstance = new CBroom_Flag(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroom_Flag");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBroom_Flag::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CBroom_Flag::Describe_Entity()
{
}
#endif // _DEBUG
