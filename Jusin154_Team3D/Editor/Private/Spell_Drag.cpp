#include "pch.h"
#include "Spell_Drag.h"
#include "GameInstance.h"

CSpell_Drag::CSpell_Drag(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSpell_Drag::CSpell_Drag(const CSpell_Drag& rhs)
	:CElementObject(rhs)
{
}

HRESULT CSpell_Drag::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpell_Drag::Initialize(void* pArg)
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

	m_fTimeMult = 3.f;
	m_fAlpha = 1.f;
	m_fAlphaTime = 1.f;
	m_fDelayTime = 0.2f;
	m_fSortZ = 0.f;
	m_iSkillType = ENUM_CLASS(SKILL_TYPE::ARRESTO_MOMENTUM);
	Compute_UI(m_iSkillType);
	m_bMove = false;
	m_bClick = false;
	static_cast<CUIObject*>(m_pOwner)->Add_Function(TEXT("Slot_Hover"), [this](void* p) {this->Set_SpellType(*reinterpret_cast<_int*>(p)); });
	return S_OK;
}

void CSpell_Drag::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpell_Drag::Update(_float fTimeDelta)
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

	Get_MousePos();

	if (m_iSpellType != -1 && m_iSpellType <= 26)
	{
		m_bHover = true;
		Compute_UI(static_cast<CUIObject*>(m_pOwner)->Get_Info(m_iSpellType).iSkill_Type);

	}
	else
	{
		m_bHover = false;
	}

	if (m_bHover == true)
	{
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON))
		{
			m_vStart_MousePos = XMVectorSet(0.f, 0.f, 0.f, 0.f);

			POINT m_pPt{};
			GetCursorPos(&m_pPt);
			ScreenToClient(g_hWnd, &m_pPt);
			_float2 fMouse{};
			fMouse.x = m_pPt.x - (g_iWinSizeX * 0.5f);
			fMouse.y = m_pPt.y - (g_iWinSizeY * 0.5f);

			m_vStart_MousePos = XMVectorSet(fMouse.x, fMouse.y, 0.f, 1.f);
		}

		if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
		{
			POINT pt{};
			GetCursorPos(&pt);
			ScreenToClient(g_hWnd, &pt);

			float moveX = pt.x - (g_iWinSizeX * 0.5f);
			float moveY = pt.y - (g_iWinSizeY * 0.5f);

			m_vMove_MousePos = XMVectorSet(moveX, moveY, 0.f, 1.f);

			_vector Dir = m_vMove_MousePos - m_vStart_MousePos;
			_float fLength2 = XMVectorGetX(XMVector2Dot(Dir, Dir));

			if (fLength2 >= 50.f)
			{
				m_bMove = true;
			}
			m_bClick = true;

		}
	}
	else
	{
		m_iSpellType = -1;
	}

	if (m_bMove == true)
	{
		MoveX(Get_MousePos().x);
		MoveY(Get_MousePos().y);
	}


	if (m_pGameInstance->Mouse_Up(DIM_LBUTTON) && m_bClick == true)
	{
		m_bHover = false;
		m_bClick = false;
		m_bMove = false;
		m_iSpellType = -1;
	}

	static_cast<CUIObject*>(m_pOwner)->Function_Callback(TEXT("Click"), &m_bClick);

	__super::Update(fTimeDelta);

}

void CSpell_Drag::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		if (m_bMove == true)
		{
			m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
		}
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CSpell_Drag::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::SPELL_DRAG))))
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

_vector CSpell_Drag::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CSpell_Drag::Bind_ShaderResources()
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
	if (FAILED(m_pDiffuse_TextureCom3->Bind_ShaderResource(m_pShaderCom, "g_Texture3", 0)))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageUV", &m_vUV, sizeof(_float4))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_vScale, sizeof(_float2))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CSpell_Drag::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_SpellType_Generic"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_SelectedPresetBorder"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_GoldLeaf"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom2), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_Atlas_Widget_Spell"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom3), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CSpell_Drag::Compute_UI(_uint SpellID)
{
	_float2 fImage_Size = { 1024.f, 1792.f };

	_uint iCountX = 4;
	_uint iCountY = 7;

	_float iImageX = 256.f;
	_float iImageY = 256.f;

	_uint iframeX = SpellID % iCountX;
	_uint iframeY = SpellID / iCountX;

	_float2 UVStart;
	UVStart.x = iframeX * iImageX / fImage_Size.x;
	UVStart.y = iframeY * iImageY / fImage_Size.y;

	_float2 UVEnd;
	UVEnd.x = UVStart.x + (iImageX / fImage_Size.x);
	UVEnd.y = UVStart.y + (iImageY / fImage_Size.y);

	_float4 UV = _float4{ UVStart.x, UVStart.y, UVEnd.x, UVEnd.y };
	m_vUV = UV;
}

void CSpell_Drag::Set_SpellType(_int SpellID)
{
	m_iSpellType = SpellID;
}

_float2 CSpell_Drag::Get_MousePos()
{
	POINT m_pPt{};
	GetCursorPos(&m_pPt);
	ScreenToClient(g_hWnd, &m_pPt);
	_float2 fMouse{};
	fMouse.x = m_pPt.x - (g_iWinSizeX * 0.5f);
	fMouse.y = m_pPt.y - (g_iWinSizeY * 0.5f);

	m_vMove_MousePos = XMVectorSet(fMouse.x, fMouse.y, 0.f, 1.f);
	return fMouse;
}

CSpell_Drag* CSpell_Drag::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpell_Drag* pInstance = new CSpell_Drag(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpell_Drag");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpell_Drag::Clone(void* pArg, CGameObject* pOwner)
{
	CSpell_Drag* pInstance = new CSpell_Drag(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Drag");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpell_Drag::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pDiffuse_TextureCom2);
	SAFE_RELEASE(m_pDiffuse_TextureCom3);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CSpell_Drag::Describe_Entity()
{
}
