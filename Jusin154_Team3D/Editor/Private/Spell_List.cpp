#include "pch.h"
#include "Spell_List.h"
#include "GameInstance.h"

CSpell_List::CSpell_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSpell_List::CSpell_List(const CSpell_List& rhs)
	:CElementObject(rhs)
{
}

HRESULT CSpell_List::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpell_List::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 0.f;
	Desc.fSizeX = 105.f;
	Desc.fSizeY = 105.f;

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
	m_fAlphaTime = 1.f;
	m_fOffSetX = 120.f;
	m_fOffSetY = 122.f;
	m_iCols = 4;
	m_pVIBufferCom->Set_Cloned(true);
	m_pVIBufferCom->Set_Pos(-185.f, 375.f, m_fOffSetX, m_fOffSetY, m_iCols);
	m_pVIBufferCom->Set_Size(m_fSizeX, m_fSizeY);
	m_fDelayTime = 0.5f;
	m_iPerSpell_Slot = -1;
	m_bClick = false;
	Color();
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("Click"), [this](void* p) {this->Click_Slot(*reinterpret_cast<_bool*>(p)); });
	return S_OK;
}

void CSpell_List::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpell_List::Update(_float fTimeDelta)
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
	if (m_bClick == false)
	{
		Hover();
	}

	__super::Update(fTimeDelta);
}

void CSpell_List::Late_Update(_float fTimeDelta)
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


HRESULT CSpell_List::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIINTANCE::COLOR))))
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

_vector CSpell_List::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CSpell_List::SizeUpX(_float fSizeX)
{
	m_fSizeX = fSizeX;
	m_pVIBufferCom->Set_SizeX(m_fSizeX);
}

void CSpell_List::SizeUpY(_float fSizeY)
{
	m_fSizeY = fSizeY;
	m_pVIBufferCom->Set_SizeY(m_fSizeY);
}

void CSpell_List::SizeUpdate(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	m_pVIBufferCom->Set_Size(m_fSizeX, m_fSizeY);
}

void CSpell_List::Color()
{
	m_pVIBufferCom->Set_Index_Renge_Color(0, 3, 0.f);
	m_pVIBufferCom->Set_Index_Renge_Color(4, 7, 1.f);
	m_pVIBufferCom->Set_Index_Renge_Color(8, 12, 2.f);
	m_pVIBufferCom->Set_Index_Renge_Color(13, 16, 3.f);
	m_pVIBufferCom->Set_Index_Renge_Color(17, 19, 4.f);
	m_pVIBufferCom->Set_Index_Renge_Color(20, 22, 5.f);
	m_pVIBufferCom->Set_Index_Renge_Color(23, 25, 3.f);
}

void CSpell_List::Hover()
{
	POINT ptMouse{};
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	_float2 fMouse;
	fMouse.x = ptMouse.x - (g_iWinSizeX * 0.5f);
	fMouse.y = -(ptMouse.y - (g_iWinSizeY * 0.5f));

	// 엘리먼트의 월드 위치를 더해주면 엘리먼트 좌표계 기준이 됨
	fMouse.x -= m_pOwner->Get_WorldPostion().m128_f32[0];
	fMouse.y -= m_pOwner->Get_WorldPostion().m128_f32[1];

	m_iSpellType = m_pVIBufferCom->Set_Mouse_Hover(fMouse);

	CUIObject::HOVER_INFO Info;
	Info.iSlotID = 0;
	if (m_iSpellType == -1)
	{
		Info.iHover_Index = -1;
		static_cast<CUIObject*>(m_pOwner)->Function_Callback(TEXT("Hover"), &Info);
	}

	else
	{
		Info.iHover_Index = m_iSpellType;
		static_cast<CUIObject*>(m_pOwner)->Function_Callback(TEXT("Hover"), &Info);
	}

}

void CSpell_List::Click_Slot(_bool bClick)
{
	if (bClick == true)
	{
		m_bClick = true;
	}
	else
	{
		m_bClick = false;
	}
}

HRESULT CSpell_List::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAngle", &m_fAngle, sizeof(_float))))
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

HRESULT CSpell_List::Ready_Components(void* pArg)
{
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_UI_Spell_List"), (CComponent**)&m_pVIBufferCom, nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_SpellType_Generic"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIINSTANCE, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSpell_List* CSpell_List::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpell_List* pInstance = new CSpell_List(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpell_List");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpell_List::Clone(void* pArg, CGameObject* pOwner)
{
	CSpell_List* pInstance = new CSpell_List(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_List");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpell_List::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CSpell_List::Describe_Entity()
{
}
