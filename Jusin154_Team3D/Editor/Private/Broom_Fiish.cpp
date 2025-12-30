#include "pch.h"
#include "Broom_Fiish.h"
#include "GameInstance.h"

CBroom_Finish::CBroom_Finish(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CBroom_Finish::CBroom_Finish(const CBroom_Finish& rhs)
	:CElementObject(rhs)
{
}

HRESULT CBroom_Finish::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom_Finish::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = -140.f;
	Desc.fSizeX = 1920.f;
	Desc.fSizeY = 1080.f;

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
	return S_OK;
}

void CBroom_Finish::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CBroom_Finish::Update(_float fTimeDelta)
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
	Set_Font();

	m_fTime += fTimeDelta * m_fTimeMult;

	__super::Update(fTimeDelta);
}

void CBroom_Finish::Late_Update(_float fTimeDelta)
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

HRESULT CBroom_Finish::Render()
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

	m_pGameInstance->Render_Text(TEXT("Font_size20"), m_fFont[0].c_str(), m_fFontPos[0]);
	m_pGameInstance->Render_Text(TEXT("Font_size20"), m_fFont[1].c_str(), m_fFontPos[1]);
	m_pGameInstance->Render_Text(TEXT("Font_size20"), m_fFont[2].c_str(), m_fFontPos[2]);
	m_pGameInstance->Render_Text(TEXT("Font_size20"), m_fFont[3].c_str(), m_fFontPos[3]);
	m_pGameInstance->Render_Text(TEXT("Font_size20"), m_fFont[4].c_str(), m_fFontPos[4]);
	m_pGameInstance->Render_Text(TEXT("Font_size20"), m_fFont[5].c_str(), m_fFontPos[5]);
	m_pGameInstance->Render_Text(TEXT("Font_size20"), m_fFont[6].c_str(), m_fFontPos[6]);


	return S_OK;
}

_vector CBroom_Finish::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CBroom_Finish::Bind_ShaderResources()
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

void CBroom_Finish::Set_Font()
{
	m_fFont[0] = TEXT("경주 시간");
	m_fFont[1] = TEXT("0:00:00");
	m_fFont[2] = TEXT("놓친 고리로 인한 패널티 X ");
	m_fFont[3] = TEXT("0");
	m_fFont[4] = TEXT("0:00:00");
	m_fFont[5] = TEXT("총 시간");
	m_fFont[6] = TEXT("0:00:00");

	m_fFontPos[0] = _float2(m_fX + 700, m_fY + 600);
	m_fFontPos[1] = _float2(m_fX + 1100, m_fY + 600);
	m_fFontPos[2] = _float2(m_fX + 700, m_fY + 640);
	m_fFontPos[3] = _float2(m_fX + 1020, m_fY + 640);
	m_fFontPos[4] = _float2(m_fX + 1100, m_fY + 640);
	m_fFontPos[5] = _float2(m_fX + 700, m_fY + 700);
	m_fFontPos[6] = _float2(m_fX + 1100, m_fY + 700);
}

HRESULT CBroom_Finish::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_Finish"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CBroom_Finish* CBroom_Finish::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_Finish* pInstance = new CBroom_Finish(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBroom_Finish");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CBroom_Finish::Clone(void* pArg, CGameObject* pOwner)
{
	CBroom_Finish* pInstance = new CBroom_Finish(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroom_Finish");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBroom_Finish::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CBroom_Finish::Describe_Entity()
{
}
