#include "pch.h"
#include "Dialogue.h"
#include "GameInstance.h"

CDialogue::CDialogue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CDialogue::CDialogue(const CDialogue& rhs)
	:CElementObject(rhs)
{
}

HRESULT CDialogue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDialogue::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 960.f;
	Desc.fY = 950.f;
	Desc.fSizeX = 74.f;
	Desc.fSizeY = 74.f;

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
	m_fOwnerAlpha = 1.f;
	m_fCanvasAlpha = 1.f;
	m_fAlphaTime = 5.f;
	SizeUpY(80);
	m_fFontX = 0.f;
	m_fFontY = -10.f;
	m_fFontSize.x = 0.f;
	m_fFontSize.y = -10.f;
	m_Name = TEXT("Visual Studio");
	m_pText = TEXT("아바다 케다브라");
	SizeUpdate(m_Name, m_pText);
	Visible(true);
	return S_OK;
}

void CDialogue::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CDialogue::Update(_float fTimeDelta)
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
			m_bHover = true;
			m_fAlpha = 1.f;
		}
	}

	if (m_bFadeOut == true)
	{
		if (m_fAlpha >= 0.f)
		{
			m_fAlpha -= fTimeDelta * m_fAlphaTime;
			m_bHover = false;
		}

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_fAlpha = 0.f;
		}
	}
	m_fTime += fTimeDelta * m_fTimeMult;

	if (m_pGameInstance->Key_Down(DIK_L))
	{
		m_Name = TEXT("Visual");
		m_pText = TEXT("아아아 케다브라");
		SizeUpdate(m_Name, m_pText);
	}

	m_vScale = _float3(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - m_fWinSizeX * 0.5f, -m_fY + m_fWinSizeY * 0.5f, m_fSortZ, 1.f));

	//__super::Update(fTimeDelta);
}

void CDialogue::Late_Update(_float fTimeDelta)
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

HRESULT CDialogue::Render()
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

	m_pGameInstance->Render_Text(TEXT("Font_size15"), m_FinalName.c_str(), _float2(m_fFontX + m_fX, m_fFontY + m_fY), XMVectorSet((221.f / 255.f) * m_fAlpha, (201.f / 255.f) * m_fAlpha, (98.f / 255.f) * m_fAlpha, m_fAlpha));
	m_pGameInstance->Render_Text(TEXT("Font_size15"), m_pText.c_str(), _float2(m_fFontSize.x + m_fX + (m_fFontSizeOffset1 + 5.f), m_fFontSize.y + m_fY), XMVectorSet(1.f * m_fAlpha, 1.f * m_fAlpha, 1.f * m_fAlpha, m_fAlpha));
	return S_OK;
}

_vector CDialogue::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CDialogue::SizeUpX(_float fSizeX)
{
	_float fX = (fSizeX - m_vScale.x) * 0.5f;
	m_fSizeX = fSizeX;
	//m_fX += fX;
	m_fLerpX += fX;
	m_fFontX -= fX;
	m_fFontSize.x -= fX;
}

void CDialogue::SizeUpY(_float fSizeY)
{
	m_fSizeY = fSizeY;
	//m_fY = m_fTopY - fSizeY * 0.5f;
}

void CDialogue::SizeUpdate(_wstring pName, _wstring pText)
{
	m_FinalName = pName + TEXT(":");
	m_pText = pText;
	m_fFontSizeOffset1 = m_pGameInstance->FontSizeX(TEXT("Font_size15"), m_FinalName.c_str());
	m_fFontSizeOffset2 = m_pGameInstance->FontSizeX(TEXT("Font_size15"), m_pText.c_str());
	SizeUpX(m_fFontSizeOffset1 + m_fFontSizeOffset2 + 80.f);
}

HRESULT CDialogue::Bind_ShaderResources()
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

HRESULT CDialogue::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_SpellWidgetPanelBack"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CDialogue* CDialogue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDialogue* pInstance = new CDialogue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDialogue");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDialogue::Clone(void* pArg, CGameObject* pOwner)
{
	CDialogue* pInstance = new CDialogue(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDialogue");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDialogue::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CDialogue::Describe_Entity()
{
}
