#include "pch.h"
#include "Dialogue_Choice.h"
#include "GameInstance.h"

CDialogue_Choice::CDialogue_Choice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CDialogue_Choice::CDialogue_Choice(const CDialogue_Choice& rhs)
	:CElementObject(rhs)
{
}

HRESULT CDialogue_Choice::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDialogue_Choice::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = -100.f;
	Desc.fY = -200.f;
	Desc.fSizeX = 100.f;
	Desc.fSizeY = 65.f;

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
	m_fAlphaTime = 3.f;
	m_vNine_Slice = _float4(20.f, 100.f, 0.f, 65.f);
	m_vImagePosi1 = _float4(0.f, 0.f, 90.f, 20.f);
	m_vImagePosi2 = _float4(0.f, 45.f, 90.f, 20.f);
	m_vImagePosi3 = _float4(15.f, 12.5f, 40.f, 40.f);
	m_fFontX = 370.f;
	m_fFontY = 607.5f;
	m_iHover = 0;
	SizeUpX(400.f);
	return S_OK;
}

void CDialogue_Choice::Mouse_Hover()
{
	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse); // 윈도우 왼쪽 상단 (0,0) 기준

	// 윈도우 실제 내부 크기를 '직접' 가져옵니다. (g_iWinSize 대신)
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	_float fWidth = (_float)(rc.right - rc.left);
	_float fHeight = (_float)(rc.bottom - rc.top);

	_float2 fMouse;
	// 캔버스 배치 로직(Update)과 마우스 변환 로직을 완벽하게 일치시킵니다.
	fMouse.x = (_float)ptMouse.x - (fWidth * 0.5f);
	fMouse.y = -((_float)ptMouse.y - (fHeight * 0.494f)); // Y축 뒤집기

	POINT pt = { (long)fMouse.x, (long)fMouse.y };

	Hover(PtInRect(&m_pRect, pt));
}

void CDialogue_Choice::Hover(_bool iHover)
{
	m_iHover = iHover;
	if (m_iHover != 0)
	{
		m_fFontX = 390.f;
		m_vImagePosi3 = _float4(35.f, 12.5f, 40.f, 40.f);
	}
	else
	{
		m_fFontX = 370.f;
		m_vImagePosi3 = _float4(15.f, 12.5f, 40.f, 40.f);
	}
}

void CDialogue_Choice::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CDialogue_Choice::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_bFadeIn == true)
	{
		if (m_fAlpha <= 7.f)
			m_fAlpha += fTimeDelta * m_fAlphaTime;

		if (m_fAlpha >= 7.f)
		{
			m_bFadeIn = false;
			m_fAlpha = 7.f;
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

	Mouse_Hover();

	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}

void CDialogue_Choice::Late_Update(_float fTimeDelta)
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

HRESULT CDialogue_Choice::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::DIALOGUECHOICE)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	m_pGameInstance->Render_Text(TEXT("Font_size20"), TEXT("아아"), _float2(m_fFontX + m_fX, m_fFontY - m_fY), XMVectorSet((208.f / 255.f) * m_fAlpha, (177.f / 255.f) * m_fAlpha, (52.f / 255.f) * m_fAlpha, m_fAlpha));

	return S_OK;
}

_vector CDialogue_Choice::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CDialogue_Choice::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOrigin_Size", &m_fOrigin_Size, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_vScale, sizeof(_float2))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fNine_Slice", &m_vNine_Slice, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos1", &m_vImagePosi1, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos2", &m_vImagePosi2, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos3", &m_vImagePosi3, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iHover", &m_iHover, sizeof(_int))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDialogue_Choice::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_Dialogue_Chice"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_InteractSelectionBracket"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_Icon_InteractType_Straight"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CDialogue_Choice* CDialogue_Choice::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDialogue_Choice* pInstance = new CDialogue_Choice(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDialogue_Choice");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDialogue_Choice::Clone(void* pArg, CGameObject* pOwner)
{
	CDialogue_Choice* pInstance = new CDialogue_Choice(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDialogue_Choice");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDialogue_Choice::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CDialogue_Choice::Describe_Entity()
{
}
