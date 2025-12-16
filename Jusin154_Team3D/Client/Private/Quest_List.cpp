#include "pch.h"
#include "Quest_List.h"
#include "GameInstance.h"
#include "InfoInstance.h"

CQuest_List::CQuest_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CQuest_List::CQuest_List(const CQuest_List& rhs)
	:CElementObject(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CQuest_List::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQuest_List::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = -475.f;
	Desc.fY = -150.f;
	Desc.fSizeX = 256.f;
	Desc.fSizeY = 64.f;
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
	m_fAlphaTime = 5.f;
	m_vNine_Slice = _float4(70.f, 186.f, 0.f, 64.f);
	SizeUpX(495.f);
	SizeUpY(80.f);
	Visible(true);
	return S_OK;
}

void CQuest_List::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CQuest_List::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bFadeIn == true)
	{
		if (m_fAlpha <= 1.f)
		{
			m_fAlpha += fTimeDelta * m_fAlphaTime;
		}

		if (m_fAlpha >= 1.f)
		{
			m_bFadeIn = false;
			m_fAlpha = 1.f;
		}
	}

	if (m_bFadeOut == true)
	{
		if (m_fAlpha >= 0.f)
			m_fAlpha -= fTimeDelta * m_fAlphaTime;;

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_fAlpha = 0.f;
		}
	}

	Hover();
	if (m_bHover == true)
	{
		m_fSizeX = 505.f;
		m_fSizeY = 90.f;
		m_fFontX = 765.f;
		m_fFontY = 520.f;
		m_pFontSize = TEXT("Font_size21");
		m_pQuest_ID = 0;
		static_cast<CUIObject*>(m_pOwner)->Function_Callback(TEXT("QuestListHover"), &m_pQuest_ID);
	}
	else
	{
		m_fSizeX = 495.f;
		m_fSizeY = 80.f;
		m_fFontX = 770.f;
		m_fFontY = 520.f;
		m_pFontSize = TEXT("Font_size20");
		m_pQuest_ID = -1;
		static_cast<CUIObject*>(m_pOwner)->Function_Callback(TEXT("QuestListHover"), &m_pQuest_ID);
	}

	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}

void CQuest_List::Late_Update(_float fTimeDelta)
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

HRESULT CQuest_List::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::QUEST_BORDER)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	m_pGameInstance->Render_Text(m_pFontSize, m_pQuest_Name.c_str(), _float2(m_fFontX + m_fX, m_fFontY - m_fY));

	return S_OK;
}

_vector CQuest_List::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CQuest_List::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iColor", &m_iColor, sizeof(_int))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CQuest_List::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_MenuTextButtonBorder"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CQuest_List::Hover()
{
	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_float2 fMouse{};
	fMouse.x = ptMouse.x - (g_iWinSizeX * 0.5f);
	fMouse.y = -(ptMouse.y - (g_iWinSizeY * 0.5f));
	POINT pt{};
	pt.x = _long(fMouse.x);
	pt.y = _long(fMouse.y);

	if (PtInRect(&m_pRect, pt))
	{
		m_bHover = true;
	}
	else
	{
		m_bHover = false;
	}
}

void CQuest_List::Set_Name(_wstring Name, _int ID)
{
	m_pQuest_Name = Name;
	m_pQuest_ID = ID;
}

CQuest_List* CQuest_List::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_List* pInstance = new CQuest_List(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CQuest_List");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CQuest_List::Clone(void* pArg, CGameObject* pOwner)
{
	CQuest_List* pInstance = new CQuest_List(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Header");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CQuest_List::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CQuest_List::Describe_Entity()
{
}
#endif // _DEBUG
